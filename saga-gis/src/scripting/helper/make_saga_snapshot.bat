@ECHO OFF

REM ___________________________________
SET SAGA_VERSION=saga_2.2.3_20151209
SET SAGA_ROOT=D:\saga\saga-code\trunk

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5

REM ___________________________________
REM MKDIR "%SAGA_VERSION%"
REM PUSHD "%SAGA_VERSION%"

REM ___________________________________
REM win32 Binaries
SET SAGA_CONFIG=win32
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y "%SAGA_ROOT%\saga-gis\bin\saga_vc_%SAGA_CONFIG%"
DEL /F saga_gui.cfg saga_gui.ini *.exp modules\*.exp modules\*.lib modules\dev_tools.*
RMDIR /S/Q _private
POPD

%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"

REM ___________________________________
REM x64 Binaries
SET SAGA_CONFIG=x64
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y "%SAGA_ROOT%\saga-gis\bin\saga_vc_%SAGA_CONFIG%"
DEL /F saga_gui.cfg saga_gui.ini *.exp modules\*.exp modules\*.lib modules\dev_tools.*
RMDIR /S/Q _private
POPD

%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"

REM ___________________________________
REM The End
REM POPD

REM PAUSE
