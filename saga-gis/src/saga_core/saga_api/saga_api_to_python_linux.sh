
echo "SWIG compilation..."
swig -c++ -python -includeall -D_SAGA_PYTHON ./saga_api.h
echo "SWIG compilation finished."

echo "Python compilation..."
python saga_api_to_python_linux.py install
cp ./saga_api.py /usr/lib/python/site-packages/
echo "Python compilation finished."
