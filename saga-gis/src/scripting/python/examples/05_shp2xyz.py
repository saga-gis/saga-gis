#! /usr/bin/env python

import saga_api, sys, os

##########################################
def shp2xyz(fshp, fxyz):
    shp    = saga_api.SG_Get_Data_Manager().Add_Shapes(unicode(fshp))
    if shp == None or shp.is_Valid() == 0:
        print 'ERROR: loading shapes [' + fshp + ']'
        return 0

    # ------------------------------------
    if os.name == 'nt':    # Windows
        saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/io_shapes.dll')
    else:                  # Linux
        saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA_MLB'] + '/libio_shapes.so')

    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('io_shapes', 2) # 'Export Shapes to XYZ'
    p      = m.Get_Parameters()
    p.Get(unicode('SHAPES'  )).Set_Value(shp)
    p.Get(unicode('FILENAME')).Set_Value(saga_api.CSG_String(fxyz))
    
    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 3:
        print 'Usage: shp2xyz.py <in: shape file> <out: x/y/z-data as text table>'
        print '... trying to run with test_data'
        fshp = './../test_data/test_pts.shp'
        fxyz = './../test_data/test.xyz'
    else:
        fshp = sys.argv[1]
        if os.path.split(fshp)[0] == '':
            fshp = './' + fshp
            fxyz = sys.argv[2]

    shp2xyz(fshp, fxyz)
