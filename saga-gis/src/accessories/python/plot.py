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
# Tables...
#________________________________________________________________________________

#################################################################################
#________________________________________________________________________________
def Plot_Table(Table, yFields=[], xField=None):
    '''
    Creates a simple plot of a saga_api.CSG_Table object 
    '''
    try:
        import matplotlib
    except:
        return False

    from PySAGA import convert

    Data = convert.Table_To_NumPy(Table, yFields, xField)
    if not Data:
        return False

    from matplotlib import pyplot

    pyplot.style.use('classic') # classic, Solarize_Light2, bmh, fivethirtyeight, ggplot, grayscale, ...

    fig, ax = pyplot.subplots()

    if xField != None and xField >= 0 and xField < Table.Get_Field_Count():
        ax.set_xlabel(Table.Get_Field_Name(xField))

    for i in range(1, len(Data)):
        ax.plot(Data[0], Data[i], label=Table.Get_Field_Name(yFields[i - 1]))

    pyplot.legend()
    pyplot.show()

    return True


#################################################################################
#
# Shapes...
#________________________________________________________________________________

#################################################################################
#________________________________________________________________________________
def Plot_Shapes(Shapes, Show=True):
    '''
    Creates a simple plot of a saga_api.CSG_Shapes object 
    '''
    try:
        import matplotlib
    except:
        return False

    import numpy; from matplotlib import pyplot

    for iShape in range(0, Shapes.Get_Count()):
        Shape = Shapes.Get_Shape(iShape)
        for iPart in range(0, Shape.Get_Part_Count()):
            x = numpy.empty(Shape.Get_Point_Count(iPart), float)
            y = numpy.empty(Shape.Get_Point_Count(iPart), float)
            for iPoint in range(0, Shape.Get_Point_Count(iPart)):
                Point = Shape.Get_Point(iPoint, iPart)
                x[iPoint] = Point.x
                y[iPoint] = Point.y

            if Shapes.Get_Type() == saga_api.SHAPE_TYPE_Point \
            or Shapes.Get_Type() == saga_api.SHAPE_TYPE_Points:
                pyplot.plot(x, y, 'o', color='black')
            if Shapes.Get_Type() == saga_api.SHAPE_TYPE_Line:
                pyplot.plot(x, y, color='black')#, label='{:s}'.format(Shape.asString(1)))
            if Shapes.Get_Type() == saga_api.SHAPE_TYPE_Polygon:
                pyplot.fill(x, y)#, label='{:s}'.format(Shape.asString(1)))

    pyplot.axis('equal')

    if Show:
        pyplot.show()

    return True


#################################################################################
#
# Grids...
#________________________________________________________________________________

#################################################################################
#________________________________________________________________________________
def Plot_Grid(Grid, Show=True):
    '''
    A simple visualization of a saga_api.CSG_Grid object 
    '''
    try:
        import matplotlib
    except:
        return False

    from PySAGA import convert

    X, Y, Z = convert.Grid_To_NumPy(Grid)

    import matplotlib.pyplot as pyplot, matplotlib.cm as cm

    pyplot.style.use('classic') # classic, Solarize_Light2, bmh, fivethirtyeight, ggplot, grayscale, ...
    pyplot.imshow(Z, extent=[Grid.Get_XMin(), Grid.Get_XMax(), Grid.Get_YMin(), Grid.Get_YMax()], cmap=cm.terrain)
 
    if Show:
        pyplot.show()

    return True


#################################################################################
#
#________________________________________________________________________________
