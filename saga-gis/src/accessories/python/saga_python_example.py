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

#_________________________________________
##########################################
# Initialize the environment...

# Windows: Let the 'SAGA_PATH' environment variable point to
# the SAGA installation folder before importing 'saga_api'!
# This can be defined globally in the Windows system or
# user environment variable settings, in the 'PySAGA/__init__.py'
# file, or in the individual Python script itself. To do the latter
# just uncomment the following line and adjust the path accordingly:
###import os; os.environ['SAGA_PATH'] = os.path.split(os.path.dirname(__file__))[0]

# Windows: The most convenient way to make PySAGA available to all your
# Python scripts is to copy the PySAGA folder to the 'Lib/site-packages/'
# folder of your Python installation. If you don't want to do this or if you
# don't have the rights to do so, you can also copy it to the folder with
# the Python scripts in which you want to use PySAGA, or alternatively
# you can add the path containing the PySAGA folder (e.g. the path to your
# SAGA installation) to the PYTHONPATH environment variable. To do this
# from within your script you can also take the following command (just
# uncomment the following line and adjust the path accordingly):
###import sys, os; sys.path.insert(1, os.path.split(os.path.dirname(__file__))[0])

# Import saga_api from PySAGA:
from PySAGA import saga_api


#_________________________________________
##########################################
def Run_Random_Terrain():
    saga_api.SG_UI_Msg_Add('Running: Random Terrain')
    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('grid_calculus', '6')
    if not Tool:
        saga_api.SG_UI_Msg_Add_Error('Failed to request tool: Random Terrain')
        import sys; sys.exit()

    Tool.Reset()
    Tool.Set_Parameter('RADIUS'          , 25)
    Tool.Set_Parameter('ITERATIONS'      , 250)
    Tool.Set_Parameter('TARGET_USER_SIZE', 10)
    Tool.Set_Parameter('TARGET_USER_XMIN', 0)
    Tool.Set_Parameter('TARGET_USER_XMAX', 2000)
    Tool.Set_Parameter('TARGET_USER_YMIN', 0)
    Tool.Set_Parameter('TARGET_USER_YMAX', 2000)

    if not Tool.Execute():
        saga_api.SG_UI_Msg_Add_Error('failed to execute tool: ' + Tool.Get_Name().c_str())
        import sys; sys.exit()

    return Tool.Get_Parameter('TARGET_OUT_GRID').asGrid()


#_________________________________________
##########################################
def Run_Slope_Aspect_Curvature(DEM):
    saga_api.SG_UI_Msg_Add('Running: Slope, Aspect, Curvature')
    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', '0')
    if not Tool:
        saga_api.SG_UI_Msg_Add_Error('Failed to request tool: Slope, Aspect, Curvature')
        return  None, None, None, None

    Tool.Reset()
    Tool.Set_Parameter('ELEVATION'  , DEM)
    Tool.Set_Parameter('METHOD'     , 6) # '9 parameter 2nd order polynom (Zevenbergen & Thorne 1987)'
    Tool.Set_Parameter('UNIT_SLOPE' , 0) # 'radians'
    Tool.Set_Parameter('UNIT_ASPECT', 1) # 'degree'
    Tool.Set_Parameter('C_LONG'     , saga_api.SG_Get_Create_Pointer()) # optional output, remove this line, if you don't want to create it
    Tool.Set_Parameter('C_CROS'     , saga_api.SG_Get_Create_Pointer()) # optional output, remove this line, if you don't want to create it

    if not Tool.Execute():
        saga_api.SG_UI_Msg_Add_Error('failed to execute tool: ' + Tool.Get_Name().c_str())
        import sys; sys.exit()

    return Tool.Get_Parameter('SLOPE' ).asGrid(),\
           Tool.Get_Parameter('ASPECT').asGrid(),\
           Tool.Get_Parameter('C_LONG').asGrid(),\
           Tool.Get_Parameter('C_CROS').asGrid()


#_________________________________________
##########################################
def Run_Grid_Difference(A, B, PythonLoop):
    if not A.is_Compatible(B):
        saga_api.SG_UI_Msg_Add_Error('grids [' + A + '] and [' + B + '] are not compatible')
        import sys; sys.exit()

    import time; Time_Start = time.time()

    # ------------------------------------
    # cell by cell, slower than second solution
    if PythonLoop:
        saga_api.SG_UI_Msg_Add('Running: Grid Difference (Cell by Cell)\n')
        C = saga_api.SG_Create_Grid(A.Get_System())
        for y in range(0, C.Get_NY()):
            saga_api.SG_UI_Msg_Add('\r{:04.1f}%%'.format((1. + y) * 100. / C.Get_NY()), False)
            for x in range(0, C.Get_NX()):
                if A.is_NoData(x, y) or B.is_NoData(x, y):
                    C.Set_NoData(x, y)
                else:
                    C.Set_Value(x, y, A.asDouble(x, y) - B.asDouble(x, y))

    # ------------------------------------
    # using built-in CSG_Grid function 'Subtract()'
    else:
        saga_api.SG_UI_Msg_Add('Running: Grid Difference (CSG_Grid::Subtract())')
        C = saga_api.SG_Create_Grid(A)
        C.Subtract(B)

    # ------------------------------------
    Time = time.time() - Time_Start
    saga_api.SG_UI_Msg_Add('finished after {:d}min {:02.2f}sec'.format(int(Time / 60.), Time % 60.))
    return C


#_________________________________________
##########################################
def Run_Contour_Lines_from_Grid(Grid):
    saga_api.SG_UI_Msg_Add('Running: Contour Lines from Grid')
    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('shapes_grid', '5')
    if not Tool:
        saga_api.SG_UI_Msg_Add_Error('Failed to request tool: Contour Lines from Grid')
        import sys; sys.exit()

    Tool.Reset()
    Tool.Set_Parameter('GRID', Grid)

    if not Tool.Execute():
        saga_api.SG_UI_Msg_Add_Error('failed to execute tool: ' + Tool.Get_Name().c_str())
        import sys; sys.exit()

    return Tool.Get_Parameter('CONTOUR').asShapes()


#_________________________________________
##########################################
def Run_Geomorphons(DEM):
    saga_api.SG_UI_Msg_Add('Running: Geomorphons')
    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_lighting', '8')
    if not Tool:
        saga_api.SG_UI_Msg_Add_Error('Failed to request tool: Geomorphons')
        import sys; sys.exit()

    Tool.Reset()
    Tool.Set_Parameter('DEM'      , DEM)
    Tool.Set_Parameter('THRESHOLD', 1)
    Tool.Set_Parameter('RADIUS'   , 10000)
    Tool.Set_Parameter('METHOD'   , 1) # 'line tracing'

    if not Tool.Execute():
        saga_api.SG_UI_Msg_Add_Error('failed to execute tool: ' + Tool.Get_Name().c_str())
        import sys; sys.exit()

    return Tool.Get_Parameter('GEOMORPHONS').asGrid()


#_________________________________________
##########################################

saga_api.SG_UI_Msg_Add('This is a simple script to demonstrate the usage of SAGA and its tools through Python.')

ReportProgress = False
if not ReportProgress:
    saga_api.SG_UI_Progress_Lock(True)

import os;
output = 'test/'
if not os.path.exists(output):
    os.mkdir(output)

#_________________________________________
# load or create a Digital Elevation Model
DEM = saga_api.SG_Get_Data_Manager().Add_Grid(output + 'dem.tif')
if not DEM:
    DEM = Run_Random_Terrain()
    DEM.Save(output + 'dem.tif')

#_________________________________________
# derive some terrain parameters
Slope, Aspect, vCurv, hCurv = Run_Slope_Aspect_Curvature(DEM)
Slope .Save(output + 'slope.tif' )
Aspect.Save(output + 'aspect.tif')
vCurv .Save(output + 'vcurv.tif' )
hCurv .Save(output + 'hcurv.tif' )

#_________________________________________
# calculate grid differences
Difference = Run_Grid_Difference(vCurv, hCurv, PythonLoop=True)
Difference.Save(output + 'diff_1.tif')

Difference = Run_Grid_Difference(vCurv, hCurv, PythonLoop=False)
Difference.Save(output + 'diff_2.tif')

#_________________________________________
# get contour lines from elevation grid
Contour = Run_Contour_Lines_from_Grid(DEM)
Contour.Save(output + 'contour.geojson')

#_________________________________________
# perform some terrain classification
Geomorphons = Run_Geomorphons(DEM)
Geomorphons.Save(output + 'geomorphons.tif')

#_________________________________________
# when job is done, free memory resources:
saga_api.SG_Get_Data_Manager().Delete_All()

if not ReportProgress:
    saga_api.SG_UI_Progress_Lock(False)

saga_api.SG_UI_Msg_Add('________________\n...the end!')


#_________________________________________
##########################################
# ...the end!
