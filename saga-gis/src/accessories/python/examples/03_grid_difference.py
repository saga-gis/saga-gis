#! /usr/bin/env python

import saga_api, sys, os


##########################################
def grid_difference(fA, fB, fC, bPixelWise):

    # -----------------------------------
    A = saga_api.SG_Create_Grid(saga_api.CSG_String(fA))
    if A.is_Valid() == False:
        print 'Error: loading grid [' + fA + ']'
        return False

    B = saga_api.SG_Create_Grid(saga_api.CSG_String(fB))
    if B.is_Valid() == False:
        print 'Error: loading grid [' + fB + ']'
        return False

    if A.is_Compatible(B) == False:
        print 'Error: grids [' + fA + '] and [' + fB + '] are not compatible'
        return False

    # -----------------------------------
    if bPixelWise == False: # using built-in CSG_Grid function 'Subtract()'
        print 'calculating difference using CSG_Grid''s subtract function'
        C = saga_api.SG_Create_Grid(A)
        C.Subtract(B)

    # -----------------------------------
    else:                   # pixelwise calculation, slower than second solution
        print 'calculating difference pixelwise'
        C   = saga_api.SG_Create_Grid(A.Get_System())
        for y in range(0, C.Get_NY()):
            for x in range(0, C.Get_NX()):
                if A.is_NoData(x, y) or B.is_NoData(x, y):
                    C.Set_NoData(x, y)
                else:
                    C.Set_Value(x, y, A.asDouble(x, y) - B.asDouble(x, y))
            print '.',
        print '!'

    # -----------------------------------
    C.Save(fC)

    print 'Success'
    return True


##########################################
if __name__ == '__main__':

    if len( sys.argv ) == 4:
        fA  = sys.argv[1]
        fB  = sys.argv[2]
        fC  = sys.argv[3]
    else:
        fA  = './vcurv.sg-grd-z'
        fB  = './hcurv.sg-grd-z'
        fC  = './difference.sg-grd-z'
        print 'Usage: grid_difference.py <in: grid a> <in: grid b> <out: grid a-b)>'
        print '... trying to run with test_data'

    grid_difference(fA, fB, fC, 0)
    grid_difference(fA, fB, fC, 1)
