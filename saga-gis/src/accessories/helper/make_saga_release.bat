@ECHO OFF

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

PUSHD %~dp0

PUSHD %CD%\..\..\..
SET SAGA_ROOT=%CD%
POPD

IF "%SAGA_BIN%" == "" (
	SET SAGA_BIN=%SAGA_ROOT%\bin\saga_x64
)

REM ___________________________________
REM Version

REM For all bug-fix-releases!
REM Don't forget to activate the variable
REM - SWITCH_TO_BRANCH -
REM SET SWITCH_TO_BRANCH=saga-9.1
SET SAGA_VER_TEXT=9.2.0
SET SAGA_VER_NEXT=9.3.0
SET SAGA_VERSION=saga-%SAGA_VER_TEXT%


REM ___________________________________
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
SET /P ANSWER=
IF /i NOT '%ANSWER%' == 'y' EXIT


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
%GITEXE% clone git://git.code.sf.net/p/saga-gis/code %SAGA_VERSION%

PUSHD %SAGA_VERSION%

IF /i NOT "%SWITCH_TO_BRANCH%" == "" (
	ECHO switch to branch %SWITCH_TO_BRANCH%
	%GITEXE% checkout %SWITCH_TO_BRANCH%
)

RMDIR /S/Q .git
POPD

REM ___________________________________
REM Zip Source Code
%ZIPEXE% a -r -y -mx5 %SAGA_VERSION%_src.zip %SAGA_VERSION%

REM Create a tarball
%ZIPEXE% a -r -y -ttar %SAGA_VERSION%.tar %SAGA_VERSION%
%ZIPEXE% a -y -mx5 -tgzip %SAGA_VERSION%.tar.gz %SAGA_VERSION%.tar
DEL %SAGA_VERSION%.tar


REM ___________________________________
REM Drop Sources
RMDIR /S/Q %SAGA_VERSION%


REM ___________________________________
REM ###################################
REM Doxygen API Documentation
REM ###################################

%DOXEXE% ..\doxygen_saga_api_html"
%ZIPEXE% a -r -y -mx5 "%SAGA_VERSION%_api_doc.zip" "%SAGA_VERSION%_api_doc"

%DOXEXE% ..\doxygen_saga_api_chm"

RMDIR /S/Q "%SAGA_VERSION%_api_doc"


REM ___________________________________
REM ###################################
REM PYTHON API
REM ###################################

CMD /C CALL ..\make_python_api.bat install false 3.6  F:\develop\libs\Python\Python-3.6
CMD /C CALL ..\make_python_api.bat install false 3.7  F:\develop\libs\Python\Python-3.7
CMD /C CALL ..\make_python_api.bat install false 3.8  F:\develop\libs\Python\Python-3.8
CMD /C CALL ..\make_python_api.bat install false 3.9  F:\develop\libs\Python\Python-3.9
CMD /C CALL ..\make_python_api.bat install false 3.10 F:\develop\libs\Python\Python-3.10
CMD /C CALL ..\make_python_api.bat install true  3.11 F:\develop\libs\Python\Python-3.11


REM ___________________________________
REM ###################################
REM QSAGA Toolboxes
REM ###################################

SET SAGA4QGIS=saga4qgis.zip

CALL ..\make_saga4qgis_toolboxes.bat

MOVE %SAGA4QGIS% %SAGA_BIN%\%SAGA4QGIS%


REM ___________________________________
REM ###################################
REM ArcSAGA Toolboxes
REM ###################################

CALL ..\make_arcsaga_toolboxes.bat


REM ___________________________________
REM ###################################
REM BINARIES
REM ###################################

REM ___________________________________
REM collect files...

MKDIR "%SAGA_VERSION%_x64"
PUSHD "%SAGA_VERSION%_x64"
XCOPY /C/S/Q/Y/H "%SAGA_BIN%"
DEL /F *.ini *.cfg *.exp *.pdb *.tmp tools\*.exp tools\*.lib tools\*.pdb tools\dev_*.*
RMDIR /S/Q PySAGA\__pycache__
POPD

REM ___________________________________
REM create zip file...

%ZIPEXE% a -r -y -mx5 "%SAGA_VERSION%_x64.zip" "%SAGA_VERSION%_x64"

REM ___________________________________
REM create setup file...

ATTRIB -H "%SAGA_VERSION%_x64\*.*" /S /D
COPY "%SAGA_ROOT%\..\saga_setup_readme.rtf" "%SAGA_VERSION%_x64"
COPY "%SAGA_ROOT%\..\saga_setup_x64.iss" "%SAGA_VERSION%_x64"
%ISETUP% "%SAGA_VERSION%_x64\saga_setup_x64.iss"
MOVE "%SAGA_VERSION%_x64\%SAGA_VERSION%_x64_setup.exe"

REM ___________________________________
REM clean up...

RMDIR /S/Q "%SAGA_VERSION%_x64"


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
ECHO - Create new branch: saga-%SAGA_VER_TEXT%
ECHO.    and don't forget to activate the SWITCH_TO_BRANCH flag
ECHO.    for all bug-fix-releases!
ECHO.
ECHO - Update version numbers accordingly:
ECHO.    ./saga_setup_x64.iss
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
