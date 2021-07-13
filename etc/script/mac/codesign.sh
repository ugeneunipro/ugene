#!/bin/bash

if [ -d "${1}" ]; then
  echo "Signing recursively all files in directory '${1}'"
  CONTENTS_DIR="${1}/Contents"
elif [ -f "${1}" ]; then
  echo "Signing file '${1}'"
  codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    "${1}" ||
    exit 1
  exit 0
else
  echo "ERROR: First arg must be directory or single file! Exit."
  exit 1
fi

#SPECIAL_TOOLS="cutadapt java8 python2 wevote"

#echo "============= Sign files in ${CONTENTS_DIR}/MacOS/tools dir, except ${SPECIAL_TOOLS} ============="
#find "$CONTENTS_DIR/MacOS/tools" -not \( \
#    -path "${CONTENTS_DIR}/MacOS/tools/cutadapt" -prune -o \
#    -path "${CONTENTS_DIR}/MacOS/tools/java8" -prune -o \
#    -path "${CONTENTS_DIR}/MacOS/tools/python2" -prune -o \
#    -path "${CONTENTS_DIR}/MacOS/tools/wevote" -prune \
#\) -type f \
#-exec echo "Signing file '${}'" \
#-exec codesign \
#    --sign "Developer ID Application: Alteametasoft" \
#    --timestamp \
#    --force \
#    --options runtime \
#    --strict \
#    --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
#    "{}" \; \
#|| exit 1

#for TOOL_DIR_NAME in ${SPECIAL_TOOLS}; do
#    echo "============= Sign special files in ${CONTENTS_DIR}/MacOS/tools/${TOOL_DIR_NAME} dir ============="
#    find "${CONTENTS_DIR}/MacOS/tools/${TOOL_DIR_NAME}" -type f \
#    -exec echo "Signing file '${}'" \
#    -exec codesign \
#        --sign "Developer ID Application: Alteametasoft" \
#        --timestamp \
#        --force \
#        --options runtime \
#        --strict \
#        --entitlements "${SCRIPTS_DIR}/dmg/Entitlements-${TOOL_DIR_NAME}.plist" \
#        "{}" \; \
#    || exit 1
#done

echo "============= Sign Qt framework in ${CONTENTS_DIR}/Frameworks dir ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/Frameworks/"* ||
  exit 1

echo "============= Sign Qt plugins in ${CONTENTS_DIR}/PlugIns dir ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/PlugIns/"* ||
  exit 1

echo "============= Sign all *.dylib files in ${CONTENTS_DIR}/MacOS dir ============="
find "${CONTENTS_DIR}/MacOS" -name '*.dylib' -maxdepth 1 \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; ||
  exit 1

echo "============= Sign all *.qm files in ${CONTENTS_DIR}/MacOS dir ============="
find "${CONTENTS_DIR}/MacOS" -name '*.qm' -maxdepth 1 \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; ||
  exit 1

echo "============= Sign all *.dylib files in ${CONTENTS_DIR}/MacOS/plugins dir ============="
find "${CONTENTS_DIR}/MacOS/plugins" -name '*.dylib' -maxdepth 1 \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; ||
  exit 1

echo "============= Sign plugin_checker  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}"/MacOS/plugins_checker ||
  exit 1

echo "============= Sign ugenem  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/MacOS/ugenem" ||
  exit 1

echo "============= Sign ugenecl  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/MacOS/ugenecl" ||
  exit 1

echo "============= Sign ugeneui  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/MacOS/ugeneui" ||
  exit 1
