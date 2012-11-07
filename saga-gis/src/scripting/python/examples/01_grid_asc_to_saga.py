import saga_api, sys, os

##########################################
def grid_asc2sgrd(fASC):

### load all module libraries from a directory at once:
#   saga_api.SG_Get_Module_Library_Manager().Add_Directory('/usr/local/lib/saga/', 0) # Linux
#   saga_api.SG_Get_Module_Library_Manager().Add_Directory(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/', 0) # Windows
#   print 'number of loaded libraries: ' + str(saga_api.SG_Get_Module_Library_Manager().Get_Count())

### load just the needed module library:
#   saga_api.SG_Get_Module_Library_Manager().Add_Library(fmlb + '/usr/local/lib/saga/libio_grid.so') # Linux
    saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/io_grid.dll')   # Windows

    sASC = saga_api.CSG_String(fASC)
    m    = saga_api.SG_Get_Module_Library_Manager().Get_Module('io_grid', 1)
    p    = m.Get_Parameters()
    p(saga_api.CSG_String('FILE')).Set_Value(sASC)

    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    if p(saga_api.CSG_String('GRID')).asGrid().Save(sASC) == 0:
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
        fASC    = './test.asc'
        fASC    = os.path.abspath(fASC)
    else:
        fASC    = sys.argv[1]
        if os.path.split(fASC)[0] == '':
            fASC    = './' + fASC
	
    grid_asc2sgrd(fASC)
