
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
bool CSG_Grid::_Load(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	bool	bResult;

	//-----------------------------------------------------
	Destroy();

	m_Type	= Type;

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Load grid"), File_Name.c_str()), true);

	if( SG_File_Cmp_Extension(File_Name, SG_T("grd")) )
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
		Set_Update_Flag();

		Set_File_Name(File_Name);

		Load_MetaData(File_Name);

		m_bCreated	= true;

		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	}
	else
	{
		Destroy();

		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(LNG("[ERR] Grid file could not be opened."));
	}

	//-----------------------------------------------------
	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::Save(const CSG_String &File_Name, int Format)
{
	return( Save(File_Name, Format, 0, 0, Get_NX(), Get_NY()) );
}

bool CSG_Grid::Save(const CSG_String &File_Name, int Format, int xA, int yA, int xN, int yN)
{
	bool		bResult;
	CSG_String	sFile_Name	= SG_File_Make_Path(NULL, File_Name, SG_T("sgrd"));

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
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), LNG("[MSG] Save grid"), File_Name.c_str()), true);

	switch( Format )
	{
	default:
	case GRID_FILE_FORMAT_Binary:	// 1 - Binary
		bResult	= _Save_Native(sFile_Name, xA, yA, xN, yN, true);
		break;

	case GRID_FILE_FORMAT_ASCII:	// 2 - ASCII
		bResult	= _Save_Native(sFile_Name, xA, yA, xN, yN, false);
		break;
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(sFile_Name);

		Save_MetaData(File_Name);

		SG_UI_Msg_Add(LNG("[MSG] okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	}
	else
	{
		SG_UI_Msg_Add(LNG("[MSG] failed"), false, SG_UI_MSG_STYLE_FAILURE);

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
bool CSG_Grid::_Load_Binary(CSG_File &Stream, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes)
{
	char	*Line, *pValue;
	int		x, y, i, iy, dy, nxBytes, nValueBytes;

	if( Stream.is_Open() && is_Valid() )
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
		if( File_Type == SG_DATATYPE_Bit )
		{
			nxBytes		= Get_NX() / 8 + 1;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal )
			{
				for(iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					Stream.Read(m_Values[y], sizeof(char), nxBytes);
				}
			}
			else
			{
				Line	= (char *)SG_Malloc(nxBytes);

				for(iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					Stream.Read(Line, sizeof(char), nxBytes);

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
			nValueBytes	= SG_Data_Type_Get_Size(File_Type);
			nxBytes		= Get_NX() * nValueBytes;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && !bSwapBytes )
			{
				for(iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					Stream.Read(m_Values[y], sizeof(char), nxBytes);
				}
			}
			else
			{
				Line	= (char *)SG_Malloc(nxBytes);

				for(iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
				{
					Stream.Read(Line, sizeof(char), nxBytes);

					for(x=0, pValue=Line; x<Get_NX(); x++, pValue+=nValueBytes)
					{
						if( bSwapBytes )
						{
							_Swap_Bytes(pValue, nValueBytes);
						}

						switch( File_Type )
						{
						default:													break;
						case SG_DATATYPE_Byte:	Set_Value(x, y, *(BYTE   *)pValue);	break;
						case SG_DATATYPE_Char:	Set_Value(x, y, *(char   *)pValue);	break;
						case SG_DATATYPE_Word:	Set_Value(x, y, *(WORD   *)pValue);	break;
						case SG_DATATYPE_Short:	Set_Value(x, y, *(short  *)pValue);	break;
						case SG_DATATYPE_DWord:	Set_Value(x, y, *(DWORD  *)pValue);	break;
						case SG_DATATYPE_Int:		Set_Value(x, y, *(int    *)pValue);	break;
						case SG_DATATYPE_Float:	Set_Value(x, y, *(float  *)pValue);	break;
						case SG_DATATYPE_Double:	Set_Value(x, y, *(double *)pValue);	break;
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
bool CSG_Grid::_Save_Binary(CSG_File &Stream, int xA, int yA, int xN, int yN, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes)
{
	char	*Line, *pValue;
	int		x, y, i, ix, iy, dy, axBytes, nxBytes, nValueBytes;

	//-----------------------------------------------------
	if( Stream.is_Open() && m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined )
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
		if( File_Type == SG_DATATYPE_Bit )
		{
			nxBytes		= xN / 8 + 1;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && xA % 8 == 0 )
			{
				axBytes		= xA / 8;

				for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
				{
					Stream.Write((char *)m_Values[y] + axBytes, sizeof(char), nxBytes);
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

					Stream.Write(Line, sizeof(char), nxBytes);
				}

				SG_Free(Line);
			}
		}

		//-------------------------------------------------
		else
		{
			nValueBytes	= SG_Data_Type_Get_Size(File_Type);
			nxBytes		= xN * nValueBytes;

			if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && !bSwapBytes )
			{
				axBytes	= xA * nValueBytes;

				for(iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
				{
					Stream.Write((char *)m_Values[y] + axBytes, sizeof(char), nxBytes);
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
						case SG_DATATYPE_Byte:	*(BYTE   *)pValue	= asChar	(x, y);	break;
						case SG_DATATYPE_Char:	*(char   *)pValue	= asChar	(x, y);	break;
						case SG_DATATYPE_Word:	*(WORD   *)pValue	= asShort	(x, y);	break;
						case SG_DATATYPE_Short:	*(short  *)pValue	= asShort	(x, y);	break;
						case SG_DATATYPE_DWord:	*(DWORD  *)pValue	= asInt		(x, y);	break;
						case SG_DATATYPE_Int:		*(int    *)pValue	= asInt		(x, y);	break;
						case SG_DATATYPE_Float:	*(float  *)pValue	= asFloat	(x, y);	break;
						case SG_DATATYPE_Double:	*(double *)pValue	= asDouble	(x, y);	break;
						}

						if( bSwapBytes )
						{
							_Swap_Bytes(pValue, nValueBytes);
						}
					}

					Stream.Write(Line, sizeof(char), nxBytes);
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
bool CSG_Grid::_Load_ASCII(CSG_File &Stream, TSG_Grid_Memory_Type Memory_Type, bool bFlip)
{
	int		x, y, iy, dy;
	double	Value;

	if( Stream.is_Open() && m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined && _Memory_Create(Memory_Type) )
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
				SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf"), &Value);

				Set_Value(x, y, Value);
			}
		}

		SG_UI_Process_Set_Ready();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_ASCII(CSG_File &Stream, int xA, int yA, int xN, int yN, bool bFlip)
{
	int		x, y, ix, iy, dy;

	if( Stream.is_Open() && is_Valid() )
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
				Stream.Printf(SG_T("%lf "), asDouble(x, y));
			}

			Stream.Printf(SG_T("\n"));
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
#include "parameters.h"

int SG_Grid_Cache_Check(CSG_Grid_System &m_System, int nValueBytes)
{
	if(	SG_Grid_Cache_Get_Automatic() && m_System.Get_NCells() * nValueBytes > SG_Grid_Cache_Get_Threshold() )
	{
		switch( SG_Grid_Cache_Get_Confirm() )
		{
		default:
			break;

		case 1:
			{
				CSG_String	s;

				s.Printf(SG_T("%s\n%s\n%s: %.2fMB"),
					LNG("Shall I activate file caching for new grid."),
					m_System.Get_Name(),
					LNG("Total memory size"),
					(m_System.Get_NCells() * nValueBytes) / (double)N_MEGABYTE_BYTES
				);

				if( SG_UI_Dlg_Continue(s, LNG("Activate Grid File Cache?")) )
				{
				//	Memory_Type	= GRID_MEMORY_Cache;

					return( SG_Grid_Cache_Get_Threshold() );
				}
			}
			break;

		case 2:
			{
				CSG_Parameters	p(NULL, LNG("Activate Grid File Cache?"), SG_T(""));

				p.Add_Value(
					NULL	, SG_T("BUFFERSIZE")	, LNG("Buffer Size [MB]"),
					SG_T(""),
					PARAMETER_TYPE_Double, SG_Grid_Cache_Get_Threshold_MB(), 0.0, true
				);

				if( SG_UI_Dlg_Parameters(&p, LNG("Activate Grid File Cache?")) )
				{
				//	Memory_Type	= GRID_MEMORY_Cache;

				//	Set_Buffer_Size((int)(p(SG_T("BUFFERSIZE"))->asDouble() * N_MEGABYTE_BYTES));

					return( (int)(p(SG_T("BUFFERSIZE"))->asDouble() * N_MEGABYTE_BYTES) );
				}
			}
			break;
		}
	}

	return( 0 );
}

//---------------------------------------------------------
bool CSG_Grid::_Load_Native(const CSG_String &File_Name, TSG_Grid_Memory_Type Memory_Type)
{
	bool			bResult, hdr_bFlip, hdr_bSwapBytes;
	int				iType, hdr_Offset, NX, NY;
	double			Cellsize, xMin, yMin;
	CSG_File		Stream;
	TSG_Data_Type	hdr_Type;
	CSG_Grid_System	System;
	CSG_String		File_Data, Value;

	//-----------------------------------------------------
	bResult	= false;

	if( Stream.Open(File_Name, SG_FILE_R, false) )
	{
		//-------------------------------------------------
		// Load Header...

		hdr_Type		= SG_DATATYPE_Undefined;
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
					File_Data	= SG_File_Make_Path(SG_File_Get_Path(File_Name), Value);
				}
				break;

			case GRID_FILE_KEY_DATAFORMAT:
				for(iType=0; iType<SG_DATATYPE_Undefined && hdr_Type == SG_DATATYPE_Undefined; iType++)
				{
					if( Value.Find(gSG_Data_Type_Identifier[iType]) >= 0 )
					{
						hdr_Type	= (TSG_Data_Type)iType;
					}
				}
				break;
			}
		}
		while( !Stream.is_EOF() );


		//-------------------------------------------------
		// Load Data...

		if( hdr_Type < SG_DATATYPE_Undefined && m_System.Assign(Cellsize, xMin, yMin, NX, NY) )
		{
			//---------------------------------------------
			// ASCII...

			if( hdr_Type >= SG_DATATYPE_Undefined )
			{
				if( m_Type >= SG_DATATYPE_Undefined )
				{
					m_Type	= SG_DATATYPE_Float;
				}

				if(	Stream.Open(File_Data											, SG_FILE_R, false)
				||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T( "dat"))	, SG_FILE_R, false)
				||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat"))	, SG_FILE_R, false) )
				{
					Stream.Seek(hdr_Offset);
					bResult	= _Load_ASCII(Stream, Memory_Type);
				}
			}

			//---------------------------------------------
			// Binary...

			else
			{
				if( m_Type >= SG_DATATYPE_Undefined )
				{
					m_Type	= hdr_Type;
				}

				if( (NX = SG_Grid_Cache_Check(m_System, Get_nValueBytes())) > 0 )
				{
					Set_Buffer_Size(NX);

					if( _Cache_Create(File_Data											, hdr_Type, hdr_Offset, hdr_bSwapBytes, hdr_bFlip)
					||	_Cache_Create(SG_File_Make_Path(NULL, File_Name, SG_T( "dat"))	, hdr_Type, hdr_Offset, hdr_bSwapBytes, hdr_bFlip)
					||	_Cache_Create(SG_File_Make_Path(NULL, File_Name, SG_T("sdat"))	, hdr_Type, hdr_Offset, hdr_bSwapBytes, hdr_bFlip) )
					{
						return( true );
					}

					Memory_Type	= GRID_MEMORY_Cache;
				}

				if( _Memory_Create(Memory_Type) )
				{
					if(	Stream.Open(File_Data											, SG_FILE_R, true)
					||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T( "dat"))	, SG_FILE_R, true)
					||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat"))	, SG_FILE_R, true) )
					{
						Stream.Seek(hdr_Offset);
						bResult	= _Load_Binary(Stream, hdr_Type, hdr_bFlip, hdr_bSwapBytes);
					}
				}
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Native(const CSG_String &File_Name, int xA, int yA, int xN, int yN, bool bBinary)
{
	bool		bResult		= false;
	CSG_File	Stream;

	if(	Stream.Open(File_Name, SG_FILE_W, false) )
	{
		//-------------------------------------------------
		// Header...

		Stream.Printf(SG_T("%s\t= %s\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_NAME			], Get_Name() );
		Stream.Printf(SG_T("%s\t= %s\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_DESCRIPTION	], Get_Description() );
		Stream.Printf(SG_T("%s\t= %s\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_UNITNAME		], Get_Unit() );
		Stream.Printf(SG_T("%s\t= %d\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_DATAFILE_OFFSET], 0 );
		Stream.Printf(SG_T("%s\t= %s\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_DATAFORMAT		], bBinary ? gSG_Data_Type_Identifier[Get_Type()] : SG_T("ASCII") );
		Stream.Printf(SG_T("%s\t= %s\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_BYTEORDER_BIG	], GRID_FILE_KEY_FALSE );
		Stream.Printf(SG_T("%s\t= %.10f\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_POSITION_XMIN	], Get_XMin() + Get_Cellsize() * xA );
		Stream.Printf(SG_T("%s\t= %.10f\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_POSITION_YMIN	], Get_YMin() + Get_Cellsize() * yA );
		Stream.Printf(SG_T("%s\t= %d\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_CELLCOUNT_X	], xN );
		Stream.Printf(SG_T("%s\t= %d\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_CELLCOUNT_Y	], yN );
		Stream.Printf(SG_T("%s\t= %.10f\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_CELLSIZE		], Get_Cellsize() );
		Stream.Printf(SG_T("%s\t= %f\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_Z_FACTOR		], m_zFactor );
		Stream.Printf(SG_T("%s\t= %f\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_NODATA_VALUE	], m_NoData_Value );
		Stream.Printf(SG_T("%s\t= %s\n")	, gSG_Grid_File_Key_Names[ GRID_FILE_KEY_TOPTOBOTTOM	], GRID_FILE_KEY_FALSE );


		//-------------------------------------------------
		// Data...

		if( Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat")), SG_FILE_W, true) )
		{
			if( bBinary )
			{
				bResult		= _Save_Binary	(Stream, xA, yA, xN, yN, Get_Type(), false, false);
			}
			else
			{
				bResult		= _Save_ASCII	(Stream, xA, yA, xN, yN);
			}
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
int CSG_Grid::_Load_Native_Get_Key(CSG_File &Stream, CSG_String &Value)
{
	int			i;
	CSG_String	sLine;

	if( Stream.Read_Line(sLine) && (i = sLine.Find('=')) > 0 )
	{
		Value	= sLine.AfterFirst('=');
		Value.Trim();

		sLine.Remove(i);

		for(i=0; i<GRID_FILE_KEY_Count; i++)
		{
			CSG_String	s(gSG_Grid_File_Key_Names[i]);

			if( s.Find(sLine.Left(s.Length())) >= 0 )
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
bool CSG_Grid::_Load_Surfer(const CSG_String &File_Name, TSG_Grid_Memory_Type Memory_Type)
{
	bool		bResult		= false;
	char		Identifier[4];
	short		sValue;
	int			x, y, NX, NY;
	float		*fLine;
	double		dValue, xMin, yMin, Cellsize;
	CSG_File	Stream;

	if( Stream.Open(File_Name, SG_FILE_R, true) )
	{
		Stream.Read(Identifier, sizeof(char), 4);

		//-------------------------------------------------
		// Binary...

		if( !strncmp(Identifier, "DSBB", 4) )
		{
			Stream.Read(&sValue	, sizeof(short));
			NX			= sValue;
			Stream.Read(&sValue	, sizeof(short));
			NY			= sValue;

			Stream.Read(&xMin	, sizeof(double));
			Stream.Read(&dValue	, sizeof(double));	// XMax
			Cellsize	= (dValue - xMin) / (NX - 1.0);

			Stream.Read(&yMin	, sizeof(double));
			Stream.Read(&dValue	, sizeof(double));	// YMax...
			//DY		= (dValue - yMin) / (NY - 1.0);		// we could check, if cellsizes (x/y) equal...

			Stream.Read(&dValue	, sizeof(double));	// ZMin...
			Stream.Read(&dValue	, sizeof(double));	// ZMax...

			//---------------------------------------------
			if( !Stream.is_EOF() && Create(SG_DATATYPE_Float, NX, NY, Cellsize, xMin, yMin, Memory_Type) )
			{
				bResult	= true;

				fLine	= (float *)SG_Malloc(Get_NX() * sizeof(float));

				for(y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
				{
					Stream.Read(fLine, sizeof(float), Get_NX());

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
			SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%d %d")	, &NX	, &NY);

			SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf %lf"), &xMin	, &dValue);
			Cellsize	= (dValue - xMin) / (NX - 1.0);

			SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf %lf"), &yMin	, &dValue);
			//DY		= (dValue - yMin) / (NY - 1.0);

			SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf %lf"), &dValue, &dValue);

			//---------------------------------------------
			if( !Stream.is_EOF() && Create(SG_DATATYPE_Float, NX, NY, Cellsize, xMin, yMin, Memory_Type) )
			{
				bResult	= true;

				for(y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
				{
					for(x=0; x<Get_NX(); x++)
					{
						SG_FILE_SCANF(Stream.Get_Stream(), SG_T("%lf"), &dValue);

						Set_Value(x, y, dValue);
					}
				}
			}
		}

		//-------------------------------------------------
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
