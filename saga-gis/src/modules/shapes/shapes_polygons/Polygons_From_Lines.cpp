/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Polygons_From_Lines.cpp                //
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
#include "Polygons_From_Lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygons_From_Lines::CPolygons_From_Lines(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Convert Lines to Polygons"));

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"Converts lines to polygons. Line arcs are closed to polygons simply by connecting the last point with the first."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	pNode	= Parameters.Add_Value(
		NULL	, "SINGLE"		, _TL("Create Single Multipart Polygon"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygons_From_Lines::On_Execute(void)
{
	bool		bSingle;
	CSG_Shape	*pLine , *pPolygon;
	CSG_Shapes	*pLines, *pPolygons;

	pPolygons	= Parameters("POLYGONS")->asShapes();
	pLines		= Parameters("LINES"   )->asShapes();
	bSingle		= Parameters("SINGLE"  )->asBool  ();

	if(	pLines->Get_Count() <= 0 )
	{
		return( false );
	}

	if( !bSingle )
	{
		pPolygons	->Create(SHAPE_TYPE_Polygon, pLines->Get_Name(), pLines);
	}
	else
	{
		pPolygons	->Create(SHAPE_TYPE_Polygon, pLines->Get_Name());
		pPolygons	->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		pPolygon	= pPolygons	->Add_Shape();
		pPolygon	->Set_Value(0, 1);
	}

	//-----------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		if( !bSingle )
		{
			pPolygon	= pPolygons->Add_Shape(pLine, SHAPE_COPY_ATTR);
		}

		for(int iPart=0, jPart=pPolygon->Get_Part_Count(); iPart<pLine->Get_Part_Count(); iPart++, jPart++)
		{
			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				pPolygon->Add_Point(pLine->Get_Point(iPoint, iPart), jPart);
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
