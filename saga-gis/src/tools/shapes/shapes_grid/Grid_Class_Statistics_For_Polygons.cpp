
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//          Grid_Class_Statistics_For_Polygons.cpp       //
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
#include "Grid_Class_Statistics_For_Polygons.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Class_Statistics_For_Polygons::CGrid_Class_Statistics_For_Polygons(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Grid Classes Area for Polygons"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Calculates for each polygon the area covered by each grid class."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(NULL,
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(NULL,
		"RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Grid(NULL,
		"GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(NULL,
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("cell center"),
			_TL("cell area")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Class_Statistics_For_Polygons::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Class_Statistics_For_Polygons::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( !Get_Classes(pGrid) )
	{
		Error_Set(_TL("undefined grid classes"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	if( pPolygons->Get_Count() <= 0 || !pPolygons->Get_Extent().Intersects(pGrid->Get_Extent()) )
	{
		Error_Set(_TL("no spatial intersection between grid and polygon layer"));

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asShapes() != NULL && Parameters("RESULT")->asShapes() != pPolygons )
	{
		Process_Set_Text(_TL("copying polygons"));

		CSG_Shapes	*pResult	= Parameters("RESULT")->asShapes();

		pResult->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s [%s]", pPolygons->Get_Name(), _TL("Grid Classes")));

		for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
		{
			pResult->Add_Shape(pPolygons->Get_Shape(i), SHAPE_COPY_GEOM);
		}

		pPolygons	= pResult;
	}

	//-----------------------------------------------------
	int	Fields	= pPolygons->Get_Field_Count();

	for(int iClass=0; iClass<m_Classes.Get_Count(); iClass++)
	{
		pPolygons->Add_Field(m_Classes.asString(iClass), SG_DATATYPE_String);
	}

	//-----------------------------------------------------
	bool	bCenter	= Parameters("METHOD")->asInt() == 0;

	Process_Set_Text(_TL("calculating class areas"));

	//-----------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(i);

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			TSG_Point	p; p.y = Get_YMin() + y * Get_Cellsize(); p.x = Get_XMin();

			for(int x=0; x<Get_NX(); x++, p.x+=Get_Cellsize())
			{
				double	Area	= Get_Intersection(pPolygon, p, bCenter);

				if( Area > 0.0 )
				{
					pPolygon->Add_Value(Fields + Get_Class(pGrid->asDouble(x, y)), 1);
				}
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pPolygons);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Class_Statistics_For_Polygons::Get_Classes(CSG_Grid *pGrid)
{
	Process_Set_Text(_TL("analyzing classes"));

	m_Classes.Create(SG_DATATYPE_Int);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				m_Classes	+= pGrid->asDouble(x, y);
			}
		}
	}

	return( m_Classes.Get_Count() > 0 );
}

//---------------------------------------------------------
inline int CGrid_Class_Statistics_For_Polygons::Get_Class(double Value)
{
	return( m_Classes.Get_Category(Value) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid_Class_Statistics_For_Polygons::Get_Intersection(CSG_Shape_Polygon *pPolygon, TSG_Point p, bool bCenter)
{
	//-----------------------------------------------------
	if( bCenter )
	{
		return( pPolygon->Contains(p) ? Get_Cellarea() : 0.0 );
	}

	//-----------------------------------------------------
	CSG_Shapes	Cells(SHAPE_TYPE_Polygon);
	CSG_Shape	*pCell	= Cells.Add_Shape();
	CSG_Shape	*pArea	= Cells.Add_Shape();

	pCell->Add_Point(p.x - 0.5 * Get_Cellsize(), p.y - 0.5 * Get_Cellsize());
	pCell->Add_Point(p.x - 0.5 * Get_Cellsize(), p.y + 0.5 * Get_Cellsize());
	pCell->Add_Point(p.x + 0.5 * Get_Cellsize(), p.y + 0.5 * Get_Cellsize());
	pCell->Add_Point(p.x + 0.5 * Get_Cellsize(), p.y - 0.5 * Get_Cellsize());

	if( SG_Polygon_Intersection(pPolygon, pCell, pArea) )
	{
		return( ((CSG_Shape_Polygon *)pArea)->Get_Area() );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
