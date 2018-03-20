@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Build SAGA's Python API
ECHO #
ECHO ##################################
ECHO.

REM ___________________________________
SET SAGA_ROOT=%SAGA%

SET SWIGEXE="D:\libs\swigwin-3.0.7\swig.exe"
SET PYTHONDIR=D:\libs\Python-2.7
SET PYTHONVER=27

REM ___________________________________
REM SWIG/Python (win32)
SET WXWINLIB="%WXWIN%\lib\vc_dll"
SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_Win32"
PUSHD "%SAGA_ROOT%\src\saga_core\saga_api"
%SWIGEXE% -c++ -python -includeall -I. -D_SAGA_PYTHON -D_SAGA_UNICODE saga_api.h
"%PYTHONDIR%\python.exe" saga_api_to_python_win.py install
MOVE saga_api.py "%PYTHONDIR%\Lib\site-packages\saga_api.py"
DEL /F saga_api_wrap.cxx
RMDIR /S/Q build
POPD
SET PYTHONOUT=Python%PYTHONVER%
XCOPY /C/Q/Y/H "%PYTHONDIR%\Lib\site-packages\*saga*.*" "%PYTHONOUT%\Lib\site-packages\"
COPY "%SAGA_ROOT%\src\accessories\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"
XCOPY /C/Q/Y/H "%SAGA_ROOT%\src\accessories\python\examples" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
REM %ZIPEXE% %SAGA_VERSION%_win32_python%PYTHONVER%.zip "%PYTHONOUT%"
REM RMDIR /S/Q "%PYTHONOUT%"

REM ___________________________________
REM The End

PAUSE
