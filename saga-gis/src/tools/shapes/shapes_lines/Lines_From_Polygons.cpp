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
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Lines_From_Polygons.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Olaf Conrad                      //
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
#include "Lines_From_Polygons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLines_From_Polygons::CLines_From_Polygons(void)
{

	Set_Name		(_TL("Convert Polygons to Lines"));

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"Convert polygons to lines."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLines_From_Polygons::On_Execute(void)
{
	CSG_Shapes	*pLines, *pPolygons;

	pPolygons	= Parameters("POLYGONS")	->asShapes();
	pLines		= Parameters("LINES")		->asShapes();

	//-----------------------------------------------------
	if(	pPolygons->Get_Count() <= 0 )
	{
		Error_Set(_TL("no polygons in input"));

		return( false );
	}

	//-----------------------------------------------------
	pLines->Create(SHAPE_TYPE_Line, pPolygons->Get_Name(), pPolygons, pPolygons->Get_Vertex_Type());

	for(int iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
	{
		CSG_Shape	*pPolygon	= pPolygons	->Get_Shape(iPolygon);
		CSG_Shape	*pLine		= pLines	->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				pLine->Add_Point(pPolygon->Get_Point(iPoint, iPart), iPart);

				if( pPolygons->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pLine->Set_Z(pPolygon->Get_Z(iPoint, iPart), iPoint, iPart);

					if( pPolygons->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pLine->Set_M(pPolygon->Get_M(iPoint, iPart), iPoint, iPart);
					}
				}
			}

			if( !CSG_Point(pPolygon->Get_Point(0, iPart)).is_Equal(pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart)) )
			{
				pLine->Add_Point(pPolygon->Get_Point(0, iPart), iPart);

				if( pPolygons->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pLine->Set_Z(pPolygon->Get_Z(0, iPart), pLine->Get_Point_Count(iPart) - 1, iPart);

					if( pPolygons->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pLine->Set_M(pPolygon->Get_M(0, iPart), pLine->Get_Point_Count(iPart) - 1, iPart);
					}
				}
			}
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
