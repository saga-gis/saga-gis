@ECHO OFF

REM ___________________________________
ECHO Enter version number:
SET /P VERSION=
ECHO.
ECHO Enter date of snapshot:
SET /P DATUM=

SET SAGA_VERSION=saga_%VERSION%_%DATUM%
SET SAGA_ROOT=%SAGA%

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5

REM ___________________________________
REM win32 Binaries
SET SAGA_CONFIG=win32
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\bin\saga_vc_%SAGA_CONFIG%"
DEL /F saga_gui.cfg saga_gui.ini *.exp tools\*.exp tools\*.lib tools\dev_tools.*
RMDIR /S/Q _private
POPD

%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"
RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"

REM ___________________________________
REM x64 Binaries
SET SAGA_CONFIG=x64
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\bin\saga_vc_%SAGA_CONFIG%"
DEL /F saga_gui.cfg saga_gui.ini *.exp tools\*.exp tools\*.lib tools\dev_tools.*
RMDIR /S/Q _private
POPD

%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"
RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"

REM ___________________________________
REM PAUSE
