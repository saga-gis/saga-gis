@ECHO OFF

REM ___________________________________
SET SAGA_ROOT=%SAGA%

REM ___________________________________
SET OUTDIR=ArcSAGA Toolboxes

IF EXIST "%OUTDIR%" RMDIR "%OUTDIR%" /S/Q
MKDIR "%OUTDIR%"

COPY "%SAGA_ROOT%\src\dev_tools\ArcSAGA.py" "%OUTDIR%"
COPY "%SAGA_ROOT%\src\dev_tools\ArcSAGA_Grid.lyr" "%OUTDIR%\grid.lyr"

"%SAGA_ROOT%\bin\saga_vc_win32\saga_cmd.exe" dev_tools 5 -BOX_NAMING=1 -ARC_VERSION=1 -DIRECTORY="%OUTDIR%"

RMDIR /S/Q "%SAGA_ROOT%\bin\saga_vc_win32\%OUTDIR%\"
XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_ROOT%\bin\saga_vc_win32\%OUTDIR%\"
RMDIR /S/Q "%SAGA_ROOT%\bin\saga_vc_x64\%OUTDIR%\"
XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_ROOT%\bin\saga_vc_x64\%OUTDIR%\"

RMDIR "%OUTDIR%" /S/Q

REM ___________________________________
SET OUTDIR=ArcSAGA Toolboxes 10.1

IF EXIST "%OUTDIR%" RMDIR "%OUTDIR%" /S/Q
MKDIR "%OUTDIR%"

COPY "%SAGA_ROOT%\src\dev_tools\ArcSAGA.py" "%OUTDIR%"
COPY "%SAGA_ROOT%\src\dev_tools\ArcSAGA_Grid.lyr" "%OUTDIR%\grid.lyr"

"%SAGA_ROOT%\bin\saga_vc_win32\saga_cmd.exe" dev_tools 5 -BOX_NAMING=1 -ARC_VERSION=0 -DIRECTORY="%OUTDIR%"

RMDIR /S/Q "%SAGA_ROOT%\bin\saga_vc_win32\%OUTDIR%\"
XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_ROOT%\bin\saga_vc_win32\%OUTDIR%\"
RMDIR /S/Q "%SAGA_ROOT%\bin\saga_vc_x64\%OUTDIR%\"
XCOPY /C/Q/Y "%OUTDIR%" "%SAGA_ROOT%\bin\saga_vc_x64\%OUTDIR%\"

RMDIR "%OUTDIR%" /S/Q

REM ___________________________________
REM PAUSE