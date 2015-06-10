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
if not exist Generated\FBCursors.cpp TextTransform.exe -out Generated\FBCursors.cpp FBCursors.cpp.tt
if not exist Generated\FBGroup.cpp TextTransform.exe -out Generated\FBGroup.cpp FBGroup.cpp.tt
if not exist Generated\FBPage.cpp TextTransform.exe -out Generated\FBPage.cpp FBPage.cpp.tt
if not exist Generated\FBPageCategory.cpp TextTransform.exe -out Generated\FBPageCategory.cpp FBPageCategory.cpp.tt
if not exist Generated\FBPaging.cpp TextTransform.exe -out Generated\FBPaging.cpp FBPaging.cpp.tt
if not exist Generated\FBPermission.cpp TextTransform.exe -out Generated\FBPermission.cpp FBPermission.cpp.tt
if not exist Generated\FBUser.cpp TextTransform.exe -out Generated\FBUser.cpp FBUser.cpp.tt
if not exist Generated\FBObject.cpp TextTransform.exe -out Generated\FBObject.cpp FBObject.cpp.tt

if not exist Generated\FBCursors.h TextTransform.exe -out Generated\FBCursors.h FBCursors.h.tt
if not exist Generated\FBGroup.h TextTransform.exe -out Generated\FBGroup.h FBGroup.h.tt
if not exist Generated\FBPage.h TextTransform.exe -out Generated\FBPage.h FBPage.h.tt
if not exist Generated\FBPageCategory.h TextTransform.exe -out Generated\FBPageCategory.h FBPageCategory.h.tt
if not exist Generated\FBPaging.h TextTransform.exe -out Generated\FBPaging.h FBPaging.h.tt
if not exist Generated\FBPermission.h TextTransform.exe -out Generated\FBPermission.h FBPermission.h.tt
if not exist Generated\FBUser.h TextTransform.exe -out Generated\FBUser.h FBUser.h.tt
if not exist Generated\FBObject.h TextTransform.exe -out Generated\FBObject.h FBObject.h.tt

goto End

:NotFound
echo Could not find TextTransform, please add it to your PATH or have it available in %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\14.0 (VS2015) or %CommonProgramFiles(x86)%\Microsoft Shared\TextTemplating\12.0 (VS2013)

:End