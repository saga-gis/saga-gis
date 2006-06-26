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
ECHO ta_channels
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_channels
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_compound
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_compound
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_hydrology
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_hydrology
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_lighting
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_lighting
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_morphometry
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_morphometry
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_preprocessor
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_preprocessor
make -fmakefile.mingw
POPD

ECHO ###################################
ECHO ta_profiles
ECHO -----------------------------------

PUSHD .\terrain_analysis\ta_profiles
make -fmakefile.mingw
POPD

pause
