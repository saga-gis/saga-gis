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

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Converts lines to polygons. "
		"Line arcs are closed to polygons simply by connecting the last point with the first. "
		"Optionally parts of polylines can be merged into one polygon optionally. "
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

	pNode	= Parameters.Add_Value(
		NULL	, "MERGE"		, _TL("Merge Line Parts to One Polygon"),
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
	bool		bSingle, bMerge;
	CSG_Shape	*pLine , *pPolygon;
	CSG_Shapes	*pLines, *pPolygons;

	pPolygons	= Parameters("POLYGONS")->asShapes();
	pLines		= Parameters("LINES"   )->asShapes();
	bSingle		= Parameters("SINGLE"  )->asBool  ();
	bMerge		= Parameters("MERGE"   )->asBool  ();

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

		if( pLine->is_Valid() )
		{
			if( !bSingle )
			{
				pPolygon	= pPolygons->Add_Shape(pLine, SHAPE_COPY_ATTR);
			}

			if( !bMerge || pLine->Get_Part_Count() == 1 )
			{
				for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
				{
					Add_Part(pPolygon, pLine, pPolygon->Get_Part_Count(), iPart);
				}
			}
			else //	if( bMerge && pLine->Get_Part_Count() > 1 )
			{
				CSG_Shapes	Copy(SHAPE_TYPE_Line);

				Add_Line(pPolygon, Copy.Add_Shape(pLine));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygons_From_Lines::Add_Part(CSG_Shape *pPolygon, CSG_Shape *pLine, int iPart_Polygon, int iPart_Line, bool bAscending)
{
	for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart_Line); iPoint++)
	{
		pPolygon->Add_Point(pLine->Get_Point(iPoint, iPart_Line, bAscending), iPart_Polygon);
	}

	return( true );
}

//---------------------------------------------------------
bool CPolygons_From_Lines::Add_Line(CSG_Shape *pPolygon, CSG_Shape *pLine, int iPart_Polygon)
{
	bool	bAscending;
	int		iPart_Line;

	Add_Part(pPolygon, pLine, iPart_Polygon, 0);	pLine->Del_Part(0);

	while( pLine->Get_Part_Count() > 0 )
	{
		if( Get_Part(pPolygon->Get_Point(0, iPart_Polygon, false), pLine, iPart_Line, bAscending) )
		{
			Add_Part(pPolygon, pLine, iPart_Polygon, iPart_Line, bAscending);	pLine->Del_Part(iPart_Line);
		}
		else // start a new polygon part
		{
			Add_Part(pPolygon, pLine, ++iPart_Polygon, 0);	pLine->Del_Part(0);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CPolygons_From_Lines::Get_Part(CSG_Point Point, CSG_Shape *pLine, int &iPart, bool &bAscending)
{
	for(iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
	{
		if( Point.is_Equal(pLine->Get_Point(0, iPart, bAscending = true)) )
		{
			return( true );
		}

		if( Point.is_Equal(pLine->Get_Point(0, iPart, bAscending = false)) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
