#!/bin/bash
# Creates final UGENE portable package for Linux from the pre-built sources.
# - Takes a pre-built version of UGENE (See build.sh)
# - Adds external tools.
# - Removes test plugins and libs.
# - Checks that result can be run: calls "ugenecl --help".
# - Validates list of files in the final bundle.
# - Packs everything into tar.gz file.
# - Build symbols.tar.gz for all our libs/executables.

TEAMCITY_WORK_DIR=$(pwd)
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"
SCRIPTS_DIR="${SOURCE_DIR}/etc/script/linux"
APP_BUNDLE_DIR_NAME=ugene_app
APP_BUNDLE_DIR="${TEAMCITY_WORK_DIR}/${APP_BUNDLE_DIR_NAME}"
SYMBOLS_DIR_NAME=symbols
SYMBOLS_DIR="${TEAMCITY_WORK_DIR}/${SYMBOLS_DIR_NAME}"
SYMBOLS_LOG="${TEAMCITY_WORK_DIR}/symbols.log"

rm -rf "${SYMBOLS_DIR}"
rm -rf "${SYMBOLS_LOG}"
rm -rf *.tar.gz

mkdir "${SYMBOLS_DIR}"

echo "##teamcity[blockOpened name='Copy files']"

# Remove excluded files from UGENE.
rm -rf "${APP_BUNDLE_DIR}/libQSpec.so"
rm -rf "${APP_BUNDLE_DIR}/plugins/"*CoreTests*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*GUITestBase*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*api_tests*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*perf_monitor*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*test_runner*

# Deprecated plugins.
rm -rf "${APP_BUNDLE_DIR}/plugins/"*clark*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*diamond*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*kraken*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*metaphlan2*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*ngs_reads_classification*
rm -rf "${APP_BUNDLE_DIR}/plugins/"*wevote*

# Deprecated workflows
rm -rf "${APP_BUNDLE_DIR}/data/workflow_samples/NGS/ngs_classification"*

# Copy UGENE files & tools into 'app' dir.
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}/tools" "${APP_BUNDLE_DIR}" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}

# Deprecated tools.
rm -rf "${APP_BUNDLE_DIR}/tools/clark"
rm -rf "${APP_BUNDLE_DIR}/tools/diamond"
rm -rf "${APP_BUNDLE_DIR}/tools/kraken"
rm -rf "${APP_BUNDLE_DIR}/tools/metaphlan2"
rm -rf "${APP_BUNDLE_DIR}/tools/wevote"

echo "##teamcity[blockClosed name='Copy files']"

echo "##teamcity[blockOpened name='Get version']"
VERSION=$("${APP_BUNDLE_DIR}/ugenecl" --version | grep 'version of UGENE' | sed -n "s/.*version of UGENE \([0-9.A-Za-z-]\+\).*/\1/p")
if [ -z "${VERSION}" ]; then
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to get version of UGENE']"
  exit 1
fi
echo "Version of UGENE is ${VERSION}"
echo "##teamcity[blockClosed name='Get version']"

echo "##teamcity[blockOpened name='Validate bundle content']"

# Validate bundle content.
REFERENCE_BUNDLE_FILE="${SCRIPTS_DIR}/release-bundle.txt"
CURRENT_BUNDLE_FILE="${TEAMCITY_WORK_DIR}/release-bundle.txt"
find "${APP_BUNDLE_DIR}"/* | sed -e "s/.*${APP_BUNDLE_DIR_NAME}\///" | sed 's/^tools\/.*\/.*$//g' | grep "\S" | sort >"${CURRENT_BUNDLE_FILE}"
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
  echo "Dumping symbols for $1"
  BASE_NAME=$(basename "${1}")
  SYMBOL_FILE="${SYMBOLS_DIR}/${BASE_NAME}.sym"

  "$SOURCE_DIR"/etc/script/linux/dump_syms "$1" >"${SYMBOLS_DIR}/${BASE_NAME}.sym" 2>"${SYMBOLS_LOG}"

  FILE_HEAD=$(head -n 1 "${SYMBOL_FILE}")
  FILE_HASH=$(echo "${FILE_HEAD}" | awk '{ print $4 }')
  FILE_NAME=$(echo "${FILE_HEAD}" | awk '{ print $5 }')

  DEST_PATH="${SYMBOLS_DIR}/${FILE_NAME}/${FILE_HASH}"
  mkdir -p "${DEST_PATH}"
  mv "${SYMBOL_FILE}" "${DEST_PATH}/${FILE_NAME}.sym"
}

find "${APP_BUNDLE_DIR}" | sed 's/.*\/tools\/.*$//g' | grep -e ugeneui -e ugenecl -e lib.*.so.* | while read -r BINARY_FILE; do
  dump_symbols "${BINARY_FILE}"
done
echo "##teamcity[blockClosed name='Dump symbols']"

echo "##teamcity[blockOpened name='Build archive']"

RELEASE_BASE_FILE_NAME="ugene-${VERSION}-r${TEAMCITY_RELEASE_BUILD_COUNTER}-b${TEAMCITY_UGENE_BUILD_COUNTER}-linux-x86-64"
RELEASE_UNPACKED_DIR_NAME="ugene-${VERSION}"

rm -rf "ugene-"*
mv "${APP_BUNDLE_DIR}" "${RELEASE_UNPACKED_DIR_NAME}"
tar cfz "${RELEASE_BASE_FILE_NAME}.tar.gz" "${RELEASE_UNPACKED_DIR_NAME}"

echo Compressing symbols...
tar cfz "${SYMBOLS_DIR_NAME}-r${TEAMCITY_RELEASE_BUILD_COUNTER}-linux-x86-64.tar.gz" "${SYMBOLS_DIR_NAME}"

echo "##teamcity[blockClosed name='Build archive']"
