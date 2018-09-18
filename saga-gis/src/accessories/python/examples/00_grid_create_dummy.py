#! /usr/bin/env python

import saga_api, sys, os

##########################################
def run_grid_create_dummy(fASC):

    ### create a dummy DEM:
    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('grid_calculus', 6)
    print Tool.Get_Name().c_str()

    p    = Tool.Get_Parameters()
    p('RADIUS'          ).Set_Value(  25)
    p('ITERATIONS'      ).Set_Value( 250)
    p('TARGET_USER_SIZE').Set_Value(  10)
    p('TARGET_USER_XMAX').Set_Value(2000)
    p('TARGET_USER_YMAX').Set_Value(2000)
    
    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    dem  = p('TARGET_OUT_GRID').asGrid()

    ### save dummy to esri ascii grid file:
    Tool    = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_grid', 0)
    print Tool.Get_Name().c_str()

    p    = Tool.Get_Parameters()
    p('GRID').Set_Value(dem)
    p('FILE').Set_Value(fASC)
    p('PREC').Set_Value(2)

    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    print 'success'
    return 1
	

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()

    saga_api.SG_UI_Msg_Lock(True)
    if os.name == 'nt':    # Windows
        os.environ['PATH'] = os.environ['PATH'] + ';' + os.environ['SAGA_32'] + '/dll'
        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_32' ] + '/tools', False)
    else:                  # Linux
        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_MLB'], False)
    saga_api.SG_UI_Msg_Lock(False)

    print 'number of loaded libraries: ' + str(saga_api.SG_Get_Tool_Library_Manager().Get_Count())
    print

    # ===================================
    if len(sys.argv) != 2:
        print 'Usage: 00_grid_create_dummy.py <out: ascii grid file>'
        print '... try to save as <dem.asc> in current working directory'
        fASC    = './dem.asc'
    else:
        fASC    = sys.argv[1]

    run_grid_create_dummy(fASC)
