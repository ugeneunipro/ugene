#!/usr/bin/bash
PRODUCT_NAME="ugeneuid"

if [ -z ${SOURCE_DIR} ]; then SOURCE_DIR=../..; fi

BUILD_DIR=./debug_bundle
DEBUG_DIR=${SOURCE_DIR}/src/_debug
TARGET_APP_DIR="$BUILD_DIR/${PRODUCT_NAME}.app"
TARGET_EXE_DIR="${TARGET_APP_DIR}/Contents/MacOS"

source bundle_common_debug.sh

echo cleaning previous bundle
rm -rf ${BUILD_DIR}
rm -rf ~/.config/Unipro/UGENE*
mkdir $BUILD_DIR


echo
echo copying UGENE bundle 
cp -R $DEBUG_DIR/${PRODUCT_NAME}.app/ "$TARGET_APP_DIR"

mkdir "${TARGET_EXE_DIR}/../Frameworks"
mkdir "${TARGET_EXE_DIR}/plugins"

echo copying translations
cp $DEBUG_DIR/transl_*.qm "$TARGET_EXE_DIR"
cp -R ./qt_menu.nib "${TARGET_EXE_DIR}/../Resources"
find "${TARGET_EXE_DIR}/../Resources/qt_menu.nib" -name ".svn" | xargs rm -rf

echo copying data dir
if [ "$1" == "-test" ]
    then
    ln -s "../../../../../../data" "${TARGET_EXE_DIR}/data"
    ln -s "../../../../../../ext_tools_mac_64-bit" "${TARGET_EXE_DIR}/tools"
else
    mkdir "${TARGET_EXE_DIR}/data"
    cp -R "$DEBUG_DIR/../../data" "${TARGET_EXE_DIR}/"
    find $TARGET_EXE_DIR -name ".svn" | xargs rm -rf
fi

if [ -e "../../cistrome" ]; then
    ln -s "../../../../../../../cistrome" "${TARGET_EXE_DIR}/data/cistrome"
fi

if [ -e "../../tools" ]; then
    ln -s "../../../../../../tools" "${TARGET_EXE_DIR}/tools"
fi

echo copying ugenem
cp "$DEBUG_DIR/ugenem.app/Contents/MacOS/ugenem" "$TARGET_EXE_DIR"

echo copying console binary
cp "$DEBUG_DIR/ugenecld.app/Contents/MacOS/ugenecld" "$TARGET_EXE_DIR"

echo copying plugin checker binary
cp "$DEBUG_DIR/plugins_checkerd" "$TARGET_EXE_DIR"

echo Copying core shared libs

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
add-library ugenedb
add-library breakpad
if [ "$1" == "-test" ]
   then
      add-library QSpec
fi

#install_name_tool -change @executable_path/../Frameworks/Breakpad.framework/Versions/A/Breakpad @executable_path/../../../../includes/breakpad/Breakpad.framework/Versions/A/BreakPad ${TARGET_EXE_DIR}/libU2Privated.1.dylib

echo Copying plugins

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
add-plugin opencl_support
add-plugin orf_marker
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

if [ "$1" == "-test" ]
   then
      add-plugin api_tests
      add-plugin CoreTests
      add-plugin perf_monitor
      add-plugin test_runner
      add-plugin GUITestBase
fi
