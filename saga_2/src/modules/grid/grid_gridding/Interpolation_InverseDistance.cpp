
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
//           Interpolation_InverseDistance.cpp           //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
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
#include "Interpolation_InverseDistance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_InverseDistance::CInterpolation_InverseDistance(void)
{
	Set_Name(_TL("Inverse Distance"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Inverse Distance method for grid interpolation from irregular distributed points.")
	);

	Parameters.Add_Value(
		NULL	, "INVDIST_POWER"	, _TL("Inverse Distance: Power"),
		"",
		PARAMETER_TYPE_Double		, 1.0
	);

	Parameters.Add_Value(
		NULL	, "INVDIST_RADIUS"	, _TL("Search Radius"),
		"",
		PARAMETER_TYPE_Double		, 100.0
	);

	Parameters.Add_Value(
		NULL	, "INVDIST_POINTS"	, _TL("Maximum Points"),
		"",
		PARAMETER_TYPE_Int			, 10.0
	);
}

//---------------------------------------------------------
CInterpolation_InverseDistance::~CInterpolation_InverseDistance(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_InverseDistance::On_Initialize_Parameters(void)
{
	InvDist_Power		= Parameters("INVDIST_POWER")	->asDouble();
	InvDist_Radius		= Parameters("INVDIST_RADIUS")	->asDouble();
	InvDist_MaxPoints	= Parameters("INVDIST_POINTS")	->asInt();

	return( true );
}

//---------------------------------------------------------
bool CInterpolation_InverseDistance::Get_Grid_Value(int x, int y)
{
	int			iPoint, nPoints;
	double		zSum, dSum, d, dx, xPos, yPos;
	TSG_Point	Point;
	CShape		*pPoint;

	xPos	= pGrid->Get_XMin() + x * pGrid->Get_Cellsize();
	yPos	= pGrid->Get_YMin() + y * pGrid->Get_Cellsize();

	if( (nPoints = SearchEngine.Select_Radius(xPos, yPos, InvDist_Radius, true, InvDist_MaxPoints)) > 0 )
	{
		for(iPoint=0, zSum=0.0, dSum=0.0; iPoint<nPoints; iPoint++)
		{
			if( (pPoint = SearchEngine.Get_Selected_Point(iPoint)) != NULL )
			{
				Point	= pPoint->Get_Point(0);

				dx		= Point.x - xPos;
				d		= Point.y - yPos;
				d		= sqrt(dx*dx + d*d);

				if( d <= 0.0 )
				{
					pGrid->Set_Value(x, y, pPoint->Get_Record()->asDouble(zField) );

					return( true );
				}

				d		= pow(d, -InvDist_Power);

				zSum	+= d * pPoint->Get_Record()->asDouble(zField);
				dSum	+= d;
			}
		}

		if( dSum > 0.0 )
		{
			pGrid->Set_Value(x, y, zSum / dSum );

			return( true );
		}
	}

	pGrid->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
