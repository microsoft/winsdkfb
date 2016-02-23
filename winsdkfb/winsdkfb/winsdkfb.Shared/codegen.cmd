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
call :GenFile FBCursors.cpp FBCursors.cpp.tt
call :GenFile FBGroup.cpp FBGroup.cpp.tt
call :GenFile FBPage.cpp FBPage.cpp.tt
call :GenFile FBPageCategory.cpp FBPageCategory.cpp.tt
call :GenFile FBPaging.cpp FBPaging.cpp.tt
call :GenFile FBPermission.cpp FBPermission.cpp.tt
call :GenFile FBUser.cpp FBUser.cpp.tt
call :GenFile FBProfilePicture.cpp FBProfilePicture.cpp.tt
call :GenFile FBProfilePictureData.cpp FBProfilePictureData.cpp.tt
call :GenFile FBAppRequest.cpp FBAppRequest.cpp.tt
call :GenFile FBObject.cpp FBObject.cpp.tt
REM header files
call :GenFile FBCursors.h FBCursors.h.tt
call :GenFile FBGroup.h FBGroup.h.tt
call :GenFile FBPage.h FBPage.h.tt
call :GenFile FBPageCategory.h FBPageCategory.h.tt
call :GenFile FBPaging.h FBPaging.h.tt
call :GenFile FBPermission.h FBPermission.h.tt
call :GenFile FBUser.h FBUser.h.tt
call :GenFile FBProfilePicture.h FBProfilePicture.h.tt
call :GenFile FBProfilePictureData.h FBProfilePictureData.h.tt
call :GenFile FBAppRequest.h FBAppRequest.h.tt
call :GenFile FBObject.h FBObject.h.tt

goto End

:NotFound
echo Could not find TextTransform, please add it to your PATH or have it available in %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\14.0 (VS2015) or %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\12.0 (VS2013)

:End
exit /b 0

REM call :GenFile GeneratedFileName TemplateFileName
:GenFile
setlocal
set CurrentGeneratedFile=%1
set CurrentTemplateFile=%2
REM check the generated file and its .tt equivalent
if not exist Generated\%CurrentGeneratedFile% goto :RunGenFile
call :ShouldRegen %CurrentGeneratedFile% %CurrentTemplateFile%
if %errorlevel% EQU 1 goto :RunGenFile
REM check the .xml file
for %%i in (%CurrentGeneratedFile%) do set BaseName=%%~ni
call :ShouldRegen %CurrentGeneratedFile% %BaseName%.xml
if %errorlevel% EQU 1 goto :RunGenFile
REM check if we need to also check a .ttinclude file
set Ext=""
set TTIncludeFile="x"
for %%i in (%CurrentGeneratedFile%) do set Ext=%%~xi
if "%Ext%" EQU ".cpp" set TTIncludeFile=FBGraphObjectImplementation.ttinclude
if "%Ext%" EQU ".h" set TTIncludeFile=FBGraphObjectHeader.ttinclude
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
set CompareTool="..\..\build\CompareFileDates.exe"
%CompareTool% "%CD%\Generated\%CurrentGeneratedFile%" "%CD%\%CurrentTemplateFile%"
if %errorlevel% EQU 1 exit /b 1 else exit /b 0
