
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
#include "esri_arcinfo.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define HDR_NROWS			"NROWS"
#define HDR_NCOLS			"NCOLS"
#define HDR_X_CORNER		"XLLCORNER"
#define HDR_Y_CORNER		"YLLCORNER"
#define HDR_X_CENTER		"XLLCENTER"
#define HDR_Y_CENTER		"YLLCENTER"
#define HDR_CELLSIZE		"CELLSIZE"
#define HDR_NODATA			"NODATA_VALUE"
#define HDR_BYTEORDER		"BYTE_ORDER"
#define HDR_BYTEORDER_HI	"MSB_FIRST"
#define HDR_BYTEORDER_LO	"LSB_FIRST"


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

	Set_Author		(SG_T("(c) 2007 by O.Conrad"));

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
}

//---------------------------------------------------------
CESRI_ArcInfo_Import::~CESRI_ArcInfo_Import(void)
{}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::On_Execute(void)
{
	bool		bResult;
	int			x, y, iy;
	float		Value, *Line;
	FILE		*Stream;
	CSG_String	fName;
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	bResult	= false;
	pGrid	= NULL;

	if( Parameters("FILE")->asString() )
	{
		//-------------------------------------------------
		// Binary...

		if(	SG_File_Cmp_Extension(Parameters("FILE")->asString(), SG_T("flt"))
		||	SG_File_Cmp_Extension(Parameters("FILE")->asString(), SG_T("hdr")) )
		{
			fName	= SG_File_Make_Path(SG_T(""), Parameters("FILE")->asString(), SG_T("hdr"));

			if( (Stream = fopen(fName.b_str(), "r")) != NULL && (pGrid = Read_Header(Stream)) != NULL )
			{
				fclose(Stream);

				fName	= SG_File_Make_Path(SG_T(""), Parameters("FILE")->asString(), SG_T("flt"));

				if( (Stream = fopen(fName.b_str(), "rb")) != NULL )
				{
					Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

					for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && !feof(Stream) && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
					{
						fread(Line, pGrid->Get_NX(), sizeof(float), Stream);

						for(x=0; x<pGrid->Get_NX(); x++)
						{
							pGrid->Set_Value(x, y, Line[x]);
						}
					}

					SG_Free(Line);

					fclose(Stream);
				}
			}
		}


		//-------------------------------------------------
		// ASCII...

		else
		{
			fName	= Parameters("FILE")->asString();

			if( (Stream = fopen(fName.b_str(), "r")) != NULL )
			{
				if( (pGrid = Read_Header(Stream)) != NULL )
				{
					for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && !feof(Stream) && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
					{
						for(x=0; x<pGrid->Get_NX(); x++)
						{
							fscanf(Stream, "%f", &Value);

							pGrid->Set_Value(x, y, Value);
						}
					}
				}

				fclose(Stream);
			}
		}

		//-------------------------------------------------
		if( pGrid )
		{
			pGrid->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));

			Parameters("GRID")->Set_Value(pGrid);

			bResult	= true;
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::Read_Line(FILE *Stream, CSG_String &sLine)
{
	if( Stream && !feof(Stream) )
	{
		char	c;

		sLine.Clear();

		while( !feof(Stream) && (c = getc(Stream)) != 0x0A && c != 0x0D )
		{
			sLine.Append(c);
		}

		sLine.Make_Upper();

		sLine.Replace(SG_T(","), SG_T("."));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::Read_Value(const CSG_String &sKey, CSG_String &sLine, int &Value)
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
bool CESRI_ArcInfo_Import::Read_Value(const CSG_String &sKey, CSG_String &sLine, double &Value)
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
CSG_Grid * CESRI_ArcInfo_Import::Read_Header(FILE *Stream)
{
	bool		bCorner_X, bCorner_Y;
	int			NX, NY;
	double		CellSize, xMin, yMin, NoData	= -9999.0;
	CSG_String	sLine;
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	if( Stream )
	{
		//-------------------------------------------------
		Read_Line(Stream, sLine);

		if( !Read_Value(HDR_NCOLS   , sLine, NX) )
			return( NULL );

		//-------------------------------------------------
		Read_Line(Stream, sLine);

		if( !Read_Value(HDR_NROWS   , sLine, NY) )
			return( NULL );

		//-------------------------------------------------
		Read_Line(Stream, sLine);

		if(	     Read_Value(HDR_X_CORNER, sLine, xMin) )
			bCorner_X	= true;
		else if( Read_Value(HDR_X_CENTER, sLine, xMin) )
			bCorner_X	= false;
		else
			return( NULL );

		//-------------------------------------------------
		Read_Line(Stream, sLine);

		if(	     Read_Value(HDR_Y_CORNER, sLine, yMin) )
			bCorner_Y	= true;
		else if( Read_Value(HDR_Y_CENTER, sLine, yMin) )
			bCorner_Y	= false;
		else
			return( NULL );

		//-------------------------------------------------
		Read_Line(Stream, sLine);

		if( !Read_Value(HDR_CELLSIZE, sLine, CellSize) )
			return( NULL );

		//-------------------------------------------------
		Read_Line(Stream, sLine);

		if( !Read_Value(HDR_NODATA  , sLine, NoData) )
		//	return( NULL );
		{}

		//-------------------------------------------------
		if( bCorner_X )
			xMin	+= CellSize / 2.0;

		if( bCorner_Y )
			yMin	+= CellSize / 2.0;

		//-------------------------------------------------
		if( (pGrid = SG_Create_Grid(SG_DATATYPE_Float, NX, NY, CellSize, xMin, yMin)) != NULL )
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

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

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
CESRI_ArcInfo_Export::~CESRI_ArcInfo_Export(void)
{}

//---------------------------------------------------------
bool CESRI_ArcInfo_Export::On_Execute(void)
{
	bool		bResult, bSwapBytes, bComma;
	int			x, y, iy, Precision;
	float		*Line;
	FILE		*Stream;
	CSG_Grid	*pGrid;
	CSG_String	fName;

	//-----------------------------------------------------
	bResult		= false;

	pGrid		= Parameters("GRID")	->asGrid();
	Precision	= Parameters("PREC")	->asInt();
	bComma		= Parameters("DECSEP")	->asInt() == 1;
	bSwapBytes	= false;	//	bSwapBytes	= Parameters("BYTEORD")	->asInt() == 1;


	//-----------------------------------------------------
	// Binary...

	if( Parameters("FORMAT")->asInt() == 0 )
	{
		fName	= SG_File_Make_Path(SG_T(""), Parameters("FILE")->asString(), SG_T("hdr"));

		if( (Stream = fopen(fName.b_str(), "w")) != NULL )
		{
			if( Write_Header(Stream, pGrid, bComma) )
			{
				fclose(Stream);

				fName	= SG_File_Make_Path(SG_T(""), Parameters("FILE")->asString(), SG_T("flt"));

				if( (Stream = fopen(fName.b_str(), "wb")) != NULL )
				{
					Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

					for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
					{
						for(x=0; x<pGrid->Get_NX(); x++)
						{
							Line[x]	= pGrid->asFloat(x, y);

							if( bSwapBytes )
							{
								SG_Swap_Bytes(Line + x, sizeof(float));
							}
						}

						fwrite(Line, pGrid->Get_NX(), sizeof(float), Stream);
					}

					SG_Free(Line);

					fclose(Stream);

					bResult	= true;
				}
			}
			else
			{
				fclose(Stream);
			}
		}
	}


	//-----------------------------------------------------
	// ASCII...

	else
	{
		fName	= Parameters("FILE")->asString();

		if( (Stream = fopen(fName.b_str(), "w")) != NULL )
		{
			if( Write_Header(Stream, pGrid, bComma) )
			{
				CSG_String	s;

				for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
				{
					for(x=0; x<pGrid->Get_NX(); x++)
					{
						if( Precision < 0 )
						{
							s.Printf(SG_T("%f")		, pGrid->asFloat(x, y));
						}
						else if( Precision == 0 )
						{
							s.Printf(SG_T("%d")		, (int)(0.5 + pGrid->asFloat(x, y)));
						}
						else
						{
							s.Printf(SG_T("%.*f")	, Precision, pGrid->asFloat(x, y));
						}

						if( bComma )
							s.Replace(SG_T("."), SG_T(","));
						else
							s.Replace(SG_T(","), SG_T("."));

						fprintf(Stream, "%s ", s.b_str());
					}

					fprintf(Stream, "\n");
				}

				bResult	= true;
			}

			fclose(Stream);
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Export::Write_Header(FILE *Stream, CSG_Grid *pGrid, bool bComma)
{
	if( Stream && pGrid && pGrid->is_Valid() )
	{
		CSG_String	s;

		s	+= CSG_String::Format(SG_T("%s %d\n")		, HDR_NCOLS		, pGrid->Get_NX());
		s	+= CSG_String::Format(SG_T("%s %d\n")		, HDR_NROWS		, pGrid->Get_NY());

		if( Parameters("GEOREF")->asInt() == 0 )
		{
			s	+= CSG_String::Format(SG_T("%s %f\n")	, HDR_X_CORNER	, pGrid->Get_XMin() - 0.5 * pGrid->Get_Cellsize());
			s	+= CSG_String::Format(SG_T("%s %f\n")	, HDR_Y_CORNER	, pGrid->Get_YMin() - 0.5 * pGrid->Get_Cellsize());
		}
		else
		{
			s	+= CSG_String::Format(SG_T("%s %f\n")	, HDR_X_CENTER	, pGrid->Get_XMin());
			s	+= CSG_String::Format(SG_T("%s %f\n")	, HDR_Y_CENTER	, pGrid->Get_YMin());
		}

		s	+= CSG_String::Format(SG_T("%s %f\n")		, HDR_CELLSIZE	, (float)pGrid->Get_Cellsize());
		s	+= CSG_String::Format(SG_T("%s %f\n")		, HDR_NODATA	, (float)pGrid->Get_NoData_Value());

		if( Parameters("FORMAT")->asInt() == 0 )	// binary
		{	if( Parameters("BYTEORD") )
			s	+= CSG_String::Format(SG_T("%s %s\n")	, HDR_BYTEORDER	, Parameters("BYTEORD")->asInt() == 1 ? HDR_BYTEORDER_LO : HDR_BYTEORDER_HI);
		}

		if( bComma )
			s.Replace(SG_T("."), SG_T(","));
		else
			s.Replace(SG_T(","), SG_T("."));

		fprintf(Stream, s.b_str());

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
