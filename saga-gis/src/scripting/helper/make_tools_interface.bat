@ECHO OFF

REM ___________________________________
SET SAGA_ROOT=D:\saga\saga-code\trunk
SET SAGA_CMD="%SAGA_ROOT%\saga-gis\bin\saga_vc_Win32\saga_cmd.exe"
SET SAGA_TOOLS_PY=%SAGA_ROOT%\saga-gis\src\scripting\python\helpers\saga_cmd_param_interface\tools.py

REM ___________________________________
%SAGA_CMD% dev_tools 6 -INPUT="%SAGA_TOOLS_PY%" -OUTPUT="%SAGA_TOOLS_PY%"

REM ___________________________________
PAUSE
