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
REM SET SAGA_LIBDIR=%SAGA_ROOT%\bin\saga_64
REM SET SAGA_VERSION=saga-major.minor.release
REM SET ZIP=C:\Program Files\7-Zip\7z.exe
REM SET SWIG=F:\develop\libs\swigwin-4.0.2\swig.exe

CMD /C CALL make_python_api.bat 2.7  win32 true false F:\develop\libs\Python\_win32\Python-2.7
CMD /C CALL make_python_api.bat 3.6  win32 true false F:\develop\libs\Python\_win32\Python-3.6
CMD /C CALL make_python_api.bat 3.7  win32 true false F:\develop\libs\Python\_win32\Python-3.7
CMD /C CALL make_python_api.bat 3.8  win32 true false F:\develop\libs\Python\_win32\Python-3.8
CMD /C CALL make_python_api.bat 3.9  win32 true false F:\develop\libs\Python\_win32\Python-3.9
CMD /C CALL make_python_api.bat 3.10 win32 true false F:\develop\libs\Python\_win32\Python-3.10
CMD /C CALL make_python_api.bat 3.11 win32 true false F:\develop\libs\Python\_win32\Python-3.11

CMD /C CALL make_python_api.bat 2.7  x64 true false F:\develop\libs\Python\Python-2.7
CMD /C CALL make_python_api.bat 3.6  x64 true false F:\develop\libs\Python\Python-3.6
CMD /C CALL make_python_api.bat 3.7  x64 true false F:\develop\libs\Python\Python-3.7
CMD /C CALL make_python_api.bat 3.8  x64 true false F:\develop\libs\Python\Python-3.8
CMD /C CALL make_python_api.bat 3.9  x64 true false F:\develop\libs\Python\Python-3.9
CMD /C CALL make_python_api.bat 3.10 x64 true false F:\develop\libs\Python\Python-3.10
CMD /C CALL make_python_api.bat 3.11 x64 true true  F:\develop\libs\Python\Python-3.11

PAUSE
