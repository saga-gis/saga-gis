import saga_api, sys, os

##########################################
def grid_contour(fGrid, fLines):
    Grid    = saga_api.SG_Create_Grid()
    if Grid.Create(saga_api.CSG_String(fGrid)) == 0:
        print 'ERROR: loading grid [' + fGrid + ']'
        return 0

    Lines   = saga_api.SG_Create_Shapes()

    # ------------------------------------
#   saga_api.SG_Get_Module_Library_Manager().Add_Library('/usr/local/lib/saga/libshapes_grid.so') # Linux
    saga_api.SG_Get_Module_Library_Manager().Add_Library(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules/shapes_grid.dll') # Windows

    m      = saga_api.SG_Get_Module_Library_Manager().Get_Module('shapes_grid', 5) # 'Contour Lines from Grid'
    m.Set_Managed(0) # tell module that we take care for data management

    p      = m.Get_Parameters()
    p.Get_Grid_System().Assign(Grid.Get_System()) # module needs to use conformant grid system!
    p(saga_api.CSG_String('INPUT'  )).Set_Value(Grid)
    p(saga_api.CSG_String('CONTOUR')).Set_Value(Lines)
    p(saga_api.CSG_String('ZSTEP'  )).Set_Value(25.0)

    if m.Execute() == 0:
        print 'ERROR: executing module [' + m.Get_Name().c_str() + ']'
        return 0

    # ------------------------------------
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
        fGrid   = './test.sgrd'
        fLines  = './contour'
    else:
        fGrid   = sys.argv[1]
        if os.path.split(fGrid)[0] == '':
            fGrid   = './' + fGrid

        fLines  = sys.argv[2]
        if os.path.split(fLines)[0] == '':
            fLines  = './' + fLines

    grid_contour(fGrid, fLines)
