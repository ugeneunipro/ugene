#!/bin/bash
# Creates a final UGENE DMG package for MacOS  from the pre-built bundle.
# - Takes a pre-built version of UGENE (See build.sh + bundle.sh)
# - Adds external tools.
# - Removes test plugins and libs.
# - Checks that result can be run: calls "ugenecl --help".
# - Validates list of files in the final bundle.
# - Packs everything into dmg file.
# - Build symbols.tar.gz for all our libs/executables.

TEAMCITY_WORK_DIR=$(pwd)
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"
SCRIPTS_DIR="${SOURCE_DIR}/etc/script/mac"
APP_BUNDLE_DIR_NAME="ugene_app"
APP_BUNDLE_DIR="${TEAMCITY_WORK_DIR}/${APP_BUNDLE_DIR_NAME}"
APP_NAME="Unipro UGENE.app"
APP_DIR="${APP_BUNDLE_DIR}/${APP_NAME}"
APP_CONTENTS_DIR="${APP_DIR}/Contents"
APP_EXE_DIR="${APP_CONTENTS_DIR}/MacOS"
SYMBOLS_DIR_NAME=symbols
SYMBOLS_DIR="${TEAMCITY_WORK_DIR}/$SYMBOLS_DIR_NAME"
SYMBOLS_LOG="${TEAMCITY_WORK_DIR}/symbols.log"
ARCHITECTURE_FILE_SUFFIX=x86-64
SIGN_IDENTITY="Developer ID Application: Alteametasoft"

rm -rf "${SYMBOLS_DIR}"
rm -rf "${SYMBOLS_LOG}"
rm -rf *.tar.gz
rm -rf *.dmg

mkdir "${SYMBOLS_DIR}"

echo "##teamcity[blockOpened name='Get version']"
VERSION=$("${APP_EXE_DIR}/ugenecl" --version | grep 'version of UGENE' | sed -n "s/.*version of UGENE \([0-9\.A-Za-z-]*\).*/\1/p")
if [ -z "${VERSION}" ]; then
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to get version of UGENE']"
  exit 1
fi
echo "Version of UGENE: ${VERSION}"
echo "##teamcity[blockClosed name='Get version']"

echo "##teamcity[blockOpened name='Copy files']"
# Remove excluded files from UGENE.
rm -rf "${APP_EXE_DIR}/libQSpec.so"
rm -rf "${APP_EXE_DIR}/plugins/"*CoreTests*
rm -rf "${APP_EXE_DIR}/plugins/"*GUITestBase*
rm -rf "${APP_EXE_DIR}/plugins/"*api_tests*
rm -rf "${APP_EXE_DIR}/plugins/"*perf_monitor*
rm -rf "${APP_EXE_DIR}/plugins/"*test_runner*

# Copy UGENE files & tools into 'bundle' dir.
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}/tools" "${APP_EXE_DIR}" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}

# These tools can't be signed as is today and require some pre-processing.
rm -rf "${APP_EXE_DIR}/MacOS/tools/python2/include"
mv "${APP_EXE_DIR}/tools/python2/lib/python2.7" "${APP_CONTENTS_DIR}/Resources/"
cd "${APP_EXE_DIR}/tools/python2/lib"
ln -s "../../../../Resources/python2.7" .
cd "${TEAMCITY_WORK_DIR}"

rm -rf "${APP_EXE_DIR}/tools/cistrome/CEAS/CEAS_Package-1.0.2-py2.7.egg-info"
rm -rf ""${APP_EXE_DIR}/MacOS/tools/cistrome/MACS/MACS-1.4.2-py2.7.egg-info"

echo " ##teamcity[blockClosed name='Copy files']"

echo "##teamcity[blockOpened name='Validate bundle content']"
# Validate bundle content.
REFERENCE_BUNDLE_FILE="${SCRIPTS_DIR}/release-bundle.txt"
CURRENT_BUNDLE_FILE="${TEAMCITY_WORK_DIR}/release-bundle.txt"
find "${APP_BUNDLE_DIR}"/* | sed -e "s/.*${APP_BUNDLE_DIR_NAME}\///" | sed 's/^.*\/tools\/.*\/.*$//g' | grep "\S" | sort >"${CURRENT_BUNDLE_FILE}"
if cmp -s "${CURRENT_BUNDLE_FILE}" "${REFERENCE_BUNDLE_FILE}"; then
  echo 'Bundle content validated successfully.'
else
  echo "The file ${CURRENT_BUNDLE_FILE} is different from ${REFERENCE_BUNDLE_FILE}"
  diff "${REFERENCE_BUNDLE_FILE}" "${CURRENT_BUNDLE_FILE}"
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to validate release bundle content']"
  exit 1
fi
echo "##teamcity[blockClosed name='Validate bundle content']"

echo "##teamcity[blockOpened name='Dump symbols']"

function dump_symbols() {
  echo "Dumping symbols for ${1}"
  BASE_NAME=$(basename "${1}")
  SYMBOL_FILE="${SYMBOLS_DIR}/${BASE_NAME}.sym"

  "${SOURCE_DIR}/etc/script/mac/dump_syms_x86_64" "${1}" >"${SYMBOLS_DIR}/${BASE_NAME}.sym" 2>"${SYMBOLS_LOG}"

  FILE_HEAD=$(head -n 1 "${SYMBOL_FILE}")
  FILE_HASH=$(echo "${FILE_HEAD}" | awk '{ print $4 }')
  FILE_NAME=$(echo "${FILE_HEAD}" | awk '{ print $5 }')

  DEST_PATH="${SYMBOLS_DIR}/${FILE_NAME}/${FILE_HASH}"
  mkdir -p "${DEST_PATH}"
  mv "${SYMBOL_FILE}" "${DEST_PATH}/${FILE_NAME}.sym"
}

find "${APP_DIR}" | sed 's/.*\/tools\/.*$//g' | grep -e ugeneui -e ugenecl -e lib.*.dylib | while read -r BINARY_FILE; do
  dump_symbols "${BINARY_FILE}"
done

echo Compressing symbols...
tar cfz "${SYMBOLS_DIR_NAME}.tar.gz" "${SYMBOLS_DIR_NAME}"

echo "##teamcity[blockClosed name='Dump symbols']"

echo "##teamcity[blockOpened name='Sign bundle content']"
codesign --deep --verbose --sign "${SIGN_IDENTITY}" --timestamp --options runtime --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${APP_EXE_DIR}/ugeneui" || exit 1
echo "##teamcity[blockClosed name='Sign bundle content']"

echo "##teamcity[blockOpened name='Pack DMG']"
echo pkg-dmg running...
RELEASE_FILE_NAME=ugene-"${VERSION}-r${TEAMCITY_RELEASE_BUILD_COUNTER}-b${TEAMCITY_UGENE_BUILD_COUNTER}-mac-${ARCHITECTURE_FILE_SUFFIX}.dmg"
"${SOURCE_DIR}/etc/script/mac/pkg-dmg" --source "${APP_BUNDLE_DIR_NAME}" \
  --target "${RELEASE_FILE_NAME}" \
  --volname "Unipro UGENE ${VERSION}" --symlink /Applications || exit 1

echo "##teamcity[blockClosed name='Pack DMG']"

echo "##teamcity[blockOpened name='Sign DMG']"
"${SOURCE_DIR}/etc/script/mac/codesign.sh" "${RELEASE_FILE_NAME}" || exit 1
codesign --sign "${SIGN_IDENTITY}" --timestamp --force "${RELEASE_FILE_NAME}" || exit 1
echo "##teamcity[blockClosed name='Sign DMG']"

echo "##teamcity[blockOpened name='Notarize DMG']"
bash "${SOURCE_DIR}/etc/script/mac/notarize.sh" -n "${RELEASE_FILE_NAME}" || exit 1
echo "##teamcity[blockClosed name='Notarize DMG']"
