@ECHO OFF

PUSHD %~dp0

REM ___________________________________
IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

IF "%SAGA_DIR_X64%" == "" (
	SET SAGA_DIR_X64=%SAGA_ROOT%\bin\build_x64\saga_x64
)

IF "%GITEXE%" == "" (
	SET GITEXE=git
)

REM ___________________________________
REM GIT Source Code Repository
SET SAGA_SRC=saga_src
%GITEXE% clone git://git.code.sf.net/p/saga-gis/code %SAGA_SRC%
PUSHD %SAGA_SRC%
RMDIR /S/Q .git
POPD

REM ___________________________________
SET SAGA_CMD="%SAGA_DIR_X64%\saga_cmd.exe"
SET RESPATH=%SAGA_ROOT%\src\saga_core\saga_gui\res

%SAGA_CMD% -f=q dev_tools 0 -TARGET="%RESPATH%\saga.lng.txt" -DIRECTORY="%SAGA_SRC%"

%SAGA_CMD% -f=s dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.ger.txt" -UNION="%RESPATH%\saga.ger.txt" -METHOD=1
%SAGA_CMD% -f=s dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.bra.txt" -UNION="%RESPATH%\saga.bra.txt" -METHOD=1
%SAGA_CMD% -f=s dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.rus.txt" -UNION="%RESPATH%\saga.rus.txt" -METHOD=1

DEL "%RESPATH%\*.mtab"

RMDIR /S/Q %SAGA_SRC%

REM ___________________________________
POPD
PAUSE
