
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
	Set_Name		(_TL("Convert Lines to Polygons"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Converts lines to polygons. "
		"Line arcs are closed to polygons simply by connecting the last point with the first. "
		"Optionally single parts of polylines can be merged into one polygon part if these share "
		"start/end vertices. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LINES"   , _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes("",
		"POLYGONS", _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Bool("",
		"RINGS"   , _TL("Rings"),
		_TL("Only convert closed rings, i.e. first and last line vertex must be identical."),
		false
	);

	Parameters.Add_Bool("",
		"SINGLE"  , _TL("Create One Single Multipart Polygon"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"MERGE"   , _TL("Merge Connected Line Parts"),
		_TL("Treat single polyline parts as one line if connected, i.e. parts share end/start vertices."),
		false
	);

	Parameters.Add_Bool("",
		"SPLIT"   , _TL("Line Parts as Individual Polygons"),
		_TL("If checked polyline parts become individual polygons. Applies only if single multipart polygon output is unchecked."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygons_From_Lines::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("SPLIT", (*pParameters)["SINGLE"].asBool() == false);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygons_From_Lines::On_Execute(void)
{
	CSG_Shapes *pLines = Parameters("LINES")->asShapes();

	if(	pLines->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	bool  bRings = Parameters("RINGS" )->asBool();
	bool  bMerge = Parameters("MERGE" )->asBool();
	bool bSingle = Parameters("SINGLE")->asBool();
	bool  bSplit = Parameters("SPLIT" )->asBool();

	if( bSingle )
	{
		pPolygons->Create(SHAPE_TYPE_Polygon, pLines->Get_Name(),   NULL, pLines->Get_Vertex_Type());
		pPolygons->Add_Field("ID", SG_DATATYPE_Int);
	}
	else
	{
		pPolygons->Create(SHAPE_TYPE_Polygon, pLines->Get_Name(), pLines, pLines->Get_Vertex_Type());
	}

	CSG_Shapes Copy(SHAPE_TYPE_Line, NULL, pLines, pLines->Get_Vertex_Type());

	//-----------------------------------------------------
	for(sLong iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape *pLine = pLines->Get_Shape(iLine)->asLine(), *pPolygon = NULL;

		if( bMerge )
		{
			pLine = Merge_Line(Copy.Add_Shape(pLine, SHAPE_COPY_ATTR), Copy.Add_Shape(pLine, SHAPE_COPY_GEOM));
		}

		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( bRings )
			{
				if( CSG_Point(pLine->Get_Point(0, iPart, true)) != pLine->Get_Point(0, iPart, false) || pLine->Get_Point_Count(iPart) <= 3 ) // only add closed rings with more than 3 vertices
				{
					continue;
				}
			}
			else if( pLine->Get_Point_Count(iPart) < 3 )
			{
				continue;
			}

			//---------------------------------------------
			if( bSingle )
			{
				pPolygon = pPolygons->Get_Shape(0);

				if( !pPolygon )
				{
					pPolygon = pPolygons->Add_Shape(); pPolygon->Set_Value(0, 1);
				}
			}
			else if( bSplit || !pPolygon )
			{
				pPolygon = pPolygons->Add_Shape(pLine, SHAPE_COPY_ATTR);
			}

			pPolygon->Add_Part(pLine->Get_Part(iPart));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CPolygons_From_Lines::Merge_Line(CSG_Shape *pLine, CSG_Shape *pParts)
{
	CSG_Shape_Part *pPart = NULL;

	while( pParts->Get_Part_Count() > 0 )
	{
		if( pPart == NULL )
		{
			pLine->Add_Part(pParts->Get_Part(0)); pParts->Del_Part(0);

			pPart = pLine->Get_Part(pLine->Get_Part_Count() - 1);
		}

		CSG_Point End(pPart->Get_Point(0, false)); bool bFound = false;

		for(int i=0; !bFound && i<pParts->Get_Part_Count(); i++)
		{
			bool bAscending;

			if( End == pParts->Get_Point(0, i, bAscending =  true)
			||  End == pParts->Get_Point(0, i, bAscending = false) )
			{
				pPart->Add_Points(pParts->Get_Part(i), bAscending);
				pParts->Del_Part(i);
				bFound = true;
			}
		}

		if( !bFound )
		{
			pPart = NULL;
		}
	}

	return( pLine );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
