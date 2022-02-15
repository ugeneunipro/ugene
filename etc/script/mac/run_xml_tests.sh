#!/bin/bash
#
# This script runs XML tests suite.
# The script expects bundled UGENE app binaries to be present in 'ugene' folder, tests in the 'ugene_git/test',
# test data in 'test_data' and external tools in 'tools'.
#
TEAMCITY_WORK_DIR=$(pwd)
UGENE_APP_DIR="${TEAMCITY_WORK_DIR}/ugene/Unipro UGENE.app"
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"
OUTPUT_DIR="${TEAMCITY_WORK_DIR}/out"

# Disable crash handler
export UGENE_USE_CRASH_HANDLER=0
export UGENE_DEV=1

# Disable check for updates. TODO: rename.
export UGENE_GUI_TEST=1

# Force English by default.
export LANG=en_US.UTF-8
export UGENE_TRANSLATION=en

# Make UGENE dump all logs to console.
export UGENE_PRINT_TO_CONSOLE=true

# Use 1 thread by default: this is the safest option because some tests are multi-threaded inside.
if [ -z "${UGENE_TEST_THREADS}" ]; then UGENE_TEST_THREADS="1"; fi

# Default test suite is a pre-commit suite.
if [ -z "${UGENE_TEST_SUITE}" ]; then UGENE_TEST_SUITE="PrecommitTests_x64.list"; fi

# UGENE_TESTS_PATH is used both by the script and by UGENE tests.
export UGENE_TESTS_PATH="${SOURCE_DIR}/tests"

# XML tests search data in this dir.
if [ -z "$COMMON_DATA_DIR" ]; then export COMMON_DATA_DIR="${TEAMCITY_WORK_DIR}/test_data/_common_data"; fi
export TEMP_DATA_DIR="${OUTPUT_DIR}/tmp"

echo "Work dir: '${TEAMCITY_WORK_DIR}', UGENE_APP_DIR: '${UGENE_APP_DIR}', test suite path: '${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}', test data dir: '${COMMON_DATA_DIR}'"

echo "##teamcity[blockOpened name='Environment']"
env
echo "##teamcity[blockClosed name='Environment']"

# Copy 'tools' into 'ugene' dir.
# Reason: Teamcity cleanup of 'ugene' dir removes all symlinks recursively and cleanup the original 'tools' repository.
echo "##teamcity[blockOpened name='Copy tools']"
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}/tools" "$UGENE_APP_DIR/Contents/MacOS" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}
echo "##teamcity[blockClosed name='Copy tools']"

echo "##teamcity[blockOpened name='Running tests: ${UGENE_TEST_SUITE}']"

# Kill any existing hanging builds.
echo "Killing hanging runs of ugeneui & ugenecl"
killall -9 ugeneui 2>&1
killall -9 ugenecl 2>&1

rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

echo "Running: $UGENE_APP_DIR/Contents/MacOS/ugeneui" --test-suite="${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}" \
  --test-report="${OUTPUT_DIR}/test_report.html" \
  --test-threads="${UGENE_TEST_THREADS}" \
  --ini-file="$OUTPUT_DIR/ugene.ini"

if "$UGENE_APP_DIR/Contents/MacOS/ugeneui" --test-suite="${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}" \
  --test-report="${OUTPUT_DIR}/test_report.html" \
  --test-threads="${UGENE_TEST_THREADS}" \
  --ini-file="$OUTPUT_DIR/ugene.ini"; then
  echo "buildStatus status='SUCCESS'"
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Testing failed']"
fi

echo "##teamcity[blockClosed name='Running tests: ${UGENE_TEST_SUITE}']"
