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

echo "============= Sign Qt frameworks in ${CONTENTS_DIR}/Frameworks dir ============="
find "${CONTENTS_DIR}/Frameworks" -name 'Current' \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; || exit 1

echo "============= Sign all *.dylib files in ${CONTENTS_DIR}/Frameworks dir ============="
find "${CONTENTS_DIR}/Frameworks" -name '*.dylib' \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; || exit 1

echo "============= Sign all *.dylib files in ${CONTENTS_DIR}/PlugIns dir ============="
find "${CONTENTS_DIR}/PlugIns" -name '*.dylib' \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; || exit 1

echo "============= Sign ugeneui & all dependencies with relaxed entitlements  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements-tools.plist" \
  "${CONTENTS_DIR}/MacOS/ugeneui" || exit 1

echo "============= Sign all *.dylib files in ${CONTENTS_DIR}/MacOS dir ============="
find "${CONTENTS_DIR}/MacOS" -name '*.dylib' -maxdepth 1 \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; || exit 1

echo "============= Sign all *.qm files in ${CONTENTS_DIR}/MacOS dir ============="
find "${CONTENTS_DIR}/MacOS" -name '*.qm' -maxdepth 1 \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; || exit 1

echo "============= Sign all *.dylib files in ${CONTENTS_DIR}/MacOS/plugins dir ============="
find "${CONTENTS_DIR}/MacOS/plugins" -name '*.dylib' -maxdepth 1 \
  -exec codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "{}" \; || exit 1

echo "============= Sign plugin_checker  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}"/MacOS/plugins_checker || exit 1

echo "============= Sign ugenem  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/MacOS/ugenem" || exit 1

echo "============= Sign ugenecl  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/MacOS/ugenecl" || exit 1

echo "============= Sign ugeneui  ============="
codesign \
  --sign "Developer ID Application: Alteametasoft" \
  --timestamp \
  --force \
  --options runtime \
  --strict \
  --entitlements "${SCRIPTS_DIR}/dmg/Entitlements.plist" \
  "${CONTENTS_DIR}/MacOS/ugeneui" || exit 1
