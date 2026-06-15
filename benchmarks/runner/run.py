import subprocess
import time
import csv
import re
import os
import signal
import socket
from pathlib import Path
from datetime import date

# ---------------- CONFIG ----------------

ROOT = Path(__file__).resolve().parents[2]

BASE_URL = "http://127.0.0.1:3143"
DURATION = 30

SERVER_BIN = ROOT / "build" / "benchmarks" / "RoutingBenchmarks"

CSV_PATH = ROOT / "benchmarks" / "results" / "routing_wrk_results.csv"

ROUTES = [
    ("baseline", "/"),
    ("static_lookup", "/runtime/static"),
    ("single_param", "/runtime/users/123"),
    ("multiple_params", "/runtime/users/asaf/posts/42"),
    ("precedence_static", "/runtime/precedence/me"),
    ("precedence_param", "/runtime/precedence/asaf"),
]

TEST_SETS = [
    {
        "name": "baseline",
        "runs": [(16, 100), (16, 100)]
    },
    {
        "name": "concurrency_curve",
        "runs": [(16, 50), (16, 100), (16, 250), (16, 500), (16, 1000)]
    },
    {
        "name": "thread_scaling",
        "runs": [(2, 500), (4, 500), (8, 500), (16, 500)]
    },
    {
        "name": "stress",
        "runs": [(16, 2000)]
    }
]

# ---------------- SERVER CONTROL ----------------

def start_server():
    print("[INFO] Starting server...")

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
        proc.wait(timeout=3)
    except Exception:
        proc.kill()


# ---------------- WRK ----------------

def run_wrk(threads, connections, path):
    url = BASE_URL + path

    print(f"[INFO] Running wrk: t={threads}, c={connections}, path={path}")

    cmd = [
        "wrk",
        "-t", str(threads),
        "-c", str(connections),
        "-d", str(DURATION),
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

    for line in output.splitlines():
        line = line.strip()

        if "Requests/sec:" in line:
            rps = float(line.split()[-1])

        elif line.startswith("Latency"):
            parts = line.split()
            avg = parts[1]
            max_lat = parts[3]

    if rps is None or avg is None or max_lat is None:
        raise RuntimeError(f"Failed to parse wrk output:\n{output}")

    return (
        rps,
        to_us(avg),
        to_us(max_lat)
    )


# ---------------- CSV ----------------

def get_git_commit():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
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
            date.today(),
            get_git_commit()
        ])

        if not file_exists:
            writer.writerow([
                "date",
                "test_set",
                "route_name",
                "path",
                "threads",
                "connections",
                "rps",
                "avg_latency_us",
                "max_latency_us"
            ])


def write_row(row):
    CSV_PATH.parent.mkdir(parents=True, exist_ok=True)

    with open(CSV_PATH, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(row)


# ---------------- MAIN ----------------

def main():
    write_session_header()

    for route_name, path in ROUTES:
        for test in TEST_SETS:
            print(
                f"\n=== Running route: {route_name} {path}, "
                f"test set: {test['name']} ==="
            )

            for threads, conns in test["runs"]:
                server = start_server()

                try:
                    wait_for_server()

                    output = run_wrk(threads, conns, path)

                    rps, avg_us, max_us = parse_wrk(output)

                    write_row([
                        date.today(),
                        test["name"],
                        route_name,
                        path,
                        threads,
                        conns,
                        rps,
                        avg_us,
                        max_us
                    ])

                    print(
                        f"[RESULT] {route_name} {test['name']} "
                        f"t={threads}, c={conns}, rps={rps}"
                    )

                finally:
                    stop_server(server)
                    time.sleep(2)


if __name__ == "__main__":
    main()
