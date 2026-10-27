In Silico PCR regression tests and benchmarks (issue #1969)
==========================================================

The embedded XML suite is tests/pcr_suite.xml and is included in
PrecommitTests_x64.list. It uses synthetic sequences, without external data.

Standalone runner (from the repository root, using the normal UGENE build):
  cmake -S . -B build -DUGENE_BUILD_PCR_TESTS=ON
  cmake --build build --target pcr pcr_tests
  build/dist/plugins/pcr_tests
The executable has an .exe suffix on Windows and may be in a configuration
subdirectory with multi-configuration generators. A single scenario can be
selected by its XML scenario name, e.g. pcr_tests mismatch_limit.

Coverage:
- Retain an exact product behind 49/50/51/100 inexact forward sites, for
  mismatch allowances 1/2/3, and behind 100 inexact reverse sites.
- Return all 80 products with more than 50 sites for each primer.
- Both primer orientations, circular origin crossings and sites near origin.
- Degenerate primers with ambiguous matching enabled and disabled.
- Independent combinations of 0..3-base 5' overhangs, both orientations;
  an overhang when the opposite site is internal to the template.
- Full product metadata and temperature compared with an independent oracle
  that checks every sequence position and every pair: fixed seed 1969,
  40 synthetic templates x 7 maximum product sizes (280 comparisons).
- Deduplicate identical products before thermodynamics; reuse primer Tm.
- Do not expose partially generated results when canceled.
- Shared Tm cache: reuse, settings isolation, invalid results, bounded LRU
  eviction, oversized input bypass, external-buffer ownership and concurrency.
- Degenerate-primer temperatures remain specific to the product binding site.

Benchmarks:
  pcr_tests --benchmark ordinary
  pcr_tests --benchmark rejected
  pcr_tests --benchmark dense
  pcr_tests --benchmark sparse
Each command performs one warm-up and seven timed runs and prints JSON.
Timing includes prepare, both real FindAlgorithmTask searches, and PCR run.
It excludes process startup, fixture construction, object destruction, and
GUI rendering. The deterministic UGENE Rough calculator is used throughout.
The search/prepare and pairing timings are reported separately.
The shared calculator cache remains warm across iterations. Peak RSS on
Unix is the process high-water mark, including libraries, warm-up and runs;
it is not a measurement of just the algorithm's allocations.

Inputs:
- ordinary: seeded 1 Mb random DNA with one inserted 80 bp amplicon, one
  allowed mismatch per primer, maximum product length 80 bp.
- rejected: 10,000 sites with a mismatch at the forward primer's 3' end,
  followed by one exact 80 bp amplicon; one allowed mismatch, perfect tail 15.
- dense: 49 copies of the 80 bp amplicon separated by 30 C bases; maximum
  length is the whole sequence. All 1,225 valid products are required.
- sparse: 10,000 such copies, maximum length 80 bp. All 10,000 local products
  are required. Exhaustive pairing would visit 100,000,000 pairs.

Use the same runner source/compiler flags/common libraries with three PCR
implementations: base revision 2eca5ce40d; that base with only the limit
changed to MAX_RESULT_TO_FIND_UNLIMITED; and the fix. When compiling a base
snapshot, use its matching InSilicoPcrTask.h as well as its .cpp (including
for the runner and test compilation units, to preserve class layout).
Use matching TmCalculator headers and U2Algorithm libraries for each revision;
older binaries must retain the calculator library they were compiled against.

Run without a concurrent build or other CPU-intensive work:
  python3 tests/pcr/benchmark.py --before /path/to/base-runner \
      --unlimited /path/to/uncapped-base-runner --after /path/to/fixed-runner \
      --output /path/to/results.json
The script repeats each process three times and retains raw samples plus
medians. Compare output counts as well as time: the limited base silently
loses products in the rejected and sparse cases. The uncapped base provides
an equal-result comparison for the optimization.

Implementation notes:
Validation is per binding site. Complementary sites are indexed by their
right endpoint, including any overhang, and each left site queries only the
allowed length interval. Circular negative distances use a separate shifted
interval. Pair selection is O(R log R + L log L + L log R + K), where K is
the number of geometrically admissible pairs. Search and per-product
sequence/temperature costs remain additional. A genuinely large output still
requires corresponding time and memory; there is no silent candidate cap.

TmCalculator now owns a per-instance LRU cache with a 64 KiB cost budget
(sequence/key/value bytes, excluding Qt bookkeeping). Longer sequences bypass
the cache. Cache access is mutex-protected; calculations run outside the lock.
Settings are immutable and caches are never shared across calculator instances.
Rough and Primer3 implement calculateMeltingTemperature(); callers continue
to use getMeltingTemperature(). PCR no longer needs its own calculator wrapper.
See TM_CACHE_RESULTS.txt for follow-up validation and performance measurements.

The local arm64 validation needed two existing build-configuration workarounds:
select arm64 instead of the root CMake file's forced x86_64; compile the
bundled rANS_static4x16pr.c without __ARM_NEON because its NEON implementation
is absent from this checkout. These were local generated-build settings;
neither workaround is part of this PCR patch.
