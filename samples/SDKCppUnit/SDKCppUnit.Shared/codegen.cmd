@echo off
setlocal

REM Add TextTransform to our path based on the default location for VS2015
set _TT=%CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\14.0
set PATH=%_TT%;%PATH%;

REM Verify that we can find TextTransform
where /q TextTransform.exe
if "%ErrorLevel%" == "0" goto :Found

REM Add TextTransform to our path based on the default location for VS2013
set _TT=%CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\12.0
set PATH=%_TT%;%PATH%;

REM Verify that we can find TextTransform
where /q TextTransform.exe
if not "%ErrorLevel%" == "0" goto :NotFound

:Found
if not exist Generated md Generated
REM cpp files
call :GenFile TestUser.cpp TestUser.cpp.tt
call :GenFile APIResult.cpp APIResult.cpp.tt
call :GenFile TestPhoto.cpp TestPhoto.cpp.tt
REM header files
call :GenFile TestUser.h TestUser.h.tt
call :GenFile APIResult.h APIResult.h.tt
call :GenFile TestPhoto.h TestPhoto.h.tt

goto End

:NotFound
echo Could not find TextTransform, please add it to your PATH or have it available in %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\14.0 (VS2015) or %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\12.0 (VS2013)

:End
exit /b 0

:GenFile
REM call :GenFile GeneratedFileName TemplateFileName
setlocal
set CurrentGeneratedFile=%1
set CurrentTemplateFile=%2
if not exist Generated\%CurrentGeneratedFile% goto :RunGenFile
call :ShouldRegen %CurrentGeneratedFile% %CurrentTemplateFile%
if %errorlevel% EQU 1 goto :RunGenFile
REM check the .xml file
for %%i in (%CurrentGeneratedFile%) do set BaseName=%%~ni
call :ShouldRegen %CurrentGeneratedFile% %BaseName%.xml
if %errorlevel% EQU 1 goto :RunGenFile
REM check if we need to alse check a .ttinclude file
set Ext=""
set TTIncludeFile="x"
for %%i in (%CurrentGeneratedFile%) do set Ext=%%~xi
if "%Ext%" EQU ".cpp" set TTIncludeFile=SDKCppUnitObjectImplementation.ttinclude
if "%Ext%" EQU ".h" set TTIncludeFile=SDKCppUnitObjectHeader.ttinclude
if "%TTIncludeFile%" EQU "x" exit /b 0
call :ShouldRegen %CurrentGeneratedFile% %TTIncludeFile%
if %errorlevel% EQU 1 goto :RunGenFile
exit /b 0

:RunGenFile
echo Generating Generated\%CurrentGeneratedFile%
TextTransform.exe -out Generated\%CurrentGeneratedFile% %CurrentTemplateFile%
exit /b 0


REM call :ShouldRegen GeneratedFileName TemplateFileName
REM returns 1 if should regen, 0 otherwise
:ShouldRegen
setlocal
set CurrentGeneratedFile=%1
set CurrentTemplateFile=%2
set CompareTool="..\..\..\winsdkfb\build\CompareFileDates.exe"
%CompareTool% "%CD%\Generated\%CurrentGeneratedFile%" "%CD%\%CurrentTemplateFile%"
if %errorlevel% EQU 1 exit /b 1
exit /b 0
