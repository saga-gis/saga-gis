@ECHO OFF

REM ___________________________________
IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

IF "%SAGA_DIR_X64%" == "" (
	SET SAGA_DIR_X64=%SAGA_ROOT%\bin\saga_x64
)

SET SAGA_CMD="%SAGA_DIR_X64%\saga_cmd.exe"

REM ___________________________________
IF "%SAGA4QGIS%" == "" (
	SET SAGA4QGIS=saga4qgis.zip
)

SET OUTDIR=saga4qgis
IF EXIST "%OUTDIR%" RMDIR "%OUTDIR%" /S/Q
MKDIR "%OUTDIR%"

%SAGA_CMD% dev_tools 7 -DIRECTORY=%OUTDIR%

IF "%ZIP%" == "" (
	SET EXE_ZIP="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
) ELSE (
	SET EXE_ZIP="%ZIP%" a -r -y -mx5
)

%EXE_ZIP% "%SAGA4QGIS%" "%OUTDIR%"

RMDIR "%OUTDIR%" /S/Q

REM ___________________________________
REM PAUSE
