#! /usr/bin/env python

import saga_api, sys, os

##########################################
def run_shp2xyz(fshp, fxyz):
    shp    = saga_api.SG_Get_Data_Manager().Add_Shapes(fshp)
    if shp == None or shp.is_Valid() == 0:
        print 'ERROR: loading shapes [' + fshp + ']'
        return 0

    # ------------------------------------
    Tool   = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_shapes', 2) # 'Export Shapes to XYZ'
    p      = Tool.Get_Parameters()
    p('POINTS'  ).Set_Value(shp)
    p('FILENAME').Set_Value(fxyz)
    
    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
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
    if len( sys.argv ) != 3:
        print 'Usage: shp2xyz.py <in: shape file> <out: x/y/z-data as text table>'
        print '... trying to run with test_data'
        fshp = './dem_contours.geojson'
        fxyz = './dem_contours.xyz'
    else:
        fshp = sys.argv[1]
        fxyz = sys.argv[2]

    run_shp2xyz(fshp, fxyz)
