/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                   grid_memory.cpp                     //
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <memory.h>

#include "grid.h"
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//					Caching Options						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static CSG_String	gSG_Grid_Cache_Directory;

const SG_Char *		SG_Grid_Cache_Get_Directory(void)
{
	return( gSG_Grid_Cache_Directory );
}

void				SG_Grid_Cache_Set_Directory(const SG_Char *Directory)
{
	if( SG_Dir_Exists(Directory) )
	{
		gSG_Grid_Cache_Directory	= Directory;
	}
}

//---------------------------------------------------------
static int			gSG_Grid_Cache_Mode	= 0;

void				SG_Grid_Cache_Set_Mode(int Mode)
{
	gSG_Grid_Cache_Mode	= Mode;
}

int					SG_Grid_Cache_Get_Mode(void)
{
	return( gSG_Grid_Cache_Mode );
}

//---------------------------------------------------------
static sLong		gSG_Grid_Cache_Threshold	= 0;

void				SG_Grid_Cache_Set_Threshold(int nBytes)
{
	if( nBytes >= 0 )
	{
		gSG_Grid_Cache_Threshold	= nBytes;
	}
}

void				SG_Grid_Cache_Set_Threshold_MB(double nMegabytes)
{
	SG_Grid_Cache_Set_Threshold((int)(nMegabytes * N_MEGABYTE_BYTES));
}

sLong				SG_Grid_Cache_Get_Threshold(void)
{
	return( gSG_Grid_Cache_Threshold );
}

double				SG_Grid_Cache_Get_Threshold_MB(void)
{
	return( (double)gSG_Grid_Cache_Threshold / (double)N_MEGABYTE_BYTES );
}


///////////////////////////////////////////////////////////
//														 //
//						Memory							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Memory_Create(bool bCached)
{
	if( !m_System.is_Valid() || m_Type == SG_DATATYPE_Undefined )
	{
		return( false );
	}

	_Memory_Destroy();

	if(	bCached || _Cache_Check() )
	{
		return( _Cache_Create() );
	}

	return( _Array_Create() );
}

//---------------------------------------------------------
void CSG_Grid::_Memory_Destroy(void)
{
	SG_FREE_SAFE(m_Index);

	if( is_Cached() )
	{
		_Cache_Destroy(false);
	}

	_Array_Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//						Array							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Array_Create(void)
{
	if( m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined )
	{
		_Array_Destroy();

		if( (m_Values = (void **)SG_Malloc(Get_NY() * sizeof(void *))) != NULL )
		{		
			if( (m_Values[0] = (void  *)SG_Calloc(Get_NY(), Get_nLineBytes())) != NULL )
			{
				char	*pLine	= (char *)m_Values[0];

				for(int y=0; y<Get_NY(); y++, pLine+=Get_nLineBytes())
				{
					m_Values[y]	 = pLine;
				}

				return( true );
			}

			SG_Free(m_Values);

			m_Values	= NULL;
		}

		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s [%.2fmb]", _TL("grid"), _TL("memory allocation failed"), Get_NY() * Get_nLineBytes() / (double)N_MEGABYTE_BYTES));
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Grid::_Array_Destroy(void)
{
	if( m_Values )
	{
		SG_Free(m_Values[0]);
		SG_Free(m_Values);

		m_Values	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Cache							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CACHE_FILE_POS(x, y)	(m_Cache_Offset + (m_Cache_bFlip ? Get_NY() - 1 - y : y) * m_nBytes_Line + x * m_nBytes_Value)

//---------------------------------------------------------
#if defined(_SAGA_LINUX)
#define CACHE_FILE_SEEK	fseek
#else
#define CACHE_FILE_SEEK	_fseeki64
#endif

//---------------------------------------------------------
bool CSG_Grid::Set_Cache(bool bOn)
{
	if( bOn )
	{
		return( is_Cached()
			|| _Cache_Create(m_Cache_File                                  , m_Type, m_Cache_Offset, m_Cache_bSwap, m_Cache_bFlip)
			|| _Cache_Create(SG_File_Make_Path("", Get_File_Name(),  "dat"), m_Type, m_Cache_Offset, m_Cache_bSwap, m_Cache_bFlip)
			|| _Cache_Create(SG_File_Make_Path("", Get_File_Name(), "sdat"), m_Type, m_Cache_Offset, m_Cache_bSwap, m_Cache_bFlip)
			|| _Cache_Create()
		);
	}

	return( !is_Cached() || _Cache_Destroy(true) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Cache_Check(void)
{
	sLong	nBytes	= m_System.Get_NCells() * Get_nValueBytes();

	if(	SG_Grid_Cache_Get_Mode() > 0 && nBytes > SG_Grid_Cache_Get_Threshold() )
	{
		if( SG_Grid_Cache_Get_Mode() == 2 )	// confirm
		{
			CSG_String	s;

			s.Printf("%s\n%s\n%s: %.2fMB",
				_TL("Shall I activate file caching for new grid."),
				m_System.Get_Name(),
				_TL("Total memory size"),
				(double)nBytes / (double)N_MEGABYTE_BYTES
			);

			return( SG_UI_Dlg_Continue(s, _TL("Activate Grid File Cache?")) );
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Cache_Create(const CSG_String &File, TSG_Data_Type Data_Type, sLong Offset, bool bSwap, bool bFlip)	// open existing file for caching
{
	if( !m_System.is_Valid() || m_Type != Data_Type || !SG_File_Exists(File) )
	{
		return( false );
	}

	if( (m_Cache_Stream = fopen(File, "r+b")) == NULL	// read and write
	&&  (m_Cache_Stream = fopen(File, "rb" )) == NULL )	// read only
	{
		return( false );
	}

	m_Cache_File	= File;
	m_Cache_bTemp	= false;
	m_Cache_Offset	= Offset;
	m_Cache_bSwap	= m_Type == SG_DATATYPE_Bit ? false : bSwap;
	m_Cache_bFlip	= bFlip;

	_Array_Destroy();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Cache_Create(void)	// create temporary cache file
{
	if( !m_System.is_Valid() || m_Type == SG_DATATYPE_Undefined && is_Cached() )
	{
		return( false );
	}

	CSG_String	File	= SG_File_Get_Name_Temp("sg_grd", SG_Grid_Cache_Get_Directory());

	if( (m_Cache_Stream = fopen(File, "w+b")) == NULL )	// read and write, create empty
	{
		return( false );
	}

	m_Cache_File	= File;
	m_Cache_bTemp	= true;
	m_Cache_Offset	= 0;
	m_Cache_bSwap	= false;
	m_Cache_bFlip	= false;

	CSG_Array	Values(1, m_Values ? 0 : Get_nLineBytes());	// dummy

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		fwrite(m_Values ? m_Values[y] : Values.Get_Array(), 1, Get_nLineBytes(), m_Cache_Stream);
	}

	SG_UI_Process_Set_Ready();

	_Array_Destroy();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Cache_Destroy(bool bMemory_Restore)
{
	if( is_Cached() )
	{
		if( bMemory_Restore && _Array_Create() && !CACHE_FILE_SEEK(m_Cache_Stream, m_Cache_Offset, SEEK_SET) )
		{
			for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				fread(m_Values[m_Cache_bFlip ? Get_NY() - 1 - y : y], 1, Get_nLineBytes(), m_Cache_Stream);

				if( m_Cache_bSwap )
				{
					char	*pValue	= (char *)m_Values[y];

					for(int x=0; x<Get_NX(); x++, pValue+=Get_nValueBytes())
					{
						_Swap_Bytes(pValue, Get_nValueBytes());
					}
				}
			}

			SG_UI_Process_Set_Ready();
		}

		//-------------------------------------------------
		fclose(m_Cache_Stream);

		m_Cache_Stream	= NULL;

		if( m_Cache_bTemp )
		{
			SG_File_Delete(m_Cache_File);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_Cache_Set_Value(int x, int y, double Value)
{
	char	Buffer[8];

	switch( m_Type )
	{
	case SG_DATATYPE_Float : (*(float  *)Buffer) = (float          )(Value); break;
	case SG_DATATYPE_Double: (*(double *)Buffer) = (double         )(Value); break;
	case SG_DATATYPE_Byte  : (*(BYTE   *)Buffer) = SG_ROUND_TO_BYTE (Value); break;
	case SG_DATATYPE_Char  : (*(char   *)Buffer) = SG_ROUND_TO_CHAR (Value); break;
	case SG_DATATYPE_Word  : (*(WORD   *)Buffer) = SG_ROUND_TO_WORD (Value); break;
	case SG_DATATYPE_Short : (*(short  *)Buffer) = SG_ROUND_TO_SHORT(Value); break;
	case SG_DATATYPE_DWord : (*(DWORD  *)Buffer) = SG_ROUND_TO_DWORD(Value); break;
	case SG_DATATYPE_Int   : (*(int    *)Buffer) = SG_ROUND_TO_INT  (Value); break;
	case SG_DATATYPE_Long  : (*(sLong  *)Buffer) = SG_ROUND_TO_SLONG(Value); break;

	default:
		return;
	}

	if( m_Cache_bSwap )
	{
		_Swap_Bytes(Buffer, Get_nValueBytes());
	}

	if( !CACHE_FILE_SEEK(m_Cache_Stream, CACHE_FILE_POS(x, y), SEEK_SET) )
	{
		fwrite(Buffer, 1, Get_nValueBytes(), m_Cache_Stream);
	}
}

//---------------------------------------------------------
double CSG_Grid::_Cache_Get_Value(int x, int y) const
{
	if( !CACHE_FILE_SEEK(m_Cache_Stream, CACHE_FILE_POS(x, y), SEEK_SET) )
	{
		char	Buffer[8];

		if( fread(Buffer, 1, Get_nValueBytes(), m_Cache_Stream) == (size_t)Get_nValueBytes() )
		{
			switch( m_Type )
			{
			case SG_DATATYPE_Byte  : return( (double)(*(BYTE   *)Buffer) );
			case SG_DATATYPE_Char  : return( (double)(*(char   *)Buffer) );
			case SG_DATATYPE_Word  : return( (double)(*(WORD   *)Buffer) );
			case SG_DATATYPE_Short : return( (double)(*(short  *)Buffer) );
			case SG_DATATYPE_DWord : return( (double)(*(DWORD  *)Buffer) );
			case SG_DATATYPE_Int   : return( (double)(*(int    *)Buffer) );
			case SG_DATATYPE_Long  : return( (double)(*(sLong  *)Buffer) );
			case SG_DATATYPE_Float : return( (double)(*(float  *)Buffer) );
			case SG_DATATYPE_Double: return( (double)(*(double *)Buffer) );

			default:
				break;
			}

			if( m_Cache_bSwap )
			{
				_Swap_Bytes(Buffer, Get_nValueBytes());
			}
		}
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
