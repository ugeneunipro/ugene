@echo off
rem
rem This script runs XML tests suite.
rem The script expects UGENE binaries to be present in 'ugene' folder, tests in the 'ugene_git/tests',
rem test data in 'test_data' and external tools in 'tools'.
rem

set TEAMCITY_WORK_DIR=%cd%
if not defined UGENE_SUB_DIR (set UGENE_SUB_DIR=ugene)

set UGENE_DIR=%TEAMCITY_WORK_DIR%\%UGENE_SUB_DIR%
set UGENE_OUTPUT_DIR=%TEAMCITY_WORK_DIR%\out
set UGENE_TOOLS_DIR=%TEAMCITY_WORK_DIR%\tools

rem Disable crash handler
set USE_CRASH_HANDLER=0

rem Force English by default.
set UGENE_TRANSLATION=en

rem Make UGENE dump all logs to console.
set UGENE_PRINT_TO_CONSOLE=true

rem Use 1 thread by default: this is the safest option because some tests are multi-threaded inside.
if not defined UGENE_TEST_THREADS (set UGENE_TEST_THREADS=1)

rem Default test suite is a pre-commit suite.
if not defined UGENE_TEST_SUITE (set UGENE_TEST_SUITE=PrecommitTests_x64.list)

rem  UGENE_TESTS_PATH is used both by the script and by UGENE tests.
set UGENE_TESTS_PATH=%TEAMCITY_WORK_DIR%\ugene_git\tests

rem XML tests search data in this dir.
if not defined COMMON_DATA_DIR (set COMMON_DATA_DIR=%TEAMCITY_WORK_DIR%\test_data\_common_data)
set TEMP_DATA_DIR=%UGENE_OUTPUT_DIR%\tmp

set UGENE_EXE=%UGENE_DIR%\ugeneui.exe

echo "Work dir: '%TEAMCITY_WORK_DIR%', UGENE_DIR: '%UGENE_DIR%', test suite path: '%UGENE_TESTS_PATH%\%UGENE_TEST_SUITE%', test data dir: '%COMMON_DATA_DIR%'"

echo "##teamcity[blockOpened name='Environment']"
set
echo "##teamcity[blockClosed name='Environment']"

rem Kill any existing ugene instances that may left in a hanging state since the previous run.
taskkill.exe /f /im ugeneui.exe
taskkill.exe /f /im ugenecl.exe

echo "##teamcity[blockOpened name='Running tests: %UGENE_TEST_SUITE%']"

rem UGENE_DEV disables crash reporter.
set UGENE_DEV=1

if exist  %UGENE_OUTPUT_DIR% rmdir /Q /S %UGENE_OUTPUT_DIR%
mkdir %UGENE_OUTPUT_DIR%

echo %UGENE_EXE% --test-suite=%UGENE_TESTS_PATH%\%UGENE_TEST_SUITE% --test-report=%UGENE_OUTPUT_DIR%\test_report.html --test-threads=%UGENE_TEST_THREADS% --ini-file=%UGENE_OUTPUT_DIR%\ugene.ini
%UGENE_EXE% --test-suite=%UGENE_TESTS_PATH%\%UGENE_TEST_SUITE% --test-report=%UGENE_OUTPUT_DIR%\test_report.html --test-threads=%UGENE_TEST_THREADS% --ini-file=%UGENE_OUTPUT_DIR%\ugene.ini

if ERRORLEVEL 1 GOTO EXIT_WITH_UGENE_EXE_FAILED

echo "##teamcity[blockClosed name='Running tests: %UGENE_TEST_SUITE%']"
exit 0

:EXIT_WITH_UGENE_EXE_FAILED
    echo ##teamcity[buildStatus status='FAILURE' text='{build.status.text}. UGENE run failed.']
    exit 0
