#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_contour(fGrid, fLines):
    Grid    = saga_api.SG_Get_Data_Manager().Add_Grid(unicode(fGrid))
    if Grid == None or Grid.is_Valid() == 0:
        print 'ERROR: loading grid [' + fGrid + ']'
        return 0
    
    # ------------------------------------
    if os.name == 'nt':    # Windows
        saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA_32' ] + '/modules/shapes_grid.dll')
    else:                  # Linux
        saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA_MLB'] + '/libshapes_grid.so')

    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module(saga_api.CSG_String('shapes_grid'), 5) # 'Contour Lines from Grid'
    p      = m.Get_Parameters()
    p(saga_api.CSG_String('GRID'   )).Set_Value(Grid)
    p(saga_api.CSG_String('ZSTEP'  )).Set_Value(25.0)

    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    Lines = p(saga_api.CSG_String('CONTOUR')).asShapes()
    Lines.Save(saga_api.CSG_String(fLines))
    
    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 4:
        print 'Usage: grid_contour.py <in: grid> <out: contour>'
        print '... trying to run with test_data'
        fGrid   = './test.sgrd'
        fLines  = './test_contours.shp'
    else:
        fGrid   = sys.argv[1]
        if os.path.split(fGrid)[0] == '':
            fGrid   = './' + fGrid

        fLines  = sys.argv[2]
        if os.path.split(fLines)[0] == '':
            fLines  = './' + fLines

    grid_contour(fGrid, fLines)
