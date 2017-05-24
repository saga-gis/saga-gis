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
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Shapes_Split.cpp                    //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                      Olaf Conrad                      //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes_split.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Split::CShapes_Split(void)
{
	Set_Name		(_TL("Split Shapes Layer"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes_List(
		NULL	, "CUTS"		, _TL("Tiles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "EXTENT"		, _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "NX"			, _TL("Number of horizontal tiles"),
		_TL(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NY"			, _TL("Number of vertical tiles"),
		_TL(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		Cut_Methods_Str(), 0
	);
}

//---------------------------------------------------------
CShapes_Split::~CShapes_Split(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Split::On_Execute(void)
{
	int			x, y, nx, ny, Method;
	CSG_Shapes	*pShapes, *pCut, *pExtent;

	//-----------------------------------------------------
	pShapes	= Parameters("SHAPES")	->asShapes();
	pExtent	= Parameters("EXTENT")	->asShapes();
	nx		= Parameters("NX")		->asInt();
	ny		= Parameters("NY")		->asInt();
	Method	= Parameters("METHOD")	->asInt();

	Parameters("CUTS")->asShapesList()->Del_Items();

	//-----------------------------------------------------
	if( pShapes->is_Valid() )
	{
		double		dx, dy;
		TSG_Rect	r;

		dx	= pShapes->Get_Extent().Get_XRange() / nx;
		dy	= pShapes->Get_Extent().Get_YRange() / ny;

		for(y=0; y<ny && Process_Get_Okay(false); y++)
		{
			r.yMin	= pShapes->Get_Extent().Get_YMin() + y * dy;
			r.yMax	= r.yMin + dy;

			for(x=0; x<nx && Process_Get_Okay(false); x++)
			{
				r.xMin	= pShapes->Get_Extent().Get_XMin() + x * dx;
				r.xMax	= r.xMin + dx;

				Cut_Set_Extent(r, pExtent, y == 0 && x == 0);

				Process_Set_Text(CSG_String::Format(SG_T("%d/%d"), y * nx + (x + 1), nx * ny));

				if( (pCut = Cut_Shapes(r, Method, pShapes)) != NULL )
				{
					pCut->Set_Name(CSG_String::Format(SG_T("%s [%d][%d]"), pShapes->Get_Name(), 1 + x, 1 + y));

					Parameters("CUTS")->asShapesList()->Add_Item(pCut);
				}
			}
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
