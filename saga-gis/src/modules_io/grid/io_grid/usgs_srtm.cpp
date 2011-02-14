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
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     USGS_SRTM.cpp                     //
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
#include <string.h>

#include "usgs_srtm.h"


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CUSGS_SRTM_Import::CUSGS_SRTM_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import USGS SRTM Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2004"));

	Set_Description	(_TW(
		"Import grid from USGS SRTM (Shuttle Radar Topography Mission) data.\n"

		"You find data and further information at:\n"
		"  <a target=\"_blank\" href=\"http://dds.cr.usgs.gov/srtm/\">"
		"  http://dds.cr.usgs.gov/srtm/</a>\n"
		"  <a target=\"_blank\" href=\"http://www.jpl.nasa.gov/srtm/\">"
		"  http://www.jpl.nasa.gov/srtm/</a>\n"

		"\nFarr, T.G., M. Kobrick (2000):\n"
		"  'Shuttle Radar Topography Mission produces a wealth of data',\n"
		"  Amer. Geophys. Union Eos, v. 81, p. 583-585\n"

		"\nRosen, P.A., S. Hensley, I.R. Joughin, F.K. Li, S.N. Madsen, E. Rodriguez, R.M. Goldstein (2000):\n"
		"  'Synthetic aperture radar interferometry'\n"
		"  Proc. IEEE, v. 88, p. 333-382\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Files"),
		_TL(""),
		_TL("USGS SRTM Grids (*.hgt)|*.hgt|All Files|*.*"), NULL, false, false, true
	);

	Parameters.Add_Choice(
		NULL	, "RESOLUTION"	, _TL("Resolution"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("1 arc-second"),
			_TL("3 arc-second")
		), 1
	);
}

//---------------------------------------------------------
CUSGS_SRTM_Import::~CUSGS_SRTM_Import(void)
{}

//---------------------------------------------------------
bool CUSGS_SRTM_Import::On_Execute(void)
{
	int						N;
	double					D;
	CSG_Strings				fNames;
	CSG_Grid				*pGrid;
	CSG_Parameter_Grid_List	*pGrids;

	pGrids	= Parameters("GRIDS")->asGridList();
	pGrids	->Del_Items();

	//-----------------------------------------------------
	switch( Parameters("RESOLUTION")->asInt() )
	{
	default:
		return( false );

	case 0:	// 1 arcsec...
		N			= 3601;
		D			= 1.0 / 3600.0;
		break;

	case 1:	// 3 arcsec...
		N			= 1201;
		D			= 3.0 / 3600.0;
		break;
	}

	//-----------------------------------------------------
	if( Parameters("FILE")->asFilePath()->Get_FilePaths(fNames) && fNames.Get_Count() > 0 )
	{
		for(int i=0; i<fNames.Get_Count(); i++)
		{
			if( (pGrid = Load(fNames.Get_String(i), N, D)) != NULL )
			{
				pGrids->Add_Item(pGrid);
			}
		}

		return( pGrids->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Grid * CUSGS_SRTM_Import::Load(CSG_String File, int N, double D)
{
	int			x, y;
	short		*sLine;
	double		xMin, yMin;
	CSG_File	Stream;
	CSG_String	fName;
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	pGrid	= NULL;
	fName	= SG_File_Get_Name(File, false);

	if( fName.Length() >= 7 )
	{
		fName	.Make_Upper();

		Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("Importing"), fName.c_str()));

		yMin	= (fName[0] == 'N' ?  1.0 : -1.0) * fName.Right(6).asInt();
		xMin	= (fName[3] == 'W' ? -1.0 :  1.0) * fName.Right(3).asInt();

		//-------------------------------------------------
		if( Stream.Open(File, SG_FILE_R, true) )
		{
			if( (pGrid = SG_Create_Grid(SG_DATATYPE_Short, N, N, D, xMin, yMin)) != NULL )
			{
				pGrid->Set_Name			(fName);
				pGrid->Get_Projection().Create(SG_T("GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]"));
				pGrid->Set_NoData_Value	(-32768);

				//-----------------------------------------
				sLine	= (short *)SG_Malloc(N * sizeof(short));

				for(y=0; y<N && !Stream.is_EOF() && Set_Progress(y, N); y++)
				{
					Stream.Read(sLine, sizeof(short), N);

					for(x=0; x<N; x++)
					{
						SG_Swap_Bytes(sLine + x, sizeof(short));

						pGrid->Set_Value(x, N - 1 - y, sLine[x]);
					}
				}

				SG_Free(sLine);
			}
		}
	}

	return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
