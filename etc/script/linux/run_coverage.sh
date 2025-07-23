#!/bin/bash

TEAMCITY_WORK_DIR=$(pwd)
SCRIPTS_DIR="${TEAMCITY_WORK_DIR}/ugene/etc/script/linux"

echo "##teamcity[blockOpened name='Build UGENE']"
"${SCRIPTS_DIR}"/build.sh
echo "##teamcity[blockClosed name='Build UGENE']"

echo "##teamcity[blockOpened name='Run XML Tests']"
"${SCRIPTS_DIR}"/run_xml_tests.sh
echo "##teamcity[blockClosed name='Run XML Tests']"

echo "##teamcity[blockOpened name='Run Coverage']"

"${LCOV_DIR}"lcov --capture --directory ./ugene/cmake-build-relwithcov --output-file coverage.info
"${LCOV_DIR}"lcov --remove coverage.info "/usr/*" "/home/ugene/Qt/*" "*_autogen*" "*qrc_*" "*breakpad*" "*qscore/src/qscore*" "*QSpec*" "*samtools/src/samtools_core*" "*sqlite*" "*qtscript*" "*zlib*" "*plugins/api_tests*" "*plugins/GUITestBase*" "*ball/src/include*" "*ball/src/source*" "*gor4/src/gor.*" "*gor4/src/nrutil.*" "*hmm2/src/hmmer2*" "*phylip/src/neighbor.h" "*phylip/src/phylip.*" "*phylip/src/protdist.*" "*phylip/src/seq.*" "*phylip/src/seqboot.*" "*primer3/src/primer3_core*" "*psipred/src/seq2mtx.cpp" "*psipred/src/ssdefs*" "*ptools/src/ptools*" "*umuscle/src/muscle*" "*ugenem*" --output-file coverage_filtred.info
"${LCOV_DIR}"genhtml coverage_filtred.info --output-directory coverage_report

echo "##teamcity[blockClosed name='Run Coverage']"