#!/bin/bash
# The script builds release version of UGENE in 'ugene' folder
# and adds all required QT libraries, data files, license files
# Only 'tools' dir is not added.
# The result  build is located in ugene/src/_release dir.

TEAMCITY_WORK_DIR="$(cygpath -aw .)"
echo "TEAMCITY_WORK_DIR $TEAMCITY_WORK_DIR"

SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene"
BUNDLE_DIR="${TEAMCITY_WORK_DIR}/bundle"
BUILD_DIR="${SOURCE_DIR}/src/_release"

rm -rf "${BUILD_DIR}"

cd "${SOURCE_DIR}" || {
  echo "Can't change dir to '${SOURCE_DIR}'"
  exit 1
}

echo "##teamcity[blockOpened name='qmake']"
echo "Running qmake"
#"${QT_DIR}/bin/qmake.exe" -r ugene.pro || {
"${QT_DIR}/bin/qmake.exe" -r ugene.pro -tp vc ${UGENE_QMAKE_PARAMS} || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. qmake failed']"
  exit 1
}
echo "##teamcity[blockClosed name='qmake']"

echo "##teamcity[blockOpened name='nmake/devenv']"
echo "Running nmake/devenv"
#nmake Release || {
devenv ugene.sln /build Release /out "build.log" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. nmake/devenv failed']"
  exit 1
}
echo "##teamcity[blockClosed name='nmake/devenv']"

echo "##teamcity[blockOpened name='bundle']"
rm -rf "${BUNDLE_DIR}"
cp -r "${BUILD_DIR}" "${BUNDLE_DIR}"
rm "${BUNDLE_DIR}/"*.lib
rm "${BUNDLE_DIR}/"*.pdb
rm "${BUNDLE_DIR}/"*.exp
rm "${BUNDLE_DIR}/plugins/"*.lib
rm "${BUNDLE_DIR}/plugins/"*.pdb
rm "${BUNDLE_DIR}/plugins/"*.exp

echo "Copy resources"
cp "${SOURCE_DIR}/LICENSE.txt" "${BUNDLE_DIR}"
cp "${SOURCE_DIR}/LICENSE.3rd_party.txt" "${BUNDLE_DIR}"
cp -r "${SOURCE_DIR}/data" "${BUNDLE_DIR}"
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
copy_with_pdb "${QT_DIR}/bin/Qt5Multimedia.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5MultimediaWidgets.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Network.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Positioning.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5PrintSupport.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Qml.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Quick.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Script.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5ScriptTools.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Sensors.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Sql.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Svg.dll" "${BUNDLE_DIR}"
copy_with_pdb "${QT_DIR}/bin/Qt5Test.dll" "${BUNDLE_DIR}"
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
