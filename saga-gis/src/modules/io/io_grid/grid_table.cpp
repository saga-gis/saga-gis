/**********************************************************
 * Version $Id: grid_table.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                    Grid_Table.cpp                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "grid_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Table_Import::CGrid_Table_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Import Grid from Table"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"Imports grid from from table.\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"			, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE_DATA"		, _TL("Table"),
		_TL("")
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "DXY"				, _TL("Cell Size"),
		_TL(""),
		PARAMETER_TYPE_Double		, 1.0
	);

	Parameters.Add_Value(
		NULL	, "XMIN"			, _TL("Left Border (X)"),
		_TL(""),
		PARAMETER_TYPE_Double		, 0.0
	);

	Parameters.Add_Value(
		NULL	, "YMIN"			, _TL("Lower Border (Y)"),
		_TL(""),
		PARAMETER_TYPE_Double		, 0.0
	);

	Parameters.Add_String(
		NULL	, "UNIT"			, _TL("Unit Name"),
		_TL(""),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "ZFACTOR"			, _TL("Z Multiplier"),
		_TL(""),
		PARAMETER_TYPE_Double		, 1.0
	);

	Parameters.Add_Value(
		NULL	, "NODATA"			, _TL("No Data Value"),
		_TL(""),
		PARAMETER_TYPE_Double		, -99999.0
	);

	Parameters.Add_Choice(
		NULL	, "DATA_TYPE"		, _TL("Data Type"),
		_TL(""), _TW(
		"1 Byte Integer (unsigned)|"
		"1 Byte Integer (signed)|"
		"2 Byte Integer (unsigned)|"
		"2 Byte Integer (signed)|"
		"4 Byte Integer (unsigned)|"
		"4 Byte Integer (signed)|"
		"4 Byte Floating Point|"
		"8 Byte Floating Point|")
	);

	Parameters.Add_Choice(
		NULL	, "TOPDOWN"			, _TL("Line Order"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Bottom to Top"),
			_TL("Top to Bottom")
		), 0
	);
}

//---------------------------------------------------------
CGrid_Table_Import::~CGrid_Table_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Table_Import::On_Execute(void)
{
	bool			bDown;
	int				x, y, nx, ny;
	double			dxy, xmin, ymin, zFactor, zNoData;
	TSG_Data_Type		data_type;
	CSG_String		FileName, Unit;
	CSG_Grid			*pGrid;
	CSG_Table			Table;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	FileName	= Parameters("FILE_DATA")		->asString();
	dxy			= Parameters("DXY")				->asDouble();
	xmin		= Parameters("XMIN")			->asDouble();
	ymin		= Parameters("YMIN")			->asDouble();
	bDown		= Parameters("TOPDOWN")			->asInt() == 1;
	Unit		= Parameters("UNIT")			->asString();
	zFactor		= Parameters("ZFACTOR")			->asDouble();
	zNoData		= Parameters("NODATA")			->asDouble();

	switch( Parameters("DATA_TYPE")->asInt() )
	{
	default:	data_type	= SG_DATATYPE_Undefined;	break;	// not handled
	case 0:		data_type	= SG_DATATYPE_Byte;			break;	// 1 Byte Integer (unsigned)
	case 1:		data_type	= SG_DATATYPE_Char;			break;	// 1 Byte Integer (signed)
	case 2:		data_type	= SG_DATATYPE_Word;			break;	// 2 Byte Integer (unsigned)
	case 3:		data_type	= SG_DATATYPE_Short;		break;	// 2 Byte Integer (signed)
	case 4:		data_type	= SG_DATATYPE_DWord;		break;	// 4 Byte Integer (unsigned)
	case 5:		data_type	= SG_DATATYPE_Int;			break;	// 4 Byte Integer (signed)
	case 6:		data_type	= SG_DATATYPE_Float;		break;	// 4 Byte Floating Point
	case 7:		data_type	= SG_DATATYPE_Double;		break;	// 8 Byte Floating Point
	}

	//-----------------------------------------------------
	if( Table.Create(FileName) && (nx = Table.Get_Field_Count()) > 0 && (ny = Table.Get_Record_Count()) > 0 )
	{
		pGrid	= SG_Create_Grid(data_type, nx, ny, dxy, xmin, ymin);

		for(y=0; y<ny && Set_Progress(y, ny); y++)
		{
			pRecord	= Table.Get_Record(bDown ? ny - 1 - y : y);

			for(x=0; x<nx; x++)
			{
				pGrid->Set_Value(x, y, pRecord->asDouble(x));
			}
		}

		pGrid->Set_Unit			(Unit);
		pGrid->Set_Scaling		(zFactor);
		pGrid->Set_NoData_Value	(zNoData);
		pGrid->Set_Name			(SG_File_Get_Name(FileName, false));

		Parameters("GRID")->Set_Value(pGrid);

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
