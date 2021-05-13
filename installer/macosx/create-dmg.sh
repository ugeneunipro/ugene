#!/bin/bash

if [ -z "$2" ]; then
    echo "ERROR: wrong args!"
    echo "Usage: create-dmg.sh <dmg-dir> <ugene-version> <app-dir> <pkg-file> <dmg-file>"
    exit -1
fi

DMG_DIR="$1"
UGENE_VERSION="$2"
APP_DIR="$3"
PKG_FILE="$4"
DMG_FILE="$5"

mkdir -p "$DMG_DIR"
cp -R "$APP_DIR"/Contents/Resources/data/samples "$DMG_DIR"
mv "$DMG_DIR"/samples "$DMG_DIR"/Samples
cp readme.txt "$DMG_DIR"
cp "$PKG_FILE" "$DMG_DIR"

if [[ 1 == 1 ]]; then
    szk=`du -ks "$DMG_DIR"  | perl -n -e 'if(/(\d+)\D.*/){print $DMG_DIR;}'`
    szm=$((szk / 1000 + 50))
    echo
    echo hdiutil create -size ${szm}m -srcfolder "$DMG_DIR" -volname "UGENE-$UGENE_VERSION" "$DMG_FILE"
    hdiutil create -size ${szm}m -srcfolder "$DMG_DIR" -volname "UGENE-$UGENE_VERSION" "$DMG_FILE"
else
    echo
    echo pkg-dmg running...
    echo ./pkg-dmg --source "$DMG_DIR" --target "$DMG_FILE" --license ./LICENSE.with_3rd_party --volname "Unipro UGENE $UGENE_VERSION" --symlink /Applications
    ./pkg-dmg --source "$DMG_DIR" --target "$DMG_FILE" --license ./LICENSE.with_3rd_party --volname "Unipro UGENE $UGENE_VERSION" --symlink /Applications
fi

echo
echo Signing dmg-file...
echo ./codesign.mac.sh "$DMG_FILE"
bash ./codesign.mac.sh "$DMG_FILE"
