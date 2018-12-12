/*******************************************************************************
    CreatePointGrid.cpp
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
#include "CreatePointGrid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCreatePointGrid::CCreatePointGrid(void)
{
	Set_Name		(_TL("Create Point Grid"));

	Set_Author		("V.Olaya (c) 2004");
	
	Set_Description	(_TW(
		"Creates a regular grid of points."
	));

	Parameters.Add_Shapes("", "POINTS"  , _TL("Points"  ), _TL(""), PARAMETER_OUTPUT);				
	Parameters.Add_Range ("", "X_EXTENT", _TL("X-Extent"), _TL(""), 0, 100);
	Parameters.Add_Range ("", "Y_EXTENT", _TL("Y-Extent"), _TL(""), 0, 100);
	Parameters.Add_Double("", "DIST"    , _TL("Distance"), _TL(""), 1, 0, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCreatePointGrid::On_Execute(void)
{
	TSG_Rect	r;

	r.xMin	= Parameters("X_EXTENT.MIN")->asDouble();
	r.xMax	= Parameters("X_EXTENT.MAX")->asDouble();
	r.yMin	= Parameters("Y_EXTENT.MIN")->asDouble();
	r.yMax	= Parameters("Y_EXTENT.MAX")->asDouble();

	double	Distance	= Parameters("DIST")->asDouble();

	if( Distance <= 0. || r.xMin >= r.xMax || r.yMin >= r.yMax )
	{
		return false;
	}

	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	pPoints->Create(SHAPE_TYPE_Point, _TL("Points"));

	pPoints->Add_Field("X", SG_DATATYPE_Double);
	pPoints->Add_Field("Y", SG_DATATYPE_Double);
	
	for(double y=r.yMin; y<r.yMax; y+=Distance)
	{
		for(double x=r.xMin; x<r.xMax; x+=Distance)
		{
			CSG_Shape	*pPoint	= pPoints->Add_Shape();

			pPoint->Add_Point(x, y);
			pPoint->Set_Value(0, x);
			pPoint->Set_Value(1, y);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
