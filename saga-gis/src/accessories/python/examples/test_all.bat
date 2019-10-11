@ECHO OFF

ECHO __________________________________
ECHO ##################################
ECHO #
ECHO # Testing SAGA's Python API
ECHO #
ECHO ##################################
ECHO.

REM Adjust here the paths to your SAGA and Python installations:
SET SAGA_PATH=D:\saga\saga-code\master\saga-gis\bin\saga_vc_x64
SET PYTHON_EXE=D:\libs\Python\Python35_x64\PCbuild\amd64\python.exe

%PYTHON_EXE% 00_grid_create_dummy.py
%PYTHON_EXE% 01_grid_asc_to_saga.py
%PYTHON_EXE% 02_grid_morphometry.py
%PYTHON_EXE% 03_grid_difference.py
%PYTHON_EXE% 04_grid_contour.py
%PYTHON_EXE% 05_shp2xyz.py
%PYTHON_EXE% 06_xyz2shp.py
%PYTHON_EXE% 07_grid_tpi.py

PAUSE
