
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_grid                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      radolan.cpp                      //
//                                                       //
//                 Copyright (C) 2025 by                 //
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
#include "radolan.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRADOLAN::CRADOLAN(void)
{
	Set_Name		(_TL("Import RADOLAN"));

	Set_Author		("O.Conrad (c) 2025");

	Set_Description	(_TW(
		"Imports binary RADOLAN grids as provided by the German Weather Service (DWD). "
	));

	Add_Reference("https://www.dwd.de/DE/leistungen/radolan/radolan.html", SG_T("DWD.RADOLAN"));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"     , _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath("",
		"FILES"     , _TL("Files"),
		_TL(""),
		CSG_String::Format("%s|*bin|%s|*",
			_TL("RADOLAN binary"),
			_TL("All Files")
		), NULL, false, false, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"COLLECTION", _TL("Multiple Files"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("single grids"),
			_TL("grid collection")
		), 1
	);

	Parameters.Add_Bool("",
		"STATIONS"  , _TL("Station Data"),
		_TL("Include interpolated station data (flag 13 is set, radar measurements not available)."),
		false
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CRADOLAN::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CRADOLAN::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FILES") )
	{
		CSG_Strings Files; pParameters->Set_Enabled("COLLECTION", pParameter->asFilePath()->Get_FilePaths(Files) && Files.Get_Count() > 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRADOLAN::On_Execute(void)
{
	CSG_Strings Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) || Files.Get_Count() == 0 )
	{
		Error_Set(_TL("no files in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table Attributes;

	Attributes.Add_Field("ID"  , SG_DATATYPE_Int   );
	Attributes.Add_Field("JDN" , SG_DATATYPE_Double);
	Attributes.Add_Field("Date", SG_DATATYPE_Date  );
	Attributes.Add_Field("Hour", SG_DATATYPE_Double);

	Attributes.Add_Record();

	CSG_Grids *pGrids = Files.Get_Count() > 1 && Parameters["COLLECTION"].asInt() == 1 ? SG_Create_Grids() : NULL;

	if( pGrids )
	{
		pGrids->Create(Attributes, 1);
	}

	//-----------------------------------------------------
	Parameters("GRIDS")->asGridList()->Del_Items();

	for(int i=0; i<Files.Get_Count() && Set_Progress(i, Files.Get_Count()); i++)
	{
		CSG_Grid *pGrid = Load_File(Files[i], Attributes[0]);

		if( pGrid )
		{
			if( pGrids )
			{
				Attributes[0].Set_Value("ID", 1 + pGrids->Get_Grid_Count());

				pGrids->Add_Grid(Attributes[0], pGrid, true);
			}
			else
			{
				pGrid->Set_Scaling(0.1);

				Parameters("GRIDS")->asGridList()->Add_Item(pGrid);
			}
		}
	}

	//-----------------------------------------------------
	if( pGrids )
	{
		if( pGrids->Get_Grid_Count() < 1 )
		{
			delete(pGrids);
		}
		else
		{
			pGrids->Set_Scaling(0.1); pGrids->Set_Name("RADOLAN");

			Parameters("GRIDS")->asGridList()->Add_Item(pGrids);
		}
	}

	if( Parameters("GRIDS")->asGridList()->Get_Grid_Count() == 0 )
	{
		Error_Set(_TL("no grids have been imported"));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CRADOLAN::Load_File(const CSG_String &File, CSG_Table_Record &Attributes)
{
	CSG_File Stream;

	if( !Stream.Open(File, SG_FILE_R, true) )
	{
		Error_Set(CSG_String::Format("%s: %s", _TL("failed to open file"), File.c_str()));

		return( NULL );
	}

	//-----------------------------------------------------
	int nx, ny;

	if( !Read_Header(Stream, nx, ny) )
	{
		Error_Set(CSG_String::Format("%s: %s", _TL("failed to read header"), File.c_str()));

		return( NULL );
	}

	//-----------------------------------------------------
	// 0_________1_________2_________3____
	// 01234567890123456789012345678901234
	// raa01-rw_10000-yymmddhhmm-dwd---bin

	int year, month, day, hour, minute; CSG_String Name = SG_File_Get_Name(File, true);

	if( !Name.Mid(15, 2).asInt(year  )
	||  !Name.Mid(17, 2).asInt(month )
	||  !Name.Mid(19, 2).asInt(day   )
	||  !Name.Mid(21, 2).asInt(hour  )
	||  !Name.Mid(23, 2).asInt(minute) )
	{
		Error_Set(CSG_String::Format("%s: %s", _TL("invalid file naming convention"), Name.c_str()));

		return( NULL );
	}

//	CSG_DateTime date((CSG_DateTime::TSG_DateTime)day, (CSG_DateTime::Month)(month - 1), year += 2000, (CSG_DateTime::TSG_DateTime)hour, (CSG_DateTime::TSG_DateTime)minute);
	CSG_DateTime date((CSG_DateTime::TSG_DateTime)day, (CSG_DateTime::Month)(month - 1), year += 2000);

	Attributes.Set_Value("JDN" , date.Get_JDN() + (hour + minute / 60.) / 24.);
	Attributes.Set_Value("Date", CSG_String::Format("%04d-%02d-%02d", year, month, day));
	Attributes.Set_Value("Hour", hour + minute / 60.);

	//-----------------------------------------------------
	CSG_Grid *pGrid = SG_Create_Grid(SG_DATATYPE_Short, nx, ny, 1000., -522962., -4658145);

	pGrid->Get_Projection().Create("+proj=stere +lat_0=90 +lat_ts=60 +lon_0=10 +x_0=0 +y_0=0 +R=6370040 +units=m +no_defs +type=crs");

	pGrid->Fmt_Name("RADOLAN %04d-%02d-%02d %02d-%02d", year, month, day, hour, minute);

	Process_Set_Text(pGrid->Get_Name());

	bool bStations = Parameters["STATIONS"].asBool();

	//-----------------------------------------------------
	for(int y=0; y<ny && !Stream.is_EOF() && Process_Get_Okay(); y++)
	{
		for(int x=0; x<nx && !Stream.is_EOF(); x++)
		{
			unsigned short value;

			switch( Read_Value(Stream, value) )
			{
			case  0: // valid data
				pGrid->Set_Value(x, y, value);
				break;

			case  1: // interpolated station data
				if( bStations )
				{
					pGrid->Set_Value(x, y, value);
					break;
				}

			default: // no-data
				pGrid->Set_NoData(x, y);
				break;
			}
		}
	}

	//-----------------------------------------------------
	return( pGrid );
}

//---------------------------------------------------------
bool CRADOLAN::Read_Header(CSG_File &Stream, int &nx, int &ny)
{
	const char ETX = (char)0x03; // End-of-Text

	CSG_String Header; char c;

	while( !Stream.is_EOF() && Stream.Read(&c, 1) && c != ETX && Header.Length() < 2048 )
	{
		Header += c;
	};

	if( Header.Find("GP") < 0 )
	{
		return( false );
	}

	Header = Header.Right(Header.Length() - (Header.Find("GP") + 2));

	return( Header.asInt(nx) && Header.AfterFirst('x').asInt(ny) && nx > 0 && ny > 0 );
}

//---------------------------------------------------------
inline int CRADOLAN::Read_Value(CSG_File &Stream, unsigned short &value)
{
	if( Stream.Read(&value, 2) && (value & 0x2000) == 0 ) // check that bit 14 is not set (no-data flag -> fehlwert)
	{
		if( (value & 0x1000) != 0 ) // bit 13 -> no radar signal, but interpolated station data
		{
			value &= 0x0fff; // clear all flags (bits 13-16)

			return( 1 );
		}

		return( 0 );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
