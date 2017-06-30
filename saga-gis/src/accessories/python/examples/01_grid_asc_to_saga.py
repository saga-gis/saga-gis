#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_asc2sgrd(fASC):

### load all tool libraries from a directory at once:
#    if os.name == 'nt':    # Windows
#        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_32' ], 0)
#    else:                  # Linux
#        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_MLB'], 0)
#    print '__________________'
#    print 'number of loaded libraries: ' + str(saga_api.SG_Get_Tool_Library_Manager().Get_Count())
#    print saga_api.SG_Get_Tool_Library_Manager().Get_Summary(saga_api.SG_SUMMARY_FMT_FLAT_NO_INTERACTIVE).c_str()
#    print '__________________'

    sASC = saga_api.CSG_String(fASC)
    m    = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('io_gdal'), 0)

    p    = m.Get_Parameters()
    p(saga_api.CSG_String('FILES')).Set_Value(sASC)

    if m.Execute() == 0:
        print 'ERROR: executing tool [' + m.Get_Name().c_str() + ']'
        return 0

    saga_api.SG_File_Set_Extension(sASC, saga_api.CSG_String('sgrd'))
    if p(saga_api.CSG_String('GRIDS')).asGridList().Get_Grid(0).Save(sASC) == 0:
        print 'ERROR: saving grid [' + sASC + ']'
        return 0

    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len(sys.argv) != 2:
        print 'Usage: grid_asc_to_saga.py <in: ascii grid file>'
        print '... trying to run with test_data'
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
	
    grid_asc2sgrd(fASC)
