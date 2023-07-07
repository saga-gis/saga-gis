@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Compiling SAGA's Python API
ECHO #
ECHO ##################################
ECHO.

PUSHD %~dp0

REM SET SAGA_ROOT=%CD%\..\..\..
REM SET SAGA_BIN=%SAGA_ROOT%\bin\saga_64
REM SET SAGA_VERSION=saga-major.minor.release
REM SET ZIP=C:\Program Files\7-Zip\7z.exe
REM SET SWIG=F:\develop\libs\swigwin-4.0.2\swig.exe
SET PYTHONS=F:\develop\libs\Python

CMD /C CALL make_python_api.bat update false 3.6  %PYTHONS%\Python-3.6
CMD /C CALL make_python_api.bat update false 3.7  %PYTHONS%\Python-3.7
CMD /C CALL make_python_api.bat update false 3.8  %PYTHONS%\Python-3.8
CMD /C CALL make_python_api.bat update false 3.9  %PYTHONS%\Python-3.9
CMD /C CALL make_python_api.bat update false 3.10 %PYTHONS%\Python-3.10
CMD /C CALL make_python_api.bat update true  3.11 %PYTHONS%\Python-3.11

PAUSE
