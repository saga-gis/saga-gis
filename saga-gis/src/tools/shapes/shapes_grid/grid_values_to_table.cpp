
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
//                grid_values_to_table.cpp               //
//                                                       //
//                 Copyright (C) 2025 by                 //
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
#include "grid_values_to_table.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Values_to_Table_for_Points::CGrid_Values_to_Table_for_Points(void)
{
	Set_Name		(_TL("Collect Grid Values for Points"));

	Set_Author		("O.Conrad (c) 2025");

	Set_Description	(_TW(
		"Collect values from selected grids for given point locations. "
		"If input grid list provides a single grid collection object, "
		"its Z-attribute will be included in the resulting table. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"     , _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("",
		"LOCATIONS" , _TL("Locations"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("LOCATIONS",
		"LOCATION"  , _TL("Identifier"),
		_TL("If not provided location entries will simply be enumerated."),
		true
	);

	Parameters.Add_Table("",
		"VALUES"    , _TL("Values"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Values_to_Table_for_Points::On_Execute(void)
{
	CSG_Parameter_Grid_List *pGrids	= Parameters("GRIDS")->asGridList();

	if(	pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grids in input list"));

		return( false );
	}

	CSG_Grids *pCollection = pGrids->Get_Item_Count() == 1 ? pGrids->Get_Item(0)->asGrids() : NULL;

	//-----------------------------------------------------
	CSG_Shapes *pPoints = Parameters("LOCATIONS")->asShapes();

	if(	pPoints->Get_Count() < 1 )
	{
		Error_Set(_TL("no points in input locations"));

		return( false );
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling	= GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling	= GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling	= GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling	= GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	CSG_Table *pTable = Parameters("VALUES")->asTable(); pTable->Destroy();

	pTable->Add_Field("id", SG_DATATYPE_Int);

	if( pCollection )
	{
		pTable->Set_Name(pCollection->Get_Name());

		pTable->Add_Field(
			pCollection->Get_Attributes().Get_Field_Name(pCollection->Get_Z_Attribute()),
			pCollection->Get_Attributes().Get_Field_Type(pCollection->Get_Z_Attribute())
		);
	}
	else
	{
		pTable->Set_Name(_TL("Grid Values"));

		pTable->Add_Field("grid", SG_DATATYPE_String);
	}

	CSG_Points Points;

	for(int iPoint=0; iPoint<pPoints->Get_Count(); iPoint++)
	{
		CSG_Shape *pPoint = pPoints->Get_Shape(iPoint);

		bool bContained = false;

		for(int i=0; !bContained && i<pGrids->Get_Item_Count(); i++)
		{
			bContained =
			   (pGrids->Get_Item(i)->asGrid () && pGrids->Get_Item(i)->asGrid ()->Get_Extent().Contains(pPoint->Get_Point()))
			|| (pGrids->Get_Item(i)->asGrids() && pGrids->Get_Item(i)->asGrids()->Get_Extent().Contains(pPoint->Get_Point()));
		}

		if( bContained )
		{
			int PointID = Parameters("LOCATION")->asInt();

			pTable->Add_Field(PointID < 0
				? CSG_String::Format("%d", 1 + iPoint)
				: CSG_String::Format("%s", pPoint->asString(PointID)),
				SG_DATATYPE_Double
			);

			Points.Add(pPoint->Get_Point());
		}
	}

	if( Points.Get_Count() < 1 )
	{
		Error_Set(_TL("no point does match input grids extent"));

		return( false );
	}

	//-----------------------------------------------------
	for(int iGrid=0; iGrid<pGrids->Get_Grid_Count() && Set_Progress(iGrid, pGrids->Get_Grid_Count()); iGrid++)
	{
		CSG_Grid *pGrid = pGrids->Get_Grid(iGrid); CSG_Table_Record &Record = *pTable->Add_Record();

		Record.Set_Value(0, 1 + iGrid);

		if( pCollection )
		{
			Record.Set_Value(1, pCollection->Get_Z(iGrid));
		}
		else
		{
			Record.Set_Value(1, pGrid->Get_Name());
		}

		for(int iPoint=0; iPoint<Points.Get_Count(); iPoint++)
		{
			double Value;

			if( pGrid->Get_Value(Points[iPoint], Value, Resampling) )
			{
				Record.Set_Value(2 + iPoint, Value);
			}
			else
			{
				Record.Set_NoData(2 + iPoint);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
