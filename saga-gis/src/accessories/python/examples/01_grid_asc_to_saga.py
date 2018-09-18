#! /usr/bin/env python

import saga_api, sys, os

##########################################
def run_grid_asc2sgrd(fASC):

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_gdal', 0)
    p    = Tool.Get_Parameters()
    p('FILES').Set_Value(fASC)

    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    sASC = saga_api.CSG_String(fASC)
    saga_api.SG_File_Set_Extension(sASC, saga_api.CSG_String('sg-grd-z'))
    if p('GRIDS').asGridList().Get_Grid(0).Save(sASC) == 0:
        print 'ERROR: saving grid'
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
        print 'Usage: grid_asc_to_saga.py <in: ascii grid file>'
        print '... trying to run with dummy data'
        fASC    = './dem.asc'
    else:
        fASC    = sys.argv[1]

    run_grid_asc2sgrd(fASC)
