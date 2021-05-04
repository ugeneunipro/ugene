#!/bin/bash

if [[ -f `dirname $0`/utils.sh ]]; then source `dirname $0`/utils.sh; fi

# Usage: altool --validate-app -f file -t platform -u username [-p password]
#        altool --upload-app -f file -t platform -u username -p password
export PATH=/Applications/Xcode.app/Contents/Applications/Application\ Loader.app/Contents/Frameworks/ITunesSoftwareService.framework/Versions/A/Support:$PATH

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
    echo "Usage: notarize.sh [-u|-v|-n] <pachage>"
    exit -1
fi

xcrun altool \
    $op \
    -f "$2" \
    -t osx \
    --primary-bundle-id net.ugene.ugene \
    --username "appleid@alteametasoft.com" \
    --password "dhtf-revf-yfqo-bsqg" 2>&1 | tee RequestUUID.txt
if [[ $? == 0 ]]; then
    cat RequestUUID.txt | perl -n -e '$_=~s/\n//; if (/RequestUUID\s*=\s*(.+)/) {print $1;}' > RequestUUID
    xcrun altool \
        --notarization-info `cat RequestUUID` \
        --username "appleid@alteametasoft.com" \
        --password "dhtf-revf-yfqo-bsqg" 2>&1
    echo xcrun altool \
        --notarization-info `cat RequestUUID` \
        --username "appleid@alteametasoft.com" \
        --password "dhtf-revf-yfqo-bsqg" 2>&1 > "$0".info.sh
    chmod a+x "$0".info.sh
fi


#xcrun altool --notarize-app \
#    --primary-bundle-id "net.ugene.ugene.zip" \
#    --username "appleid@alteametasoft.com" \
#    --password "jbiw-xjmq-alts-vnjr" \
#    --file "$1"

