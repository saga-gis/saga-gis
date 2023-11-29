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
The PySAGA.data.chelsa module provides easy-to-use functions for accessing
``CHELSA (Climatologies at High resolution for the Earth's Land Surface Areas)``
global climate data. For more information on the data itself refer to
    https://chelsa-climate.org/

For downloading requested files the wget Python package is used, which needs to be
installed in order to work. Installation can be done through pip:
    pip install wget

Basic usage:
    import PySAGA.data.chelsa as chelsa, PySAGA.helper as saga_helper

    chelsa.Dir_Global = 'C:/chelsa/global'

    chelsa.Dir_Target = 'C:/chelsa/germany_utm32n'

    AOI = saga_helper.Get_AOI_From_Extent(279000, 920000, 5235000, 6102000, 32632)

    chelsa.Get_Climatology('tas'   , AOI)

    chelsa.Get_Climatology('tasmin', AOI)

    chelsa.Get_Climatology('tasmax', AOI)

    chelsa.Get_Climatology('pr'    , AOI)

    chelsa.Get_Climatology('pet'   , AOI)
'''


#################################################################################
#
# Globals
#________________________________________________________________________________

import os, PySAGA.saga_api as saga_api, PySAGA.data.helper

Dir_Global = os.getcwd() + '/global'
Dir_Target = os.getcwd() + '/aoi'
Ext_Target = 'sg-grd-z'

Download_Root = 'https://os.zhdk.cloud.switch.ch/envicloud/chelsa/chelsa_V2/GLOBAL'
Download_Retries = 4


#################################################################################
#________________________________________________________________________________
def _Variable_Unit_Conversion(Variable, Monthly):
    if Variable == 'tas' or Variable == 'tasmin' or Variable == 'tasmax': # temperatures
        return 0.1 , -273.15, 'Celsius'    # 1/10 Kelvin => ° Celsius

    if Variable == 'pr': # precipitation
        if Monthly: # monthly time series scaling differs from climatologies !!!
            return 0.01,    0.  , 'mm / month' # 1/100 kg / m² / month
        return 0.1 ,    0.  , 'mm / month' # 1/100 kg / m² / month

    if Variable == 'pet': # potential evaporation
        return 0.01,    0.  , 'mm / month' # 1/100 kg / m² / month

    if Variable == 'hurs': # near-surface relative humidity
        return 0.01 , 0., '%'

    if Variable == 'rsds': # surface downwelling shortwave flux in air
        return 0.001 , 0., 'MJ / m2 / day'

    if Variable == 'sfcWind': # near-surface wind speed
        return 0.001 , 0., 'm / sec'

    if Variable == 'vpd': # vapor pressure deficit
        return 0.1 , 0., 'Pa'

    return 1., 0., None

#________________________________________________________________________________
def _Variable_File_Suffix(Variable):
    if Variable == 'pet':
        return '_penman'

    return ''


#################################################################################
#
# Climatologies...
#________________________________________________________________________________

#________________________________________________________________________________
# Returns file path to requested data set or 'None'. Local storage path is defined
# by the global 'Dir_Global' variable and defaults to 'global' subfolder relative
# to the current working directory.
#________________________________________________________________________________
def Get_Global_Climatology_Month(Variable, Month):
    '''
    Returns file path to requested data set or 'None'. Local storage path is defined
    by the global 'Dir_Global' variable and defaults to 'global' subfolder relative
    to the current working directory.
    '''

    File       = 'CHELSA_{:s}{:s}_{:02d}_1981-2010_V.2.1.tif'.format(Variable, _Variable_File_Suffix(Variable), Month)
    Local_Dir  = '{:s}/{:s}'.format(Dir_Global, Variable)
    Remote_Dir = 'climatologies/1981-2010/{:s}'.format(Variable)

    return PySAGA.data.helper.Get_File(File, Local_Dir, Download_Root + '/' + Remote_Dir, Download_Retries)

#________________________________________________________________________________
# Get the original file of a variable for all months.
#________________________________________________________________________________
def Get_Global_Climatology(Variable):
    '''
    Get the original file of a variable for all months.
    '''

    nFailed = 0
    for Month in range(1, 12 + 1):
        if not Get_Global_Climatology_Month(Variable, Month):
            nFailed += 1; saga_api.SG_UI_Msg_Add('download failed for {:s}-{:02d}'.format(Variable, Month))

    if nFailed > 0:
        saga_api.SG_UI_Msg_Add_Error('{:d} download(s) failed'.format(nFailed))
        return False
    return True

#________________________________________________________________________________
# Get a variable for the area of your interest (AOI)...
#________________________________________________________________________________
def Get_Climatology_Month(Variable, Month, AOI=None, bDeleteGlobal=False):
    '''
    Get a variable for the area of your interest (AOI)...
    '''

    Scaling, Offset, Unit = _Variable_Unit_Conversion(Variable, False)
    Target_File = '{:s}/{:s}/{:s}_{:02d}.{:s}'.format(Dir_Target, Variable, Variable, Month, Ext_Target)
    return os.path.exists(Target_File) or Get_Variable(Get_Global_Climatology_Month(Variable, Month), Target_File, AOI, Scaling, Offset, Unit, bDeleteGlobal)

#________________________________________________________________________________
def Get_Climatology(Variable, AOI=None, bDeleteGlobal=False):
    for Month in range(1, 12 + 1):
        Get_Climatology_Month(Variable, Month, AOI, bDeleteGlobal)


#################################################################################
#
# Future Projections...
#________________________________________________________________________________

#________________________________________________________________________________
# Returns file path to requested data set or 'None'. Local storage path is defined
# by the global 'Dir_Global' variable and defaults to 'global' subfolder relative
# to the current working directory.
#________________________________________________________________________________
def Get_Global_Projection(Variable, Month, Period = '2041-2070', Model = 'MPI-ESM1-2-HR', SSP = '585'):
    '''
    Returns file path to requested data set or 'None'. Local storage path is defined
    by the global 'Dir_Global' variable and defaults to 'global' subfolder relative
    to the current working directory.
    '''

    File       = 'CHELSA_{:s}_r1i1p1f1_w5e5_ssp{:s}_{:s}_{:02d}_{:s}_norm.tif'.format(Model.lower(), SSP, Variable, Month, Period.replace('-', '_'))
    Local_Dir  = '{:s}/{:s}'.format(Dir_Global, Variable)
    Remote_Dir = 'climatologies/{:s}/{:s}/ssp{:s}/{:s}'.format(Period, Model.upper(), SSP, Variable)

    return PySAGA.data.helper.Get_File(File, Local_Dir, Download_Root + '/' + Remote_Dir, Download_Retries)

#________________________________________________________________________________
# Get the original file of a variable for all months.
#________________________________________________________________________________
def Get_Global_Projection_AllMonths(Variable, Period = '2041-2070', Model = 'MPI-ESM1-2-HR', SSP = '585'):
    '''
    Get the original file of a variable for all months.
    '''

    nFailed = 0
    for Month in range(1, 12 + 1):
        if not Get_Global_Projection(Variable, Month, Period, Model, SSP):
            nFailed += 1; saga_api.SG_UI_Msg_Add('download failed for {:s}-{:02d} [{:s}/{:s}/{:s}]'.format(Variable, Month, Period, SSP, Model))

    if nFailed > 0:
        saga_api.SG_UI_Msg_Add_Error('{:d} download(s) failed'.format(nFailed))
        return False
    return True

#________________________________________________________________________________
# Get a variable for the area of your interest (AOI)...
#________________________________________________________________________________
def Get_Projection_Month(Variable, Month, AOI=None, bDeleteGlobal=False):
    '''
    Get a variable for the area of your interest (AOI)...
    '''

    Scaling, Offset, Unit = _Variable_Unit_Conversion(Variable, False)
    Target_File = '{:s}/{:s}/{:s}_{:02d}.{:s}'.format(Dir_Target, Variable, Variable, Month, Ext_Target)
    return os.path.exists(Target_File) or Get_Variable(Get_Global_Climatology_Month(Variable, Month), Target_File, AOI, Scaling, Offset, Unit, bDeleteGlobal)

#________________________________________________________________________________
def Get_Projection(Variable, AOI=None, bDeleteGlobal=False):
    for Month in range(1, 12 + 1):
        Get_Projection_Month(Variable, Month, AOI, bDeleteGlobal)


#################################################################################
#
# Monthly Time Series...
#________________________________________________________________________________

#________________________________________________________________________________
# Returns file path to requested data set or 'None'. Local storage path is defined
# by the global 'Dir_Global' variable and defaults to 'global' subfolder relative
# to the current working directory.
#________________________________________________________________________________
def Get_Global_Monthly(Variable, Year, Month):
    '''
    Returns file path to requested data set or 'None'. Local storage path is defined
    by the global 'Dir_Global' variable and defaults to 'global' subfolder relative
    to the current working directory.
    '''

    File       = 'CHELSA_{:s}{:s}_{:02d}_{:04d}_V.2.1.tif'.format(Variable, _Variable_File_Suffix(Variable), Month, Year)
    Local_Dir  = '{:s}/{:s}'.format(Dir_Global, Variable)
    Remote_Dir = 'monthly/{:s}'.format(Variable)

    return PySAGA.data.helper.Get_File(File, Local_Dir, Download_Root + '/' + Remote_Dir, Download_Retries)

#________________________________________________________________________________
# Get the original file of a variable for the given monthly and year's range.
#________________________________________________________________________________
def Get_Global_Monthly_Series(Variable, Years = [1980, 2019], Months=[1, 12]):
    '''
    Get the original file of a variable for the given monthly and year's range.
    '''

    nFailed = 0
    for Year in range(Years[0], Years[1] + 1):
        for Month in range(1, 12 + 1):
            if not Get_Global_Monthly(Variable, Year, Month):
                nFailed += 1; saga_api.SG_UI_Msg_Add('download failed for {:s}-{:04d}-{:02d}'.format(Variable, Year, Month))

    if nFailed > 0:
        saga_api.SG_UI_Msg_Add_Error('{:d} download(s) failed'.format(nFailed))
        return False
    return True

#________________________________________________________________________________
# Get a variable for the area of your interest (AOI)...
#________________________________________________________________________________
def Get_Monthly(Variable, Year, Month, AOI=None, bDeleteGlobal=False):
    '''
    Get a variable for the area of your interest (AOI)...
    '''

    Scaling, Offset, Unit = _Variable_Unit_Conversion(Variable, True)
    Target_File = '{:s}/{:s}/{:s}_{:04d}_{:02d}.{:s}'.format(Dir_Target, Variable, Variable, Year, Month, Ext_Target)
    return os.path.exists(Target_File) or Get_Variable(Get_Global_Monthly(Variable, Year, Month), Target_File, AOI, Scaling, Offset, Unit, bDeleteGlobal)

#________________________________________________________________________________
def Get_Monthly_Series(Variable, AOI=None, Years=[1980, 2019], Months=[1, 12], bDeleteGlobal=False):
    for Year in range(Years[0], Years[1] + 1):
        for Month in range(Months[0], Months[1] + 1):
            Get_Monthly(Variable, Year, Month, AOI, bDeleteGlobal)


#################################################################################
#
# Extract and project a variable to fit the given area of your interest (AOI)...
#________________________________________________________________________________
def Get_Variable(Global_File, Target_File, AOI, Scaling=1., Offset=0., Unit=None, Round=True, bDeleteGlobal=False):
    '''
    Extract and project a variable to fit the given area of your interest (AOI)...
    '''

    #____________________________________________________________________________
    def Round_Extent(Extent, Resolution=1000.):
        if Round:
            from math import floor, ceil
            Extent.xMin = Resolution * floor(Extent.xMin / Resolution)
            Extent.xMax = Resolution * ceil (Extent.xMax / Resolution)
            Extent.yMin = Resolution * floor(Extent.yMin / Resolution)
            Extent.yMax = Resolution * ceil (Extent.yMax / Resolution)
        return True

    def Import_Raster(File, AOI):
        if not AOI or not AOI.is_Valid() or not AOI.Get_Projection().is_Okay():
            saga_api.SG_UI_Msg_Add_Error('invalid AOI')
            return None

        Extent = saga_api.CSG_Rect(AOI.Get_Extent())
        if not AOI.Get_Projection().is_Geographic():
            Round_Extent(Extent)
            _AOI = saga_api.CSG_Shapes(saga_api.SHAPE_TYPE_Point); _AOI.Get_Projection().Create(AOI.Get_Projection())
            _AOI.Add_Shape().Add_Point(Extent.Get_XMin   (), Extent.Get_YMin   ())
            _AOI.Add_Shape().Add_Point(Extent.Get_XMin   (), Extent.Get_YCenter())
            _AOI.Add_Shape().Add_Point(Extent.Get_XMin   (), Extent.Get_YMax   ())
            _AOI.Add_Shape().Add_Point(Extent.Get_XCenter(), Extent.Get_YMax   ())
            _AOI.Add_Shape().Add_Point(Extent.Get_XMax   (), Extent.Get_YMax   ())
            _AOI.Add_Shape().Add_Point(Extent.Get_XMax   (), Extent.Get_YCenter())
            _AOI.Add_Shape().Add_Point(Extent.Get_XMax   (), Extent.Get_YMin   ())
            _AOI.Add_Shape().Add_Point(Extent.Get_XCenter(), Extent.Get_YMin   ())
            if not saga_api.SG_Get_Projected(_AOI, None, saga_api.CSG_Projections().Get_GCS_WGS84()):
                del(_AOI); saga_api.SG_UI_Msg_Add_Error('failed to project AOI to GCS')
                return None
            Extent = _AOI.Get_Extent(); Extent.Inflate(10 * 30 / 3600, False)
            del(_AOI)

        #________________________________________________________________________
        Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_gdal', '0')
        if not Tool:
            saga_api.SG_UI_Msg_Add_Error('failed to request tool \'{:s}\''.format('Import Raster'))
            return None

        Tool.Reset()
        Tool.Set_Parameter('FILES', File)
        Tool.Set_Parameter('EXTENT', 1) # 'user defined'
        Tool.Set_Parameter('EXTENT_XMIN', Extent.Get_XMin())
        Tool.Set_Parameter('EXTENT_XMAX', Extent.Get_XMax())
        Tool.Set_Parameter('EXTENT_YMIN', Extent.Get_YMin())
        Tool.Set_Parameter('EXTENT_YMAX', Extent.Get_YMax())

        if not Tool.Execute():
            saga_api.SG_UI_Msg_Add_Error('failed to execute tool \'{:s}\''.format(Tool.Get_Name().c_str()))
            return None

        Grid = Tool.Get_Parameter('GRIDS').asGridList().Get_Grid(0)

        if Grid.Get_Type() == saga_api.SG_DATATYPE_Short and Grid.Get_NoData_Value() < -32767:
            Grid.Set_NoData_Value(-32767)

        if AOI.Get_Projection().is_Geographic():
            return Grid

        #________________________________________________________________________
        Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('pj_proj4', '4')
        if not Tool:
            saga_api.SG_UI_Msg_Add_Error('failed to request tool \'{:s}\''.format('Coordinate Transformation (Grid)'))
            saga_api.SG_Get_Data_Manager().Delete(Grid)
            return None

        Extent = saga_api.CSG_Rect(AOI.Get_Extent()); Round_Extent(Extent)

        Tool.Reset()
        Tool.Set_Parameter('CRS_METHOD', 0)  # 'Proj4 Parameters'
        Tool.Set_Parameter('CRS_PROJ4' , AOI.Get_Projection().Get_Proj4())
        Tool.Set_Parameter('SOURCE'    , Grid)
        Tool.Set_Parameter('RESAMPLING', 3)  # B-Spline
        Tool.Set_Parameter('DATA_TYPE' , 10) # Preserve
        Tool.Set_Parameter('TARGET_DEFINITION', 0) # 'user defined'
        Tool.Set_Parameter('TARGET_USER_SIZE', 1000.)
        Tool.Set_Parameter('TARGET_USER_XMAX', Extent.xMax)
        Tool.Set_Parameter('TARGET_USER_XMIN', Extent.xMin)
        Tool.Set_Parameter('TARGET_USER_YMAX', Extent.yMax)
        Tool.Set_Parameter('TARGET_USER_YMIN', Extent.yMin)

        if not Tool.Execute():
            saga_api.SG_UI_Msg_Add_Error('failed to execute tool \'{:s}\''.format(Tool.Get_Name().c_str()))
            saga_api.SG_Get_Data_Manager().Delete(Grid)
            return None

        saga_api.SG_Get_Data_Manager().Delete(Grid)

        return Tool.Get_Parameter('GRID').asGrid()


    #############################################################################
    #____________________________________________________________________________
    if os.path.exists(Target_File):
        return True # has already been processed

    if not Global_File or not os.path.exists(Global_File):
        return False # download of original file seems to have failed

    #____________________________________________________________________________
    saga_api.SG_UI_Msg_Add('processing: {:s}... '.format(Target_File))

    saga_api.SG_UI_ProgressAndMsg_Lock(True) # suppress noise

    Grid = Import_Raster(Global_File, AOI)
    if Grid:
        Target_Dir = os.path.split(Target_File)[0]
        if not os.path.exists(Target_Dir):
            os.makedirs(Target_Dir)

        Grid.Set_Scaling(Scaling, Offset)
        Grid.Set_Unit(saga_api.CSG_String(Unit))
        Grid.Save(Target_File)

        saga_api.SG_Get_Data_Manager().Delete(Grid) # free memory

        if bDeleteGlobal and os.path.exists(Global_File):
            os.remove(Global_File)

        saga_api.SG_UI_Msg_Add('okay', False)

    saga_api.SG_UI_ProgressAndMsg_Lock(False)

    return Grid != None


#################################################################################
#
#________________________________________________________________________________
