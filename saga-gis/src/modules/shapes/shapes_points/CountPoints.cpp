/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    CountPoints.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "CountPoints.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCountPoints::CCountPoints(void)
{	
	Set_Name		(_TL("Count Points in Polygons"));

	Set_Author		(SG_T("Victor Olaya (c) 2004"));

	Set_Description	(_TW(
		"Count Points in Polygons."
	));

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""), 
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCountPoints::On_Execute(void)
{
	CSG_Shapes *pPoints		= Parameters("POINTS")  ->asShapes();
	CSG_Shapes *pPolygons	= Parameters("POLYGONS")->asShapes();

	int	Field	= pPolygons->Get_Field_Count();

	pPolygons->Add_Field(_TL("Points"), SG_DATATYPE_Int);

	for(int iPolygon=0; iPolygon<pPolygons->Get_Count() && Set_Progress(iPolygon, pPolygons->Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

		int	nPoints	= 0;

		for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
		{
			CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

			if( pPolygon->Contains(pPoint->Get_Point(iPoint)) )
			{
				nPoints++;
			}
		}

		pPolygon->Set_Value(Field, nPoints);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
