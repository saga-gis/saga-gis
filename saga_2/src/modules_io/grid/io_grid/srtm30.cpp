
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      SRTM30.cpp                       //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "srtm30.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSRTM30_Import::CSRTM30_Import(void)
{
	CSG_Parameter	*pNode_0;
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Set_Name(_TL("Import SRTM30 DEM"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description	(_TW(
		"Extracts elevation grids from SRTM30 data.\n\n"

		"<i>\"SRTM30 is a near-global digital elevation model (DEM) comprising a "
		"combination of data from the Shuttle Radar Topography Mission, flown "
		"in February, 2000 and the the U.S. Geological Survey's GTOPO30 data "
		"set. It can be considered to be either an SRTM data set enhanced with "
		"GTOPO30, or as an upgrade to GTOPO30.\"</i> (NASA)\n\n"

		"Further information about the GTOPO30 data set:\n"
		"<a target=\"_blank\" href=\"http://edcdaac.usgs.gov/gtopo30/gtopo30.html\">"
		"http://edcdaac.usgs.gov/gtopo30/gtopo30.html</a>\n\n"

		"SRTM30 data can be downloaded from:\n"
		"<a target=\"_blank\" href=\"ftp://e0srp01u.ecs.nasa.gov/srtm/version2/SRTM30/\">"
		"ftp://e0srp01u.ecs.nasa.gov/srtm/version2/SRTM30/</a>\n\n"

		"A directory, that contains the uncompressed SRTM30 DEM files, can be located using "
		"the \"Path\" Parameter of this module.")
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		_TL("")
	);

	pNode_0	= Parameters.Add_FilePath(
		NULL	, "PATH"		, _TL("Path"),
		_TL(""),
		NULL, NULL, false, true
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Value(
		NULL	, "XMIN"		, _TL("West []"),
		_TL(""),
		PARAMETER_TYPE_Int, 60.0
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "XMAX"		, _TL("East []"),
		_TL(""),
		PARAMETER_TYPE_Int, 120.0
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "YMIN"		, _TL("South []"),
		_TL(""),
		PARAMETER_TYPE_Int, 20.0
	);
	pNode_0	= Parameters.Add_Value(
		NULL	, "YMAX"		, _TL("North []"),
		_TL(""),
		PARAMETER_TYPE_Int, 50.0
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("TILE", _TL(""), _TL(""));

	pNode_0	= pParameters->Add_Info_String(
		NULL	, "INFO"		, _TL("File does not exist:"),
		_TL(""),
		_TL("")
	);

	pNode_0	= pParameters->Add_FilePath(
		NULL	, "PATH"		, _TL("Select file"),
		_TL(""),
		_TL("SRTM30 DEM Tiles (*.dem)|*.dem|All Files|*.*")
	);
}

//---------------------------------------------------------
CSRTM30_Import::~CSRTM30_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	X_WIDTH		4800
#define	Y_WIDTH		6000

//---------------------------------------------------------
bool CSRTM30_Import::On_Execute(void)
{
	char	x_sTile[9][5]	= {	"W180", "W140", "W100", "W060", "W020", "E020", "E060", "E100", "E140"	},
			y_sTile[3][4]	= {	"S10", "N40", "N90"	};

	double	dSize			= 30.0 / (60.0 * 60.0);

	//-----------------------------------------------------
	int			xTile, yTile;
	double		xMin, xMax, yMin, yMax;
	TSG_Rect	rOut, rTile;
	CSG_String	sTile;
	CSG_Grid		*pOut;

	//-----------------------------------------------------
	xMin		= Parameters("XMIN")->asInt();
	xMax		= Parameters("XMAX")->asInt();
	yMin		= Parameters("YMIN")->asInt();
	yMax		= Parameters("YMAX")->asInt();

	rOut.xMin	= (180 + xMin) / 40.0 * X_WIDTH;
	rOut.xMax	= rOut.xMin + (int)((xMax - xMin) / dSize);
	rOut.yMin	= ( 60 + yMin) / 50.0 * Y_WIDTH;
	rOut.yMax	= rOut.yMin + (int)((yMax - yMin) / dSize);

	//-----------------------------------------------------
	pOut		= SG_Create_Grid(SG_DATATYPE_Short,
					(int)(rOut.xMax - rOut.xMin),
					(int)(rOut.yMax - rOut.yMin),
					dSize,
					xMin + 0.5 * dSize,
					yMin + 0.5 * dSize
				);

	pOut->Set_NoData_Value(-9999);
	pOut->Assign_NoData();
	pOut->Set_Name(SG_T("SRTM30"));

	//-----------------------------------------------------
	for(yTile=0, rTile.yMin=0, rTile.yMax=Y_WIDTH; yTile<3; yTile++, rTile.yMin+=Y_WIDTH, rTile.yMax+=Y_WIDTH)
	{
		for(xTile=0, rTile.xMin=0, rTile.xMax=X_WIDTH; xTile<9; xTile++, rTile.xMin+=X_WIDTH, rTile.xMax+=X_WIDTH)
		{
			sTile.Printf(SG_T("Tile: %s%s"), x_sTile[xTile], y_sTile[yTile]);
			Process_Set_Text(sTile);

			sTile.Printf(SG_T("%s%s%s.dem"), Parameters("PATH")->asString(), x_sTile[xTile], y_sTile[yTile]);
			Tile_Load(sTile, rTile, pOut, rOut);
		}
	}

	//-----------------------------------------------------
	Parameters("GRID")->Set_Value(pOut);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSRTM30_Import::Tile_Load(const SG_Char *sTile, TSG_Rect &rTile, CSG_Grid *pOut, TSG_Rect &rOut)
{
	short		Value;
	int			x, y, xOut, yOut;
	FILE		*Stream;
	CSG_Rect	r(rTile);

	if( r.Intersects(rOut) != INTERSECTION_None && (Stream = Tile_Open(sTile)) != NULL )
	{
		for(y=0, yOut=(int)(rTile.yMax-rOut.yMin); y<Y_WIDTH && yOut>=0 && Set_Progress(y, Y_WIDTH); y++, yOut--)
		{
			for(x=0, xOut=(int)(rTile.xMin-rOut.xMin); x<X_WIDTH; x++, xOut++)
			{
				fread(&Value, 1, sizeof(short), Stream);

				if(	xOut >= 0 && xOut < pOut->Get_NX() && yOut >= 0 && yOut < pOut->Get_NY() )
				{
					SG_Swap_Bytes(&Value, sizeof(short));

					pOut->Set_Value(xOut, yOut, Value);
				}
			}
		}

		fclose(Stream);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
FILE * CSRTM30_Import::Tile_Open(const SG_Char *sTile)
{
	const SG_Char	*sPath;
	FILE			*Stream;
	CSG_String		fName;
	CSG_Parameters	*pParameters;

	fName	= sTile;

	if( (Stream = fopen(fName.b_str(), "rb")) == NULL )
	{
		pParameters	= Get_Parameters("TILE");
		pParameters->Get_Parameter("INFO")->Set_Value(sTile);

		if( Dlg_Parameters(pParameters, _TL("Locate STRM30 Data File")) && (sPath = pParameters->Get_Parameter("PATH")->asString()) != NULL )
		{
			fName	= sPath;
			Stream	= fopen(fName.b_str(), "rb");
		}
	}

	return( Stream );
}
