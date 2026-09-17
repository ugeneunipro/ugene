@echo off
setlocal
set "PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer;%PATH%"

set "CMAKE_GENERATOR=Visual Studio 17 2022"
for /f "tokens=*" %%i in ('vswhere.exe -latest -products * -version "[17.0,18.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do set "VS_INSTALL=%%i"

rem Allow newer MSVC installations for local verification. TeamCity uses VS 2022.
if not defined VS_INSTALL (
  for /f "tokens=*" %%i in ('vswhere.exe -latest -products * -version "[18.0,19.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do set "VS_INSTALL=%%i"
  set "CMAKE_GENERATOR=Visual Studio 18 2026"
)

if not defined VS_INSTALL (
  echo Visual Studio with the Desktop development with C++ workload is required.
  exit /b 1
)

call "%VS_INSTALL%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

set "PATH=%VS_INSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;C:\Program Files\CMake\bin;%PATH%"
set "BASH_EXE="
if exist "C:\cygwin64\bin\bash.exe" set "BASH_EXE=C:\cygwin64\bin\bash.exe"
if not defined BASH_EXE if exist "C:\Program Files\Git\bin\bash.exe" set "BASH_EXE=C:\Program Files\Git\bin\bash.exe"
if not defined BASH_EXE if exist "C:\tools\git\bin\bash.exe" set "BASH_EXE=C:\tools\git\bin\bash.exe"
if not defined BASH_EXE (
  echo Cygwin or Git for Windows bash is required.
  exit /b 1
)

"%BASH_EXE%" -c ./ugene/etc/script/windows/build.sh
exit /b %ERRORLEVEL%
