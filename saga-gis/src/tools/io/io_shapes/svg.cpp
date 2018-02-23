/**********************************************************
 * Version $Id: svg.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        svg.cpp                        //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
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
#include "svg.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	SYMBOL_POINT_CIRCLE	= 0,
	SYMBOL_POINT_SQUARE
};


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSVG_Export::CSVG_Export(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Export Scalable Vector Graphics (SVG) File"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Export shapes to Scalable Vector Graphics (SVG) File."
	));

	Add_Reference(
		"http://www.w3.org/TR/SVG11/", SG_T("SVG specification at World Wide Web Consortium (W3C)")
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("single layer"),
			_TL("multiple layers")
		), 1
	);

	Parameters.Add_Shapes_List("",
		"LAYERS"	, _TL("Layers"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"LAYER"		, _TL("Layer"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("LAYER",
		"FIELD"		, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.svg)|*.svg|%s|*.*",
			_TL("Scalable Vector Graphics Files"),
			_TL("All Files")
		), NULL, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSVG_Export::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "OUTPUT") )
	{
		pParameters->Set_Enabled("LAYER" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("LAYERS", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVG_Export::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Shapes_List	*pList	= Parameters("LAYERS")->asShapesList();

	if( Parameters("OUTPUT")->asInt() == 0 )	// single layer
	{
		pList->Del_Items();
		pList->Add_Item(Parameters("LAYER")->asShapes());

		m_Field	= Parameters("FIELD")->asInt();
	}
	else	// multiple layers
	{
		m_Field	= -1;
	}

	if( pList->Get_Item_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int				i, Width, Height;
	double			Size_Point, Size_Line;
	CSG_Rect		Extent;
	CSG_MetaData	SVG, *pGroup;

	//-----------------------------------------------------
	Width	= 800;
	Height	= 800;

	Extent	= pList->Get_Shapes(0)->Get_Extent();

	for(i=1; i<pList->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		Extent.Union(pList->Get_Shapes(i)->Get_Extent());
	}

	//-----------------------------------------------------
	SVG.Set_Name("svg");
	SVG.Add_Property("xmlns"      , "http://www.w3.org/2000/svg");
	SVG.Add_Property("xmlns:xlink", "http://www.w3.org/1999/xlink");
	SVG.Add_Property("xmlns:ev"   , "http://www.w3.org/2001/xml-events");
	SVG.Add_Property("version"    , "1.1");
	SVG.Add_Property("baseProfile", "tiny");
	SVG.Add_Property("width"      , CSG_String::Format("%d", Width));
	SVG.Add_Property("height"     , CSG_String::Format("%d", Height));
	SVG.Add_Property("viewBox"    , CSG_String::Format("%f %f %f %f", Extent.Get_XMin(), -Extent.Get_YMax(), Extent.Get_XRange(), Extent.Get_YRange()));

	Size_Point	= Extent.Get_XRange() /  200.0;
	Size_Line	= Extent.Get_XRange() /  500.0;
	m_dStroke	= Extent.Get_XRange() / 1000.0;

	//-----------------------------------------------------
	for(i=0; i<pList->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		CSG_Shapes	*pShapes	= pList->Get_Shapes(i);

		pGroup	= SVG.Add_Child("g");
		pGroup->Add_Property("id"       , pShapes->Get_Name());
		pGroup->Add_Property("transform", "scale(1,-1)");

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				switch( pShapes->Get_Type() )
				{
				case SHAPE_TYPE_Point  :
				case SHAPE_TYPE_Points : Add_Points (*pGroup, pShape, iPart, SG_COLOR_RED, Size_Point, SYMBOL_POINT_SQUARE);	break;
				case SHAPE_TYPE_Line   : Add_Line   (*pGroup, pShape, iPart, SG_COLOR_BLUE_DARK	, Size_Line);	break;
				case SHAPE_TYPE_Polygon: Add_Polygon(*pGroup, pShape, iPart, SG_COLOR_GREEN);	break;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( SVG.Save(Parameters("FILE")->asString()) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVG_Export::Get_Points(CSG_Shape *pShape, int iPart, CSG_String &Points)
{
	Points.Clear();

	for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
	{
		TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

		if( iPoint > 0 )
		{
			Points	+= " ";
		}

		Points	+= CSG_String::Format("%f,%f", Point.x, Point.y);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSVG_Export::Add_Points(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color, double Size, int Symbol)
{
	for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
	{
		TSG_Point		Point	= pShape->Get_Point(0, 0);

		CSG_MetaData	*pSVG	= SVG.Add_Child();

		switch( Symbol )
		{
		case SYMBOL_POINT_CIRCLE: default:
			pSVG->Set_Name("circle");
			pSVG->Add_Property("cx"    , Point.x);
			pSVG->Add_Property("cy"    , Point.y);
			pSVG->Add_Property("length", Size);
			break;

		case SYMBOL_POINT_SQUARE:
			pSVG->Set_Name("rect");
			pSVG->Add_Property("x"     , Point.x - Size / 2.0);
			pSVG->Add_Property("y"     , Point.y - Size / 2.0);
			pSVG->Add_Property("width" , Size);
			pSVG->Add_Property("height", Size);
			break;
		}

		pSVG->Add_Property("fill"        , CSG_String::Format("rgb(%d,%d,%d)", SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)));
		pSVG->Add_Property("stroke"      , "black");
		pSVG->Add_Property("stroke-width", m_dStroke);

		if( m_Field >= 0 )
		{
			pSVG->Add_Property("attribute", pShape->asString(m_Field));
		}
	}
}

//---------------------------------------------------------
void CSVG_Export::Add_Line(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color, double Size)
{
	CSG_String	Points;

	if( Get_Points(pShape, iPart, Points) )
	{
		CSG_MetaData	*pSVG	= SVG.Add_Child("polyline");

		pSVG->Add_Property("points"      , Points);
		pSVG->Add_Property("fill"        , "none");
		pSVG->Add_Property("stroke"      , CSG_String::Format("rgb(%d,%d,%d)", SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)));
		pSVG->Add_Property("stroke-width", Size);

		if( m_Field >= 0 )
		{
			pSVG->Add_Property("attribute", pShape->asString(m_Field));
		}
	}
}

//---------------------------------------------------------
void CSVG_Export::Add_Polygon(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color)
{
	CSG_String	Points;

	if( Get_Points(pShape, iPart, Points) )
	{
		CSG_MetaData	*pSVG	= SVG.Add_Child("polygon");

		pSVG->Add_Property("points"      , Points);
		pSVG->Add_Property("fill"        , CSG_String::Format("rgb(%d,%d,%d)", SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)));
		pSVG->Add_Property("stroke"      , "black");
		pSVG->Add_Property("stroke-width", m_dStroke);

		if( m_Field >= 0 )
		{
			pSVG->Add_Property("attribute", pShape->asString(m_Field));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSVG_Import::CSVG_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Scalable Vector Graphics (SVG) File"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Import shapes from Scalable Vector Graphics (SVG) File."
	));

	Add_Reference(
		"http://www.w3.org/TR/SVG11/", SG_T("SVG specification at World Wide Web Consortium (W3C)")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		""	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		""	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.svg)|*.svg|%s|*.*",
			_TL("Scalable Vector Graphics Files"),
			_TL("All Files")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVG_Import::On_Execute(void)
{
	CSG_MetaData	SVG;

	//-----------------------------------------------------
	CSG_Parameter_Shapes_List	*pList	= Parameters("SHAPES")->asShapesList();

	//-----------------------------------------------------
	if( SVG.Load(Parameters("FILE")->asString()) )
	{
	}

	//-----------------------------------------------------
	return( pList->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
