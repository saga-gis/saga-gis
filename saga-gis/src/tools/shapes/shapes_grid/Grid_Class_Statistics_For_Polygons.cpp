
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

	Parameters.Add_Choice(NULL,
		"GRID_VALUES"	, _TL("Value Interpretation"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("values are class identifiers"),
			_TL("use look-up table")
		), 1
	);

	CSG_Parameter	*pNode	= Parameters.Add_Table(NULL, "GRID_LUT", _TL("Look-up Table"  ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Table_Field(pNode, "GRID_LUT_MIN", _TL("Value"          ), _TL(""), false);
	Parameters.Add_Table_Field(pNode, "GRID_LUT_MAX", _TL("Value (Maximum)"), _TL(""), true );
	Parameters.Add_Table_Field(pNode, "GRID_LUT_NAM", _TL("Name"           ), _TL(""), true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Class_Statistics_For_Polygons::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("GRID_VALUES") )
	{
		pParameters->Set_Enabled("GRID_LUT", pParameter->asInt() == 1);
	}

	if(	pParameter->Cmp_Identifier("GRID_LUT") )
	{
		pParameters->Set_Enabled("GRID_LUT_MIN", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("GRID_LUT_MAX", pParameter->asTable() != NULL);
		pParameters->Set_Enabled("GRID_LUT_NAM", pParameter->asTable() != NULL);
	}

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
	int	fStart	= pPolygons->Get_Field_Count();

	if( !Get_Classes(pGrid, pPolygons) )
	{
		Error_Set(_TL("undefined grid classes"));

		return( false );
	}

	//-----------------------------------------------------
	bool	bCenter	= Parameters("METHOD")->asInt() == 0;

	Process_Set_Text(_TL("calculating class areas"));

	//-----------------------------------------------------
	int			x, y;
	TSG_Point	p;

	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( m_Classes.asInt(x, y) >= 0 )
			{
				int	fClass	= fStart + m_Classes.asInt(x, y);

				#pragma omp parallel for
				for(int i=0; i<pPolygons->Get_Count(); i++)
				{
					CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(i);

					double	Area	= Get_Intersection(pPolygon, p, bCenter);

					if( Area > 0.0 )
					{
						pPolygon->Add_Value(fClass, Area);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Classes.Destroy();

	DataObject_Update(pPolygons);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Class_Statistics_For_Polygons::Get_Classes(CSG_Grid *pGrid, CSG_Shapes *pPolygons)
{
	m_Classes.Destroy();

	//-----------------------------------------------------
	if( Parameters("GRID_VALUES")->asInt() == 0 )
	{
		sLong	iCell;

		CSG_Category_Statistics	Classes(pGrid->Get_Type());

		for(iCell=0; iCell<pGrid->Get_NCells(); iCell++)
		{
			if( !pGrid->is_NoData(iCell) )
			{
				Classes	+= pGrid->asDouble(iCell);
			}
		}

		//-------------------------------------------------
		if( Classes.Get_Count() > 0 )
		{
			Classes.Sort();

			for(int iClass=0; iClass<Classes.Get_Count(); iClass++)
			{
				pPolygons->Add_Field(Classes.asString(iClass), SG_DATATYPE_Double);
			}

			m_Classes.Create(*Get_System(), SG_DATATYPE_Short);

			for(iCell=0; iCell<pGrid->Get_NCells(); iCell++)
			{
				m_Classes.Set_Value(iCell, pGrid->is_NoData(iCell) ? -1 : Classes.Get_Category(pGrid->asDouble(iCell)));
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		int		fNam, fMin, fMax;

		CSG_Table	*pLUT	= NULL;

		if( (pLUT = Parameters("GRID_LUT")->asTable()) != NULL )
		{
			fNam	= Parameters("GRID_LUT_NAM")->asInt();
			fMin	= Parameters("GRID_LUT_MIN")->asInt();
			fMax	= Parameters("GRID_LUT_MAX")->asInt();

			if( fNam < 0 || fNam >= pLUT->Get_Field_Count() )	{	fNam	= fMin;	}
			if( fMax < 0 || fMax >= pLUT->Get_Field_Count() )	{	fMax	= fMin;	}
		}
		else if( DataObject_Get_Parameter(pGrid, "LUT") && (pLUT = DataObject_Get_Parameter(pGrid, "LUT")->asTable()) != NULL )
		{
			fNam	= 1;
			fMin	= 3;
			fMax	= 4;
		}

		//-------------------------------------------------
		if( pLUT && pLUT->Get_Count() > 0 )
		{
			for(int iClass=0; iClass<pLUT->Get_Count(); iClass++)
			{
				pPolygons->Add_Field((*pLUT)[iClass].asString(fNam), SG_DATATYPE_Double);
			}

			m_Classes.Create(*Get_System(), SG_DATATYPE_Short);

			for(sLong iCell=0; iCell<pGrid->Get_NCells(); iCell++)
			{
				m_Classes.Set_Value(iCell, Get_Class(pGrid->asDouble(iCell), *pLUT, fMin, fMax));
			}
		}
	}

	//-----------------------------------------------------
	return( m_Classes.is_Valid() );
}

//---------------------------------------------------------
int CGrid_Class_Statistics_For_Polygons::Get_Class(double Value, const CSG_Table &LUT, int fMin, int fMax)
{
	for(int i=0; i<LUT.Get_Count(); i++)
	{
		if( LUT[i].asDouble(fMin) <= Value && Value <= LUT[i].asDouble(fMax) )
		{
			return( i );
		}
	}

	return( -1 );
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
