import csv
import os
import re
import signal
import shutil
import socket
import statistics
import subprocess
import time
from datetime import datetime
from pathlib import Path

# ---------------- CONFIG ----------------

ROOT = Path(__file__).resolve().parents[2]
BENCHMARKS_DIR = Path(__file__).resolve().parent.parent
SCRIPTS_DIR = BENCHMARKS_DIR / "scripts"

BASE_URL = "http://127.0.0.1:3143"
SERVER_HOST = "127.0.0.1"
SERVER_PORT = 3143

SERVER_BIN = ROOT / "build" / "benchmarks" / "FrameworkBenchmarks"

RESULTS_DIR = ROOT / "benchmarks" / "results"
CSV_PATH = RESULTS_DIR / "framework_wrk_results.csv"
SUMMARY_PATH = RESULTS_DIR / "framework_wrk_summary.csv"

THREADS = 16
CONNECTIONS = 100

WARMUP_DURATION = "5s"
MEASURED_DURATION = "30s"
MEASURED_RUNS = 5

BETWEEN_RUNS_SECONDS = 3
BETWEEN_BENCHMARKS_SECONDS = 2

POST_SCRIPT = SCRIPTS_DIR / "post.lua"

BENCHMARKS = [
    {
        "name": "plaintext",
        "method": "GET",
        "path": "/plaintext",
    },
    {
        "name": "json",
        "method": "GET",
        "path": "/json",
    },
    {
        "name": "routing",
        "method": "GET",
        "path": "/users/42",
    },
    {
        "name": "model_binding",
        "method": "POST",
        "path": "/users",
    },
    {
        "name": "minimal_api",
        "method": "POST",
        "path": "/minimal/users/42",
    },
    {
        "name": "controller",
        "method": "POST",
        "path": "/controller/users/42/direct",
    },
    {
        "name": "controller_di",
        "method": "POST",
        "path": "/controller/users/42/service",
    },
]

# Separate soak tests. These are intentionally not part of the performance
# summary and are disabled by default. Set RUN_STABILITY_TESTS = True when
# doing the final stability pass.
RUN_STABILITY_TESTS = True
STABILITY_DURATION = "10m"
STABILITY_BENCHMARKS = [
    "plaintext",
    "minimal_api",
    "controller_di",
]

# ---------------- SERVER CONTROL ----------------


def start_server():
    print("[INFO] Starting framework benchmark server...")

    return subprocess.Popen(
        [str(SERVER_BIN)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def wait_for_server(proc):
    print("[INFO] Waiting for server...")

    for _ in range(100):
        if proc.poll() is not None:
            raise RuntimeError(
                f"Benchmark server exited early with code {proc.returncode}"
            )

        try:
            with socket.create_connection(
                (SERVER_HOST, SERVER_PORT),
                timeout=0.1,
            ):
                print("[INFO] Server is ready.")
                return
        except OSError:
            time.sleep(0.1)

    raise RuntimeError("Server did not become ready")


def stop_server(proc):
    if proc.poll() is not None:
        return

    print("[INFO] Stopping server...")

    try:
        if os.name == "nt":
            proc.terminate()
        else:
            os.kill(proc.pid, signal.SIGTERM)

        proc.wait(timeout=5)
    except Exception:
        proc.kill()
        proc.wait()


# ---------------- WRK ----------------


def wrk_command(benchmark, duration, latency=True):
    cmd = [
        "wrk",
        "-t", str(THREADS),
        "-c", str(CONNECTIONS),
        "-d", duration,
    ]

    if latency:
        cmd.append("--latency")

    if benchmark["method"] == "POST":
        cmd.extend(["-s", str(POST_SCRIPT)])

    cmd.append(BASE_URL + benchmark["path"])
    return cmd


def run_wrk(benchmark, duration, *, latency=True, print_output=True):
    cmd = wrk_command(benchmark, duration, latency=latency)

    print(
        f"[INFO] wrk: {benchmark['method']} {benchmark['path']} "
        f"(t={THREADS}, c={CONNECTIONS}, d={duration})"
    )

    result = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )

    if print_output:
        print(result.stdout)

    if result.returncode != 0:
        raise RuntimeError(
            f"wrk failed with exit code {result.returncode}:\n"
            f"{result.stdout}"
        )

    return result.stdout


# ---------------- PARSING ----------------


def metric_to_float(value):
    match = re.fullmatch(r"([0-9.]+)([kKmMgG]?)", value)

    if not match:
        raise ValueError(f"Bad metric format: {value}")

    number = float(match.group(1))
    suffix = match.group(2).lower()

    multipliers = {
        "": 1.0,
        "k": 1_000.0,
        "m": 1_000_000.0,
        "g": 1_000_000_000.0,
    }

    return number * multipliers[suffix]


def latency_to_us(value):
    match = re.fullmatch(r"([0-9.]+)(us|ms|s)", value)

    if not match:
        raise ValueError(f"Bad latency format: {value}")

    number = float(match.group(1))
    unit = match.group(2)

    if unit == "us":
        return number
    if unit == "ms":
        return number * 1_000
    return number * 1_000_000


def parse_wrk(output):
    rps = None
    avg_latency = None
    max_latency = None
    requests = None
    transfer = None
    socket_errors = 0
    non_2xx_3xx = 0
    percentiles = {}

    percentile_re = re.compile(
        r"^\s*(50|75|90|99)%\s+([0-9.]+(?:us|ms|s))\s*$"
    )

    for raw_line in output.splitlines():
        line = raw_line.strip()

        if line.startswith("Requests/sec:"):
            rps = metric_to_float(line.split()[-1])

        elif line.startswith("Transfer/sec:"):
            transfer = line.split()[-1]

        elif line.startswith("Latency"):
            parts = line.split()
            if len(parts) >= 4:
                avg_latency = latency_to_us(parts[1])
                max_latency = latency_to_us(parts[3])

        elif "requests in" in line:
            match = re.search(r"([0-9]+)\s+requests in", line)
            if match:
                requests = int(match.group(1))

        elif line.startswith("Socket errors:"):
            socket_errors = sum(
                int(value)
                for value in re.findall(
                    r"(?:connect|read|write|timeout)\s+([0-9]+)",
                    line,
                )
            )

        elif line.startswith("Non-2xx or 3xx responses:"):
            non_2xx_3xx = int(line.split(":")[-1].strip())

        else:
            match = percentile_re.match(raw_line)
            if match:
                percentiles[int(match.group(1))] = latency_to_us(
                    match.group(2)
                )

    if rps is None or avg_latency is None or max_latency is None:
        raise RuntimeError(f"Failed to parse wrk output:\n{output}")

    return {
        "rps": rps,
        "avg_latency_us": avg_latency,
        "max_latency_us": max_latency,
        "p50_latency_us": percentiles.get(50),
        "p75_latency_us": percentiles.get(75),
        "p90_latency_us": percentiles.get(90),
        "p99_latency_us": percentiles.get(99),
        "requests": requests,
        "transfer_per_sec": transfer,
        "socket_errors": socket_errors,
        "non_2xx_3xx": non_2xx_3xx,
    }


# ---------------- RESULTS ----------------


RESULT_FIELDS = [
    "session_id",
    "timestamp",
    "git_commit",
    "benchmark",
    "method",
    "path",
    "phase",
    "run",
    "threads",
    "connections",
    "duration",
    "rps",
    "avg_latency_us",
    "max_latency_us",
    "p50_latency_us",
    "p75_latency_us",
    "p90_latency_us",
    "p99_latency_us",
    "requests",
    "transfer_per_sec",
    "socket_errors",
    "non_2xx_3xx",
]


SUMMARY_FIELDS = [
    "session_id",
    "timestamp",
    "git_commit",
    "benchmark",
    "method",
    "path",
    "runs",
    "threads",
    "connections",
    "duration",
    "median_rps",
    "min_rps",
    "max_rps",
    "median_avg_latency_us",
    "median_p50_latency_us",
    "median_p90_latency_us",
    "median_p99_latency_us",
    "total_socket_errors",
    "total_non_2xx_3xx",
]


def get_git_commit():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            cwd=ROOT,
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except Exception:
        return "unknown"


def append_result(row):
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)
    exists = CSV_PATH.exists()

    with CSV_PATH.open("a", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=RESULT_FIELDS)

        if not exists:
            writer.writeheader()

        writer.writerow(row)


def median_present(rows, key):
    values = [row[key] for row in rows if row[key] is not None]
    return statistics.median(values) if values else None


def write_summary(all_results, session_id, commit):
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)
    exists = SUMMARY_PATH.exists()
    timestamp = datetime.now().isoformat(timespec="seconds")

    with SUMMARY_PATH.open("a", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=SUMMARY_FIELDS)

        if not exists:
            writer.writeheader()

        for benchmark in BENCHMARKS:
            rows = all_results[benchmark["name"]]
            rps_values = [row["rps"] for row in rows]

            writer.writerow({
                "session_id": session_id,
                "timestamp": timestamp,
                "git_commit": commit,
                "benchmark": benchmark["name"],
                "method": benchmark["method"],
                "path": benchmark["path"],
                "runs": len(rows),
                "threads": THREADS,
                "connections": CONNECTIONS,
                "duration": MEASURED_DURATION,
                "median_rps": statistics.median(rps_values),
                "min_rps": min(rps_values),
                "max_rps": max(rps_values),
                "median_avg_latency_us": median_present(
                    rows, "avg_latency_us"
                ),
                "median_p50_latency_us": median_present(
                    rows, "p50_latency_us"
                ),
                "median_p90_latency_us": median_present(
                    rows, "p90_latency_us"
                ),
                "median_p99_latency_us": median_present(
                    rows, "p99_latency_us"
                ),
                "total_socket_errors": sum(
                    row["socket_errors"] for row in rows
                ),
                "total_non_2xx_3xx": sum(
                    row["non_2xx_3xx"] for row in rows
                ),
            })


# ---------------- PERFORMANCE RUNNER ----------------


def run_benchmark(benchmark, commit, session_id):
    print()
    print("=" * 72)
    print(
        f"BENCHMARK: {benchmark['name']} "
        f"({benchmark['method']} {benchmark['path']})"
    )
    print("=" * 72)

    server = start_server()
    measured = []

    try:
        wait_for_server(server)

        print(
            f"[INFO] Warmup: {WARMUP_DURATION} "
            f"for {benchmark['name']}"
        )
        run_wrk(
            benchmark,
            WARMUP_DURATION,
            latency=False,
            print_output=False,
        )

        for run_number in range(1, MEASURED_RUNS + 1):
            print(
                f"\n[INFO] Measured run "
                f"{run_number}/{MEASURED_RUNS}"
            )

            output = run_wrk(
                benchmark,
                MEASURED_DURATION,
                latency=True,
            )
            parsed = parse_wrk(output)

            row = {
                "session_id": session_id,
                "timestamp": datetime.now().isoformat(timespec="seconds"),
                "git_commit": commit,
                "benchmark": benchmark["name"],
                "method": benchmark["method"],
                "path": benchmark["path"],
                "phase": "measured",
                "run": run_number,
                "threads": THREADS,
                "connections": CONNECTIONS,
                "duration": MEASURED_DURATION,
                **parsed,
            }

            append_result(row)
            measured.append(parsed)

            print(
                f"[RESULT] run={run_number} "
                f"rps={parsed['rps']:.2f}, "
                f"avg={parsed['avg_latency_us']:.2f}us, "
                f"p99={parsed['p99_latency_us']}us, "
                f"errors={parsed['socket_errors'] + parsed['non_2xx_3xx']}"
            )

            if run_number != MEASURED_RUNS:
                time.sleep(BETWEEN_RUNS_SECONDS)

    finally:
        stop_server(server)

    rps_values = [row["rps"] for row in measured]

    print(
        f"\n[SUMMARY] {benchmark['name']}: "
        f"median={statistics.median(rps_values):.2f} req/s, "
        f"range={min(rps_values):.2f}-{max(rps_values):.2f}"
    )

    return measured


# ---------------- STABILITY ----------------


def run_stability_test(benchmark, commit, session_id):
    print()
    print("=" * 72)
    print(
        f"STABILITY: {benchmark['name']} "
        f"({benchmark['method']} {benchmark['path']})"
    )
    print("=" * 72)

    server = start_server()

    try:
        wait_for_server(server)

        print(f"[INFO] Warmup: {WARMUP_DURATION}")
        run_wrk(
            benchmark,
            WARMUP_DURATION,
            latency=False,
            print_output=False,
        )

        output = run_wrk(
            benchmark,
            STABILITY_DURATION,
            latency=True,
        )
        parsed = parse_wrk(output)

        append_result({
            "session_id": session_id,
            "timestamp": datetime.now().isoformat(timespec="seconds"),
            "git_commit": commit,
            "benchmark": benchmark["name"],
            "method": benchmark["method"],
            "path": benchmark["path"],
            "phase": "stability",
            "run": 1,
            "threads": THREADS,
            "connections": CONNECTIONS,
            "duration": STABILITY_DURATION,
            **parsed,
        })

        print(
            f"[STABILITY RESULT] "
            f"rps={parsed['rps']:.2f}, "
            f"avg={parsed['avg_latency_us']:.2f}us, "
            f"p99={parsed['p99_latency_us']}us, "
            f"socket_errors={parsed['socket_errors']}, "
            f"non_2xx_3xx={parsed['non_2xx_3xx']}"
        )

    finally:
        stop_server(server)


# ---------------- MAIN ----------------


def main():
    if shutil.which("wrk") is None:
        raise RuntimeError(
            "wrk was not found in PATH. Install wrk before running benchmarks."
        )

    if not SERVER_BIN.is_file():
        raise FileNotFoundError(
            f"Benchmark server executable not found: {SERVER_BIN}"
        )

    if not os.access(SERVER_BIN, os.X_OK):
        raise PermissionError(
            f"Benchmark server is not executable: {SERVER_BIN}"
        )

    if not POST_SCRIPT.is_file():
        raise FileNotFoundError(
            f"wrk POST script not found: {POST_SCRIPT}"
        )

    commit = get_git_commit()
    session_id = datetime.now().strftime("%Y%m%d_%H%M%S")

    print("Mach framework benchmark")
    print(f"Session ID:       {session_id}")
    print(f"Git commit:       {commit}")
    print(f"Threads:          {THREADS}")
    print(f"Connections:      {CONNECTIONS}")
    print(f"Warmup:           {WARMUP_DURATION}")
    print(f"Measured runs:    {MEASURED_RUNS}")
    print(f"Measured duration:{MEASURED_DURATION}")
    print(f"Server binary:    {SERVER_BIN}")

    all_results = {}

    for index, benchmark in enumerate(BENCHMARKS):
        all_results[benchmark["name"]] = run_benchmark(
            benchmark,
            commit,
            session_id,
        )

        if index != len(BENCHMARKS) - 1:
            time.sleep(BETWEEN_BENCHMARKS_SECONDS)

    write_summary(all_results, session_id, commit)

    print(f"\n[INFO] Raw results: {CSV_PATH}")
    print(f"[INFO] Summary:     {SUMMARY_PATH}")

    if RUN_STABILITY_TESTS:
        selected = {
            benchmark["name"]: benchmark
            for benchmark in BENCHMARKS
        }

        for name in STABILITY_BENCHMARKS:
            time.sleep(BETWEEN_BENCHMARKS_SECONDS)
            run_stability_test(selected[name], commit, session_id)


if __name__ == "__main__":
    main()
