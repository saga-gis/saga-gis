@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Compiling SAGA's Python API
ECHO #
ECHO ##################################
ECHO.

PUSHD %~dp0

REM SET ZIP=C:\Program Files\7-Zip\7z.exe
REM SET SWIG=F:\develop\libs\swigwin-4.0.2\swig.exe

CALL make_python_api.bat 27 win32 true false F:\develop\libs\Python\_win32\Python-2.7.10
CALL make_python_api.bat 35 win32 true false F:\develop\libs\Python\_win32\Python-3.5.9
CALL make_python_api.bat 36 win32 true false F:\develop\libs\Python\_win32\Python-3.6.14
CALL make_python_api.bat 37 win32 true false F:\develop\libs\Python\_win32\Python-3.7.11
CALL make_python_api.bat 38 win32 true false F:\develop\libs\Python\_win32\Python-3.8.11
CALL make_python_api.bat 39 win32 true false F:\develop\libs\Python\_win32\Python-3.9.6

CALL make_python_api.bat 27 x64 true false F:\develop\libs\Python\Python-2.7.10
CALL make_python_api.bat 35 x64 true false F:\develop\libs\Python\Python-3.5.9
CALL make_python_api.bat 36 x64 true false F:\develop\libs\Python\Python-3.6.14
CALL make_python_api.bat 37 x64 true false F:\develop\libs\Python\Python-3.7.11
CALL make_python_api.bat 38 x64 true false F:\develop\libs\Python\Python-3.8.11
CALL make_python_api.bat 39 x64 true true  F:\develop\libs\Python\Python-3.9.6

PAUSE
