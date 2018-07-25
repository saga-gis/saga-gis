#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_tpi(fDEM):
    # ------------------------------------
    # initializations
    
    dem    = saga_api.SG_Get_Data_Manager().Add_Grid(unicode(fDEM))
    if dem == None or dem.is_Valid() == 0:
        print 'ERROR: loading grid [' + fDEM + ']'
        return 0
    print 'grid file [' + fDEM + '] has been loaded'
    
    path   = os.path.split(fDEM)[0]
#    if path == '':
#        path = './'

    landforms = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())

    # ------------------------------------
    # 'TPI Based Landform Classification'
    
    m      = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('ta_morphometry'), 19)
    p      = m.Get_Parameters()
    p.Get_Grid_System().Assign(dem.Get_System())        # grid tool needs to use conformant grid system!
    p(saga_api.CSG_String('DEM'         )).Set_Value(dem)
    p(saga_api.CSG_String('LANDFORMS'   )).Set_Value(landforms)
    p(saga_api.CSG_String('RADIUS_A'    )).asRange().Set_Range(0,  100)
    p(saga_api.CSG_String('RADIUS_B'    )).asRange().Set_Range(0, 1000)
    p(saga_api.CSG_String('DW_WEIGHTING')).Set_Value(0)

    if m.Execute() == 0:
        print 'ERROR: executing tool [' + m.Get_Name().c_str() + ']'
        return 0

    landforms.Save(saga_api.CSG_String(path + '/landforms'  + '.sg-grd-z'), 0)
    
    # ------------------------------------
    print 'success'
    
    return 1


##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 2:
        print 'Usage: 07_grid_tpi.py <in: elevation>'
        print '... trying to run with dummy data'
        fDEM    = '../test_data/test.sgrd'
    else:
        fDEM    = sys.argv[1]
        if os.path.split(fDEM)[0] == '':
            fDEM    = './' + fDEM

    saga_api.SG_UI_Msg_Lock(True)
    if os.name == 'nt':    # Windows
        os.environ['PATH'] = os.environ['PATH'] + ';' + os.environ['SAGA_32'] + '/dll'
        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_32' ] + '/tools', False)
    else:                  # Linux
        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_MLB'], False)
    saga_api.SG_UI_Msg_Lock(False)

    grid_tpi(fDEM)
