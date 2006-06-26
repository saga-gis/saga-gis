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
ECHO io_esri_e00
ECHO -----------------------------------

PUSHD .\esri_e00\io_esri_e00
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_grid
ECHO -----------------------------------

PUSHD .\grid\io_grid
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_grid_image
ECHO -----------------------------------

PUSHD .\grid\io_grid_image
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_shapes
ECHO -----------------------------------

PUSHD .\shapes\io_shapes
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO io_table_odbc
ECHO -----------------------------------

PUSHD .\table\io_table_odbc
make -fmakefile.mingw
POPD

pause
