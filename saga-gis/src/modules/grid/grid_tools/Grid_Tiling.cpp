/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Tiling.cpp                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "Grid_Tiling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Tiling::CGrid_Tiling(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Tiling"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "TILES"			, _TL("Tiles"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	pNode	= Parameters.Add_Value(
		NULL	, "OVERLAP"			, _TL("Overlapping Cells"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	Parameters.Add_Choice(
		pNode	, "OVERLAP_SYM"		, _TL("Add Cells"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("symmetric"),
			_TL("bottom / left"),
			_TL("top / right")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Tile Size Definition"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("number of grid cells per tile"),
			_TL("coordinates (offset, range, cell size, tile size)")
		), 0
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_A"			, _TL("Number of Grid Cells"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "NX"				, _TL("Number of Column Cells"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		pNode	, "NY"				, _TL("Number of Row Cells"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_B"			, _TL("Coordinates"),
		_TL("")
	);

	Parameters.Add_Range(
		pNode	, "XRANGE"			, _TL("Offset and Range (X)"),
		_TL(""),
		0.0, 1000.0
	);

	Parameters.Add_Range(
		pNode	, "YRANGE"			, _TL("Offset and Range (Y)"),
		_TL(""),
		0.0, 1000.0
	);

	Parameters.Add_Value(
		pNode	, "DCELL"			, _TL("Cell Size"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "DX"				, _TL("Tile Size (X)"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "DY"				, _TL("Tile Size (Y)"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Tiling::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("PARAMETERS_GRID_SYSTEM")) )
	{
		CSG_Grid_System	System(1.0, 0.0, 0.0, 101, 101);

		if( pParameter->asGrid_System() && pParameter->asGrid_System()->is_Valid() )
		{
			System	= *pParameter->asGrid_System();
		}

		pParameters->Get_Parameter("NX")	->Set_Value(System.Get_NX() / 2);
		pParameters->Get_Parameter("NY")	->Set_Value(System.Get_NX() / 2);

		pParameters->Get_Parameter("XRANGE")->asRange()->Set_Range(System.Get_XMin(), System.Get_XMax());
		pParameters->Get_Parameter("YRANGE")->asRange()->Set_Range(System.Get_YMin(), System.Get_YMax());
		pParameters->Get_Parameter("DCELL")	->Set_Value(System.Get_Cellsize());
		pParameters->Get_Parameter("DX")	->Set_Value(System.Get_XRange() / 2.0);
		pParameters->Get_Parameter("DY")	->Set_Value(System.Get_YRange() / 2.0);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Tiling::On_Execute(void)
{
	int						ix, iy, nx, ny, Overlap;
	double					y, x, dx, dy, dCell;
	TSG_Data_Type			Type;
	TSG_Rect				Extent;
	TSG_Grid_Interpolation	Interpolation;
	CSG_Grid				*pGrid, *pTile;
	CSG_Parameter_Grid_List	*pTiles;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pTiles	= Parameters("TILES")	->asGridList();
	Overlap	= Parameters("OVERLAP")	->asInt();

	switch( Parameters("METHOD")->asInt() )
	{
	case 0: default:
		Extent.xMin		= pGrid->Get_XMin();
		Extent.xMax		= pGrid->Get_XMax();
		Extent.yMin		= pGrid->Get_YMin();
		Extent.yMax		= pGrid->Get_YMax();
		dCell			= pGrid->Get_Cellsize();
		nx				= Parameters("NX")		->asInt();
		ny				= Parameters("NY")		->asInt();
		dx				= dCell * nx;
		dy				= dCell * ny;
		Type			= pGrid->Get_Type();
		Interpolation	= GRID_INTERPOLATION_NearestNeighbour;
		break;

	case 1:
		Extent.xMin		= Parameters("XRANGE")	->asRange()->Get_LoVal();
		Extent.xMax		= Parameters("XRANGE")	->asRange()->Get_HiVal();
		Extent.yMin		= Parameters("YRANGE")	->asRange()->Get_LoVal();
		Extent.yMax		= Parameters("YRANGE")	->asRange()->Get_HiVal();
		dCell			= Parameters("DCELL")	->asDouble();
		dx				= Parameters("DX")		->asDouble();
		dy				= Parameters("DY")		->asDouble();
		nx				= (int)(dx / dCell);
		ny				= (int)(dy / dCell);
		Type			= pGrid->Get_Type();
		Interpolation	= GRID_INTERPOLATION_Undefined;
		break;
	}

	switch( Parameters("OVERLAP_SYM")->asInt() )
	{
	case 0: default:	// symetric
		nx		+= Overlap * 2;
		ny		+= Overlap * 2;
		break;

	case 1:				// bottom / left
		nx		+= Overlap;
		ny		+= Overlap;
		break;

	case 2:				// top / right
		nx		+= Overlap;
		ny		+= Overlap;
		Overlap	 = 0;
		break;
	}

	pTiles->Del_Items();

	//-----------------------------------------------------
	if( dx <= 0.0 || dy <= 0.0 || dCell <= 0.0 )
	{
		Message_Add(_TL("no intersection with mask grid."));

		return( false );
	}

	//-----------------------------------------------------
	for(y=Extent.yMin, iy=1; y<Extent.yMax && Process_Get_Okay(); y+=dy, iy++)
	{
		for(x=Extent.xMin, ix=1; x<Extent.xMax; x+=dx, ix++)
		{
			pTile	= SG_Create_Grid(Type, nx, ny, dCell, x - dCell * Overlap, y - dCell * Overlap);
			pTile	->Assign(pGrid, Interpolation);
			pTile	->Set_Name(CSG_String::Format(SG_T("%s [%d, %d]"), pGrid->Get_Name(), iy, ix));

			if( pTile->Get_NoData_Count() == pTile->Get_NCells() )
			{
				delete(pTile);
			}
			else
			{
				pTiles->Add_Item(pTile);
			}
		}
	}

	return( pTiles->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
