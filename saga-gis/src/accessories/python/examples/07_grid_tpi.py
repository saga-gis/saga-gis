#! /usr/bin/env python

import saga_api, saga_helper, sys, os


##########################################
def grid_tpi(File):

    # ------------------------------------
    Grid = saga_api.SG_Get_Data_Manager().Add_Grid(File)
    if Grid == None or Grid.is_Valid() == False:
        print 'Error: loading grid [' + File + ']'
        return False

    # ------------------------------------
    # 'TPI Based Landform Classification'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('ta_morphometry'), 19)
    Parm = Tool.Get_Parameters()
    Parm.Reset_Grid_System() # grid tool needs to use conformant grid system!
    Parm('DEM'         ).Set_Value(Grid)
    Parm('DW_WEIGHTING').Set_Value(0)
    Parm('RADIUS_A'    ).asRange().Set_Range(0,  2 * Grid.Get_Cellsize())
    Parm('RADIUS_B'    ).asRange().Set_Range(0, 10 * Grid.Get_Cellsize())

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    Grid = Parm('LANDFORMS').asGrid()
    File = os.path.split(File)[0] + '/landforms.sg-grd-z'
    if Grid.Save(File) == False:
        print 'Error: saving grid [' + File + ']'
        return False

    # ------------------------------------
    print 'Success'    
    return True


##########################################
if __name__ == '__main__':

    saga_helper.Load_Tool_Libraries(True)

    # -----------------------------------
    if len( sys.argv ) == 2:
        File = sys.argv[1]
    else:
        File = './dem.sg-grd-z'
        print 'Usage: 07_grid_tpi.py <in: elevation>'
        print '... trying to run with dummy data'

    grid_tpi(File)
