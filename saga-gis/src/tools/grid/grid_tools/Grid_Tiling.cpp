/**********************************************************
 * Version $Id$
 *********************************************************/

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
#include "Grid_Tiling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Tiling::CGrid_Tiling(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Tiling"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(NULL,
		"TILES"			, _TL("Tiles"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	Parameters.Add_Bool(NULL,
		"TILES_SAVE"	, _TL("Save Tiles to Disk"),
		_TL("Save tiles to disk individually"),
		false
	);

	Parameters.Add_FilePath(Parameters("TILES_SAVE"),
		"TILES_PATH"    , _TL("Output Directory"),
		_TL(""),
		NULL, NULL, true, true
	);

	Parameters.Add_String(Parameters("TILES_SAVE"),
		"TILES_NAME"	, _TL("Base Name"),
		_TL("The base name of the tiles"),
		"tile"
	);

	//-----------------------------------------------------
	Parameters.Add_Int(NULL,
		"OVERLAP"		, _TL("Overlapping Cells"),
		_TL(""),
		0, 0, true
	);

	Parameters.Add_Choice(Parameters("OVERLAP"),
		"OVERLAP_SYM"	, _TL("Add Cells"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("symmetric"),
			_TL("bottom / left"),
			_TL("top / right")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(NULL,
		"METHOD"		, _TL("Tile Size Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("number of grid cells per tile"),
			_TL("coordinates (offset, range, cell size, tile size)")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Int(Parameters("METHOD"),
		"NX"			, _TL("Number of Column Cells"),
		_TL(""),
		100, 1, true
	);

	Parameters.Add_Int(Parameters("METHOD"),
		"NY"			, _TL("Number of Row Cells"),
		_TL(""),
		100, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Range(Parameters("METHOD"),
		"XRANGE"		, _TL("Offset and Range (X)"),
		_TL(""),
		0.0, 1000.0
	);

	Parameters.Add_Range(Parameters("METHOD"),
		"YRANGE"		, _TL("Offset and Range (Y)"),
		_TL(""),
		0.0, 1000.0
	);

	Parameters.Add_Double(Parameters("METHOD"),
		"DCELL"			, _TL("Cell Size"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("METHOD"),
		"DX"			, _TL("Tile Size (X)"),
		_TL(""),
		100.0, 0.0, true
	);

	Parameters.Add_Double(Parameters("METHOD"),
		"DY"			, _TL("Tile Size (Y)"),
		_TL(""),
		100.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Tiling::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter == pParameters->Get_Grid_System_Parameter() )
	{
		CSG_Grid_System	System(1.0, 0.0, 0.0, 101, 101);

		if( pParameter->asGrid_System() && pParameter->asGrid_System()->is_Valid() )
		{
			System	= *pParameter->asGrid_System();
		}

		pParameters->Get_Parameter("NX"    )->Set_Value(System.Get_NX() / 2);
		pParameters->Get_Parameter("NY"    )->Set_Value(System.Get_NX() / 2);

		pParameters->Get_Parameter("XRANGE")->asRange()->Set_Range(System.Get_XMin(), System.Get_XMax());
		pParameters->Get_Parameter("YRANGE")->asRange()->Set_Range(System.Get_YMin(), System.Get_YMax());
		pParameters->Get_Parameter("DCELL" )->Set_Value(System.Get_Cellsize());
		pParameters->Get_Parameter("DX"    )->Set_Value(System.Get_XRange() / 2.0);
		pParameters->Get_Parameter("DY"    )->Set_Value(System.Get_YRange() / 2.0);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Tiling::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TILES_SAVE") )
	{
		pParameters->Set_Enabled("TILES_PATH", pParameter->asBool());
		pParameters->Set_Enabled("TILES_NAME", pParameter->asBool());
	}

	if( pParameter->Cmp_Identifier("OVERLAP") )
	{
		pParameters->Set_Enabled("OVERLAP_SYM", pParameter->asInt() > 0);
	}

	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("NX"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("NY"    , pParameter->asInt() == 0);

		pParameters->Set_Enabled("XRANGE", pParameter->asInt() == 1);
		pParameters->Set_Enabled("YRANGE", pParameter->asInt() == 1);
		pParameters->Set_Enabled("DCELL" , pParameter->asInt() == 1);
		pParameters->Set_Enabled("DX"    , pParameter->asInt() == 1);
		pParameters->Set_Enabled("DY"    , pParameter->asInt() == 1);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Tiling::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	CSG_Parameter_Grid_List	*pTiles	= Parameters("TILES")->asGridList();

	pTiles->Del_Items();

	//-----------------------------------------------------
	int					nx, ny;
	double				dx, dy, dCell;
	TSG_Rect			Extent;
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("METHOD")->asInt() )
	{
	default:	// number of grid cells per tile
		Extent.xMin	= pGrid->Get_XMin();
		Extent.xMax	= pGrid->Get_XMax();
		Extent.yMin	= pGrid->Get_YMin();
		Extent.yMax	= pGrid->Get_YMax();
		dCell		= pGrid->Get_Cellsize();
		nx			= Parameters("NX")->asInt();
		ny			= Parameters("NY")->asInt();
		dx			= dCell * nx;
		dy			= dCell * ny;
		Resampling	= GRID_RESAMPLING_NearestNeighbour;
		break;

	case  1:	// coordinates (offset, range, cell size, tile size)
		Extent.xMin	= Parameters("XRANGE")->asRange()->Get_Min();
		Extent.xMax	= Parameters("XRANGE")->asRange()->Get_Max();
		Extent.yMin	= Parameters("YRANGE")->asRange()->Get_Min();
		Extent.yMax	= Parameters("YRANGE")->asRange()->Get_Max();
		dCell		= Parameters("DCELL" )->asDouble();
		dx			= Parameters("DX"    )->asDouble();
		dy			= Parameters("DY"    )->asDouble();
		nx			= (int)(dx / dCell);
		ny			= (int)(dy / dCell);
		Resampling	= GRID_RESAMPLING_Undefined;
		break;
	}

	//-----------------------------------------------------
	if( dx <= 0.0 || dy <= 0.0 || dCell <= 0.0 )
	{
		Message_Add(_TL("no intersection with mask grid."));

		return( false );
	}

	//-----------------------------------------------------
	int	Overlap	= Parameters("OVERLAP")->asInt();

	switch( Parameters("OVERLAP_SYM")->asInt() )
	{
	default:	// symetric
		nx		+= Overlap * 2;
		ny		+= Overlap * 2;
		break;

	case  1:	// bottom / left
		nx		+= Overlap;
		ny		+= Overlap;
		break;

	case  2:	// top / right
		nx		+= Overlap;
		ny		+= Overlap;
		Overlap	 = 0;
		break;
	}

	//-----------------------------------------------------
	CSG_String	Tiles_Name, Tiles_Path;

	bool	Tiles_bSave	= Parameters("TILES_SAVE")->asBool();

	if( Tiles_bSave )
	{
		Tiles_Name	= Parameters("TILES_NAME")->asString();

		if( Tiles_Name.is_Empty() )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid base name for the output files!"));

			return( false );
		}

		Tiles_Path	= Parameters("TILES_PATH")->asString();

		if( !SG_Dir_Exists(SG_File_Get_Path(Tiles_Path)) )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid output directory for the output files!"));

			return( false );
		}
	}


	//-----------------------------------------------------
	int	iy	= 1, nTiles = 0;

	for(double y=Extent.yMin; y<Extent.yMax && Process_Get_Okay(); y+=dy, iy++)
	{
		int	ix	= 1;

		for(double x=Extent.xMin; x<Extent.xMax; x+=dx, ix++)
		{
			CSG_Grid	*pTile	= SG_Create_Grid(pGrid->Get_Type(), nx, ny, dCell,
				x - dCell * Overlap,
				y - dCell * Overlap
			);

			pTile->Assign(pGrid, Resampling);

			if( pTile->Get_NoData_Count() == pTile->Get_NCells() )
			{
				delete(pTile);
			}
			else
			{
				if( Tiles_bSave )
				{
					CSG_String FileName = CSG_String::Format("%s/%s_%d_%d.sgrd", Tiles_Path.c_str(), Tiles_Name.c_str(), iy, ix);

					pTile->Save(FileName);

					delete(pTile);
				}
				else
				{
					pTile->Fmt_Name("%s [%d, %d]", pGrid->Get_Name(), iy, ix);

					pTiles->Add_Item(pTile);
				}

				nTiles++;
			}
		}
	}

	SG_UI_Msg_Add(CSG_String::Format("%s: %d", _TL("Number of tiles"), nTiles), true);

	return( nTiles > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
