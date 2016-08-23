/**********************************************************
 * Version $Id: esri_arcinfo.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                   ESRI_ArcInfo.cpp                    //
//                                                       //
//                 Copyright (C) 2007 by                 //
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
#include "esri_arcinfo.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define HDR_NROWS			SG_T("NROWS")
#define HDR_NCOLS			SG_T("NCOLS")
#define HDR_X_CORNER		SG_T("XLLCORNER")
#define HDR_Y_CORNER		SG_T("YLLCORNER")
#define HDR_X_CENTER		SG_T("XLLCENTER")
#define HDR_Y_CENTER		SG_T("YLLCENTER")
#define HDR_CELLSIZE		SG_T("CELLSIZE")
#define HDR_NODATA			SG_T("NODATA_VALUE")
#define HDR_BYTEORDER		SG_T("BYTE_ORDER")
#define HDR_BYTEORDER_HI	SG_T("MSB_FIRST")
#define HDR_BYTEORDER_LO	SG_T("LSB_FIRST")


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CESRI_ArcInfo_Import::CESRI_ArcInfo_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import ESRI Arc/Info Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2007"));

	Set_Description	(_TW(
		"Import grid from ESRI's Arc/Info grid format.")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),

		CSG_String::Format(
			SG_T("%s|*.asc;*.flt|%s|*.asc|%s|*.flt|%s|*.*"),
			_TL("ESRI Arc/Info Grids"),
			_TL("ESRI Arc/Info ASCII Grids (*.asc)"),
			_TL("ESRI Arc/Info Binary Grids (*.flt)"),
			_TL("All Files")
		)
	);

	CSG_Parameter	*pNode;

	pNode = Parameters.Add_Node(
		NULL	, "NODE_ASCII"	, _TL("ASCII Grid Options"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode	, "GRID_TYPE"	, _TL("Target Grid Type"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Integer (2 byte)"),
			_TL("Integer (4 byte)"),
			_TL("Floating Point (4 byte)"),
			_TL("Floating Point (8 byte)")
		), 2
	);

	Parameters.Add_Choice(
		pNode	, "NODATA"	, _TL("NoData Value"),
		_TL("Choose whether the input file's NoData value or a user specified NoData value is written"),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Input File's NoData Value"),
			_TL("User Defined NoData Value")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "NODATA_VAL"	, _TL("User Defined NoData Value"),
		_TL(""),
		PARAMETER_TYPE_Double	, -99999.0
	);
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::On_Execute(void)
{
	CSG_File		Stream;
	CSG_String		fName;
	CSG_Grid		*pGrid;
	TSG_Data_Type	Datatype;
	int				iNoData;
	double			dNoData;

	//-----------------------------------------------------
	pGrid		= NULL;
	fName		= Parameters("FILE")->asString();
	iNoData		= Parameters("NODATA")->asInt();
	dNoData		= Parameters("NODATA_VAL")->asDouble();

	switch( Parameters("GRID_TYPE")->asInt() )
	{
	case 0:		Datatype = SG_DATATYPE_Short;	break;
	case 1:		Datatype = SG_DATATYPE_Int;		break;
	case 2:
	default:	Datatype = SG_DATATYPE_Float;	break;
	case 3:		Datatype = SG_DATATYPE_Double;	break;
	}


	//-------------------------------------------------
	// Binary...

	if( Stream.Open(SG_File_Make_Path(SG_T(""), fName, SG_T("hdr")), SG_FILE_R, false) && (pGrid = Read_Header(Stream)) != NULL )
	{
		if( Stream.Open(SG_File_Make_Path(SG_T(""), fName, SG_T("flt")), SG_FILE_R, true) )
		{
			float	*Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

			for(int iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && !Stream.is_EOF() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
			{
				Stream.Read(Line, sizeof(float), pGrid->Get_NX());

				for(int x=0; x<pGrid->Get_NX(); x++)
				{
					pGrid->Set_Value(x, y, Line[x]);
				}
			}

			SG_Free(Line);
		}
		else
		{
			delete(pGrid);

			return( false );
		}
	}

	//-------------------------------------------------
	// ASCII...

	else if( Stream.Open(fName, SG_FILE_R, false) && (pGrid = Read_Header(Stream, Datatype)) != NULL )
	{
		double	dValue;

		for(int iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && !Stream.is_EOF() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
		{
			for(int x=0; x<pGrid->Get_NX(); x++)
			{
				dValue	= Read_Value(Stream);

				if( iNoData == 1 && dValue == pGrid->Get_NoData_Value() )
					dValue	= dNoData;

				pGrid->Set_Value(x, y, dValue);
			}
		}

		if( iNoData == 1 )
		{
			pGrid->Set_NoData_Value(dNoData);
		}
	}

	//-------------------------------------------------
	else
	{
		return( false );
	}

	//-------------------------------------------------
	pGrid->Set_Name(SG_File_Get_Name(fName, false));

	pGrid->Get_Projection().Load(SG_File_Make_Path(NULL, fName, SG_T("prj")));

	Parameters("GRID")->Set_Value(pGrid);

	return( true );
}

//---------------------------------------------------------
inline bool SG_is_Numeric(int Character)
{
	switch( Character )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
	case '+':
	case '.':
	case ',':
	case 'e':
	case 'E':
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CESRI_ArcInfo_Import::Read_Value(CSG_File &Stream)
{
	int			c;
	CSG_String	s;

	while( !Stream.is_EOF() && !SG_is_Numeric(c = Stream.Read_Char()) );	// ignore leading white space...

	if( !Stream.is_EOF() && SG_is_Numeric(c) )
	{
		do
		{
			if( c == ',' )
			{
				c	= '.';
			}

			s	+= (char)c;
		}
		while( !Stream.is_EOF() && SG_is_Numeric(c = Stream.Read_Char()) );
	}

	return( s.asDouble() );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::Read_Header_Line(CSG_File &Stream, CSG_String &sLine)
{
	int		c;

	sLine.Clear();

	while( !Stream.is_EOF() && (c = Stream.Read_Char()) != 0x0A )
	{
		if( c != 0x0D )
		{
			sLine	+= (char)c;
		}
	}

	sLine.Make_Upper();
	sLine.Replace(SG_T(","), SG_T("."));

	return( sLine.Length() > 0 );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::Read_Header_Value(const CSG_String &sKey, CSG_String &sLine, int &Value)
{
	sLine.Make_Upper();

	if( sLine.Contains(sKey) )
	{
		CSG_String	sValue(sLine.c_str() + sKey.Length());

		return( sValue.asInt(Value) );
	}

	return( false );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::Read_Header_Value(const CSG_String &sKey, CSG_String &sLine, double &Value)
{
	sLine.Make_Upper();

	if( sLine.Contains(sKey) )
	{
		CSG_String	sValue(sLine.c_str() + sKey.Length());

		return( sValue.asDouble(Value) );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Grid * CESRI_ArcInfo_Import::Read_Header(CSG_File &Stream, TSG_Data_Type Datatype)
{
	bool		bCorner_X, bCorner_Y;
	int			NX, NY;
	double		CellSize, xMin, yMin, NoData	= -9999.0;
	CSG_String	sLine;
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	if( !Stream.is_EOF() )
	{
		//-------------------------------------------------
		Read_Header_Line(Stream, sLine);

		if( !Read_Header_Value(HDR_NCOLS   , sLine, NX) )
			return( NULL );

		//-------------------------------------------------
		Read_Header_Line(Stream, sLine);

		if( !Read_Header_Value(HDR_NROWS   , sLine, NY) )
			return( NULL );

		//-------------------------------------------------
		Read_Header_Line(Stream, sLine);

		if(	     Read_Header_Value(HDR_X_CORNER, sLine, xMin) )
			bCorner_X	= true;
		else if( Read_Header_Value(HDR_X_CENTER, sLine, xMin) )
			bCorner_X	= false;
		else
			return( NULL );

		//-------------------------------------------------
		Read_Header_Line(Stream, sLine);

		if(	     Read_Header_Value(HDR_Y_CORNER, sLine, yMin) )
			bCorner_Y	= true;
		else if( Read_Header_Value(HDR_Y_CENTER, sLine, yMin) )
			bCorner_Y	= false;
		else
			return( NULL );

		//-------------------------------------------------
		Read_Header_Line(Stream, sLine);

		if( !Read_Header_Value(HDR_CELLSIZE, sLine, CellSize) )
			return( NULL );

		//-------------------------------------------------
		Read_Header_Line(Stream, sLine);

		if( !Read_Header_Value(HDR_NODATA  , sLine, NoData) )
		//	return( NULL );
		{}

		//-------------------------------------------------
		if( bCorner_X )
			xMin	+= CellSize / 2.0;

		if( bCorner_Y )
			yMin	+= CellSize / 2.0;

		//-------------------------------------------------
		if( (pGrid = SG_Create_Grid(Datatype, NX, NY, CellSize, xMin, yMin)) != NULL )
		{
			pGrid->Set_NoData_Value(NoData);

			return( pGrid );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CESRI_ArcInfo_Export::CESRI_ArcInfo_Export(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Export ESRI Arc/Info Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2007"));

	Set_Description	(_TW(
		"Export grid to ESRI's Arc/Info grid format.")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),

		CSG_String::Format(
			SG_T("%s|*.asc;*.flt|%s|*.asc|%s|*.flt|%s|*.*"),
			_TL("ESRI Arc/Info Grids"),
			_TL("ESRI Arc/Info ASCII Grids (*.asc)"),
			_TL("ESRI Arc/Info Binary Grids (*.flt)"),
			_TL("All Files")
		), NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "FORMAT"	, _TL("Format"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("binary"),
			_TL("ASCII")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "GEOREF"	, _TL("Geo-Reference"),
		_TL("The grids geo-reference must be related either to the center or the corner of its lower left grid cell."),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("corner"),
			_TL("center")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "PREC"	, _TL("ASCII Precision"),
		_TL("Number of decimals when writing floating point values in ASCII format."),
		PARAMETER_TYPE_Int	, 4, -1, true
	);

	Parameters.Add_Choice(
		NULL	, "DECSEP"	, _TL("ASCII Decimal Separator"),
		_TL("Applies also to the binary format header file."),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("point (.)"),
			_TL("comma (,)")
		), 0
	);

//	Parameters.Add_Choice(
//		NULL	, "BYTEORD"	, _TL("Binary Byte Order"),
//		_TL("Byte order when writing floating point values in binary format"),
//
//		CSG_String::Format(SG_T("%s|%s|"),
//			_TL("most significant first"),
//			_TL("least significant first")
//		), 0
//	);
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Export::On_Execute(void)
{
	bool		bResult, bSwapBytes, bComma;
	int			x, y, iy, Precision;
	CSG_File	Stream;
	CSG_Grid	*pGrid;
	CSG_String	fName;

	//-----------------------------------------------------
	bResult		= false;

	pGrid		= Parameters("GRID")	->asGrid();
	fName		= Parameters("FILE")	->asString();
	Precision	= Parameters("PREC")	->asInt();
	bComma		= Parameters("DECSEP")	->asInt() == 1;
	bSwapBytes	= false;	//	bSwapBytes	= Parameters("BYTEORD")	->asInt() == 1;


	//-----------------------------------------------------
	// Binary...

	if( Parameters("FORMAT")->asInt() == 0 )
	{
		if( Stream.Open(SG_File_Make_Path(SG_T(""), fName, SG_T("hdr")), SG_FILE_W, false)
		&&	Write_Header(Stream, pGrid, bComma)
		&&	Stream.Open(SG_File_Make_Path(SG_T(""), fName, SG_T("flt")), SG_FILE_W, true) )
		{
			float	*Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

			for(int iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
			{
				for(int x=0; x<pGrid->Get_NX(); x++)
				{
					Line[x]	= pGrid->asFloat(x, y);

					if( bSwapBytes )
					{
						SG_Swap_Bytes(Line + x, sizeof(float));
					}
				}

				Stream.Write(Line, sizeof(float), pGrid->Get_NX());
			}

			SG_Free(Line);

			pGrid->Get_Projection().Save(SG_File_Make_Path(NULL, fName, SG_T("prj")));

			return( true );
		}
	}


	//-----------------------------------------------------
	// ASCII...

	else if( Stream.Open(fName, SG_FILE_W, false) && Write_Header(Stream, pGrid, bComma) )
	{
		for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
		{
			for(x=0; x<pGrid->Get_NX(); x++)
			{
				if( x > 0 )
				{
					fputs(" ",Stream.Get_Stream());
				}

				fputs(Write_Value(pGrid->asDouble(x, y), Precision, bComma).b_str(),Stream.Get_Stream());
			}

			fputs("\n", Stream.Get_Stream());
		}

		pGrid->Get_Projection().Save(SG_File_Make_Path(NULL, fName, SG_T("prj")));

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
inline CSG_String CESRI_ArcInfo_Export::Write_Value(double Value, int Precision, bool bComma)
{
	CSG_String	s;

	if( Precision < 0 )
	{
		s.Printf(SG_T("%f")  , Value);
	}
	else if( Precision > 0 )
	{
		s.Printf(SG_T("%.*f"), Precision, Value);
	}
	else
	{
		s.Printf(SG_T("%d")  , (int)(Value > 0.0 ? Value + 0.5 : Value - 0.5));
	}

	if( bComma )
	{
		s.Replace(SG_T("."), SG_T(","));
	}
	else
	{
		s.Replace(SG_T(","), SG_T("."));
	}

	return( s );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Export::Write_Header(CSG_File &Stream, CSG_Grid *pGrid, bool bComma)
{
	if( Stream.is_Open() && pGrid && pGrid->is_Valid() )
	{
		CSG_String	s;

		s	+= CSG_String::Format(SG_T("%s %d\n")		, HDR_NCOLS		, pGrid->Get_NX());
		s	+= CSG_String::Format(SG_T("%s %d\n")		, HDR_NROWS		, pGrid->Get_NY());

		if( Parameters("GEOREF")->asInt() == 0 )
		{
			s	+= CSG_String::Format(SG_T("%s %s\n")	, HDR_X_CORNER	, Write_Value(pGrid->Get_XMin() - 0.5 * pGrid->Get_Cellsize(), 10, bComma).c_str());
			s	+= CSG_String::Format(SG_T("%s %s\n")	, HDR_Y_CORNER	, Write_Value(pGrid->Get_YMin() - 0.5 * pGrid->Get_Cellsize(), 10, bComma).c_str());
		}
		else
		{
			s	+= CSG_String::Format(SG_T("%s %s\n")	, HDR_X_CENTER	, Write_Value(pGrid->Get_XMin(), 10, bComma).c_str());
			s	+= CSG_String::Format(SG_T("%s %s\n")	, HDR_Y_CENTER	, Write_Value(pGrid->Get_YMin(), 10, bComma).c_str());
		}

		s	+= CSG_String::Format(SG_T("%s %s\n")		, HDR_CELLSIZE	, Write_Value(pGrid->Get_Cellsize(), -1, bComma).c_str());
		s	+= CSG_String::Format(SG_T("%s %s\n")		, HDR_NODATA	, Write_Value(pGrid->Get_NoData_Value(), Parameters("PREC")->asInt(), bComma).c_str());

		if( Parameters("FORMAT")->asInt() == 0 )	// binary
		{	if( Parameters("BYTEORD") )
			s	+= CSG_String::Format(SG_T("%s %s\n")	, HDR_BYTEORDER	, Parameters("BYTEORD")->asInt() == 1 ? HDR_BYTEORDER_LO : HDR_BYTEORDER_HI);
		}

		fputs(s.b_str(),Stream.Get_Stream());

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
