@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #                                #
ECHO #     Test SAGA-Python-APIs      #
ECHO #                                #
ECHO ##################################
ECHO.

PUSHD %~dp0

SET SAGA_PATH=F:\develop\saga\saga-code\master\saga-gis\bin_win32\saga_Win32
SET PYTHON_DIR=F:\develop\libs\Python\_win32\Python-
%PYTHON_DIR%2.7\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.6\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.7\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.8\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.9\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.10\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.11\python.exe saga_python_version_tester.py

SET SAGA_PATH=F:\develop\saga\saga-code\master\saga-gis\bin\saga_x64
SET PYTHON_DIR=F:\develop\libs\Python\Python-
%PYTHON_DIR%2.7\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.6\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.7\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.8\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.9\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.10\python.exe saga_python_version_tester.py
%PYTHON_DIR%3.11\python.exe saga_python_version_tester.py

PAUSE
