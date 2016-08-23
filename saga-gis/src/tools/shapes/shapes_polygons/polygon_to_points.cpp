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
//                    Shapes_Polygon                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Polygon_To_Points.cpp                 //
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
#include "polygon_to_points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_To_Points::CPolygon_To_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Convert Polygon/Line Vertices to Points"));

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
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_To_Points::On_Execute(void)
{
	CSG_Shapes		*pShapes, *pPoints;

	pShapes		= Parameters("SHAPES")	->asShapes();
	pPoints		= Parameters("POINTS")	->asShapes();

	//-----------------------------------------------------
	if( pShapes->is_Valid() )
	{
		pPoints->Create(SHAPE_TYPE_Point, pShapes->Get_Name());
		pPoints->Add_Field(SG_T("ID")		, SG_DATATYPE_String);
		pPoints->Add_Field(SG_T("ID_SHAPE")	, SG_DATATYPE_Int);
		pPoints->Add_Field(SG_T("ID_PART")	, SG_DATATYPE_Int);
		pPoints->Add_Field(SG_T("ID_POINT")	, SG_DATATYPE_Int);

		if( pShapes->Get_Type() == SHAPE_TYPE_Polygon )
		{
			pPoints->Add_Field(SG_T("CLOCKWISE"), SG_DATATYPE_String);
			pPoints->Add_Field(SG_T("LAKE")		, SG_DATATYPE_String);
		}

		switch( pShapes->Get_Vertex_Type() )
		{
		case SG_VERTEX_TYPE_XY:
			break;

		case SG_VERTEX_TYPE_XYZ:
			pPoints->Add_Field(SG_T("Z"), SG_DATATYPE_Double);
			break;

		case SG_VERTEX_TYPE_XYZM:
			pPoints->Add_Field(SG_T("Z"), SG_DATATYPE_Double);
			pPoints->Add_Field(SG_T("M"), SG_DATATYPE_Double);
			break;
		}

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					CSG_Shape	*pPoint	= pPoints->Add_Shape();

					pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));

					int	n	= 0;

					pPoint->Set_Value(n++, CSG_String::Format(SG_T("%d/%d/%d"), iShape, iPart, iPoint));
					pPoint->Set_Value(n++, iShape);
					pPoint->Set_Value(n++, iPart);
					pPoint->Set_Value(n++, iPoint);

					if( pShapes->Get_Type() == SHAPE_TYPE_Polygon )
					{
						pPoint->Set_Value(n++, ((CSG_Shape_Polygon *)pShape)->is_Clockwise(iPart) ? SG_T("Y") : SG_T("N"));
						pPoint->Set_Value(n++, ((CSG_Shape_Polygon *)pShape)->is_Lake     (iPart) ? SG_T("Y") : SG_T("N"));
					}

					switch( pShapes->Get_Vertex_Type() )
					{
					case SG_VERTEX_TYPE_XY:
						break;

					case SG_VERTEX_TYPE_XYZ:
						pPoint->Set_Value(n++, pShape->Get_Z(iPoint, iPart));
						break;

					case SG_VERTEX_TYPE_XYZM:
						pPoint->Set_Value(n++, pShape->Get_Z(iPoint, iPart));
						pPoint->Set_Value(n++, pShape->Get_M(iPoint, iPart));
						break;
					}
				}
			}
		}

		return( pPoints->is_Valid() );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
