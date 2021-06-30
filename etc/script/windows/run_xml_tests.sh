#!/bin/bash

# This script runs XML tests suite.
# The script expects UGENE binaries to be present in 'ugene' folder, tests in the 'ugene_git/tests',
# test data in 'test_data' and external tools in 'tools'.

TEAMCITY_WORK_DIR="$(cygpath -aw .)"
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"

UGENE_OUTPUT_DIR="${TEAMCITY_WORK_DIR}/out"

# Use 1 thread by default: this is the safest option because some tests are multi-threaded inside.
if [ -z "$UGENE_TEST_THREADS" ]; then UGENE_TEST_THREADS="1"; fi

# Default test suite is a pre-commit suite.
if [ -z "$UGENE_TEST_SUITE" ]; then UGENE_TEST_SUITE="PrecommitTests_x64.list"; fi

# External tools location.
export UGENE_TOOLS_DIR="${TEAMCITY_WORK_DIR}/tools"

# Disable crash handler
export USE_CRASH_HANDLER=0

# Force English by default.
export UGENE_TRANSLATION=en

# Make UGENE dump all logs to console.
export UGENE_PRINT_TO_CONSOLE=true

# UGENE_DEV disables crash reporter.
export UGENE_DEV=1

# UGENE_TESTS_PATH is used both by the script and by UGENE tests.
export UGENE_TESTS_PATH="${SOURCE_DIR}/tests"

# XML tests search data in this dir.
if [ -z "${COMMON_DATA_DIR}" ]; then export COMMON_DATA_DIR="${TEAMCITY_WORK_DIR}/test_data/_common_data"; fi

echo "##teamcity[blockOpened name='Environment']"
echo "Work dir: '${TEAMCITY_WORK_DIR}', UGENE_DIR: '${UGENE_DIR}', test suite path: '${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}', test data dir: '${COMMON_DATA_DIR}'"
env
echo "##teamcity[blockClosed name='Environment']"

echo "##teamcity[blockOpened name='Running tests: ${UGENE_TEST_SUITE}']"

# Kill any existing ugene instances that may left in a hanging state since the previous run.
killall -q -9 ugeneui.exe
killall -q -9 ugenecl.exe

rm -rf "${UGENE_OUTPUT_DIR}"

echo "${UGENE_DIR}/ugeneui.exe --test-suite=${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE} --test-report=${UGENE_OUTPUT_DIR}/test_report.html --test-threads=${UGENE_TEST_THREADS} --ini-file=${UGENE_OUTPUT_DIR}/ugene.ini"
"${UGENE_DIR}/ugeneui.exe" --test-suite="${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}" \
  --test-report="${UGENE_OUTPUT_DIR}/test_report.html" \
  --test-threads="${UGENE_TEST_THREADS}" \
  --ini-file="${UGENE_OUTPUT_DIR}\ugene.ini"

EXIT_CODE=$?

# Kill all bad UGENE instances copies that left, so no files are blocked.
killall -q -9 ugeneui.exe
killall -q -9 ugenecl.exe

if [ ${EXIT_CODE} -ne 0 ]; then
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Testing failed, exit code: ${EXIT_CODE}']"
fi

echo "##teamcity[blockClosed name='Running tests: ${UGENE_TEST_SUITE}']"
