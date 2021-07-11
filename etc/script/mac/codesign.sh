#!/bin/bash

if [ -d "${1}" ]; then
    echo "Signing recursively all files in directory '${1}'"
    contents_dir="$1/Contents"
elif [ -f "${1}" ]; then
    echo "Signing file '${1}'"
    codesign \
        --sign "Developer ID Application: Alteametasoft" \
        --timestamp \
        --force \
        --verbose=4 \
        "${1}" \
    || exit 1
    exit 0
else
    echo "ERROR: First arg must be directory or single file! Exit."
    exit 1
fi


SPECIAL_TOOLS="cutadapt java8 python2 wevote"

echo "============= Sign files in $contents_dir/MacOS/tools dir, except $SPECIAL_TOOLS ============="
find "$contents_dir/MacOS/tools" -not \( \
    -path "$contents_dir"/MacOS/tools/cutadapt -prune -o \
    -path "$contents_dir"/MacOS/tools/java8 -prune -o \
    -path "$contents_dir"/MacOS/tools/python2 -prune -o \
    -path "$contents_dir"/MacOS/tools/wevote -prune \
\) -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "{}" \; \
|| exit 1

for TOOL_DIR_NAME in $SPECIAL_TOOLS; do
    echo "============= Sign special files in $contents_dir/MacOS/tools/${TOOL_DIR_NAME} dir ============="
    find "${contents_dir}/MacOS/tools/${TOOL_DIR_NAME}" -type f \
    -exec codesign \
        --sign "Developer ID Application: Alteametasoft" \
        --timestamp \
        --force \
        --verbose=11 \
        --options runtime \
        --strict \
        --entitlements "${SCRIPTS_DIR}/dmg/Entitlements-${TOOL_DIR_NAME}.plist" \
        "{}" \; \
    || exit 1
done 

echo "============= Sign all frameworks ============="
codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "$1"/Contents/Frameworks/* \
|| exit 1

codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "$1"/Contents/PlugIns/* \
|| exit 1

echo "============= Sign all files in $contents_dir/MacOS dir ============="
find "$contents_dir"/MacOS -name '*.dylib' -maxdepth 1 \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "{}" \; \
|| exit 1

find "$contents_dir"/MacOS -name '*.a' -maxdepth 1 \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "{}" \; \
|| exit 1

find "$contents_dir"/MacOS/plugins -name '*.a' -maxdepth 1 \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "{}" \; \
|| exit 1

codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "$contents_dir"/MacOS/plugins_checker \
|| exit 1

codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "$contents_dir"/MacOS/ugenem \
|| exit 1

codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "$contents_dir"/MacOS/ugenecl \
|| exit 1

codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options runtime \
    --strict \
    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
    "$contents_dir"/MacOS/ugeneui \
|| exit 1
