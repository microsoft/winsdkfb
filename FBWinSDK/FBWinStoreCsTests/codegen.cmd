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
if not exist Generated\FBObject.cs TextTransform.exe -out Generated\FBObject.cs FBObject.tt
if not exist Generated\FBPhoto.cs TextTransform.exe -out Generated\FBPhoto.cs FBPhoto.tt
if not exist Generated\FBSuccess.cs TextTransform.exe -out Generated\FBSuccess.cs FBSuccess.tt
if not exist Generated\FBTestUser.cs TextTransform.exe -out Generated\FBTestUser.cs FBTestUser.tt

goto End

:NotFound
echo Could not find TextTransform, please add it to your PATH or have it available in %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\14.0 (VS2015) or %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\12.0 (VS2013)

:End