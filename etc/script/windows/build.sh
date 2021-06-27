# The script builds release version of UGENE in 'ugene' folder
# and adds all required QT libraries, data files, license files
# Only 'tools' dir is not added.
# The result  build is located in ugene/src/_release dir.

TEAMCITY_WORK_DIR="$(cygpath -aw .)"
echo "TEAMCITY_WORK_DIR $TEAMCITY_WORK_DIR"

SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene"
BUILD_DIR="${SOURCE_DIR}/src/_release"

cd "${SOURCE_DIR}" || {
  echo "Can't change dir to '${SOURCE_DIR}'"
  exit 1
}

echo "Running qmake"
"${QT_DIR}/bin/qmake.exe" -r ugene.pro

echo "Running nmake"
nmake Release

echo "Copy resources"
cp "${SOURCE_DIR}/LICENSE.txt" "${BUILD_DIR}"
cp "${SOURCE_DIR}/LICENSE.3rd_party.txt" "${BUILD_DIR}"
cp -r "${SOURCE_DIR}/data" "${BUILD_DIR}"
cp "${PATH_TO_INCLUDE_LIBS}/"* "${BUILD_DIR}"

echo copy Qt libraries
cp "${QT_DIR}/bin/Qt5Core.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Gui.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Multimedia.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5MultimediaWidgets.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Network.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5OpenGL.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Positioning.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5PrintSupport.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Qml.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Quick.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Script.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5ScriptTools.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Sensors.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Sql.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Svg.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Test.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Widgets.dll" "${BUILD_DIR}"
cp "${QT_DIR}/bin/Qt5Xml.dll" "${BUILD_DIR}"

mkdir "${BUILD_DIR}/sqldrivers"
cp "${QT_DIR}/plugins/sqldrivers/qsqlmysql.dll" "${BUILD_DIR}/sqldrivers"

mkdir "${BUILD_DIR}\imageformats"
cp "${QT_DIR}/plugins/imageformats/qgif.dll" "${BUILD_DIR}/imageformats"
cp "${QT_DIR}/plugins/imageformats/qjpeg.dll" "${BUILD_DIR}/imageformats"
cp "${QT_DIR}/plugins/imageformats/qsvg.dll" "${BUILD_DIR}/imageformats"
cp "${QT_DIR}/plugins/imageformats/qtiff.dll" "${BUILD_DIR}/imageformats"

mkdir "${BUILD_DIR}/platforms"
cp "${QT_DIR}/plugins/platforms\qwindows.dll" "${BUILD_DIR}/platforms"
