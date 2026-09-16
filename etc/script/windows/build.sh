#!/bin/bash
# The script builds release version of UGENE in 'ugene' folder
# and adds all required QT libraries, data files, license files
# Only 'tools' dir is not added.
# The result  build is located in ugene/src/_release dir.

TEAMCITY_WORK_DIR="$(cygpath -am .)"
echo "TEAMCITY_WORK_DIR $TEAMCITY_WORK_DIR"

UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
BUNDLE_DIR="${TEAMCITY_WORK_DIR}/bundle"
BUILD_DIR="${UGENE_DIR}/build"
DIST_DIR="${BUILD_DIR}/dist"

# Needed by CMake and Qt deployment tools.
export Qt6_DIR="${QT_DIR}"
export CMAKE_PREFIX_PATH="${QT_DIR}"

rm -rf "${BUILD_DIR}"

cd "${UGENE_DIR}" || {
  echo "Can't change dir to '${UGENE_DIR}'"
  exit 1
}

echo "##teamcity[blockOpened name='env']"
env
echo "##teamcity[blockClosed name='env']"

#### CMake ####
echo "##teamcity[blockOpened name='CMake']"
if
#  cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" -S "${UGENE_DIR}" -B "${BUILD_DIR}"
  cmake -DCMAKE_CONFIGURATION_TYPES=Release -G "${CMAKE_GENERATOR:-Visual Studio 17 2022}" -A x64 -S "${UGENE_DIR}" -B "${BUILD_DIR}"
then
  echo "CMake finished successfully"
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. CMake failed']"
  exit 1
fi
echo "##teamcity[blockClosed name='CMake']"

echo "##teamcity[blockOpened name='make']"
if
  # We want these params to be individual params, so disabling inspection for quotes.
  # shellcheck disable=SC2086
  cmake --build "${BUILD_DIR}" --parallel --config Release
then
  echo
else
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. make failed']"
  exit 1
fi
echo "##teamcity[blockClosed name='make']"

echo "##teamcity[blockOpened name='bundle']"
rm -rf "${BUNDLE_DIR}"
cp -r "${DIST_DIR}" "${BUNDLE_DIR}"
rm "${BUNDLE_DIR}/"*.exp
rm "${BUNDLE_DIR}/"*.lib
rm "${BUNDLE_DIR}/"*.map
rm "${BUNDLE_DIR}/"*.pdb
rm "${BUNDLE_DIR}/plugins/"*.exp
rm "${BUNDLE_DIR}/plugins/"*.lib
rm "${BUNDLE_DIR}/plugins/"*.map
rm "${BUNDLE_DIR}/plugins/"*.pdb

echo "Copy resources"
cp "${UGENE_DIR}/LICENSE.txt" "${BUNDLE_DIR}"
cp "${UGENE_DIR}/LICENSE.3rd_party.txt" "${BUNDLE_DIR}"
cp -r "${UGENE_DIR}/data" "${BUNDLE_DIR}"
if [ -n "${PATH_TO_INCLUDE_LIBS}" ] && [ -d "${PATH_TO_INCLUDE_LIBS}" ]; then
  cp "${PATH_TO_INCLUDE_LIBS}/"* "${BUNDLE_DIR}"
fi

echo "Deploy Qt libraries"
WINDEPLOYQT_ARGS=(
  --release
  --no-translations
  --compiler-runtime
  --dir "${BUNDLE_DIR}"
)
if [ "${UGENE_BUILD_KEEP_PDB_FILES}" == "1" ]; then
  WINDEPLOYQT_ARGS+=(--pdb)
fi

"${QT_DIR}/bin/windeployqt.exe" "${WINDEPLOYQT_ARGS[@]}" \
  "${BUNDLE_DIR}/ugeneui.exe" \
  "${BUNDLE_DIR}/ugenecl.exe" \
  "${BUNDLE_DIR}/ugenem.exe" \
  "${BUNDLE_DIR}/plugins_checker.exe" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. windeployqt failed']"
  exit 1
}

echo "##teamcity[blockClosed name='bundle']"
