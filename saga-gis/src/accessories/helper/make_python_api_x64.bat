@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Build SAGA's Python API (64-bit)
ECHO #
ECHO ##################################
ECHO.

REM ___________________________________
REM VS2015 x86 x64 Cross Tools Command Prompt
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64

REM ___________________________________
SET SAGA_ROOT=%SAGA%
SET SAGA_VERSION=saga-6.5.0

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5

SET SWIGEXE="D:\libs\swigwin-3.0.7\swig.exe"

SET PYTHONVER=27
SET PYTHONDIR=D:\libs\Python\Python27_x64
SET DISTUTILS_USE_SDK=1
SET MSSDK=1

SET WXWINLIB="%WXWIN%\lib\vc_x64_dll"
SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_x64"

REM ___________________________________
REM SWIG/Python

PUSHD "%SAGA_ROOT%\src\saga_core\saga_api"
ECHO __________________
ECHO SWIG Compilation
%SWIGEXE% -c++ -python -includeall -I. -D_SAGA_PYTHON -D_SAGA_UNICODE saga_api.h

ECHO __________________
ECHO Python Compilation
"%PYTHONDIR%\python.exe" saga_api_to_python_win.py install

MOVE saga_api.py "%PYTHONDIR%\Lib\site-packages\saga_api.py"
DEL /F saga_api_wrap.cxx
RMDIR /S/Q build
POPD
SET PYTHONOUT=Python%PYTHONVER%
XCOPY /C/Q/Y/H "%PYTHONDIR%\Lib\site-packages\*saga*.*" "%PYTHONOUT%\Lib\site-packages\"
COPY "%SAGA_ROOT%\src\accessories\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"
XCOPY /C/Q/Y/H "%SAGA_ROOT%\src\accessories\python\examples" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
%ZIPEXE% %SAGA_VERSION%_x64_python%PYTHONVER%.zip "%PYTHONOUT%"
RMDIR /S/Q "%PYTHONOUT%"

REM ___________________________________
REM The End

PAUSE
