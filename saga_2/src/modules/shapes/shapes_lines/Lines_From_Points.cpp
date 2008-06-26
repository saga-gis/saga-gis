
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
//                 Lines_From_Points.cpp                 //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "Lines_From_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLines_From_Points::CLines_From_Points(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Convert Points to Line"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Converts points to a line."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ORDER"		, _TL("Order by..."),
		_TL(""),
		true
	);
}

//---------------------------------------------------------
CLines_From_Points::~CLines_From_Points(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLines_From_Points::On_Execute(void)
{
	CSG_Shape	*pLine , *pPoint;
	CSG_Shapes	*pLines, *pPoints;

	pPoints	= Parameters("POINTS")	->asShapes();
	pLines	= Parameters("LINES")	->asShapes();

	if(	pPoints->Get_Count() > 0 )
	{
		pPoints->Set_Index(Parameters("ORDER")->asInt(), TABLE_INDEX_Ascending);

		pLines->Create(SHAPE_TYPE_Line, pPoints->Get_Name(), &pPoints->Get_Table());
		pLines->Get_Table().Add_Field(SG_T("ID"), TABLE_FIELDTYPE_Int);
		pLine	= pLines->Add_Shape();

		//-------------------------------------------------
		for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
		{
			pPoint	= pPoints->Get_Shape_byIndex(iPoint);

			pLine->Add_Point(pPoint->Get_Point(0));
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
