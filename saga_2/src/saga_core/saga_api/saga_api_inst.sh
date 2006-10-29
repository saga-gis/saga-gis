#! /bin/sh

#
# This is a simple script to copy SAGA API headers to
#   /usr/local/include/saga_api
# and the SAGA API shared library to
#   /usr/local/lib
#

echo "INSTALLING SAGA API"
echo ". This is a simple script to copy SAGA API headers to"
echo ".   /usr/local/include/saga_api"
echo ". and the SAGA API shared library to"
echo ".   /usr/local/lib"
echo ". (you probably must have root rights to execute this script successfully)"

echo ". copying files..."

if test ! -r "/usr/local/include/saga_api"; then
  mkdir /usr/local/include/saga_api
fi

cp -f ./api_core.h       /usr/local/include/saga_api
cp -f ./dataobject.h     /usr/local/include/saga_api
cp -f ./doc_html.h       /usr/local/include/saga_api
cp -f ./doc_pdf.h        /usr/local/include/saga_api
cp -f ./doc_svg.h        /usr/local/include/saga_api
cp -f ./geo_tools.h      /usr/local/include/saga_api
cp -f ./grid.h           /usr/local/include/saga_api
cp -f ./mat_tools.h      /usr/local/include/saga_api
cp -f ./module.h         /usr/local/include/saga_api
cp -f ./module_library.h /usr/local/include/saga_api
cp -f ./parameters.h     /usr/local/include/saga_api
cp -f ./shapes.h         /usr/local/include/saga_api
cp -f ./table.h          /usr/local/include/saga_api
cp -f ./tin.h            /usr/local/include/saga_api
cp -f ./saga_api.h       /usr/local/include/saga_api

if test -r "./../../../bin/saga_gtk/libsaga_api.so"; then
  cp -f ./../../../bin/saga_gtk/libsaga_api.so /usr/local/lib
fi

echo ". ...install completed successfully!"
