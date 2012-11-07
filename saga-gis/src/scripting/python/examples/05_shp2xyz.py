import saga_api, sys, os

##########################################
def shp2xyz(fshp, fxyz):
    shp     = saga_api.SG_Create_Shapes()
    if shp.Create(saga_api.CSG_String(fshp)) == 0:
        print 'ERROR: loading shapes [' + fshpM + ']'
        return 0

    # ------------------------------------
#   saga_api.SG_Get_Module_Library_Manager().Add_Library('/usr/local/lib/saga/libio_shapes.so') # Linux
    saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/io_shapes.dll') # Windows

    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('io_shapes', 2) # 'Export Shapes to XYZ'
    m.Set_Managed(0) # tell module that we take care for data management

    p   = m.Get_Parameters()
    p(saga_api.CSG_String('SHAPES'  )).Set_Value(shp)
    p(saga_api.CSG_String('FILENAME')).Set_Value(saga_api.CSG_String(fxyz))
    
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
        fshp = './contour.shp'
        fxyz = './test.xyz'
    else:
        fshp = sys.argv[1]
        if os.path.split(fshp)[0] == '':
            fshp = './' + fshp
            fxyz = sys.argv[2]

    shp2xyz(fshp, fxyz)
