#! /usr/bin/env python

import sys, os, saga_helper, saga_api


#########################################
def run_morphometry(File):
    Grid = saga_api.SG_Get_Data_Manager().Add(File)
    if Grid == None or Grid.is_Valid() == False:
        print('Error: loading grid [' + File + ']')
        return False

    Path = os.path.split(File)[0]

    # -----------------------------------
    # 'Slope, Aspect, Curvature'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', 0)
    Tool.Get_Parameters().Reset_Grid_System()
    Tool.Set_Parameter('ELEVATION', Grid)
    Tool.Set_Parameter('C_CROS'   , saga_api.SG_Get_Create_Pointer()) # optional grid output
    Tool.Set_Parameter('C_LONG'   , saga_api.SG_Get_Create_Pointer()) # optional grid output

    if Tool.Execute() == False:
        print('Error: executing tool [' + Tool.Get_Name().c_str() + ']')
        return False

    Tool.Get_Parameter('SLOPE' ).asGrid().Save(Path + '/slope'  + '.sg-grd-z')
    Tool.Get_Parameter('ASPECT').asGrid().Save(Path + '/aspect' + '.sg-grd-z')
    Tool.Get_Parameter('C_CROS').asGrid().Save(Path + '/hcurv'  + '.sg-grd-z')
    Tool.Get_Parameter('C_LONG').asGrid().Save(Path + '/vcurv'  + '.sg-grd-z')

    # -----------------------------------
    # 'Curvature Classification'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', 4)
    Tool.Get_Parameters().Reset_Grid_System()
    Tool.Set_Parameter('DEM', Grid)

    if Tool.Execute() == False:
        print('Error: executing tool [' + Tool.Get_Name().c_str() + ']')
        return False

    Tool.Get_Parameter('CLASSES').asGrid().Save(Path + '/ccurv' + '.sg-grd-z')

    # -----------------------------------
    print('Success')
    return True


#########################################
if __name__ == '__main__':

    saga_helper.Initialize(True)

    # -----------------------------------
    if len(sys.argv) == 2:
        File = sys.argv[1]
    else:
        File = './dem.sg-grd-z'
        print('Usage: morphometry.py <in: elevation>')
        print('... trying to run with dummy data')

    run_morphometry(File)
