#! /usr/bin/env python

import saga_api, saga_helper, sys, os


##########################################
def run_grid_create_dummy(File):

    # -----------------------------------
    # Create a dummy grid

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('grid_calculus', 6)
    Parm = Tool.Get_Parameters()
    Parm('RADIUS'          ).Set_Value(  25)
    Parm('ITERATIONS'      ).Set_Value( 250)
    Parm('TARGET_USER_SIZE').Set_Value(  10)
    Parm('TARGET_USER_XMAX').Set_Value(2000)
    Parm('TARGET_USER_YMAX').Set_Value(2000)

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    Grid = Parm('TARGET_OUT_GRID').asGrid()

    # -----------------------------------
    # Save dummy to esri ascii grid file

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_grid', 0)
    Parm = Tool.Get_Parameters()
    Parm('GRID').Set_Value(Grid)
    Parm('FILE').Set_Value(File)
    Parm('PREC').Set_Value(2)

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
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
        print 'Usage: 00_grid_create_dummy.py <out: ascii grid file>'
        print '... trying to save as ''dem.asc'' in current working directory'

    run_grid_create_dummy(File)
