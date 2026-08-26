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

# Qt core libraries.
QT_LIBS=("Core" "Core5Compat" "DBus" "Gui" "Network" "NetworkAuth" "OpenGL" "OpenGLWidgets" "PrintSupport" "Svg" "Test" "WebSockets" "Widgets" "Xml")
for LIB in ${QT_LIBS[*]}; do
  FULL_LIB_NAME="libQt6${LIB}.so.6"
  rm -rf "${UGENE_DIR:?}/${FULL_LIB_NAME}"
  echo "Copying ${QT_DIR}/lib/${FULL_LIB_NAME} to ${UGENE_DIR}"
  cp -L "${QT_DIR}/lib/${FULL_LIB_NAME}" "${UGENE_DIR}/"
done
# shellcheck disable=SC2016
find "${UGENE_DIR}" -maxdepth 1 -name "*.so.6" -exec patchelf --force-rpath --set-rpath '$ORIGIN' {} \;

# Platform drivers.
rm -rf "${UGENE_DIR}/platforms"
mkdir "${UGENE_DIR}/platforms"
for PLATFORM in libqxcb.so libqoffscreen.so libqminimal.so libqeglfs.so; do
  if [ -f "${QT_DIR}/plugins/platforms/${PLATFORM}" ]; then
    cp -L "${QT_DIR}/plugins/platforms/${PLATFORM}" "${UGENE_DIR}/platforms"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/platforms" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# Image formats.
rm -rf "${UGENE_DIR}/imageformats"
mkdir "${UGENE_DIR}/imageformats"
for IMGFMT in "${QT_DIR}"/plugins/imageformats/libq*.so; do
  if [ -f "${IMGFMT}" ]; then
    cp -L "${IMGFMT}" "${UGENE_DIR}/imageformats"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/imageformats" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# Platform themes.
rm -rf "${UGENE_DIR}/platformthemes"
mkdir "${UGENE_DIR}/platformthemes"
for THEME in "${QT_DIR}"/plugins/platformthemes/libq*.so; do
  if [ -f "${THEME}" ]; then
    cp -L "${THEME}" "${UGENE_DIR}/platformthemes"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/platformthemes" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# TLS backends (OpenSSL).
rm -rf "${UGENE_DIR}/tls"
mkdir "${UGENE_DIR}/tls"
for TLS in "${QT_DIR}"/plugins/tls/libq*.so; do
  if [ -f "${TLS}" ]; then
    cp -L "${TLS}" "${UGENE_DIR}/tls"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/tls" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# OpenGL support (xcb GLX/EGL integrations), if present.
rm -rf "${UGENE_DIR}/xcbglintegrations"
mkdir "${UGENE_DIR}/xcbglintegrations"
for GLINT in "${QT_DIR}"/plugins/xcbglintegrations/libq*.so; do
  if [ -f "${GLINT}" ]; then
    cp -L "${GLINT}" "${UGENE_DIR}/xcbglintegrations"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/xcbglintegrations" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# Icon engines.
rm -rf "${UGENE_DIR}/iconengines"
mkdir "${UGENE_DIR}/iconengines"
for ICON in "${QT_DIR}"/plugins/iconengines/libq*.so; do
  if [ -f "${ICON}" ]; then
    cp -L "${ICON}" "${UGENE_DIR}/iconengines"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/iconengines" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;

# Generic plugins (platform integration extras, if any).
rm -rf "${UGENE_DIR}/generic"
mkdir "${UGENE_DIR}/generic"
for GENERIC in "${QT_DIR}"/plugins/generic/libq*.so; do
  if [ -f "${GENERIC}" ]; then
    cp -L "${GENERIC}" "${UGENE_DIR}/generic"
  fi
done
# shellcheck disable=SC2016
find "${UGENE_DIR}/generic" -maxdepth 1 -name "*.so" -exec patchelf --force-rpath --set-rpath '$ORIGIN/..' {} \;
