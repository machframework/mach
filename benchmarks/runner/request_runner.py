import csv
import os
import re
import signal
import socket
import subprocess
import time
from datetime import datetime
from pathlib import Path

# ---------------- CONFIG ----------------

ROOT = Path(__file__).resolve().parents[2]

BASE_URL = "http://127.0.0.1:3143"

SERVER_BIN = ROOT / "build" / "release" / "benchmarks" / "RequestBenchmarks.exe"

CSV_PATH = ROOT / "benchmarks" / "results" / "request_wrk_results.csv"

SCRIPTS = ROOT / "benchmarks" / "scripts" / "request"

DURATION = "30s"

TEST_CONFIGS = [
    (1, 1),
    (2, 10),
    (4, 25),
    (8, 50),
    (16, 100),
    (16, 250),
    (16, 500),
]

BENCHMARKS = [
    {
        "name": "plain",
        "route": "/benchmark/request/plain",
    },
    {
        "name": "headers",
        "route": "/benchmark/request/headers",
        "headers": [
            "X-Test-1: value-1",
            "X-Test-2: value-2",
            "User-Agent: MachBenchmark/1.0",
            "Accept: */*",
            "Accept-Encoding: identity",
        ],
    },
    {
        "name": "cookies",
        "route": "/benchmark/request/cookies",
        "headers": [
            "Cookie: session=abcdef123456; theme=dark; locale=en-US; tracking=xyz789",
        ],
    },
    {
        "name": "body",
        "route": "/benchmark/request/body",
        "script": "wrk-body.lua",
    },
    {
        "name": "binding_small",
        "route": "/benchmark/request/binding/small",
        "script": "wrk-small.lua",
    },
    {
        "name": "binding_large",
        "route": "/benchmark/request/binding/large",
        "script": "wrk-large.lua",
    },
]

RPS_RE = re.compile(r"Requests/sec:\s+([0-9.]+)")

LATENCY_RE = re.compile(
    r"Latency\s+"
    r"([0-9.]+(?:ns|us|ms|s))\s+"
    r"([0-9.]+(?:ns|us|ms|s))\s+"
    r"([0-9.]+(?:ns|us|ms|s))"
)

REQUESTS_RE = re.compile(r"(\d+)\s+requests in")

TRANSFER_RE = re.compile(r"Transfer/sec:\s+([0-9.]+\w+)")


# ---------------- SERVER CONTROL ----------------

def start_server():
    print("[INFO] Starting request benchmark server...")

    return subprocess.Popen(
        [str(SERVER_BIN)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def wait_for_server():
    print("[INFO] Waiting for server...")

    for _ in range(100):
        try:
            with socket.create_connection(
                ("127.0.0.1", 3143),
                timeout=0.1,
            ):
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


# ---------------- METADATA ----------------

def get_git_commit():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            cwd=ROOT,
            text=True,
        ).strip()
    except Exception:
        return "unknown"


# ---------------- WRK ----------------

def run_wrk(benchmark, threads, connections):
    cmd = [
        "wrk",
        "-t", str(threads),
        "-c", str(connections),
        "-d", DURATION,
    ]

    for header in benchmark.get("headers", []):
        cmd.extend([
            "-H",
            header,
        ])

    script_name = benchmark.get("script")

    if script_name:
        cmd.extend([
            "-s",
            str(SCRIPTS / script_name),
        ])

    cmd.append(
        BASE_URL + benchmark["route"]
    )

    print(
        f"\n[INFO] Running wrk: "
        f"benchmark={benchmark['name']}, "
        f"t={threads}, c={connections}, d={DURATION}"
    )

    result = subprocess.run(
        cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )

    return result.stdout


# ---------------- PARSING ----------------

def parse_wrk(output):
    print(output)

    rps_match = RPS_RE.search(output)
    latency_match = LATENCY_RE.search(output)
    requests_match = REQUESTS_RE.search(output)
    transfer_match = TRANSFER_RE.search(output)

    if not rps_match or not latency_match:
        raise RuntimeError(
            f"Failed to parse wrk output:\n{output}"
        )

    return {
        "rps": float(rps_match.group(1)),
        "avg_latency": latency_match.group(1),
        "latency_stdev": latency_match.group(2),
        "max_latency": latency_match.group(3),
        "requests": (
            int(requests_match.group(1))
            if requests_match
            else None
        ),
        "transfer_per_sec": (
            transfer_match.group(1)
            if transfer_match
            else ""
        ),
    }


# ---------------- CSV ----------------

def write_session_header():
    CSV_PATH.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    file_exists = CSV_PATH.exists()

    with CSV_PATH.open("a", newline="") as f:
        writer = csv.writer(f)

        writer.writerow([])

        writer.writerow([
            "SESSION START",
            datetime.now().isoformat(
                timespec="seconds"
            ),
            get_git_commit(),
        ])

        if not file_exists:
            writer.writerow([
                "timestamp",
                "benchmark",
                "route",
                "threads",
                "connections",
                "duration",
                "rps",
                "avg_latency",
                "latency_stdev",
                "max_latency",
                "requests",
                "transfer_per_sec",
            ])


def write_row(row):
    CSV_PATH.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    with CSV_PATH.open("a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(row)


# ---------------- MAIN ----------------

def main():
    write_session_header()

    server = start_server()

    try:
        wait_for_server()

        for benchmark in BENCHMARKS:
            for threads, connections in TEST_CONFIGS:
                output = run_wrk(
                    benchmark,
                    threads,
                    connections,
                )

                parsed = parse_wrk(output)

                write_row([
                    datetime.now().isoformat(
                        timespec="seconds"
                    ),
                    benchmark["name"],
                    benchmark["route"],
                    threads,
                    connections,
                    DURATION,
                    parsed["rps"],
                    parsed["avg_latency"],
                    parsed["latency_stdev"],
                    parsed["max_latency"],
                    parsed["requests"],
                    parsed["transfer_per_sec"],
                ])

                print(
                    f"[RESULT] "
                    f"{benchmark['name']} "
                    f"t={threads}, "
                    f"c={connections}, "
                    f"rps={parsed['rps']}, "
                    f"avg={parsed['avg_latency']}, "
                    f"max={parsed['max_latency']}"
                )

                time.sleep(2)

    finally:
        stop_server(server)


if __name__ == "__main__":
    main()
