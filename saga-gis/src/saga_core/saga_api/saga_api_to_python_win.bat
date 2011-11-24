@echo off

rem #########################################
set WXWIN=H:\libs\libs\wxMSW-2.8.11
set SAGA=H:\saga\saga_svn_wx28\saga-gis
rem set SWIG=C:\Program Files\swigwin-1.3.25
set PYTHONPATH=C:\Programme (x86)\PYTHON26
rem #########################################

rem #########################################
echo SWIG compilation...

"%SWIG%\swig" -c++ -python -includeall -D_SAGA_PYTHON saga_api.h

echo SWIG compilation finished.
rem #########################################

rem #########################################
echo Python compilation...

saga_api_to_python_win.py install
"%PYTHONPATH%\python.exe" saga_api_to_python_win.py install

copy saga_api.py "%PYTHONPATH%\Lib\site-packages\saga_api.py"

echo Python compilation finished.
rem #########################################

pause
