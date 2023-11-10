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
