@ECHO OFF

SET TEAMCITY_WORK_DIR=%cd%
SET UGENE_APP_DIR=%TEAMCITY_WORK_DIR%\ugene_app
SET UGENE_OUTPUT_DIR=%TEAMCITY_WORK_DIR%\out

SET UGENE_USE_CRASH_HANDLER=0
SET UGENE_DEV=1
SET UGENE_GUI_TEST=1
SET UGENE_USE_NATIVE_DIALOGS=0
SET UGENE_PRINT_TO_CONSOLE=true
SET UGENE_ENABLE_EXPERIMENTAL_FEATURES=1

SET UGENE_TESTS_PATH=%TEAMCITY_WORK_DIR%\test
SET UGENE_DATA_PATH=%UGENE_APP_DIR%\data
SET UGENE_TOOLS_DIR=%TEAMCITY_WORK_DIR%\tools
SET UGENE_SNPEFF_DB_LIST=%UGENE_OUTPUT_DIR%\SnpEff_DB.list
SET UGENE_TRANSLATION=en

REM %UGENE_MASTER_USER_INI% is copied before each test run into a dedicated ugene.ini used by the test.
SET UGENE_MASTER_USER_INI=%UGENE_OUTPUT_DIR%\ugene_TestLauncher.ini
SET UGENE_USE_MASTER_USER_INI=1
SET UGENE_TEST_EXTERNAL_SCRIPT_TO_RESTORE=%TEAMCITY_WORK_DIR%\ugene_git\etc\script\windows\restore_dirs_before_test.cmd
SET UGENE_WORKFLOW_OUTPUT_DIR=%TEAMCITY_WORK_DIR%\out_workflow
SET UGENE_PRIMER_LIBRARY_PATH=%TEAMCITY_WORK_DIR%\out_primers

IF NOT DEFINED UGENE_GUI_SUITE_NUMBER (SET UGENE_GUI_SUITE_NUMBER=0)
IF NOT DEFINED UGENE_GUI_TEST_ARG (SET UGENE_GUI_TEST_ARG=--gui-test-suite=%UGENE_GUI_SUITE_NUMBER%)

RD /S /Q "%UGENE_WORKFLOW_OUTPUT_DIR"
RD /S /Q "%UGENE_PRIMER_LIBRARY_PATH"
RD /S /Q "%UGENE_OUTPUT_DIR%"

ECHO "======= ENVIRONMENT (START) ======="
SET
ECHO "======= ENVIRONMENT (END) ========="

REM Sync working and original SVN dirs: this is faster than clean checkout.
ECHO "##teamcity[blockOpened name='Create a copy of _common_data dir']"
IF NOT EXIST test mkdir test
rsync -a --delete --exclude=.svn* test_data/_common_data/ test/_common_data/
ECHO "##teamcity[blockClosed name='Create a copy of _common_data dir']"

ECHO "##teamcity[blockOpened name='Running tests']"

ECHO "Running UGENE %UGENE_APP_DIR%\ugeneui --ini-file="%UGENE_MASTER_USER_INI%" %UGENE_GUI_TEST_ARG%"
%UGENE_APP_DIR%\ugeneui --ini-file="%UGENE_MASTER_USER_INI%" %UGENE_GUI_TEST_ARG%

IF NOT ERRORLEVEL 0 (
    SET ERROR_MESSAGE="ugeneui exited with a non-0 error level."
    GOTO END_WITH_ERROR
)

ECHO "##teamcity[blockClosed name='Running Tests']"

exit 0

:END_WITH_ERROR
    ECHO "ERROR: %ERROR_MESSAGE"
    ECHO "##teamcity[buildStatus status='FAILURE' text='{build.status.text}.']"
    exit 1
