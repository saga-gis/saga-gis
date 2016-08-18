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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Convert Points to Line(s)"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Converts points to line(s)."
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

	Parameters.Add_Table_Field(
		pNode	, "SEPARATE"	, _TL("Separate by..."),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLines_From_Points::On_Execute(void)
{
	int			Order, Separate, Elevation;
	CSG_String	s;
	CSG_Shape	*pLine , *pPoint;
	CSG_Shapes	*pLines, *pPoints;

	pPoints		= Parameters("POINTS"   )->asShapes();
	pLines		= Parameters("LINES"    )->asShapes();
	Order		= Parameters("ORDER"    )->asInt();
	Separate	= Parameters("SEPARATE" )->asInt();
	Elevation	= Parameters("ELEVATION")->asInt();

	//-------------------------------------------------
	if(	pPoints->Get_Count() < 1 )
	{
		return( false );
	}

	//-------------------------------------------------
	pLines->Create(SHAPE_TYPE_Line, pPoints->Get_Name(), NULL, Elevation >= 0 ? SG_VERTEX_TYPE_XYZ : SG_VERTEX_TYPE_XY);

	pLines->Add_Field(SG_T("ID"), SG_DATATYPE_Int);

	if( Separate >= 0 )
	{
		pLines->Add_Field(pPoints->Get_Field_Name(Separate), pPoints->Get_Field_Type(Separate));

		pPoints->Set_Index(Separate, TABLE_INDEX_Ascending, Order, TABLE_INDEX_Ascending);
	}
	else
	{
		pPoints->Set_Index(Order, TABLE_INDEX_Ascending);
	}

	//-------------------------------------------------
	int	iVertex = 0;

	for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		pPoint	= pPoints->Get_Shape_byIndex(iPoint);

		if( pLines->Get_Count() == 0 || (Separate >= 0 && s.Cmp(pPoint->asString(Separate))) )
		{
			pLine	= pLines->Add_Shape();
			iVertex	= 0;

			pLine->Set_Value(0, pLines->Get_Count());

			if( Separate >= 0 )
			{
				pLine->Set_Value(1, s = pPoint->asString(Separate));
			}
		}

		pLine->Add_Point(pPoint->Get_Point(0));

		if( Elevation >= 0 )
		{
			pLine->Set_Z(pPoint->asDouble(Elevation), iVertex);
			iVertex++;
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
