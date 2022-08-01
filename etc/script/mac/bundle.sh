#!/bin/bash

# The script takes a pre-built version of UGENE from 'ugene/src/_release' dir and packs it into Mac OS applications.
# The script does not add external tools into the app and does not sign the result application.

if [ ! -d "${QT_DIR}" ]; then
  echo "QT_DIR: ${QT_DIR} is not found."
  exit 1
fi

TEAMCITY_WORK_DIR=$(pwd)
APP_NAME="Unipro UGENE.app"
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene"
BUILD_DIR="${SOURCE_DIR}/src/_release"
BUNDLE_DIR="${TEAMCITY_WORK_DIR}/bundle"
TARGET_APP_DIR="${BUNDLE_DIR}/${APP_NAME}/"
TARGET_EXE_DIR="${TARGET_APP_DIR}/Contents/MacOS"
TARGET_PLUGINS_DIR="${TARGET_EXE_DIR}/plugins"

echo Creating UGENE application bundle
rm -rf "${BUNDLE_DIR}"
mkdir "${BUNDLE_DIR}"
mkdir "${TARGET_APP_DIR}"
mkdir "${TARGET_APP_DIR}/Contents"
mkdir "${TARGET_APP_DIR}/Contents/Frameworks"
mkdir "${TARGET_APP_DIR}/Contents/Resources"
mkdir "${TARGET_EXE_DIR}"
mkdir "${TARGET_PLUGINS_DIR}"

echo Copying icons
cp "${SOURCE_DIR}/src/ugeneui/images/ugene-doc.icns" "${TARGET_APP_DIR}/Contents/Resources"
cp "${SOURCE_DIR}/src/ugeneui/images/ugeneui.icns" "${TARGET_APP_DIR}/Contents/Resources"
cp "${SOURCE_DIR}/etc/script/mac/dmg/Info.plist" "${TARGET_APP_DIR}/Contents"

echo Copying translations
cp "${BUILD_DIR}"/transl_*.qm "${TARGET_EXE_DIR}"
cp -R "${SOURCE_DIR}/etc/script/mac/dmg/qt_menu.nib" "${TARGET_APP_DIR}/Contents/Resources"

echo Copying data dir
cp -R "${SOURCE_DIR}/data" "${TARGET_EXE_DIR}/"

function add-binary() {
  BINARY=$1
  echo "Adding binary: ${BINARY}"
  BINARY_PATH="${BUILD_DIR}/${BINARY}"
  if [ ! -f "${BINARY_PATH}" ]; then
    echo "Error: binary file is not found: ${BINARY}"
    exit 1
  fi

  cp -f "${BINARY_PATH}" "${TARGET_EXE_DIR}"
}

function add-plugin() {
  plugin=$1
  echo "Adding plugin: ${plugin}"
  PLUGIN_LIB="lib${plugin}.dylib"
  PLUGIN_DESC="${plugin}.plugin"
  PLUGIN_LICENSE="${plugin}.license"

  if [ ! -f "${BUILD_DIR}"/plugins/"${PLUGIN_LIB}" ]; then
    echo "Plugin library file is not found: ${PLUGIN_LIB} !"
    exit 1
  fi

  if [ ! -f "${BUILD_DIR}"/plugins/"${PLUGIN_DESC}" ]; then
    echo "Plugin descriptor file is not found: ${PLUGIN_DESC} !"
    exit 1
  fi

  if [ ! -f "${BUILD_DIR}"/plugins/"${PLUGIN_LICENSE}" ]; then
    echo "Plugin license file is not found: ${PLUGIN_LICENSE} !"
    exit 1
  fi

  cp "${BUILD_DIR}/plugins/${PLUGIN_LIB}" "${TARGET_PLUGINS_DIR}/"
  cp "${BUILD_DIR}/plugins/${PLUGIN_DESC}" "${TARGET_PLUGINS_DIR}/"
  cp "${BUILD_DIR}/plugins/${PLUGIN_LICENSE}" "${TARGET_PLUGINS_DIR}/"
}

function add-library() {
  lib=$1
  echo "Adding library: ${lib}"
  LIB_FILE="lib${lib}.1.dylib"
  if [ ! -f "${BUILD_DIR}"/"${LIB_FILE}" ]; then
    echo "Library file is not found: ${LIB_FILE} !"
    exit 1
  fi
  cp "${BUILD_DIR}/${LIB_FILE}" "${TARGET_EXE_DIR}/"
}

echo Copying UGENE binaries
add-binary ugeneui
add-binary ugenem
add-binary ugenecl
add-binary plugins_checker
cp "${SOURCE_DIR}/etc/script/mac/dmg/ugene" "${TARGET_EXE_DIR}"

echo Copying core libs
add-library QSpec
add-library U2Algorithm
add-library U2Core
add-library U2Designer
add-library U2Formats
add-library U2Gui
add-library U2Lang
add-library U2Private
add-library U2Script
add-library U2Test
add-library U2View
add-library breakpad
add-library ugenedb

# Plugins to add to the bundle
echo Copying plugins
PLUGIN_LIST="annotator \
             api_tests \
             CoreTests \
             GUITestBase \
             ball \
             biostruct3d_view \
             chroma_view \
             circular_view \
             dbi_bam \
             dna_export \
             dna_flexibility \
             dna_graphpack \
             dna_stat \
             dotplot \
             enzymes \
             external_tool_support \
             genome_aligner \
             gor4 \
             hmm2 \
             kalign \
             linkdata_support \
             opencl_support \
             orf_marker \
             pcr \
             pcr_primer_design_for_dna_assembly \
             perf_monitor \
             phylip \
             primer3 \
             psipred \
             ptools \
             query_designer \
             remote_blast \
             repeat_finder \
             sitecon \
             smith_waterman \
             test_runner \
             umuscle \
             variants \
             weight_matrix \
             workflow_designer"

for PLUGIN in ${PLUGIN_LIST}; do
  add-plugin "${PLUGIN}"
done

echo Running macdeployqt
"${QT_DIR}/bin/macdeployqt" "${TARGET_APP_DIR}" -no-strip \
  -executable="${TARGET_EXE_DIR}/ugeneui" \
  -executable="${TARGET_EXE_DIR}/ugenecl" \
  -executable="${TARGET_EXE_DIR}/ugenem" \
  -executable="${TARGET_EXE_DIR}/plugins_checker"

echo Copying extra libraries
cp "${QT_DIR}/extra_libs/"* "${TARGET_APP_DIR}/Contents/Frameworks"

echo Copying readme.txt file
cp "${SOURCE_DIR}/etc/script/mac/dmg/readme.txt" "${BUNDLE_DIR}/readme.txt"
echo Linking Samples
cd "${TARGET_APP_DIR}/.."
ln -s "./${APP_NAME}/Contents/MacOS/data/samples" ./Samples
cd "${TEAMCITY_WORK_DIR}"

echo "Compressing app into a tar.gz"
rm "${TEAMCITY_WORK_DIR}/"*.gz
tar cfz "bundle-mac-b${TEAMCITY_BUILD_COUNTER}.tar.gz" -C "${BUNDLE_DIR}" .
