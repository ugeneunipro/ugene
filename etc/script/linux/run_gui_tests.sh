#!/bin/bash
#
# This script runs GUI tests suite.
# The script expects UGENE binaries to be present in 'ugene' folder, tests in the 'test',
# ugene sources in 'ugene_git' and external tools in 'tools'.
#

# ============== Environment for test script
TEAMCITY_WORK_DIR=$(pwd)
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"
SCRIPTS_DIR="${SOURCE_DIR}/etc/script/linux"
echo "TEAMCITY_WORK_DIR: '${TEAMCITY_WORK_DIR}', UGENE_DIR: '${UGENE_DIR}'"

if [ -z "${UGENE_PREPARE_DATABASE}" ]; then UGENE_PREPARE_DATABASE="1"; fi
if [ -z "${UGENE_SKIP_COPY_TOOLS}" ]; then UGENE_SKIP_COPY_TOOLS="0"; fi
if [ -z "${UGENE_BUILD_AND_TEST_SKIP_TESTING}" ]; then UGENE_BUILD_AND_TEST_SKIP_TESTING="0"; fi
if [ -z "${UGENE_SKIP_DISPLAY_INITIALIZATION}" ]; then UGENE_SKIP_DISPLAY_INITIALIZATION="0"; fi
if [ -z "${FIRST_DISPLAY}" ]; then FIRST_DISPLAY=513; fi
if [ -z "${NUMBER_OF_DISPLAYS}" ]; then NUMBER_OF_DISPLAYS=5; fi
if [ -z "${UGENE_TEST_EXTERNAL_SCRIPT_TO_RESTORE}" ]; then export UGENE_TEST_EXTERNAL_SCRIPT_TO_RESTORE="${SCRIPTS_DIR}/restore_dirs_before_test.sh"; fi
if [ -z "${UGENE_TEST_NUMBER_RERUN_FAILED_TEST}" ]; then export UGENE_TEST_NUMBER_RERUN_FAILED_TEST="1"; fi
if [ -z "${UGENE_TEST_SKIP_BACKUP_AND_RESTORE}" ]; then export UGENE_TEST_SKIP_BACKUP_AND_RESTORE="1"; fi
if [ -z "${UGENE_USE_DIRECT_API_TO_OPEN_FILES}" ]; then export UGENE_USE_DIRECT_API_TO_OPEN_FILES="1"; fi
if [ -z "${UGENE_TEST_ENABLE_VIDEO_RECORDING}" ]; then export UGENE_TEST_ENABLE_VIDEO_RECORDING="1"; fi
if [ -z "${UGENE_USE_MASTER_USER_INI}" ]; then export UGENE_USE_MASTER_USER_INI="1"; fi

# Data/files dirs prefixes.
UGENE_WORKFLOW_OUTPUT_DIR_PREFIX="${TEAMCITY_WORK_DIR}/workflow_output_"
UGENE_SAVE_FILES_DIR_PREFIX="${TEAMCITY_WORK_DIR}/UGENE_Files_"
UGENE_SAVE_DATA_DIR_PREFIX="${TEAMCITY_WORK_DIR}/UGENE_Data_"

# ============== Environment for UGENE: all parameters must be exported
export UGENE_USE_CRASH_HANDLER=0
export UGENE_GUI_TEST=1
export UGENE_DEV=1
export UGENE_USE_NATIVE_DIALOGS=0
export UGENE_PRINT_TO_CONSOLE=1

# Force English by default.
export LANG=en_US.UTF-8
export UGENE_TRANSLATION=en

# ============== Cleanup
# Cleanup files from the previous run.
rm -rf "${UGENE_WORKFLOW_OUTPUT_DIR_PREFIX}"*
rm -rf "${UGENE_SAVE_FILES_DIR_PREFIX}"*
rm -rf "${UGENE_SAVE_DATA_DIR_PREFIX}"*

# Copy 'tools' into 'ugene' dir.
# Reason: Teamcity cleanup of 'ugene' dir removes all symlinks recursively and cleanup the original 'tools' repository.
if [ "${UGENE_SKIP_COPY_TOOLS}" -ne "1" ]; then
  echo "##teamcity[blockOpened name='Copy tools']"
  rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}/tools" "${UGENE_DIR}" || {
    echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
  }
  echo "##teamcity[blockClosed name='Copy tools']"
fi

# Kill any existing ugene instances that may left in a hanging state since the previous run.
killall -q -9 ugeneui
killall -q -9 ugenecl

# Initialize virtual displays.
if [ "${UGENE_SKIP_DISPLAY_INITIALIZATION}" -ne "1" ]; then
  "${SCRIPTS_DIR}/initialize_displays.sh" start
fi

# ============== Run tests
if [ "${UGENE_BUILD_AND_TEST_SKIP_TESTING}" -ne "1" ]; then
  echo "##teamcity[blockOpened name='Running tests']"

  echo "##teamcity[blockOpened name='Environment']"
  env
  echo "##teamcity[blockClosed name='Environment']"

  # Create a copy on _common_data dir for each suite
  echo "##teamcity[blockOpened name='Copying test data']"
  i=1
  while [ ${i} -le ${NUMBER_OF_DISPLAYS} ]; do
    mkdir -p "test${i}/_common_data"
    # Sync tests dir
    rsync -a --delete test/_common_data/ "test${i}/_common_data/"
    # Sync data dir
    rsync -a --delete "${UGENE_DIR}/data/" "data${i}/"
    echo "Finished copying data for SUITE ${i}"
    i=$((i + 1))
  done
  echo "##teamcity[blockClosed name='Copying test data']"

  # Check there are no parallel runs or artifacts from old runs.
  # Sometime UGENE run before is not correctly stopped and may use up to 100% CPU
  i=${FIRST_DISPLAY}
  while [ $i -lt $((FIRST_DISPLAY + NUMBER_OF_DISPLAYS)) ]; do
    export UGENE_MASTER_USER_INI="${UGENE_DIR}/ugene$i-master.ini"
    export UGENE_GUI_TEST_SUITE_NUMBER=$((i - (FIRST_DISPLAY - 1)))
    export UGENE_TESTS_PATH="${TEAMCITY_WORK_DIR}/test${UGENE_GUI_TEST_SUITE_NUMBER}/"
    export UGENE_DATA_PATH="${TEAMCITY_WORK_DIR}/data${UGENE_GUI_TEST_SUITE_NUMBER}/"
    export UGENE_SAVE_DATA_DIR="${UGENE_SAVE_DATA_DIR_PREFIX}${UGENE_GUI_TEST_SUITE_NUMBER}"
    export UGENE_SAVE_FILES_DIR="${UGENE_SAVE_FILES_DIR_PREFIX}${UGENE_GUI_TEST_SUITE_NUMBER}"
    export UGENE_WORKFLOW_OUTPUT_DIR="${UGENE_WORKFLOW_OUTPUT_DIR_PREFIX}${UGENE_GUI_TEST_SUITE_NUMBER}"
    export UGENE_SNPEFF_DB_LIST="${UGENE_SAVE_DATA_DIR}/SnpEff_DB.list"
    export UGENE_PRIMER_LIBRARY_PATH="/tmp/primer_library_${UGENE_GUI_TEST_SUITE_NUMBER}.ugenedb"
    export DISPLAY=":${i}"

    echo "${UGENE_DIR}/ugeneui --gui-test-suite=${UGENE_GUI_TEST_SUITE_NUMBER}"
    if [ "${i}" == "${FIRST_DISPLAY}" ]; then
      stdbuf -oL "${UGENE_DIR}/ugeneui" --ini-file="${UGENE_MASTER_USER_INI}" --gui-test-suite="${UGENE_GUI_TEST_SUITE_NUMBER}" | tee "output${i}.txt" &
    else
      "${UGENE_DIR}/ugeneui" --ini-file="${UGENE_MASTER_USER_INI}" --gui-test-suite="${UGENE_GUI_TEST_SUITE_NUMBER}" >"output${i}.txt" 2>&1 &
    fi
    sleep 10s
    i=$((i + 1))
  done

  # Merge outputs.
  if wait; then
    i=${FIRST_DISPLAY}
    i=$((i + 1))
    while [ $i -lt $((FIRST_DISPLAY + NUMBER_OF_DISPLAYS)) ]; do
      cat "output${i}.txt"
      i=$((i + 1))
    done
    echo "buildStatus status='SUCCESS'"
  else
    echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Testing failed']"
  fi

  echo "##teamcity[blockClosed name='Running tests']"
fi

# Stop displays
if [ "${UGENE_SKIP_DISPLAY_INITIALIZATION}" -ne "1" ]; then
  "${SCRIPTS_DIR}/initialize_displays.sh" stop
fi
