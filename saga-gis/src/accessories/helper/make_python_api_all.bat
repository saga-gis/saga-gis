@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Compiling SAGA's Python API
ECHO #
ECHO ##################################
ECHO.

SET SAGA_VERSION=saga-current

CALL make_python_api.bat 2 win32 true false
CALL make_python_api.bat 3 win32 true false
CALL make_python_api.bat 2 x64   true false
CALL make_python_api.bat 3 x64   true true

PAUSE
