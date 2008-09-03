
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Shapes_Tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Shapes_Extents.cpp                   //
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
#include "shapes_extents.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Extents::CShapes_Extents(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Get Shapes Extents"));

	Set_Author		(_TL("(c) 2008 by O. Conrad"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "EXTENTS"		, _TL("Extents"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "PARTS"		, _TL("Parts"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);
}

//---------------------------------------------------------
CShapes_Extents::~CShapes_Extents(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Extents::On_Execute(void)
{
	bool		bParts;
	TSG_Rect	r;
	CSG_Shape	*pShape , *pExtent;
	CSG_Shapes	*pShapes, *pExtents;

	pShapes		= Parameters("SHAPES")	->asShapes();
	pExtents	= Parameters("EXTENTS")	->asShapes();
	bParts		= Parameters("PARTS")	->asBool();

	//-----------------------------------------------------
	if( pShapes->Get_Type() == SHAPE_TYPE_Point )
	{
		Message_Add(_TL("no 'get extents' support for single point layers"));

		return( false );
	}

	if( !pShapes->is_Valid() )
	{
		Message_Add(_TL("invalid input"));

		return( false );
	}

	//-----------------------------------------------------
	pExtents->Create(SHAPE_TYPE_Polygon, pShapes->Get_Name(), &pShapes->Get_Table());

	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		pShape	= pShapes ->Get_Shape(iShape);

		if( bParts )
		{
			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				r		= pShape->Get_Extent(iPart);

				pExtent	= pExtents->Add_Shape(pShape->Get_Record());

				pExtent->Add_Point(r.xMin, r.yMin);
				pExtent->Add_Point(r.xMin, r.yMax);
				pExtent->Add_Point(r.xMax, r.yMax);
				pExtent->Add_Point(r.xMax, r.yMin);
			}
		}
		else
		{
			r		= pShape->Get_Extent();

			pExtent	= pExtents->Add_Shape(pShape->Get_Record());

			pExtent->Add_Point(r.xMin, r.yMin);
			pExtent->Add_Point(r.xMin, r.yMax);
			pExtent->Add_Point(r.xMax, r.yMax);
			pExtent->Add_Point(r.xMax, r.yMin);
		}
	}

	//-----------------------------------------------------
	return( pExtents->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
