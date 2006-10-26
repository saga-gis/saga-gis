
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
//                        Raw.cpp                        //
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
#include "raw.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRaw_Import::CRaw_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Import Binary Raw Data"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Imports grid from binary raw data.\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"			, _TL("Grid"),
		""
	);

	Parameters.Add_FilePath(
		NULL	, "FILE_DATA"		, _TL("Raw Data File"),
		""
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "NX"				, _TL("Cell Count (X)"),
		"",
		PARAMETER_TYPE_Int			, 1
	);

	Parameters.Add_Value(
		NULL	, "NY"				, _TL("Cell Count (Y)"),
		"",
		PARAMETER_TYPE_Int			, 1
	);

	Parameters.Add_Value(
		NULL	, "DXY"				, _TL("Cell Size"),
		"",
		PARAMETER_TYPE_Double		, 1.0
	);

	Parameters.Add_Value(
		NULL	, "XMIN"			, _TL("Left Border (X)"),
		"",
		PARAMETER_TYPE_Double		, 0.0
	);

	Parameters.Add_Value(
		NULL	, "YMIN"			, _TL("Lower Border (Y)"),
		"",
		PARAMETER_TYPE_Double		, 0.0
	);

	Parameters.Add_String(
		NULL	, "UNIT"			, _TL("Unit Name"),
		"",
		" "
	);

	Parameters.Add_Value(
		NULL	, "ZFACTOR"			, _TL("Z Multiplier"),
		"",
		PARAMETER_TYPE_Double		, 1.0
	);

	Parameters.Add_Value(
		NULL	, "NODATA"			, _TL("No Data Value"),
		"",
		PARAMETER_TYPE_Double		, -99999.0
	);

	Parameters.Add_Value(
		NULL	, "DATA_OFFSET"		, _TL("Data Offset (Bytes)"),
		"",
		PARAMETER_TYPE_Int			, 0.0
	);

	Parameters.Add_Value(
		NULL	, "LINE_OFFSET"		, _TL("Line Offset (Bytes)"),
		"",
		PARAMETER_TYPE_Int			, 0.0
	);

	Parameters.Add_Value(
		NULL	, "LINE_ENDSET"		, _TL("Line Endset (Bytes)"),
		"",
		PARAMETER_TYPE_Int			, 0.0
	);

	Parameters.Add_Choice(
		NULL	, "DATA_TYPE"		, _TL("Data Type"),
		"",_TL(
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
		NULL	, "BYTEORDER_BIG"	, _TL("Byte Order"),
		"",
		_TL(
		"Little Endian (Intel)|"
		"Big Endian (Motorola)|")	, 0
	);

	Parameters.Add_Choice(
		NULL	, "TOPDOWN"			, _TL("Line Order"),
		"",
		_TL(
		"Bottom to Top|"
		"Top to Bottom|")			, 0
	);
}

//---------------------------------------------------------
CRaw_Import::~CRaw_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRaw_Import::On_Execute(void)
{
	bool		bDown, bBig;
	int			nx, ny, data_head, line_head, line_tail;
	double		dxy, xmin, ymin, zFactor, zNoData;
	FILE		*Stream;
	TSG_Grid_Type	data_type;
	CSG_String	FileName, Unit;
	CSG_Grid		*pGrid;

	//-----------------------------------------------------
	pGrid		= NULL;

	FileName	= Parameters("FILE_DATA")		->asString();
	nx			= Parameters("NX")				->asInt();
	ny			= Parameters("NY")				->asInt();
	dxy			= Parameters("DXY")				->asDouble();
	xmin		= Parameters("XMIN")			->asDouble();
	ymin		= Parameters("YMIN")			->asDouble();
	data_head	= Parameters("DATA_OFFSET")		->asInt();
	line_head	= Parameters("LINE_OFFSET")		->asInt();
	line_tail	= Parameters("LINE_ENDSET")		->asInt();
	bDown		= Parameters("TOPDOWN")			->asInt() == 1;
	bBig		= Parameters("BYTEORDER_BIG")	->asInt() == 1;
	Unit		= Parameters("UNIT")			->asString();
	zFactor		= Parameters("ZFACTOR")			->asDouble();
	zNoData		= Parameters("NODATA")			->asDouble();

	switch( Parameters("DATA_TYPE")->asInt() )
	{
	default:	data_type	= GRID_TYPE_Undefined;	break;	// not handled
	case 0:		data_type	= GRID_TYPE_Byte;		break;	// 1 Byte Integer (unsigned)
	case 1:		data_type	= GRID_TYPE_Char;		break;	// 1 Byte Integer (signed)
	case 2:		data_type	= GRID_TYPE_Word;		break;	// 2 Byte Integer (unsigned)
	case 3:		data_type	= GRID_TYPE_Short;		break;	// 2 Byte Integer (signed)
	case 4:		data_type	= GRID_TYPE_DWord;		break;	// 4 Byte Integer (unsigned)
	case 5:		data_type	= GRID_TYPE_Int;		break;	// 4 Byte Integer (signed)
	case 6:		data_type	= GRID_TYPE_Float;		break;	// 4 Byte Floating Point
	case 7:		data_type	= GRID_TYPE_Double;		break;	// 8 Byte Floating Point
	}

	//-----------------------------------------------------
	if( data_type != GRID_TYPE_Undefined && (Stream = fopen(FileName, "rb")) != NULL )
	{
		if( (pGrid = Load_Data(Stream, data_type, nx, ny, dxy, xmin, ymin, data_head, line_head, line_tail, bDown, bBig)) != NULL )
		{
			pGrid->Set_Unit			(Unit);
			pGrid->Set_ZFactor		(zFactor);
			pGrid->Set_NoData_Value	(zNoData);
			pGrid->Set_Name			(SG_File_Get_Name(FileName, true));

			Parameters("GRID")->Set_Value(pGrid);
		}

		fclose(Stream);
	}

	//-----------------------------------------------------
	return( pGrid != NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CRaw_Import::Load_Data(FILE *Stream, TSG_Grid_Type data_type, int nx, int ny, double dxy, double xmin, double ymin, int data_head, int line_head, int line_tail, bool bDown, bool bBig)
{
	char	*pLine, *pValue;
	int		x, y, nBytes_Value, nBytes_Line;

	CSG_Grid	*pGrid	= NULL;

	//-----------------------------------------------------
	if( Stream && data_type != GRID_TYPE_Undefined )
	{
		for(x=0; x<data_head && !feof(Stream); x++)
		{
			fgetc(Stream);
		}

		//-------------------------------------------------
		if( !feof(Stream) )
		{
			pGrid			= SG_Create_Grid(data_type, nx, ny, dxy, xmin, ymin);
			nBytes_Value	= GRID_TYPE_SIZES[data_type];
			nBytes_Line		= nBytes_Value * nx;
			pLine			= (char *)SG_Malloc(nBytes_Line);

			//---------------------------------------------
			for(y=0; y<pGrid->Get_NY() && !feof(Stream) && Set_Progress(y, pGrid->Get_NY()); y++)
			{
				for(x=0; x<line_head; x++)
				{
					fgetc(Stream);
				}

				fread(pLine, nBytes_Line, sizeof(char), Stream);

				for(x=0, pValue=pLine; x<pGrid->Get_NX(); x++, pValue+=nBytes_Value)
				{
					if( bBig )
					{
						SG_Swap_Bytes(pValue, nBytes_Value);
					}

					switch( data_type )
					{
					case GRID_TYPE_Byte:	pGrid->Set_Value(x, y, *(unsigned char  *)pValue);	break;	// 1 Byte Integer (unsigned)
					case GRID_TYPE_Char:	pGrid->Set_Value(x, y, *(signed char    *)pValue);	break;	// 1 Byte Integer (signed)
					case GRID_TYPE_Word:	pGrid->Set_Value(x, y, *(unsigned short *)pValue);	break;	// 2 Byte Integer (unsigned)
					case GRID_TYPE_Short:	pGrid->Set_Value(x, y, *(signed short   *)pValue);	break;	// 2 Byte Integer (signed)
					case GRID_TYPE_DWord:	pGrid->Set_Value(x, y, *(unsigned int   *)pValue);	break;	// 4 Byte Integer (unsigned)
					case GRID_TYPE_Int:		pGrid->Set_Value(x, y, *(signed int     *)pValue);	break;	// 4 Byte Integer (signed)
					case GRID_TYPE_Float:	pGrid->Set_Value(x, y, *(float          *)pValue);	break;	// 4 Byte Floating Point
					case GRID_TYPE_Double:	pGrid->Set_Value(x, y, *(double         *)pValue);	break;	// 8 Byte Floating Point
					}
				}

				for(x=0; x<line_tail; x++)
				{
					fgetc(Stream);
				}
			}

			//---------------------------------------------
			SG_Free(pLine);

			if( bDown )
			{
				pGrid->Flip();
			}
		}
	}

	//-----------------------------------------------------
	return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
