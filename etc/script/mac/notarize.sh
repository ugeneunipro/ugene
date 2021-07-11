#!/bin/bash
export PATH=/Applications/Xcode.app/Contents/Applications/Application\ Loader.app/Contents/Frameworks/ITunesSoftwareService.framework/Versions/A/Support:$PATH

if [ -z "${SIGN_USER_ID}" ]; then
  echo "SIGN_USER_ID is not set"
  exit 1
fi

if [ -z "${SIGN_USER_PASSWORD}" ]; then
  echo "SIGN_USER_PASSWORD is not set"
  exit 1
fi

op=""
if [[ "$1" == "-u" ]]; then
    op=--upload-app
elif [[ "$1" == "-v" ]]; then
    op=--validate-app
else
    op=--notarize-app
fi

if [ -z "$2" ]; then
    echo "ERROR: wrong args!"
    echo "Usage: notarize.sh [-u|-v|-n] <package>"
    exit 1
fi

xcrun altool \
    $op \
    --show-progress \
    --verbose \
    -f "$2" \
    -t osx \
    --primary-bundle-id net.ugene.ugene \
    --username "${SIGN_USER_ID}" \
    --password "${SIGN_USER_PASSWORD}" 2>&1 | tee RequestUUID.txt

if [[ $? == 0 ]]; then
    perl -n -e '$_=~s/\n//; if (/RequestUUID\s*=\s*(.+)/) {if("$uuid" ne "$1") {$uuid=$1;print $uuid;}}' < RequestUUID.txt > RequestUUID
    xcrun altool \
        --notarization-info "$(cat ./RequestUUID)" \
        --username "${SIGN_USER_ID}" \
        --password "${SIGN_USER_PASSWORD}"
fi
