/**********************************************************
 * Version $Id: wrf.cpp 911 2011-11-11 11:11:11Z oconrad $
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
//                        wrf.cpp                        //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "wrf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

#define	VAL_CATEGORICAL		0
#define VAL_CONTINUOUS		1
#define VAL_ENDIAN_BIG		0
#define VAL_ENDIAN_LITTLE	1
#define VAL_BOTTOM_TOP		0
#define VAL_TOP_BOTTOM		1

//---------------------------------------------------------
CWRF_Index::CWRF_Index(void)
{
	Reset();
}

//---------------------------------------------------------
bool CWRF_Index::Reset(void)
{
	m_PROJECTION	= SG_T("");
		// A character string specifying the projection of the data, which may be
		// either lambert, polar, mercator, regular_ll, albers_nad83, or polar_wgs84.
		// No default value.

	m_TYPE			= 0;
		// A character string, either categorical or continuous, that determines
		// whether the data in the data files should be interpreted as a continuous field or as discrete
		// indices. For categorical data represented by a fractional field for each possible category,
		// type should be set to continuous.
		// No default value.

	m_SIGNED		= false;
		// Either yes or no, indicating whether the values in the data files (which are
		// always represented as integers) are signed in two's complement form or not.
		// Default value is no.

	m_UNITS			= SG_T("");
		// A character string, enclosed in quotation marks ("), specifying the units of the
		// interpolated field; the string will be written to the geogrid output files as a variable timeindependent
		// attribute.
		// No default value.

	m_DESCRIPTION	= SG_T("");
		// A character string, enclosed in quotation marks ("), giving a short
		// description of the interpolated field; the string will be written to the geogrid output files
		// as a variable time-independent attribute.
		// No default value.

	m_DX			= 0.0;
		// A real value giving the grid spacing in the x-direction of the data set. If
		// projection is one of lambert, polar, mercator, albers_nad83, or polar_wgs84, dx
		// gives the grid spacing in meters; if projection is regular_ll, dx gives the grid spacing
		// in degrees.
		// No default value.

	m_DY			= 0.0;
		// A real value giving the grid spacing in the y-direction of the data set. If
		// projection is one of lambert, polar, mercator, albers_nad83, or polar_wgs84, dy
		// gives the grid spacing in meters; if projection is regular_ll, dy gives the grid spacing
		// in degrees.
		// No default value.

	m_KNOWN_X		= 1.0;
		// A real value specifying the i-coordinate of an (i,j) location
		// corresponding to a (latitude, longitude) location that is known in the projection.
		// Default value is 1.

	m_KNOWN_Y		= 1.0;
		// A real value specifying the j-coordinate of an (i,j) location
		// corresponding to a (latitude, longitude) location that is known in the projection.
		// Default value is 1.

	m_KNOWN_LAT		= 0.0;
		// A real value specifying the latitude of a (latitude, longitude)
		// location that is known in the projection.
		// No default value.

	m_KNOWN_LON		= 0.0;
		// A real value specifying the longitude of a (latitude, longitude)
		// location that is known in the projection.
		// No default value.

	m_STDLON		= 0.0;
		// A real value specifying the longitude that is parallel with the y-axis in
		// conic and azimuthal projections.
		// No default value.

	m_TRUELAT1		= 0.0;
		// A real value specifying the first true latitude for conic projections or
		// the only true latitude for azimuthal projections.
		// No default value.

	m_TRUELAT2		= 0.0;
		// A real value specifying the second true latitude for conic projections.
		// No default value.

	m_WORDSIZE		= 1;
		// An integer giving the number of bytes used to represent the value of
		// each grid point in the data files.
		// No default value.

	m_TILE_X		= 0;
		// An integer specifying the number of grid points in the x-direction,
		// excluding any halo points, for a single tile of source data.
		// No default value.

	m_TILE_Y		= 0;
		// An integer specifying the number of grid points in the y-direction,
		// excluding any halo points, for a single tile of source data.
		// No default value.

	m_TILE_Z		= 1;
		// An integer specifying the number of grid points in the z-direction for a
		// single tile of source data; this keyword serves as an alternative to the pair of keywords
		// tile_z_start and tile_z_end, and when this keyword is used, the starting z-index is
		// assumed to be 1.
		// No default value.

	m_TILE_Z_START	= 1;
		// An integer specifying the starting index in the z-direction of the
		// array in the data files. If this keyword is used, tile_z_end must also be specified.
		// No default value.

	m_TILE_Z_END	= 1;
		// An integer specifying the ending index in the z-direction of the array
		// in the data files. If this keyword is used, tile_z_start must also be specified.
		// No default value

	m_CATEGORY_MIN	= 0;
		// For categorical data (type=categorical), an integer specifying
		// the minimum category index that is found in the data set. If this keyword is used,
		// category_max must also be specified.
		// No default value.

	m_CATEGORY_MAX	= 0;
		// For categorical data (type=categorical), an integer
		// specifying the maximum category index that is found in the data set. If this keyword is
		// used, category_min must also be specified.
		// No default value.

	m_TILE_BDR		= 0;
		// An integer specifying the halo width, in grid points, for each tile of data.
		// Default value is 0.

	m_MISSING_VALUE	= -99999.0;
		// A real value that, when encountered in the data set, should be interpreted as missing data.
		// No default value.

	m_SCALE_FACTOR	= 1.0;
		// A real value that data should be scaled by (through
		// multiplication) after being read in as integers from tiles of the data set.
		// Default value is 1.

	m_ROW_ORDER		= VAL_BOTTOM_TOP;
		// A character string, either bottom_top or top_bottom, specifying
		// whether the rows of the data set arrays were written proceeding from the lowest-index
		// row to the highest (bottom_top) or from highest to lowest (top_bottom). This keyword
		// may be useful when utilizing some USGS data sets, which are provided in top_bottom
		// order.
		// Default value is bottom_top.

	m_ENDIAN		= VAL_ENDIAN_BIG;
		// A character string, either big or little, specifying whether the values in
		// the static data set arrays are in big-endian or little-endian byte order.
		// Default value is big.

	m_ISWATER		= 16;
		// An integer specifying the land use category of water.
		// Default value is 16.

	m_ISLAKE		= -1;
		// An integer specifying the land use category of inland water bodies.
		// Default value is -1 (i.e., no separate inland water category).

	m_ISICE			= 24;
		// An integer specifying the land use category of ice.
		// Default value is 24.

	m_ISURBAN		= 1;
		// An integer specifying the land use category of urban areas.
		// Default value is 1.

	m_ISOILWATER	= 14;
		// An integer specifying the soil category of water.
		// Default value is 14.

	m_MMINLU		= SG_T("USGS");
		// A character string, enclosed in quotation marks ("), indicating which
		// section of WRF's LANDUSE.TBL and VEGPARM.TBL will be used when looking up
		// parameters for land use categories.
		// Default value is "USGS".

	return( true );
}

//---------------------------------------------------------
bool CWRF_Index::Load(const CSG_String &File)
{
	Reset();

	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R, false) )
	{
		return( false );
	}

	CSG_String	sLine, sKey, sValue;

	while( Stream.Read_Line(sLine) )
	{
		sKey	= sLine.BeforeFirst(SG_T('=')).Make_Upper();	sKey  .Trim();	sKey  .Trim(true);
		sValue	= sLine.AfterFirst (SG_T('=')).Make_Upper();	sValue.Trim();	sValue.Trim(true);

		if( !sKey.Cmp(SG_T("PROJECTION")) )
		{
			m_PROJECTION	= sValue;
		}
		else if( !sKey.Cmp(SG_T("TYPE")) )
		{
			m_TYPE			= sValue.Contains(SG_T("CATEGORICAL")) ? VAL_CATEGORICAL : VAL_CONTINUOUS;
		}
		else if( !sKey.Cmp(SG_T("SIGNED")) )
		{
			m_SIGNED		= sValue.Contains(SG_T("YES"));
		}
		else if( !sKey.Cmp(SG_T("UNITS")) )
		{
			m_UNITS			= sValue;
		}
		else if( !sKey.Cmp(SG_T("DESCRIPTION")) )
		{
			m_DESCRIPTION	= sValue;
		}
		else if( !sKey.Cmp(SG_T("DX")) )
		{
			m_DX			= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("DY")) )
		{
			m_DY			= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("KNOWN_X")) )
		{
			m_KNOWN_X		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("KNOWN_Y")) )
		{
			m_KNOWN_Y		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("KNOWN_LAT")) )
		{
			m_KNOWN_LAT		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("KNOWN_LON")) )
		{
			m_KNOWN_LON		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("STDLON")) )
		{
			m_STDLON		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("TRUELAT1")) )
		{
			m_TRUELAT1		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("TRUELAT2")) )
		{
			m_TRUELAT2		= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("WORDSIZE")) )
		{
			m_WORDSIZE		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("TILE_X")) )
		{
			m_TILE_X		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("TILE_Y")) )
		{
			m_TILE_Y		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("TILE_Z")) )
		{
			m_TILE_Z		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("TILE_Z_START")) )
		{
			m_TILE_Z_START	= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("TILE_Z_END")) )
		{
			m_TILE_Z_END	= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("CATEGORY_MIN")) )
		{
			m_CATEGORY_MIN	= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("CATEGORY_MAX")) )
		{
			m_CATEGORY_MAX	= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("TILE_BDR")) )
		{
			m_TILE_BDR		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("MISSING_VALUE")) )
		{
			m_MISSING_VALUE	= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("SCALE_FACTOR")) )
		{
			m_SCALE_FACTOR	= sValue.asDouble();
		}
		else if( !sKey.Cmp(SG_T("ROW_ORDER")) )
		{
			m_ROW_ORDER		= sValue.Contains(SG_T("BOTTOM_TOP")) ? VAL_BOTTOM_TOP : VAL_TOP_BOTTOM;
		}
		else if( !sKey.Cmp(SG_T("ENDIAN")) )
		{
			m_ENDIAN		= sValue.Contains(SG_T("BIG")) ? VAL_ENDIAN_BIG : VAL_ENDIAN_LITTLE;
		}
		else if( !sKey.Cmp(SG_T("ISWATER")) )
		{
			m_ISWATER		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("ISLAKE")) )
		{
			m_ISLAKE		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("ISICE")) )
		{
			m_ISICE			= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("ISURBAN")) )
		{
			m_ISURBAN		= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("ISOILWATER")) )
		{
			m_ISOILWATER	= sValue.asInt();
		}
		else if( !sKey.Cmp(SG_T("MMINLU")) )
		{
			m_MMINLU		= sValue;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWRF_Index::Save(const CSG_String &File)
{
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_W, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Stream.Printf(SG_T("%s=%s\n"), SG_T("TYPE")				, m_TYPE == VAL_CATEGORICAL ? SG_T("CATEGORICAL") : SG_T("CONTINUOUS"));

	if( m_CATEGORY_MIN < m_CATEGORY_MAX )
	{
		Stream.Printf(SG_T("%s=%d\n"), SG_T("CATEGORY_MIN")	, m_CATEGORY_MIN);
		Stream.Printf(SG_T("%s=%d\n"), SG_T("CATEGORY_MAX")	, m_CATEGORY_MAX);
	}

	Stream.Printf(SG_T("%s=%s\n"), SG_T("PROJECTION")		, m_PROJECTION.c_str());

	if( m_PROJECTION.CmpNoCase(SG_T("regular_ll")) )
	{
		Stream.Printf(SG_T("%s=%f\n"), SG_T("STDLON")		, m_STDLON);
		Stream.Printf(SG_T("%s=%f\n"), SG_T("TRUELAT1")		, m_TRUELAT1);
		Stream.Printf(SG_T("%s=%f\n"), SG_T("TRUELAT2")		, m_TRUELAT2);
	}

	Stream.Printf(SG_T("%s=%.8f\n"), SG_T("DX")				, m_DX);
	Stream.Printf(SG_T("%s=%.8f\n"), SG_T("DY")				, m_DY);

	Stream.Printf(SG_T("%s=%f\n"), SG_T("KNOWN_X")			, m_KNOWN_X);
	Stream.Printf(SG_T("%s=%f\n"), SG_T("KNOWN_Y")			, m_KNOWN_Y);
	Stream.Printf(SG_T("%s=%.5f\n"), SG_T("KNOWN_LAT")		, m_KNOWN_LAT);
	Stream.Printf(SG_T("%s=%.5f\n"), SG_T("KNOWN_LON")		, m_KNOWN_LON);

	if( m_SIGNED )
	{
		Stream.Printf(SG_T("%s=%s\n"), SG_T("SIGNED")		, SG_T("YES"));
	}

	Stream.Printf(SG_T("%s=%d\n"), SG_T("WORDSIZE")			, m_WORDSIZE);

	Stream.Printf(SG_T("%s=%d\n"), SG_T("TILE_X")			, m_TILE_X);
	Stream.Printf(SG_T("%s=%d\n"), SG_T("TILE_Y")			, m_TILE_Y);

	if( m_TILE_Z == 1 )
	{
		Stream.Printf(SG_T("%s=%d\n"), SG_T("TILE_Z")		, m_TILE_Z);
	}
	else
	{
		Stream.Printf(SG_T("%s=%d\n"), SG_T("TILE_Z_START")	, m_TILE_Z_START);
		Stream.Printf(SG_T("%s=%d\n"), SG_T("TILE_Z_END")	, m_TILE_Z_END);
	}

	if( m_TILE_BDR > 0 )
	{
		Stream.Printf(SG_T("%s=%d\n"), SG_T("TILE_BDR")		, m_TILE_BDR);
	}

	Stream.Printf(SG_T("%s=%f\n"), SG_T("MISSING_VALUE")	, m_MISSING_VALUE);

	if( m_SCALE_FACTOR != 1.0 )
	{
		Stream.Printf(SG_T("%s=%f\n"), SG_T("SCALE_FACTOR")	, m_SCALE_FACTOR);
	}

	if( m_ROW_ORDER == VAL_TOP_BOTTOM )
	{
		Stream.Printf(SG_T("%s=%s\n"), SG_T("ROW_ORDER")	, SG_T("TOP_BOTTOM"));
	}

	if( m_ENDIAN == VAL_ENDIAN_LITTLE )
	{
		Stream.Printf(SG_T("%s=%s\n"), SG_T("ENDIAN")		, SG_T("LITTLE"));
	}

	if( m_TYPE == VAL_CATEGORICAL )
	{
		Stream.Printf(SG_T("%s=\"%s\"\n"), SG_T("UNITS")		, SG_T("CATEGORY"));
	}
	else if( m_UNITS.Length() > 0 )
	{
		Stream.Printf(SG_T("%s=\"%s\"\n"), SG_T("UNITS")		, m_UNITS.c_str());
	}

	if( m_DESCRIPTION.Length() > 0 )
	{
		Stream.Printf(SG_T("%s=\"%s\"\n"), SG_T("DESCRIPTION")	, m_DESCRIPTION.c_str());
	}

	if( m_MMINLU.CmpNoCase(SG_T("USGS")) )
	{
		Stream.Printf(SG_T("%s=%s\n"), SG_T("MMINLU")		, m_MMINLU.c_str());
	}

	if( m_ISWATER    != 16 ) Stream.Printf(SG_T("%s=%d\n"), SG_T("ISWATER")     , m_ISWATER);
	if( m_ISLAKE     != -1 ) Stream.Printf(SG_T("%s=%d\n"), SG_T("ISLAKE")      , m_ISLAKE);
	if( m_ISICE      != 24 ) Stream.Printf(SG_T("%s=%d\n"), SG_T("_ISICE")      , m_ISICE);
	if( m_ISURBAN    !=  1 ) Stream.Printf(SG_T("%s=%d\n"), SG_T("m_ISURBAN")   , m_ISURBAN);
	if( m_ISOILWATER != 14 ) Stream.Printf(SG_T("%s=%d\n"), SG_T("m_ISOILWATER"), m_ISOILWATER);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWRF_Import::CWRF_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import WRF Geogrid Binary Format"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Imports grid(s) from Weather Research and Forcasting Model (WRF) geogrid binary format."
		"\n"
		"<a target=\"_blank\" href=\"http://www.wrf-model.org\">WRF Homepage</a>"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("File"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWRF_Import::On_Execute(void)
{
	CSG_String		File;

	//-----------------------------------------------------
	File		= Parameters("FILE")	->asString();

	Parameters("GRIDS")->asGridList()->Del_Items();

	//-----------------------------------------------------
	if( !m_Index.Load(SG_File_Make_Path(SG_File_Get_Path(File), SG_T("index"))) )
	{
		Error_Set(_TL("error reading index file"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Load(File) )
	{
		Error_Set(_TL("error loading data file"));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWRF_Import::Load(const CSG_String &File)
{
	//-----------------------------------------------------
	// 00001-00600.00001-00600
	// 01234567890123456789012

	CSG_String	Name	= SG_File_Get_Name(File, true);

	if( Name.Length() != 23 || Name[5] != SG_T('-') || Name[11] != SG_T('.') || Name[17] != SG_T('-') )
	{
		Error_Set(_TL("invalid geogrid file name"));

		return( false );
	}

	int	xOffset	= Name.asInt() - 1;
	int	yOffset	= Name.AfterFirst(SG_T('.')).asInt() - 1;

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File, SG_FILE_R) )
	{
		Error_Set(_TL("data file could not be openend"));

		return( false );
	}

	//-----------------------------------------------------
	TSG_Data_Type	Type;

	switch( m_Index.m_WORDSIZE )
	{
	default:
		Error_Set(_TL("invalid word size"));

		return( false );

	case 1:	Type = m_Index.m_SIGNED == false ? SG_DATATYPE_Byte  : SG_DATATYPE_Char;  break;
	case 2:	Type = m_Index.m_SIGNED == false ? SG_DATATYPE_Word  : SG_DATATYPE_Short; break;
	case 4:	Type = m_Index.m_SIGNED == false ? SG_DATATYPE_DWord : SG_DATATYPE_Int;   break;
	}

	//-----------------------------------------------------
	char	*pLine, *pValue;
	int		x, y, nBytes_Line;

	nBytes_Line	= (m_Index.m_TILE_X + 2 * m_Index.m_TILE_BDR) * m_Index.m_WORDSIZE;
	pLine		= (char *)SG_Malloc(nBytes_Line);

	//-----------------------------------------------------
	for(int z=m_Index.m_TILE_Z_START; z<=m_Index.m_TILE_Z_END && !Stream.is_EOF() && Process_Get_Okay(); z++)
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(
			Type,
			m_Index.m_TILE_X + 2 * m_Index.m_TILE_BDR,
			m_Index.m_TILE_Y + 2 * m_Index.m_TILE_BDR,
			m_Index.m_DX,
			m_Index.m_KNOWN_LON + (xOffset - m_Index.m_TILE_BDR) * m_Index.m_DX,
			m_Index.m_KNOWN_LAT + (yOffset - m_Index.m_TILE_BDR) * m_Index.m_DY
		);

		pGrid->Set_Name			(CSG_String::Format(SG_T("%s_%02d"), SG_File_Get_Name(File, false).c_str(), z));
		pGrid->Set_Description	(m_Index.m_DESCRIPTION);
		pGrid->Set_Unit			(m_Index.m_UNITS);
		pGrid->Set_NoData_Value	(m_Index.m_MISSING_VALUE);
		pGrid->Set_Scaling		(m_Index.m_SCALE_FACTOR);

		Parameters("GRIDS")->asGridList()->Add_Item(pGrid);

		//-------------------------------------------------
		for(y=0; y<pGrid->Get_NY() && !Stream.is_EOF() && Set_Progress(y, pGrid->Get_NY()); y++)
		{
			int	yy	= m_Index.m_ROW_ORDER == VAL_TOP_BOTTOM ? pGrid->Get_NY() - 1 - y : y;

			Stream.Read(pLine, sizeof(char), nBytes_Line);

			for(x=0, pValue=pLine; x<pGrid->Get_NX(); x++, pValue+=m_Index.m_WORDSIZE)
			{
				if( m_Index.m_ENDIAN == VAL_ENDIAN_BIG )
				{
					SG_Swap_Bytes(pValue, m_Index.m_WORDSIZE);
				}

				switch( pGrid->Get_Type() )
				{
				case SG_DATATYPE_Byte:		pGrid->Set_Value(x, yy, *(unsigned char  *)pValue);	break;	// 1 Byte Integer (unsigned)
				case SG_DATATYPE_Char:		pGrid->Set_Value(x, yy, *(signed char    *)pValue);	break;	// 1 Byte Integer (signed)
				case SG_DATATYPE_Word:		pGrid->Set_Value(x, yy, *(unsigned short *)pValue);	break;	// 2 Byte Integer (unsigned)
				case SG_DATATYPE_Short:		pGrid->Set_Value(x, yy, *(signed short   *)pValue);	break;	// 2 Byte Integer (signed)
				case SG_DATATYPE_DWord:		pGrid->Set_Value(x, yy, *(unsigned int   *)pValue);	break;	// 4 Byte Integer (unsigned)
				case SG_DATATYPE_Int:		pGrid->Set_Value(x, yy, *(signed int     *)pValue);	break;	// 4 Byte Integer (signed)
				}
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(pLine);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWRF_Export::CWRF_Export(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Export WRF Geogrid Binary Format"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Exports grid(s) to Weather Research and Forcasting Model (WRF) geogrid binary format."
		"\n"
		"<a target=\"_blank\" href=\"http://www.wrf-model.org\">WRF Homepage</a>"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"			, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("Directory"),
		_TL(""),
		NULL, NULL, true, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_TYPE", _TL("Projection"), _TL(""));

	Parameters.Add_Choice(
		pNode	, "DATATYPE"	, _TL("Data Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("1 byte unsigned"),
			_TL("1 byte signed"),
			_TL("2 byte unsigned"),
			_TL("2 byte signed"),
			_TL("4 byte unsigned"),
			_TL("4 byte signed")
		), 0
	);

	Parameters.Add_Choice(
		pNode	, "TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("categorical"),
			_TL("continuous")
		), 0
	);

	Parameters.Add_Value(pNode	, "MISSING"		, _TL("Missing Value")		, _TL(""), PARAMETER_TYPE_Double, m_Index.m_MISSING_VALUE);
	Parameters.Add_Value(pNode	, "SCALE"		, _TL("Scale Factor")		, _TL(""), PARAMETER_TYPE_Double, m_Index.m_SCALE_FACTOR);

	Parameters.Add_String(pNode	, "UNITS"		, _TL("Units")				, _TL(""), m_Index.m_UNITS);
	Parameters.Add_String(pNode	, "DESCRIPTION"	, _TL("Description")		, _TL(""), m_Index.m_DESCRIPTION);
	Parameters.Add_String(pNode	, "MMINLU"		, _TL("Look Up Section")	, _TL(""), m_Index.m_MMINLU);

	Parameters.Add_Value(pNode	, "TILE_BDR"	, _TL("Halo Width")			, _TL(""), PARAMETER_TYPE_Int, 0, 0, true);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_PRJ", _TL("Projection"), _TL(""));

	Parameters.Add_Choice(
		pNode	, "PROJECTION"	, _TL("Projection"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			SG_T("lambert"),
			SG_T("polar"),
			SG_T("mercator"),
			SG_T("regular_ll"),
			SG_T("albers_nad83"),
			SG_T("polar_wgs84")
		), 3
	);

	Parameters.Add_Value(pNode	, "SDTLON"		, _TL("Standard Longitude")	, _TL(""), PARAMETER_TYPE_Double,  0.0);
	Parameters.Add_Value(pNode	, "TRUELAT1"	, _TL("True Latitude 1")	, _TL(""), PARAMETER_TYPE_Double, 45.0);
	Parameters.Add_Value(pNode	, "TRUELAT2"	, _TL("True Latitude 2")	, _TL(""), PARAMETER_TYPE_Double, 35.0);

	//-----------------------------------------------------
//	pNode	= Parameters.Add_Node(NULL, "NODE_REF", _TL("Referencing"), _TL(""));

//	Parameters.Add_Value(pNode	, "KNOWN_X"		, _TL("Known X")			, _TL(""), PARAMETER_TYPE_Double,    1.0);
//	Parameters.Add_Value(pNode	, "KNOWN_Y"		, _TL("Known Y")			, _TL(""), PARAMETER_TYPE_Double,    1.0);
//	Parameters.Add_Value(pNode	, "KNOWN_LON"	, _TL("Longitude")			, _TL(""), PARAMETER_TYPE_Double, -180.0);
//	Parameters.Add_Value(pNode	, "KNOWN_LAT"	, _TL("Latitude")			, _TL(""), PARAMETER_TYPE_Double,  -90.0);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(NULL, "NODE_CAT", _TL("Categories"), _TL(""));

	Parameters.Add_Value(pNode	, "ISWATER"		, _TL("Water")				, _TL(""), PARAMETER_TYPE_Int, m_Index.m_ISWATER);
	Parameters.Add_Value(pNode	, "ISLAKE"		, _TL("Lake")				, _TL(""), PARAMETER_TYPE_Int, m_Index.m_ISLAKE);
	Parameters.Add_Value(pNode	, "ISICE"		, _TL("Ice")				, _TL(""), PARAMETER_TYPE_Int, m_Index.m_ISICE);
	Parameters.Add_Value(pNode	, "ISURBAN"		, _TL("Urban")				, _TL(""), PARAMETER_TYPE_Int, m_Index.m_ISURBAN);
	Parameters.Add_Value(pNode	, "ISOILWATER"	, _TL("Soil Water")			, _TL(""), PARAMETER_TYPE_Int, m_Index.m_ISOILWATER);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWRF_Export::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("MMINLU")) )
	{
		if( !SG_STR_CMP(pParameter->asString(), SG_T("USGS")) )
		{
			pParameters->Get_Parameter("DESCRIPTION")->Set_Value(SG_T("24-category USGS landuse"));
		}
	}

	//-----------------------------------------------------
	return( 1 );
}

//---------------------------------------------------------
int CWRF_Export::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("PROJECTION")) )
	{
		pParameters->Get_Parameter("SDTLON"  )->Set_Enabled(pParameter->asInt() != 3);
		pParameters->Get_Parameter("TRUELAT1")->Set_Enabled(pParameter->asInt() != 3);
		pParameters->Get_Parameter("TRUELAT2")->Set_Enabled(pParameter->asInt() != 3);
	}

	//-----------------------------------------------------
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWRF_Export::On_Execute(void)
{
	CSG_String				Directory;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	Directory	= Parameters("FILE")	->asString();
	pGrids		= Parameters("GRIDS")	->asGridList();

	//-----------------------------------------------------
	m_Index.Reset();

	switch( Parameters("DATATYPE")->asInt() )
	{
	case 0:	default:	m_Index.m_WORDSIZE	= 1;	m_Index.m_SIGNED	= false;	break;
	case 1:				m_Index.m_WORDSIZE	= 1;	m_Index.m_SIGNED	=  true;	break;
	case 2:				m_Index.m_WORDSIZE	= 2;	m_Index.m_SIGNED	= false;	break;
	case 3:				m_Index.m_WORDSIZE	= 2;	m_Index.m_SIGNED	=  true;	break;
	case 4:				m_Index.m_WORDSIZE	= 4;	m_Index.m_SIGNED	= false;	break;
	case 5:				m_Index.m_WORDSIZE	= 4;	m_Index.m_SIGNED	=  true;	break;
	}

	m_Index.m_TYPE			= Parameters("TYPE")		->asInt();
	m_Index.m_MISSING_VALUE	= Parameters("MISSING")		->asDouble();
	m_Index.m_SCALE_FACTOR	= Parameters("SCALE")		->asDouble();
	m_Index.m_UNITS			= Parameters("UNITS")		->asString();
	m_Index.m_DESCRIPTION	= Parameters("DESCRIPTION")	->asString();
	m_Index.m_MMINLU		= Parameters("MMINLU")		->asString();

	m_Index.m_TILE_BDR		= Parameters("TILE_BDR")	->asInt();
	m_Index.m_TILE_X		= Get_NX() - 2 * m_Index.m_TILE_BDR;
	m_Index.m_TILE_Y		= Get_NY() - 2 * m_Index.m_TILE_BDR;
	m_Index.m_TILE_Z		= pGrids->Get_Count();
	m_Index.m_TILE_Z_START	= 1;
	m_Index.m_TILE_Z_END	= pGrids->Get_Count();
	m_Index.m_DX			= Get_Cellsize();
	m_Index.m_DY			= Get_Cellsize();
	m_Index.m_ENDIAN		= VAL_ENDIAN_LITTLE;
	m_Index.m_ROW_ORDER		= VAL_BOTTOM_TOP;

	m_Index.m_PROJECTION	= Parameters("PROJECTION")	->asString();
	m_Index.m_STDLON		= Parameters("SDTLON")		->asDouble();
	m_Index.m_TRUELAT1		= Parameters("TRUELAT1")	->asDouble();
	m_Index.m_TRUELAT2		= Parameters("TRUELAT2")	->asDouble();

	m_Index.m_KNOWN_LAT		= - 90.0 + 0.5 * m_Index.m_DY;
	m_Index.m_KNOWN_LON		= -180.0 + 0.5 * m_Index.m_DX;

//	m_Index.m_KNOWN_X		= Parameters("KNOWN_X")		->asDouble();
//	m_Index.m_KNOWN_Y		= Parameters("KNOWN_Y")		->asDouble();
//	m_Index.m_KNOWN_LAT		= Parameters("KNOWN_LAT")	->asDouble();
//	m_Index.m_KNOWN_LON		= Parameters("KNOWN_LON")	->asDouble();

	if( m_Index.m_TILE_Z == 1 )
	{
		m_Index.m_CATEGORY_MIN	= m_Index.m_TYPE == VAL_CATEGORICAL ? (int)pGrids->asGrid(0)->Get_ZMin() : 0;
		m_Index.m_CATEGORY_MAX	= m_Index.m_TYPE == VAL_CATEGORICAL ? (int)pGrids->asGrid(0)->Get_ZMax() : 0;
	}
	else
	{
		m_Index.m_CATEGORY_MIN	= m_Index.m_TILE_Z_START;
		m_Index.m_CATEGORY_MAX	= m_Index.m_TILE_Z_END;
	}

	m_Index.m_ISWATER		= Parameters("ISWATER")		->asInt();
	m_Index.m_ISLAKE		= Parameters("ISLAKE")		->asInt();
	m_Index.m_ISICE			= Parameters("ISICE")		->asInt();
	m_Index.m_ISURBAN		= Parameters("ISURBAN")		->asInt();
	m_Index.m_ISOILWATER	= Parameters("ISOILWATER")	->asInt();

	//-----------------------------------------------------
	if( !m_Index.Save(SG_File_Make_Path(Directory, SG_T("index"))) )
	{
		Error_Set(_TL("error saving index file"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Save(Directory, pGrids) )
	{
		Error_Set(_TL("error saving data file"));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWRF_Export::Save(const CSG_String &Directory, CSG_Parameter_Grid_List *pGrids)
{
	//-----------------------------------------------------
	// 00001-00600.00001-00600
	// 01234567890123456789012

	int	xOffset	= m_Index.m_TILE_BDR + (int)(0.5 + (Get_XMin() - m_Index.m_KNOWN_LON) / Get_Cellsize());
	int	yOffset	= m_Index.m_TILE_BDR + (int)(0.5 + (Get_YMin() - m_Index.m_KNOWN_LAT) / Get_Cellsize());

	CSG_String	Name	= SG_File_Get_Name(Directory, true);

	Name.Printf(SG_T("%05d-%05d.%05d-%05d"), xOffset + 1, xOffset + m_Index.m_TILE_X, yOffset + 1, yOffset + m_Index.m_TILE_Y);

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(SG_File_Make_Path(Directory, Name), SG_FILE_W) )
	{
		Error_Set(_TL("data file could not be openend"));

		return( false );
	}

	//-----------------------------------------------------
	char	*pLine, *pValue;
	int		x, y, nBytes_Line;

	nBytes_Line	= Get_NX() * m_Index.m_WORDSIZE;
	pLine		= (char *)SG_Malloc(nBytes_Line);

	//-----------------------------------------------------
	for(int z=0; z<pGrids->Get_Count() && Process_Get_Okay(); z++)
	{
		CSG_Grid	*pGrid	= pGrids->asGrid(z);

		//-------------------------------------------------
		for(y=0; y<pGrid->Get_NY() && !Stream.is_EOF() && Set_Progress(y, pGrid->Get_NY()); y++)
		{
			int	yy	= m_Index.m_ROW_ORDER == VAL_TOP_BOTTOM ? pGrid->Get_NY() - 1 - y : y;

			for(x=0, pValue=pLine; x<pGrid->Get_NX(); x++, pValue+=m_Index.m_WORDSIZE)
			{
				if( m_Index.m_SIGNED )
				{
					switch( m_Index.m_WORDSIZE )
					{
					case 1:	*((signed char    *)pValue)	= (signed char   )pGrid->asInt(x, yy);	break;
					case 2:	*((signed short   *)pValue)	= (signed short  )pGrid->asInt(x, yy);	break;
					case 4:	*((signed int     *)pValue)	= (signed int    )pGrid->asInt(x, yy);	break;
					}
				}
				else
				{
					switch( m_Index.m_WORDSIZE )
					{
					case 1:	*((unsigned char  *)pValue)	= (unsigned char )pGrid->asInt(x, yy);	break;
					case 2:	*((unsigned short *)pValue)	= (unsigned short)pGrid->asInt(x, yy);	break;
					case 4:	*((unsigned int   *)pValue)	= (unsigned int  )pGrid->asInt(x, yy);	break;
					}
				}

				if( m_Index.m_ENDIAN == VAL_ENDIAN_BIG )
				{
					SG_Swap_Bytes(pValue, m_Index.m_WORDSIZE);
				}
			}

			Stream.Write(pLine, sizeof(char), nBytes_Line);
		}
	}

	//-----------------------------------------------------
	SG_Free(pLine);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
