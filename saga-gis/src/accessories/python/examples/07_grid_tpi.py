#! /usr/bin/env python

import sys, os, saga_helper, saga_api


##########################################
def grid_tpi(File):

    # ------------------------------------
    Grid = saga_api.SG_Get_Data_Manager().Add_Grid(File)
    if Grid == None or Grid.is_Valid() == False:
        print('Error: loading grid [' + File + ']')
        return False

    # ------------------------------------
    # 'TPI Based Landform Classification'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('ta_morphometry'), 19)

    Tool.Get_Parameters().Reset_Grid_System() # grid tool needs to use conformant grid system!
	
    Tool.Set_Parameter('DEM'         , Grid)
    Tool.Set_Parameter('DW_WEIGHTING',    0)
    Tool.Get_Parameter('RADIUS_A').asRange().Set_Range(0.,  2. * Grid.Get_Cellsize())
    Tool.Get_Parameter('RADIUS_B').asRange().Set_Range(0., 10. * Grid.Get_Cellsize())

    if Tool.Execute() == False:
        print('Error: executing tool [' + Tool.Get_Name().c_str() + ']')
        return False

    Grid = Tool.Get_Parameter('LANDFORMS').asGrid()
    File = os.path.split(File)[0] + '/landforms.sg-grd-z'
    if Grid.Save(File) == False:
        print('Error: saving grid [' + File + ']')
        return False

    # ------------------------------------
    print('Success')
    return True


##########################################
if __name__ == '__main__':

    saga_helper.Initialize(True)

    # -----------------------------------
    if len( sys.argv ) == 2:
        File = sys.argv[1]
    else:
        File = './dem.sg-grd-z'
        print('Usage: 07_grid_tpi.py <in: elevation>')
        print('... trying to run with dummy data')

    grid_tpi(File)
