
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "srtm30.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSRTM30_Import::CSRTM30_Import(void)
{
	Set_Name		(_TL("Import SRTM30 DEM"));

	Set_Author		("O.Conrad (c) 2004");

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
		"the \"Path\" Parameter of this tool.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"GRID"		, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath("",
		"PATH"		, _TL("Path"),
		_TL(""),
		NULL, NULL, false, true
	);

	//-----------------------------------------------------
	Parameters.Add_Int("", "XMIN", _TL("West" ), _TL(""),  60);
	Parameters.Add_Int("", "XMAX", _TL("East" ), _TL(""), 120);
	Parameters.Add_Int("", "YMIN", _TL("South"), _TL(""),  20);
	Parameters.Add_Int("", "YMAX", _TL("North"), _TL(""),  50);
}


///////////////////////////////////////////////////////////
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
	CSG_Grid	*pOut;

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
	pOut->Get_Projection().Create(SG_T("GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]"));

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSRTM30_Import::Tile_Load(const SG_Char *sTile, TSG_Rect &rTile, CSG_Grid *pOut, TSG_Rect &rOut)
{
	short		Value;
	int			x, y, xOut, yOut;
	FILE		*Stream;
	CSG_Rect	r(rTile);
	CSG_String	fName(sTile);

	if( r.Intersects(rOut) != INTERSECTION_None && (Stream = fopen(fName.b_str(), "rb")) != NULL )
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
