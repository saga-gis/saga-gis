#! /usr/bin/env python

import sys, os, saga_helper, saga_api


##########################################
def run_grid_create_dummy(File):

    # -----------------------------------
    # Create a dummy grid

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('grid_calculus', 6)

    Tool.Set_Parameter('RADIUS'          ,   25)
    Tool.Set_Parameter('ITERATIONS'      ,  250)
    Tool.Set_Parameter('TARGET_USER_SIZE',   10)
    Tool.Set_Parameter('TARGET_USER_XMAX', 2000)
    Tool.Set_Parameter('TARGET_USER_YMAX', 2000)

    if Tool.Execute() == False:
        print('Error: executing tool [' + Tool.Get_Name().c_str() + ']')
        return False

    Grid = Tool.Get_Parameter('TARGET_OUT_GRID').asGrid()

    # -----------------------------------
    # Save dummy to esri ascii grid file

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_grid', 0)

    Tool.Set_Parameter('GRID', Grid)
    Tool.Set_Parameter('FILE', File)
    Tool.Set_Parameter('PREC',    2)

    if Tool.Execute() == False:
        print('Error: executing tool [' + Tool.Get_Name().c_str() + ']')
        return False

    # -----------------------------------
    print('Success')
    return True
	

##########################################
if __name__ == '__main__':

    saga_helper.Initialize(True)

    # -----------------------------------
    if len(sys.argv) == 2:
        File = sys.argv[1]
    else:
        File = './dem.asc'
        print('Usage: 00_grid_create_dummy.py <out: ascii grid file>')
        print('... trying to save as ''dem.asc'' in current working directory')

    run_grid_create_dummy(File)
