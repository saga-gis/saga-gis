
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include "Interpolation_Shepard.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_Shepard::CInterpolation_Shepard(void)
	: CInterpolation(true, false)
{
	Set_Name		(_TL("Modified Quadratic Shepard"));

	Set_Author		("A.Ringeler (c) 2003");

	Set_Description	(_TW(
		"Modified  Quadratic Shepard method for grid interpolation "
		"from irregular distributed points. This tool is based on "
		"Tool 660 in TOMS.\n"
		"QSHEP2D: Fortran routines implementing the Quadratic Shepard "
		"method for bivariate interpolation of scattered data "
		"(see R. J. Renka, ACM TOMS 14 (1988) pp.149-150).\n"
		"Classes: E2b. Interpolation of scattered, non-gridded multivariate data."
	));

	Parameters.Add_Int("",
		"QUADRATIC_NEIGHBORS"	, _TL("Quadratic Neighbors"),
		_TL(""),
		13, 5, true
	);

	Parameters.Add_Int("",
		"WEIGHTING_NEIGHBORS"	, _TL("Weighting Neighbors"),
		_TL(""),
		19, 3, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_Shepard::On_Initialize(void)
{
	CSG_Shapes *pPoints = Get_Points();

	if( pPoints->Get_Count() > std::numeric_limits<int>::max() )
	{
		Error_Set(_TL("too many points (exceeds size of 32bit integer)"));

		return( false );
	}

	m_Points[0].Destroy();
	m_Points[1].Destroy();
	m_Points[2].Destroy();

	for(sLong iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(iPoint);

		if( !pPoint->is_NoData(Get_Field()) )
		{
			m_Points[0].Add_Row(pPoint->Get_Point().x);
			m_Points[1].Add_Row(pPoint->Get_Point().y);
			m_Points[2].Add_Row(pPoint->asDouble(Get_Field()));
		}
	}

	Remove_Duplicate();

	Interpolator.Set_Missing(Get_Grid()->Get_NoData_Value());

	Interpolator.Interpolate(
		m_Points[0].Get_Data(),
		m_Points[1].Get_Data(),
		m_Points[2].Get_Data(),
		(int)m_Points[0].Get_Size() - 1,
		Parameters("QUADRATIC_NEIGHBORS")->asInt(),
		Parameters("WEIGHTING_NEIGHBORS")->asInt()
	);

	return( m_Points[0].Get_Size() > 1 );
}

//---------------------------------------------------------
bool CInterpolation_Shepard::On_Finalize(void)
{
	m_Points[0].Destroy();
	m_Points[1].Destroy();
	m_Points[2].Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_Shepard::Get_Value(double x, double y, double &z)
{
	Interpolator.GetValue(x, y, z);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct 
{
	double	x, y, z;
}
Data_Point;

//---------------------------------------------------------
static int Comp_Func(const void * vData1, const void * vData2)
{
	Data_Point	*val_1	= (Data_Point *)vData1;
    Data_Point	*val_2	= (Data_Point *)vData2;
	
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

//---------------------------------------------------------
#define eps 1e-7

//---------------------------------------------------------
void CInterpolation_Shepard::Remove_Duplicate()
{
	Data_Point	*Data;

	size_t	i, j, n	= m_Points[0].Get_Size() - 1;

	Data	= (Data_Point *)malloc(n * sizeof(Data_Point));

	for(i=0; i<n; i++)
	{
		Data[i].x 	= m_Points[0][i];
		Data[i].y	= m_Points[1][i];
		Data[i].z	= m_Points[2][i];
	}

	qsort((void *)Data, n, sizeof(Data_Point), Comp_Func);

	bool	dirty	= true;
	
	while( dirty && n > 1 )
	{
		dirty = false;

		for(i=0; n>1 && i<n-1; ++i)
		{
			if( fabs(Data[i].y - Data[i + 1].y) < eps
			&&	fabs(Data[i].x - Data[i + 1].x) < eps )
			{
				for(j=i; j<n-1; j++)
				{
					Data[j].x	= Data[j + 1].x;
					Data[j].y	= Data[j + 1].y;
					Data[j].z	= Data[j + 1].z;
				}
				
				n--;
				dirty	= true;
			}
		}

		qsort((void *)Data, n, sizeof(Data_Point), Comp_Func);
	}

	if( n < m_Points[0].Get_Size() )
	{
		m_Points[0].Create(n);
		m_Points[1].Create(n);
		m_Points[2].Create(n);

		for(i=0; i<n; i++)
		{
			m_Points[0][i]	= Data[i].x;
			m_Points[1][i]	= Data[i].y;
			m_Points[2][i]	= Data[i].z;
		}
	}

	free( Data );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
