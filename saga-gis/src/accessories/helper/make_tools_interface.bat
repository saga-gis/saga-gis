@ECHO OFF

PUSHD %~dp0

REM ___________________________________
IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

IF "%SAGA_DIR_X64%" == "" (
	SET SAGA_DIR_X64=%SAGA_ROOT%\bin\saga_x64
)

REM ___________________________________
SET SAGA_CMD="%SAGA_DIR_X64%\saga_cmd.exe"

SET SAGA_TOOLS_PY=%SAGA_ROOT%\src\accessories\python\helpers\saga_cmd_param_interface\tools.py

REM ___________________________________
%SAGA_CMD% dev_tools 6 -INPUT="%SAGA_TOOLS_PY%" -OUTPUT="%SAGA_TOOLS_PY%"

REM ___________________________________
POPD
PAUSE
