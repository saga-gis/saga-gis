#! /usr/bin/env python

import saga_api, saga_helper, sys, os


##########################################
def run_grid_asc2sgrd(File):

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_gdal', 0)
    Parm = Tool.Get_Parameters()
    Parm('FILES').Set_Value(File)

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    Grid = Parm('GRIDS').asGridList().Get_Grid(0)

    File = saga_api.CSG_String(File)
    saga_api.SG_File_Set_Extension(File, saga_api.CSG_String('sg-grd-z'))

    if Grid.Save(File) == False:
        print 'Error: saving grid [' + File.c_str() + ']'
        return False

    # -----------------------------------
    print 'Success'
    return True


##########################################
if __name__ == '__main__':

    saga_helper.Load_Tool_Libraries(True)

    # -----------------------------------
    if len(sys.argv) == 2:
        File = sys.argv[1]
    else:
        File = './dem.asc'
        print 'Usage: grid_asc_to_saga.py <in: ascii grid file>'
        print '... trying to run with dummy data'

    run_grid_asc2sgrd(File)
