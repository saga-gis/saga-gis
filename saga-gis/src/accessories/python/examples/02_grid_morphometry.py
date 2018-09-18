#! /usr/bin/env python

import saga_api, sys, os

#########################################
def run_morphometry(fDEM):
    # -----------------------------------
    # initializations
    
    dem    = saga_api.SG_Get_Data_Manager().Add_Grid(fDEM)
    if dem == None or dem.is_Valid() == 0:
        print 'ERROR: loading grid [' + fDEM + ']'
        return 0
    print 'grid file [' + fDEM + '] has been loaded'

    System = dem.Get_System()

    path   = os.path.split(fDEM)[0]

    # -----------------------------------
    # 'Slope, Aspect, Curvature'

    Tool   = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', 0)
    p      = Tool.Get_Parameters()
    p.Get_Grid_System().Assign(System) # just in case it was used before, adjust the tool's grid system!
    p('ELEVATION').Set_Value(dem)
    p('C_CROS'   ).Set_Value(saga_api.SG_Get_Create_Pointer()) # optional grid output
    p('C_LONG'   ).Set_Value(saga_api.SG_Get_Create_Pointer()) # optional grid output

    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    p('SLOPE' ).asGrid().Save(path + '/slope'  + '.sg-grd-z')
    p('ASPECT').asGrid().Save(path + '/aspect' + '.sg-grd-z')
    p('C_CROS').asGrid().Save(path + '/hcurv'  + '.sg-grd-z')
    p('C_LONG').asGrid().Save(path + '/vcurv'  + '.sg-grd-z')
    
    # -----------------------------------
    # 'Curvature Classification'
    
    Tool    = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', 4)
    p       = Tool.Get_Parameters()
    p.Get_Grid_System().Assign(System) # just in case it was used before, adjust the tool's grid system!
    p('DEM').Set_Value(dem)
    
    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    p('CLASS').asGrid().Save(path + '/ccurv' + '.sg-grd-z')

    # -----------------------------------
    print 'success'
    return 1


#########################################
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
    if len( sys.argv ) != 2:
        print 'Usage: morphometry.py <in: elevation>'
        print '... trying to run with dummy data'
        fDEM    = './dem.sg-grd-z'
    else:
        fDEM    = sys.argv[1]

    run_morphometry(fDEM)
