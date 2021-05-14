_PATH_SAVED_=$PATH

if [ -f "$UGENE_QMAKE" ]; then
    UGENE_QMAKE_PATH=$(dirname "$UGENE_QMAKE")
    UGENE_QMAKE_PATH=$(cd "$UGENE_QMAKE_PATH"; pwd)
    echo "Add ${UGENE_QMAKE_PATH} to PATH env var"
    export PATH=${UGENE_QMAKE_PATH}:$PATH
fi

PRODUCT_NAME="ugeneui"
PRODUCT_DISPLAY_NAME="Unipro UGENE"

if [ -z "${SOURCE_DIR}" ]; then SOURCE_DIR=../..; fi

echo Source: $SOURCE_DIR

VERSION_MAJOR=`cat ${SOURCE_DIR}/src/ugene_version.pri | grep 'UGENE_VER_MAJOR=' | awk -F'=' '{print $2}'`
VERSION_MINOR=`cat ${SOURCE_DIR}/src/ugene_version.pri | grep 'UGENE_VER_MINOR=' | awk -F'=' '{print $2}'`
VERSION_SUFFIX=`cat ${SOURCE_DIR}/src/ugene_version.pri | grep 'UGENE_VER_SUFFIX=' | awk -F'=' '{print $2}'`
UGENE_VERSION=`cat ${SOURCE_DIR}/src/ugene_version.pri | grep UGENE_VERSION | awk -F'=' '{print $2}' | \
               sed -e 's/$${UGENE_VER_MAJOR}/'"$VERSION_MAJOR"'/g' \
                   -e 's/$${UGENE_VER_MINOR}/'"$VERSION_MINOR"'/g' \
                   -e 's/$${UGENE_VER_SUFFIX}/'"$VERSION_SUFFIX"'/g'`

#ARCHITECTURE=`uname -m`
ARCHITECTURE=x86_64
BUILD_DIR=./release_bundle
RELEASE_DIR=${SOURCE_DIR}/src/_release
TARGET_APP_DIR="$BUILD_DIR/${PRODUCT_NAME}.app/"
TARGET_APP_DIR_RENAMED="$BUILD_DIR/${PRODUCT_DISPLAY_NAME}.app/"
TARGET_EXE_DIR="${TARGET_APP_DIR}/Contents/MacOS"
TARGET_RESOURCES_DIR="${TARGET_APP_DIR}/Contents/Resources"
TARGET_DATA_DIR="${TARGET_APP_DIR}/Contents/Resources/data"
TARGET_PLUGINS_DIR="${TARGET_APP_DIR}/Contents/Resources/plugins"
TARGET_TOOLS_DIR="${TARGET_APP_DIR}/Contents/Resources/tools"
SYMBOLS_DIR=symbols


source bundle_common.sh

echo cleaning previous bundle
rm -rf ${BUILD_DIR}
rm -rf ~/.config/Unipro/UGENE*
mkdir $BUILD_DIR

echo Preparing debug symbols location
rm -rf ${SYMBOLS_DIR}
rm -f "${SYMBOLS_DIR}.tar.gz"
mkdir "${SYMBOLS_DIR}"

echo
echo creating UGENE bundle

mkdir "${TARGET_APP_DIR}"
mkdir "${TARGET_APP_DIR}/Contents"
mkdir "${TARGET_APP_DIR}/Contents/Frameworks"
mkdir "${TARGET_APP_DIR}/Contents/MacOS"
mkdir "${TARGET_RESOURCES_DIR}"
mkdir "${TARGET_RESOURCES_DIR}/plugins"

echo copying icons
cp ${SOURCE_DIR}/src/ugeneui/images/ugene-doc.icns "$TARGET_APP_DIR/Contents/Resources"
cp ${SOURCE_DIR}/src/ugeneui/images/ugeneui.icns "$TARGET_APP_DIR/Contents/Resources"
cp ${SOURCE_DIR}/installer/macosx/Info.plist "$TARGET_APP_DIR/Contents"

echo copying translations
cp $RELEASE_DIR/transl_*.qm "${TARGET_RESOURCES_DIR}/"
cp -R ./qt_menu.nib "${TARGET_RESOURCES_DIR}/"
find "${TARGET_RESOURCES_DIR}/qt_menu.nib" -name ".svn" | xargs rm -rf

##############################################
# codesign don't like dot '.' in dir names
# MacOS/data must be link to ../Resources/data
echo copying data dir
cp -R "$RELEASE_DIR/../../data" "${TARGET_RESOURCES_DIR}/"
find "${TARGET_RESOURCES_DIR}" -name ".svn" | xargs rm -rf

##############################################
# codesign don't like dot '.' in dir names
# MacOS/tools must be link to ../Resources/tools
echo copying tools dir
#include external tools package if applicable
if [ -e "$RELEASE_DIR/../../tools" ]; then
    cp -R "$RELEASE_DIR/../../tools" "${TARGET_RESOURCES_DIR}/"
    find "${TARGET_RESOURCES_DIR}" -name ".svn" | xargs rm -rf
fi

echo Copying UGENE binaries
add-binary ugeneui
add-binary ugenem
add-binary ugenecl
add-binary plugins_checker
#cp ./ugene "$TARGET_EXE_DIR"

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

echo Copying plugins

# plugins to copy to the bundle
# to ignore plugin remove it
PLUGIN_LIST="annotator \
            ball \
            biostruct3d_view \
            chroma_view \
            circular_view \
            clark_support \
            dbi_bam \
            diamond_support \
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
            kraken_support \
            linkdata_support \
            metaphlan2_support \
            ngs_reads_classification \
            opencl_support \
            orf_marker \
            pcr \
            phylip \
            primer3 \
            psipred \
            ptools \
            query_designer \
            remote_blast \
            repeat_finder \
            sitecon \
            smith_waterman \
            umuscle \
            variants \
            weight_matrix \
            wevote_support \
            workflow_designer"

if [ "$1" == "-test" ]
   then
   PLUGIN_LIST="$PLUGIN_LIST CoreTests \
                             GUITestBase \
                             perf_monitor \
                             test_runner \
                             api_tests"
fi

##############################################
# codesign don't like dot '.' in dir names
# MacOS/plugins must be link to ../Resources/plugins
mkdir -p "${TARGET_PLUGINS_DIR}"
for PLUGIN in $PLUGIN_LIST
do
    add-plugin $PLUGIN
done

echo
echo macdeployqt running...
echo "pwd="$(pwd)
echo "which macdeployqt="$(which macdeployqt)
echo macdeployqt "$TARGET_APP_DIR" -no-strip -executable="$TARGET_EXE_DIR"/ugeneui -executable="$TARGET_EXE_DIR"/ugenecl -executable="$TARGET_EXE_DIR"/ugenem -executable="$TARGET_EXE_DIR"/plugins_checker
macdeployqt "$TARGET_APP_DIR" -no-strip -executable="$TARGET_EXE_DIR"/ugeneui -executable="$TARGET_EXE_DIR"/ugenecl -executable="$TARGET_EXE_DIR"/ugenem -executable="$TARGET_EXE_DIR"/plugins_checker

echo mv "$TARGET_APP_DIR" "$TARGET_APP_DIR_RENAMED"
mv "$TARGET_APP_DIR" "$TARGET_APP_DIR_RENAMED"

cd  $BUILD_DIR
ln -s ./Unipro\ UGENE.app/Contents/MacOS/data/samples ./Samples
cd ..

echo copy readme.txt file
cp ./readme.txt $BUILD_DIR/readme.txt

if [ ! "$1" ]; then
    set -x

    echo "##teamcity[blockOpened name='Bundle code signing']"
    echo
    echo Code signing...
    echo pwd=`pwd`
    echo ./codesign_clear-i386.sh "${TARGET_APP_DIR_RENAMED}"
    bash ./codesign_clear-i386.sh "${TARGET_APP_DIR_RENAMED}"
    echo ./codesign.mac.sh "${TARGET_APP_DIR_RENAMED}"
    bash ./codesign.mac.sh "${TARGET_APP_DIR_RENAMED}"
    echo "##teamcity[blockClosed name='Bundle code signing']"
    
    echo
    echo Create pkg file
    if [ -f "${TARGET_APP_DIR_RENAMED}/Contents/MacOS/ugeneui" ]; then
        archx=`file ${TARGET_APP_DIR_RENAMED}/Contents/MacOS/ugeneui | grep x86_64`
        if [ ! -z "$archx" ]; then
            ARCHITECTURE=x86_64
        fi
    fi
    if [ -z "${BUILD_VCS_NUMBER_new_trunk}" ]; then
        BUILD_VCS_NUMBER_new_trunk=`git rev-parse --short HEAD`
    fi
    bash ./productbuild.sh \
        "${TARGET_APP_DIR_RENAMED}" \
        "ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}".not-signed.pkg \
        "ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}".pkg

    echo
    echo Compressing symbols...
    echo tar czf "${SYMBOLS_DIR}.tar.gz" "${SYMBOLS_DIR}"
    tar czf "${SYMBOLS_DIR}.tar.gz" "${SYMBOLS_DIR}"

    #echo
    #echo pkg-dmg running...
    #echo ./pkg-dmg --source $BUILD_DIR --target ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}.dmg --license ./LICENSE.with_3rd_party --volname "Unipro UGENE $UGENE_VERSION" --symlink /Applications
    #./pkg-dmg --source $BUILD_DIR --target ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}.dmg --license ./LICENSE.with_3rd_party --volname "Unipro UGENE $UGENE_VERSION" --symlink /Applications
    
    echo
    echo Create dmg-file
    echo ./create-dmg.sh \
        ugene-app.dmg-dir \
        "${UGENE_VERSION}" \
        "${TARGET_APP_DIR_RENAMED}" \
        "ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}".pkg \
        "ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}".dmg
    bash ./create-dmg.sh \
        ugene-app.dmg-dir \
        "${UGENE_VERSION}" \
        "${TARGET_APP_DIR_RENAMED}" \
        "ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}".pkg \
        "ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}".dmg

    echo
    echo Signing dmg-file...
    echo ./codesign.mac.sh ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}.dmg
    bash ./codesign.mac.sh ugene-${UGENE_VERSION}-mac-${ARCHITECTURE}-r${BUILD_VCS_NUMBER_new_trunk}.dmg

    set +x
fi

echo "Restore PATH env var"
export PATH=${_PATH_SAVED_}
