
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
#include "raw.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CRaw_Import::CRaw_Import(void)
{
	Set_Name		(_TL("Import Binary Raw Data"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Imports grid from binary raw data."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("", "GRID", _TL("Grid"), _TL(""));
	Parameters.Add_FilePath   ("", "FILE", _TL("File"), _TL(""));

	//-----------------------------------------------------
	Parameters.Add_Int        ("", "NX"         , _TL("Number of Columns"), _TL(""), 1, 1, true);
	Parameters.Add_Int        ("", "NY"         , _TL("Number of Rows"   ), _TL(""), 1, 1, true);
	Parameters.Add_Double     ("", "CELLSIZE"   , _TL("Cell Size"        ), _TL(""), 1., 0., true);

	//-----------------------------------------------------
	Parameters.Add_Choice     ("", "POS_VECTOR" , _TL("Position Vector"  ), _TL(""),
		CSG_String::Format("%s|%s", _TL("cell's center"), _TL("cell's corner"))
	);

	Parameters.Add_Double     ("POS_VECTOR", "POS_X"     , _TL("X"   ), _TL(""));
	Parameters.Add_Choice     ("POS_X"     , "POS_X_SIDE", _TL("Side"), _TL(""),
		CSG_String::Format("%s|%s", _TL("left"), _TL("right"))
	);

	Parameters.Add_Double     ("POS_VECTOR", "POS_Y"     , _TL("Y"   ), _TL(""));
	Parameters.Add_Choice     ("POS_Y"     , "POS_Y_SIDE", _TL("Side"), _TL(""),
		CSG_String::Format("%s|%s|", _TL("top"), _TL("bottom"))
	);

	//-----------------------------------------------------
	Parameters.Add_Data_Type  ("", "DATA_TYPE"    , _TL("Data Type"    ), _TL(""), SG_DATATYPES_Numeric);
	Parameters.Add_Choice     ("", "BYTEORDER"    , _TL("Byte Order"   ), _TL(""),
		CSG_String::Format("%s|%s|", _TL("Little Endian (Intel)"), _TL("Big Endian (Motorola)")), 0
	);

	Parameters.Add_String     ("", "UNIT"         , _TL("Unit"         ), _TL(""), "");
	Parameters.Add_Double     ("", "ZFACTOR"      , _TL("Z-Scale"      ), _TL(""), 1.);
	Parameters.Add_Double     ("", "NODATA"       , _TL("No Data Value"), _TL(""), -99999.);

	//-----------------------------------------------------
	Parameters.Add_Int        ("", "DATA_OFFSET"  , _TL("Data Offset (Bytes)"  ), _TL(""));
	Parameters.Add_Int        ("", "LINE_OFFSET"  , _TL("Record Offset (Bytes)"), _TL(""));
	Parameters.Add_Int        ("", "LINE_ENDSET"  , _TL("Record Endset (Bytes)"), _TL(""));
	Parameters.Add_Choice     ("", "ORDER"        , _TL("Value Order"          ), _TL(""),
		CSG_String::Format("%s|%s|", _TL("columns by rows"), _TL("rows by columns")), 0
	);

	Parameters.Add_Bool       ("", "TOPDOWN"      , _TL("Invert Row Order"     ), _TL(""),  true);
	Parameters.Add_Bool       ("", "LEFTRIGHT"    , _TL("Invert Column Order"  ), _TL(""), false);

	//-----------------------------------------------------
	Parameters.Add_String     ("", "CRS_PROJ"     , _TL("PROJ Parameters"), _TL(""),     "")->Set_UseInGUI(false);
	Parameters.Add_Int        ("", "CRS_CODE"     , _TL("Code ID"        ), _TL(""),     -1)->Set_UseInGUI(false);
	Parameters.Add_String     ("", "CRS_AUTHORITY", _TL("Code Authority" ), _TL(""), "EPSG")->Set_UseInGUI(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRaw_Import::On_Before_Execution(void)
{
	if( has_GUI() )
	{
		m_pCRS = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 15, true);	// CCRS_Picker

		m_pCRS->Set_Parameter("CRS_EPSG"     , Parameters["CRS_CODE"     ].asInt   ());
		m_pCRS->Set_Parameter("CRS_EPSG_AUTH", Parameters["CRS_AUTHORITY"].asString());
		m_pCRS->Set_Parameter("CRS_PROJ4"    , Parameters["CRS_PROJ"     ].asString());

		Parameters.Add_Parameters("POINTS", "CRS_PICKER", _TL("Coordinate Reference System"), _TL(""))
			->asParameters()->Create(*m_pCRS->Get_Parameters());
	}

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CRaw_Import::On_After_Execution(void)
{
	if( Parameters("CRS_PICKER") )
	{
		Parameters.Del_Parameter("CRS_PICKER");
		SG_Get_Tool_Library_Manager().Delete_Tool(m_pCRS);
		m_pCRS = NULL;
	}

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CRaw_Import::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CRS_PICKER") )
	{
		pParameters->Set_Parameter("CRS_CODE"     , (*pParameter->asParameters())("CRS_EPSG"     )->asInt   ());
		pParameters->Set_Parameter("CRS_AUTHORITY", (*pParameter->asParameters())("CRS_EPSG_AUTH")->asInt   ());
		pParameters->Set_Parameter("CRS_PROJ"     , (*pParameter->asParameters())("CRS_PROJ4"    )->asString());
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRaw_Import::On_Execute(void)
{
	CSG_File Stream;

	if( !Stream.Open(Parameters("FILE")->asString(), SG_FILE_R, true) )
	{
		Error_Fmt("%s [%s]", _TL("could not open file"), Parameters("FILE")->asString());

		return( false );
	}

	Skip(Stream, Parameters("DATA_OFFSET")->asInt());

	//-----------------------------------------------------
	CSG_Grid *pGrid = Get_Grid();

	if( !pGrid )
	{
		Error_Set(_TL("could not create grid"));

		return( false );
	}

	pGrid->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));

	//-----------------------------------------------------
	if( pGrid->Get_Projection().Create(Parameters["CRS_CODE"].asInt(), Parameters["CRS_AUTHORITY"].asString())
	||  pGrid->Get_Projection().Create(Parameters["CRS_PROJ"].asString(), SG_PROJ_FMT_Proj4) )
	{
		Message_Fmt("\n%s: %s\n", _TL("CRS"), pGrid->Get_Projection().Get_Proj4().c_str());
	}

	//-----------------------------------------------------
	bool  bRecIsRow = Parameters("ORDER"    )->asInt() == 0;
	bool bRecInvert = Parameters("TOPDOWN"  )->asBool() == false;
	bool bColInvert = Parameters("LEFTRIGHT")->asBool() == false;

	int nRecords = bRecIsRow ? pGrid->Get_NY() : pGrid->Get_NX();
	int nValues  = bRecIsRow ? pGrid->Get_NX() : pGrid->Get_NY();

	CSG_Array Record(SG_Data_Type_Get_Size(pGrid->Get_Type()), nValues);

	bool bBigOrder = Record.Get_Value_Size() > 1 && Parameters("BYTEORDER")->asInt() == 1;

	int Record_Head	= Parameters("LINE_OFFSET")->asInt();
	int Record_Tail	= Parameters("LINE_ENDSET")->asInt();

	//-----------------------------------------------------
	for(int iRecord=0; !Stream.is_EOF() && iRecord<nRecords && Set_Progress(iRecord, nRecords); iRecord++)
	{
		Skip(Stream, Record_Head);

		Stream.Read(Record.Get_Array(), Record.Get_Value_Size() * Record.Get_Size());

		for(int iValue=0; iValue<nValues; iValue++)
		{
			if( bBigOrder )
			{
				SG_Swap_Bytes(Record.Get_Entry(iValue), (int)Record.Get_Value_Size());
			}

			int y = bRecIsRow ? iRecord : iValue; if( bRecInvert ) y = pGrid->Get_NY() - 1 - y;
			int x = bRecIsRow ? iValue : iRecord; if( bColInvert ) x = pGrid->Get_NX() - 1 - x;

			switch( pGrid->Get_Type() )
			{
			default: break;
			case SG_DATATYPE_Byte  : pGrid->Set_Value(x, y, *(unsigned char  *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_Char  : pGrid->Set_Value(x, y, *(signed char    *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_Word  : pGrid->Set_Value(x, y, *(unsigned short *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_Short : pGrid->Set_Value(x, y, *(signed short   *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_DWord : pGrid->Set_Value(x, y, *(unsigned int   *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_Int   : pGrid->Set_Value(x, y, *(signed int     *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_Float : pGrid->Set_Value(x, y, *(float          *)Record.Get_Entry(iValue)); break;
			case SG_DATATYPE_Double: pGrid->Set_Value(x, y, *(double         *)Record.Get_Entry(iValue)); break;
			}
		}

		Skip(Stream, Record_Tail);
	}

	//-----------------------------------------------------
	pGrid->Set_Unit        (Parameters("UNIT"   )->asString());
	pGrid->Set_Scaling     (Parameters("ZFACTOR")->asDouble());
	pGrid->Set_NoData_Value(Parameters("NODATA" )->asDouble());

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CRaw_Import::Skip(CSG_File &Stream, size_t nBytes)
{
	for(size_t i=0; i<nBytes && !Stream.is_EOF(); i++)
	{
		Stream.Read_Char();
	}

	return( !Stream.is_EOF() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CRaw_Import::Get_Grid(void)
{
	TSG_Data_Type Type = Parameters("DATA_TYPE")->asDataType()->Get_Data_Type();
	int             nx = Parameters("NX")->asInt();
	int             ny = Parameters("NY")->asInt();
	double          cs = Parameters("CELLSIZE")->asDouble();
	bool       bCorner = Parameters("POS_VECTOR")->asInt() == 1;

	//-----------------------------------------------------
	double x = Parameters("POS_X")->asDouble();

	if( Parameters("POS_X_SIDE")->asInt() == 1 ) // right
	{
		x -= cs * nx;
		
		if( bCorner )
		{
			x -= cs / 2.;
		}

	}
	else if( bCorner )
	{
		x += cs / 2.;
	}

	//-----------------------------------------------------
	double y = Parameters("POS_Y")->asDouble();

	if( Parameters("POS_Y_SIDE")->asInt() == 0 ) // top
	{
		y -= cs * ny;

		if( bCorner )
		{
			y -= cs / 2.;
		}
	}
	else if( bCorner )
	{
		y += cs / 2.;
	}

	//-----------------------------------------------------
	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	if( !pGrid )
	{
		Parameters("GRID")->Set_Value(pGrid = SG_Create_Grid());
	}

	pGrid->Create(Type, nx, ny, cs, x, y);

	return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
