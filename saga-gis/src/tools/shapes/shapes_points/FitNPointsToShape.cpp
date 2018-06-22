/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    FitNPointsToShape.cpp
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "FitNPointsToShape.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFitNPointsToShape::CFitNPointsToShape(void)
{
	Set_Name		(_TL("Populate Polygons with Points"));

	Set_Author		("V.Olaya (c) 2004, O.Conrad (c) 2018");

	Set_Description	(_TW(
		"For each selected polygon of the input layer or for all "
		"polygons, if none is selected, a multi-point record is "
		"created with evenly distributed points trying to meet "
		"the specified number of points per polygon. "
	));

	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"), 
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Points
	);

	Parameters.Add_Table_Field("POLYGONS",
		"NUMFIELD"	, _TL("Number of Points"), 
		_TL("Desired number of points per polygon."),
		true
	);

	Parameters.Add_Int("NUMFIELD",
		"NUMPOINTS"	, _TL("Number of Points"), 
		_TL("Desired number of points per polygon."),
		100, 1, true
	);

	Parameters.Add_Int("",
		"MAXITER"	, _TL("Maximum Iterations"), 
		_TL(""),
		30, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFitNPointsToShape::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NUMFIELD") )
	{
		pParameters->Set_Enabled("NUMPOINTS", pParameter->asInt() < 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define nPolygons		(pPolygons->Get_Selection() ? (int)pPolygons->Get_Selection_Count() : pPolygons->Get_Count())
#define Get_Polygon(i)	((CSG_Shape_Polygon *)(pPolygons->Get_Selection() ? pPolygons->Get_Selection(i) : pPolygons->Get_Shape(i)))

//---------------------------------------------------------
bool CFitNPointsToShape::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();
	CSG_Shapes	*pPoints	= Parameters("POINTS"  )->asShapes();

	pPoints->Create(SHAPE_TYPE_Points, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Points")), pPolygons);

	int	nField	= Parameters("NUMFIELD" )->asInt();
	int	nPoints	= Parameters("NUMPOINTS")->asInt();
	int	maxIter	= Parameters("MAXITER"  )->asInt();

	//-----------------------------------------------------
	for(int iPolygon=0; iPolygon<nPolygons && Set_Progress(iPolygon, nPolygons); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= Get_Polygon(iPolygon);

		//-------------------------------------------------
		if( nField >= 0 )
		{
			nPoints	= pPolygon->asInt(nField);

			if( nPoints < 1 )
			{
				continue;
			}
		}

		double	dDist	= sqrt(pPolygon->Get_Area() / (nPoints));
		double	dInf	= sqrt(pPolygon->Get_Area() / (nPoints + 2));
		double	dSup	= sqrt(pPolygon->Get_Area() / (nPoints - min(2, nPoints - 1)));
		
		//-------------------------------------------------
		CSG_Shape	*pPoint	= pPoints->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

		for(int i=0; pPoint->Get_Point_Count()!=nPoints && i<maxIter; i++)
		{
			pPoint->Del_Parts();

			for(double x=pPolygon->Get_Extent().Get_XMin(); x<=pPolygon->Get_Extent().Get_XMax(); x+=dDist)
			{
				for(double y=pPolygon->Get_Extent().Get_YMin(); y<=pPolygon->Get_Extent().Get_YMax(); y+=dDist)
				{
					if( pPolygon->Contains(x, y) )
					{
						pPoint->Add_Point(x, y);
					}
				}
			}

			if( pPoint->Get_Point_Count() > nPoints )
			{
				dInf	= dDist;
				dDist	= (dInf + dSup) / 2.;
			}
			else if( pPoint->Get_Point_Count() < nPoints )
			{
				dSup	= dDist;
				dDist	= (dInf + dSup) / 2.;
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
