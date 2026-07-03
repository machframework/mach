import subprocess
import time
import csv
import re
import os
import signal
import socket
from pathlib import Path
from datetime import date, datetime

# ---------------- CONFIG ----------------

ROOT = Path(__file__).resolve().parents[2]

BASE_URL = "http://127.0.0.1:3143"

SERVER_BIN = ROOT / "build" / "benchmarks" / "PipelineBenchmarks"

CSV_PATH = ROOT / "benchmarks" / "results" / "pipeline_stability_wrk_results.csv"

ROUTES = [
    ("controller_static", "/runtime/controller/static"),
    ("controller_di_singleton", "/runtime/controller/request-id"),
    ("controller_multiple_params", "/runtime/users/asaf/posts/42"),
]

TEST_SETS = [
    {
        "name": "endurance_10m_500",
        "reuse_server": False,
        "runs": [
            (16, 500, "10m", "steady"),
        ],
    },
    {
        "name": "endurance_10m_1000",
        "reuse_server": False,
        "runs": [
            (16, 1000, "10m", "steady"),
        ],
    },
    {
        "name": "recovery_spike",
        "reuse_server": True,
        "runs": [
            (16, 2000, "2m", "spike_1"),
            (16, 100, "2m", "recovery_1"),
            (16, 2000, "2m", "spike_2"),
            (16, 100, "2m", "recovery_2"),
        ],
    },
]

# ---------------- SERVER CONTROL ----------------

def start_server():
    print("[INFO] Starting stability benchmark server...")

    return subprocess.Popen(
        [str(SERVER_BIN)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )


def wait_for_server():
    print("[INFO] Waiting for server...")

    for _ in range(100):
        try:
            with socket.create_connection(("127.0.0.1", 3143), timeout=0.1):
                return
        except OSError:
            time.sleep(0.1)

    raise RuntimeError("Server did not become ready")


def stop_server(proc):
    print("[INFO] Stopping server...")

    try:
        os.kill(proc.pid, signal.SIGTERM)
        proc.wait(timeout=5)
    except Exception:
        proc.kill()


# ---------------- WRK ----------------

def run_wrk(threads, connections, duration, path):
    url = BASE_URL + path

    print(
        f"[INFO] Running wrk: "
        f"t={threads}, c={connections}, d={duration}, path={path}"
    )

    cmd = [
        "wrk",
        "-t", str(threads),
        "-c", str(connections),
        "-d", duration,
        url
    ]

    result = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True
    )

    return result.stdout


# ---------------- PARSING ----------------

def to_us(value_with_unit):
    match = re.match(r"([0-9.]+)([a-z]+)", value_with_unit)

    if not match:
        raise ValueError(f"Bad latency format: {value_with_unit}")

    val, unit = match.groups()
    val = float(val)

    if unit == "ms":
        return val * 1000

    if unit == "us":
        return val

    if unit == "s":
        return val * 1_000_000

    return val


def parse_wrk(output):
    print(output)

    rps = None
    avg = None
    max_lat = None
    requests = None
    transfer = None

    for line in output.splitlines():
        line = line.strip()

        if "Requests/sec:" in line:
            rps = float(line.split()[-1])

        elif "Transfer/sec:" in line:
            transfer = line.split()[-1]

        elif line.startswith("Latency"):
            parts = line.split()
            avg = parts[1]
            max_lat = parts[3]

        elif "requests in" in line:
            match = re.search(r"(\d+)\s+requests in", line)
            if match:
                requests = int(match.group(1))

    if rps is None or avg is None or max_lat is None:
        raise RuntimeError(f"Failed to parse wrk output:\n{output}")

    return {
        "rps": rps,
        "avg_latency_us": to_us(avg),
        "max_latency_us": to_us(max_lat),
        "requests": requests,
        "transfer_per_sec": transfer,
    }


# ---------------- CSV ----------------

def get_git_commit():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            cwd=ROOT,
            text=True
        ).strip()
    except Exception:
        return "unknown"


def write_session_header():
    CSV_PATH.parent.mkdir(parents=True, exist_ok=True)

    file_exists = CSV_PATH.exists()

    with open(CSV_PATH, "a", newline="") as f:
        writer = csv.writer(f)

        writer.writerow([])
        writer.writerow([
            "SESSION START",
            datetime.now().isoformat(timespec="seconds"),
            get_git_commit()
        ])

        if not file_exists:
            writer.writerow([
                "date",
                "timestamp",
                "test_set",
                "phase",
                "route_name",
                "path",
                "threads",
                "connections",
                "duration",
                "rps",
                "avg_latency_us",
                "max_latency_us",
                "requests",
                "transfer_per_sec"
            ])


def write_row(row):
    CSV_PATH.parent.mkdir(parents=True, exist_ok=True)

    with open(CSV_PATH, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(row)


# ---------------- RUNNERS ----------------

def run_single_server_test(route_name, path, test):
    server = start_server()

    try:
        wait_for_server()

        for threads, conns, duration, phase in test["runs"]:
            output = run_wrk(threads, conns, duration, path)
            parsed = parse_wrk(output)

            write_result(
                test["name"],
                phase,
                route_name,
                path,
                threads,
                conns,
                duration,
                parsed
            )

            time.sleep(5)

    finally:
        stop_server(server)
        time.sleep(2)


def run_fresh_server_test(route_name, path, test):
    for threads, conns, duration, phase in test["runs"]:
        server = start_server()

        try:
            wait_for_server()

            output = run_wrk(threads, conns, duration, path)
            parsed = parse_wrk(output)

            write_result(
                test["name"],
                phase,
                route_name,
                path,
                threads,
                conns,
                duration,
                parsed
            )

        finally:
            stop_server(server)
            time.sleep(2)


def write_result(
    test_name,
    phase,
    route_name,
    path,
    threads,
    conns,
    duration,
    parsed
):
    write_row([
        date.today(),
        datetime.now().isoformat(timespec="seconds"),
        test_name,
        phase,
        route_name,
        path,
        threads,
        conns,
        duration,
        parsed["rps"],
        parsed["avg_latency_us"],
        parsed["max_latency_us"],
        parsed["requests"],
        parsed["transfer_per_sec"],
    ])

    print(
        f"[RESULT] {route_name} {test_name}/{phase} "
        f"t={threads}, c={conns}, d={duration}, "
        f"rps={parsed['rps']}, avg_us={parsed['avg_latency_us']}, "
        f"max_us={parsed['max_latency_us']}"
    )


# ---------------- MAIN ----------------

def main():
    write_session_header()

    for route_name, path in ROUTES:
        for test in TEST_SETS:
            print(
                f"\n=== Running route: {route_name} {path}, "
                f"test set: {test['name']} ==="
            )

            if test["reuse_server"]:
                run_single_server_test(route_name, path, test)
            else:
                run_fresh_server_test(route_name, path, test)


if __name__ == "__main__":
    main()