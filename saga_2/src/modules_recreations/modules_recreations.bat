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
ECHO recreations_fractals
ECHO -----------------------------------

PUSHD .\recreations\recreations_fractals
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO recreations_games
ECHO -----------------------------------

PUSHD .\recreations\recreations_games
make -fmakefile.mingw
POPD

pause
