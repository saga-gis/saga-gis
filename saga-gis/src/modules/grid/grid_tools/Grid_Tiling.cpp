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

	Parameters.Add_Value(
		NULL	, "SAVE_TILES"			, _TL("Save Tiles to Disk"),
		_TL("Save tiles to disk individually"),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_String(
		Parameters("SAVE_TILES"),	"TILE_BASENAME"	, _TL("Base Name"),
		_TL("The base name of the tiles"),
		SG_T("")
	);

	Parameters.Add_FilePath(
		Parameters("SAVE_TILES"),	"TILE_PATH"    , _TL("Output Directory"),
		_TL(""),
		NULL, NULL, true, true
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
	bool					bSaveTiles;
	int						ix, iy, nx, ny, Overlap;
	double					y, x, dx, dy, dCell;
	CSG_String				FilePath, BaseName;
	TSG_Data_Type			Type;
	TSG_Rect				Extent;
	TSG_Grid_Interpolation	Interpolation;
	CSG_Grid				*pGrid, *pTile;
	CSG_Parameter_Grid_List	*pTiles;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pTiles	= Parameters("TILES")	->asGridList();
	Overlap	= Parameters("OVERLAP")	->asInt();

	bSaveTiles	= Parameters("SAVE_TILES")		->asBool();
	BaseName	= Parameters("TILE_BASENAME")	->asString();
	FilePath	= Parameters("TILE_PATH")		->asString();


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

	if( bSaveTiles )
	{
		if( !SG_STR_CMP(BaseName, SG_T("")) )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid base name for the output files!"));
			return( false );
		}
		if( !SG_STR_CMP(FilePath, SG_T("")) )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid output directory for the output files!"));
			return( false );
		}
	}


	//-----------------------------------------------------
	int		iTiles = 0;

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
				if( bSaveTiles )
				{
					CSG_String FileName = CSG_String::Format(SG_T("%s/%s_%d_%d"), FilePath.c_str(), BaseName.c_str(), iy, ix);
					pTile->Save(FileName);
					delete(pTile);
				}
				else
				{
					pTiles->Add_Item(pTile);
				}

				iTiles++;
			}
		}
	}

	SG_UI_Msg_Add(CSG_String::Format(_TL("%d tiles created."), iTiles), true);

	return( iTiles > 0 );
}


//---------------------------------------------------------
int CGrid_Tiling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		pParameters->Get_Parameter("NODE_A"			)->Set_Enabled( pParameter->asInt() == 0 );
		pParameters->Get_Parameter("NODE_B"			)->Set_Enabled( pParameter->asInt() == 1 );
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SAVE_TILES")) )
	{
		pParameters->Get_Parameter("TILE_BASENAME"	)->Set_Enabled( pParameter->asBool() );
		pParameters->Get_Parameter("TILE_PATH"		)->Set_Enabled( pParameter->asBool() );
	}

	//-----------------------------------------------------
	return( 1 );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
