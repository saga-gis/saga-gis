@ECHO OFF

REM ___________________________________
REM ###################################
REM PRE-RELEASE STEPS
REM ###################################

REM ___________________________________
SET SAGA_VER_MAJOR=7
SET SAGA_VER_MINOR=3
SET SAGA_VER_RELEASE=0
SET SAGA_VER_TEXT=%SAGA_VER_MAJOR%.%SAGA_VER_MINOR%.%SAGA_VER_RELEASE%
SET SAGA_VERSION=saga-%SAGA_VER_TEXT%

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # MAKE SAGA RELEASE: %SAGA_VER_TEXT%
ECHO #
ECHO ##################################
ECHO.
ECHO Things you should have updated before:
ECHO - ArcSAGA Tools
ECHO - Translation Files
ECHO - Python Tools Interface Update
ECHO.
ECHO Enter 'y' to continue!
SET /P CONTINUE=
IF NOT '%CONTINUE%' == 'y' EXIT


REM ___________________________________
REM ###################################
REM MAKE RELEASE
REM ###################################

REM ___________________________________
SET SAGA_ROOT=%SAGA%

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
SET ISETUP="C:\Program Files (x86)\Inno Setup 5\ISCC.exe"
SET GITEXE=git
SET DOXEXE=doxygen.exe
SET SWIGEXE="D:\libs\swigwin-3.0.7\swig.exe"
SET PYTHONVER=27
SET PYTHONx32=D:\libs\Python\Python27_win32
SET PYTHONx64=D:\libs\Python\Python27_x64

REM ___________________________________
MKDIR "%SAGA_VERSION%"
PUSHD "%SAGA_VERSION%"


REM ___________________________________
REM ###################################
REM PACKAGE BINARIES
REM ###################################

REM ___________________________________
REM win32 Binaries
SET SAGA_CONFIG=win32
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\bin\saga_vc_%SAGA_CONFIG%"
DEL /F saga_gui.cfg *.ini *.exp *.pdb tools\*.exp tools\*.pdb tools\*.lib tools\dev_tools.*
RMDIR /S/Q _private
POPD
%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"

COPY "%SAGA_ROOT%\..\saga_setup_readme.rtf" "%SAGA_VERSION%_%SAGA_CONFIG%"
COPY "%SAGA_ROOT%\..\saga_setup_%SAGA_CONFIG%.iss" "%SAGA_VERSION%_%SAGA_CONFIG%"
%ISETUP% "%SAGA_VERSION%_%SAGA_CONFIG%\saga_setup_%SAGA_CONFIG%.iss"
MOVE "%SAGA_VERSION%_%SAGA_CONFIG%\%SAGA_VERSION%_%SAGA_CONFIG%_setup.exe"

RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"

REM ___________________________________
REM x64 Binaries
SET SAGA_CONFIG=x64
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\bin\saga_vc_%SAGA_CONFIG%"
DEL /F saga_gui.cfg *.ini *.exp *.pdb tools\*.exp tools\*.pdb tools\*.lib tools\dev_tools.*
RMDIR /S/Q _private
POPD
%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"

COPY "%SAGA_ROOT%\..\saga_setup_readme.rtf" "%SAGA_VERSION%_%SAGA_CONFIG%"
COPY "%SAGA_ROOT%\..\saga_setup_%SAGA_CONFIG%.iss" "%SAGA_VERSION%_%SAGA_CONFIG%"
%ISETUP% "%SAGA_VERSION%_%SAGA_CONFIG%\saga_setup_%SAGA_CONFIG%.iss"
MOVE "%SAGA_VERSION%_%SAGA_CONFIG%\%SAGA_VERSION%_%SAGA_CONFIG%_setup.exe"

RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"


REM ___________________________________
REM ###################################
REM SOURCE AND HELP
REM ###################################

REM ___________________________________
REM GIT Source Code Repository
%GITEXE% clone git://git.code.sf.net/p/saga-gis/code %SAGA_VERSION%_src -q
PUSHD %SAGA_VERSION%_src
REM Create a branch (better do manually)
REM %GITEXE% branch release-%SAGA_VER_TEXT%
%GITEXE% checkout release-%SAGA_VER_TEXT%
RMDIR /S/Q .git
POPD

REM ___________________________________
REM Source Code
%ZIPEXE% %SAGA_VERSION%_src.zip %SAGA_VERSION%_src

REM ___________________________________
REM Doxygen API Documentation
PUSHD %SAGA_VERSION%_src
%DOXEXE% saga_api_Doxyfile
POPD
%ZIPEXE% "%SAGA_VERSION%_api_doc.zip" "%SAGA_VERSION%_api_doc"
RMDIR /S/Q "%SAGA_VERSION%_api_doc"

REM ___________________________________
REM Drop Sources
RMDIR /S/Q %SAGA_VERSION%_src


REM ___________________________________
REM ###################################
REM PYTHON API
REM ###################################

REM ___________________________________
REM SWIG/Python (win32)
SET WXWINLIB="%WXWIN%\lib\vc_dll"
SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_Win32"

PUSHD "%SAGA_ROOT%\src\saga_core\saga_api"
%SWIGEXE% -c++ -python -includeall -I. -D_SAGA_PYTHON saga_api.h
"%PYTHONx32%\python.exe" saga_api_to_python_win.py install
MOVE saga_api.py "%PYTHONx32%\Lib\site-packages\saga_api.py"
DEL /F saga_api_wrap.cxx
RMDIR /S/Q build
POPD
SET PYTHONOUT=Python%PYTHONVER%
XCOPY /C/Q/Y/H "%PYTHONx32%\Lib\site-packages\*saga*.*" "%PYTHONOUT%\Lib\site-packages\"
COPY "%SAGA_ROOT%\src\accessories\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"
XCOPY /C/Q/Y/H "%SAGA_ROOT%\src\accessories\python\examples" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
%ZIPEXE% %SAGA_VERSION%_win32_python%PYTHONVER%.zip "%PYTHONOUT%"
RMDIR /S/Q "%PYTHONOUT%"


REM ___________________________________
REM SWIG/Python (x64)
SET DISTUTILS_USE_SDK=1
SET MSSDK=1
SET WXWINLIB="%WXWIN%\lib\vc_x64_dll"
SET SAGA_LIB="%SAGA_ROOT%\bin\saga_vc_x64"

PUSHD "%SAGA_ROOT%\src\saga_core\saga_api"
%SWIGEXE% -c++ -python -includeall -I. -D_SAGA_PYTHON saga_api.h
"%PYTHONx64%\python.exe" saga_api_to_python_win.py install
MOVE saga_api.py "%PYTHONx64%\Lib\site-packages\saga_api.py"
DEL /F saga_api_wrap.cxx
RMDIR /S/Q build
POPD
SET PYTHONOUT=Python%PYTHONVER%
XCOPY /C/Q/Y/H "%PYTHONx64%\Lib\site-packages\*saga*.*" "%PYTHONOUT%\Lib\site-packages\"
COPY "%SAGA_ROOT%\src\accessories\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"
XCOPY /C/Q/Y/H "%SAGA_ROOT%\src\accessories\python\examples" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
%ZIPEXE% %SAGA_VERSION%_x64_python%PYTHONVER%.zip "%PYTHONOUT%"
RMDIR /S/Q "%PYTHONOUT%"


REM ___________________________________
REM ###################################
REM END: MAKE RELEASE
REM ###################################

REM ___________________________________
POPD

REM ###################################
ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # What is left to do ?!
ECHO #
ECHO ##################################
ECHO.
ECHO - Don't forget to make the Linux tarball!
ECHO.    make dist
ECHO.
ECHO - Upload all release files
ECHO.    including an up-to-date 'readme.txt'
ECHO.
ECHO - Upload API Documentation to saga-gis.org
ECHO.
ECHO - Create SAGA Tools Reference Documentation
ECHO.    sagadoc-code: ./parse_modules.py
ECHO.    upload created version folder to saga-gis.org and update link
ECHO.
ECHO - Create new branch: release-%SAGA_VER_TEXT%
ECHO.
ECHO - Add new bug tracker milestone for next aspired version
ECHO.    https://sourceforge.net/p/saga-gis/bugs/milestones
ECHO.
ECHO - Update version numbers accordingly:
ECHO.    ./saga_setup_win32.iss
ECHO.    ./saga_setup_x64.iss
ECHO.    ./saga_api_Doxyfile
ECHO.    ./saga-gis/configure.ac
ECHO.    ./saga-gis/version.cmake
ECHO.    ./saga-gis/README
ECHO.    ./saga-gis/src/saga_core/saga_api/saga_api.h
ECHO.    ./saga-gis/src/accessories/helper/make_saga_release.bat (this file!)
ECHO.
ECHO - Commit a comment like: SAGA version updated to %SAGA_VER_TEXT%

PAUSE
