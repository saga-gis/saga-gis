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
The PySAGA.plot module provides a collection of simple tools for plotting SAGA
data objects with matplot tools.
'''


#################################################################################
#
# Globals
#________________________________________________________________________________

from PySAGA import saga_api


#################################################################################
#
# Grids...
#________________________________________________________________________________

#################################################################################
#________________________________________________________________________________
def Plot_Grid(Grid):
    '''
    A simple visualization of a saga_api.CSG_Grid object 
    '''
    try:
        import matplotlib
    except:
        return False

    from PySAGA import convert

    X, Y, Z = convert.Grid_To_NumPy(Grid)

    import matplotlib.pyplot as plot, matplotlib.cm as cm

    plot.style.use('classic') # classic, Solarize_Light2, bmh, fivethirtyeight, ggplot, grayscale, ...
    plot.imshow(Z, cmap=cm.terrain)
    plot.show()

    return True


#################################################################################
#
#________________________________________________________________________________
