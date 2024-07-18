@ECHO OFF

PUSHD %~dp0

REM ___________________________________
SET PYTHON="F:\develop\libs\Python\Python-3.12\python.exe"

%PYTHON% .\create_python_toolboxes.py %1

REM ___________________________________
REM PAUSE
