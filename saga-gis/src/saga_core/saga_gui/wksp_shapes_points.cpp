
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                WKSP_Shapes_Points.cpp                 //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_gdi/sgdi_helper.h>

#include "wksp_shapes_points.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes_Points::CWKSP_Shapes_Points(CSG_Shapes *pShapes)
	: CWKSP_Shapes_Point(pShapes)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes_Points::Draw_Shape(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int Selection)
{
	if( (m_Size.Field >= 0 && pShape->is_NoData(m_Size.Field)) || (m_Stretch.Value >= 0 && pShape->is_NoData(m_Stretch.Value)) )
	{
		return;
	}

	//-----------------------------------------------------
	int Size;

	if( CWKSP_Shapes_Point::Draw_Initialize(dc_Map, Size, pShape, Selection) )
	{
		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				TSG_Point_Int p = dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

				Draw_Symbol(dc_Map, p.x, p.y, Size);
			}
		}

		//-------------------------------------------------
		if( Selection )
		{
			CWKSP_Shapes_Point::Draw_Initialize(dc_Map, 0);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes_Points::Draw_Label(CSG_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label)
{
	TSG_Point_Int p(dc_Map.World2DC(pShape->Get_Extent().Get_Center()));

	dc_Map.DrawText(TEXTALIGN_CENTER, p.x, p.y, Label, m_Label.Effect, m_Label.Effect_Color, m_Label.Effect_Size);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
