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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
static bool			gSG_Grid_Cache_bAutomatic	= false;

void				SG_Grid_Cache_Set_Automatic(bool bOn)
{
	gSG_Grid_Cache_bAutomatic	= bOn;
}

bool				SG_Grid_Cache_Get_Automatic(void)
{
	return( gSG_Grid_Cache_bAutomatic );
}

//---------------------------------------------------------
static int			gSG_Grid_Cache_Confirm		= 2;

void				SG_Grid_Cache_Set_Confirm(int Confirm)
{
	gSG_Grid_Cache_Confirm		= Confirm;
}

int					SG_Grid_Cache_Get_Confirm(void)
{
	return( gSG_Grid_Cache_Confirm );
}

//---------------------------------------------------------
static sLong		gSG_Grid_Cache_Threshold	= 250 * N_MEGABYTE_BYTES;

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
bool CSG_Grid::_Memory_Create(TSG_Grid_Memory_Type Memory_Type)
{
	if( m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined )
	{
		_Memory_Destroy();

		Set_Buffer_Size(gSG_Grid_Cache_Threshold);

		if(	Memory_Type != GRID_MEMORY_Cache && gSG_Grid_Cache_bAutomatic && Get_Memory_Size() > gSG_Grid_Cache_Threshold )
		{
			switch( gSG_Grid_Cache_Confirm )
			{
			default:
				Memory_Type	= GRID_MEMORY_Cache;
				break;

			case 1:
				{
					CSG_String	s;

					s.Printf(SG_T("%s\n%s\n%s: %.2fMB"),
						_TL("Shall I activate file caching for new grid."),
						m_System.Get_Name(),
						_TL("Total memory size"),
						Get_Memory_Size_MB()
					);

					if( SG_UI_Dlg_Continue(s, _TL("Activate Grid File Cache?")) )
					{
						Memory_Type	= GRID_MEMORY_Cache;
					}
				}
				break;

			case 2:
				{
					CSG_Parameters	p(NULL, _TL("Activate Grid File Cache?"), SG_T(""));

					p.Add_Double("", "BUFFERSIZE", _TL("Buffer Size [MB]"), "",
						SG_Grid_Cache_Get_Threshold_MB(), 0.0, true
					);

					if( SG_UI_Dlg_Parameters(&p, _TL("Activate Grid File Cache?")) )
					{
						Memory_Type	= GRID_MEMORY_Cache;

						Set_Buffer_Size((sLong)(p(SG_T("BUFFERSIZE"))->asDouble() * N_MEGABYTE_BYTES));
					}
				}
				break;
			}
		}

		//-------------------------------------------------
		switch( Memory_Type )
		{
		case GRID_MEMORY_Normal:
			return( _Array_Create() );

		case GRID_MEMORY_Cache:
			return( _Cache_Create() );

		case GRID_MEMORY_Compression:
			return( _Compr_Create() );
		}
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Grid::_Memory_Destroy(void)
{
	m_bIndex	= false;
	SG_FREE_SAFE(m_Index);

	switch( m_Memory_Type )
	{
	case GRID_MEMORY_Normal:		_Array_Destroy();		break;
	case GRID_MEMORY_Cache:			_Cache_Destroy(false);	break;
	case GRID_MEMORY_Compression:	_Compr_Destroy(false);	break;
	}

	_LineBuffer_Destroy();

	m_Memory_Type	= GRID_MEMORY_Normal;
}


///////////////////////////////////////////////////////////
//														 //
//						LineBuffer						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_LineBuffer_Create(void)
{
	_LineBuffer_Destroy();

	m_LineBuffer	= (TSG_Grid_Line *)SG_Malloc(m_LineBuffer_Count * sizeof(TSG_Grid_Line));

	for(int i=0; i<m_LineBuffer_Count; i++)
	{
		m_LineBuffer[i].Data		= (char *)SG_Malloc(Get_nLineBytes());
		m_LineBuffer[i].y			= -1;
		m_LineBuffer[i].bModified	= false;
	}
}

//---------------------------------------------------------
void CSG_Grid::_LineBuffer_Destroy(void)
{
	if( m_LineBuffer )
	{
		for(int i=0; i<m_LineBuffer_Count; i++)
		{
			if( m_LineBuffer[i].Data )
			{
				SG_Free(m_LineBuffer[i].Data);
			}
		}

		SG_FREE_SAFE(m_LineBuffer);
	}
}

//---------------------------------------------------------
bool CSG_Grid::Set_Buffer_Size(sLong nBytes)
{
	if( m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined )
	{
		int	nLines	= (int)(nBytes / Get_nLineBytes());

		if( nLines < 1 )
		{
			nLines	= 1;
		}
		else if( nLines >= Get_NY() )
		{
			nLines	= Get_NY() - 1;
		}

		if( nLines != m_LineBuffer_Count )
		{
			if( m_LineBuffer )
			{
				if( nLines > m_LineBuffer_Count )
				{
					m_LineBuffer	= (TSG_Grid_Line *)SG_Realloc(m_LineBuffer, nLines * sizeof(TSG_Grid_Line));

					for(int i=m_LineBuffer_Count; i<nLines; i++)
					{
						m_LineBuffer[i].Data		= (char *)SG_Malloc(Get_nLineBytes());
						m_LineBuffer[i].y			= -1;
						m_LineBuffer[i].bModified	= false;
					}
				}
				else
				{
					for(int i=nLines; i<m_LineBuffer_Count; i++)
					{
						if( m_LineBuffer[i].Data )
						{
							SG_Free(m_LineBuffer[i].Data);
						}
					}

					m_LineBuffer	= (TSG_Grid_Line *)SG_Realloc(m_LineBuffer, nLines * sizeof(TSG_Grid_Line));
				}
			}

			m_LineBuffer_Count	= nLines;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Grid::_LineBuffer_Flush(void)
{
	if( m_LineBuffer )
	{
		for(int i=0; i<m_LineBuffer_Count; i++)
		{
			switch( m_Memory_Type )
			{
		    default:
		        break;

			case GRID_MEMORY_Cache:
				_Cache_LineBuffer_Save(m_LineBuffer + i);
				break;

			case GRID_MEMORY_Compression:
				_Compr_LineBuffer_Save(m_LineBuffer + i);
				break;
			}
		}
	}
}

//---------------------------------------------------------
CSG_Grid::TSG_Grid_Line * CSG_Grid::_LineBuffer_Get_Line(int y) const
{
	int				i, iLine;
	TSG_Grid_Line	tmp_Line;

	if( m_LineBuffer && y >= 0 && y < Get_NY() )
	{
		//-------------------------------------------------
		if( y != m_LineBuffer[0].y )
		{
			for(i=1, iLine=0; i<m_LineBuffer_Count && !iLine; i++)
			{
				if( y == m_LineBuffer[i].y )
				{
					iLine	= i;
				}
			}

			//---------------------------------------------
			if( !iLine )
			{
				iLine	= m_LineBuffer_Count - 1;

				switch( m_Memory_Type )
				{
				default:
					break;

				case GRID_MEMORY_Cache:
					_Cache_LineBuffer_Save(m_LineBuffer + iLine);
					_Cache_LineBuffer_Load(m_LineBuffer + iLine, y);
					break;

				case GRID_MEMORY_Compression:
					_Compr_LineBuffer_Save(m_LineBuffer + iLine);
					_Compr_LineBuffer_Load(m_LineBuffer + iLine, y);
					break;
				}
			}

			//---------------------------------------------
			tmp_Line		= m_LineBuffer[iLine];

			for(i=iLine; i>0; i--)
			{
				m_LineBuffer[i]	= m_LineBuffer[i - 1];
			}	// memmove(m_LineBuffer + 1, m_LineBuffer, (iLine - 1) * sizeof(TSG_Grid_Line));

			m_LineBuffer[0]	= tmp_Line;
		}

		//-------------------------------------------------
		return( m_LineBuffer );
	}

	return( NULL );
}

//---------------------------------------------------------
void CSG_Grid::_LineBuffer_Set_Value(int x, int y, double Value)
{
	TSG_Grid_Line	*pLine;

	if( (pLine = _LineBuffer_Get_Line(y)) != NULL )
	{
		switch( m_Type )
		{
		case SG_DATATYPE_Byte  :	((BYTE   *)pLine->Data)[x]	= (BYTE  )Value;	break;
		case SG_DATATYPE_Char  :	((char   *)pLine->Data)[x]	= (char  )Value;	break;
		case SG_DATATYPE_Word  :	((WORD   *)pLine->Data)[x]	= (WORD  )Value;	break;
		case SG_DATATYPE_Short :	((short  *)pLine->Data)[x]	= (short )Value;	break;
		case SG_DATATYPE_DWord :	((DWORD  *)pLine->Data)[x]	= (DWORD )Value;	break;
		case SG_DATATYPE_Int   :	((int    *)pLine->Data)[x]	= (int   )Value;	break;
		case SG_DATATYPE_Long  :	((sLong  *)pLine->Data)[x]	= (int   )Value;	break;
		case SG_DATATYPE_Float :	((float  *)pLine->Data)[x]	= (float )Value;	break;
		case SG_DATATYPE_Double:	((double *)pLine->Data)[x]	= (double)Value;	break;
		default:	break;
		}

		pLine->bModified	= true;
	}
}

//---------------------------------------------------------
double CSG_Grid::_LineBuffer_Get_Value(int x, int y) const
{
	TSG_Grid_Line	*pLine;

	if( (pLine = _LineBuffer_Get_Line(y)) != NULL )
	{
		switch( m_Type )
		{
		case SG_DATATYPE_Byte  :	return( (double)((BYTE   *)pLine->Data)[x] );
		case SG_DATATYPE_Char  :	return( (double)((char   *)pLine->Data)[x] );
		case SG_DATATYPE_Word  :	return( (double)((WORD   *)pLine->Data)[x] );
		case SG_DATATYPE_Short :	return( (double)((short  *)pLine->Data)[x] );
		case SG_DATATYPE_DWord :	return( (double)((DWORD  *)pLine->Data)[x] );
		case SG_DATATYPE_Int   :	return( (double)((int    *)pLine->Data)[x] );
		case SG_DATATYPE_Long  :	return( (double)((sLong  *)pLine->Data)[x] );
		case SG_DATATYPE_Float :	return( (double)((float  *)pLine->Data)[x] );
		case SG_DATATYPE_Double:	return( (double)((double *)pLine->Data)[x] );
		default:	break;
		}
	}

	return( 0.0 );
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
			if( (m_Values[0] = (void  *)SG_Calloc(Get_NY(), _Get_nLineBytes())) != NULL )
			{
				char	*pLine	= (char *)m_Values[0];

				for(int y=1; y<Get_NY(); y++)
				{
					pLine		+=_Get_nLineBytes();
					m_Values[y]	 = pLine;
				}

				return( true );
			}

			SG_Free(m_Values);

			m_Values	= NULL;
		}

		SG_UI_Msg_Add_Error(CSG_String::Format(SG_T("%s: %s [%.2fmb]"), _TL("grid"), _TL("memory allocation failed"), Get_NY() * _Get_nLineBytes() / (double)N_MEGABYTE_BYTES));
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
bool CSG_Grid::Set_Cache(bool bOn)
{
	return( bOn ? _Cache_Create() : _Cache_Destroy(true) );
}


///////////////////////////////////////////////////////////
//														 //
//				Cache: Create / Destroy					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Cache_Create(const SG_Char *FilePath, TSG_Data_Type File_Type, sLong Offset, bool bSwap, bool bFlip)
{
	if( m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined && m_Memory_Type == GRID_MEMORY_Normal )
	{
		m_Cache_Path	= FilePath;

		if( m_Type == File_Type
		&&	(	m_Cache_Stream.Open(m_Cache_Path, SG_FILE_RW, true)
			||	m_Cache_Stream.Open(m_Cache_Path, SG_FILE_R , true)) )
		{
			m_Memory_bLock	= true;

			m_Cache_bTemp	= false;

			m_Cache_Offset	= Offset;
			m_Cache_bSwap	= bSwap;
			m_Cache_bFlip	= bFlip;

			_LineBuffer_Create();

			m_Memory_bLock	= false;
			m_Memory_Type	= GRID_MEMORY_Cache;

			_Array_Destroy();
		}
	}

	return( is_Cached() );
}

//---------------------------------------------------------
bool CSG_Grid::_Cache_Create(void)
{
	TSG_Grid_Line	Line;

	if( m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined && m_Memory_Type == GRID_MEMORY_Normal )
	{
		m_Cache_Path	= SG_File_Get_Name_Temp("sg_grd", SG_Grid_Cache_Get_Directory());

		if( m_Cache_Stream.Open(m_Cache_Path, SG_FILE_RW, true) )
		{
			m_Memory_bLock	= true;

			m_Cache_bTemp	= true;

			m_Cache_Offset	= 0;
			m_Cache_bSwap	= false;
			m_Cache_bFlip	= false;

			_LineBuffer_Create();

			if( m_Values )
			{
				Line.Data	= (char *)SG_Malloc(Get_nLineBytes());

				for(Line.y=0; Line.y<Get_NY() && SG_UI_Process_Set_Progress(Line.y, Get_NY()); Line.y++)
				{
					Line.bModified	= true;
					memcpy(Line.Data, m_Values[Line.y], Get_nLineBytes());
					_Cache_LineBuffer_Save(&Line);
				}

				SG_Free(Line.Data);

				_Array_Destroy();

				SG_UI_Process_Set_Ready();
			}

			m_Memory_bLock	= false;
			m_Memory_Type	= GRID_MEMORY_Cache;
		}
	}

	return( is_Cached() );
}

//---------------------------------------------------------
bool CSG_Grid::_Cache_Destroy(bool bMemory_Restore)
{
	int				y;
	TSG_Grid_Line	*pLine;

	if( is_Valid() && m_Memory_Type == GRID_MEMORY_Cache )
	{
		m_Memory_bLock	= true;

		if( !m_Cache_bTemp )
		{
			_LineBuffer_Flush();
		}

		if( bMemory_Restore && _Array_Create() )
		{
			for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				if( (pLine = _LineBuffer_Get_Line(y)) != NULL )
				{
					memcpy(m_Values[y], pLine->Data, Get_nLineBytes());
				}
			}

			SG_UI_Process_Set_Ready();
		}

		_LineBuffer_Destroy();

		m_Memory_bLock	= false;
		m_Memory_Type	= GRID_MEMORY_Normal;

		//-------------------------------------------------
		m_Cache_Stream.Close();

		if( m_Cache_bTemp )
		{
			SG_File_Delete(m_Cache_Path);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//					Cache: Save / Load					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_Cache_LineBuffer_Save(TSG_Grid_Line *pLine) const
{
	if( pLine && pLine->bModified )
	{
		pLine->bModified	= false;

		if( pLine->y >= 0 && pLine->y < Get_NY() )
		{
			sLong	Line_Y		= m_Cache_bFlip ? Get_NY() - 1 - pLine->y : pLine->y;
			sLong	Line_Size	= Get_nLineBytes();
			sLong	Line_Pos	= m_Cache_Offset + Line_Y * Line_Size;

			//-------------------------------------------------
			if( m_Cache_bSwap && m_Type != SG_DATATYPE_Bit )
			{
				char	*pValue	= pLine->Data;

				for(int x=0; x<Get_NX(); x++, pValue+=Get_nValueBytes())
				{
					_Swap_Bytes(pValue, Get_nValueBytes());
				}
			}

			m_Cache_Stream.Seek(Line_Pos);
			m_Cache_Stream.Write(pLine->Data, sizeof(char), (size_t)Line_Size);

			if( m_Cache_bSwap && m_Type != SG_DATATYPE_Bit )
			{
				char	*pValue	= pLine->Data;

				for(int x=0; x<Get_NX(); x++, pValue+=Get_nValueBytes())
				{
					_Swap_Bytes(pValue, Get_nValueBytes());
				}
			}
		}
	}
}

//---------------------------------------------------------
void CSG_Grid::_Cache_LineBuffer_Load(TSG_Grid_Line *pLine, int y) const
{
	if( pLine )
	{
		pLine->bModified	= false;
		pLine->y			= y;

		if( pLine->y >= 0 && pLine->y < Get_NY() )
		{
			sLong	Line_Y		= m_Cache_bFlip ? Get_NY() - 1 - pLine->y : pLine->y;
			sLong	Line_Size	= Get_nLineBytes();
			sLong	Line_Pos	= m_Cache_Offset + Line_Y * Line_Size;

			//-------------------------------------------------
			m_Cache_Stream.Seek(Line_Pos);
			m_Cache_Stream.Read(pLine->Data, sizeof(char), (size_t)Line_Size);

			if( m_Cache_bSwap && m_Type != SG_DATATYPE_Bit )
			{
				char	*pValue	= pLine->Data;

				for(int x=0; x<Get_NX(); x++, pValue+=Get_nValueBytes())
				{
					_Swap_Bytes(pValue, Get_nValueBytes());
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//					RTL - Compression					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Set_Compression(bool bOn)
{
	return( bOn ? _Compr_Create() : _Compr_Destroy(true) );
}

//---------------------------------------------------------
double CSG_Grid::Get_Compression_Ratio(void) const
{
	if( is_Valid() && is_Compressed() )
	{
		sLong	nCompressed	= 0;

		for(int y=0; y<Get_NY(); y++)
		{
			nCompressed	+= *((int *)m_Values[y]);
		}

		return( (double)nCompressed / (double)(Get_NCells() * Get_nValueBytes()) );
	}

	return( 1.0 );
}


///////////////////////////////////////////////////////////
//														 //
//			RTL - Compression: Create / Destroy			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Compr_Create(void)
{
	TSG_Grid_Line	Line;

	if( m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined && m_Memory_Type == GRID_MEMORY_Normal )
	{
		m_Memory_bLock	= true;

		Line.Data		= (char *)SG_Calloc(1, _Get_nLineBytes());

		if( m_Values )	// compress loaded data...
		{
			void	**Values	= m_Values;
			
			m_Values	= (void **)SG_Calloc(Get_NY(), sizeof(void *));

			for(Line.y=0; Line.y<Get_NY() && SG_UI_Process_Set_Progress(Line.y, Get_NY()); Line.y++)
			{
				memcpy(Line.Data, Values[Line.y], Get_nLineBytes());
				Line.bModified	= true;
				_Compr_LineBuffer_Save(&Line);
			}

			SG_Free(Values[0]);
			SG_Free(Values);
		}
		else			// create empty grid...
		{
			m_Values	= (void **)SG_Malloc(Get_NY() * sizeof(void *));

			for(Line.y=0; Line.y<Get_NY() && SG_UI_Process_Set_Progress(Line.y, Get_NY()); Line.y++)
			{
				m_Values[Line.y]	= (void *)SG_Calloc(1, Get_nLineBytes());
				Line.bModified	= true;
				_Compr_LineBuffer_Save(&Line);
			}
		}

		SG_Free(Line.Data);

		_LineBuffer_Create();

		m_Memory_bLock	= false;
		m_Memory_Type	= GRID_MEMORY_Compression;

		SG_UI_Process_Set_Ready();
	}

	return( is_Compressed() );
}

//---------------------------------------------------------
bool CSG_Grid::_Compr_Destroy(bool bMemory_Restore)
{
	TSG_Grid_Line	Line;

	if( !is_Valid() || m_Memory_Type != GRID_MEMORY_Compression )
	{
		return( false );
	}

	m_Memory_bLock	= true;

	//-----------------------------------------------------
	if( !bMemory_Restore )
	{
		_Array_Destroy();
	}
	else
	{
		_LineBuffer_Flush();

		void	**vCompr	= m_Values;

		m_Values	= NULL;

		if( !_Array_Create() )
		{
			m_Values		= vCompr;
			m_Memory_bLock	= false;

			return( false );
		}

		void	**vArray	= m_Values;

		m_Values	= vCompr;
		Line.Data	= (char *)SG_Calloc(1, _Get_nLineBytes());

		for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			_Compr_LineBuffer_Load(&Line, y);
			memcpy(vArray[y], Line.Data, Get_nLineBytes());
			SG_Free(vCompr[y]);
		}

		m_Values	= vArray;

		SG_Free(vCompr);

		SG_Free(Line.Data);

		SG_UI_Process_Set_Ready();
	}

	//-----------------------------------------------------
	_LineBuffer_Destroy();

	m_Memory_bLock	= false;
	m_Memory_Type	= GRID_MEMORY_Normal;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//			RTL - Compression: Save / Load				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_Compr_LineBuffer_Save(TSG_Grid_Line *pLine) const
{
	char	*pResult, *pNoCompress, *pCompress, *pNext;
	int		x, ix, Threshold, nCompress, nNoCompress, nBytesTotal, Line_Pos;

	if( pLine && pLine->bModified )
	{
		pLine->bModified	= false;

		if( pLine->y >= 0 && pLine->y < Get_NY() )
		{
			Threshold	= 1 + (sizeof(WORD) + sizeof(bool) + Get_nValueBytes()) / Get_nValueBytes();

			nBytesTotal	= Line_Pos	= sizeof(nBytesTotal);
			pResult		= (char *)SG_Malloc(nBytesTotal);
			pNoCompress	= pLine->Data;
			nNoCompress	= 0;

			//---------------------------------------------
			for(x=0; x<Get_NX(); )
			{
				nCompress	= 1;
				pCompress	= pLine->Data + x * Get_nValueBytes();
				pNext		= pCompress + Get_nValueBytes();

				for(ix=x+1; ix<Get_NX(); ix++, pNext+=Get_nValueBytes())
				{
					if( memcmp(pCompress, pNext, Get_nValueBytes()) )
					{
						break;
					}
					else
					{
						nCompress++;
					}
				}

				//-----------------------------------------
				if( nCompress <= Threshold )
				{
					nNoCompress++;
					x++;
				}
				else
				{
					nBytesTotal	+= sizeof(WORD) + sizeof(bool) + Get_nValueBytes();

					if( nNoCompress > 0 )
					{
						nBytesTotal	+= sizeof(WORD) + sizeof(bool) + nNoCompress * Get_nValueBytes();
						pResult	= (char *)SG_Realloc(pResult, nBytesTotal);

						*((WORD *)(pResult + Line_Pos))	= nNoCompress;
						Line_Pos	+= sizeof(WORD);
						*((bool *)(pResult + Line_Pos))	= false;
						Line_Pos	+= sizeof(bool);

						memcpy(pResult + Line_Pos, pNoCompress, nNoCompress * Get_nValueBytes());
						Line_Pos	+= nNoCompress * Get_nValueBytes();

						nNoCompress	= 0;
					}
					else
					{
						pResult	= (char *)SG_Realloc(pResult, nBytesTotal);
					}

					*((WORD *)(pResult + Line_Pos))	= nCompress;
					Line_Pos	+= sizeof(WORD);
					*((bool *)(pResult + Line_Pos))	= true;
					Line_Pos	+= sizeof(bool);

					memcpy(pResult + Line_Pos, pCompress, Get_nValueBytes());
					Line_Pos	+= Get_nValueBytes();

					pNoCompress	= pCompress + nCompress * Get_nValueBytes();
					x			+= nCompress;
				}
			}

			//---------------------------------------------
			if( nNoCompress > 0 )
			{
				nBytesTotal	+= sizeof(WORD) + sizeof(bool) + nNoCompress * Get_nValueBytes();
				pResult	= (char *)SG_Realloc(pResult, nBytesTotal);

				*((WORD *)(pResult + Line_Pos))	= nNoCompress;
				Line_Pos	+= sizeof(WORD);
				*((bool *)(pResult + Line_Pos))	= false;
				Line_Pos	+= sizeof(bool);

				memcpy(pResult + Line_Pos, pNoCompress, nNoCompress * Get_nValueBytes());
			}

			//-------------------------------------------------
			memcpy(pResult, &nBytesTotal, sizeof(nBytesTotal));

			if( m_Values[pLine->y] )
			{
				SG_Free(m_Values[pLine->y]);
			}

			m_Values[pLine->y]	= pResult;
		}
	}
}

//---------------------------------------------------------
void CSG_Grid::_Compr_LineBuffer_Load(TSG_Grid_Line *pLine, int y) const
{
	bool	bCompressed;
	char	*pData, *pValue;
	int		x, iValue;
	WORD	nValues;

	if( pLine )
	{
		pLine->bModified	= false;
		pLine->y			= y;

		if( pLine->y >= 0 && pLine->y < Get_NY() )
		{
			pValue		= (char *)m_Values[y] + sizeof(int);
			pData		= pLine->Data;

			for(x=0; x<Get_NX(); )
			{
				nValues		= *((WORD *)pValue);
				pValue		+= sizeof(WORD);
				bCompressed	= *((bool *)pValue);
				pValue		+= sizeof(bool);

				if( bCompressed )
				{
					for(iValue=0; iValue<nValues && x<Get_NX(); iValue++, x++, pData+=Get_nValueBytes())
					{
						memcpy(pData, pValue, Get_nValueBytes());
					}

					pValue	+= Get_nValueBytes();
				}
				else
				{
					memcpy(pData, pValue, Get_nValueBytes() * nValues);
					x		+= nValues;
					pData	+= Get_nValueBytes() * nValues;
					pValue	+= Get_nValueBytes() * nValues;
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
