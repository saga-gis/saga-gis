#! /usr/bin/env python

import saga_api, saga_helper, sys, os


##########################################
def run_shp2xyz(File):
    
    # ------------------------------------
    Shapes = saga_api.SG_Get_Data_Manager().Add_Shapes(File)
    if Shapes == None or Shapes.is_Valid() == False:
        print 'Error: loading shapes [' + File + ']'
        return False

    # ------------------------------------
    # 'Export Shapes to XYZ'

    File = saga_api.CSG_String(File)
    saga_api.SG_File_Set_Extension(File, saga_api.CSG_String('xyz'))

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('io_shapes', 2)
    Parm = Tool.Get_Parameters()
    Parm('POINTS'  ).Set_Value(Shapes)
    Parm('FILENAME').Set_Value(File)

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    # ------------------------------------
    print 'Success'
    return True


##########################################
if __name__ == '__main__':

    saga_helper.Load_Tool_Libraries(True)

    # -----------------------------------
    if len( sys.argv ) == 2:
        File = sys.argv[1]
    else:
        File = './dem.geojson'
        print 'Usage: shp2xyz.py <in: shape file> <out: x/y/z-data as text table>'
        print '... trying to run with test_data'

    run_shp2xyz(File)
