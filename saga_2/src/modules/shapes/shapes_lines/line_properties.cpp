
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  line_properties.cpp                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "line_properties.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Properties::CLine_Properties(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Properties"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Line properties: length, number of vertices."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "OUTPUT"		, _TL("Lines with Property Attributes added"),
		_TL("If not set property attributes will be added to the orignal lines layer."),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Line
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Properties::On_Execute(void)
{
	CSG_Shapes	*pLines	= Parameters("LINES")->asShapes();

	if(	pLines->is_Valid() && pLines->Get_Count() > 0 )
	{
		if( Parameters("OUTPUT")->asShapes() && Parameters("OUTPUT")->asShapes() != pLines )
		{
			pLines	= Parameters("OUTPUT")->asShapes();
			pLines->Create(*Parameters("LINES")->asShapes());
		}

		//-------------------------------------------------
		int		iOffset	= pLines->Get_Field_Count();

		pLines->Add_Field(SG_T("N_VERTICES"), SG_DATATYPE_Int);
		pLines->Add_Field(SG_T("LENGTH")	, SG_DATATYPE_Double);

		//-------------------------------------------------
		for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
		{
			CSG_Shape	*pLine	= pLines->Get_Shape(iLine);

			pLine->Set_Value(iOffset + 0, pLine->Get_Point_Count());
			pLine->Set_Value(iOffset + 1, ((CSG_Shape_Line *)pLine)->Get_Length());
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
