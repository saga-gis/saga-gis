#! /usr/bin/env python

#################################################################################
# MIT License

# Copyright (c) 2023 Olaf Conrad

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#################################################################################
# Purpose
#################################################################################

'''
The PySAGA.convert module provides a collection of conversion tools.
'''


#################################################################################
#
# Globals
#________________________________________________________________________________

from PySAGA import saga_api


#################################################################################
#
# numpy...
#________________________________________________________________________________

#################################################################################
#________________________________________________________________________________
def Table_To_NumPy(Table, yFields=[], xField=None):
    '''
    Converts a saga_api.CSG_Grid object into a list of numpy arrays.
    '''
    try:
        import numpy
    except:
        return None

    try:
        Table.Get_Count()
    except:
        return None

    if xField:
        try:
            xField = int(xField)
        except:
            xField = -1
    else:
        xField = -1

    Fields = []; Data = [numpy.empty(Table.Get_Count(), float)]

    for i in range(0, len(yFields)):
        try:
            yFields[i] = int(yFields[i])
            if yFields[i] >= 0 and yFields[i] < Table.Get_Field_Count():
                Fields.append(yFields[i]); Data.append(numpy.empty(Table.Get_Count()))
        except:
            yFields[i] = -1

    for i in range(0, Table.Get_Count()):
        Record = Table.Get_Record(i)
        Data[0][i] = i if xField < 0 else Record.asDouble(xField)
        for j in range(1, len(Data)):
            Data[j][i] = Record.asDouble(Fields[j - 1])

    return Data


#################################################################################
#________________________________________________________________________________
def Grid_To_NumPy(Grid):
    '''
    Converts a saga_api.CSG_Grid object to numpy arrays.
    '''
    try:
        import numpy
    except:
        return None

    X, Y = numpy.meshgrid(
        numpy.linspace(Grid.Get_XMin(), Grid.Get_XMax(), Grid.Get_NX()),
        numpy.linspace(Grid.Get_YMin(), Grid.Get_YMax(), Grid.Get_NY())
    )

    Z = X + Y

    for y in range(0, Grid.Get_NY()):
        yy = Grid.Get_NY() - 1 - y
        for x in range(0, Grid.Get_NX()):
            Z[y, x] = Grid.asDouble(x, yy)

    return X, Y, Z


#################################################################################
#
#________________________________________________________________________________
