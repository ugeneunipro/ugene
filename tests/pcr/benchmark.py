#!/usr/bin/env python3
"""Compare PCR runners built from the base, uncapped base, and fixed source."""
import argparse
import json
import platform
import statistics
import subprocess
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
for variant in ("before", "unlimited", "after"):
    parser.add_argument("--" + variant, type=Path, required=True)
parser.add_argument("--output", type=Path, required=True)
parser.add_argument("--rounds", type=int, default=3)
args = parser.parse_args()
rows = []
for case in ("ordinary", "rejected", "dense", "sparse"):
    samples = {variant: [] for variant in ("before", "unlimited", "after")}
    for _ in range(args.rounds):
        for variant in samples:
            executable = getattr(args, variant).resolve()
            result = subprocess.run([str(executable), "--benchmark", case],
                                    capture_output=True, text=True, check=True)
            samples[variant].append(json.loads(result.stdout))
    for variant, values in samples.items():
        assert len({sample["products"] for sample in values}) == 1
        row = {"case": case, "variant": variant, "products": values[0]["products"],
               "sequence_bp": values[0]["sequence_bp"]}
        for key in ("total_ms", "search_ms", "pairing_ms", "peak_rss_bytes"):
            if key in values[0]:
                row[key] = statistics.median(sample[key] for sample in values)
        row["samples"] = values
        rows.append(row)
        print(json.dumps({key: value for key, value in row.items() if key != "samples"}), flush=True)
args.output.write_text(json.dumps({"platform": platform.platform(), "rounds": args.rounds,
                                  "calculator": "UGENE Rough", "rows": rows}, indent=2) + "\n")
