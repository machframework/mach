import csv
import subprocess
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCRIPTS = ROOT / "scripts" / "binding"

SERVER = "http://127.0.0.1:3143"

THREADS = 16
CONNECTIONS = 100
DURATION = "30s"

BENCHMARKS = [
    ("baseline", "/benchmark/baseline", "baseline.lua"),
    ("small", "/benchmark/binding/small", "small.lua"),
    ("large", "/benchmark/binding/large", "large.lua"),
    ("malformed", "/benchmark/binding/failure", "malformed.lua"),
]

RPS_RE = re.compile(r"Requests/sec:\s+([0-9.]+)")
LAT_RE = re.compile(r"Latency\s+([0-9.]+\w+)")
TRANSFER_RE = re.compile(r"Transfer/sec:\s+([0-9.]+\w+)")


def run_wrk(route: str, script: Path):
    cmd = [
        "wrk",
        "-t", str(THREADS),
        "-c", str(CONNECTIONS),
        "-d", DURATION,
        "-s", str(script),
        SERVER + route,
    ]

    print("Running:", " ".join(cmd))

    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        cwd=SCRIPTS,
        check=True,
    )

    output = result.stdout

    rps = RPS_RE.search(output)
    latency = LAT_RE.search(output)
    transfer = TRANSFER_RE.search(output)

    return {
        "raw_output": output,
        "requests_per_sec": rps.group(1) if rps else "",
        "latency": latency.group(1) if latency else "",
        "transfer_per_sec": transfer.group(1) if transfer else "",
    }


def main():
    output_file = ROOT / "results" / "binding.csv"
    output_file.parent.mkdir(parents=True, exist_ok=True)

    with output_file.open("w", newline="") as csvfile:
        writer = csv.writer(csvfile)

        writer.writerow([
            "Benchmark",
            "Threads",
            "Connections",
            "Duration",
            "Latency",
            "Requests/sec",
            "Transfer/sec",
        ])

        for name, route, script_name in BENCHMARKS:
            data = run_wrk(route, SCRIPTS / script_name)

            writer.writerow([
                name,
                THREADS,
                CONNECTIONS,
                DURATION,
                data["latency"],
                data["requests_per_sec"],
                data["transfer_per_sec"],
            ])

    print(f"\nResults written to: {output_file}")


if __name__ == "__main__":
    main()