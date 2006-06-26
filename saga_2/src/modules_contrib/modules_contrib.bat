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
ECHO contrib_t_wutzler
ECHO -----------------------------------

PUSHD .\contrib_t_wutzler\contrib_t_wutzler
make -fmakefile.mingw
POPD

pause
