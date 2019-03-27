#! /usr/bin/env python

import saga_api, saga_helper, sys, os


#########################################
def run_morphometry(File):

    # -----------------------------------
    Grid = saga_api.SG_Get_Data_Manager().Add(File)
    if Grid == None or Grid.is_Valid() == False:
        print 'Error: loading grid [' + File + ']'
        return False

    Path = os.path.split(File)[0]

    # -----------------------------------
    # 'Slope, Aspect, Curvature'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', 0)
    Parm = Tool.Get_Parameters()
    Parm.Reset_Grid_System()
    Parm('ELEVATION').Set_Value(Grid)
    Parm('C_CROS'   ).Set_Value(saga_api.SG_Get_Create_Pointer()) # optional grid output
    Parm('C_LONG'   ).Set_Value(saga_api.SG_Get_Create_Pointer()) # optional grid output

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    Parm('SLOPE' ).asGrid().Save(Path + '/slope'  + '.sg-grd-z')
    Parm('ASPECT').asGrid().Save(Path + '/aspect' + '.sg-grd-z')
    Parm('C_CROS').asGrid().Save(Path + '/hcurv'  + '.sg-grd-z')
    Parm('C_LONG').asGrid().Save(Path + '/vcurv'  + '.sg-grd-z')

    # -----------------------------------
    # 'Curvature Classification'

    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('ta_morphometry', 4)
    Parm = Tool.Get_Parameters()
    Parm.Reset_Grid_System()
    Parm('DEM').Set_Value(Grid)

    if Tool.Execute() == False:
        print 'Error: executing tool [' + Tool.Get_Name().c_str() + ']'
        return False

    Parm('CLASS').asGrid().Save(Path + '/ccurv' + '.sg-grd-z')

    # -----------------------------------
    print 'Success'
    return True


#########################################
if __name__ == '__main__':

    saga_helper.Load_Tool_Libraries(True)

    # -----------------------------------
    if len(sys.argv) == 2:
        File = sys.argv[1]
    else:
        File = './dem.sg-grd-z'
        print 'Usage: morphometry.py <in: elevation>'
        print '... trying to run with dummy data'

    run_morphometry(File)
