#!/bin/bash
# Creates a final UGENE portable package for Windows from the pre-built sources.
# - Takes a pre-built version of UGENE (See build.sh)
# - Adds external tools.
# - Removes test plugins and libs.
# - Checks that result can be run: calls "ugenecl --help".
# - Validates list of files in the final bundle.
# - Packs everything into zip file.
# - Build symbols.tar.gz for all our libs/executables.

TEAMCITY_WORK_DIR=$(pwd)
UGENE_DIR="${TEAMCITY_WORK_DIR}/ugene"
SCRIPTS_DIR="${UGENE_DIR}/etc/script/windows"
DIST_DIR_NAME=bundle
DIST_DIR="${TEAMCITY_WORK_DIR}/${DIST_DIR_NAME}"
SYMBOLS_DIR_NAME=symbols
SYMBOLS_DIR="${TEAMCITY_WORK_DIR}/${SYMBOLS_DIR_NAME}"
SYMBOLS_LOG="${TEAMCITY_WORK_DIR}/symbols.log"

rm -rf "${SYMBOLS_DIR}"
rm -rf "${SYMBOLS_LOG}"
rm -rf ./*.zip
rm -rf ./*.tar.gz

mkdir "${SYMBOLS_DIR}"

echo "##teamcity[blockOpened name='Copy files']"

# Remove excluded files from UGENE.
rm -rf "${DIST_DIR}/"*QSpec*
rm -rf "${DIST_DIR}/"*Qt5Test*
rm -rf "${DIST_DIR}/plugins/"*CoreTests*
rm -rf "${DIST_DIR}/plugins/"*GUITestBase*
rm -rf "${DIST_DIR}/plugins/"*api_tests*
rm -rf "${DIST_DIR}/plugins/"*perf_monitor*
rm -rf "${DIST_DIR}/plugins/"*test_runner*

# Copy UGENE files & tools into 'app' dir.
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}/tools" "${DIST_DIR}" || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}

echo "##teamcity[blockClosed name='Copy files']"

echo "##teamcity[blockOpened name='Get version']"
VERSION=$("${DIST_DIR}/ugenecl.exe" --version | grep 'version of UGENE' | sed -n "s/.*version of UGENE \([0-9.A-Za-z-]\+\).*/\1/p")
if [ -z "${VERSION}" ]; then
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to get version of UGENE']"
  exit 1
fi
echo "Version of UGENE is ${VERSION}"
echo "##teamcity[blockClosed name='Get version']"

echo "##teamcity[blockOpened name='Dump symbols']"

function dump_symbols() {
  echo "Dumping symbols for ${1}"
  BASE_NAME=$(basename "${1}")
  SYMBOL_FILE="${SYMBOLS_DIR}/${BASE_NAME}.sym"

  dump_syms.exe "$1" >"${SYMBOLS_DIR}/${BASE_NAME}.sym" 2>"${SYMBOLS_LOG}"

  FILE_HEAD=$(head -n 1 "${SYMBOL_FILE}")
  FILE_HASH=$(echo "${FILE_HEAD}" | awk '{ print $4 }')
  FILE_NAME=$(echo "${FILE_HEAD}" | awk '{ print $5 }' | tr -d "\r" | sed -e 's/\.pdb//g')

  DEST_PATH="${SYMBOLS_DIR}/${FILE_NAME}.pdb/${FILE_HASH}"
  mkdir -p "${DEST_PATH}"
  mv "${SYMBOL_FILE}" "${DEST_PATH}/${FILE_NAME}.sym"
}

find "${DIST_DIR_NAME}" | sed 's/.*\/tools\/.*$//g' | grep -e ugeneui.exe -e ugenecl.exe -e .dll$ | grep -v vcruntime | while read -r BINARY_FILE; do
  dump_symbols "${BINARY_FILE}"
done

# Remove pdb files used for symbol generation.
echo "Removing not needed PDB files."
rm "${DIST_DIR_NAME}/"*.pdb
rm "${DIST_DIR_NAME}/imageformats/"*.pdb
rm "${DIST_DIR_NAME}/platforms/"*.pdb
rm "${DIST_DIR_NAME}/styles/"*.pdb
rm "${DIST_DIR_NAME}/printsupport/"*.pdb

echo "##teamcity[blockClosed name='Dump symbols']"

# Validate bundle content.
echo "##teamcity[blockOpened name='Validate bundle content']"
REFERENCE_BUNDLE_FILE="${SCRIPTS_DIR}/release-bundle.txt"
CURRENT_BUNDLE_FILE="${TEAMCITY_WORK_DIR}/release-bundle.txt"
find "${DIST_DIR}"/* | sed -e "s/.*${DIST_DIR_NAME}\///" | sed 's/^tools\/.*\/.*$//g' | grep "\S" | sort >"${CURRENT_BUNDLE_FILE}"

if diff --strip-trailing-cr "${REFERENCE_BUNDLE_FILE}" "${CURRENT_BUNDLE_FILE}"; then
  echo "Bundle content validated successfully."
else
  echo "The file ${CURRENT_BUNDLE_FILE} is different from ${REFERENCE_BUNDLE_FILE}"
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to validate release bundle content']"
  exit 1
fi
echo "##teamcity[blockClosed name='Validate bundle content']"

echo "##teamcity[blockOpened name='Sign']"
function code_sign() {
  FILE_TO_SIGN=$1
  echo "Signing ${FILE_TO_SIGN}"
  if signtool.exe sign /a /tr http://timestamp.globalsign.com/tsa/r6advanced1 /td SHA256 /fd SHA256 /n "NTSIT UNIPRO LLC" /debug /v "${FILE_TO_SIGN}"; then
    echo "File is signed successfully: ${FILE_TO_SIGN}"
  else
    echo "Failed to sign ${FILE_TO_SIGN}"
    exit 1
  fi
}

find "${DIST_DIR_NAME}" | grep -e .exe$ -e .dll$ | grep -v vcruntime | while read -r BINARY_FILE; do
  code_sign "${BINARY_FILE}" || exit 1
done
echo "##teamcity[blockClosed name='Sign']"

echo "##teamcity[blockOpened name='Build archive']"

RELEASE_BASE_FILE_NAME="ugene-${VERSION}-p${TEAMCITY_BUILD_COUNTER}-win-x86-64"
RELEASE_UNPACKED_DIR_NAME="ugene-${VERSION}"

rm -rf "ugene-"*
mv "${DIST_DIR}" "${RELEASE_UNPACKED_DIR_NAME}"
7z a -r "${RELEASE_BASE_FILE_NAME}.zip" "${RELEASE_UNPACKED_DIR_NAME}/"*

echo Compressing symbols...
tar cfz "${SYMBOLS_DIR_NAME}-p${TEAMCITY_BUILD_COUNTER}-win-x86-64.tar.gz" "${SYMBOLS_DIR_NAME}"

echo "${VERSION}" >"${TEAMCITY_WORK_DIR}/version.txt"

echo "##teamcity[blockClosed name='Build archive']"
