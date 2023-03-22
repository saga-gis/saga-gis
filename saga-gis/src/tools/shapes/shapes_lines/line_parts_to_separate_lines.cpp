
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
//            line_parts_to_separate_lines.cpp           //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "line_parts_to_separate_lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Parts_to_Separate_Lines::CLine_Parts_to_Separate_Lines(void)
{
	Set_Name		(_TL("Line Parts to Separate Lines"));

	Set_Author		("V. Wichmann (c) 2023");

	Set_Description	(_TW(
		"The tool allows one to split multi-part lines into separate lines. "
		"Invalid line parts with less than two points are skipped.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"	, _TL("Lines"),
		_TL("The multi-part lines."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);
	
	Parameters.Add_Shapes("",
		"PARTS"	, _TL("Line Parts"),
		_TL("The separated lines."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Parts_to_Separate_Lines::On_Execute(void)
{
	CSG_Shapes  *pLines	= Parameters("LINES")->asShapes();
	CSG_Shapes  *pParts	= Parameters("PARTS")->asShapes();

	pParts->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pLines->Get_Name(), _TL("Parts")), pLines, pLines->Get_Vertex_Type());

    //--------------------------------------------------------
    for(sLong iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
    {
        CSG_Shape *pLine = pLines->Get_Shape(iLine);

        for(int iPart=0; iPart< pLine->Get_Part_Count(); iPart++)
        {
			if (pLine->Get_Point_Count(iPart) < 2)
			{
				continue;
			}

			CSG_Shape *pShape = pParts->Add_Shape(pLine, SHAPE_COPY_ATTR);

			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				switch (pLines->Get_Vertex_Type())
				{
				default:
				case SG_VERTEX_TYPE_XY  : pShape->Add_Point(pLine->Get_Point   (iPoint, iPart)); break;
				case SG_VERTEX_TYPE_XYZ : pShape->Add_Point(pLine->Get_Point_Z (iPoint, iPart)); break;
				case SG_VERTEX_TYPE_XYZM: pShape->Add_Point(pLine->Get_Point_ZM(iPoint, iPart)); break;
				}
			}
		}
	}

	//--------------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
