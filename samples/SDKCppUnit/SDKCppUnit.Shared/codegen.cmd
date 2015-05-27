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
if not exist Generated\TestUser.h TextTransform.exe -out Generated\TestUser.h TestUser.h.tt
if not exist Generated\TestUser.cpp TextTransform.exe -out Generated\TestUser.cpp TestUser.cpp.tt

if not exist Generated\APIResult.h TextTransform.exe -out Generated\APIResult.h APIResult.h.tt
if not exist Generated\APIResult.cpp TextTransform.exe -out Generated\APIResult.cpp APIResult.cpp.tt

if not exist Generated\TestPhoto.h TextTransform.exe -out Generated\TestPhoto.h TestPhoto.h.tt
if not exist Generated\TestPhoto.cpp TextTransform.exe -out Generated\TestPhoto.cpp TestPhoto.cpp.tt

goto End

:NotFound
echo Could not find TextTransform, please add it to your PATH or have it available in %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\14.0 (VS2015) or %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\12.0 (VS2013)


:End