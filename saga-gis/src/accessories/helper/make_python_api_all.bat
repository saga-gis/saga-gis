@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Compiling SAGA's Python API
ECHO #
ECHO ##################################
ECHO.

PUSHD %~dp0

SET SAGA_VERSION=saga-current

SET PYTHONDIR=F:\develop\libs\Python\Python27_win32
CALL make_python_api.bat 27 win32 true false

SET PYTHONDIR=F:\develop\libs\Python\Python27_x64
CALL make_python_api.bat 27 x64   true false

SET PYTHONDIR=F:\develop\libs\Python\Python35_win32
CALL make_python_api.bat 35 win32 true false

SET PYTHONDIR=F:\develop\libs\Python\Python35_x64
CALL make_python_api.bat 35 x64   true true

PAUSE
