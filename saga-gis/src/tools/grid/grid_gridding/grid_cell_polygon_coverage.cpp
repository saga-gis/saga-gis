
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             grid_cell_polygon_coverage.cpp            //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "grid_cell_polygon_coverage.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cell_Polygon_Coverage::CGrid_Cell_Polygon_Coverage(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Cell Area Covered by Polygons"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"This tool calculates for each grid cell of the selected grid system "
		"the area that is covered by the input polygons layer. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL("Choose cell wise, if you have not many polygons. Polygon wise will show much better performance, if you have many (small) polygons."),
		CSG_String::Format("%s|%s|",
			_TL("cell wise"),
			_TL("polygon wise")
		), 1
	);

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("area"),
			_TL("percentage")
		), 1
	);

	Parameters.Add_Bool("",
		"SELECTION"	, _TL("Only Selected Polygons"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("AREA", _TL("Area of Coverage"), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Cell_Polygon_Coverage::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "POLYGONS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Cell_Polygon_Coverage::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "POLYGONS") )
	{
		pParameters->Set_Enabled("SELECTION", pParameter->asShapes() && pParameter->asShapes()->Get_Selection_Count() > 0);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_NPOLYGONS	(bSelection ? pPolygons->Get_Selection_Count() : pPolygons->Get_Count())
#define GET_POLYGON(i)	((CSG_Shape_Polygon *)(bSelection ? pPolygons->Get_Selection(i) : pPolygons->Get_Shape(i)))

//---------------------------------------------------------
bool CGrid_Cell_Polygon_Coverage::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	CSG_Grid	*pArea	= m_Grid_Target.Get_Grid("AREA");

	if( pPolygons->Get_Count() <= 0 || pArea == NULL || !pPolygons->Get_Extent().Intersects(pArea->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid system and polygon layer"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bSelection	= pPolygons->Get_Selection_Count() > 0 ? Parameters("SELECTION")->asBool() : false;

	pArea->Set_Name(CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Coverage")));
	pArea->Set_NoData_Value(0.0);

	DataObject_Add(pArea);
	DataObject_Set_Colors(pArea, 11, SG_COLORS_RED_GREEN, true);

	//-----------------------------------------------------
	if( Parameters("METHOD")->asInt() == 0 )
	{
		CSG_Grid_System	s(pArea->Get_System());

		for(int y=0; y<s.Get_NY() && Set_Progress(y, s.Get_NY()); y++)
		{
			double	py	= s.Get_YMin() + s.Get_Cellsize() * (y - 0.5);

			#ifndef _DEBUG
			#pragma omp parallel for
			#endif
			for(int x=0; x<s.Get_NX(); x++)
			{
				double	px	= s.Get_XMin() + s.Get_Cellsize() * (x - 0.5);

				CSG_Shapes	Cell(SHAPE_TYPE_Polygon);
				CSG_Shape_Polygon	*pCell	= (CSG_Shape_Polygon *)Cell.Add_Shape();

				pCell->Add_Point(px                   , py                   );
				pCell->Add_Point(px + s.Get_Cellsize(), py                   );
				pCell->Add_Point(px + s.Get_Cellsize(), py + s.Get_Cellsize());
				pCell->Add_Point(px                   , py + s.Get_Cellsize());

				//---------------------------------------------
				if( pPolygons->Get_Extent().Intersects(pCell->Get_Extent()) )
				{
					for(size_t i=0; pCell->Get_Area() > 0.0 && i<GET_NPOLYGONS; i++)
					{
						if( !SG_Polygon_Difference(pCell, GET_POLYGON(i)) )	// completely contained or identical > difference is empty !
						{
							pCell->Del_Parts();
						}
					}
				}

				pArea->Set_Value(x, y, s.Get_Cellarea() > pCell->Get_Area() ? s.Get_Cellarea() - pCell->Get_Area() : 0.0);
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		pArea->Assign(0.0);

		for(size_t i=0; i<GET_NPOLYGONS && Set_Progress(i, GET_NPOLYGONS); i++)
		{
			Get_Area(GET_POLYGON(i), pArea);
		}
	}

	//-----------------------------------------------------
	if( Parameters("OUTPUT")->asInt() == 1 )	// Percentage
	{
		pArea->Multiply(100.0 / pArea->Get_Cellarea());
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cell_Polygon_Coverage::Get_Area(CSG_Shape_Polygon *pPolygon, CSG_Grid *pArea)
{
	CSG_Grid_System	s(pArea->Get_System());

	int	xMin = s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin()); if( xMin <  0          ) xMin = 0;
	int	xMax = s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax()); if( xMax >= s.Get_NX() ) xMax = s.Get_NX() - 1;
	int	yMin = s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMin()); if( yMin <  0          ) yMin = 0;
	int	yMax = s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMax()); if( yMax >= s.Get_NY() ) yMax = s.Get_NY() - 1;

	CSG_Shapes Cell(SHAPE_TYPE_Polygon); CSG_Shape_Polygon *pCell = (CSG_Shape_Polygon *)Cell.Add_Shape();

	TSG_Point	p;	p.y	= s.Get_YMin() + s.Get_Cellsize() * yMin;

	for(int y=yMin; y<=yMax; y++, p.y+=s.Get_Cellsize())
	{
		p.x	= s.Get_XMin() + s.Get_Cellsize() * xMin;

		for(int x=xMin; x<=xMax; x++, p.x+=s.Get_Cellsize())
		{
			pCell->Add_Point(p.x - 0.5 * s.Get_Cellsize(), p.y - 0.5 * s.Get_Cellsize());
			pCell->Add_Point(p.x - 0.5 * s.Get_Cellsize(), p.y + 0.5 * s.Get_Cellsize());
			pCell->Add_Point(p.x + 0.5 * s.Get_Cellsize(), p.y + 0.5 * s.Get_Cellsize());
			pCell->Add_Point(p.x + 0.5 * s.Get_Cellsize(), p.y - 0.5 * s.Get_Cellsize());

			if( SG_Polygon_Intersection(pCell, pPolygon) && pCell->Get_Area() > 0.0 )
			{
				pArea->Add_Value(x, y, pCell->Get_Area());
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
