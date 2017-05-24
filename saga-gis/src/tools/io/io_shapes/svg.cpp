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
	// 1. Info...

	Set_Name		(_TL("Export Scalable Vector Graphics (SVG) File"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Export shapes to Scalable Vector Graphics (SVG) File.\n"
		"SVG specification at World Wide Web Consortium (W3C)\n"
		"<a target=\"_blank\" href=\"http://www.w3.org/TR/SVG11/\">Scalable Vector Graphics (SVG) 1.1</a>"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.svg|%s|*.*"),
			_TL("Scalable Vector Graphics Files (*.svg)"),
			_TL("All Files")
		), NULL, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSVG_Export::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Shapes_List	*pList	= Parameters("SHAPES")->asShapesList();

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
	SVG.Set_Name(SG_T("svg"));
	SVG.Add_Property(SG_T("xmlns")		, SG_T("http://www.w3.org/2000/svg"));
	SVG.Add_Property(SG_T("xmlns:xlink"), SG_T("http://www.w3.org/1999/xlink"));
	SVG.Add_Property(SG_T("xmlns:ev")	, SG_T("http://www.w3.org/2001/xml-events"));
	SVG.Add_Property(SG_T("version")	, SG_T("1.1"));
	SVG.Add_Property(SG_T("baseProfile"), SG_T("tiny"));
	SVG.Add_Property(SG_T("width")		, CSG_String::Format(SG_T("%d"), Width));
	SVG.Add_Property(SG_T("height")		, CSG_String::Format(SG_T("%d"), Height));
	SVG.Add_Property(SG_T("viewBox")	, CSG_String::Format(SG_T("%f %f %f %f"), Extent.Get_XMin(), -Extent.Get_YMax(), Extent.Get_XRange(), Extent.Get_YRange()));

	Size_Point	= Extent.Get_XRange() /  200.0;
	Size_Line	= Extent.Get_XRange() /  500.0;
	m_dStroke	= Extent.Get_XRange() / 1000.0;

	//-----------------------------------------------------
	for(i=0; i<pList->Get_Item_Count() && Process_Get_Okay(); i++)
	{
		CSG_Shapes	*pShapes	= pList->Get_Shapes(i);

		pGroup	= SVG.Add_Child(SG_T("g"));
		pGroup->Add_Property(SG_T("id")			, pShapes->Get_Name());
		pGroup->Add_Property(SG_T("transform")	, SG_T("scale(1,-1)"));

		for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				switch( pShapes->Get_Type() )
				{
				case SHAPE_TYPE_Point:
				case SHAPE_TYPE_Points:		Add_Points	(*pGroup, pShape, iPart, SG_COLOR_RED		, Size_Point, SYMBOL_POINT_SQUARE);	break;
				case SHAPE_TYPE_Line:		Add_Line	(*pGroup, pShape, iPart, SG_COLOR_BLUE_DARK	, Size_Line);	break;
				case SHAPE_TYPE_Polygon:	Add_Polygon	(*pGroup, pShape, iPart, SG_COLOR_GREEN		);	break;
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
			Points	+= SG_T(" ");
		}

		Points	+= CSG_String::Format(SG_T("%f,%f"), Point.x, Point.y);
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
			pSVG->Set_Name(SG_T("circle"));
			pSVG->Add_Property(SG_T("cx")		, Point.x);
			pSVG->Add_Property(SG_T("cy")		, Point.y);
			pSVG->Add_Property(SG_T("length")	, Size);
			break;

		case SYMBOL_POINT_SQUARE:
			pSVG->Set_Name(SG_T("rect"));
			pSVG->Add_Property(SG_T("x")		, Point.x - Size / 2.0);
			pSVG->Add_Property(SG_T("y")		, Point.y - Size / 2.0);
			pSVG->Add_Property(SG_T("width")	, Size);
			pSVG->Add_Property(SG_T("height")	, Size);
			break;
		}

		pSVG->Add_Property(SG_T("fill")			, CSG_String::Format(SG_T("rgb(%d,%d,%d)"), SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)));
		pSVG->Add_Property(SG_T("stroke")		, SG_T("black"));
		pSVG->Add_Property(SG_T("stroke-width")	, m_dStroke);
	}
}

//---------------------------------------------------------
void CSVG_Export::Add_Line(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color, double Size)
{
	CSG_String	Points;

	if( Get_Points(pShape, iPart, Points) )
	{
		CSG_MetaData	*pSVG	= SVG.Add_Child(SG_T("polyline"));

		pSVG->Add_Property(SG_T("points")		, Points);
		pSVG->Add_Property(SG_T("fill")			, SG_T("none"));
		pSVG->Add_Property(SG_T("stroke")		, CSG_String::Format(SG_T("rgb(%d,%d,%d)"), SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)));
		pSVG->Add_Property(SG_T("stroke-width")	, Size);
	}
}

//---------------------------------------------------------
void CSVG_Export::Add_Polygon(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color)
{
	CSG_String	Points;

	if( Get_Points(pShape, iPart, Points) )
	{
		CSG_MetaData	*pSVG	= SVG.Add_Child(SG_T("polygon"));

		pSVG->Add_Property(SG_T("points")		, Points);
		pSVG->Add_Property(SG_T("fill")			, CSG_String::Format(SG_T("rgb(%d,%d,%d)"), SG_GET_R(Color), SG_GET_G(Color), SG_GET_B(Color)));
		pSVG->Add_Property(SG_T("stroke")		, SG_T("black"));
		pSVG->Add_Property(SG_T("stroke-width")	, m_dStroke);
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
	// 1. Info...

	Set_Name		(_TL("Import Scalable Vector Graphics (SVG) File"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.svg|%s|*.*"),
			_TL("Scalable Vector Graphics Files (*.svg)"),
			_TL("All Files")
		)
	);
}

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
