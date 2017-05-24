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
//                    Shapes_Cut.cpp                     //
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
#include "shapes_cut.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cut_Shapes(CSG_Shapes *pPolygons, int Method, CSG_Shapes *pShapes, CSG_Shapes *pCut, double Overlap)
{
	if( !pCut || !pShapes || !pShapes->is_Valid() || !pPolygons || !pPolygons->is_Valid() || !pPolygons->Get_Extent().Intersects(pShapes->Get_Extent()) || pPolygons->Get_Type() != SHAPE_TYPE_Polygon )
	{
		return( false );
	}

	pCut->Create(pShapes->Get_Type(), CSG_String::Format("%s [%s]", pShapes->Get_Name(), _TL("Cut")), pShapes, pShapes->Get_Vertex_Type());

	//-----------------------------------------------------
	CSG_Shapes	Intersect(SHAPE_TYPE_Polygon);
	CSG_Shape_Polygon	*pIntersect = Overlap > 0.0 && pShapes->Get_Type() == SHAPE_TYPE_Polygon
		? (CSG_Shape_Polygon *)Intersect.Add_Shape() : NULL;

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		bool	bAdd	= false;

		for(int iPolygon=0; !bAdd && iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

			switch( Method )
			{
			case  2:	// center
				bAdd	= pPolygon->Contains(pShapes->Get_Type() == SHAPE_TYPE_Polygon
					? ((CSG_Shape_Polygon *)pShape)->Get_Centroid() : (TSG_Point)pShape->Get_Extent().Get_Center()
				);
				break;

			case  1:	// intersects
				bAdd	= pPolygon->Intersects(pShape) != INTERSECTION_None;

				if( bAdd && pIntersect )
				{
					bAdd	= SG_Polygon_Intersection(pPolygon, pShape, pIntersect)
						&& Overlap <= pIntersect->Get_Area() / ((CSG_Shape_Polygon *)pShape)->Get_Area();
				}
				break;

			default:	// completely contained
				bAdd	= pPolygon->Intersects(pShape) == INTERSECTION_Contains;
				break;
			}
		}

		//-------------------------------------------------
		if( bAdd )
		{
			pCut->Add_Shape(pShape);
		}
	}

	//-----------------------------------------------------
	return( pCut->Get_Count() > 0 );
}

//---------------------------------------------------------
CSG_Shapes * Cut_Shapes(CSG_Shapes *pPolygons, int Method, CSG_Shapes *pShapes, double Overlap)
{
	CSG_Shapes	*pCut	= SG_Create_Shapes();

	if( Cut_Shapes(pPolygons, Method, pShapes, pCut, Overlap) )
	{
		return( pCut );
	}

	delete(pCut);

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cut_Shapes(CSG_Rect Extent, int Method, CSG_Shapes *pShapes, CSG_Shapes *pCut)
{
	if( !pCut || !pShapes || !pShapes->is_Valid() || !Extent.Intersects(pShapes->Get_Extent()) )
	{
		return( false );
	}

	pCut->Create(pShapes->Get_Type(), CSG_String::Format("%s [%s]", pShapes->Get_Name(), _TL("Cut")), pShapes, pShapes->Get_Vertex_Type());

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		bool	bAdd;

		switch( Method )
		{
		case  2:	// center
			bAdd	= Extent.Contains(pShapes->Get_Type() == SHAPE_TYPE_Polygon
				? ((CSG_Shape_Polygon *)pShape)->Get_Centroid() : (TSG_Point)pShape->Get_Extent().Get_Center()
			);
			break;

		case  1:	// intersects
			bAdd	= pShape->Intersects(Extent) != INTERSECTION_None;
			break;

		default:	// completely contained
			bAdd	= pShape->Intersects(Extent) == INTERSECTION_Contained;
			break;
		}

		//-------------------------------------------------
		if( bAdd )
		{
			pCut->Add_Shape(pShape);
		}
	}

	//-----------------------------------------------------
	return( pCut->Get_Count() > 0 );
}

//---------------------------------------------------------
CSG_Shapes * Cut_Shapes(CSG_Rect Extent, int Method, CSG_Shapes *pShapes)
{
	CSG_Shapes	*pCut	= SG_Create_Shapes();

	if( Cut_Shapes(Extent, Method, pShapes, pCut) )
	{
		return( pCut );
	}

	delete(pCut);

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String Cut_Methods_Str(void)
{
	return( CSG_String::Format("%s|%s|%s|",
		_TL("completely contained"),
		_TL("intersects"),
		_TL("center")
	));
}

//---------------------------------------------------------
bool Cut_Set_Extent(CSG_Rect Extent, CSG_Shapes *pExtent, bool bClear)
{
	if( pExtent )
	{
		if( bClear )
		{
			pExtent->Create(SHAPE_TYPE_Polygon, _TL("Extent [Cut]"));
			pExtent->Add_Field("ID", SG_DATATYPE_Int);
		}

		if( pExtent->Get_Type() == SHAPE_TYPE_Polygon )
		{
			CSG_Shape	*pShape	= pExtent->Add_Shape();

			pShape->Set_Value("ID", pExtent->Get_Count());

			pShape->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());
			pShape->Add_Point(Extent.Get_XMin(), Extent.Get_YMax());
			pShape->Add_Point(Extent.Get_XMax(), Extent.Get_YMax());
			pShape->Add_Point(Extent.Get_XMax(), Extent.Get_YMin());
			pShape->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());

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
CShapes_Cut::CShapes_Cut(void)
{
	Set_Name		(_TL("Copy Selected Shapes"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes_List(
		NULL	, "CUT"			, _TL("Selection"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		Cut_Methods_Str(), 1
	);

	Parameters.Add_Choice(
		NULL	, "EXTENT"		, _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("user defined"),
			_TL("grid project"),
			_TL("shapes layer extent"),
			_TL("polygons")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Double(NULL, "AX", _TL("Left"            ), _TL(""), 0.0);
	Parameters.Add_Double(NULL, "BX", _TL("Right"           ), _TL(""), 1.0);
	Parameters.Add_Double(NULL, "AY", _TL("Bottom"          ), _TL(""), 0.0);
	Parameters.Add_Double(NULL, "BY", _TL("Top"             ), _TL(""), 1.0);
	Parameters.Add_Double(NULL, "DX", _TL("Horizontal Range"), _TL(""), 1.0, 0.0, true);
	Parameters.Add_Double(NULL, "DY", _TL("Vertical Range"  ), _TL(""), 1.0, 0.0, true);

	Parameters.Add_Grid_System(
		NULL	, "GRID_SYS"	, _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes(
		NULL	, "SHAPES_EXT"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double(
		NULL	, "OVERLAP"		, _TL("Minimum Overlap"),
		_TL("minimum overlapping area as percentage of the total size of the input shape. applies to polygon layers only."),
		50.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Cut::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES") )
	{
		if( pParameter->asShapesList()->Get_Item_Count() > 0 )
		{
			CSG_Rect	Extent(pParameter->asShapesList()->Get_Shapes(0)->Get_Extent());

			for(int i=1; i<pParameter->asShapesList()->Get_Item_Count(); i++)
			{
				Extent.Union(pParameter->asShapesList()->Get_Shapes(i)->Get_Extent());
			}

			pParameters->Set_Parameter("AX", Extent.Get_XMin  ());
			pParameters->Set_Parameter("AY", Extent.Get_YMin  ());
			pParameters->Set_Parameter("BX", Extent.Get_XMax  ());
			pParameters->Set_Parameter("BY", Extent.Get_YMax  ());
			pParameters->Set_Parameter("DX", Extent.Get_XRange());
			pParameters->Set_Parameter("DY", Extent.Get_YRange());
		}
	}

	else if( pParameters->Get_Parameter("EXTENT")->asInt() == 0 )
	{
		double	ax	= pParameters->Get_Parameter("AX")->asDouble();
		double	ay	= pParameters->Get_Parameter("AY")->asDouble();
		double	bx	= pParameters->Get_Parameter("BX")->asDouble();
		double	by	= pParameters->Get_Parameter("BY")->asDouble();
		double	dx	= pParameters->Get_Parameter("DX")->asDouble();
		double	dy	= pParameters->Get_Parameter("DY")->asDouble();

		if( ax > bx )	{	double	d	= ax;	ax	= bx;	bx	= d;	}
		if( ay > by )	{	double	d	= ay;	ay	= by;	by	= d;	}

		if     ( !SG_STR_CMP(pParameter->Get_Identifier(), "DX") )
		{
			bx	= ax + dx;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), "AX")
			||   !SG_STR_CMP(pParameter->Get_Identifier(), "BX") )
		{
			dx	= bx - ax;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), "DY") )
		{
			by	= ay + dy;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), "AY")
			||   !SG_STR_CMP(pParameter->Get_Identifier(), "BY") )
		{
			dy	= by - ay;
		}

		pParameters->Set_Parameter("AX", ax);
		pParameters->Set_Parameter("AY", ay);
		pParameters->Set_Parameter("BX", bx);
		pParameters->Set_Parameter("BY", by);
		pParameters->Set_Parameter("DX", dx);
		pParameters->Set_Parameter("DY", dy);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CShapes_Cut::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "EXTENT") )
	{
		pParameters->Set_Enabled("AX"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("AY"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("BX"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("BY"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("DX"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("DY"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("GRID_SYS"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("SHAPES_EXT", pParameter->asInt() == 2);
		pParameters->Set_Enabled("POLYGONS"  , pParameter->asInt() == 3);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SHAPES")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		bool	bEnable	= pParameters->Get_Parameter("METHOD")->asInt() == 1;	// intersects

		if( bEnable )
		{
			CSG_Parameter_Shapes_List	*pShapes	= pParameters->Get_Parameter("SHAPES")->asShapesList();

			bEnable	= false;

			for(int i=0; !bEnable && i<pShapes->Get_Item_Count(); i++)
			{
				bEnable	= pShapes->Get_Shapes(i)->Get_Type() == SHAPE_TYPE_Polygon;
			}
		}

		pParameters->Set_Enabled("OVERLAP"   , bEnable);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut::On_Execute(void)
{
	CSG_Parameter_Shapes_List	*pShapes	= Parameters("SHAPES")->asShapesList();

	if( pShapes->Get_Item_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Rect	Extent;
	CSG_Shapes	Polygons(SHAPE_TYPE_Polygon), *pPolygons	= NULL;

	switch( Parameters("EXTENT")->asInt() )
	{
	case 0:	// user defined
		Extent.Assign(
			Parameters("AX")->asDouble(),
			Parameters("AY")->asDouble(),
			Parameters("BX")->asDouble(),
			Parameters("BY")->asDouble()
		);
		break;

	case 1:	// grid project
		Extent.Assign(Parameters("GRID_SYS")->asGrid_System()->Get_Extent());
		break;

	case 2:	// shapes extent
		Extent.Assign(Parameters("SHAPES_EXT")->asShapes()->Get_Extent());
		break;

	case 3:	// polygons
		pPolygons	= Parameters("POLYGONS")->asShapes();
		break;
	}

	if( !pPolygons )
	{
		CSG_Shape	*pPolygon	= Polygons.Add_Shape();

		pPolygon->Add_Point(Extent.Get_XMin(), Extent.Get_YMin());
		pPolygon->Add_Point(Extent.Get_XMin(), Extent.Get_YMax());
		pPolygon->Add_Point(Extent.Get_XMax(), Extent.Get_YMax());
		pPolygon->Add_Point(Extent.Get_XMax(), Extent.Get_YMin());

		pPolygons	= &Polygons;
	}

	//-----------------------------------------------------
	CSG_Parameter_Shapes_List	*pCuts	= Parameters("CUT")->asShapesList();

	pCuts->Del_Items();

	int	Method	= Parameters("METHOD")->asInt();

	double	Overlap	= Parameters("OVERLAP")->asDouble() / 100.0;

	for(int i=0; i<pShapes->Get_Item_Count(); i++)
	{
		CSG_Shapes	*pCut	= Cut_Shapes(pPolygons, Method, pShapes->Get_Shapes(i), Overlap);

		if( pCut )
		{
			pCuts->Add_Item(pCut);
		}
	}

	//-----------------------------------------------------
	return( pCuts->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
