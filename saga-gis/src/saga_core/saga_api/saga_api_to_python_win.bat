@echo off

rem #########################################
set SWIG=D:\libs\swigwin-3.0.7
set PYTHONPATH=D:\libs\Python-2.7

set SAGA_LIB=%SAGA%\bin\saga_vc_Win32
rem set SAGA_LIB=%SAGA%\bin\saga_vc_x64

rem #########################################
echo ________________________________________
echo SWIG compilation...
"%SWIG%\swig" -c++ -python -includeall -I%SAGA_LIB%/include/saga_api -D_SAGA_PYTHON saga_api.h
echo SWIG compilation finished.

rem #########################################
echo ________________________________________
echo Python compilation...
"%PYTHONPATH%\python.exe" saga_api_to_python_win.py install
echo Python compilation finished.

copy saga_api.py "%PYTHONPATH%\Lib\site-packages\saga_api.py"

rem #########################################
echo ________________________________________
echo post compilation jobs...

set OUTDIR=D:\saga\saga_python

if not exist "%OUTDIR%" mkdir "%OUTDIR%"
if not exist "%OUTDIR%\Python27" mkdir "%OUTDIR%\Python27"
if not exist "%OUTDIR%\Python27\Lib" mkdir "%OUTDIR%\Python27\Lib"
if not exist "%OUTDIR%\Python27\Lib\site-packages" mkdir "%OUTDIR%\Python27\Lib\site-packages"

copy "%PYTHONPATH%\Lib\site-packages\*.*" "%OUTDIR%\Python27\Lib\site-packages"

rem goto FINISH

rem #########################################
echo ________________________________________
echo remove temporary files...
del saga_api.py
del saga_api_wrap.cxx
rmdir build /s /q

rem #########################################

:FINISH
pause
