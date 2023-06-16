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
//                line_flip_direction.cpp                //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "line_flip_direction.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Flip_Direction::CLine_Flip_Direction(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Flip Line Direction"));

	Set_Author		(SG_T("V. Wichmann (c) 2023"));

	Set_Description	(_TW(
		"The tool allows one to reverse the from-to direction of line "
		"features. Flipping can be useful, when the line orientation "
		"represents flow direction, for example.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"		, _TL("Lines"),
		_TL("The input line shapefile."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);
	
	Parameters.Add_Shapes("",
		"FLIPPED"	, _TL("Flipped Lines"),
		_TL("The output line shapefile with the flipped lines."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Flip_Direction::On_Execute(void)
{
    CSG_Shapes	*pShapes	= Parameters("FLIPPED")->asShapes();

	if( pShapes && pShapes != Parameters("LINES")->asShapes() )
	{
		pShapes->Create(*Parameters("LINES")->asShapes());

		DataObject_Set_Parameters(pShapes, Parameters("LINES")->asShapes());

		pShapes->Fmt_Name("%s [%s]", pShapes->Get_Name(), _TL("Flipped"));
	}
	else
	{
		pShapes	= Parameters("LINES")->asShapes();
	}


    //--------------------------------------------------------
    for(sLong iLine=0; iLine<pShapes->Get_Count() && Set_Progress(iLine, pShapes->Get_Count()); iLine++)
    {
        CSG_Shape *pLine = pShapes->Get_Shape(iLine);

        for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
        {
			pLine->Revert_Points(iPart);
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
