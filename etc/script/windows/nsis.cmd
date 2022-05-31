set TEAMCITY_WORK_DIR=%cd%
set UGENE_APP_DIR=%TEAMCITY_WORK_DIR%\ugene_app
set UGENE_GIT_DIR=%TEAMCITY_WORK_DIR%\ugene_git

SET GET_VERSION_CMD="type version.txt"
FOR /F "tokens=* USEBACKQ" %%F IN (`%GET_VERSION_CMD%`) DO (SET UGENE_VERSION=%%F)
SET UGENE_DISTRIB_FILE_NAME=ugene-n%TEAMCITY_BUILD_COUNTER%-p%TEAMCITY_PORTABLE_BUILD_COUNTER%-%UGENE_VERSION%-win-x86-64.exe
SET GENERATED_BUILD_NSH="%TEAMCITY_WORK_DIR%\build.nsh"

echo "    ##teamcity[blockOpened name='Removing unneeded files']"
    IF EXIST %UGENE_APP_DIR%\ugene-%UGENE_VERSION%\UGENE.ini DEL /F %UGENE_APP_DIR%\ugene-%UGENE_VERSION%\UGENE.ini
echo "    ##teamcity[blockClosed name='Removing unneeded files']"

echo "    ##teamcity[blockOpened name='Building NSIS binary']"
    echo !define UGENE_DISTRIB_FILE_NAME %UGENE_DISTRIB_FILE_NAME% > %GENERATED_BUILD_NSH%
    echo !define UGENE_BUNDLE_DIR %UGENE_APP_DIR%\ugene-%UGENE_VERSION% >> %GENERATED_BUILD_NSH%
    echo !define UGENE_GIT_DIR %UGENE_GIT_DIR% >> %GENERATED_BUILD_NSH%
    echo !define ProductVersion %UGENE_VERSION% | sed 's/[\.-]/_/g' >> %GENERATED_BUILD_NSH%
    echo !define PrintableVersion %UGENE_VERSION%  >> %GENERATED_BUILD_NSH%
    cat "%UGENE_GIT_DIR%/etc/script/windows/nsis/ugene.nsh" >> %GENERATED_BUILD_NSH%
    cp "%UGENE_GIT_DIR%/etc/script/windows/nsis/ugene_extensions.nsh" .
    makensis "%GENERATED_BUILD_NSH%"
echo "    ##teamcity[blockClosed name='Building NSIS binary']"

echo "    ##teamcity[blockOpened name='Signing installer']"
    signtool.exe sign /a /t http://timestamp.digicert.com /s MY /n "Novosibirsk Center of Information Technologies UNIPRO Ltd." /debug  /v %TEAMCITY_WORK_DIR%\%UGENE_DISTRIB_FILE_NAME%
echo "    ##teamcity[blockClosed name='Signing installer']"

