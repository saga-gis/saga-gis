@ECHO OFF

REM ___________________________________
REM ###################################
REM This batch script expects 5 arguments
REM 1. output ('zip', 'install', 'update')
REM 2. clean swig wrapper ('true', 'false')
REM 3. Python version suffix (e.g. '3.11')
REM 4. Python root directory
REM 5. architecture ('win32', defaults to 'x64')

SETLOCAL EnableDelayedExpansion

SET OUTPUT=%1
SET MAKE_CLEAN=%2
SET PYTHON_VERSION=%3
SET PYTHON_DIR=%4
SET ARCHITECTURE=%5


REM ___________________________________
REM ###################################
REM File paths, adjusted to your system in the calling batch or take the defaults!

IF "%SWIG%" == "" (
	SET SWIG="F:\develop\libs\swigwin-4.0.2\swig.exe"
)

IF NOT EXIST "%SWIG%" (
	ECHO ERROR: swig executable not found ("%SWIG%")
)

IF "%PYTHON_DIR%" == "" (
	SET PYTHON_DIR=F:\develop\libs\Python\Python-3.11
)

IF NOT EXIST "%PYTHON_DIR%\python.exe" (
	ECHO ERROR: Python executable not found ("%PYTHON_DIR%")
)

SET PYTHON_PKG=%PYTHON_DIR%\Lib\site-packages

IF "%SAGA_ROOT%" == "" (
	SET SAGA_ROOT=%CD%\..\..\..
)

IF "%SAGA_BIN%" == "" (
	IF /i "%ARCHITECTURE%" == "win32" (
		SET SAGA_BIN=%SAGA_ROOT%\bin_win32\saga_%ARCHITECTURE%
	) ELSE (
		SET ARCHITECTURE=x64
		SET SAGA_BIN=%SAGA_ROOT%\bin\saga_x64
	)
)


REM ___________________________________
REM ###################################
REM Initialization of the MSVC environment

IF "%VARSALL%" == "" (
REM	SET "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
	SET EXE_VARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

IF /i "%ARCHITECTURE%" == "win32" (
	REM VS2015 x86 x64 Cross Tools Command Prompt
	CALL %EXE_VARSALL% x86
	SET PYTHONEGG=%PYTHON_PKG%\SAGA_Python_API-1.0-py%PYTHON_VERSION%-win-win32.egg

) ELSE (
	REM VS2015 x86 x64 Cross Tools Command Prompt
	CALL %EXE_VARSALL% x86_amd64
	SET DISTUTILS_USE_SDK=1
	SET MSSDK=1
	SET PYTHONEGG=%PYTHON_PKG%\SAGA_Python_API-1.0-py%PYTHON_VERSION%-win-amd64.egg
)


REM ___________________________________
REM ###################################
REM Compiling SWIG / Python

ECHO __________________
ECHO ##################
ECHO Generating the SAGA Python-API...

SET SAGA_LIBDIR=%SAGA_BIN%
SET SAGA_LIBSRC=%SAGA_ROOT%\src\saga_core\saga_api

PUSHD "%SAGA_LIBSRC%"

IF NOT EXIST saga_api_wrap.cxx (
	ECHO __________________
	ECHO SWIG Compilation...
	ECHO.

	"%SWIG%" -c++ -python -includeall -I. -D_SAGA_PYTHON saga_api.i
)

ECHO __________________
ECHO Python%PYTHON_VERSION% Compilation (%ARCHITECTURE%)...
ECHO.

rem "%PYTHON_DIR%\python.exe" saga_api_to_python.py install
"%PYTHON_DIR%\python.exe" saga_api_to_python.py build_ext --inplace

POPD


REM ___________________________________
REM ###################################
REM Collecting files...

REM ___________________________________
REM update files in Python's \Lib\site-packages directory...
IF /i "%OUTPUT%" == "update" (
	PUSHD "%PYTHON_PKG%"

	REM remove previous instances of the SAGA Python-API
	DEL *saga_*.py*"
	DEL *saga_*.egg-info"
	IF EXIST "%PYTHONEGG%" (
		RMDIR /S/Q "%PYTHONEGG%"
	)
	IF EXIST PySAGA (
		RMDIR /S/Q PySAGA
	)
	MKDIR PySAGA
	PUSHD PySAGA
	COPY "%SAGA_LIBSRC%\saga_api.py"
	COPY "%SAGA_LIBSRC%\_saga_api*.pyd"
	XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\src\accessories\python"
	POPD

	POPD
)

REM ___________________________________
REM install files to SAGA PySAGA directory...
IF /i "%OUTPUT%" == "install" (
	IF NOT EXIST "%SAGA_BIN%\PySAGA" (
		MKDIR "%SAGA_BIN%\PySAGA"
	)
	PUSHD "%SAGA_BIN%\PySAGA"
	COPY "%SAGA_LIBSRC%\saga_api.py"
	COPY "%SAGA_LIBSRC%\_saga_api*.pyd"
	XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\src\accessories\python"
	POPD
)

REM ___________________________________
REM zipping files...
IF /i "%OUTPUT%" == "zip" (
	SET TARGET=Python%PYTHON_VERSION%_%ARCHITECTURE%
	IF EXIST "!TARGET!" (
		RMDIR /S/Q "!TARGET!"
	)
	MKDIR "!TARGET!"
	PUSHD "!TARGET!"

	MKDIR PySAGA
	PUSHD PySAGA
	COPY "%SAGA_LIBSRC%\saga_api.py"
	COPY "%SAGA_LIBSRC%\_saga_api*.pyd"
	XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\src\accessories\python"
	POPD

	POPD

	IF "%ZIP%" == "" (
		SET ZIP=C:\Program Files\7-Zip\7z.exe
	)

	IF "%SAGA_VERSION%" == "" (
		SET SAGA_VERSION=saga-major.minor.release
	)

	"!ZIP!" a -r -y -mx5 !SAGA_VERSION!_%ARCHITECTURE%_python%PYTHON_VERSION%.zip "!TARGET!"

	RMDIR /S/Q "!TARGET!"
)


REM ___________________________________
REM ###################################
REM clean up...

PUSHD "%SAGA_LIBSRC%"

RMDIR /S/Q build

DEL /F _saga_api*.pyd

IF EXIST "SAGA_Python_API.egg-info" (
	RMDIR /S/Q "SAGA_Python_API.egg-info"
)

IF EXIST "dist" (
	RMDIR /S/Q "dist"
)

IF /i "%MAKE_CLEAN%" == "true" (
	DEL /F saga_api_wrap.cxx
	DEL /F saga_api.py
)

POPD

ECHO __________________________________
ECHO ...finished!
ECHO.
ECHO.
ECHO.

EXIT

REM ___________________________________
REM The End
