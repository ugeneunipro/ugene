#!/bin/bash

if [[ -f `dirname $0`/utils.sh ]]; then source `dirname $0`/utils.sh; fi

if [[ -z "$1" ]]; then echo ERROR!; exit -1; fi

productbuild \
    --component "$1" \
    /Applications/ \
    "$2"

productsign \
    --sign "Developer ID Installer: Alteametasoft" \
    --keychain ~/Library/Keychains/login.keychain \
    "$2" \
    "$3"

