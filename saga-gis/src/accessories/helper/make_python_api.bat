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


REM ___________________________________
REM Tools

SET EXE_ZIP="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
SET EXE_SWIG="D:\libs\swigwin-4.0.1\swig.exe"


REM ___________________________________
SET SAGA_ROOT=%CD%\..\..\..

IF /i "%ARCHITECTURE%" == "win32" (
	SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_win32"

	REM VS2015 x86 x64 Cross Tools Command Prompt
	rem	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
	
) ELSE (
	SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_x64"

	REM VS2015 x86 x64 Cross Tools Command Prompt
	call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86_amd64
	SET DISTUTILS_USE_SDK=1
	SET MSSDK=1
)

IF "%PYTHON_VERSION%" == "2" (
	SET PYTHONVER=27
	IF /i "%ARCHITECTURE%" == "win32" (
		SET PYTHONDIR=D:\libs\Python\Python27_win32
		SET PYTHONPKG=D:\libs\Python\Python27_win32\Lib\site-packages
	) ELSE (
		SET PYTHONDIR=D:\libs\Python\Python27_x64
		SET PYTHONPKG=D:\libs\Python\Python27_x64\Lib\site-packages
	)	
) ELSE (
	SET PYTHONVER=35
	IF /i "%ARCHITECTURE%" == "win32" (
		SET PYTHONDIR=D:\libs\Python\Python35_win32\PCbuild\win32
		SET PYTHONPKG=D:\libs\Python\Python35_win32\Lib\site-packages
	) ELSE (
		SET PYTHONDIR=D:\libs\Python\Python35_x64\PCbuild\amd64
		SET PYTHONPKG=D:\libs\Python\Python35_x64\Lib\site-packages
	)	
)


REM ___________________________________
REM Compiling SWIG / Python

PUSHD "%SAGA_ROOT%\src\saga_core\saga_api"

IF NOT EXIST saga_api_wrap.cxx (
	ECHO __________________
	ECHO SWIG Compilation
	%EXE_SWIG% -c++ -python -includeall -I. -D_SAGA_PYTHON saga_api.h
)

ECHO __________________
ECHO Python Compilation
"%PYTHONDIR%\python.exe" saga_api_to_python_win.py install

RMDIR /S/Q build

COPY saga_api.py "%PYTHONPKG%\saga_api.py"

IF /i "%MAKE_CLEAN%" == "true" (
	DEL /F saga_api_wrap.cxx
	DEL /F saga_api.py
)

POPD


REM ___________________________________
REM Collecting files...

SET PYTHONOUT=Python%PYTHONVER%_%ARCHITECTURE%

RMDIR /S/Q "%PYTHONOUT%"

XCOPY /C/Q/Y/H "%SAGA_ROOT%\src\accessories\python\examples\*.py" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
COPY "%SAGA_ROOT%\src\accessories\python\examples\dem.sg-grd-z" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
COPY "%SAGA_ROOT%\src\accessories\python\examples\test_all.bat" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
COPY "%SAGA_ROOT%\src\accessories\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"

COPY "%PYTHONPKG%\*saga_api*.pyd" "%PYTHONOUT%\Lib\site-packages\"
COPY "%PYTHONPKG%\*saga_api*.py"  "%PYTHONOUT%\Lib\site-packages\"

IF /i "%MAKE_ZIP%" == "true" (
	%EXE_ZIP% %SAGA_VERSION%_%ARCHITECTURE%_python%PYTHONVER%.zip "%PYTHONOUT%"
	RMDIR /S/Q "%PYTHONOUT%"
)


REM ___________________________________
REM The End
