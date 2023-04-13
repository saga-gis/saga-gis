
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
//                  Grid_To_Points.cpp                   //
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
#include "Grid_To_Points.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_To_Points::CGrid_To_Points(void)
{
	Set_Name		(_TL("Grid Values to Points"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"This tool saves grid values to point (grid nodes) or polygon (grid cells) shapes. Optionally only points "
		"can be saved, which are contained by polygons of the specified shapes layer. "
		"In addition, it is possible to exclude all cells that are coded NoData in the "
		"first grid of the grid list."
	));

	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("POLYGONS",
		"ATTRIBUTE"	, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"CELLS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"NODATA"	, _TL("No-Data Cells"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("include all cells"),
			_TL("include cell if at least one grid provides data"),
			_TL("exclude cell if at least one grid does not provide data")
		)
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("nodes"),
			_TL("cells")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_To_Points::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TYPE") )
	{
		pParameters->Set_Enabled("POINTS", pParameter->asInt() == 0);
		pParameters->Set_Enabled("CELLS" , pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Points::On_Execute(void)
{
	CSG_Parameter_Grid_List *pGrids = Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grids in input list"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid Mask; CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	Get_Mask(Mask, Parameters("NODATA")->asInt() ? pGrids : NULL, pPolygons);

	//-----------------------------------------------------
	CSG_Shapes *pShapes;

	switch( Parameters("TYPE")->asInt() )
	{
	default: pShapes = Parameters("POINTS")->asShapes(); pShapes->Create(SHAPE_TYPE_Point  , CSG_String::Format("%s %s", _TL("Grid"), _TL("Nodes"))); break;
	case 1 : pShapes = Parameters("CELLS" )->asShapes(); pShapes->Create(SHAPE_TYPE_Polygon, CSG_String::Format("%s %s", _TL("Grid"), _TL("Cells"))); break;
	}

	int Attribute = Parameters("ATTRIBUTE")->asInt();

	if( pPolygons )
	{
		if( Attribute >= 0 )
		{
			pShapes->Add_Field(pPolygons->Get_Field_Name(Attribute), pPolygons->Get_Field_Type(Attribute));
		}
		else
		{
			pShapes->Add_Field("POLYGON_NR", SG_DATATYPE_Int);
		}
	}

	int Offset = pShapes->Get_Field_Count();

	for(int i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		CSG_Grid *pGrid = pGrids->Get_Grid(i);

		pShapes->Add_Field(pGrid->Get_Name(), pGrid->is_Scaled() ? SG_DATATYPE_Double : pGrid->Get_Type());
	}

	//-------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		double py = Get_YMin() + y * Get_Cellsize();

		for(int x=0; x<Get_NX(); x++)
		{
			double px = Get_XMin() + x * Get_Cellsize();

			if( !Mask.is_Valid() || !Mask.is_NoData(x, y) )
			{
				CSG_Shape *pShape = pShapes->Add_Shape();

				if( pShapes->Get_Type() == SHAPE_TYPE_Polygon )
				{
					#define dc (Get_Cellsize() / 2.)

					pShape->Add_Point(px - dc, py - dc);
					pShape->Add_Point(px + dc, py - dc);
					pShape->Add_Point(px + dc, py + dc);
					pShape->Add_Point(px - dc, py + dc);
				}
				else
				{
					pShape->Add_Point(px     , py     );
				}

				if( pPolygons )
				{
					int i = Mask.asInt(x, y);

					if( Attribute >= 0 && i >= 0 && i < (int)pPolygons->Get_Count() )
					{
						pShape->Set_Value(0, pPolygons->Get_Shape(i)->asString(Attribute));
					}
					else
					{
						pShape->Set_Value(0, i + 1);
					}
				}

				for(int i=0; i<pGrids->Get_Grid_Count(); i++)
				{
					CSG_Grid *pGrid = pGrids->Get_Grid(i);

					if( pGrid->is_NoData(x, y) )
					{
						pShape->Set_NoData(Offset + i);
					}
					else
					{
						pShape->Set_Value(Offset + i, pGrid->asDouble(x, y));
					}
				}
			}
		}
	}

	return( pShapes->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Points::Get_Mask(CSG_Grid &Mask, CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pPolygons)
{
	if( pGrids == NULL && pPolygons == NULL )
	{
		return( false ); // no mask needed, just include all cells!
	}

	Mask.Create(Get_System(), pPolygons ? SG_DATATYPE_Int : SG_DATATYPE_Char);

	Mask.Set_NoData_Value(-1.);

	bool bSkipNoData = Parameters("NODATA")->asInt() == 2;

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			int id = 0;

			if( !id && pGrids )
			{
				if( bSkipNoData ) // exclude cell if at least one grid does not provide data
				{
					for(int i=0; !id && i<pGrids->Get_Grid_Count(); i++)
					{
						if( pGrids->Get_Grid(i)->is_NoData(x, y) )
						{
							id = -1; // at least one grid provides no data!
						}
					}
				}
				else              // include cell if at least one grid provides data
				{
					id = -1;

					for(int i=0; id<0 && i<pGrids->Get_Grid_Count(); i++)
					{
						if( !pGrids->Get_Grid(i)->is_NoData(x, y) )
						{
							id = 0; // at least one grid provides a value!
						}
					}
				}
			}

			if( !id && pPolygons )
			{
				id = -1;

				CSG_Point p(Get_XMin() + x * Get_Cellsize(), Get_YMin() + y * Get_Cellsize());

				for(int i=0; id<0 && i<(int)pPolygons->Get_Count(); i++)
				{
					if( pPolygons->Get_Shape(i)->asPolygon()->Contains(p) )
					{
						id = i;
					}
				}
			}

			Mask.Set_Value(x, y, id);
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
