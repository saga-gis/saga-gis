@ECHO OFF

REM ___________________________________
IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

SET SAGA_CMD=%SAGA_ROOT%\bin\saga_x64\saga_cmd.exe

REM ___________________________________
SET OUTDIR=%1

IF "%OUTDIR%" == "" (
	SET OUTDIR=%SAGA_ROOT%\bin\saga_x64\PySAGA\tools
)

IF EXIST "%OUTDIR%" RMDIR "%OUTDIR%" /S/Q

REM ___________________________________
"%SAGA_CMD%" dev_tools 9 -TARGET=1 -FOLDER="%OUTDIR%" -NAMING=0 -CLEAN=1

REM ___________________________________
REM PAUSE
