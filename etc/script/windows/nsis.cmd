set TEAMCITY_WORK_DIR=%cd%
set UGENE_DIR=%TEAMCITY_WORK_DIR%\ugene

SET GET_VERSION_CMD="type version.txt"
FOR /F "tokens=* USEBACKQ" %%F IN (`%GET_VERSION_CMD%`) DO (SET UGENE_VERSION=%%F)
SET UGENE_DISTRIB_FILE_NAME=ugene-b%TEAMCITY_BUILD_NUMBER%-%UGENE_VERSION%-win-x86-64.exe
SET BUILD_NSH=%TEAMCITY_WORK_DIR%\build.nsh

echo "    ##teamcity[blockOpened name='Building NSIS binary']"
    echo !define UGENE_DISTRIB_FILE_NAME %UGENE_DISTRIB_FILE_NAME% > %BUILD_NSH%
    echo !define UGENE_BUNDLE_DIR ugene_portable\ugene-%UGENE_VERSION% >> %BUILD_NSH%
    echo !define ProductVersion %UGENE_VERSION% | sed 's/[\.-]/_/g' >> %BUILD_NSH%
    echo !define PrintableVersion %UGENE_VERSION%  >> %BUILD_NSH%
    cat dev_scripts/windows/nsis/ugene.nsh >> %BUILD_NSH%
    cp dev_scripts/windows/nsis/ugene_extensions.nsh .
    makensis %BUILD_NSH%
echo "    ##teamcity[blockClosed name='Building NSIS binary']"

echo "    ##teamcity[blockOpened name='Signing installer']"
    signtool.exe sign /a /t http://timestamp.digicert.com /s MY /n "Novosibirsk Center of Information Technologies UNIPRO Ltd." /debug  /v %TEAMCITY_WORK_DIR%\%UGENE_DISTRIB_FILE_NAME%
echo "    ##teamcity[blockClosed name='Signing installer']"

