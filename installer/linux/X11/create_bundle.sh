#!/bin/bash
# The scripts current dir is ${git-root}/installer/linux/X11

if [ -z "$QT_DIR" ]; then
  echo QT_DIR environment variable is not set!
  exit 1
fi

PRODUCT_NAME="ugene"

VERSION_MAJOR=$(cat ../../../src/ugene_version.pri | grep 'UGENE_VER_MAJOR=' | awk -F'=' '{print $2}')
VERSION_MINOR=$(cat ../../../src/ugene_version.pri | grep 'UGENE_VER_MINOR=' | awk -F'=' '{print $2}')
VERSION=$(cat ../../../src/ugene_version.pri | grep UGENE_VERSION | awk -F'=' '{print $2}' |
  sed -e 's/$${UGENE_VER_MAJOR}/'"$VERSION_MAJOR"'/g' \
    -e 's/$${UGENE_VER_MINOR}/'"$VERSION_MINOR"'/g')

RELEASE_DIR=../../../src/_release
SYMBOLS_DIR=symbols
DUMP_SYMBOLS_LOG=dump_symbols_log.txt
TARGET_APP_DIR="${PRODUCT_NAME}-${VERSION}"
PACKAGE_TYPE="linux"
ARCH=$(uname -m)

source create_bundle_common.sh

echo cleaning previous bundle
rm -rf "${TARGET_APP_DIR}"
rm -rf "${SYMBOLS_DIR}"
rm -f "${DUMP_SYMBOLS_LOG}"
# shellcheck disable=SC2035
rm -rf *.tar.gz
mkdir "${TARGET_APP_DIR}"
mkdir "${SYMBOLS_DIR}"

echo
echo copying ugenecl
add-binary ugenecl

echo
echo copying ugeneui
add-binary ugeneui

echo
echo copying ugenem
add-binary ugenem

echo
echo copying plugins_checker
add-binary plugins_checker

echo
echo copying ugene startup script
cp -v $RELEASE_DIR/ugene "$TARGET_APP_DIR"

echo
echo copying man page for UGENE
mkdir -v "$TARGET_APP_DIR/man1"
cp -v ../../_common_data/ugene.1.gz "$TARGET_APP_DIR/man1"

echo
echo copying README file
cp -v ../../_common_data/README "$TARGET_APP_DIR"

echo
echo copying LICENSE file
cp -v ../../_common_data/LICENSE "$TARGET_APP_DIR"
echo copying LICENSE.3rd_party file
cp -v ../../_common_data/LICENSE.3rd_party "$TARGET_APP_DIR"

echo
echo copying file association script files
cp -v ../../_common_data/Associate_files_to_UGENE.sh "$TARGET_APP_DIR"
cp -v ../../_common_data/icons.tar.gz "$TARGET_APP_DIR"
cp -v ../../_common_data/application-x-ugene.xml "$TARGET_APP_DIR"
cp -v ../../_common_data/ugene.desktop "$TARGET_APP_DIR"
cp -v ../../_common_data/ugene.png "$TARGET_APP_DIR"

echo
echo copying translations
cp -v $RELEASE_DIR/transl_en.qm "$TARGET_APP_DIR"
cp -v $RELEASE_DIR/transl_ru.qm "$TARGET_APP_DIR"

echo
echo copying data dir
cp -r "../../../data" "${TARGET_APP_DIR}"

# Include external tools.
if [ -e "$RELEASE_DIR/../../tools" ]; then
  echo
  echo copying tools dir
  cp -R "$RELEASE_DIR/../../tools" "${TARGET_APP_DIR}/"
  PACKAGE_TYPE="linux-full"
fi

echo
echo copying core shared libs
add-core-library U2Algorithm
add-core-library U2Core
add-core-library U2Designer
add-core-library U2Formats
add-core-library U2Gui
add-core-library U2Lang
add-core-library U2Private
add-core-library U2Test
add-core-library U2View
add-core-library ugenedb
add-core-library breakpad

echo
echo copying qt libraries
add-qt-library Qt5Core
add-qt-library Qt5DBus
add-qt-library Qt5Gui
add-qt-library Qt5Network
add-qt-library Qt5OpenGL
add-qt-library Qt5PrintSupport
add-qt-library Qt5Script
add-qt-library Qt5ScriptTools
add-qt-library Qt5Sql
add-qt-library Qt5Svg
add-qt-library Qt5Test
add-qt-library Qt5Widgets
add-qt-library Qt5XcbQpa
add-qt-library Qt5Xml
if [ "$1" == "-test" ]; then
  add-qt-library Qt5Test
fi

if [ ! -z "$PATH_TO_INCLUDE_LIBS" ]; then
  echo
  echo copying include libs from "$PATH_TO_INCLUDE_LIBS"
  cp -v "$PATH_TO_INCLUDE_LIBS"/* "${TARGET_APP_DIR}"
fi

mkdir "${TARGET_APP_DIR}/sqldrivers"
cp -v "${QT_DIR}/plugins/sqldrivers/libqsqlmysql.so" "${TARGET_APP_DIR}/sqldrivers"
strip -v "${TARGET_APP_DIR}/sqldrivers/libqsqlmysql.so"
# Make 'sql' libs search for qt libs in 1 folder up.
chrpath -r '$ORIGIN/..' "${TARGET_APP_DIR}/sqldrivers/libqsqlmysql.so"

cp -r -v "${QT_DIR}/plugins/platforms" "${TARGET_APP_DIR}"
strip -v "${TARGET_APP_DIR}/platforms"/*.so
# Make 'platform' libs search for qt libs in 1 folder up.
chrpath -r '$ORIGIN/..' "${TARGET_APP_DIR}/platforms"/*.so

cp -r -v "${QT_DIR}/plugins/imageformats" "${TARGET_APP_DIR}"
strip -v "${TARGET_APP_DIR}"/imageformats/*.so

PATH_TO_ICU_DATA_LIB=$(ldd "${QT_DIR}/lib/libQt5Widgets.so.5" | grep libicudata.so | cut -d " " -f3)
cp -v -L "$PATH_TO_ICU_DATA_LIB" "${TARGET_APP_DIR}"
PATH_TO_ICU_I18N_LIB=$(ldd "${QT_DIR}/lib/libQt5Widgets.so.5" | grep libicui18n.so | cut -d " " -f3)
cp -v -L "$PATH_TO_ICU_I18N_LIB" "${TARGET_APP_DIR}"
PATH_TO_ICU_UUC_LIB=$(ldd "${QT_DIR}/lib/libQt5Widgets.so.5" | grep libicuuc.so | cut -d " " -f3)
cp -v -L "$PATH_TO_ICU_UUC_LIB" "${TARGET_APP_DIR}"

echo
echo copying plugins
mkdir "${TARGET_APP_DIR}/plugins"
add-plugin annotator
add-plugin ball
add-plugin biostruct3d_view
add-plugin chroma_view
add-plugin circular_view
add-plugin clark_support
add-plugin dbi_bam
add-plugin diamond_support
add-plugin dna_export
add-plugin dna_flexibility
add-plugin dna_graphpack
add-plugin dna_stat
add-plugin dotplot
add-plugin enzymes
add-plugin external_tool_support
add-plugin genome_aligner
add-plugin gor4
add-plugin hmm2
add-plugin kalign
add-plugin kraken_support
add-plugin linkdata_support
add-plugin metaphlan2_support
add-plugin ngs_reads_classification
add-plugin orf_marker
add-plugin opencl_support
add-plugin pcr
add-plugin phylip
add-plugin primer3
add-plugin psipred
add-plugin ptools
add-plugin query_designer
add-plugin remote_blast
add-plugin repeat_finder
add-plugin sitecon
add-plugin smith_waterman
add-plugin umuscle
add-plugin variants
add-plugin weight_matrix
add-plugin wevote_support
add-plugin workflow_designer
if [ "$1" == "-test" ]; then
  add-plugin test_runner
fi

# Remove svn dirs
find "$TARGET_APP_DIR" -name ".svn" | xargs rm -rf

if [ -z "$REVISION" ]; then
  REVISION=$(git rev-parse --short HEAD)
fi

if [ "$1" == "-test" ]; then
  TEST="-test"
fi

# shellcheck disable=SC2027
PACKAGE_NAME=$PRODUCT_NAME"-"$VERSION"-$PACKAGE_TYPE-"$ARCH"-r"$REVISION$TEST

tar -czf ${SYMBOLS_DIR}.tar.gz "${SYMBOLS_DIR}"/
tar -czf "${PACKAGE_NAME}".tar.gz "${TARGET_APP_DIR}"/
