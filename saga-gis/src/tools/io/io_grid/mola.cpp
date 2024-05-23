
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
//                       MOLA.cpp                        //
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
#include "mola.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMOLA_Import::CMOLA_Import(void)
{
	Set_Name		(_TL("Import MOLA Grid (MEGDR)"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Import Mars Orbit Laser Altimeter (MOLA) grids of the Mars Global Surveyor (MGS) Mission "
		"(Topographic maps, Mission Experiment Gridded Data Records - MEGDRs). "
		"Find more information and obtain free data from "
		"<a target=\"_blank\" href=\"https://pds-geosciences.wustl.edu/missions/mgs/mola.html\">"
		"Mars Global Surveyor: MOLA (NASA)</a>\n\n"
	));

	Add_Reference(
		"https://pds-geosciences.wustl.edu/missions/mgs/mola.html", SG_T("Mars Global Surveyor: MOLA")
	);

	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output("",
		"GRID"  , _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath("",
		"FILE"  , _TL("File"),
		_TL(""),
		CSG_String::Format("MOLA %s (*.img)|*.img|%s|*.*",
			_TL("Grids"),
			_TL("All Files")
		)
	);

	Parameters.Add_Choice("",
		"TYPE"  , _TL("Grid Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("2 byte integer"),
			_TL("4 byte floating point")
		), 1
	);

	Parameters.Add_Choice("",
		"ORIENT", _TL("Orientation"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("normal"),
			_TL("down under")
		), 1
	);
}

//---------------------------------------------------------
bool CMOLA_Import::On_Execute(void)
{
	//-----------------------------------------------------
	// MEGpxxnyyyrv
	// 012345678901
	//  p indicates the product type (A for areoid, C for counts, R for
	//    radius, and T for topography)
	//  xx is the latitude of the upper left corner of the image
	//  n indicates whether the latitude is north (N) or south (S)
	//  yyy is the east longitude of the upper left corner of the image
	//  r is the map resolution using the pattern
	//    c =   4 pixel per degree
	//    e =  16 pixel per degree
	//    f =  32 pixel per degree
	//    g =  64 pixel per degree
	//    h = 128 pixel per degree
	//    (This convention is consistent with that used for the Mars Digital
	//    Image Model [MDIM] archives.)
	//  v is a letter indicating the product version.
	//-----------------------------------------------------

	CSG_String File(SG_File_Get_Name(Parameters("FILE")->asString(), false));

	File.Make_Upper();

	if( File.Length() < 12 )
	{
		Error_Set(_TL("input file name"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String Name;

	switch( File[3] )
	{
	case 'A': Name.Printf("MOLA: Areoid v%c"    , File[11]); break;
	case 'C': Name.Printf("MOLA: Counts v%c"    , File[11]); break;
	case 'R': Name.Printf("MOLA: Radius v%c"    , File[11]); break;
	case 'T': Name.Printf("MOLA: Topography v%c", File[11]); break;
	default: Error_Set(_TL("input file name")); return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System System; bool bDown = Parameters("ORIENT")->asInt() == 1;

	switch( File[10] )
	{
	case 'C': System.Create(1. /  4., -180., -90.,  4 * 360,  4 * 180); break; // 1 /  4th degree...
	case 'D': System.Create(1. /  8., -180., -90.,  8 * 360,  8 * 180); break; // 1 /  8th degree...
	case 'E': System.Create(1. / 16., -180., -90., 16 * 360, 16 * 180); break; // 1 / 16th degree...
	case 'F': System.Create(1. / 32., -180., -90., 32 * 360, 32 * 180); break; // 1 / 32th degree...

	case 'G': { // 1 /  64th degree...
		System.Create(1. /  64., 0., 0.,  64 * 180,  64 * 90);
		double xMin = File.Right(5).asInt(); if( xMin >= 180. ) { xMin -= 360.; }
		double yMin = (File[6] == 'S' ? -1. : 1.) * File.Right(8).asInt(); yMin = !bDown ? -yMin : yMin - System.Get_NY() * System.Get_Cellsize();
		System.Create(System.Get_Cellsize(), xMin, yMin, System.Get_NX(), System.Get_NY());
		break; }

	case 'H': { // 1 / 128th degree...
		System.Create(1. / 128., 0., 0., 128 *  90, 128 * 44);
		double xMin = File.Right(5).asInt(); if( xMin >= 180. ) { xMin -= 360.; }
		double yMin = (File[6] == 'S' ? -1. : 1.) * File.Right(8).asInt(); yMin = !bDown ? -yMin : yMin - System.Get_NY() * System.Get_Cellsize();
		System.Create(System.Get_Cellsize(), xMin, yMin, System.Get_NX(), System.Get_NY());
		break; }

	default: Error_Set(_TL("input file name")); return( false );
	}

	//-----------------------------------------------------
	CSG_File Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_R, true) )
	{
		Error_Set(_TL("opening file"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pGrid = SG_Create_Grid(Parameters("TYPE")->asInt() ? SG_DATATYPE_Float : SG_DATATYPE_Short,
		System.Get_NX(), System.Get_NY(), System.Get_Cellsize(),
		System.Get_XMin() + System.Get_Cellsize() / 2.,
		System.Get_YMin() + System.Get_Cellsize() / 2.
	);

	if( !pGrid )
	{
		Error_Set(_TL("failed to allocate memory for grid"));

		return( false );
	}

	pGrid->Set_Name(Name);
	pGrid->Set_NoData_Value(-999999.);
	pGrid->Get_Projection().Create("+proj=lonlat +units=m +a=3396200.000000 +b=3376200.000000");

	//-----------------------------------------------------
	short *sLine = (short *)SG_Malloc(pGrid->Get_NX() * sizeof(short));

	for(int y=0; y<pGrid->Get_NY() && !Stream.is_EOF() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		int yy = bDown ? pGrid->Get_NY() - 1 - y : y;

		Stream.Read(sLine, pGrid->Get_NX(), sizeof(short));

		if( File[10] == 'G' || File[10] == 'H' )
		{
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				SG_Swap_Bytes(sLine + x, sizeof(short));

				pGrid->Set_Value(x, yy, sLine[x]);
			}
		}
		else
		{
			for(int xa=0, xb=pGrid->Get_NX()/2; xb<pGrid->Get_NX(); xa++, xb++)
			{
				SG_Swap_Bytes(sLine + xa, sizeof(short));
				SG_Swap_Bytes(sLine + xb, sizeof(short));

				pGrid->Set_Value(xa, yy, sLine[xb]);
				pGrid->Set_Value(xb, yy, sLine[xa]);
			}
		}
	}

	SG_Free(sLine);

	//-----------------------------------------------------
	Parameters("GRID")->Set_Value(pGrid);

	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
