@ECHO OFF

REM ___________________________________
SET SAGA_ROOT=D:\saga\saga-code\trunk\saga-gis
SET SAGA_CMD="%SAGA_ROOT%\bin\saga_vc_win32\saga_cmd.exe" -f=s

SET RESPATH=%SAGA_ROOT%\src\saga_core\saga_gui\res

REM ___________________________________
%SAGA_CMD% dev_tools 0 -TARGET="%RESPATH%\saga.lng.txt" -DIRECTORY="%SAGA_ROOT%\src"

%SAGA_CMD% dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.ger.txt" -UNION="%RESPATH%\saga.ger.txt" -METHOD=1

DEL "%RESPATH%\*.mtab"

REM ___________________________________
PAUSE