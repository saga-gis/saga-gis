@ECHO OFF

REM -----------------------------------
REM Make SAGA using MinGW...
REM -----------------------------------

REM SET MINGW=C:\dev-cpp

PATH=%PATH%;%MINGW%\bin


REM -----------------------------------
REM SAGA location...
REM -----------------------------------

REM SET SAGA=C:\saga_2


REM -----------------------------------
REM SAGA Core...
REM -----------------------------------

SET BIN_SAGA=%SAGA%\bin\saga_mingw
SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw

MD %SAGA%\bin
MD %SAGA%\bin\saga_mingw
MD %SAGA%\bin\tmp
MD %SAGA%\bin\tmp\saga_mingw

ECHO ###################################
ECHO SAGA CMD
ECHO -----------------------------------

SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw\saga_cmd\
SET TMP_SAGA=%OBJ_SAGA%
MD %OBJ_SAGA%

PUSHD %SAGA%\src\saga_core\saga_cmd
make -fmakefile.mingw
POPD

PAUSE
