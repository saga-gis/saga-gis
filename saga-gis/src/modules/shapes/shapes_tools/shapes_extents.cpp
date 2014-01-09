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

	Set_Author		(SG_T("O. Conrad (c) 2008"));

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

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Get Extent for ..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("all shapes"),
			_TL("each shape"),
			_TL("each shape's part")
		), 1
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
	CSG_Shapes	*pShapes	= Parameters("SHAPES" )->asShapes();
	CSG_Shapes	*pExtents	= Parameters("EXTENTS")->asShapes();

	//-----------------------------------------------------
	if( !pShapes->is_Valid() )
	{
		Message_Add(_TL("invalid input"));

		return( false );
	}

	//-----------------------------------------------------
	int	iOutput	= Parameters("OUTPUT")->asInt();

	if( iOutput == 0 )	// all shapes
	{
		pExtents->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Extent")));
		pExtents->Add_Field(SG_T("OID"), SG_DATATYPE_Int);

		TSG_Rect	r			= pShapes->Get_Extent();
		CSG_Shape	*pExtent	= pExtents->Add_Shape();

		pExtent->Set_Value(0, 1);

		pExtent->Add_Point(r.xMin, r.yMin);
		pExtent->Add_Point(r.xMin, r.yMax);
		pExtent->Add_Point(r.xMax, r.yMax);
		pExtent->Add_Point(r.xMax, r.yMin);

		return( true );
	}

	//-----------------------------------------------------
	if( pShapes->Get_Type() == SHAPE_TYPE_Point )
	{
		Message_Add(_TL("no 'get extents' support for single point layers"));

		return( false );
	}

	//-----------------------------------------------------
	pExtents->Create(SHAPE_TYPE_Polygon, pShapes->Get_Name(), pShapes);

	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( iOutput == 1 )	// each shape
		{
			TSG_Rect	r			= pShape->Get_Extent();
			CSG_Shape	*pExtent	= pExtents->Add_Shape(pShape, SHAPE_COPY_ATTR);

			pExtent->Add_Point(r.xMin, r.yMin);
			pExtent->Add_Point(r.xMin, r.yMax);
			pExtent->Add_Point(r.xMax, r.yMax);
			pExtent->Add_Point(r.xMax, r.yMin);
		}
		else // if( iOutput == 2 )	// each shape's part
		{
			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				TSG_Rect	r			= pShape->Get_Extent(iPart);
				CSG_Shape	*pExtent	= pExtents->Add_Shape(pShape, SHAPE_COPY_ATTR);

				pExtent->Add_Point(r.xMin, r.yMin);
				pExtent->Add_Point(r.xMin, r.yMax);
				pExtent->Add_Point(r.xMax, r.yMax);
				pExtent->Add_Point(r.xMax, r.yMin);
			}
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
