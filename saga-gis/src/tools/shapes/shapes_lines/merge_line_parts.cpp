
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
//                  merge_line_parts.cpp                 //
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
#include "merge_line_parts.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMerge_Line_Parts::CMerge_Line_Parts(void)
{
	Set_Name		(_TL("Merge Line Parts to Lines"));

	Set_Author		("V. Wichmann (c) 2023");

	Set_Description	(_TW(
		"The tool allows one to merge the parts of multipart lines into lines. "
		"Invalid line parts with less than two points are skipped.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"PARTS"	, _TL("Multipart Lines"),
		_TL("The multipart lines."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);
	
	Parameters.Add_Shapes("",
		"LINES"	, _TL("Lines"),
		_TL("The merged lines."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double("",
		"EPSILON"	, _TL("Epsilon"),
		_TL("The tolerance used to detect connected parts [map units]."),
		0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMerge_Line_Parts::On_Execute(void)
{
	CSG_Shapes  *pParts	= Parameters("PARTS")->asShapes();
	CSG_Shapes  *pLines	= Parameters("LINES")->asShapes();
	double		Epsilon	= Parameters("EPSILON")->asDouble();

	pLines->Create(SHAPE_TYPE_Line, CSG_String::Format("%s [%s]", pParts->Get_Name(), _TL("Lines")), pParts, pParts->Get_Vertex_Type());

	//--------------------------------------------------------
	for(sLong iLine=0; iLine<pParts->Get_Count() && Set_Progress(iLine, pParts->Get_Count()); iLine++)
	{
		CSG_Shape *pLine  = pParts->Get_Shape(iLine);

		CSG_Shape *pShape = pLines->Add_Shape(pLine, SHAPE_COPY_ATTR);

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( pLine->Get_Point_Count(iPart) < 2 )
			{
				continue;
			}

			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				if( iPart > 0 && iPoint == 0 )
				{
					TSG_Point B = pLine->Get_Point(pLine->Get_Point_Count(iPart-1) - 1, iPart - 1);
					TSG_Point A = pLine->Get_Point(iPoint, iPart);

					if( SG_Get_Distance(A, B) < Epsilon )
					{
						continue;
					}
				}
				
				switch( pParts->Get_Vertex_Type() )
				{
				default:
				case SG_VERTEX_TYPE_XY  : pShape->Add_Point(pLine->Get_Point   (iPoint, iPart)); break;
				case SG_VERTEX_TYPE_XYZ : pShape->Add_Point(pLine->Get_Point_Z (iPoint, iPart)); break;
				case SG_VERTEX_TYPE_XYZM: pShape->Add_Point(pLine->Get_Point_ZM(iPoint, iPart)); break;
				}
			}
		}

		if( pShape->Get_Point_Count() < 2 )
		{
			pLines->Del_Shape(pShape);
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
