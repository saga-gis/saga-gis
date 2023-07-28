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

#################################################################################
# Globals
#________________________________________________________________________________

import os; from PySAGA import saga_api

Dir_Global = os.getcwd() + '/srtm_global'

Download_Retries = 4


#################################################################################
#
# Providing the original SRTM files...
#________________________________________________________________________________

#________________________________________________________________________________
# This function checks if the requested 'File' exists in 'Local_Dir' and if not
# it tries to download the file from 'Remote_Dir'. The function returns the file
# path to the requested data set if it exists in 'Local_Dir' or 'None'.
#________________________________________________________________________________
def Get_File(File, Local_Dir, Remote_Dir):
    Local_File = '{:s}/{:s}'.format(Local_Dir, File)
    if os.path.exists(Local_File):
        return Local_File

    if not os.path.exists(Local_Dir):
        os.makedirs(Local_Dir)

    Remote_File = '{:s}/{:s}'.format(Remote_Dir, File)

    Retry = Download_Retries
    if Retry < 0:
        Retry = 0
    Retry += 1

    while Retry > 0:
        try:
            import wget
            Local_File = wget.download(Remote_File, Local_Dir) # returns full path to downloaded file or 'None'
            if Local_File:
                return Local_File
            Retry -= 1
        except: # remote file might not exist or internet connection is not available
            break

    print('Error: downloading ' + File)
    return None


#################################################################################
#
# Virtual Raster Tiles...
#________________________________________________________________________________

#________________________________________________________________________________
# This function creates or updates the virtual raster (VRT) file named 'File'
# using all raster tiles found in 'Dir_Global'. It will be stored in the same
# directory.
#________________________________________________________________________________
def Set_VRT(VRT_Name='srtm_global'):
    import glob; Files = glob.glob('{:s}/*.tif'.format(Dir_Global))
    if len(Files) < 1:
        print('Error: directory \'{:s}\' does not contain any TIFF!'.format(Dir_Global))
        return None

    Tiles = ''
    for File in Files:
        Tiles += '\"{:s}\" '.format(File)

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_gdal', '12')
    if not Tool:
        print('Failed to request tool: Create Virtual Raster (VRT)')
        return None

    Tool.Reset()
    Tool.Set_Parameter('FILES'   , Tiles)
    Tool.Set_Parameter('VRT_NAME', '{:s}/{:s}.vrt'.format(Dir_Global, VRT_Name))

    saga_api.SG_UI_ProgressAndMsg_Lock(True)
    if not Tool.Execute():
        saga_api.SG_UI_ProgressAndMsg_Lock(False)
        print('failed to execute tool: ' + Tool.Get_Name().c_str())
        return None

    saga_api.SG_UI_ProgressAndMsg_Lock(False)
    return Tool.Get_Parameter("VRT_NAME").asString()


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
    if Col < 1 or Col >= 72:
        print('Error: requested column {:d} is out-of-range (1-72)'.format(Row))
        return -1

    if Row < 1 or Row >= 24:
        print('Error: requested row {:d} is out-of-range (1-24)'.format(Col))
        return -1

    Local_File = '{:s}/srtm_{:02d}_{:02d}.tif'.format(Dir_Global, Col, Row)
    if os.path.exists(Local_File):
        return 0

    Remote_Dir = 'https://srtm.csi.cgiar.org/wp-content/uploads/files/srtm_5x5/TIFF'

    Zip_File = Get_File('srtm_{:02d}_{:02d}.zip'.format(Col, Row), Dir_Global, Remote_Dir)
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

    print('requesting tiles for rows {:d}-{:d} and columns {:d}-{:d}\n'.format(Rows[0], Rows[1], Cols[0], Cols[1]))
    nAdded = 0; nFailed = 0; nFound = 0
    for Col in range(Cols[0], Cols[1] + 1):
        for Row in range(Rows[0], Rows[1] + 1):
            if Col >= 1 and Col <= 72 and Row >= 1 and Row <= 24:
                Result = CGIAR_Get_Tile(Col, Row, DeleteZip)
                if Result < 0:
                    nAdded  += 1
                elif Result < 0:
                    nFailed += 1
                else:
                    nFound  += 1

    if nFailed > 0:
        print('Error: {:d} download(s) of {:d} failed'.format(nFailed, nFailed + nAdded))
    if nAdded > 0:
        Set_VRT()
    return nAdded + nFound > 0

#________________________________________________________________________________
def CGIAR_Get_Tiles_byExtent(Lon=[-180, 180], Lat=[-60, 60], DeleteZip=True):
    Cellsize = 3. / 3600.; Lon[0] -= Cellsize; Lon[1] += Cellsize; Lat[0] -= Cellsize; Lat[1] += Cellsize

    Cols = [1 + int((Lon[0] + 180.) / 5.), 1 + int((Lon[1] + 180.) / 5.)]
    Rows = [1 + int(( 60. - Lat[1]) / 5.), 1 + int(( 60. - Lat[0]) / 5.)]

    return CGIAR_Get_Tiles(Cols, Rows, DeleteZip)

#________________________________________________________________________________
def CGIAR_Get_AOI(AOI, Target_File, Target_Resolution=90, DeleteZip=True, Verbose=False):

    #____________________________________________________________________________
    def Import_Raster():
        if not AOI or not AOI.is_Valid() or not AOI.Get_Projection().is_Okay():
            print('Error: invalid AOI')
            return None

        Extent = saga_api.CSG_Rect(AOI.Get_Extent())
        if not AOI.Get_Projection().is_Geographic():
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
                del(_AOI); print('Error: failed to project AOI to GCS')
                return None
            Extent = _AOI.Get_Extent()#; Extent.Inflate(10 * 3 / 3600, False)
            del(_AOI)

        Lon = [Extent.Get_XMin(), Extent.Get_XMax()]; Lat=[Extent.Get_YMin(), Extent.Get_YMax()]

        #________________________________________________________________________
        if not CGIAR_Get_Tiles_byExtent([Extent.Get_XMin(), Extent.Get_XMax()], [Extent.Get_YMin(), Extent.Get_YMax()], DeleteZip):
            print('Error: failed to update virtual raster tiles for requested extent')
            return None

        #________________________________________________________________________
        Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_gdal', '0')
        if not Tool:
            print('Error: failed to request tool \'{:s}\''.format('Import Raster'))
            return None

        Tool.Reset()
        Tool.Set_Parameter('FILES', '{:s}/srtm_global.vrt'.format(Dir_Global))
        Tool.Set_Parameter('EXTENT', 1) # 'user defined'
        Tool.Set_Parameter('EXTENT_XMIN', Lon[0])
        Tool.Set_Parameter('EXTENT_XMAX', Lon[1])
        Tool.Set_Parameter('EXTENT_YMIN', Lat[0])
        Tool.Set_Parameter('EXTENT_YMAX', Lat[1])

        if not Tool.Execute():
            print('Error: failed to execute tool \'{:s}\''.format(Tool.Get_Name().c_str()))
            return None

        Grid = Tool.Get_Parameter('GRIDS').asGridList().Get_Grid(0)
        if AOI.Get_Projection().is_Geographic():
            return Grid

        #________________________________________________________________________
        Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('pj_proj4', '4')
        if not Tool:
            print('Error: failed to request tool \'{:s}\''.format('Coordinate Transformation (Grid)'))
            saga_api.SG_Get_Data_Manager().Delete(Grid)
            return None

        Tool.Reset()
        Tool.Set_Parameter('CRS_METHOD', 0) # 'Proj4 Parameters'
        Tool.Set_Parameter('CRS_PROJ4' , AOI.Get_Projection().Get_Proj4())
        Tool.Set_Parameter('SOURCE'    , Grid)
        Tool.Set_Parameter('KEEP_TYPE' , False)
        Tool.Set_Parameter('TARGET_DEFINITION', 0) # 'user defined'
        Tool.Set_Parameter('TARGET_USER_SIZE', Target_Resolution)
        Tool.Set_Parameter('TARGET_USER_XMAX', AOI.Get_Extent().Get_XMax())
        Tool.Set_Parameter('TARGET_USER_XMIN', AOI.Get_Extent().Get_XMin())
        Tool.Set_Parameter('TARGET_USER_YMAX', AOI.Get_Extent().Get_YMax())
        Tool.Set_Parameter('TARGET_USER_YMIN', AOI.Get_Extent().Get_YMin())

        if not Tool.Execute():
            print('Error: failed to execute tool \{:s}\''.format(Tool.Get_Name().c_str()))
            saga_api.SG_Get_Data_Manager().Delete(Grid)
            return None

        saga_api.SG_Get_Data_Manager().Delete(Grid)

        return Tool.Get_Parameter('GRID').asGrid()


    #############################################################################
    #____________________________________________________________________________
    print('\nprocessing: {:s}...'.format(Target_File))

    if not Verbose:
        saga_api.SG_UI_ProgressAndMsg_Lock(True) # suppress noise

    Grid = Import_Raster()
    if Grid:
        Target_Dir = os.path.split(Target_File)[0]
        if not os.path.exists(Target_Dir):
            os.makedirs(Target_Dir)

        Grid.Save(Target_File)

        saga_api.SG_Get_Data_Manager().Delete(Grid) # free memory

        print('okay')

    if not Verbose:
        saga_api.SG_UI_ProgressAndMsg_Lock(False)

    return Grid != None


#################################################################################
#
# Defining the Area of Interest...
#________________________________________________________________________________

#################################################################################
# Create area of interest from extent coordinates and EPSG code for CRS
#________________________________________________________________________________
def Get_AOI_From_Extent(Xmin, Xmax, Ymin, Ymax, EPSG=4326):
    AOI = saga_api.CSG_Shapes(saga_api.SHAPE_TYPE_Polygon)
    AOI.Get_Projection().Create(EPSG)
    Shape = AOI.Add_Shape()
    Shape.Add_Point(Xmin, Ymin)
    Shape.Add_Point(Xmin, Ymax)
    Shape.Add_Point(Xmax, Ymax)
    Shape.Add_Point(Xmax, Ymin)
    return AOI


#################################################################################
# Create area of interest from file. Expects to represent vector data with CRS
# information set correctly.
#________________________________________________________________________________
def Get_AOI_From_Features(File):
    AOI = saga_api.SG_Create_Shapes(File)
    if not AOI:
        print('Error: failed to load AOI from file \n\t\'{:s}\''.format(File))
        return None

    if not AOI.Get_Projection().is_Okay():
        del(AOI)
        print('Error: coordinate reference system of AOI is not defined \n\t\'{:s}\''.format(File))
        return None

    return AOI


#################################################################################
# Create area of interest from file. Expects to represent raster data with CRS
# information set correctly.
#________________________________________________________________________________
def Get_AOI_From_Raster(File):
    Grid = saga_api.SG_Create_Grid(File)
    if not Grid:
        print('Error: failed to load AOI from file \n\t\'{:s}\''.format(File))
        return None

    if not Grid.Get_Projection().is_Okay():
        del(Grid)
        print('Error: coordinate reference system of AOI is not defined \n\t\'{:s}\''.format(File))
        return None

    AOI = saga_api.CSG_Shapes(saga_api.SHAPE_TYPE_Polygon)
    AOI.Get_Projection().Create(Grid.Get_Projection())
    Shape = AOI.Add_Shape()
    Shape.Add_Point(Grid.Get_XMin(), Grid.Get_YMin())
    Shape.Add_Point(Grid.Get_XMin(), Grid.Get_YMax())
    Shape.Add_Point(Grid.Get_XMax(), Grid.Get_YMax())
    Shape.Add_Point(Grid.Get_XMax(), Grid.Get_YMin())
    del(Grid)
    return AOI


#################################################################################
#
# Basic usage...
#________________________________________________________________________________

# from PySAGA.data import srtm
# srtm.Dir_Global = 'C:/srtm_3arcsec/global'
# AOI = srtm.Get_AOI_From_Extent(279000, 920000, 5235000, 6102000, 32632)
# srtm.CGIAR_Get_AOfInterest(AOI, 'C:/srtm_3arcsec/germany_utm32n.sg-grd-z')
