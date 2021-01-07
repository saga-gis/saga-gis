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
//                extract_closed_lines.cpp               //
//                                                       //
//                 Copyright (C) 2021 by                 //
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
#include "extract_closed_lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExtract_Closed_Lines::CExtract_Closed_Lines(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Extract Closed Lines"));

	Set_Author		(SG_T("V. Wichmann (c) 2021"));

	Set_Description	(_TW(
		"The tool allows one to extract all closed lines from the input "
		"shapefile. Closed lines are detected by examining the distance "
        "between the first and last line vertex.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES_IN"		, _TL("Lines"),
		_TL("The input line shapefile."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);
	
	Parameters.Add_Shapes("",
		"LINES_OUT"	, _TL("Closed Lines"),
		_TL("The output line shapefile with the extracted lines."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Double("",
		"TOLERANCE"	, _TL("Tolerance"),
		_TL("The maximum distance between the first and last line vertex [map units]."),
		0.001, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExtract_Closed_Lines::On_Execute(void)
{

	CSG_Shapes  *pLines		= Parameters("LINES_IN")->asShapes();
	CSG_Shapes  *pClosed	= Parameters("LINES_OUT")->asShapes();
	double      dTolerance	= Parameters("TOLERANCE")->asDouble();


    pClosed->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s_closed_lines"), pLines->Get_Name()), pLines, pLines->Get_Vertex_Type());


    //--------------------------------------------------------
    for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
    {
        CSG_Shape *pLineIn = pLines->Get_Shape(iLine);

        //--------------------------------------------------------
        for(int iPart=0; iPart<pLineIn->Get_Part_Count(); iPart++)
        {
            TSG_Point_Z   p1, p2;

            p1.x = pLineIn->Get_Point(0, iPart).x;
            p1.y = pLineIn->Get_Point(0, iPart).y;
            p2.x = pLineIn->Get_Point(pLineIn->Get_Point_Count(iPart) - 1, iPart).x;
            p2.y = pLineIn->Get_Point(pLineIn->Get_Point_Count(iPart) - 1, iPart).y;

            if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
            {
                p1.z = pLineIn->Get_Z(0, iPart);
                p2.z = pLineIn->Get_Z(pLineIn->Get_Point_Count(iPart) - 1, iPart);
            }
            else
            {
                p1.z = p2.z = 0.0;
            }

            //--------------------------------------------------------
            if( SG_Get_Distance(p1, p2) <= dTolerance )
            {
                CSG_Shape *pLineOut = pClosed->Add_Shape(pLineIn, SHAPE_COPY_ATTR);

                for(int iPoint=0; iPoint<pLineIn->Get_Point_Count(iPart); iPoint++)
                {
                    pLineOut->Add_Point(pLineIn->Get_Point(iPoint), iPart);

                    if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
                    {
                        pLineOut->Set_Z(pLineIn->Get_Z(iPoint, iPart), iPoint, iPart);

                        if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
                        {
                            pLineOut->Set_M(pLineIn->Get_M(iPoint, iPart), iPoint, iPart);
                        }
                    }
                }
            }
        } // iPart
    } // iLine


	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
