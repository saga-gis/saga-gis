#! /usr/bin/env python

import saga_api, sys, os

##########################################
def xyz2shp(fTable):
    table   = saga_api.SG_Create_Table()
    if table.Create(saga_api.CSG_String(fTable)) == 0:
        table.Add_Field('X', saga_api.TABLE_FIELDTYPE_Float)
        table.Add_Field('Y', saga_api.TABLE_FIELDTYPE_Float)
        table.Add_Field('Z', saga_api.TABLE_FIELDTYPE_Float)
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

    points = saga_api.SG_Create_Shapes(saga_api.SHAPE_TYPE_Point)

    # ------------------------------------
    if os.name == 'nt':    # Windows
        saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/shapes_points.dll')
    else:                  # Linux
        saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA_MLB'] + '/libshapes_points.so')


    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('shapes_points', 0) # 'Convert Table to Points'
    m.Set_Managed(0) # tell module that we take care for data management

    p      = m.Get_Parameters()
    p(saga_api.CSG_String('TABLE' )).Set_Value(table)
    p(saga_api.CSG_String('POINTS')).Set_Value(points)
    p(saga_api.CSG_String('X'     )).Set_Value(0)
    p(saga_api.CSG_String('Y'     )).Set_Value(1)
    
    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
    points.Save(saga_api.CSG_String(fTable))

    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 2:
        print 'Usage: xyz2shp.py <in: x/y/z-data as text or dbase table>'
        print '... trying to run with test_data'
        fTable = './../test_data/test_pts_xyz.xyz'
    else:
        fTable = sys.argv[ 1 ]
        if os.path.split(fTable)[0] == '':
            fTable = './' + fTable

    xyz2shp(fTable)
