#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_tpi(fDEM):
    # ------------------------------------
    # initializations
    
    dem    = saga_api.SG_Get_Data_Manager().Add_Grid(fDEM)
    if dem == None or dem.is_Valid() == 0:
        print 'ERROR: loading grid [' + fDEM + ']'
        return 0
    print 'grid file [' + fDEM + '] has been loaded'
    
    path   = os.path.split(fDEM)[0]

    d      = dem.Get_Cellsize()

    # ------------------------------------
    # 'TPI Based Landform Classification'
    
    Tool   = saga_api.SG_Get_Tool_Library_Manager().Get_Tool(saga_api.CSG_String('ta_morphometry'), 19)
    p      = Tool.Get_Parameters()
    p.Get_Grid_System().Assign(dem.Get_System())   # grid tool needs to use conformant grid system!
    p('DEM'         ).Set_Value(dem)
    p('DW_WEIGHTING').Set_Value(0)
    p('RADIUS_A'    ).asRange().Set_Range(0, d *  2)
    p('RADIUS_B'    ).asRange().Set_Range(0, d * 10)

    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    p('LANDFORMS').asGrid().Save(path + '/landforms.sg-grd-z')
    print 'Saved as: ' + p('LANDFORMS').asGrid().Get_File_Name()
    
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
    if len( sys.argv ) != 2:
        print 'Usage: 07_grid_tpi.py <in: elevation>'
        print '... trying to run with dummy data'
        fDEM    = './dem.sg-grd-z'
    else:
        fDEM    = sys.argv[1]

    grid_tpi(fDEM)
