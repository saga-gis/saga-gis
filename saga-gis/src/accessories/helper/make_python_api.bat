@ECHO OFF

REM ___________________________________
REM SET SAGA_VERSION=saga-7.4.0

REM This batch script expects 4 arguments
REM 1. python version (2/3)
REM 2. architecture (win32/x64)
REM 3. output to zip (true/false)
REM 4. clean swig wrapper (true/false)

SET PYTHON_VERSION=%1
SET ARCHITECTURE=%2
SET MAKE_ZIP=%3
SET MAKE_CLEAN=%4

IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

REM ___________________________________
REM Tool paths, adjust to your system!

IF "%ZIP%" == "" (
	SET EXE_ZIP="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
) ELSE (
	SET EXE_ZIP="%ZIP%" a -r -y -mx5
)

IF "%SWIG%" == "" (
	SET EXE_SWIG="D:\develop\libs\swigwin-4.0.2\swig.exe"
) ELSE (
	SET EXE_SWIG="%SWIG%"
)

IF "%VARSALL%" == "" (
REM	SET "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
	SET EXE_VARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

IF "%PYTHONDIR%" == "" (
	SET PYTHONDIR=F:\develop\libs\Python\Python35_x64
)


REM ___________________________________
IF /i "%ARCHITECTURE%" == "win32" (
	SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_win32"

	REM VS2015 x86 x64 Cross Tools Command Prompt
	CALL %EXE_VARSALL%
	
) ELSE (
	SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_x64"

	REM VS2015 x86 x64 Cross Tools Command Prompt
	CALL %EXE_VARSALL% x86_amd64
	SET DISTUTILS_USE_SDK=1
	SET MSSDK=1
)


REM ___________________________________
REM Compiling SWIG / Python

ECHO __________________
ECHO ##################
ECHO Generating SAGA-Python-API...

PUSHD "%SAGA_ROOT%\src\saga_core\saga_api"

IF NOT EXIST saga_api_wrap.cxx (
	ECHO __________________
	ECHO SWIG Compilation...
	ECHO.

	%EXE_SWIG% -c++ -python -includeall -I. -D_SAGA_PYTHON saga_api.i
)

ECHO __________________
ECHO Python%PYTHON_VERSION% Compilation (%ARCHITECTURE%)...
ECHO.

"%PYTHONDIR%\python.exe" saga_api_to_python.py install

RMDIR /S/Q build

SET PYTHONPKG=%PYTHONDIR%\Lib\site-packages

COPY saga_api.py "%PYTHONPKG%\saga_api.py"

IF /i "%MAKE_CLEAN%" == "true" (
	DEL /F saga_api_wrap.cxx
	DEL /F saga_api.py
)

POPD


REM ___________________________________
REM Collecting files...

ECHO __________________
ECHO Collecting files...
ECHO.

SET PYTHONOUT=Python%PYTHON_VERSION%_%ARCHITECTURE%

IF EXIST "%PYTHONOUT%" (
	RMDIR /S/Q "%PYTHONOUT%"
)

XCOPY /C/Q/Y/H "%SAGA_ROOT%\src\accessories\python\examples\*.py" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
COPY "%SAGA_ROOT%\src\accessories\python\examples\test_all.bat" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
COPY "%SAGA_ROOT%\src\accessories\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"

COPY "%PYTHONPKG%\*saga_api*.pyd" "%PYTHONOUT%\Lib\site-packages\"
COPY "%PYTHONPKG%\*saga_api*.py"  "%PYTHONOUT%\Lib\site-packages\"

IF /i "%MAKE_ZIP%" == "true" (
	%EXE_ZIP% %SAGA_VERSION%_%ARCHITECTURE%_python%PYTHON_VERSION%.zip "%PYTHONOUT%"
	RMDIR /S/Q "%PYTHONOUT%"
)

ECHO __________________
ECHO ...finished!
ECHO.
ECHO.
ECHO.


REM ___________________________________
REM The End
