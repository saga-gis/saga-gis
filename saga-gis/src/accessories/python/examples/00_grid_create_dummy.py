#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_create_dummy(fASC):

    print '__________________'
    print 'number of loaded libraries: ' + str(saga_api.SG_Get_Tool_Library_Manager().Get_Count())
    #print saga_api.SG_Get_Tool_Library_Manager().Get_Summary(saga_api.SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE).c_str()
    print '__________________'

    ### create a dummy DEM:
    m    = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('grid_calculus'), 6)
    print m.Get_Name().c_str()

    p    = m.Get_Parameters()
    p(saga_api.CSG_String('RADIUS'          )).Set_Value(  25)
    p(saga_api.CSG_String('ITERATIONS'      )).Set_Value( 250)
    p(saga_api.CSG_String('TARGET_USER_SIZE')).Set_Value(  10)
    p(saga_api.CSG_String('TARGET_USER_XMAX')).Set_Value(2000)
    p(saga_api.CSG_String('TARGET_USER_YMAX')).Set_Value(2000)
    
    if m.Execute() == 0:
        print 'ERROR: executing tool [' + m.Get_Name().c_str() + ']'
        return 0

    dem  = p(saga_api.CSG_String('TARGET_OUT_GRID')).asGrid()

    ### save dummy to esri ascii grid file:
    m    = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('io_grid'), 0)
    print m.Get_Name().c_str()

    p    = m.Get_Parameters()
    p(saga_api.CSG_String('GRID')).Set_Value(dem)
    p(saga_api.CSG_String('FILE')).Set_Value(saga_api.CSG_String(fASC))
    p(saga_api.CSG_String('PREC')).Set_Value(2)

    if m.Execute() == 0:
        print 'ERROR: executing tool [' + m.Get_Name().c_str() + ']'
        return 0

    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len(sys.argv) != 2:
        print 'Usage: 00_grid_create_dummy.py <out: ascii grid file>'
        print '... try to save as <test.asc> in current working directory'
        fASC    = './test.asc'
    else:
        fASC    = sys.argv[1]
        if os.path.split(fASC)[0] == '':
            fASC    = './' + fASC

    saga_api.SG_UI_Msg_Lock(True)
    if os.name == 'nt':    # Windows
        os.environ['PATH'] = os.environ['PATH'] + ';' + os.environ['SAGA_32'] + '/dll'
        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_32' ] + '/tools', False)
    else:                  # Linux
        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_MLB'], False)
    saga_api.SG_UI_Msg_Lock(False)
	
    grid_create_dummy(fASC)
