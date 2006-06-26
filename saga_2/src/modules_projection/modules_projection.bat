@ECHO OFF

REM -----------------------------------
REM DevCpp resp. root location of MinGW...
REM -----------------------------------

REM SET MINGW=C:\dev-cpp

PATH=%PATH%;%MINGW%\bin


REM -----------------------------------
REM SAGA location...
REM -----------------------------------

REM SET SAGA=C:\saga_2


REM -----------------------------------
REM Module Libraries...
REM -----------------------------------

ECHO ###################################
ECHO pj_georeference
ECHO -----------------------------------

PUSHD .\pj_georeference\pj_georeference
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO pj_geotrans
ECHO -----------------------------------

PUSHD .\pj_geotrans\pj_geotrans
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO pj_proj4
ECHO -----------------------------------

PUSHD .\pj_proj4\pj_proj4
make -fmakefile.mingw
POPD

pause
