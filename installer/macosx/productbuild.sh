#!/bin/bash
set -x

if [[ -f `dirname $0`/utils.sh ]]; then source `dirname $0`/utils.sh; fi

if [[ -z "$1" ]]; then 
    echo "ERROR: 1st arg must be application dir!" 
    exit -1
fi
if [[ -z "$2" ]]; then 
    echo "ERROR: 2nd arg must be package name!" 
    exit -2
fi
if [[ -z "$3" ]]; then 
    echo "ERROR: 3rd arg must be new signed package name!" 
    exit -2
fi

#productbuild \
#    --component "$1" \
#    /Applications/ \
#    "$2"

pkgbuild \
    --analyze \
    --root "$1" \
    "Unipro UGENE-component.plist"

plutil \
    -replace BundleIsRelocatable \
    -bool NO \
    "Unipro UGENE-component.plist"

pkgbuild \
    --root "$1" \
    --identifier net.ugene.ugene \
    --version 39.0 \
    --install-location /Applications/UGENE.app \
    --component-plist "Unipro UGENE-component.plist" \
    "$2"

productsign \
    --sign "Developer ID Installer: Alteametasoft" \
    --keychain ~/Library/Keychains/login.keychain \
    "$2" \
    "$3"

