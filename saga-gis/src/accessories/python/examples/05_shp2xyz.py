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
        saga_api.SG_Get_Tool_Library_Manager().Add_Library(os.environ['SAGA_32' ] + '/tools/io_shapes.dll')
    else:                  # Linux
        saga_api.SG_Get_Tool_Library_Manager().Add_Library(os.environ['SAGA_MLB'] + '/libio_shapes.so')

    m      = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('io_shapes'), 2) # 'Export Shapes to XYZ'
    p      = m.Get_Parameters()
    p(saga_api.CSG_String('POINTS'  )).Set_Value(shp)
    p(saga_api.CSG_String('FILENAME')).Set_Value(saga_api.CSG_String(fxyz))
    
    if m.Execute() == 0:
        print 'ERROR: executing tool [' + m.Get_Name().c_str() + ']'
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
        fshp = './dem_contours.shp'
        fxyz = './dem_contours.xyz'
    else:
        fshp = sys.argv[1]
        if os.path.split(fshp)[0] == '':
            fshp = './' + fshp
            fxyz = sys.argv[2]

    shp2xyz(fshp, fxyz)
