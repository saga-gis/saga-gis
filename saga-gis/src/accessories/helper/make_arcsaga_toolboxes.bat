@ECHO OFF

REM ___________________________________
IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

IF "%SAGA_DIR_X64%" == "" (
	SET SAGA_DIR_X64=%SAGA_ROOT%\bin\saga_x64
)

IF "%SAGA_DIR_WIN32%" == "" (
	SET SAGA_DIR_WIN32=%SAGA_ROOT%\bin_win32\saga_Win32
)

REM ___________________________________
IF /i "%ARC_VERSION%" == "0" (
	SET OUTDIR=ArcSAGA Toolboxes 10.1
	SET ARC_VERSION=0
) ELSE (
	SET OUTDIR=ArcSAGA Toolboxes
	SET ARC_VERSION=1
)

IF EXIST "%OUTDIR%" RMDIR "%OUTDIR%" /S/Q
MKDIR "%OUTDIR%"

COPY "%SAGA_ROOT%\src\tools\develop\dev_tools\ArcSAGA.py" "%OUTDIR%"
COPY "%SAGA_ROOT%\src\tools\develop\dev_tools\ArcSAGA_Grid.lyr" "%OUTDIR%\grid.lyr"

"%SAGA_DIR_X64%\saga_cmd.exe" dev_tools 5 -BOX_NAMING=1 -ARC_VERSION=%ARC_VERSION% -DIRECTORY="%OUTDIR%"

REM ___________________________________
IF EXIST "%SAGA_DIR_WIN32%" (
	IF EXIST "%SAGA_DIR_WIN32%\%OUTDIR%\" (
		RMDIR /S/Q "%SAGA_DIR_WIN32%\%OUTDIR%\"
	)
	XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_DIR_WIN32%\%OUTDIR%\"
)

IF EXIST "%SAGA_DIR_X64%" (
	IF EXIST "%SAGA_DIR_X64%\%OUTDIR%\" (
		RMDIR /S/Q "%SAGA_DIR_X64%\%OUTDIR%\"
	)
	XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_DIR_X64%\%OUTDIR%\"
)

RMDIR "%OUTDIR%" /S/Q

REM ___________________________________
REM PAUSE
