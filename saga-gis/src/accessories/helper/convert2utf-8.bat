@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #                                #
ECHO #  UTF-8 Source File Conversion  #
ECHO #                                #
ECHO ##################################
ECHO.

PUSHD %~dp0

REM Adjust here the paths to your SAGA and Python installations:

SET SAGA_PATH=F:\develop\saga\saga-code\master\saga-gis\bin\saga_x64
SET PYTHON_EXE=F:\develop\libs\Python\Python-3.12\python.exe

%PYTHON_EXE% convert2utf-8.py

PAUSE
