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
The PySAGA.data.copernicus_dem module provides easy-to-use functions for accessing
``Copernicus DEM`` global elevation data with either 90 meter (3 arcsec) or
30 meter (1 arcsec) resolution. For more information on the data refer to
    https://sentinels.copernicus.eu/web/sentinel/-/copernicus-dem-new-direct-data-download-access/

For downloading requested files the wget Python package is used, which needs to be
installed in order to work. Installation can be done through pip:
    pip install wget

Basic usage:
    import PySAGA.helper; AOI = PySAGA.helper.Get_AOI_From_Extent(279000, 920000, 5235000, 6102000, 32632)

    from PySAGA.data import copernicus_dem

    copernicus_dem.Dir_Global = 'C:/Copernicus_DSM_10/_global'

    copernicus_dem.DSM_10_Get_AOI(AOI,
        'C:/Copernicus_DSM_10/germany_utm32n_30m.sg-grd-z'
    )
'''


#################################################################################
#
# Globals
#________________________________________________________________________________

import os; from PySAGA import saga_api

Dir_Global = os.getcwd() + '/copernicus_global'

VRT_Name = 'dem_global'

Version = '2022_1' # Copernicus DEM version string, currently either '2021_1' or '2022_1'

Download_Retries = 4


#################################################################################
#
# DSM_10 (=> 30m)...
#________________________________________________________________________________

#________________________________________________________________________________
def Get_Tile(Lon, Lat, SrcRes_1arcsec=True, DeleteArchive=True):
    '''
    Downloads the ``Copernicus DSM 10`` tile with the lower left corner coordinate
    as specified by *Lon* (longitude in the range of -180 to 179) and *Lat*
    (latitude in the range of -90 to 89).
    Returns 0 if tile already exists in the local storage path, 1 if it has
    been download successfully, or -1 if the download failed. Local storage path is
    defined by the global variable *Dir_Global* and defaults to 'global' subfolder
    relative to the current working directory.
    '''

    if Lon < -180 or Lon >= 180:
        saga_api.SG_UI_Console_Print_StdErr('requested longitude {:d} is out-of-range'.format(Lon))
        return -1

    if Lat <  -90 or Lat >=  90:
        saga_api.SG_UI_Console_Print_StdErr('requested latitude {:d} is out-of-range'.format(Lat))
        return -1

    if Lon < 0:
        Easting  = 'W{:03d}_00'.format(abs(int(Lon)))
    else:
        Easting  = 'E{:03d}_00'.format(    int(Lon) )

    if Lat < 0:
        Northing = 'S{:02d}_00'.format(abs(int(Lat)))
    else:
        Northing = 'N{:02d}_00'.format(    int(Lat) )

    if SrcRes_1arcsec:
        DEM_SrcRes = '30'; DEM_Name = 'Copernicus_DSM_10_{:s}_{:s}'.format(Northing, Easting)
    else:
        DEM_SrcRes = '90'; DEM_Name = 'Copernicus_DSM_30_{:s}_{:s}'.format(Northing, Easting)

    DEM_File = '{:s}_DEM.tif'.format(DEM_Name)
    if os.path.exists('{:s}/{:s}'.format(Dir_Global, DEM_File)):
        return 0

    Tar_File = '{:s}/{:s}.tar'.format(Dir_Global, DEM_Name)
    if not os.path.exists(Tar_File):
        Remote_Root = 'https://prism-dem-open.copernicus.eu/pd-desk-open-access/prismDownload' 
        Remote_Dir = '{:s}/COP-DEM_GLO-{:s}-DGED__{:s}'.format(Remote_Root, DEM_SrcRes, Version)
        import PySAGA.data.helper
        Tar_File = PySAGA.data.helper.Get_File('{:s}.tar'.format(DEM_Name), Dir_Global, Remote_Dir, Download_Retries)
        if not Tar_File:
            return -1

    import tarfile; tf = tarfile.TarFile(Tar_File, 'r')

    buf = tf.extractfile('{:s}/DEM/{:s}'.format(DEM_Name, DEM_File))
    if not buf:
        tf.close(); return -1

    out = open('{:s}/{:s}'.format(Dir_Global, DEM_File), 'wb')
    if not out:
        tf.close(); return -1

    out.write(buf.read()); out.close(); tf.close()

    if DeleteArchive:
        os.remove(Tar_File)
    return 1

#________________________________________________________________________________
def Get_Tiles(Lon=[0, 0], Lat=[0, 0], SrcRes_1arcsec=True, DeleteArchive=True):
    '''
    Downloads all Copernicus DSM 10 tiles in the given longitudinal
    and latitudinal range.
    '''

    if Lon[1] < Lon[0]:
        d = Lon[0]; Lon[0] = Lon[1]; Lon[1] = d
    if Lon[0] < -180:
        Lon[0] = -180
    if Lon[1] >= 180:
        Lon[1] = 179

    if Lat[1] < Lat[0]:
        d = Lat[0]; Lat[0] = Lat[1]; Lon[1] = d
    if Lat[0] < -90:
        Lat[0] = -90
    elif Lat[1] >= 90:
        Lat[1] = 89

    from math import floor, ceil
    saga_api.SG_UI_Msg_Add('requesting tiles for latitude range {:f}-{:f} and longitude range {:f}-{:f}\n'.format(Lat[0], Lat[1], Lon[0], Lon[1]))
    nAdded = 0; nFailed = 0; nFound = 0
    for iLon in range(floor(Lon[0]), ceil(Lon[1])):
        for iLat in range(floor(Lat[0]), ceil(Lat[1])):
            if iLon >= -180 and iLon < 180 and iLat >= -90 and iLat < 90:
                Result = Get_Tile(iLon, iLat, SrcRes_1arcsec, DeleteArchive)
                if Result > 0:
                    nAdded  += 1
                elif Result < 0:
                    nFailed += 1
                else:
                    nFound  += 1

    if nFailed > 0:
        saga_api.SG_UI_Console_Print_StdErr('{:d} download(s) of {:d} failed'.format(nFailed, nFailed + nAdded))

    if nAdded > 0 or not os.path.exists('{:s}/{:s}.vrt'.format(Dir_Global, VRT_Name)):
        import PySAGA.helper; PySAGA.helper.Get_VRT(Dir_Global, 'tif', VRT_Name)

    return nAdded + nFound > 0

#________________________________________________________________________________
def Get_Tiles_byExtent(Lon=[-180, 180], Lat=[-90, 90], SrcRes_1arcsec=True, DeleteArchive=True):
    '''
    Downloads all Copernicus DSM 10 tiles for the given longitudinal and latitudinal range.
    '''

    if SrcRes_1arcsec:
        Cellsize = 1. / 3600.
    else:
        Cellsize = 3. / 3600.

    Lon[0] -= Cellsize; Lon[1] += Cellsize; Lat[0] -= Cellsize; Lat[1] += Cellsize

    return Get_Tiles(Lon, Lat, SrcRes_1arcsec, DeleteArchive)

#________________________________________________________________________________
def Get_AOI(AOI, Target_File, Target_Resolution=0, SrcRes_1arcsec=True, DeleteArchive=True, Verbose=False):
    '''
    Downloads, clips and projects ``Copernicus DSM 10`` data matching the desired area of interest
    (*AOI*) including the resampling to the specified *Target_Resolution*. The *AOI* has to be
    supplied as saga_api.CSG_Shapes object. The resampling is only involved if the area of interest
    is in a projected coordinate system (i.e. not geographic coordinates). If *Target_Resolution*
    is zero (the default) the appropriate resolution is chosen according to the selected source data
    resolution. This is controlled with the *SrcRes_1arcsec* flag and refers to either 30 meters
    (1 arcsec) or 90 meters (3 arcsec).
    '''

    #____________________________________________________________________________
    def Import_Raster():
        if not AOI or not AOI.is_Valid() or not AOI.Get_Projection().is_Okay():
            saga_api.SG_UI_Console_Print_StdErr('invalid AOI')
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
                del(_AOI); saga_api.SG_UI_Console_Print_StdErr('failed to project AOI to GCS')
                return None
            Extent = _AOI.Get_Extent()#; Extent.Inflate(10 * 3 / 3600, False)
            del(_AOI)

        Lon = [Extent.Get_XMin(), Extent.Get_XMax()]; Lat=[Extent.Get_YMin(), Extent.Get_YMax()]

        #________________________________________________________________________
        if not Get_Tiles_byExtent([Extent.Get_XMin(), Extent.Get_XMax()], [Extent.Get_YMin(), Extent.Get_YMax()], SrcRes_1arcsec, DeleteArchive):
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

        Resolution = Target_Resolution
        if Resolution <= 0:
            if SrcRes_1arcsec:
                Resolution = 30
            else:
                Resolution = 90

        Tool.Reset()
        Tool.Set_Parameter('CRS_METHOD', 0) # 'Proj4 Parameters'
        Tool.Set_Parameter('CRS_PROJ4' , AOI.Get_Projection().Get_Proj4())
        Tool.Set_Parameter('SOURCE'    , Grid)
        Tool.Set_Parameter('KEEP_TYPE' , False)
        Tool.Set_Parameter('TARGET_DEFINITION', 0) # 'user defined'
        Tool.Set_Parameter('TARGET_USER_SIZE', Resolution)
        Tool.Set_Parameter('TARGET_USER_XMAX', AOI.Get_Extent().Get_XMax())
        Tool.Set_Parameter('TARGET_USER_XMIN', AOI.Get_Extent().Get_XMin())
        Tool.Set_Parameter('TARGET_USER_YMAX', AOI.Get_Extent().Get_YMax())
        Tool.Set_Parameter('TARGET_USER_YMIN', AOI.Get_Extent().Get_YMin())

        if not Tool.Execute():
            saga_api.SG_UI_Console_Print_StdErr('failed to execute tool \{:s}\''.format(Tool.Get_Name().c_str()))
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

        saga_api.SG_UI_Msg_Add('okay', False)

    if not Verbose:
        saga_api.SG_UI_ProgressAndMsg_Lock(False)

    return Grid != None


#################################################################################
#
#________________________________________________________________________________
