@ECHO OFF

REM ___________________________________
REM ###################################
REM PRE-RELEASE STEPS
REM ###################################

REM ___________________________________
REM Tools

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
SET ISETUP="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
SET GITEXE="C:\Program Files\Git\bin\git.exe"
SET DOXEXE="C:\Program Files\doxygen\bin\doxygen.exe"

REM ___________________________________
REM Absolute Path to SAGA Root Directory

PUSHD %CD%\..\..\..
SET SAGA_ROOT=%CD%
POPD

REM ___________________________________
REM Version

SET SAGA_VER_TEXT=8.0.0
SET SAGA_VER_NEXT=8.1.0
SET SAGA_VERSION=saga-%SAGA_VER_TEXT%
REM SET SWITCH_TO_BRANCH=true

ECHO __________________________________
ECHO ##################################
ECHO #                                #
ECHO #           B E G I N            #
ECHO #      with the preparation      #
ECHO #    of the release files for    #
ECHO #                                #
ECHO #          SAGA %SAGA_VER_TEXT%            #
ECHO #                                #
ECHO ##################################
ECHO __________________________________
ECHO Things you should have updated and committed before:
ECHO.  - dev_tools.dll (x64)
ECHO.  - Translation Files
ECHO.  - Tools Interface (Python)
ECHO.
ECHO Enter 'y' to continue!
SET /P CONTINUE=
IF NOT '%CONTINUE%' == 'y' EXIT


REM ___________________________________
REM ###################################
REM BEGIN: MAKE RELEASE
REM ###################################

CLS

MKDIR "%SAGA_VERSION%"
PUSHD "%SAGA_VERSION%"


REM ___________________________________
REM ###################################
REM SOURCE
REM ###################################

REM ___________________________________
REM GIT Source Code Repository
%GITEXE% clone git://git.code.sf.net/p/saga-gis/code %SAGA_VERSION%_src

PUSHD %SAGA_VERSION%_src

REM Create a branch (better do manually)
REM %GITEXE% branch release-%SAGA_VER_TEXT%

IF /i "%SWITCH_TO_BRANCH%" == "true" (
	%GITEXE% checkout release-%SAGA_VER_TEXT%
)

RMDIR /S/Q .git
POPD

REM ___________________________________
REM Zip Source Code
%ZIPEXE% %SAGA_VERSION%_src.zip %SAGA_VERSION%_src

REM ___________________________________
REM Drop Sources
RMDIR /S/Q %SAGA_VERSION%_src


REM ___________________________________
REM ###################################
REM BINARIES
REM ###################################

REM ___________________________________
SET SAGA4QGIS=saga4qgis.zip

CALL ..\make_saga4qgis_toolboxes.bat

CALL ..\make_arcsaga_toolboxes.bat

REM ___________________________________
REM win32 Binaries
SET SAGA_CONFIG=win32
MKDIR "%SAGA_VERSION%_%SAGA_CONFIG%"
PUSHD "%SAGA_VERSION%_%SAGA_CONFIG%"
XCOPY /C/S/Q/Y/H "%SAGA_ROOT%\bin\saga_vc_%SAGA_CONFIG%"
DEL /F *.ini *.cfg *.exp *.pdb *.tmp tools\*.exp tools\*.lib tools\*.pdb tools\dev_tools.*
COPY ..\%SAGA4QGIS% .\%SAGA4QGIS%
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
DEL /F *.ini *.cfg *.exp *.pdb *.tmp tools\*.exp tools\*.lib tools\*.pdb tools\dev_tools.*
COPY ..\%SAGA4QGIS% .\%SAGA4QGIS%
POPD
%ZIPEXE% "%SAGA_VERSION%_%SAGA_CONFIG%.zip" "%SAGA_VERSION%_%SAGA_CONFIG%"

COPY "%SAGA_ROOT%\..\saga_setup_readme.rtf" "%SAGA_VERSION%_%SAGA_CONFIG%"
COPY "%SAGA_ROOT%\..\saga_setup_%SAGA_CONFIG%.iss" "%SAGA_VERSION%_%SAGA_CONFIG%"
%ISETUP% "%SAGA_VERSION%_%SAGA_CONFIG%\saga_setup_%SAGA_CONFIG%.iss"
MOVE "%SAGA_VERSION%_%SAGA_CONFIG%\%SAGA_VERSION%_%SAGA_CONFIG%_setup.exe"

RMDIR /S/Q "%SAGA_VERSION%_%SAGA_CONFIG%"

DEL /F %SAGA4QGIS%


REM ___________________________________
REM ###################################
REM Doxygen API Documentation
REM ###################################

%DOXEXE% ..\doxygen_saga_api_html"
%ZIPEXE% "%SAGA_VERSION%_api_doc.zip" "%SAGA_VERSION%_api_doc"

%DOXEXE% ..\doxygen_saga_api_chm"

RMDIR /S/Q "%SAGA_VERSION%_api_doc"


REM ___________________________________
REM ###################################
REM PYTHON API
REM ###################################

SET PYTHONDIR=F:\develop\libs\Python\Python27_win32
CALL ..\make_python_api.bat 27 win32 true false

SET PYTHONDIR=F:\develop\libs\Python\Python27_x64
CALL ..\make_python_api.bat 27 x64 true false

SET PYTHONDIR=F:\develop\libs\Python\Python35_win32
CALL ..\make_python_api.bat 35 win32 true false

SET PYTHONDIR=F:\develop\libs\Python\Python35_x64
CALL ..\make_python_api.bat 35 x64 true true


REM ___________________________________
REM ###################################
REM END: MAKE RELEASE
REM ###################################

POPD

CLS

ECHO __________________________________
ECHO ##################################
ECHO #                                #
ECHO #        F I N I S H E D         #
ECHO #      with the preparation      #
ECHO #    of the release files for    #
ECHO #                                #
ECHO #          SAGA %SAGA_VER_TEXT%            #
ECHO #                                #
ECHO ##################################
ECHO __________________________________
ECHO What is left to do ?!
ECHO.
ECHO - Don't forget to make the Linux tarball!
ECHO.    make dist
ECHO.
ECHO - Upload all release files
ECHO.    including an up-to-date 'readme.txt'
ECHO.
ECHO - Upload API Documentation to saga-gis.org
ECHO.
ECHO - Create new branch: release-%SAGA_VER_NEXT%
ECHO.
ECHO - Update version numbers accordingly:
ECHO.    ./saga_setup_x64.iss
ECHO.    ./saga_setup_win32.iss
ECHO.    ./saga-gis/configure.ac
ECHO.    ./saga-gis/version.cmake
ECHO.    ./saga-gis/src/saga_core/saga_api/saga_api.h
ECHO.    ./saga-gis/src/saga_core/saga_cmd/man/saga_cmd.1
ECHO.    ./saga-gis/src/saga_core/saga_gui/man/saga_gui.1
ECHO.    ./saga-gis/src/accessories/helper/doxygen_saga_api_chm
ECHO.    ./saga-gis/src/accessories/helper/doxygen_saga_api_html
ECHO.    ./saga-gis/src/accessories/helper/make_saga_release.bat (this file!)
ECHO.  and commit: SAGA version updated to next aspired version %SAGA_VER_NEXT%
ECHO.
ECHO - Create SAGA Tools Reference Documentation
ECHO.    sagadoc-code: ./parse_modules.py
ECHO.    upload created version folder to saga-gis.org and update link
ECHO.
ECHO - Add new bug tracker milestone for next aspired version
ECHO.    https://sourceforge.net/p/saga-gis/bugs/milestones
ECHO.
ECHO - Commit a comment like: SAGA version updated to %SAGA_VER_TEXT%
ECHO.    https://sourceforge.net/p/saga-gis/news/
ECHO.
ECHO __________________________________

PAUSE
