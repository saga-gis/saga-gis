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
The PySAGA.data.srtm module provides easy-to-use functions for accessing
``SRTM (Shuttle Radar Topography Mission)`` global elevation data.

Currently support is only given for the SRTM 90m DEM Digital Elevation
Database provided by CGIAR CSI. For more information on the data refer to
    https://srtm.csi.cgiar.org/

For downloading requested files the wget Python package is used, which needs to be
installed in order to work. Installation can be done through pip:
    pip install wget

Basic usage:
    import PySAGA.data.srtm as srtm, PySAGA.helper as saga_helper

    srtm.Dir_Global = 'C:/srtm_3arcsec/_global'

    srtm.CGIAR_Get_AOI(
        saga_helper.Get_AOI_From_Extent(279000, 920000, 5235000, 6102000, 32632),
        'C:/srtm_3arcsec/germany_utm32n_1000m.sg-grd-z', 1000
    )
'''


#################################################################################
#
# Globals
#________________________________________________________________________________

import os, PySAGA.saga_api as saga_api, PySAGA.helper, PySAGA.data.helper

Dir_Global = os.getcwd() + '/srtm_global'

VRT_Name = 'srtm_global'

Download_Retries = 4


#################################################################################
#
# Providing the original SRTM files...
#________________________________________________________________________________

#################################################################################
#
# CGIAR SRTM 3''...
#________________________________________________________________________________

#________________________________________________________________________________
# Returns 0 if tile is already present in the local storage path, 1 if it has
# been download successfully, or -1 if the download failed. Local storage path is
# defined by the global 'Dir_Global' variable and defaults to 'global' subfolder
# relative to the current working directory.
#________________________________________________________________________________
def CGIAR_Get_Tile(Col, Row, DeleteZip=True):
    '''
    Downloads the ``SRTM 3arcsec`` tile for the specified column (range 1 to 72)
    and row (range 1 to 24).
    Returns 0 if tile is already present in the local storage path, 1 if it has
    been download successfully, or -1 if the download failed. Local storage path is
    defined by the global 'Dir_Global' variable and defaults to 'global' subfolder
    relative to the current working directory.
    '''

    if Col < 1 or Col >= 72:
        saga_api.SG_UI_Console_Print_StdErr('requested column {:d} is out-of-range (1-72)'.format(Row))
        return -1

    if Row < 1 or Row >= 24:
        saga_api.SG_UI_Console_Print_StdErr('requested row {:d} is out-of-range (1-24)'.format(Col))
        return -1

    Local_File = '{:s}/srtm_{:02d}_{:02d}.tif'.format(Dir_Global, Col, Row)
    if os.path.exists(Local_File):
        return 0

    Remote_Dir = 'https://srtm.csi.cgiar.org/wp-content/uploads/files/srtm_5x5/TIFF'

    Zip_File = PySAGA.data.helper.Get_File('srtm_{:02d}_{:02d}.zip'.format(Col, Row), Dir_Global, Remote_Dir)
    if not Zip_File:
        return -1

    import zipfile; zf = zipfile.ZipFile(Zip_File, 'r')
    Local_File = zf.extract('srtm_{:02d}_{:02d}.tif'.format(Col, Row), Dir_Global)
    zf.close()
    if DeleteZip:
        os.remove(Zip_File)
    return 1

#________________________________________________________________________________
def CGIAR_Get_Tiles(Cols=[1, 1], Rows=[1, 1], DeleteZip=True):
    '''
    Downloads all SRTM 3arcsec tiles for the given column and row span.
    '''

    if Cols[1] < Cols[0]:
        Col = Cols[0]; Cols[0] = Cols[1]; Cols[1] = Col
    if Cols[0] < 1:
        Cols[0] = 1
    elif Cols[1] > 72:
        Cols[1] = 72

    if Rows[1] < Rows[0]:
        Row = Rows[0]; Rows[0] = Rows[1]; Cols[1] = Row
    if Rows[0] < 1:
        Rows[0] = 1
    elif Rows[1] > 24:
        Rows[1] = 24

    saga_api.SG_UI_Msg_Add('requesting tiles for rows {:d}-{:d} and columns {:d}-{:d}\n'.format(Rows[0], Rows[1], Cols[0], Cols[1]))
    nAdded = 0; nFailed = 0; nFound = 0
    for Col in range(Cols[0], Cols[1] + 1):
        for Row in range(Rows[0], Rows[1] + 1):
            if Col >= 1 and Col <= 72 and Row >= 1 and Row <= 24:
                Result = CGIAR_Get_Tile(Col, Row, DeleteZip)
                if Result > 0:
                    nAdded  += 1
                elif Result < 0:
                    nFailed += 1
                else:
                    nFound  += 1

    if nFailed > 0:
        saga_api.SG_UI_Console_Print_StdErr('{:d} download(s) of {:d} failed'.format(nFailed, nFailed + nAdded))

    if nAdded > 0 or not os.path.exists('{:s}/{:s}.vrt'.format(Dir_Global, VRT_Name)):
        PySAGA.helper.Get_VRT(Dir_Global, 'tif', VRT_Name)

    return nAdded + nFound > 0

#________________________________________________________________________________
def CGIAR_Get_Tiles_byExtent(Lon=[-180, 180], Lat=[-60, 60], DeleteZip=True):
    '''
    Downloads all SRTM 3arcsec tiles for the given longitudinal and latitudinal range.
    '''

    Cellsize = 3. / 3600.; Lon[0] -= Cellsize; Lon[1] += Cellsize; Lat[0] -= Cellsize; Lat[1] += Cellsize

    Cols = [1 + int((Lon[0] + 180.) / 5.), 1 + int((Lon[1] + 180.) / 5.)]
    Rows = [1 + int(( 60. - Lat[1]) / 5.), 1 + int(( 60. - Lat[0]) / 5.)]

    return CGIAR_Get_Tiles(Cols, Rows, DeleteZip)

#________________________________________________________________________________
def CGIAR_Get_AOI(AOI, Target_File, Target_Resolution=90., Round=True, DeleteZip=True, Verbose=False):
    '''
    Downloads, clips and projects ``SRTM 3arcsec`` data matching the desired area of interest
    (*AOI*) including the resampling to the specified *Target_Resolution*.
    '''

    #____________________________________________________________________________
    def Round_Extent(Extent, Resolution):
        if Round:
            from math import floor, ceil
            Extent.xMin = Resolution * floor(Extent.xMin / Resolution)
            Extent.xMax = Resolution * ceil (Extent.xMax / Resolution)
            Extent.yMin = Resolution * floor(Extent.yMin / Resolution)
            Extent.yMax = Resolution * ceil (Extent.yMax / Resolution)
        return True

    def Import_Raster():
        if not AOI or not AOI.is_Valid() or not AOI.Get_Projection().is_Okay():
            saga_api.SG_UI_Console_Print_StdErr('invalid AOI')
            return None

        Extent = saga_api.CSG_Rect(AOI.Get_Extent())
        if not AOI.Get_Projection().is_Geographic():
            Round_Extent(Extent, Target_Resolution)
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
                del(_AOI); saga_api.SG_UI_Console_Print_StdErr('failed to project AOI to GCS')
                return None
            Extent = _AOI.Get_Extent()#; Extent.Inflate(10 * 3 / 3600, False)
            del(_AOI)

        Lon = [Extent.Get_XMin(), Extent.Get_XMax()]; Lat=[Extent.Get_YMin(), Extent.Get_YMax()]

        #________________________________________________________________________
        if not CGIAR_Get_Tiles_byExtent([Extent.Get_XMin(), Extent.Get_XMax()], [Extent.Get_YMin(), Extent.Get_YMax()], DeleteZip):
            saga_api.SG_UI_Console_Print_StdErr('failed to update virtual raster tiles for requested extent')
            return None

        #________________________________________________________________________
        Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_gdal', '0')
        if not Tool:
            saga_api.SG_UI_Console_Print_StdErr('failed to request tool \'{:s}\''.format('Import Raster'))
            return None

        Tool.Reset()
        Tool.Set_Parameter('FILES', '{:s}/{:s}.vrt'.format(Dir_Global, VRT_Name))
        Tool.Set_Parameter('EXTENT', 1) # 'user defined'
        Tool.Set_Parameter('EXTENT_XMIN', Lon[0])
        Tool.Set_Parameter('EXTENT_XMAX', Lon[1])
        Tool.Set_Parameter('EXTENT_YMIN', Lat[0])
        Tool.Set_Parameter('EXTENT_YMAX', Lat[1])

        if not Tool.Execute():
            saga_api.SG_UI_Console_Print_StdErr('failed to execute tool \'{:s}\''.format(Tool.Get_Name().c_str()))
            return None

        Grid = Tool.Get_Parameter('GRIDS').asGridList().Get_Grid(0)
        if AOI.Get_Projection().is_Geographic():
            return Grid

        #________________________________________________________________________
        Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('pj_proj4', '4')
        if not Tool:
            saga_api.SG_UI_Console_Print_StdErr('failed to request tool \'{:s}\''.format('Coordinate Transformation (Grid)'))
            saga_api.SG_Get_Data_Manager().Delete(Grid)
            return None

        Extent = saga_api.CSG_Rect(AOI.Get_Extent()); Round_Extent(Extent, Target_Resolution)

        Tool.Reset()
        Tool.Set_Parameter('CRS_METHOD', 0)  # 'Proj4 Parameters'
        Tool.Set_Parameter('CRS_PROJ4' , AOI.Get_Projection().Get_Proj4())
        Tool.Set_Parameter('SOURCE'    , Grid)
        Tool.Set_Parameter('RESAMPLING', 3)  # B-Spline
        Tool.Set_Parameter('DATA_TYPE' , 10) # Preserve
        Tool.Set_Parameter('TARGET_DEFINITION', 0) # 'user defined'
        Tool.Set_Parameter('TARGET_USER_SIZE', Target_Resolution)
        Tool.Set_Parameter('TARGET_USER_XMAX', Extent.xMax)
        Tool.Set_Parameter('TARGET_USER_XMIN', Extent.xMin)
        Tool.Set_Parameter('TARGET_USER_YMAX', Extent.yMax)
        Tool.Set_Parameter('TARGET_USER_YMIN', Extent.yMin)

        if not Tool.Execute():
            saga_api.SG_UI_Console_Print_StdErr('failed to execute tool \'{:s}\''.format(Tool.Get_Name().c_str()))
            saga_api.SG_Get_Data_Manager().Delete(Grid)
            return None

        saga_api.SG_Get_Data_Manager().Delete(Grid)

        return Tool.Get_Parameter('GRID').asGrid()


    #############################################################################
    #____________________________________________________________________________
    saga_api.SG_UI_Console_Print_StdOut('processing: {:s}... '.format(Target_File))

    if not Verbose:
        saga_api.SG_UI_ProgressAndMsg_Lock(True) # suppress noise

    Grid = Import_Raster()
    if Grid:
        Target_Dir = os.path.split(Target_File)[0]
        if not os.path.exists(Target_Dir):
            os.makedirs(Target_Dir)

        Grid.Save(Target_File)

        saga_api.SG_Get_Data_Manager().Delete(Grid) # free memory

        saga_api.SG_UI_Console_Print_StdOut('okay', False)

    if not Verbose:
        saga_api.SG_UI_ProgressAndMsg_Lock(False)

    return Grid != None


#################################################################################
#
#________________________________________________________________________________
