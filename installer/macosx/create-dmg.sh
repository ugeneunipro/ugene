#!/bin/bash

if [ -z "$2" ]; then
    echo "ERROR: wrong args!"
    echo "Usage: create-dmg.sh <dmg-dir> <dmg-file>"
    exit -1
fi

DMG_IMAGE=$1
UGENE_VERSION=$2
DMG_FILE="Unipro UGENE.dmg"

mkdir -p "$DMG_IMAGE"
cp -R Unipro\ UGENE.app/Contents/Resources/data/samples "$1"
mv "$1"/samples "$1"/Samples
cp readme.txt "$1"
xcrun stapler staple Unipro\ UGENE.app
cp Unipro\ UGENE.pkg "$1"

if [[ 1 == 1 ]]; then
    szk=`du -ks "$DMG_IMAGE"  | perl -n -e 'if(/(\d+)\D.*/){print $1;}'`
    szm=$((szk / 1000 + 50))
    echo
    echo hdiutil create -size ${szm}m -srcfolder "$1" -volname "UGENE-$2" "$DMG_FILE"
    hdiutil create -size ${szm}m -srcfolder "$1" -volname "UGENE-$2" "$DMG_FILE"
else
    echo
    echo pkg-dmg running...
    echo ./pkg-dmg --source "$1" --target "$DMG_FILE" --license ./LICENSE.with_3rd_party --volname "Unipro UGENE $UGENE_VERSION" --symlink /Applications
    ./pkg-dmg --source "$1" --target "$DMG_FILE" --license ./LICENSE.with_3rd_party --volname "Unipro UGENE $UGENE_VERSION" --symlink /Applications
fi

echo
echo Signing dmg-file...
echo ./codesign.mac.sh "$DMG_FILE"
bash ./codesign.mac.sh "$DMG_FILE"
