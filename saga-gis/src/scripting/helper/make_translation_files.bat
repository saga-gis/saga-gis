@ECHO OFF

REM ___________________________________
SET SVNEXE=svn

SET SAGA_ROOT=D:\saga\saga-code\trunk\saga-gis
SET SAGA_CMD="%SAGA_ROOT%\bin\saga_vc_win32\saga_cmd.exe" -f=s
SET SAGA_SRC=saga_src

SET RESPATH=%SAGA_ROOT%\src\saga_core\saga_gui\res

REM ___________________________________
REM SVN Source Code Repository
%SVNEXE% checkout svn://svn.code.sf.net/p/saga-gis/code-0/trunk %SAGA_SRC% -q --non-interactive
PUSHD %SAGA_SRC%
RMDIR /S/Q .svn
POPD

REM ___________________________________
%SAGA_CMD% dev_tools 0 -TARGET="%RESPATH%\saga.lng.txt" -DIRECTORY="%SAGA_SRC%"

%SAGA_CMD% dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.ger.txt" -UNION="%RESPATH%\saga.ger.txt" -METHOD=1
%SAGA_CMD% dev_tools 1 -MASTER="%RESPATH%\saga.lng.txt" -IMPORT="%RESPATH%\saga.bra.txt" -UNION="%RESPATH%\saga.bra.txt" -METHOD=1

DEL "%RESPATH%\*.mtab"

RMDIR /S/Q %SAGA_SRC%

REM ___________________________________
PAUSE