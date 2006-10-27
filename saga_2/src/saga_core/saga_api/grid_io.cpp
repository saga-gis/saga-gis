
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     grid_io.cpp                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//				Grid: File Operations					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>

#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load(const char *File_Name, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	bool	bResult;

	//-----------------------------------------------------
	Destroy();

	m_Type	= Type;

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Load grid"), File_Name), true);

	if( SG_File_Cmp_Extension(File_Name, "grd") )
	{
		bResult	= _Load_Surfer(File_Name, Memory_Type);
	}
	else
	{
		bResult	= _Load_Native(File_Name, Memory_Type);
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_File_Name(File_Name);

		m_bCreated	= true;
		m_bUpdate	= true;

		if( !Get_History().Load(File_Name, HISTORY_EXT_GRID) )
		{
			Get_History().Add_Entry(LNG("[HST] Loaded from file"), File_Name);
		}

		SG_UI_Msg_Add(LNG("[MSG] okay"), false);
	}
	else
	{
		Destroy();

		SG_UI_Msg_Add(LNG("[MSG] failed"), false);

		SG_UI_Msg_Add_Error(LNG("[ERR] Grid file could not be opened."));
	}

	//-----------------------------------------------------
	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::Save(const char *File_Name, int Format)
{
	return( Save(File_Name, Format, 0, 0, Get_NX(), Get_NY()) );
}

bool CSG_Grid::Save(const char *File_Name, int Format, int xA, int yA, int xN, int yN)
{
	bool	bResult;

	//-----------------------------------------------------
	if( xA	< 0 || xA >= Get_NX() - 1 )
	{
		xA	= 0;
	}

	if( yA	< 0 || yA >= Get_NY() - 1 )
	{
		yA	= 0;
	}

	if( xN	> Get_NX() - xA )
	{
		xN	= Get_NX() - xA;
	}

	if( yN	> Get_NY() - yA )
	{
		yN	= Get_NY() - yA;
	}

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", LNG("[MSG] Save grid"), File_Name), true);

	switch( Format )
	{
	default:
	case GRID_FILE_FORMAT_Binary:	// 1 - Binary
		bResult	= _Save_Native(File_Name, xA, yA, xN, yN, true);
		break;

	case GRID_FILE_FORMAT_ASCII:	// 2 - ASCII
		bResult	= _Save_Native(File_Name, xA, yA, xN, yN, false);
		break;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(File_Name);

		Get_History().Save(File_Name, HISTORY_EXT_GRID);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false);
	}
	else
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false);

		SG_UI_Msg_Add_Error(LNG("[ERR] Grid file could not be saved."));
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//						Binary							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_Swap_Bytes(char *Bytes, int nBytes) const
{
	char	Byte, *p;

	p	= Bytes + nBytes - 1;

	while( Bytes < p )
	{
		Byte		= *Bytes;
		*(Bytes++)	= *p;
		*(p--)		= Byte;
	}
}

//---------------------------------------------------------
bool CSG_Grid::_Load_Binary(FILE *Stream, TSG_Grid_Type File_Type, bool bFlip, bool bSwapBytes)
{
	char	*Line, *pValue;
	int		x, y, i, iy, dy, nxBytes, nValueBytes;

	if( Stream && is_Valid() )
	{
		Set_File_Type(GRID_FILE_FORMAT_Binary);

		if( bFlip )
		{
			y	= Get_NY() - 1;
			dy	= -1;
		}
		else
		{
			y	= 0;
			dy	= 1;
		}

		//-------------------------------------------------
		if( File_Type == GRID_TYPE_Bit )
		{
			nxBytes		= Get_NX() / 8 + 1;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal )
			{
				for(iy=0; iy<Get_NY() && !feof(Stream) && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					fread(m_Values[y], sizeof(char), nxBytes, Stream);
				}
			}
			else
			{
				Line	= (char *)SG_Malloc(nxBytes);

				for(iy=0; iy<Get_NY() && !feof(Stream) && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					fread(Line		, nxBytes	, sizeof(char), Stream);

					for(x=0, pValue=Line; x<Get_NX(); pValue++)
					{
						for(i=0; i<8 && x<Get_NX(); i++, x++)
						{
							Set_Value(x, y, (*pValue & m_Bitmask[i]) == 0 ? 0.0 : 1.0);
						}
					}
				}

				SG_Free(Line);
			}
		}

		//-------------------------------------------------
		else
		{
			nValueBytes	= gSG_Grid_Type_Sizes[File_Type];
			nxBytes		= Get_NX() * nValueBytes;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && !bSwapBytes )
			{
				for(iy=0; iy<Get_NY() && !feof(Stream) && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					fread(m_Values[y], sizeof(char), nxBytes, Stream);
				}
			}
			else
			{
				Line	= (char *)SG_Malloc(nxBytes);

				for(iy=0; iy<Get_NY() && !feof(Stream) && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					fread(Line		, nxBytes	, sizeof(char), Stream);

					for(x=0, pValue=Line; x<Get_NX(); x++, pValue+=nValueBytes)
					{
						if( bSwapBytes )
						{
							_Swap_Bytes(pValue, nValueBytes);
						}

						switch( File_Type )
						{
						default:													break;
						case GRID_TYPE_Byte:	Set_Value(x, y, *(BYTE   *)pValue);	break;
						case GRID_TYPE_Char:	Set_Value(x, y, *(char   *)pValue);	break;
						case GRID_TYPE_Word:	Set_Value(x, y, *(WORD   *)pValue);	break;
						case GRID_TYPE_Short:	Set_Value(x, y, *(short  *)pValue);	break;
						case GRID_TYPE_DWord:	Set_Value(x, y, *(DWORD  *)pValue);	break;
						case GRID_TYPE_Int:		Set_Value(x, y, *(int    *)pValue);	break;
						case GRID_TYPE_Long:	Set_Value(x, y, *(long   *)pValue);	break;
						case GRID_TYPE_Float:	Set_Value(x, y, *(float  *)pValue);	break;
						case GRID_TYPE_Double:	Set_Value(x, y, *(double *)pValue);	break;
						}
					}
				}

				SG_Free(Line);
			}
		}

		//-------------------------------------------------
		SG_UI_Process_Set_Ready();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Binary(FILE *Stream, int xA, int yA, int xN, int yN, TSG_Grid_Type File_Type, bool bFlip, bool bSwapBytes)
{
	char	*Line, *pValue;
	int		x, y, i, ix, iy, dy, axBytes, nxBytes, nValueBytes;

	//-----------------------------------------------------
	if( Stream && m_System.is_Valid() && m_Type > 0 && m_Type < GRID_TYPE_Count )
	{
		Set_File_Type(GRID_FILE_FORMAT_Binary);

		if( bFlip )
		{
			y	= yA + yN - 1;
			dy	= -1;
		}
		else
		{
			y	= yA;
			dy	= 1;
		}

		//-------------------------------------------------
		if( File_Type == GRID_TYPE_Bit )
		{
			nxBytes		= xN / 8 + 1;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && xA % 8 == 0 )
			{
				axBytes		= xA / 8;

				for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
				{
					fwrite((char *)m_Values[y] + axBytes, nxBytes, sizeof(char), Stream);
				}
			}
			else
			{
				Line	= (char *)SG_Malloc(nxBytes);

				for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
				{
					for(ix=0, x=xA, pValue=Line; ix<xN; pValue++)
					{
						for(i=0; i<8 && ix<xN; i++, ix++, x++)
						{
							*pValue	= asChar(x, y) != 0.0 ? *pValue | m_Bitmask[i] : *pValue & (~m_Bitmask[i]);
						}
					}

					fwrite(Line		, nxBytes	, sizeof(char), Stream);
				}

				SG_Free(Line);
			}
		}

		//-------------------------------------------------
		else
		{
			nValueBytes	= gSG_Grid_Type_Sizes[File_Type];
			nxBytes		= xN * nValueBytes;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && !bSwapBytes )
			{
				axBytes	= xA * nValueBytes;

				for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
				{
					fwrite((char *)m_Values[y] + axBytes, nxBytes, sizeof(char), Stream);
				}
			}
			else
			{
				Line	= (char *)SG_Malloc(nxBytes);

				for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
				{
					for(ix=0, x=xA, pValue=Line; ix<xN; ix++, x++, pValue+=nValueBytes)
					{
						switch( File_Type )
						{
						default:														break;
						case GRID_TYPE_Byte:	*(BYTE   *)pValue	= asChar	(x, y);	break;
						case GRID_TYPE_Char:	*(char   *)pValue	= asChar	(x, y);	break;
						case GRID_TYPE_Word:	*(WORD   *)pValue	= asShort	(x, y);	break;
						case GRID_TYPE_Short:	*(short  *)pValue	= asShort	(x, y);	break;
						case GRID_TYPE_DWord:	*(DWORD  *)pValue	= asInt		(x, y);	break;
						case GRID_TYPE_Int:		*(int    *)pValue	= asInt		(x, y);	break;
						case GRID_TYPE_Long:	*(long   *)pValue	= asLong	(x, y);	break;
						case GRID_TYPE_Float:	*(float  *)pValue	= asFloat	(x, y);	break;
						case GRID_TYPE_Double:	*(double *)pValue	= asDouble	(x, y);	break;
						}

						if( bSwapBytes )
						{
							_Swap_Bytes(pValue, nValueBytes);
						}
					}

					fwrite(Line		, nxBytes	, sizeof(char), Stream);
				}

				SG_Free(Line);
			}
		}

		//-------------------------------------------------
		SG_UI_Process_Set_Ready();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//							ASCII						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_ASCII(FILE *Stream, TSG_Grid_Memory_Type Memory_Type, bool bFlip)
{
	int		x, y, iy, dy;
	double	Value;

	if( Stream && m_System.is_Valid() && m_Type > 0 && m_Type < GRID_TYPE_Count && _Memory_Create(Memory_Type) )
	{
		Set_File_Type(GRID_FILE_FORMAT_ASCII);

		if( bFlip )
		{
			y	= Get_NY() - 1;
			dy	= -1;
		}
		else
		{
			y	= 0;
			dy	= 1;
		}

		//-------------------------------------------------
		for(iy=0; iy<Get_NY() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
		{
			for(x=0; x<Get_NX(); x++)
			{
				fscanf(Stream, "%lf", &Value);

				Set_Value(x, y, Value);
			}
		}

		SG_UI_Process_Set_Ready();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_ASCII(FILE *Stream, int xA, int yA, int xN, int yN, bool bFlip)
{
	int		x, y, ix, iy, dy;

	if( Stream && is_Valid() )
	{
		Set_File_Type(GRID_FILE_FORMAT_ASCII);

		if( bFlip )
		{
			y	= yA + yN - 1;
			dy	= -1;
		}
		else
		{
			y	= yA;
			dy	= 1;
		}

		//-------------------------------------------------
		for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
		{
			for(ix=0, x=xA; ix<xN; ix++, x++)
			{
				fprintf(Stream, "%lf ", asDouble(x, y));
			}

			fprintf(Stream, "\n");
		}

		SG_UI_Process_Set_Ready();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Native							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_Native(const char *File_Header, TSG_Grid_Memory_Type Memory_Type)
{
	bool			bResult, hdr_bFlip, hdr_bSwapBytes;
	int				iType, hdr_Offset, NX, NY;
	double			Cellsize, xMin, yMin;
	FILE			*Stream;
	TSG_Grid_Type		hdr_Type;
	CSG_Grid_System	System;
	CSG_String		File_Data, Value;

	//-----------------------------------------------------
	bResult	= false;

	if( File_Header && (Stream = fopen(File_Header, "r")) != NULL )
	{
		//-------------------------------------------------
		// Load Header...

		hdr_Type		= GRID_TYPE_Count;
		hdr_Offset		= 0;
		hdr_bFlip		= false;
		hdr_bSwapBytes	= false;

		NX	= NY		= 0;
		Cellsize		= 0.0;
		xMin			= 0.0;
		yMin			= 0.0;

		//-------------------------------------------------
		do
		{
			switch( _Load_Native_Get_Key(Stream, Value) )
			{
			case GRID_FILE_KEY_NAME:			Set_Name		(Value);			break;
			case GRID_FILE_KEY_DESCRIPTION:		Set_Description	(Value);			break;
			case GRID_FILE_KEY_UNITNAME:		Set_Unit		(Value);			break;

			case GRID_FILE_KEY_CELLCOUNT_X:		NX				= Value.asInt();	break;
			case GRID_FILE_KEY_CELLCOUNT_Y:		NY				= Value.asInt();	break;
			case GRID_FILE_KEY_POSITION_XMIN:	xMin			= Value.asDouble();	break;
			case GRID_FILE_KEY_POSITION_YMIN:	yMin			= Value.asDouble();	break;
			case GRID_FILE_KEY_CELLSIZE:		Cellsize		= Value.asDouble();	break;
			case GRID_FILE_KEY_Z_FACTOR:		m_zFactor		= Value.asDouble();	break;
			case GRID_FILE_KEY_NODATA_VALUE:	m_NoData_Value	= m_NoData_hiValue	= Value.asDouble();	break;

			case GRID_FILE_KEY_DATAFILE_OFFSET:	hdr_Offset		= Value.asInt();	break;
			case GRID_FILE_KEY_BYTEORDER_BIG:	hdr_bSwapBytes	= Value.Find(GRID_FILE_KEY_TRUE) >= 0;	break;
			case GRID_FILE_KEY_TOPTOBOTTOM:		hdr_bFlip		= Value.Find(GRID_FILE_KEY_TRUE) >= 0;	break;

			case GRID_FILE_KEY_DATAFILE_NAME:
				if( SG_File_Get_Path(Value).Length() > 0 )
				{
					File_Data	= Value;
				}
				else
				{
					File_Data	= SG_File_Make_Path(SG_File_Get_Path(Value), Value);
				}
				break;

			case GRID_FILE_KEY_DATAFORMAT:
				for(iType=0; iType<GRID_TYPE_Count && hdr_Type == GRID_TYPE_Count; iType++)
				{
					if( Value.Find(gSG_Grid_Type_Names[iType]) >= 0 )
					{
						hdr_Type	= (TSG_Grid_Type)iType;
					}
				}
				break;
			}
		}
		while( !feof(Stream) );

		fclose(Stream);


		//-------------------------------------------------
		// Load Data...

		if( hdr_Type < GRID_TYPE_Count && m_System.Assign(Cellsize, xMin, yMin, NX, NY) )
		{
			//---------------------------------------------
			// ASCII...

			if( hdr_Type == GRID_TYPE_Undefined )
			{
				if( m_Type <= GRID_TYPE_Undefined || m_Type >= GRID_TYPE_Count )
				{
					m_Type	= GRID_TYPE_Float;
				}

				if(	(File_Data.Length() > 0 && (Stream = fopen(File_Data			, "r")) != NULL)
				||	(Stream = fopen(SG_File_Make_Path(NULL, File_Header,  "dat")	, "r")) != NULL
				||	(Stream = fopen(SG_File_Make_Path(NULL, File_Header, "sdat")	, "r")) != NULL )
				{
					fseek(Stream, hdr_Offset, SEEK_SET);
					bResult	= _Load_ASCII(Stream, Memory_Type);
					fclose(Stream);
				}
			}

			//---------------------------------------------
			// Binary...

			else if( hdr_Type > GRID_TYPE_Undefined )
			{
				if( m_Type <= GRID_TYPE_Undefined || m_Type >= GRID_TYPE_Count )
				{
					m_Type	= hdr_Type;
				}

				if( Memory_Type == GRID_MEMORY_Cache && _Cache_Create(File_Data, hdr_Type, hdr_Offset, hdr_bSwapBytes, hdr_bFlip) )
				{
					return( true );
				}

				if( _Memory_Create(Memory_Type) )
				{
					if(	(File_Data.Length() > 0 && (Stream = fopen(File_Data			, "rb")) != NULL)
					||	(Stream = fopen(SG_File_Make_Path(NULL, File_Header,  "dat")	, "rb")) != NULL
					||	(Stream = fopen(SG_File_Make_Path(NULL, File_Header, "sdat")	, "rb")) != NULL )
					{
						fseek(Stream, hdr_Offset, SEEK_SET);
						bResult	= _Load_Binary(Stream, hdr_Type, hdr_bFlip, hdr_bSwapBytes);
						fclose(Stream);
					}
				}
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Native(const char *File_Name, int xA, int yA, int xN, int yN, bool bBinary)
{
	bool	bResult		= false;
	FILE	*Stream;

	if(	(Stream = fopen(SG_File_Make_Path(NULL, File_Name, "sgrd"), "w")) != NULL )
	{
		//-------------------------------------------------
		// Header...

		fprintf(Stream, "%s\t= %s\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_NAME			], Get_Name() );
		fprintf(Stream, "%s\t= %s\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_DESCRIPTION	], Get_Description() );
		fprintf(Stream, "%s\t= %s\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_UNITNAME		], Get_Unit() );
		fprintf(Stream, "%s\t= %d\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_DATAFILE_OFFSET], 0 );
		fprintf(Stream, "%s\t= %s\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_DATAFORMAT		], gSG_Grid_Type_Names[bBinary ? Get_Type() : 0] );
		fprintf(Stream, "%s\t= %s\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_BYTEORDER_BIG	], GRID_FILE_KEY_FALSE );
		fprintf(Stream, "%s\t= %f\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_POSITION_XMIN	], Get_XMin() + Get_Cellsize() * xA );
		fprintf(Stream, "%s\t= %f\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_POSITION_YMIN	], Get_YMin() + Get_Cellsize() * yA );
		fprintf(Stream, "%s\t= %d\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_CELLCOUNT_X	], xN );
		fprintf(Stream, "%s\t= %d\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_CELLCOUNT_Y	], yN );
		fprintf(Stream, "%s\t= %f\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_CELLSIZE		], Get_Cellsize() );
		fprintf(Stream, "%s\t= %f\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_Z_FACTOR		], m_zFactor );
		fprintf(Stream, "%s\t= %f\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_NODATA_VALUE	], m_NoData_Value );
		fprintf(Stream, "%s\t= %s\n", gSG_Grid_File_Key_Names[ GRID_FILE_KEY_TOPTOBOTTOM	], GRID_FILE_KEY_FALSE );

		fclose(Stream);


		//-------------------------------------------------
		// Data...

		if( (Stream = fopen(SG_File_Make_Path(NULL, File_Name, "sdat"), "wb")) != NULL )
		{
			if( bBinary )
			{
				bResult		= _Save_Binary	(Stream, xA, yA, xN, yN, Get_Type(), false, false);
			}
			else
			{
				bResult		= _Save_ASCII	(Stream, xA, yA, xN, yN);
			}

			fclose(Stream);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Grid::_Load_Native_Get_Key(FILE *Stream, CSG_String &Value)
{
	int			i;
	CSG_String	sLine;

	if( SG_Read_Line(Stream, sLine) && (i = sLine.Find('=')) > 0 )
	{
		Value.Printf(sLine.AfterFirst('='));
		Value.Remove_WhiteChars();

		sLine.Remove(i);

		for(i=0; i<GRID_FILE_KEY_Count; i++)
		{
			if( sLine.Find(gSG_Grid_File_Key_Names[i]) >= 0 )
			{
				return( i );
			}
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_Surfer(const char *File_Name, TSG_Grid_Memory_Type Memory_Type)
{
	bool	bResult		= false;
	char	Identifier[4];
	short	sValue;
	int		x, y, NX, NY;
	float	*fLine;
	double	dValue, xMin, yMin, Cellsize;
	FILE	*Stream;

	if( (Stream = fopen(File_Name, "rb")) != NULL )
	{
		fread(Identifier, 4, sizeof(char), Stream);

		//-------------------------------------------------
		// Binary...

		if( !strncmp(Identifier, "DSBB", 4) )
		{
			fread(&sValue	, 1, sizeof(short)	, Stream);
			NX			= sValue;
			fread(&sValue	, 1, sizeof(short)	, Stream);
			NY			= sValue;

			fread(&xMin		, 1, sizeof(double)	, Stream);
			fread(&dValue	, 1, sizeof(double)	, Stream);	// XMax
			Cellsize	= (dValue - xMin) / (NX - 1.0);

			fread(&yMin		, 1, sizeof(double)	, Stream);
			fread(&dValue	, 1, sizeof(double)	, Stream);	// YMax...
			//DY		= (dValue - yMin) / (NY - 1.0);		// we could check, if cellsizes (x/y) equal...

			fread(&dValue	, 1, sizeof(double)	, Stream);	// ZMin...
			fread(&dValue	, 1, sizeof(double)	, Stream);	// ZMax...

			//---------------------------------------------
			if( !feof(Stream) && Create(GRID_TYPE_Float, NX, NY, Cellsize, xMin, yMin, Memory_Type) )
			{
				bResult	= true;

				fLine	= (float *)SG_Malloc(Get_NX() * sizeof(float));

				for(y=0; y<Get_NY() && !feof(Stream) && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
				{
					fread(fLine, Get_NX(), sizeof(float), Stream);

					for(x=0; x<Get_NX(); x++)
					{
						Set_Value(x, y, fLine[x]);
					}
				}

				SG_Free(fLine);
			}
		}


		//-------------------------------------------------
		// ASCII...

		else if( !strncmp(Identifier, "DSAA", 4) )
		{
			fscanf(Stream, "%d %d"	, &NX	, &NY);

			fscanf(Stream, "%lf %lf", &xMin	, &dValue);
			Cellsize	= (dValue - xMin) / (NX - 1.0);

			fscanf(Stream, "%lf %lf", &yMin	, &dValue);
			//DY		= (dValue - yMin) / (NY - 1.0);

			fscanf(Stream, "%lf %lf", &dValue, &dValue);

			//---------------------------------------------
			if( !feof(Stream) && Create(GRID_TYPE_Float, NX, NY, Cellsize, xMin, yMin, Memory_Type) )
			{
				bResult	= true;

				for(y=0; y<Get_NY() && !feof(Stream) && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
				{
					for(x=0; x<Get_NX(); x++)
					{
						fscanf(Stream, "%lf", &dValue);

						Set_Value(x, y, dValue);
					}
				}
			}
		}

		//-------------------------------------------------
		fclose(Stream);

		SG_UI_Process_Set_Ready();
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
