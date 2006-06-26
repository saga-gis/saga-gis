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
ECHO sim_cellular_automata
ECHO -----------------------------------

PUSHD .\cellular_automata\sim_cellular_automata
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO sim_ecosystems_hugget
ECHO -----------------------------------

PUSHD .\ecosystems\sim_ecosystems_hugget
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO sim_hydrology
ECHO -----------------------------------

PUSHD .\hydrology\sim_hydrology
make -fmakefile.mingw
POPD

pause
