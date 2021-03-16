#!/bin/bash
#set -x

#/Users/ichebyki/UGENE/git/master/ugene/installer/macosx/pkg-dmg \
#    --source /Users/ichebyki/Downloads/code-sign \
#    --target ugene-37.0-mac-x86_64-r11111.dmg \
#    --license /Users/ichebyki/UGENE/git/master/ugene/installer/macosx/LICENSE.with_3rd_party \
#    --volname Unipro UGENE 37.0 \
#    --symlink /Applications

#To get a list of signing identities, I found the perfect answer in this objc.io article:
#    security find-identity -v -p codesigning

if [ -d "$1" ]; then
    echo "Signing recursively all files in directory '$1'"
    if [ -f "$2" ]; then
	entitlements="$2"
    else
	entitlements="$1/Info.plist"
    fi
elif [ -f "$1" ]; then
    if [ ! -f "$2" ]; then
        echo "ERROR: Second arg must be entitlements file. Exit"
    elif [ -z "$2" ]; then
        echo "ERROR: Second arg must be entitlements file. Exit"
    fi
    entitlements="$2"
    echo "Signing file '$1'"
    codesign \
        --sign "Developer ID Application: Alteametasoft" \
        --timestamp \
        --force \
        --verbose=11 \
        --entitlements "$entitlements" \
        "$1" \
    || exit -1
    exit 0
else
    echo "ERROR: First arg must be directory or single file! Exit."
    exit -1
fi

echo "============= Sign all frameworks ============="
codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "$1"/Frameworks/* \
|| exit -1

echo "============= Sign all files in PlugIns dir ============="
find "$1"/PlugIns -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "{}" \; \
|| exit -1

echo "============= Sign all files in Resources dir ============="
find "$1"/Resources -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "{}" \; \
|| exit -1

echo "============= Sign all files in MacOS/data dir ============="
find "$1"/MacOS/data -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "{}" \; \
|| exit -1

echo "============= Sign all files in MacOS/plugins dir ============="
find "$1"/MacOS/plugins -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "{}" \; \
|| exit -1

echo "============= Sign all files in MacOS/tools dir ============="
find "$1"/MacOS/tools -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "{}" \; \
|| exit -1

echo "============= Sign all files in MacOS dir ============="
find "$1"/MacOS -maxdepth 1 -a -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --entitlements "$entitlements" \
    "{}" \; \
|| exit -1

