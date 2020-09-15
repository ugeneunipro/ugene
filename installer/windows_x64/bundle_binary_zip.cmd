REM the root dir for this scirpt is svn trunk
set U_ROOT=%cd%
set U_VERSION=%1
set OUTPUT_DIR=%U_ROOT%\installer\windows_x64\ugene-%U_VERSION%
set INSTALL_DIR=%U_ROOT%\installer\windows_x64
set BINARY_ZIPFILE=%INSTALL_DIR%\ugene-%U_VERSION%-win-x64-r%BUILD_VCS_NUMBER_new_trunk%.zip
set SYMBOLS_DIR_NAME=symbols
set SYMBOLS_DIR=%U_ROOT%\installer\windows_x64\%SYMBOLS_DIR_NAME%
set SYMBOLS_DIR_ZIPFILE=%SYMBOLS_DIR%.zip
set DUMP_SYMBOLS_LOG=%INSTALL_DIR%\dump_symbols_log.txt

IF EXIST %OUTPUT_DIR% del /F /S /Q %OUTPUT_DIR%
IF EXIST %SYMBOLS_DIR% del /F /S /Q %SYMBOLS_DIR%
IF EXIST %SYMBOLS_DIR% del /F /S /Q %DUMP_SYMBOLS_LOG%
IF EXIST %SYMBOLS_DIR% del /F /S /Q %SYMBOLS_DIR_ZIPFILE%
IF EXIST %BINARY_ZIPFILE% del /F /Q %BINARY_ZIPFILE%
set RELEASE_DIR=src\_release

REM create release dir
mkdir %OUTPUT_DIR%
mkdir "%OUTPUT_DIR%\plugins"
mkdir %SYMBOLS_DIR%

REM copy includes
copy %U_ROOT%\LICENSE.txt %OUTPUT_DIR%
copy %U_ROOT%\LICENSE.3rd_party.txt %OUTPUT_DIR%
xcopy /E %PATH_TO_INCLUDE_LIBS%\* %OUTPUT_DIR%
REM copy Qt libraries
copy %PATH_TO_QT_LIBS%\Qt5Core.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Gui.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Multimedia.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5MultimediaWidgets.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Network.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5OpenGL.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Positioning.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5PrintSupport.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Qml.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Quick.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Script.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5ScriptTools.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Sensors.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Sql.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Svg.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Test.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Widgets.dll %OUTPUT_DIR%
copy %PATH_TO_QT_LIBS%\Qt5Xml.dll %OUTPUT_DIR%
mkdir "%OUTPUT_DIR%\sqldrivers"
copy %PATH_TO_QT_LIBS%\..\plugins\sqldrivers\qsqlmysql.dll %OUTPUT_DIR%\sqldrivers\
mkdir "%OUTPUT_DIR%\imageformats"
copy %PATH_TO_QT_LIBS%\..\plugins\imageformats\qgif.dll %OUTPUT_DIR%\imageformats\
copy %PATH_TO_QT_LIBS%\..\plugins\imageformats\qjpeg.dll %OUTPUT_DIR%\imageformats\
copy %PATH_TO_QT_LIBS%\..\plugins\imageformats\qsvg.dll %OUTPUT_DIR%\imageformats\
copy %PATH_TO_QT_LIBS%\..\plugins\imageformats\qtiff.dll %OUTPUT_DIR%\imageformats\
mkdir "%OUTPUT_DIR%\platforms"
copy %PATH_TO_QT_LIBS%\..\plugins\platforms\qwindows.dll %OUTPUT_DIR%\platforms\

REM copy external tools if exists
xcopy /E /Y %RELEASE_DIR%\tools\* %OUTPUT_DIR%\tools\
if DEFINED UGENE_R_DIST_PATH (
    xcopy /E %UGENE_R_DIST_PATH% %OUTPUT_DIR%\tools\
)

REM copy executables
call %INSTALL_DIR%\copy_executable.cmd ugeneui
call %INSTALL_DIR%\copy_executable.cmd ugenecl
call %INSTALL_DIR%\copy_executable.cmd ugenem
call %INSTALL_DIR%\copy_executable.cmd plugins_checker
echo. > %OUTPUT_DIR%\UGENE.ini

REM copy translations
copy %RELEASE_DIR%\transl_en.qm %OUTPUT_DIR%
copy %RELEASE_DIR%\transl_ru.qm %OUTPUT_DIR%

REM copy data
xcopy /I /S %U_ROOT%\data %OUTPUT_DIR%\data
copy %U_ROOT%\data\manuals\*.pdf %OUTPUT_DIR%\data\manuals\
if DEFINED UGENE_CISTROME_PATH (
    move %UGENE_CISTROME_PATH% %OUTPUT_DIR%\data\
)

REM copy libs
call %INSTALL_DIR%\copy_lib.cmd ugenedb
call %INSTALL_DIR%\copy_lib.cmd breakpad
call %INSTALL_DIR%\copy_lib.cmd U2Algorithm
call %INSTALL_DIR%\copy_lib.cmd U2Core
call %INSTALL_DIR%\copy_lib.cmd U2Designer
call %INSTALL_DIR%\copy_lib.cmd U2Formats
call %INSTALL_DIR%\copy_lib.cmd U2Gui
call %INSTALL_DIR%\copy_lib.cmd U2Lang
call %INSTALL_DIR%\copy_lib.cmd U2Private
call %INSTALL_DIR%\copy_lib.cmd U2Script
call %INSTALL_DIR%\copy_lib.cmd U2Test
call %INSTALL_DIR%\copy_lib.cmd U2View

REM copy plugins
call %INSTALL_DIR%\copy_plugin.cmd annotator
call %INSTALL_DIR%\copy_plugin.cmd ball
call %INSTALL_DIR%\copy_plugin.cmd biostruct3d_view
call %INSTALL_DIR%\copy_plugin.cmd chroma_view
call %INSTALL_DIR%\copy_plugin.cmd circular_view
call %INSTALL_DIR%\copy_plugin.cmd clark_support
call %INSTALL_DIR%\copy_plugin.cmd cuda_support
call %INSTALL_DIR%\copy_plugin.cmd dbi_bam
call %INSTALL_DIR%\copy_plugin.cmd diamond_support
call %INSTALL_DIR%\copy_plugin.cmd dna_export
call %INSTALL_DIR%\copy_plugin.cmd dna_flexibility
call %INSTALL_DIR%\copy_plugin.cmd dna_graphpack
call %INSTALL_DIR%\copy_plugin.cmd dna_stat
call %INSTALL_DIR%\copy_plugin.cmd dotplot
call %INSTALL_DIR%\copy_plugin.cmd enzymes
call %INSTALL_DIR%\copy_plugin.cmd external_tool_support
call %INSTALL_DIR%\copy_plugin.cmd genome_aligner
call %INSTALL_DIR%\copy_plugin.cmd gor4
call %INSTALL_DIR%\copy_plugin.cmd hmm2
call %INSTALL_DIR%\copy_plugin.cmd kalign
call %INSTALL_DIR%\copy_plugin.cmd kraken_support
call %INSTALL_DIR%\copy_plugin.cmd linkdata_support
call %INSTALL_DIR%\copy_plugin.cmd metaphlan2_support
call %INSTALL_DIR%\copy_plugin.cmd ngs_reads_classification
call %INSTALL_DIR%\copy_plugin.cmd opencl_support
call %INSTALL_DIR%\copy_plugin.cmd orf_marker
call %INSTALL_DIR%\copy_plugin.cmd pcr
call %INSTALL_DIR%\copy_plugin.cmd phylip
call %INSTALL_DIR%\copy_plugin.cmd primer3
call %INSTALL_DIR%\copy_plugin.cmd psipred
call %INSTALL_DIR%\copy_plugin.cmd ptools
call %INSTALL_DIR%\copy_plugin.cmd query_designer
call %INSTALL_DIR%\copy_plugin.cmd remote_blast
call %INSTALL_DIR%\copy_plugin.cmd repeat_finder
call %INSTALL_DIR%\copy_plugin.cmd sitecon
call %INSTALL_DIR%\copy_plugin.cmd smith_waterman
call %INSTALL_DIR%\copy_plugin.cmd umuscle
call %INSTALL_DIR%\copy_plugin.cmd variants
call %INSTALL_DIR%\copy_plugin.cmd weight_matrix
call %INSTALL_DIR%\copy_plugin.cmd wevote_support
call %INSTALL_DIR%\copy_plugin.cmd workflow_designer


cd %INSTALL_DIR%

resort_symbols.py %SYMBOLS_DIR% >> %DUMP_SYMBOLS_LOG% 2>&1
REM zip -r %SYMBOLS_DIR_ZIPFILE% %SYMBOLS_DIR_NAME%\*
7z a -r %SYMBOLS_DIR_ZIPFILE% %SYMBOLS_DIR_NAME%/*

if NOT DEFINED UGENE_CISTROME_PATH (
REM zip -r %BINARY_ZIPFILE% ugene-%U_VERSION%\*
7z a -r %BINARY_ZIPFILE% ugene-%U_VERSION%/*
)

if DEFINED UGENE_CISTROME_PATH (
    7z a -r %BINARY_ZIPFILE% ugene-%U_VERSION%/*
    move %OUTPUT_DIR%\data\cistrome %UGENE_CISTROME_PATH%
)
cd %U_ROOT%
