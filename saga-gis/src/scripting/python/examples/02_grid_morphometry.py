import saga_api, sys, os

##########################################
def morphometry(fDEM):
    DEM    = saga_api.SG_Create_Grid()
    if DEM.Create(saga_api.CSG_String(fDEM)) == 0:
        print 'ERROR: loading grid [' + fDEM + ']'
        return 0
    
    Slope  = saga_api.SG_Create_Grid(DEM.Get_System())
    Aspect = saga_api.SG_Create_Grid(DEM.Get_System())
    hCurv  = saga_api.SG_Create_Grid(DEM.Get_System())
    vCurv  = saga_api.SG_Create_Grid(DEM.Get_System())
    Class  = saga_api.SG_Create_Grid(DEM.Get_System())

    # ------------------------------------
#   saga_api.SG_Get_Module_Library_Manager().Add_Library('/usr/local/lib/saga/libta_morphometry.so') # Linux
    saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/ta_morphometry.dll') # Windows

    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('ta_morphometry', 0) # 'Slope, Aspect, Curvature'
    m.Set_Managed(0) # tell module that we take care for data management

    p      = m.Get_Parameters()
    p.Get_Grid_System().Assign(DEM.Get_System()) # module needs to use conformant grid system!
    p(saga_api.CSG_String('ELEVATION')).Set_Value(DEM   )
    p(saga_api.CSG_String('SLOPE'    )).Set_Value(Slope )
    p(saga_api.CSG_String('ASPECT'   )).Set_Value(Aspect)
    p(saga_api.CSG_String('HCURV'    )).Set_Value(hCurv )
    p(saga_api.CSG_String('VCURV'    )).Set_Value(vCurv )
    
    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('ta_morphometry', 4) # 'Curvature Classification'
    m.Set_Managed(0) # tell module that we take care for data management

    p      = m.Get_Parameters()
    p.Get_Grid_System().Assign(DEM.Get_System()) # module needs to use conformant grid system!
    p(saga_api.CSG_String('CPLAN'    )).Set_Value(hCurv )
    p(saga_api.CSG_String('CPROF'    )).Set_Value(vCurv )
    p(saga_api.CSG_String('CLASS'    )).Set_Value(Class )
    
    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    path   = os.path.split(fDEM)[0]
    if path == '':
        path = './'
    
    Slope .Save(saga_api.CSG_String(path + '/slope'))
    Aspect.Save(saga_api.CSG_String(path + '/aspect'))
    hCurv .Save(saga_api.CSG_String(path + '/curv_plan'))
    vCurv .Save(saga_api.CSG_String(path + '/curv_profile'))
    Class .Save(saga_api.CSG_String(path + '/curv_class'))
    
    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 2:
        print 'Usage: morphometry.py <in: elevation>'
        fDEM    = './test.sgrd'
    else:
        fDEM    = sys.argv[1]
        if os.path.split(fDEM)[0] == '':
            fDEM    = './' + fDEM

    morphometry(fDEM)
