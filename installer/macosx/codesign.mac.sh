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
        entitlements="$1/Contents/Info.plist"
    fi
    contents_dir="$1/Contents"
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
        --verbose=4 \
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
    --options hard \
    --strict \
    "$1"/Contents/Frameworks/* \
|| exit -1

echo "============= Sign all files in $contents_dir/PlugIns dir ============="
find "$contents_dir"/PlugIns -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "{}" \; \
|| exit -1

echo "============= Sign all files in $contents_dir/Resources dir ============="
find "$contents_dir"/Resources -type f \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "{}" \; \
|| exit -1

echo "============= Sign all files in $contents_dir/MacOS dir ============="
find "$contents_dir"/MacOS -name '*.dylib' \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "{}" \; \
|| exit -1
find "$contents_dir"/MacOS -name '*.a' \
-exec codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "{}" \; \
|| exit -1
codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "$contents_dir"/MacOS/plugins_checker \
|| exit -1
codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "$contents_dir"/MacOS/ugenem \
|| exit -1
codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "$contents_dir"/MacOS/ugenecl \
|| exit -1
codesign \
    --sign "Developer ID Application: Alteametasoft" \
    --timestamp \
    --force \
    --verbose=11 \
    --options hard \
    --strict \
    "$contents_dir"/MacOS/ugeneui \
|| exit -1

