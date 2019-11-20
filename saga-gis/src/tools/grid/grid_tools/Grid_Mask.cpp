
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Mask.cpp                      //
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
#include "Grid_Mask.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Mask::CGrid_Mask(void)
{
	Set_Name		(_TL("Grid Masking"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Cells of the input grid will be set to no-data, if their cell "
		"center lies outside or within a no-data cell of the mask grid."
	));

	//-----------------------------------------------------
	Parameters.Add_Bool(Parameters.Get_Grid_System_Parameter(),
		"LIST"			, _TL("List Processing"),
		_TL(""),
		false
	);

	Parameters.Add_Grid("",
		"GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"MASKED"		, _TL("Masked Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid_List("",
		"GRIDS"			, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Bool("GRIDS",
		"GRIDS_CREATE"	, _TL("Create Copies"),
		_TL("Work on copies instead of overwriting the originals."),
		false
	);

	Parameters.Add_Grid_List("",
		"GRIDS_MASKED"	, _TL("Masked Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"MASK"			, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Choice("MASK",
		"NODATA"		, _TL("Mask Cells"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("no-data cells"),
			_TL("data cells")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Mask::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("LIST") )
	{
		pParameters->Set_Enabled("GRID"        , pParameter->asBool() == false);
		pParameters->Set_Enabled("MASKED"      , pParameter->asBool() == false);

		pParameters->Set_Enabled("GRIDS"       , pParameter->asBool() ==  true);
		pParameters->Set_Enabled("GRIDS_MASKED", pParameter->asBool() ==  true);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Mask::On_Execute(void)
{
	CSG_Grid	*pMask	= Parameters("MASK")->asGrid();

	if( !pMask->is_Intersecting(Get_System().Get_Extent()) )
	{
		Message_Add(_TL("no intersection with mask grid."));

		return( false );
	}

	bool	bNoData	= Parameters("NODATA")->asInt() == 0;

	//-----------------------------------------------------
	return( Parameters("LIST")->asBool()
		? Mask_Grids(pMask, bNoData)
		: Mask_Grid (pMask, bNoData)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Mask::Mask_Grid(CSG_Grid *pMask, bool bNoData)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( Parameters("MASKED")->asGrid() && Parameters("MASKED")->asGrid() != pGrid )
	{
		CSG_Grid	*pMasked	= Parameters("MASKED")->asGrid();

		pMasked->Create(*pGrid);
		pMasked->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("masked"));

		pGrid	= pMasked;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				double	px	= Get_XMin() + x * Get_Cellsize();

				if( bNoData != pMask->is_InGrid_byPos(px, py) )
				{
					pGrid->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pGrid);

	if( pGrid != Parameters("GRID")->asGrid() )
	{
		DataObject_Set_Parameters(pGrid, Parameters("GRID")->asGrid());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Mask::Mask_Grids(CSG_Grid *pMask, bool bNoData)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( Parameters("GRIDS_CREATE")->asBool() )
	{
		CSG_Parameter_Grid_List	*pList	= Parameters("GRIDS_MASKED")->asGridList();

		pList->Del_Items();

		for(int i=0; i<pGrids->Get_Item_Count(); i++)
		{
			CSG_Data_Object	*pMasked	= pGrids->Get_Item(i);

			switch( pMasked->Get_ObjectType() )
			{
			case SG_DATAOBJECT_TYPE_Grid : pMasked = SG_Create_Grid (*pMasked->asGrid ()); break;
			case SG_DATAOBJECT_TYPE_Grids: pMasked = SG_Create_Grids(*pMasked->asGrids()); break;
			}

			if( !pMasked )
			{
				Error_Set(_TL("failed to create an input data set copy"));

				return( false );
			}

			pMasked->Fmt_Name("%s [%s]", pGrids->Get_Item(i)->Get_Name(), _TL("masked"));

			pList->Add_Item(pMasked);
		}

		pGrids	= pList;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	px	= Get_XMin() + x * Get_Cellsize();

			if( bNoData != pMask->is_InGrid_byPos(px, py) )
			{
				for(int i=0; i<pGrids->Get_Grid_Count(); i++)
				{
					if( !pGrids->Get_Grid(i)->is_NoData(x, y) )
					{
						pGrids->Get_Grid(i)->Set_NoData(x, y);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pInput	= Parameters("GRIDS")->asGridList();

	for(int i=0; i<pGrids->Get_Item_Count(); i++)
	{
		if( pGrids == pInput )
		{
			DataObject_Update        (pGrids->Get_Item(i));
		}
		else if( Parameters("GRIDS_CREATE")->asBool() )
		{
			DataObject_Add           (pGrids->Get_Item(i));
			DataObject_Set_Parameters(pGrids->Get_Item(i), pInput->Get_Item(i));
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
