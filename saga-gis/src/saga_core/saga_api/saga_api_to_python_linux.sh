#!/bin/bash

# compilation script for the SAGA Python Interface
# adjust path to your site-packages directory
# also adjust paths in saga_api_to_python_linux.py
# run with administrative rights

SITEPACKDIR=/usr/local/lib/python2.6/site-packages

echo "Compiling SAGA Python Interface ..."

echo "SWIG compilation..."
swig -c++ -python -includeall -D_SAGA_PYTHON -D_SAGA_UNICODE ./saga_api.h
echo "SWIG compilation finished."

echo "Python compilation..."
python saga_api_to_python_linux.py install
cp ./saga_api.py $SITEPACKDIR/saga_api.py
echo "Python compilation finished."
