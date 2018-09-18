#! /usr/bin/env python

import saga_api, sys, os

#########################################
def run_xyz2shp(fTable):
    table   = saga_api.SG_Get_Data_Manager().Add_Table(fTable)
    if table.is_Valid() == 0 or table.Get_Count() == 0:
        table.Add_Field(saga_api.CSG_String('X'), saga_api.SG_DATATYPE_Double)
        table.Add_Field(saga_api.CSG_String('Y'), saga_api.SG_DATATYPE_Double)
        table.Add_Field(saga_api.CSG_String('Z'), saga_api.SG_DATATYPE_Double)
        rec = table.Add_Record()
        rec.Set_Value(0,0)
        rec.Set_Value(1,0)
        rec.Set_Value(2,2)
        rec = table.Add_Record()
        rec.Set_Value(0,0)
        rec.Set_Value(1,1)
        rec.Set_Value(2,2)
        rec = table.Add_Record()
        rec.Set_Value(0,1)
        rec.Set_Value(1,1)
        rec.Set_Value(2,1)
        rec = table.Add_Record()
        rec.Set_Value(0,1)
        rec.Set_Value(1,0)
        rec.Set_Value(2,1)
    else:
        print 'table loaded: ' + fTable

    print 'Number of records: ' + str(table.Get_Count())

    points = saga_api.SG_Get_Data_Manager().Add_Shapes(saga_api.SHAPE_TYPE_Point)

    # -----------------------------------
    Tool   = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('shapes_points', 0) # 'Convert Table to Points'
    p      = Tool.Get_Parameters()
    p('TABLE' ).Set_Value(table)
    p('POINTS').Set_Value(points)
    p('X'     ).Set_Value(0)
    p('Y'     ).Set_Value(1)
    
    if Tool.Execute() == 0:
        print 'ERROR: executing tool [' + Tool.Get_Name().c_str() + ']'
        return 0

    print 'Number of records: ' + str(points.Get_Count())

    # ------------------------------------
    if points.Save('points.geojson') == 0:
        print 'failed to save points'
        return 0
    
    print points.Get_File_Name()

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
        print 'Usage: xyz2shp.py <in: x/y/z-data as text or dbase table>'
        print '... trying to run with test_data'
        fTable = './dem_contours.xyz'
    else:
        fTable = sys.argv[1]

    run_xyz2shp(fTable)
