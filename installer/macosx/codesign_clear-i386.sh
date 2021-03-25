#!/bin/bash

__pwd__=`pwd`
rootdir=`dirname $0`
rootdir=`(cd $rootdir; pwd)`

if [ ! -d $rootdir ]; then
    exit
fi

SECONDS=0
LOG_FILE=$rootdir/`basename $0`.log
# Save stdout, stderr
exec 3>&1 4>&2
# Redirect output ( > ) into a named pipe ( >() ) running "tee"
exec >  >(tee -i ${LOG_FILE}) 2>&1

function finish {
    EXIT_CODE=$?
    pkill -P $$
    SCRIPT_RUN_TIME=$SECONDS
    # Restore output
    exec 1>&3 2>&4
    exit $EXIT_CODE
}
trap finish EXIT

echo '#!/bin/bash'                                   > thin_x86_64.sh
echo 'f="$1"'                                           >> thin_x86_64.sh
echo 'echo mv "${f}" "${f}.bak"'                        >> thin_x86_64.sh
echo 'mv "${f}" "${f}.bak"'                             >> thin_x86_64.sh
echo 'echo lipo "${f}.bak" -thin x86_64 -output "${f}"' >> thin_x86_64.sh
echo 'lipo "${f}.bak" -thin x86_64 -output "${f}"'      >> thin_x86_64.sh
echo 'echo rm -f "${f}.bak"'                            >> thin_x86_64.sh
echo 'rm -f "${f}.bak"'                                 >> thin_x86_64.sh

find "$1" -type f -exec file {} \; 2>&1 \
    | grep "architectures" \
    | grep i386: \
    | grep x86_64: \
    | tee lipo-i386-list.txt

cat lipo-i386-list.txt \
    | perl -n -e '$_=~s/([^:]+)\:.*/$1/; print $_;' \
    | xargs -L 1 -I "{}" bash ./thin_x86_64.sh "{}"



# lipo _MDmod.so.bak -thin x86_64 -output _MDmod.so
# lipo: can't figure out the architecture type of:

# _MDmod.so: Mach-O universal binary with 2 architectures: [i386: Mach-O bundle i386] [x86_64: Mach-O 64-bit bundle x86_64]
# _MDmod.so (for architecture i386):	Mach-O bundle i386
# _MDmod.so (for architecture x86_64):	Mach-O 64-bit bundle x86_64
