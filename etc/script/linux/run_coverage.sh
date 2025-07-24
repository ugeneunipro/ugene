#!/bin/bash

TEAMCITY_WORK_DIR=$(pwd)
SCRIPTS_DIR="${TEAMCITY_WORK_DIR}/ugene/etc/script/linux"

echo "##teamcity[blockOpened name='Cleanup']"
rm -r coverage_report
rm coverage.info
echo "##teamcity[blockClosed name='Cleanup']"

echo "##teamcity[blockOpened name='Build UGENE']"
#"${SCRIPTS_DIR}"/build.sh
echo "##teamcity[blockClosed name='Build UGENE']"

echo "##teamcity[blockOpened name='Run XML Tests']"
#"${SCRIPTS_DIR}"/run_xml_tests.sh
echo "##teamcity[blockClosed name='Run XML Tests']"

echo "##teamcity[blockOpened name='Run Coverage']"

echo "Generate coverage"

"${LCOV_DIR}"lcov --capture --directory ./ugene/cmake-build-relwithcov --exclude "/usr/*" --exclude "/home/ugene/Qt/*" --exclude "*_autogen*" --exclude "*qrc_*" --exclude "*breakpad*" --exclude "*qscore/src/qscore*" --exclude "*QSpec*" --exclude "*samtools/src/samtools_core*" --exclude "*sqlite*" --exclude "*qtscript*" --exclude "*zlib*" --exclude "*plugins/api_tests*" --exclude "*plugins/GUITestBase*" --exclude "*ball/src/include*" --exclude "*ball/src/source*" --exclude "*gor4/src/gor.*" --exclude "*gor4/src/nrutil.*" --exclude "*hmm2/src/hmmer2*" --exclude "*phylip/src/neighbor.h" --exclude "*phylip/src/phylip.*" --exclude "*phylip/src/protdist.*" --exclude "*phylip/src/seq.*" --exclude "*phylip/src/seqboot.*" --exclude "*primer3/src/primer3_core*" --exclude "*psipred/src/seq2mtx.cpp" --exclude "*psipred/src/ssdefs*" --exclude "*ptools/src/ptools*" --exclude "*umuscle/src/muscle*" --exclude "*ugenem*" --output-file coverage.info --ignore-errors negative,unused

echo "Generate report"
"${LCOV_DIR}"genhtml coverage.info --output-directory coverage_report --ignore-errors inconsistent

echo "##teamcity[blockClosed name='Run Coverage']"

echo "##teamcity[blockOpened name='Run on server']"

cd ugene

BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
echo "Branch name: ${BRANCH_NAME}"

cd ..

# Убрать все символы, которые могут сломать путь
SAFE_BRANCH_NAME=$(echo "$BRANCH_NAME" | sed 's#[^a-zA-Z0-9._-]#_#g')
echo "Safe branch name: ${SAFE_BRANCH_NAME}"

# Целевая директория
TARGET_DIR="/var/www/html/coverage/${SAFE_BRANCH_NAME}"
echo "Target dir: ${TARGET_DIR}"

#mkdir -p "$TARGET_DIR"
#cp -r coverage_report "$TARGET_DIR/"

#echo "##teamcity[message text='Coverage report: http://ugene-cuda/coverage/${SAFE_BRANCH_NAME}/coverage_report/index.html']"
#echo "http://your-domain/coverage/${SAFE_BRANCH_NAME}/coverage_report/index.html" > coverage_url.txt

echo "##teamcity[blockClosed name='Run on server']"
