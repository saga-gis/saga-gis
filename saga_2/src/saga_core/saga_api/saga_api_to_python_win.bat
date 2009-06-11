@echo off

rem set SWIG=C:\Program Files\swigwin-1.3.25
rem set PYTHON=E:\develop\Python-2.4.6

echo SWIG compilation...
"%SWIG%\swig" -c++ -python -includeall saga_api.h
echo SWIG compilation finished.

echo Python compilation...
"%PYTHON%\python.exe" saga_api_to_python_win.py install
copy saga_api.py "%PYTHON%\Lib\site-packages\saga_api.py"
echo Python compilation finished.

pause
