#! /usr/bin/env python

import saga_api, saga_helper, sys, os


#########################################
def run_xyz2shp(File):

    # -----------------------------------
    Table = saga_api.SG_Get_Data_Manager().Add_Table(File)

    if Table != None and Table.is_Valid() == True and Table.Get_Count() > 0:
        print 'table loaded: ' + File

    else:
    #    Table = saga_api.CSG_Table()
        Table = saga_api.SG_Get_Data_Manager().Add_Table()

        Table.Add_Field(saga_api.CSG_String('X'), saga_api.SG_DATATYPE_Double)
        Table.Add_Field(saga_api.CSG_String('Y'), saga_api.SG_DATATYPE_Double)
        Table.Add_Field(saga_api.CSG_String('Z'), saga_api.SG_DATATYPE_Double)

        Random = saga_api.CSG_Random()

        for i in range(0, 100):
            Record = Table.Add_Record()
            Record.Set_Value(0, Random.Get_Gaussian(0, 100))
            Record.Set_Value(1, Random.Get_Gaussian(0, 100))
            Record.Set_Value(2, Random.Get_Gaussian(0, 100))

    print 'Number of records: ' + str(Table.Get_Count())

    # -----------------------------------
    # 'Convert Table to Points'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('shapes_points', 0)
    Parm = Tool.Get_Parameters()
    Parm('TABLE').Set_Value(Table)
    Parm('X'    ).Set_Value(0)
    Parm('Y'    ).Set_Value(1)
    
    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    if Parm('POINTS').asShapes().Save(os.path.split(File)[0] + '/points.geojson') == False:
        print 'Error: saving points'
        return False

    # ------------------------------------
    print 'Success'
    return True


#########################################
if __name__ == '__main__':

    saga_helper.Load_Tool_Libraries(True)

    # -----------------------------------
    if len(sys.argv) == 2:
        File = sys.argv[1]
    else:
        File = './dem.xyz'
        print 'Usage: xyz2shp.py <in: x/y/z-data as text or dbase table>'
        print '... trying to run with test_data'

    run_xyz2shp(File)
