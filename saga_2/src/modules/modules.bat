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
ECHO grid_analysis
ECHO -----------------------------------

PUSHD .\grid\grid_analysis
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_calculus
ECHO -----------------------------------

PUSHD .\grid\grid_calculus
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_discretisation
ECHO -----------------------------------

PUSHD .\grid\grid_discretisation
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_filter
ECHO -----------------------------------

PUSHD .\grid\grid_filter
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_gridding
ECHO -----------------------------------

PUSHD .\grid\grid_gridding
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_tools
ECHO -----------------------------------

PUSHD .\grid\grid_tools
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO grid_visualisation
ECHO -----------------------------------

PUSHD .\grid\grid_visualisation
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_grid
ECHO -----------------------------------

PUSHD .\shapes\shapes_grid
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_lines
ECHO -----------------------------------

PUSHD .\shapes\shapes_lines
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_points
ECHO -----------------------------------

PUSHD .\shapes\shapes_points
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_polygons
ECHO -----------------------------------

PUSHD .\shapes\shapes_polygons
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO shapes_tools
ECHO -----------------------------------

PUSHD .\shapes\shapes_tools
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO table_calculus
ECHO -----------------------------------

PUSHD .\table\table_calculus
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO table_tools
ECHO -----------------------------------

PUSHD .\table\table_tools
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO tin_tools
ECHO -----------------------------------

PUSHD .\tin\tin_tools
make -fmakefile.mingw
POPD

pause
