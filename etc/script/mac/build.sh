#!/bin/bash

# The script builds release version of UGENE.
# Source code of UGENE is expected to be in 'ugene' folder, the build version is in 'ugene/src/_release'.
# The script does not pack the build into MacOS application bundle.

if [ ! -d "${QT_DIR}" ]; then
  echo "QT_DIR: ${QT_DIR} is not found."
  exit 1
fi

TEAMCITY_WORK_DIR=$(pwd)
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
BUILD_DIR="${TEAMCITY_WORK_DIR}/build"

cd "${UGENE_DIR}" || {
  echo "Can't change dir to '${UGENE_DIR}'"
  exit 1
}

# Needed by CMake.
export Qt5_DIR="${QT_DIR}"

echo "##teamcity[blockOpened name='env']"
env
echo "##teamcity[blockClosed name='env']"
if [ -z "${UGENE_BUILD_AND_TEST_SKIP_CLEAN}" ]; then UGENE_BUILD_AND_TEST_SKIP_CLEAN="0"; fi

##### Clean ####
if [ "${UGENE_BUILD_AND_TEST_SKIP_CLEAN}" -eq "1" ]; then
  echo "Skipping clean"
elif [ "${UGENE_BUILD_AND_TEST_SKIP_CLEAN}" -eq "2" ]; then
  echo "##teamcity[blockOpened name='fast clean']"
  rm -rf "${BUILD_DIR}"
  echo "##teamcity[blockClosed name='fast clean']"
else
  echo "##teamcity[blockOpened name='make clean']"
  if
      cmake --build "${BUILD_DIR}" --target clean
    then
      echo "Clean completed successfully"
    else
      echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. cmake clean failed']"
      exit 1
    fi
  echo "##teamcity[blockClosed name='make clean']"
fi

#### CMake ####
echo "##teamcity[blockOpened name='CMake']"
if
  cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -S "${UGENE_DIR}" -B "${BUILD_DIR}"
then
  echo "CMake finished successfully"
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. CMake failed']"
  exit 1
fi
echo "##teamcity[blockClosed name='CMake']"

#### Make ####
UGENE_MAKE_PARAMS="-j $(sysctl -n hw.ncpu)"
echo "##teamcity[blockOpened name='make ${UGENE_MAKE_PARAMS}']"
if
  # We want these params to be individual params, so disabling inspection for quotes.
  # shellcheck disable=SC2086
  cmake --build "${BUILD_DIR}" --target all -- ${UGENE_MAKE_PARAMS}
then
  echo
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. make ${UGENE_MAKE_PARAMS} failed']"
  exit 1
fi
echo "##teamcity[blockClosed name='make ${UGENE_MAKE_PARAMS}']"
