@ECHO OFF
REM Restores test folder & prepares initial UGENE.init file before each test run.

IF %UGENE_USE_MASTER_USER_INI%==1 (
    IF DEFINED UGENE_USER_INI (
        IF EXIST %UGENE_MASTER_USER_INI% (
            ECHO f | xcopy  "%UGENE_MASTER_USER_INI%" "%UGENE_USER_INI:/=\%"
        )
    )
)
