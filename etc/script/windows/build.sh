#!/bin/bash
# The script builds release version of UGENE in 'ugene' folder
# and adds all required QT libraries, data files, license files
# Only 'tools' dir is not added.
# The result  build is located in ugene/src/_release dir.

TEAMCITY_WORK_DIR="$(cygpath -aw .)"
echo "TEAMCITY_WORK_DIR $TEAMCITY_WORK_DIR"

UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
BUNDLE_DIR="${TEAMCITY_WORK_DIR}/bundle"
BUILD_DIR="${UGENE_DIR}/build"
DIST_DIR="${BUILD_DIR}/dist"

# Needed by CMake.
export Qt5_DIR="${QT_DIR}"

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
  cmake -DCMAKE_CONFIGURATION_TYPES=Release -G "Visual Studio 16 2019" -S "${UGENE_DIR}" -B "${BUILD_DIR}"
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
cp "${PATH_TO_INCLUDE_LIBS}/"* "${BUNDLE_DIR}"

echo copy Qt libraries

function copy_with_pdb() {
  LIB_PATH="${1}"
  TARGET_DIR="${2}"
  echo "Copying ${LIB_PATH}"
  cp "${LIB_PATH}" "${TARGET_DIR}"

  if [ "${UGENE_BUILD_KEEP_PDB_FILES}" == "1" ]; then
    PDB_PATH="${LIB_PATH/.dll/.pdb}"
    if [ -f "${PDB_PATH}" ]; then
      echo "Copying ${PDB_PATH}"
      cp "${PDB_PATH}" "${TARGET_DIR}"
    fi
  fi
}

copy_with_pdb "${QT_DIR}/bin/Qt5Core.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Gui.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Network.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5NetworkAuth.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5PrintSupport.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Svg.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Test.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5WebSockets.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Widgets.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Xml.dll" "${BUNDLE_DIR}"

# Open GL support.
copy_with_pdb "${QT_DIR}/bin/libEGL.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/libGLESv2.dll" "${BUNDLE_DIR}"

mkdir "${BUNDLE_DIR}/styles"
copy_with_pdb "${QT_DIR}/plugins/styles/qwindowsvistastyle.dll" "${BUNDLE_DIR}/styles"

mkdir "${BUNDLE_DIR}/imageformats"
copy_with_pdb "${QT_DIR}/plugins/imageformats/qgif.dll" "${BUNDLE_DIR}/imageformats"
copy_with_pdb "${QT_DIR}/plugins/imageformats/qjpeg.dll" "${BUNDLE_DIR}/imageformats"
copy_with_pdb "${QT_DIR}/plugins/imageformats/qsvg.dll" "${BUNDLE_DIR}/imageformats"
copy_with_pdb "${QT_DIR}/plugins/imageformats/qtiff.dll" "${BUNDLE_DIR}/imageformats"

mkdir "${BUNDLE_DIR}/platforms"
copy_with_pdb "${QT_DIR}/plugins/platforms/qwindows.dll" "${BUNDLE_DIR}/platforms"

mkdir "${BUNDLE_DIR}/printsupport"
copy_with_pdb "${QT_DIR}/plugins/printsupport/windowsprintersupport.dll" "${BUNDLE_DIR}/printsupport"

echo "##teamcity[blockClosed name='bundle']"
