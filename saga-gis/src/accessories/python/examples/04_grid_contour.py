#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_contour(fGrid, fLines):
    Grid    = saga_api.SG_Get_Data_Manager().Add_Grid(fGrid)
    if Grid == None or Grid.is_Valid() == 0:
        print 'ERROR: loading grid [' + fGrid + ']'
        return 0

    # ------------------------------------
    zStep  = Grid.Get_Range() / 10

    Tool   = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('shapes_grid', 5) # 'Contour Lines from Grid'
    p      = Tool.Get_Parameters()
    p('GRID' ).Set_Value(Grid)
    p('ZSTEP').Set_Value(zStep)

    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    p('CONTOUR').asShapes().Save(fLines)

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
    if len( sys.argv ) != 4:
        print 'Usage: grid_contour.py <in: grid> <out: contour>'
        print '... trying to run with test_data'
        fGrid   = './dem.sg-grd-z'
        fLines  = './dem_contours.geojson'
    else:
        fGrid   = sys.argv[1]
        if os.path.split(fGrid)[0] == '':
            fGrid   = './' + fGrid

        fLines  = sys.argv[2]
        if os.path.split(fLines)[0] == '':
            fLines  = './' + fLines

    grid_contour(fGrid, fLines)
