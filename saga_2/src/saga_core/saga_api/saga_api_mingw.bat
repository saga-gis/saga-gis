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
ECHO SAGA API
ECHO -----------------------------------

SET OBJ_SAGA=%SAGA%\bin\tmp\saga_mingw\saga_api\
SET TMP_SAGA=
MD %OBJ_SAGA%

PUSHD %SAGA%\src\saga_core\saga_api
MOVE %OBJ_SAGA%*.o %TMP_SAGA%
make -fmakefile.mingw
MOVE %TMP_SAGA%*.o %OBJ_SAGA%
POPD

PAUSE
