#!/bin/bash
# Copies QT and other 3rd party libs (like openssl) to the UGENE dir.
# As the result UGENE does not depend on libraries in other folders than its own.
# Usage: copy_qt_files_to_ugene <QT_DIR> <UGENE_DIR>.
QT_DIR=$1
UGENE_DIR=$2

if [ ! -d "${QT_DIR}" ]; then
  echo "QT_DIR: ${QT_DIR} is not found."
  exit 1
fi

if [ ! -d "${UGENE_DIR}" ]; then
  echo "UGENE_DIR: ${UGENE_DIR} does not exist."
  exit 1
fi

echo "copy_qt_files_to_ugene.sh is called with QT_DIR: '${QT_DIR}', UGENE dir: '${UGENE_DIR}'"

# QT core libraries.
QT_LIBS=("Core" "DBus" "Gui" "Network" "PrintSupport" "Script" "ScriptTools" "Svg" "Test" "Widgets" "XcbQpa" "Xml")
for LIB in ${QT_LIBS[*]}; do
  FULL_LIB_NAME="libQt5${LIB}.so.5"
  rm -rf "${UGENE_DIR:?}/${FULL_LIB_NAME}"
  cp "${QT_DIR}/lib/${FULL_LIB_NAME}" "${UGENE_DIR}/"
done
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN' "${UGENE_DIR}"/*.so.5

# Platform drivers.
rm -rf "${UGENE_DIR}/platforms"
mkdir "${UGENE_DIR}/platforms"
cp "${QT_DIR}/plugins/platforms/libqxcb.so" "${UGENE_DIR}/platforms"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN/..' "${UGENE_DIR}/platforms"/*.so
# Qt 5.15 requires external libxcb-xinerama.so.0 library.
cp "${QT_DIR}/lib_extra/libxcb-xinerama.so.0" "${UGENE_DIR}/"

# Image formats.
rm -rf "${UGENE_DIR}/imageformats"
mkdir "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqgif.so" "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqicns.so" "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqjpeg.so" "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqsvg.so" "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqtiff.so" "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqwbmp.so" "${UGENE_DIR}/imageformats"
cp -r "${QT_DIR}/plugins/imageformats/libqwebp.so" "${UGENE_DIR}/imageformats"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN/..' "${UGENE_DIR}/imageformats"/*.so

# GTK3 platform theme.
rm -rf "${UGENE_DIR}/platformthemes"
mkdir "${UGENE_DIR}/platformthemes"
cp -r "${QT_DIR}/plugins/platformthemes/libqgtk3.so" "${UGENE_DIR}/platformthemes"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN/..' "${UGENE_DIR}/platformthemes"/*.so

# OpenGL support
rm -rf "${UGENE_DIR}/xcbglintegrations"
mkdir "${UGENE_DIR}/xcbglintegrations"
cp -r "${QT_DIR}/plugins/xcbglintegrations/libqxcb-egl-integration.so" "${UGENE_DIR}/xcbglintegrations"
cp -r "${QT_DIR}/plugins/xcbglintegrations/libqxcb-glx-integration.so" "${UGENE_DIR}/xcbglintegrations"
# shellcheck disable=SC2016
patchelf --force-rpath --set-rpath '$ORIGIN/..' "${UGENE_DIR}/xcbglintegrations"/*.so
