@echo off

rem #########################################
set WXWIN=H:\libs\libs\wxWidgets-2.9.4
set SAGA=H:\saga\saga_2\trunk\saga-gis
rem set SWIG=C:\Program Files\swigwin-1.3.25
set PYTHONPATH=H:\libs\_libs\Python\Python-2.6
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
