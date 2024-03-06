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

# Windows: The most convenient way to make PySAGA available to your
# Python scripts is to add the path containing the PySAGA folder
# (i.e. the path to your SAGA installation) to the PYTHONPATH
# environment variable. You can do this from within your script
# with the following command (just uncomment the following line
# and adjust the path accordingly):
###import sys, os; sys.path.insert(1, 'C:/saga-9.3.0_x64')

#_________________________________________
##########################################
# Import SAGA API from PySAGA
#_________________________________________
from PySAGA import saga_api

print(saga_api.SG_Get_Tool_Library_Manager().Get_Summary(saga_api.SG_SUMMARY_FMT_FLAT).c_str())

#_________________________________________
##########################################
# Get Some Test Data
#_________________________________________
import os
WorkDir = os.getcwd() + '/test'
if not os.path.exists(WorkDir):
    os.makedirs(WorkDir)
os.chdir(WorkDir)

#_________________________________________
File = 'dem.tif'
if not os.path.exists(File):
    from PySAGA import helper
    aoi = helper.Get_AOI_From_Extent(560000, 580000, 5700000, 5720000, EPSG=32632)
    from PySAGA.data import srtm
    if not srtm.CGIAR_Get_AOI(aoi, '{:s}/{:s}'.format(WorkDir, File)):
        print('\nfailed to download SRTM from CGIAR, trying to download Copernicus DEM instead...')
        from PySAGA.data import copernicus_dem
        if not copernicus_dem.Get_AOI(aoi, '{:s}/{:s}'.format(WorkDir, File)):
            print('\nfailed to download DEM data'); import sys; sys.exit()

#_________________________________________
dem = saga_api.SG_Get_Data_Manager().Add_Grid(File)
if not dem or not dem.is_Valid():
    print('\nfailed to load ' + File); import sys; sys.exit()
else:
    print('\nsucccessfully loaded ' + File)

#_________________________________________
from PySAGA import plot; Plot_Results = True

if Plot_Results:
    plot.Plot_Grid(dem)

#_________________________________________
##########################################
# Geomorphometric Analysis
#_________________________________________
from PySAGA.tools import ta_morphometry, grid_filter

slope     = saga_api.SG_Get_Data_Manager().Add_Grid()
curvature = saga_api.SG_Get_Data_Manager().Add_Grid()

if ta_morphometry.Slope_Aspect_Curvature(ELEVATION=dem, SLOPE=slope, UNIT_SLOPE='degree', C_GENE=curvature):
     # save results to file...
    slope.Save('slope.sg-grd-z'); curvature.Save('curvature.sg-grd-z')

    # ...and plot the results!
    if Plot_Results:
        plot.Plot_Grid(slope); plot.Plot_Grid(curvature)

if ta_morphometry.Slope_Aspect_Curvature(ELEVATION=dem, C_TANG=curvature):
    curvature.Save('curvature_tangential.tif')
    if Plot_Results:
        plot.Plot_Grid(curvature)

landforms = saga_api.SG_Get_Data_Manager().Add_Grid()
if ta_morphometry.TPI_Based_Landform_Classification(DEM=dem, LANDFORMS=landforms, RADIUS_A='0; 100', RADIUS_B='100; 1000'):
    landforms.Save('landforms.tif')
    if Plot_Results:
        plot.Plot_Grid(landforms)

if grid_filter.MajorityMinority_Filter(INPUT=landforms, KERNEL_RADIUS=3):
    landforms.Save('landforms_filtered.tif')
    if Plot_Results:
        plot.Plot_Grid(landforms)

from PySAGA.tools import shapes_grid
polygons = saga_api.SG_Get_Data_Manager().Add_Shapes()
shapes_grid.Vectorizing_Grid_Classes(GRID=landforms, POLYGONS=polygons)
if Plot_Results:
    plot.Plot_Shapes(polygons)

table = saga_api.SG_Get_Data_Manager().Add_Table()
if ta_morphometry.Hypsometry(ELEVATION=dem, TABLE=table):
    table.Save('hypsometry.txt')

    from PySAGA import helper; helper.Print_Table(table)

    if Plot_Results:
        plot.Plot_Table(table, yFields=[0], xField=1)

#_________________________________________
##########################################
# Hydrologic Analysis
#_________________________________________
from PySAGA.tools import ta_preprocessor, ta_hydrology

dem_nosinks = saga_api.SG_Get_Data_Manager().Add_Grid()

ta_preprocessor.Sink_Removal(DEM=dem, DEM_PREPROC=dem_nosinks)

# detect the filled sinks
from PySAGA.tools import grid_calculus

sinks = saga_api.SG_Get_Data_Manager().Add_Grid()
grid_calculus.Grid_Calculator(GRIDS=[dem, dem_nosinks], RESULT=sinks, FORMULA='g2 - g1')
sinks.Set_NoData_Value(0.)
sinks.Save('closed_depressions.tif')

# calculate the flow accumulation using the depressionless DEM
from PySAGA.tools import ta_hydrology

flow_acc = saga_api.SG_Get_Data_Manager().Add_Grid()
ta_hydrology.Flow_Accumulation_TopDown(ELEVATION=dem_nosinks, FLOW=flow_acc, METHOD='Multiple Triangular Flow Directon')
flow_acc.Save('flow_acc.tif')
if Plot_Results:
    plot.Plot_Grid(flow_acc)

sca = saga_api.SG_Get_Data_Manager().Add_Grid()
twi = saga_api.SG_Get_Data_Manager().Add_Grid()
ta_morphometry.Slope_Aspect_Curvature(ELEVATION=dem, SLOPE=slope, UNIT_SLOPE='radians')
ta_hydrology.Flow_Width_and_Specific_Catchment_Area(DEM=dem_nosinks, TCA=flow_acc, SCA=sca)
ta_hydrology.Topographic_Wetness_Index(SLOPE=slope, AREA=sca, TWI=twi)
twi.Save('twi.sg-grd-z')
if Plot_Results:
    plot.Plot_Grid(twi)

#_________________________________________
##########################################
# Deriving Contour Lines
#_________________________________________
from PySAGA.tools import shapes_grid

contour = saga_api.SG_Get_Data_Manager().Add_Shapes()
shapes_grid.Contour_Lines_from_Grid(GRID=dem, CONTOUR=contour, INTERVALS='equal intervals', ZSTEP=10)
contour.Save('contour.shp')

if Plot_Results:
    plot.Plot_Grid(dem, False) # don't show/finish the plot before the contour lines have been added
    plot.Plot_Shapes(contour)

#_________________________________________
##########################################
# Check and free memory resources

print(saga_api.SG_Get_Data_Manager().Get_Summary().c_str())

saga_api.SG_Get_Data_Manager().Delete(sca)
saga_api.SG_Get_Data_Manager().Delete(twi)
saga_api.SG_Get_Data_Manager().Delete(table)

print(saga_api.SG_Get_Data_Manager().Get_Summary().c_str())

saga_api.SG_Get_Data_Manager().Delete()

print(saga_api.SG_Get_Data_Manager().Get_Summary().c_str())

#_________________________________________
##########################################
# ...that's it so far!
