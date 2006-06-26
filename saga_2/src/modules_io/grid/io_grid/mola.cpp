
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
#include "mola.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMOLA_Import::CMOLA_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import MOLA Grid (MEGDR)"));

	Set_Author		(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TL(
		"Import Mars Orbit Laser Altimeter (MOLA) grids of the Mars Global Surveyor (MGS) Mission "
		"(Topographic maps, Mission Experiment Gridded Data Records - MEGDRs). "
		"Find more information and obtain free data from "
		"<a target=\"_blank\" href=\"http://pds-geosciences.wustl.edu/missions/mgs/mola.html\">"
		"Mars Global Surveyor: MOLA (NASA)</a>\n\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		""
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		"",
		_TL("MOLA Grids (*.img)|*.img|All Files|*.*")
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"	, _TL("Grid Type"),
		"",
		_TL("2 byte integer|4 byte floating point|"), 1
	);

	Parameters.Add_Choice(
		NULL	, "ORIENT"	, _TL("Orientation"),
		"",
		_TL("normal|down under|"), 1
	);
}

//---------------------------------------------------------
CMOLA_Import::~CMOLA_Import(void)
{}

//---------------------------------------------------------
bool CMOLA_Import::On_Execute(void)
{
	bool		bDown;
	int			xa, xb, y, yy, NX, NY;
	short		*sLine;
	double		D, xMin, yMin;
	FILE		*Stream;
	TGrid_Type	Type;
	CGrid		*pGrid;
	CAPI_String	fName, sName;

	//-----------------------------------------------------
	pGrid	= NULL;

	switch( Parameters("TYPE")->asInt() )
	{
	case 0:				Type	= GRID_TYPE_Short;	break;
	case 1: default:	Type	= GRID_TYPE_Float;	break;
	}

	bDown	= Parameters("ORIENT")->asInt() == 1;

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

	fName	= API_Extract_File_Name(Parameters("FILE")->asString(), false);
	fName.Make_Upper();

	if( fName.Length() < 12 )
	{
		return( false );
	}

	//-----------------------------------------------------
	switch( fName[3] )
	{
	default:
		return( false );

	case 'A':
		sName.Printf("MOLA: Areoid v%c"		, fName[11]);
		break;

	case 'C':
		sName.Printf("MOLA: Counts v%c"		, fName[11]);
		break;

	case 'R':
		sName.Printf("MOLA: Radius v%c"		, fName[11]);
		break;

	case 'T':
		sName.Printf("MOLA: Topography v%c"	, fName[11]);
		break;
	}

	//-----------------------------------------------------
	switch( fName[10] )
	{
	default:
		return( false );

	case 'C':	// 1/4th degree...
		D		= 1.0 /   4.0;
		NX		=   4 * 360;
		NY		=   4 * 180;
		yMin	= - 90.0;
		xMin	= -180.0;
		break;

	case 'D':	// 1/8th degree...
		D		= 1.0 /   8.0;
		NX		=   8 * 360;
		NY		=   8 * 180;
		yMin	= - 90.0;
		xMin	= -180.0;
		break;

	case 'E':	// 1/16th degree...
		D		= 1.0 /  16.0;
		NX		=  16 * 360;
		NY		=  16 * 180;
		yMin	= - 90.0;
		xMin	= -180.0;
		break;

	case 'F':	// 1/32th degree...
		D		= 1.0 /  32.0;
		NX		=  32 * 360;
		NY		=  32 * 180;
		yMin	= - 90.0;
		xMin	= -180.0;
		break;

	case 'G':	// 1/64th degree...
		D		= 1.0 /  64.0;
		NX		=  64 * 180;
		NY		=  64 *  90;
		yMin	= (fName[6] == 'S' ? -1.0 :  1.0) * atoi(fName.c_str() + 4);
		yMin	= yMin - NY * D;
		xMin	= atoi(fName.c_str() + 7);
		if( xMin >= 180.0 )
		{
			xMin	-= 360.0;
		}
		break;

	case 'H':	// 1/128th degree...
		D		= 1.0 / 128.0;
		NX		= 128 *  90;
		NY		= 128 *  44;
		yMin	= (fName[6] == 'S' ? -1.0 :  1.0) * atoi(fName.c_str() + 4);
		yMin	= yMin - NY * D;
		xMin	= atoi(fName.c_str() + 7);
		if( xMin >= 180.0 )
		{
			xMin	-= 360.0;
		}
		break;
	}

	//-----------------------------------------------------
	if( (Stream = fopen(Parameters("FILE")->asString(), "rb")) != NULL )
	{
		if( (pGrid = API_Create_Grid(Type, NX, NY, D, xMin + D / 2.0, yMin + D / 2.0)) != NULL )
		{
			pGrid->Set_Name(sName);
			pGrid->Set_NoData_Value(-999999);

			//---------------------------------------------
			sLine	= (short *)API_Malloc(NX * sizeof(short));

			for(y=0; y<NY && !feof(Stream) && Set_Progress(y, NY); y++)
			{
				yy	= bDown ? NY - 1 - y : y;

				fread(sLine, NX, sizeof(short), Stream);

				if( fName[10] == 'G' || fName[10] == 'H' )
				{
					for(xa=0; xa<NX; xa++)
					{
						API_Swap_Bytes(sLine + xa, sizeof(short));

						pGrid->Set_Value(xa, yy, sLine[xa]);
					}
				}
				else
				{
					for(xa=0, xb=NX/2; xb<NX; xa++, xb++)
					{
						API_Swap_Bytes(sLine + xa, sizeof(short));
						API_Swap_Bytes(sLine + xb, sizeof(short));

						pGrid->Set_Value(xa, yy, sLine[xb]);
						pGrid->Set_Value(xb, yy, sLine[xa]);
					}
				}
			}

			//---------------------------------------------
			API_Free(sLine);

			Parameters("GRID")->Set_Value(pGrid);

			DataObject_Add		(pGrid);
			DataObject_Update	(pGrid, -8200.0, 21200.0);
		}

		fclose(Stream);
	}

	return( pGrid != NULL );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
