@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #                                #
ECHO #  Running SAGA through Python   #
ECHO #                                #
ECHO ##################################
ECHO.

PUSHD %~dp0

REM Adjust here the paths to your SAGA and Python installations:
SET PYTHONPATH=C:\saga-9.3.0_x64
SET PYTHON_EXE=C:\Python-3.11.1\python.exe

"%PYTHON_EXE%" saga_python_example.py

PAUSE
