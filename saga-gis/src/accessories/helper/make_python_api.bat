@ECHO OFF

REM ___________________________________
REM This batch script expects 4 arguments
REM 1. Python version suffix ('27', '35', ...)
REM 2. architecture (win32/x64)
REM 3. output to zip (true/false)
REM 4. clean swig wrapper (true/false)
REM 5. Python root directory

SET PYTHON_VERSION=%1
SET ARCHITECTURE=%2
SET MAKE_ZIP=%3
SET MAKE_CLEAN=%4
SET PYTHONDIR=%5


REM ___________________________________
REM File paths, adjusted to your system in the calling batch or take the defaults!

IF "%ZIP%" == "" (
	SET EXE_ZIP="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
) ELSE (
	SET EXE_ZIP="%ZIP%" a -r -y -mx5
)

IF "%SWIG%" == "" (
	SET EXE_SWIG="F:\develop\libs\swigwin-4.0.2\swig.exe"
) ELSE (
	SET EXE_SWIG="%SWIG%"
)

IF "%PYTHONDIR%" == "" (
	SET PYTHONDIR=F:\develop\libs\Python\Python39_x64
)

IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

IF "%SAGA_LIBDIR%" == "" (
	SET SAGA_LIBDIR=%SAGA_ROOT%\bin\saga_vc_%ARCHITECTURE%
)

IF "%VARSALL%" == "" (
REM	SET "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
	SET EXE_VARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
)


REM ___________________________________
REM Initialization of the MSVC environment
IF /i "%ARCHITECTURE%" == "win32" (
	REM VS2015 x86 x64 Cross Tools Command Prompt
	CALL %EXE_VARSALL% x86

) ELSE (
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

SET PYTHONPKG=%PYTHONDIR%\Lib\site-packages

REM Remove previous instances of saga-python-api
DEL "%PYTHONPKG%\*saga_api*.py*"
DEL "%PYTHONPKG%\*saga_*.egg-info"

REM Compilation
"%PYTHONDIR%\python.exe" saga_api_to_python.py install

REM Postprocessing jobs
RMDIR /S/Q build

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
	SETLOCAL EnableDelayedExpansion

	IF "%SAGA_VERSION%" == "" (
		SET SAGA_VERSION=saga-snapshot
	)

	%EXE_ZIP% !SAGA_VERSION!_%ARCHITECTURE%_python%PYTHON_VERSION%.zip "%PYTHONOUT%"
	RMDIR /S/Q "%PYTHONOUT%"
)

ECHO __________________
ECHO ...finished!
ECHO.
ECHO.
ECHO.

EXIT

REM ___________________________________
REM The End
