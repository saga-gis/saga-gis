#! /usr/bin/env python

import sys, os, saga_helper, saga_api


##########################################
def grid_contour(File):

    # ------------------------------------
    Grid = saga_api.SG_Get_Data_Manager().Add_Grid(File)
    if Grid == None or Grid.is_Valid() == False:
        print('Error: loading grid [' + File + ']')
        return False

    # ------------------------------------
    # 'Contour Lines from Grid'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('shapes_grid', 5)

    Tool.Set_Parameter('GRID' , Grid)
    Tool.Set_Parameter('ZSTEP', Grid.Get_Range() / 10.)

    if Tool.Execute() == False:
        print('Error: executing tool [' + Tool.Get_Name().c_str() + ']')
        return False

    File = saga_api.CSG_String(File)
    saga_api.SG_File_Set_Extension(File, saga_api.CSG_String('geojson'))
    Tool.Get_Parameter('CONTOUR').asShapes().Save(File)

    # ------------------------------------
    print('Success')
    return True


##########################################
if __name__ == '__main__':

    saga_helper.Initialize(True)

    # -----------------------------------
    if len(sys.argv) == 2:
        File = sys.argv[1]
    else:
        File = './dem.sg-grd-z'
        print('Usage: grid_contour.py <in: grid>')
        print('... trying to run with test_data')

    grid_contour(File)
