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
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TIN_To_Shapes.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "TIN_To_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_To_Shapes::CTIN_To_Shapes(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("TIN to Shapes"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description(
		_TL("Converts a TIN data set to shapes layers.\n\n")
	);


	//-----------------------------------------------------
	pNode	= Parameters.Add_TIN(
		NULL	, "TIN"			, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "CENTER"		, _TL("Center of Triangles"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "EDGES"		, _TL("Edges"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "TRIANGLES"	, _TL("Triangles"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}

//---------------------------------------------------------
CTIN_To_Shapes::~CTIN_To_Shapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_To_Shapes::On_Execute(void)
{
	int					i, j;
	CSG_TIN_Node		*pPoint;
	CSG_TIN_Edge		*pEdge;
	CSG_TIN_Triangle	*pTriangle;
	CSG_TIN				*pTIN;
	CSG_Shape			*pShape;
	CSG_Shapes			*pShapes;

	//-----------------------------------------------------
	pTIN		= Parameters("TIN")			->asTIN();

	//-----------------------------------------------------
	pShapes		= Parameters("POINTS")		->asShapes();
	pShapes->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), _TL("TIN"), pTIN->Get_Name()));

	pShapes->Add_Field("POINT_ID", SG_DATATYPE_Int);
	for(j=0; j<pTIN->Get_Field_Count(); j++)
	{
		pShapes->Add_Field(pTIN->Get_Field_Name(j), pTIN->Get_Field_Type(j));
	}

	for(i=0; i<pTIN->Get_Node_Count() && Set_Progress(i, pTIN->Get_Node_Count()); i++)
	{
		pPoint		= pTIN->Get_Node(i);
		pShape		= pShapes->Add_Shape();

		pShape->Add_Point(pPoint->Get_Point());

		pShape->Set_Value(0, 1 + i);
		for(j=0; j<pTIN->Get_Field_Count(); j++)
		{
			pShape->Set_Value(j + 1, pPoint->asString(j));
		}
	}

	//-----------------------------------------------------
	pShapes		= Parameters("EDGES")	->asShapes();
	pShapes->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s]"), _TL("TIN Edges"), pTIN->Get_Name()));

	pShapes->Add_Field("ID"			, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_A"	, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_B"	, SG_DATATYPE_Int);

	for(i=0; i<pTIN->Get_Edge_Count() && Set_Progress(i, pTIN->Get_Edge_Count()); i++)
	{
		pEdge		= pTIN->Get_Edge(i);
		pShape		= pShapes->Add_Shape();

		pShape->Add_Point(pEdge->Get_Node(0)->Get_Point());
		pShape->Add_Point(pEdge->Get_Node(1)->Get_Point());

		pShape->Set_Value(0, 1 + i);
		pShape->Set_Value(1, 1 + pEdge->Get_Node(0)->Get_ID());
		pShape->Set_Value(2, 1 + pEdge->Get_Node(1)->Get_ID());
	}

	//-----------------------------------------------------
	pShapes		= Parameters("TRIANGLES")	->asShapes();
	pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), _TL("TIN Triangles"), pTIN->Get_Name()));

	pShapes->Add_Field("ID"			, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_A"	, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_B"	, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_C"	, SG_DATATYPE_Int);

	for(i=0; i<pTIN->Get_Triangle_Count() && Set_Progress(i, pTIN->Get_Triangle_Count()); i++)
	{
		pTriangle	= pTIN->Get_Triangle(i);
		pShape		= pShapes->Add_Shape();

		pShape->Add_Point(pTriangle->Get_Node(0)->Get_Point());
		pShape->Add_Point(pTriangle->Get_Node(1)->Get_Point());
		pShape->Add_Point(pTriangle->Get_Node(2)->Get_Point());

		pShape->Set_Value(0, 1 + i);
		pShape->Set_Value(1, 1 + pTriangle->Get_Node(0)->Get_ID());
		pShape->Set_Value(2, 1 + pTriangle->Get_Node(1)->Get_ID());
		pShape->Set_Value(3, 1 + pTriangle->Get_Node(2)->Get_ID());
	}

	//-----------------------------------------------------
	pShapes		= Parameters("CENTER")		->asShapes();
	pShapes->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), _TL("TIN Centroids"), pTIN->Get_Name()));

	pShapes->Add_Field("ID"			, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_A"	, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_B"	, SG_DATATYPE_Int);
	pShapes->Add_Field("POINT_ID_C"	, SG_DATATYPE_Int);

	for(i=0; i<pTIN->Get_Triangle_Count() && Set_Progress(i, pTIN->Get_Triangle_Count()); i++)
	{
		pTriangle	= pTIN->Get_Triangle(i);
		pShape		= pShapes->Add_Shape();

		pShape->Add_Point(pTriangle->Get_CircumCircle_Point());

		pShape->Set_Value(0, 1 + i);
		pShape->Set_Value(1, 1 + pTriangle->Get_Node(0)->Get_Index());
		pShape->Set_Value(2, 1 + pTriangle->Get_Node(1)->Get_Index());
		pShape->Set_Value(3, 1 + pTriangle->Get_Node(2)->Get_Index());
	}

	//-----------------------------------------------------
	CSG_Points	Points;

	pShapes		= Parameters("POLYGONS")	->asShapes();
	pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("%s [%s]"), _TL("TIN Voronoi"), pTIN->Get_Name()));

	pShapes->Add_Field("POINT_ID", SG_DATATYPE_Int);
	for(j=0; j<pTIN->Get_Field_Count(); j++)
	{
		pShapes->Add_Field(pTIN->Get_Field_Name(j), pTIN->Get_Field_Type(j));
	}

	for(i=0; i<pTIN->Get_Node_Count() && Set_Progress(i, pTIN->Get_Node_Count()); i++)
	{
		pPoint		= pTIN->Get_Node(i);

		if( pPoint->Get_Polygon(Points) )
		{
			pShape		= pShapes->Add_Shape();

			for(j=0; j<Points.Get_Count(); j++)
			{
				pShape->Add_Point(Points[j]);
			}

			pShape->Set_Value(0, 1 + i);
			for(j=0; j<pTIN->Get_Field_Count(); j++)
			{
				pShape->Set_Value(j + 1, pPoint->asString(j));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
