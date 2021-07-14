#!/bin/bash
#
# This script runs XML tests suite.
# The script expects UGENE binaries to be present in 'ugene' folder, tests in the 'ugene_git/test',
# test data in 'test_data' and external tools in 'tools'.
#
TEAMCITY_WORK_DIR=$(pwd)
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
UGENE_OUTPUT_DIR="${TEAMCITY_WORK_DIR}/out"
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"
SCRIPTS_DIR="${SOURCE_DIR}/etc/script/linux"

# UGENE_TESTS_PATH is used both by the script and by UGENE tests.
export UGENE_TESTS_PATH="${SOURCE_DIR}/tests"

# Disable crash handler.
export USE_CRASH_HANDLER=0
export UGENE_DEV=1

# Disable check for updates. TODO: rename.
export UGENE_GUI_TEST=1

# Make UGENE dump all logs to console.
export UGENE_PRINT_TO_CONSOLE=true

# Force English by default.
export LANG=en_US.UTF-8
export UGENE_TRANSLATION=en

if [ -z "${UGENE_SKIP_DISPLAY_INITIALIZATION}" ]; then UGENE_SKIP_DISPLAY_INITIALIZATION="0"; fi
if [ -z "${DISPLAY}" ]; then export DISPLAY=":551"; fi

# Use 1 thread by default: this is the safest option because some tests are multi-threaded inside.
if [ -z "$UGENE_TEST_THREADS" ]; then UGENE_TEST_THREADS="1"; fi

# Default test suite is a pre-commit suite.
if [ -z "$UGENE_TEST_SUITE" ]; then UGENE_TEST_SUITE="PrecommitTests_x64.list"; fi

# XML tests search data in this dir.
if [ -z "${COMMON_DATA_DIR}" ]; then export COMMON_DATA_DIR="${TEAMCITY_WORK_DIR}/test_data/_common_data"; fi
export TEMP_DATA_DIR="${UGENE_OUTPUT_DIR}/tmp"

echo "Work dir: '${TEAMCITY_WORK_DIR}', UGENE_DIR: '${UGENE_DIR}', test suite path: '${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}', test data dir: '${COMMON_DATA_DIR}'"

echo "##teamcity[blockOpened name='Environment']"
env
echo "##teamcity[blockClosed name='Environment']"

# Kill any existing ugene instances that may left in a hanging state since the previous run.
killall -q -9 ugeneui
killall -q -9 ugenecl

# Copy 'tools' into 'ugene' dir.
# Reason: Teamcity cleanup of 'ugene' dir removes all symlinks recursively and cleanup the original 'tools' repository.
echo "##teamcity[blockOpened name='Copy tools']"
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}"/tools "${UGENE_DIR}" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}
echo "##teamcity[blockClosed name='Copy tools']"

echo "##teamcity[blockOpened name='Running tests: ${UGENE_TEST_SUITE}']"

# Restart VNC server.
if [ "${UGENE_SKIP_DISPLAY_INITIALIZATION}" -ne "1" ]; then
  "${SCRIPTS_DIR}/initialize_displays.sh" start ${DISPLAY}
fi

rm -rf "${UGENE_OUTPUT_DIR}"
mkdir -p "${UGENE_OUTPUT_DIR}"
if stdbuf -oL "${UGENE_DIR}/ugeneui" --test-suite="${UGENE_TESTS_PATH}/${UGENE_TEST_SUITE}" \
  --test-report="${UGENE_OUTPUT_DIR}/test_report.html" \
  --test-threads="${UGENE_TEST_THREADS}" \
  --ini-file="${UGENE_OUTPUT_DIR}/ugene.ini"; then
  echo "buildStatus status='SUCCESS'"
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Testing failed']"
fi

# Stop displays.
if [ "${UGENE_SKIP_DISPLAY_INITIALIZATION}" -ne "1" ]; then
  "${SCRIPTS_DIR}/initialize_displays.sh" stop ${DISPLAY}
fi

echo "##teamcity[blockClosed name='Running tests: ${UGENE_TEST_SUITE}']"
