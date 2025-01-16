#!/bin/bash
#
# This script builds ugene in the 'ugene' folder into a target folder (ugene/src/_release or ugene/src/_debug).
# After the build the script copies & patches all required QT libraries and 'data' folder into the target folder.
# As the result the target folder contains a portable standalone package of UGENE with all available functionality
# and plugins included.
#
# The bundle does not include test data or external tools packages.
# The script requires only 'QT_DIR' parameter and builds 'release' bundle by default.
#

#### Configure ####
if [ ! -d "${QT_DIR}" ]; then
  echo "QT_DIR: ${QT_DIR} is not found."
  exit 1
fi

TEAMCITY_WORK_DIR=$(pwd)
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
#UGENE_DIR="${TEAMCITY_WORK_DIR}"
SCRIPTS_DIR="${UGENE_DIR}/etc/script/linux"
BUILD_DIR="${UGENE_DIR}/cmake-build-release"
DIST_DIR="${BUILD_DIR}/dist"

# Needed by CMake.
export Qt5_DIR="${QT_DIR}"

echo "##teamcity[blockOpened name='env']"
env
echo "##teamcity[blockClosed name='env']"

# Below this point the script works in 'UGENE_DIR' folder.
cd "${UGENE_DIR}" || {
  echo "Can't change dir to '${UGENE_DIR}'"
  exit 1
}

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
UGENE_MAKE_PARAMS="-j $(grep "cpu cores" /proc/cpuinfo | uniq | cut -d ':' -f2)"
echo "##teamcity[blockOpened name='make ${UGENE_MAKE_PARAMS}']"
if
  # We want these params to be individual params, so disabling inspection for quotes.
  # shellcheck disable=SC2086
  cmake --build "${BUILD_DIR}" -- ${UGENE_MAKE_PARAMS}
then
  echo
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. make ${UGENE_MAKE_PARAMS} failed']"
  exit 1
fi
echo "##teamcity[blockClosed name='make ${UGENE_MAKE_PARAMS}']"

#### BUNDLE ####
echo "##teamcity[blockOpened name='Bundle']"

# Copy & patch Qt libs.
"${SCRIPTS_DIR}/copy_qt_files_to_ugene.sh" "${QT_DIR}" "${DIST_DIR}" || {
  echo "'copy_qt_files_to_ugene.sh' script failed"
  exit 1
}

# Patch RPATH for UGENE binaries and plugins.

# Update RPATH for all shared libraries in the plugins directory.
# shellcheck disable=SC2016
find "${DIST_DIR}/plugins" -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# Update RPATH for all shared libraries in the build directory.
# shellcheck disable=SC2016
find "${DIST_DIR}" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN' {} \;

# Update RPATH for individual binaries.
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN' "${DIST_DIR}/plugins_checker"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN' "${DIST_DIR}/ugenecl"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN' "${DIST_DIR}/ugenem"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN' "${DIST_DIR}/ugeneui"

# Add data.
rm -rf "${DIST_DIR}/data"
cp -r ./data "${DIST_DIR}"

cp "${UGENE_DIR}/etc/shared/ugene.png" "${DIST_DIR}/"
cp "${UGENE_DIR}/LICENSE.txt" "${DIST_DIR}/"
cp "${UGENE_DIR}/LICENSE.3rd_party.txt" "${DIST_DIR}/"

echo "Compressing app into a tar.gz"
rm "${TEAMCITY_WORK_DIR}/"*.gz
tar cfz "${TEAMCITY_WORK_DIR}/bundle-linux-b${TEAMCITY_BUILD_COUNTER}.tar.gz" -C "${DIST_DIR}" .

echo "##teamcity[blockClosed name='Bundle']"
