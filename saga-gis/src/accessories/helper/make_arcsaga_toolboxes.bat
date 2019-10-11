@ECHO OFF

REM ___________________________________
SET SAGA_ROOT=%SAGA%

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

COPY "%SAGA_ROOT%\src\dev_tools\ArcSAGA.py" "%OUTDIR%"
COPY "%SAGA_ROOT%\src\dev_tools\ArcSAGA_Grid.lyr" "%OUTDIR%\grid.lyr"

"%SAGA_ROOT%\bin\saga_vc_win32\saga_cmd.exe" dev_tools 5 -BOX_NAMING=1 -ARC_VERSION=%ARC_VERSION% -DIRECTORY="%OUTDIR%"

RMDIR /S/Q "%SAGA_ROOT%\bin\saga_vc_win32\%OUTDIR%\"
XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_ROOT%\bin\saga_vc_win32\%OUTDIR%\"
RMDIR /S/Q "%SAGA_ROOT%\bin\saga_vc_x64\%OUTDIR%\"
XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_ROOT%\bin\saga_vc_x64\%OUTDIR%\"

RMDIR "%OUTDIR%" /S/Q

REM ___________________________________
REM PAUSE
