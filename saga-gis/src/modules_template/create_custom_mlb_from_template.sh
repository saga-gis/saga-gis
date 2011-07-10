#!/bin/sh
#
# This script can be used to create a copy of the SAGA module library template
# including the automatic renaming of the library and the module. Just call
# the script from here, in case no arguments are provided information on
# its usage is printed.
# 
# Author: Christian Georges
#

if [ $# -le 1 ]
then
   echo ""
   echo "Usage: `basename $0` <my module library name> <my module name>"
   echo ""
   exit 3 
fi

MODLIBTEMPLATE=Template
MODULETEMPLATE=My_Module

MYMODLIB=$1
MYMODULE=$2
MYMODLIBDIR=../modules_contrib/$MYMODLIB

# create custom dir with custom named files:
mkdir $MYMODLIBDIR
cp Makefile* MLB_Interface.* $MYMODLIBDIR/.
for FILE in $MODLIBTEMPLATE.*; do 
    cp $FILE $MYMODLIBDIR/$MYMODLIB.${FILE#$MODLIBTEMPLATE.*}
done
for FILE in $MODULETEMPLATE.*; do 
    cp $FILE $MYMODLIBDIR/$MYMODULE.${FILE#$MODULETEMPLATE.*}
done

# replace module library name and module name in files:
for FILE in $(ls -f $MYMODLIBDIR/*); do
    sed s,$MODLIBTEMPLATE,$MYMODLIB,g < $FILE > $FILE.tmp
    sed s,$MODULETEMPLATE,$MYMODULE,g < $FILE.tmp > $FILE
    rm $FILE.tmp
done

