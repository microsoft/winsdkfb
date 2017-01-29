rem ---------------------------------------------------------------------------
rem Script to build the Microsoft SDK for Facebook Integration.  This script 
rem will build the Windows 8.1 and Windows Phone 8.1 versions of the SDK, then
rem the Windows 10 Universal version, then package everything up with NuGet.
rem 
rem On completion, the SDK packages should be located in the directory
rem winsdkfb\bin under the root of the repository. A synopsis of the build will
rem exist in the file sdk-build.log in the same directory as the build script.
rem 
rem This script assumes it is located in the Git repository and location in 
rem which it originated.  If it is moved, it will cease to function!
rem ---------------------------------------------------------------------------

set starting_dir=%cd%
set script_dir=%~dp0
set logfile=%script_dir%\sdk-build.log
cd /d %script_dir%\..

@echo Starting build... >%logfile%

rem ---------------------------------------------------------------------------
rem build for Windows 8.1 and Phone 8.1
rem ---------------------------------------------------------------------------
call :build_one_flavor winsdkfb.sln Win32 Debug
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb.sln Win32 Release 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb.sln x64 Debug 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb.sln x64 Release 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb.sln ARM Debug 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb.sln ARM Release 
if errorlevel 1 goto errorExit

rem ---------------------------------------------------------------------------
rem build for Windows 10 Universal 
rem ---------------------------------------------------------------------------

call :build_one_flavor winsdkfb_uwp/winsdkfb_uwp.sln Win32 Debug
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb_uwp/winsdkfb_uwp.sln Win32 Release
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb_uwp/winsdkfb_uwp.sln x64 Debug 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb_uwp/winsdkfb_uwp.sln x64 Release 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb_uwp/winsdkfb_uwp.sln ARM Debug 
if errorlevel 1 goto errorExit

call :build_one_flavor winsdkfb_uwp/winsdkfb_uwp.sln ARM Release 
if errorlevel 1 goto errorExit

rem ---------------------------------------------------------------------------
rem build NuGet packages
rem ---------------------------------------------------------------------------
:build_nuget
cd %script_dir%\nuget

rem Get version number, so we can test for packages after build attempt
for /f "delims=" %%i in ('type VERSION') do set version=%%i

@echo Checking for nuget.exe... >>%logfile%
if not exist nuget.exe call download-nuget.cmd
if not exist nuget.exe (
    @echo Error: failed to download nuget.exe, can't package the SDK
    goto :eof
)

@echo Packaging SDK... >>%logfile%

call build-nupkg.cmd
if not exist ..\..\bin\winsdkfb.%version%.nupkg (
    @echo Error: failed to build SDK NuGet package
    goto :eof
)

cd /d %starting_dir%
@echo Build successful >>%logfile%
@echo "All done!"
goto :eof

rem ---------------------------------------------------------------------------
rem Subroutine to build for one solution, platform, and configuration
rem ---------------------------------------------------------------------------
:build_one_flavor
@echo Starting build for solution %1, platform %2, configuration %3 >>%logfile%

%script_dir%nuget\nuget.exe restore %1

msbuild %1 /p:Platform=%2;Configuration=%3
if errorlevel 1 (
    @echo Error building solution %1, platform %2, configuration %3 >>%logfile%
    exit /b
)
@echo Completed build for solution %1, platform %2, configurattion %3 >>%logfile%
exit /b

rem ---------------------------------------------------------------------------
rem In case of build failure, err out here
rem ---------------------------------------------------------------------------
:errorExit
@echo "Error: one or more targets failed to build."
cd /d %starting_dir%
goto :eof

