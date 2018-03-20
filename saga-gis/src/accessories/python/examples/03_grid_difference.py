#! /usr/bin/env python

import saga_api, sys, os

##########################################
def grid_difference(fA, fB, fC, bPixelWise):
    A   = saga_api.SG_Create_Grid()
    if A.Create(saga_api.CSG_String(fA)) == 0:
        print 'ERROR: loading grid [' + fA + ']'
        return 0

    B   = saga_api.SG_Create_Grid()
    if B.Create(saga_api.CSG_String(fB)) == 0:
        print 'ERROR: loading grid [' + fB + ']'
        return 0

    if A.is_Compatible(B) == 0:
        print 'ERROR: grids [' + fA + '] and [' + fB + '] are not compatible'
        return 0

    if bPixelWise == 1: # pixelwise calculation, slower than second solution
        C   = saga_api.SG_Create_Grid(A.Get_System())
        for y in range(0, C.Get_NY()):
            for x in range(0, C.Get_NX()):
                if A.is_NoData(x, y) or B.is_NoData(x, y):
                    C.Set_NoData(x, y)
                else:
                    C.Set_Value(x, y, A.asDouble(x, y) - B.asDouble(x, y))
            print '.',
        C.Save(saga_api.CSG_String(fC), 0)
        print
        
    else:               # using built-in CSG_Grid function 'Subtract()'
        C   = saga_api.SG_Create_Grid(A)
        C.Subtract(B)
        C.Save(saga_api.CSG_String(fC), 0)
        
    print 'success'
    return 1

##########################################
if __name__ == '__main__':
    print 'Python - Version ' + sys.version
    print saga_api.SAGA_API_Get_Version()
    print

    if len( sys.argv ) != 4:
        print 'Usage: grid_difference.py <in: grid a> <in: grid b> <out: grid a-b)>'
        print '... trying to run with test_data'
        fA  = './vcurv.sg-grd-z'
        fB  = './hcurv.sg-grd-z'
        fC  = './difference.sg-grd-z'
    else:
        fA  = sys.argv[1]
        if os.path.split(fA)[0] == '':
            fA  = './' + fA

        fB  = sys.argv[2]
        if os.path.split(fB)[0] == '':
            fB  = './' + fB

        fC  = sys.argv[3]
        if os.path.split(fC)[0] == '':
            fC  = './' + fC

    grid_difference(fA, fB, fC, 0)
