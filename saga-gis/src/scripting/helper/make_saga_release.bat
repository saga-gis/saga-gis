@ECHO OFF

REM ___________________________________
SET SAGA_VERSION=saga_2.2.3
SET SAGA_ROOT=D:\saga\saga-code\trunk

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
SET ISETUP="C:\Program Files (x86)\Inno Setup 5\ISCC.exe"
SET SVNEXE=svn
SET SVNVER=2-2-3
SET DOXEXE=doxygen.exe
SET SWIGEXE="D:\libs\swigwin-3.0.7\swig.exe"
SET PYTHONDIR=D:\libs\Python-2.7
SET PYTHONVER=27

REM ___________________________________
MKDIR "%SAGA_VERSION%"
PUSHD "%SAGA_VERSION%"

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

COPY "%SAGA_ROOT%\saga_setup_readme.rtf" "%SAGA_VERSION%_%SAGA_CONFIG%"
COPY "%SAGA_ROOT%\saga_setup_%SAGA_CONFIG%.iss" "%SAGA_VERSION%_%SAGA_CONFIG%"
%ISETUP% "%SAGA_VERSION%_%SAGA_CONFIG%\saga_setup_%SAGA_CONFIG%.iss"
MOVE "%SAGA_VERSION%_%SAGA_CONFIG%\%SAGA_VERSION%_%SAGA_CONFIG%_setup.exe"

RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"

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

COPY "%SAGA_ROOT%\saga_setup_readme.rtf" "%SAGA_VERSION%_%SAGA_CONFIG%"
COPY "%SAGA_ROOT%\saga_setup_%SAGA_CONFIG%.iss" "%SAGA_VERSION%_%SAGA_CONFIG%"
%ISETUP% "%SAGA_VERSION%_%SAGA_CONFIG%\saga_setup_%SAGA_CONFIG%.iss"
MOVE "%SAGA_VERSION%_%SAGA_CONFIG%\%SAGA_VERSION%_%SAGA_CONFIG%_setup.exe"

RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"

REM ___________________________________
REM SVN Source Code Repository
%SVNEXE% checkout svn://svn.code.sf.net/p/saga-gis/code-0/trunk %SAGA_VERSION%_src -q --non-interactive
PUSHD %SAGA_VERSION%_src
RMDIR /S/Q .svn
POPD
%ZIPEXE% %SAGA_VERSION%_src.zip %SAGA_VERSION%_src

REM ___________________________________
REM Doxygen API Documentation
PUSHD %SAGA_VERSION%_src
%DOXEXE% saga_api_Doxyfile
POPD
%ZIPEXE% "%SAGA_VERSION%_api_doc.zip" "%SAGA_VERSION%_api_doc"
RMDIR /S/Q "%SAGA_VERSION%_api_doc"

REM ___________________________________
REM SWIG/Python (win32)
SET WXWINLIB="%WXWIN%\lib\vc_dll"
SET SAGA="%SAGA_ROOT%\saga-gis\bin\saga_vc_Win32"
PUSHD "%SAGA_ROOT%\saga-gis\src\saga_core\saga_api"
%SWIGEXE% -c++ -python -includeall -I. -D_SAGA_PYTHON -D_SAGA_UNICODE saga_api.h
"%PYTHONDIR%\python.exe" saga_api_to_python_win.py install
MOVE saga_api.py "%PYTHONDIR%\Lib\site-packages\saga_api.py"
DEL /F saga_api_wrap.cxx
RMDIR /S/Q build
POPD
SET PYTHONOUT=Python%PYTHONVER%
XCOPY /C/Q/Y "%PYTHONDIR%\Lib\site-packages\*saga*.*" "%PYTHONOUT%\Lib\site-packages\"
COPY "%SAGA_ROOT%\saga-gis\src\scripting\python\saga_python_api.txt" "%PYTHONOUT%\Lib\site-packages\"
XCOPY /C/Q/Y "%SAGA_ROOT%\saga-gis\src\scripting\python\examples" "%PYTHONOUT%\Lib\site-packages\saga_api_examples\"
%ZIPEXE% %SAGA_VERSION%_win32_python%PYTHONVER%.zip "%PYTHONOUT%"
RMDIR /S/Q "%PYTHONOUT%"

REM ___________________________________
REM The End
RMDIR /S/Q %SAGA_VERSION%_src
POPD

REM %SVNEXE% copy svn://svn.code.sf.net/p/saga-gis/code-0/trunk svn://svn.code.sf.net/p/saga-gis/code-0/branches/release-%SVNVER% -m "branch release-%SVNVER% created from trunk"
REM %SVNEXE% copy https://svn.code.sf.net/p/saga-gis/code-0/trunk https://svn.code.sf.net/p/saga-gis/code-0/branches/release-%SVNVER% -m "branch release-%SVNVER% created from trunk"

PAUSE
