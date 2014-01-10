/**********************************************************
 * Version $Id: mola.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
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
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		_TL("MOLA Grids (*.img)|*.img|All Files|*.*")
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"	, _TL("Grid Type"),
		_TL(""),
		_TL("2 byte integer|4 byte floating point|"), 1
	);

	Parameters.Add_Choice(
		NULL	, "ORIENT"	, _TL("Orientation"),
		_TL(""),
		_TL("normal|down under|"), 1
	);
}

//---------------------------------------------------------
CMOLA_Import::~CMOLA_Import(void)
{}

//---------------------------------------------------------
bool CMOLA_Import::On_Execute(void)
{
	bool			bDown;
	int				xa, xb, y, yy, NX, NY;
	short			*sLine;
	double			D, xMin, yMin;
	CSG_File		Stream;
	TSG_Data_Type	Type;
	CSG_Grid		*pGrid;
	CSG_String		fName, sName;

	//-----------------------------------------------------
	pGrid	= NULL;

	switch( Parameters("TYPE")->asInt() )
	{
	case 0:				Type	= SG_DATATYPE_Short;	break;
	case 1: default:	Type	= SG_DATATYPE_Float;	break;
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

	fName	= SG_File_Get_Name(Parameters("FILE")->asString(), false);
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
		sName.Printf(SG_T("MOLA: Areoid v%c")		, fName[11]);
		break;

	case 'C':
		sName.Printf(SG_T("MOLA: Counts v%c")		, fName[11]);
		break;

	case 'R':
		sName.Printf(SG_T("MOLA: Radius v%c")		, fName[11]);
		break;

	case 'T':
		sName.Printf(SG_T("MOLA: Topography v%c")	, fName[11]);
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
		yMin	= (fName[6] == 'S' ? -1.0 :  1.0) * fName.Right(8).asInt();
		yMin	= bDown ? yMin - NY * D : -yMin;
		xMin	= fName.Right(5).asInt();
		if( xMin >= 180.0 )
		{
			xMin	-= 360.0;
		}
		break;

	case 'H':	// 1/128th degree...
		D		= 1.0 / 128.0;
		NX		= 128 *  90;
		NY		= 128 *  44;
		yMin	= (fName[6] == 'S' ? -1.0 :  1.0) * fName.Right(8).asInt();
		yMin	= bDown ? yMin - NY * D : -yMin;
		xMin	= fName.Right(5).asInt();
		if( xMin >= 180.0 )
		{
			xMin	-= 360.0;
		}
		break;
	}

	//-----------------------------------------------------
	if( Stream.Open(Parameters("FILE")->asString(), SG_FILE_R, true) )
	{
		if( (pGrid = SG_Create_Grid(Type, NX, NY, D, xMin + D / 2.0, yMin + D / 2.0)) != NULL )
		{
			pGrid->Set_Name(sName);
			pGrid->Set_NoData_Value(-999999);
			pGrid->Get_Projection().Create(SG_T("+proj=lonlat +units=m +a=3396200.000000 +b=3376200.000000"), SG_PROJ_FMT_Proj4);

			//---------------------------------------------
			sLine	= (short *)SG_Malloc(NX * sizeof(short));

			for(y=0; y<NY && !Stream.is_EOF() && Set_Progress(y, NY); y++)
			{
				yy	= bDown ? NY - 1 - y : y;

				Stream.Read(sLine, NX, sizeof(short));

				if( fName[10] == 'G' || fName[10] == 'H' )
				{
					for(xa=0; xa<NX; xa++)
					{
						SG_Swap_Bytes(sLine + xa, sizeof(short));

						pGrid->Set_Value(xa, yy, sLine[xa]);
					}
				}
				else
				{
					for(xa=0, xb=NX/2; xb<NX; xa++, xb++)
					{
						SG_Swap_Bytes(sLine + xa, sizeof(short));
						SG_Swap_Bytes(sLine + xb, sizeof(short));

						pGrid->Set_Value(xa, yy, sLine[xb]);
						pGrid->Set_Value(xb, yy, sLine[xa]);
					}
				}
			}

			//---------------------------------------------
			SG_Free(sLine);

			Parameters("GRID")->Set_Value(pGrid);
		}
	}

	return( pGrid != NULL );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
