@ECHO OFF

REM ___________________________________
REM ###################################
REM    Make SAGA-API Documentation
REM ###################################

REM ___________________________________
SET ZIPEXE="C:\Program Files\7-Zip\7z.exe" a -r -y -mx5
SET DOXEXE="C:\Program Files\doxygen\bin\doxygen.exe"

SET SAGA_VERSION=saga-api-doc

MKDIR "%SAGA_VERSION%"
PUSHD "%SAGA_VERSION%"

%DOXEXE% ..\doxygen_saga_api_html"
%ZIPEXE% "%SAGA_VERSION%_api_doc.zip" "%SAGA_VERSION%_api_doc"

%DOXEXE% ..\doxygen_saga_api_chm"

RMDIR /S/Q "%SAGA_VERSION%_api_doc"
POPD

PAUSE
