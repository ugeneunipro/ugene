#!/bin/bash
# Creates final UGENE portable package for Linux from the pre-built sources.
# - Takes pre-built version of UGENE (See Build->Linux Teamcity target or linux/build.sh)
# - Adds external tools.
# - Checks that result can be run: calls "ugenecl --help"
# - Packs everything into tar.gz file.
# - Build symbols.tar.gz for all our libs/executable

TEAMCITY_WORK_DIR=$(pwd)
SOURCE_DIR="${TEAMCITY_WORK_DIR}/ugene_git"
SCRIPTS_DIR="${SOURCE_DIR}/etc/script/linux"
BUILD_DIR="${TEAMCITY_WORK_DIR}/ugene"
SYMBOLS_DIR_NAME=symbols
SYMBOLS_DIR="${TEAMCITY_WORK_DIR}/$SYMBOLS_DIR_NAME"
SYMBOLS_LOG="${TEAMCITY_WORK_DIR}/symbols.log"

rm -rf "${SYMBOLS_DIR}"
rm -rf "${SYMBOLS_LOG}"
rm -rf bundle
rm -rf *.tar.gz

mkdir "${SYMBOLS_DIR}"

echo "##teamcity[blockOpened name='Copy files']"
cp -r "${BUILD_DIR}" bundle || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy UGENE dir']"
}

# Remove excluded files from UGENE.
rm -rf bundle/libQSpec.so
rm -rf bundle/plugins/*CoreTests*
rm -rf bundle/plugins/*GUITestBase*
rm -rf bundle/plugins/*api_tests*
rm -rf bundle/plugins/*perf_monitor*

# Copy UGENE files & tools into 'bundle' dir.
rsync -a --exclude=.svn* "${TEAMCITY_WORK_DIR}"/tools bundle || {
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
}
echo "##teamcity[blockClosed name='Copy files']"

echo "##teamcity[blockOpened name='Validate bundle content']"
# Validate bundle content.
REFERENCE_BUNDLE_FILE="${SCRIPTS_DIR}"/bundle.txt
CURRENT_BUNDLE_FILE="${TEAMCITY_WORK_DIR}"/bundle.txt
find bundle/* | sed -e "s/^bundle\///" | sed 's/tools\/.*$//g' | grep "\S" | sort >"${CURRENT_BUNDLE_FILE}"
if cmp -s "${CURRENT_BUNDLE_FILE}" "${SCRIPTS_DIR}/bundle.txt"; then
  echo 'Bundle content validated successfully.'
else
  echo "The file ${CURRENT_BUNDLE_FILE} is different from ${REFERENCE_BUNDLE_FILE}"
  diff "${REFERENCE_BUNDLE_FILE}" "${CURRENT_BUNDLE_FILE}"
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to copy tools dir']"
  exit 1
fi
echo "##teamcity[blockClosed name='Validate bundle content']"

echo "##teamcity[blockOpened name='Dump symbols']"

function dump_symbols() {
  echo "Dumping symbols for $1"
  BASE_NAME=$(basename "${1}")
  SYMBOL_FILE="${SYMBOLS_DIR}/${BASE_NAME}.sym"

  "$SOURCE_DIR"/etc/bin/linux/dump_syms "$1" >"${SYMBOLS_DIR}/${BASE_NAME}.sym" 2>"${SYMBOLS_LOG}"

  FILE_HEAD=$(head -n 1 "${SYMBOL_FILE}")
  FILE_HASH=$(echo "${FILE_HEAD}" | awk '{ print $4 }')
  FILE_NAME=$(echo "${FILE_HEAD}" | awk '{ print $5 }')

  DEST_PATH="${SYMBOLS_DIR}/${FILE_NAME}/${FILE_HASH}"
  mkdir -p "${DEST_PATH}"
  mv "${SYMBOL_FILE}" "${DEST_PATH}/${FILE_NAME}.sym"
}

for BINARY_FILE in $(find bundle/* | sed 's/tools\/.*$//g' | grep "\S" | grep -e ugeneui -e ugenecl -e lib.*.so.*); do
  dump_symbols "${BINARY_FILE}"
done
echo "##teamcity[blockClosed name='Dump symbols']"

echo "##teamcity[blockOpened name='Archive']"
# Run UGENE not from the 'bundle' dir but from the original 'ugene' dir to avoid inclusion of run-artifacts into the release.
VERSION=$(bundle/ugenecl --version | grep 'version of UGENE' | sed -n "s/.*version of UGENE \([0-9.A-Za-z-]\+\).*/\1/p")

if [ -z "${VERSION}" ]; then
  echo "##teamcity[buildStatus status='FAILURE' text='{build.status.text}. Failed to get version of UGENE']"
  exit 1
fi

RELEASE_BASE_FILE_NAME=ugene-"${VERSION}"-b"${TEAMCITY_BUILD_NUMBER}"-linux-x86-64
RELEASE_UNPACKED_DIR_NAME=ugene-"${VERSION}"

rm -rf "ugene-"*
mv bundle "${RELEASE_UNPACKED_DIR_NAME}"
tar cfz "${RELEASE_BASE_FILE_NAME}.tar.gz" "${RELEASE_UNPACKED_DIR_NAME}"

echo Compressing symbols...
tar cfz "${SYMBOLS_DIR_NAME}.tar.gz" "${SYMBOLS_DIR_NAME}"

echo "##teamcity[blockClosed name='Archive']"
