@echo off

rem #########################################
set WXWIN=C:\DEVELOP\wxWidgets-3.0.0
set SAGA=C:\DEVELOP\saga\trunk\saga-gis
set SWIG=C:\DEVELOP\swigwin-2.0.1
set PYTHONPATH=C:\Python27
rem #########################################

rem #########################################
echo ________________________________________
echo SWIG compilation...

"%SWIG%\swig" -c++ -python -includeall -I%SAGA%/src/saga_core/saga_api -D_SAGA_PYTHON -D_SAGA_UNICODE saga_api.h

echo SWIG compilation finished.
rem #########################################

rem #########################################
echo ________________________________________
echo Python compilation...

rem saga_api_to_python_win.py install
"%PYTHONPATH%\python.exe" saga_api_to_python_win.py install

copy saga_api.py "%PYTHONPATH%\Lib\site-packages\saga_api.py"

echo Python compilation finished.
rem #########################################

pause
