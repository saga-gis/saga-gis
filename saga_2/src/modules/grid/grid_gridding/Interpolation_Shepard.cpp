
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Interpolation_Shepard.cpp               //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Andre Ringeler                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Interpolation_Shepard.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_Shepard::CInterpolation_Shepard(void)
{
	Set_Name(_TL("Modifed Quadratic Shepard"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description(_TL(
		"Modified  Quadratic Shepard method for grid interpolation "
		"from irregular distributed points. This module is based on "
		"Module 660 in TOMS.\n"

		"QSHEP2D: Fortran routines implementing the Quadratic Shepard "
		"method for bivariate interpolation of scattered data "
		"(see R. J. Renka, ACM TOMS 14 (1988) pp.149-150).\n"

		"Classes: E2b. Interpolation of scattered, non-gridded multivariate data.")
	);

	Parameters.Add_Value(
		NULL	, "QUADRATIC_NEIGHBORS"	, _TL("Quadratic Neighbors"),
		"",
		PARAMETER_TYPE_Int		, 13, 5, true
	);

	Parameters.Add_Value(
		NULL	, "WEIGHTING_NEIGHBORS"	, _TL("Weighting Neighbors"),
		"",
		PARAMETER_TYPE_Int		, 19, 3, true
	);


}

//---------------------------------------------------------
CInterpolation_Shepard::~CInterpolation_Shepard(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_Shepard::On_Initialize_Parameters(void)
{
	CShapes	*pPoints;

	Quadratic_Neighbors	= Parameters("QUADRATIC_NEIGHBORS")	->asInt();
	Weighting_Neighbors	= Parameters("WEIGHTING_NEIGHBORS")	->asInt();

	pShapes				= Parameters("SHAPES")				->asShapes();
	pPoints				= _Get_Point_Shapes(pShapes);
	MaxPoints			= pPoints->Get_Count();

	if( MaxPoints > 1 )
	{
		x_vals		= (double *)malloc(MaxPoints*sizeof(double));
		y_vals		= (double *)malloc(MaxPoints*sizeof(double));
		f_vals		= (double *)malloc(MaxPoints*sizeof(double));

		for(int iPoint=0; iPoint<MaxPoints; iPoint++)
		{
			x_vals[iPoint]	= pPoints->Get_Shape(iPoint)->Get_Point(0).x;
			y_vals[iPoint]	= pPoints->Get_Shape(iPoint)->Get_Point(0).y;
			f_vals[iPoint]	= pPoints->Get_Shape(iPoint)->Get_Record()->asDouble(zField);
		}

		Remove_Duplicate();

		Interpolator.Interpolate(x_vals, y_vals, f_vals, MaxPoints - 1, Quadratic_Neighbors, Weighting_Neighbors);
	}

	if( pPoints != pShapes )
	{
		delete(pPoints);
	}

	return( MaxPoints > 1 );
}

//---------------------------------------------------------
void CInterpolation_Shepard::On_Finalize_Parameters(void)
{
	free(x_vals);
	free(y_vals);
	free(f_vals);
}

//---------------------------------------------------------
CShapes * CInterpolation_Shepard::_Get_Point_Shapes(CShapes *pShapes)
{
	int		iShape, iPart, iPoint;
	CShape	*pShape, *pPoint;
	CShapes	*pPoints;

	if( pShapes->Get_Type() != SHAPE_TYPE_Point )
	{
		pPoints	= API_Create_Shapes(SHAPE_TYPE_Point, NULL, &pShapes->Get_Table());

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pShape->Get_Record());
					pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));
				}
			}
		}

		return( pPoints );
	}

	return( pShapes );
}

//---------------------------------------------------------
bool CInterpolation_Shepard::Get_Grid_Value(int x, int y)
{
	double	xPos, yPos, Result;

	xPos	= pGrid->Get_XMin() + x * pGrid->Get_Cellsize();
	yPos	= pGrid->Get_YMin() + y * pGrid->Get_Cellsize();

	if( MaxPoints > 1 )
	{
		Interpolator.GetValue(xPos, yPos, Result);

		pGrid->Set_Value(x, y, Result );

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
static int Comp_Func(const void * vData1, const void * vData2)
{
	Data_Point * val_1	=	(Data_Point *)vData1;
    Data_Point * val_2	=	(Data_Point *)vData2;
	
    if (val_1->y < val_2->y)
	{
        return (-1);
	}
    if (val_1->y > val_2->y)
	{
        return (1);
	}
    if (val_1->x < val_2->x)
	{
        return (-1);
	}
    if (val_1->x > val_2->x)
	{
        return (1);
	}
    return (0);
}

#define eps 1e-7

void CInterpolation_Shepard::Remove_Duplicate()
{
	Data_Point * Data;
	int i,j;

	Data = (Data_Point * ) malloc (MaxPoints * sizeof(Data_Point) );

	for (i = 0; i < MaxPoints; i++)
	{
		Data[i].x	=	x_vals[i];
		Data[i].y	=	y_vals[i];
		Data[i].val	=	f_vals[i];
	}

	qsort((void *)Data, MaxPoints, sizeof(Data_Point), Comp_Func);

	bool durty = true;
	
	while (durty)
	{
		durty = false;
		for (i = 0; i < MaxPoints -1; ++i)
		{
			if (fabs(Data[i].y - Data[i + 1].y) < eps)
				if (fabs(Data[i].x - Data[i + 1].x) < eps)
				{
					for (j = i; j < MaxPoints -1; j++)
					{
						Data[j].x	=	Data[j + 1].x;
						Data[j].y	=	Data[j + 1].y;
						Data[j].val	=	Data[j + 1].val;
					}
					
					MaxPoints--;
					durty	=	true;
				}
		}			
		qsort((void *)Data, MaxPoints, sizeof(Data_Point), Comp_Func);
	}

	for (i = 0; i < MaxPoints; i++)
	{
		x_vals[i]	=	Data[i].x;
		y_vals[i]	=	Data[i].y;
		f_vals[i]	=	Data[i].val;
	}

	free( Data );
}
