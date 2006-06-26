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
ECHO geostatistics_grid
ECHO -----------------------------------

PUSHD .\geostatistics\geostatistics_grid
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO geostatistics_kriging
ECHO -----------------------------------

PUSHD .\geostatistics\geostatistics_kriging
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO geostatistics_points
ECHO -----------------------------------

PUSHD .\geostatistics\geostatistics_points
make -fmakefile.mingw
POPD

pause
