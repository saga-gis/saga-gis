/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Visibility_Points.cpp                 //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Visibility_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVisibility_Points::CVisibility_Points(void)
{
	Set_Name(_TL("Visibility (points)"));

	Set_Author(SG_T("Volker Wichmann (c) 2013"));

	Set_Description(_TW(
		"This tool computes a visibility analysis using observer points from a "
		"point shapefile.\n\n"
	));

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "VISIBILITY"	, _TL("Visibility"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	CSG_Parameter	*pNode = Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL("Observer points."),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_HEIGHT", _TL("Height"),
		_TL("Height of the light source above ground."),
		false
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Unit"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Visibility"),
			_TL("Shade"),
			_TL("Distance"),
			_TL("Size")
		), 1
	);
}

//---------------------------------------------------------
CVisibility_Points::~CVisibility_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVisibility_Points::On_Execute(void)
{
	CSG_Grid		*pDTM, *pVisibility;
	CSG_Shapes		*pShapes;
	int				iMethod, iField;

	pDTM			= Parameters("ELEVATION")	->asGrid();
	pVisibility		= Parameters("VISIBILITY")	->asGrid();
	pShapes			= Parameters("POINTS")		->asShapes();
	iField			= Parameters("FIELD_HEIGHT")->asInt();
	iMethod			= Parameters("METHOD")		->asInt();

	Initialize(pVisibility, iMethod);

	for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
	{
		Process_Set_Text("%s %d...", _TL("processing observer"), iShape + 1);

		int	x	= Get_System().Get_xWorld_to_Grid(pShapes->Get_Shape(iShape)->Get_Point(0).x);
		int	y	= Get_System().Get_yWorld_to_Grid(pShapes->Get_Shape(iShape)->Get_Point(0).y);

		if( pDTM->is_InGrid(x, y, true) )
		{
			double	dHeight = pShapes->Get_Record(iShape)->asDouble(iField);
			double	z		= pDTM->asDouble(x, y) + dHeight;

			Set_Visibility(pDTM, pVisibility, x, y, z, dHeight, iMethod);
		}
	}

	//-----------------------------------------------------
	Finalize(pVisibility, iMethod);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
