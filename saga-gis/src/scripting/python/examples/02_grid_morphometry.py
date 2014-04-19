#! /usr/bin/env python

import saga_api, sys, os

##########################################
def morphometry(fDEM):
    # ------------------------------------
    # initializations
    
    dem    = saga_api.SG_Get_Data_Manager().Add_Grid(unicode(fDEM))
    if dem == None or dem.is_Valid() == 0:
        print 'ERROR: loading grid [' + fDEM + ']'
        return 0
    
    slope      = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())
    aspect     = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())
    plancurv   = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())
    profcurv   = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())
    classcurv  = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())

    # ------------------------------------
    # 'Slope, Aspect, Curvature'
    
    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('ta_morphometry', 0)
    p      = m.Get_Parameters()
    p.Get_Grid_System().Assign(dem.Get_System())        # grid module needs to use conformant grid system!
    p.Get(unicode('ELEVATION')).Set_Value(dem)
    p.Get(unicode('SLOPE'    )).Set_Value(slope)
    p.Get(unicode('ASPECT'   )).Set_Value(aspect)
    p.Get(unicode('C_PLAN'   )).Set_Value(plancurv)
    p.Get(unicode('C_PROF'   )).Set_Value(profcurv)

    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    # 'Curvature Classification'
    
    m       = saga_api.SG_Get_Module_Library_Manager().Get_Module('ta_morphometry', 4)
    p       = m.Get_Parameters()
    p.Get_Grid_System().Assign(dem.Get_System())        # grid module needs to use conformant grid system!
    p.Get(unicode('DEM'      )).Set_Value(dem)
    p.Get(unicode('CLASS'    )).Set_Value(classcurv)
    
    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    # save results
    
    path   = os.path.split(fDEM)[0]
    if path == '':
        path = './'

    slope    .Save(saga_api.CSG_String(path + '/slope'))
    aspect   .Save(saga_api.CSG_String(path + '/aspect'))
    plancurv .Save(saga_api.CSG_String(path + '/plancurv'))
    profcurv .Save(saga_api.CSG_String(path + '/profcurv'))
    classcurv.Save(saga_api.CSG_String(path + '/classcurv'))
    
    # ------------------------------------
    print 'success'
    
    return 1


##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 2:
        print 'Usage: morphometry.py <in: elevation>'
        print '... trying to run with test_data'
        fDEM    = './../test_data/test.sgrd'
    else:
        fDEM    = sys.argv[1]
        if os.path.split(fDEM)[0] == '':
            fDEM    = './' + fDEM

    saga_api.SG_UI_Msg_Lock(1)
    if os.name == 'nt':    # Windows
        os.environ['PATH'] = os.environ['PATH'] + ';' + os.environ['SAGA'] + '/bin/saga_vc_Win32/dll'
        saga_api.SG_Get_Module_Library_Manager().Add_Directory(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules', 0)
    else:                  # Linux
        saga_api.SG_Get_Module_Library_Manager().Add_Directory(os.environ['SAGA_MLB'], 0)
    saga_api.SG_UI_Msg_Lock(0)

    morphometry(fDEM)
