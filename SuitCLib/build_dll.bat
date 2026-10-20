@echo off
REM Build SuitCLib.dll in Release with MSBuild.
REM Visual Studio is located with vswhere, and the solution is taken from this
REM script's own directory, so the script works from any checkout on any box.
REM
REM Usage: build_dll.bat [x64|x86]     (x64 is the default)
REM Debug x64 also works (same results, about 3x slower); this script builds Release.
setlocal

set "PLATFORM=%~1"
if "%PLATFORM%"=="" set "PLATFORM=x64"
if /I "%PLATFORM%"=="x86" (set "ARCH=x86") else (set "ARCH=amd64")

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto :novs

REM VsDevCmd.bat itself shells out to a bare `vswhere`, so its directory has to be
REM on PATH or the call prints "'vswhere.exe' is not recognized" on the way through.
for %%d in ("%VSWHERE%") do set "PATH=%PATH%;%%~dpd"

set "VSINSTALL="
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL goto :novs
set "VSDEVCMD=%VSINSTALL%\Common7\Tools\VsDevCmd.bat"
if not exist "%VSDEVCMD%" goto :novs

call "%VSDEVCMD%" -arch=%ARCH% -no_logo
if errorlevel 1 goto :devcmdfailed

pushd "%~dp0"
msbuild SuitCLib.sln /p:Configuration=Release /p:Platform=%PLATFORM% /t:SuitCLib /m /v:minimal
set "RC=%errorlevel%"
popd
echo BUILD_EXIT_CODE=%RC%
exit /b %RC%

:novs
echo [ERROR] No Visual Studio installation with MSBuild was found.
echo         Looked for vswhere at "%VSWHERE%".
exit /b 1

:devcmdfailed
echo [ERROR] "%VSDEVCMD%" failed for -arch=%ARCH%.
exit /b 1
