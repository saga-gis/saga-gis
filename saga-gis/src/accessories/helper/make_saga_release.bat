@ECHO OFF

PUSHD %~dp0

REM ___________________________________
REM ###################################
REM PRE-RELEASE STEPS
REM ###################################

REM ___________________________________
REM Tools

SET ZIPEXE="C:\Program Files\7-Zip\7z.exe"
SET ISETUP="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
SET GITEXE="C:\Program Files\Git\bin\git.exe"
SET DOXEXE="C:\Program Files\doxygen\bin\doxygen.exe"

REM ___________________________________
REM Absolute Path to SAGA Root Directory

PUSHD %CD%\..\..\..
SET SAGA_ROOT=%CD%
POPD

IF "%SAGA_BIN%" == "" (
	SET SAGA_BIN=%SAGA_ROOT%\bin\saga_x64
)

REM ___________________________________
REM Version

SET SAGA_VERSION=9.3
SET SAGA_RELEASE=9.3.0
SET SAGA_VERSION_NEXT=9.4
SET SAGA_RELEASE_NEXT=9.4.0
SET SAGA_RELEASE_NAME=saga-%SAGA_RELEASE%

REM !!! For all bug-fix-releases !!!
REM Don't forget to activate the variable
REM - SWITCH_TO_BRANCH -
REM SET SWITCH_TO_BRANCH=saga-%SAGA_VERSION%


REM ___________________________________
ECHO __________________________________
ECHO ##################################
ECHO #                                #
ECHO #           B E G I N            #
ECHO #      with the preparation      #
ECHO #    of the release files for    #
ECHO #                                #
ECHO #          %SAGA_RELEASE_NAME%            #
ECHO #                                #
ECHO ##################################
ECHO __________________________________
ECHO Things you should have updated and committed before:
ECHO.  - Windows Build (including dev_tools.dll)
ECHO.  - Translation Files
ECHO.
ECHO Enter 'y' to continue!
SET /P ANSWER=
IF /i NOT '%ANSWER%' == 'y' EXIT


REM ___________________________________
REM ###################################
REM BEGIN: MAKE RELEASE
REM ###################################

CLS

MKDIR "%SAGA_RELEASE_NAME%"
PUSHD "%SAGA_RELEASE_NAME%"


REM ___________________________________
REM ###################################
REM SOURCE
REM ###################################

REM ___________________________________
REM GIT Source Code Repository
%GITEXE% clone git://git.code.sf.net/p/saga-gis/code %SAGA_RELEASE_NAME%

PUSHD %SAGA_RELEASE_NAME%

IF /i NOT "%SWITCH_TO_BRANCH%" == "" (
	ECHO switch to branch %SWITCH_TO_BRANCH%
	%GITEXE% checkout %SWITCH_TO_BRANCH%
)

RMDIR /S/Q .git
POPD

REM ___________________________________
REM Include the Python Toolboxes
CALL ..\make_python_toolboxes.bat "%SAGA_RELEASE_NAME%\saga-gis\src\accessories\python\tools"

REM ___________________________________
REM Zip Source Code
%ZIPEXE% a -r -y -mx5 %SAGA_RELEASE_NAME%_src.zip %SAGA_RELEASE_NAME%

REM Create a tarball
%ZIPEXE% a -r -y -ttar %SAGA_RELEASE_NAME%.tar %SAGA_RELEASE_NAME%
%ZIPEXE% a -y -mx5 -tgzip %SAGA_RELEASE_NAME%.tar.gz %SAGA_RELEASE_NAME%.tar
DEL %SAGA_RELEASE_NAME%.tar


REM ___________________________________
REM Drop Sources
RMDIR /S/Q %SAGA_RELEASE_NAME%


REM ___________________________________
REM ###################################
REM Doxygen API Documentation
REM ###################################

%DOXEXE% ..\doxygen_saga_api_html"
%ZIPEXE% a -r -y -mx5 "%SAGA_RELEASE_NAME%_api_doc.zip" "%SAGA_RELEASE_NAME%_api_doc"

%DOXEXE% ..\doxygen_saga_api_chm"

RMDIR /S/Q "%SAGA_RELEASE_NAME%_api_doc"


REM ___________________________________
REM ###################################
REM PySAGA
REM ###################################

RMDIR /S/Q %SAGA_BIN%\PySAGA

CMD /C CALL ..\make_python_api.bat install false 3.6  F:\develop\libs\Python\Python-3.6
CMD /C CALL ..\make_python_api.bat install false 3.7  F:\develop\libs\Python\Python-3.7
CMD /C CALL ..\make_python_api.bat install false 3.8  F:\develop\libs\Python\Python-3.8
CMD /C CALL ..\make_python_api.bat install false 3.9  F:\develop\libs\Python\Python-3.9
CMD /C CALL ..\make_python_api.bat install false 3.10 F:\develop\libs\Python\Python-3.10
CMD /C CALL ..\make_python_api.bat install true  3.11 F:\develop\libs\Python\Python-3.11
CMD /C CALL ..\make_python_api.bat install true  3.12 F:\develop\libs\Python\Python-3.12


REM ___________________________________
REM ###################################
REM PySAGA Toolboxes
REM ###################################

CALL ..\make_python_toolboxes.bat


REM ___________________________________
REM ###################################
REM ArcSAGA Toolboxes
REM ###################################

CALL ..\make_arcsaga_toolboxes.bat


REM ___________________________________
REM ###################################
REM QSAGA Toolboxes
REM ###################################

SET SAGA4QGIS=saga4qgis.zip
CALL ..\make_saga4qgis_toolboxes.bat
MOVE %SAGA4QGIS% %SAGA_BIN%\%SAGA4QGIS%


REM ___________________________________
REM ###################################
REM BINARIES
REM ###################################

REM ___________________________________
REM collect files...

MKDIR "%SAGA_RELEASE_NAME%_x64"
PUSHD "%SAGA_RELEASE_NAME%_x64"
XCOPY /C/S/Q/Y/H "%SAGA_BIN%"
DEL /F *.ini *.cfg *.exp *.pdb *.tmp tools\*.exp tools\*.lib tools\*.pdb tools\dev_*.*
POPD

REM ___________________________________
REM create zip file...

%ZIPEXE% a -r -y -mx5 "%SAGA_RELEASE_NAME%_x64.zip" "%SAGA_RELEASE_NAME%_x64"

REM ___________________________________
REM create setup file...

ATTRIB -H "%SAGA_RELEASE_NAME%_x64\*.*" /S /D
COPY "..\saga_setup_readme.rtf" "%SAGA_RELEASE_NAME%_x64"
COPY "..\saga_setup_x64.iss" "%SAGA_RELEASE_NAME%_x64"
%ISETUP% "%SAGA_RELEASE_NAME%_x64\saga_setup_x64.iss"
MOVE "%SAGA_RELEASE_NAME%_x64\%SAGA_RELEASE_NAME%_x64_setup.exe"

REM ___________________________________
REM clean up...

RMDIR /S/Q "%SAGA_RELEASE_NAME%_x64"


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
ECHO #          %SAGA_RELEASE_NAME%            #
ECHO #                                #
ECHO ##################################
ECHO __________________________________
ECHO What is left to do ?!
ECHO.
ECHO - Upload all release files
ECHO.    including an up-to-date 'readme.txt'
ECHO.
ECHO - Upload API Documentation to saga-gis.org
ECHO.    saga_api_doc (HTML)
ECHO.
IF /i "%SWITCH_TO_BRANCH%" == "" (
ECHO - Add new bug tracker milestone for next aspired version
ECHO     major.minor version without patch number: saga-%SAGA_VERSION_NEXT%
ECHO.    https://sourceforge.net/p/saga-gis/bugs/milestones
ECHO.
ECHO - Create new bug-fix-branch: saga-%SAGA_VERSION%
ECHO.    activate the SWITCH_TO_BRANCH flag for this branch!
ECHO.
)
ECHO - Create new tag: saga-%SAGA_RELEASE%
ECHO.
ECHO - Update version numbers accordingly:
ECHO.    ./saga-gis/version.cmake
ECHO.    ./saga-gis/src/saga_core/saga_api/saga_api.h
ECHO.    ./saga-gis/src/saga_core/saga_cmd/man/saga_cmd.1
ECHO.    ./saga-gis/src/saga_core/saga_gui/man/saga_gui.1
ECHO.    ./saga-gis/src/accessories/helper/doxygen_saga_api_chm
ECHO.    ./saga-gis/src/accessories/helper/doxygen_saga_api_html
ECHO.    ./saga-gis/src/accessories/helper/saga_setup_x64.iss
ECHO.    ./saga-gis/src/accessories/helper/make_saga_release.bat (this file!)
ECHO.  and commit: SAGA version updated to next aspired version %SAGA_RELEASE_NEXT%
ECHO.
ECHO - Create SAGA Tools Reference Documentation
ECHO.    sagadoc-code: ./parse_modules.py
ECHO.    upload created version folder to saga-gis.org and update link
ECHO.
ECHO - Commit a comment like: SAGA version updated to %SAGA_RELEASE%
ECHO.    https://sourceforge.net/p/saga-gis/news/
ECHO.
ECHO __________________________________

PAUSE
