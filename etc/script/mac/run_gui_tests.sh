#!/bin/bash
#
# This script runs GUI tests suite.
# The script expects UGENE binaries to be present in 'ugene_app' folder, tests in the 'test_data',
# ugene sources in 'ugene_git' and external tools in 'tools'.
#

# ============== Environment for test script
TEAMCITY_WORK_DIR=$(pwd)
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene_app/Unipro UGENE.app/Contents/MacOS"
echo "TEAMCITY_WORK_DIR: '${TEAMCITY_WORK_DIR}', UGENE_DIR: '${UGENE_DIR}'"

if [ -z "${UGENE_USE_DIRECT_API_TO_OPEN_FILES}" ]; then export UGENE_USE_DIRECT_API_TO_OPEN_FILES="1"; fi
if [ -z "${UGENE_USE_MASTER_USER_INI}" ]; then export UGENE_USE_MASTER_USER_INI="1"; fi
if [ -z "${UGENE_GUI_TEST_SUITE}" ]; then export UGENE_GUI_TEST_SUITE="1"; fi

# Data/files dirs prefixes.
UGENE_WORKFLOW_OUTPUT_DIR="${TEAMCITY_WORK_DIR}/workflow_output"
UGENE_SAVE_FILES_DIR="${TEAMCITY_WORK_DIR}/UGENE_Files"
UGENE_SAVE_DATA_DIR="${TEAMCITY_WORK_DIR}/UGENE_Data"

# ============== Environment for UGENE: all parameters must be exported
# Disable crash handler
export USE_CRASH_HANDLER=0
export UGENE_SEND_CRASH_REPORTS=0
export UGENE_GUI_TEST=1
export UGENE_DEV=1
export UGENE_USE_NATIVE_DIALOGS=0
export UGENE_PRINT_TO_CONSOLE=1

# Force English by default.
export LANG=en_US.UTF-8
export UGENE_TRANSLATION=en

# ============== Cleanup
# Cleanup files from the previous run.
rm -rf "${UGENE_WORKFLOW_OUTPUT_DIR}"*
rm -rf "${UGENE_SAVE_FILES_DIR}"*
rm -rf "${UGENE_SAVE_DATA_DIR}"*

echo "##teamcity[blockOpened name='Environment']"
env
echo "##teamcity[blockClosed name='Environment']"

# Copy 'tools' into 'ugene' dir.
# Reason: Teamcity cleanup of 'ugene' dir removes all symlinks recursively and cleanup the original 'tools' repository.
echo "##teamcity[blockOpened name='Copy tools']"
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}/tools" "${UGENE_DIR}" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}
echo "##teamcity[blockClosed name='Copy tools']"

# ============== Run tests
echo "##teamcity[blockOpened name='Running tests']"

# Create a blueprint copy of _common_data dir.
echo "##teamcity[blockOpened name='Copying test data']"
mkdir -p "test/_common_data"
rsync -a --delete test_data/_common_data/ test/_common_data/
echo "Finished copying data"
echo "##teamcity[blockClosed name='Copying test data']"

export UGENE_MASTER_USER_INI="${TEAMCITY_WORK_DIR}/ugene-master.ini"

export UGENE_TESTS_PATH="${TEAMCITY_WORK_DIR}/test/"
export UGENE_DATA_PATH="${TEAMCITY_WORK_DIR}/data/"
export UGENE_SAVE_DATA_DIR="${UGENE_SAVE_DATA_DIR}"
export UGENE_SNPEFF_DB_LIST="${UGENE_SAVE_DATA_DIR}/SnpEff_DB.list"
export UGENE_PRIMER_LIBRARY_PATH="${UGENE_SAVE_DATA_DIR}/primer_library.ugenedb"

"${UGENE_DIR}/ugeneui" --ini-file="${UGENE_MASTER_USER_INI}" --gui-test-suite="${UGENE_GUI_TEST_SUITE}" | tee "output.txt"

echo "##teamcity[blockClosed name='Running tests']"
