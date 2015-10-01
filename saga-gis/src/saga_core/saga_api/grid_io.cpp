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
//				Grid: File Operations					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <stdint.h>
#include <string.h>

#ifdef _SAGA_LINUX
#include "config.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#include <WinSock2.h>
#endif

#include "grid.h"

#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type, bool bLoadData)
{
	m_Type	= Type;

	//-----------------------------------------------------
	if( _Load_Native(File_Name, Memory_Type, bLoadData) )
	{
		Load_MetaData(File_Name);

		Set_File_Name(File_Name, true);

		return( true );
	}

	if( SG_File_Cmp_Extension(File_Name, SG_T("sgrd")) || SG_File_Cmp_Extension(File_Name, SG_T("dgm")) )
	{	// couldn't load saga raster ??? then return immediately !!!
		return( false );
	}

	Set_File_Name(File_Name, false);

	//-----------------------------------------------------
	if( _Load_Surfer(File_Name, Memory_Type, bLoadData) )
	{
		return( true );
	}

	//-----------------------------------------------------
	CSG_Data_Manager	tmpMgr;

	if( tmpMgr.Add(File_Name) && tmpMgr.Get_Grid_System(0) && tmpMgr.Get_Grid_System(0)->Get(0) && tmpMgr.Get_Grid_System(0)->Get(0)->is_Valid() )
	{
		CSG_Grid	*pGrid	= (CSG_Grid *)tmpMgr.Get_Grid_System(0)->Get(0);

		if( pGrid->is_Cached() || pGrid->is_Compressed() )
		{
			return( Create(*pGrid) );
		}

		Set_Name			(pGrid->Get_Name());
		Set_Description		(pGrid->Get_Description());

		m_System			= pGrid->m_System;
		m_Type				= pGrid->m_Type;
		m_Values			= pGrid->m_Values;	pGrid->m_Values	= NULL;	// take ownership of data array

		m_zOffset			= pGrid->m_zOffset;
		m_zScale			= pGrid->m_zScale;
		m_Unit				= pGrid->m_Unit;

		Get_MetaData  ()	= pGrid->Get_MetaData  ();
		Get_Projection()	= pGrid->Get_Projection();

		Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
		
		return( true );
	}

	//-----------------------------------------------------
	return( false );
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
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s: %s..."), _TL("Save grid"), File_Name.c_str()), true);

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

		Set_File_Name(sFile_Name, true);

		Save_MetaData(File_Name);

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	}
	else
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

		SG_UI_Msg_Add_Error(_TL("Grid file could not be saved."));
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
	if( nBytes == 2 ) 
	{
		uint16_t val, valSwapped;
		memcpy(&val, Bytes, nBytes);
		valSwapped = ntohs(val);
		memcpy(Bytes, &valSwapped, nBytes);
	} 
	else if( nBytes == 4 )
	{
		uint32_t val, valSwapped;

		memcpy(&val, Bytes, nBytes);
		valSwapped = ntohl(val);
		memcpy(Bytes, &valSwapped, nBytes);
	}
}

//---------------------------------------------------------
bool CSG_Grid::_Load_Binary(CSG_File &Stream, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes)
{
	if( !Stream.is_Open() || !is_Valid() )
	{
		return( false );
	}

	Set_File_Type(GRID_FILE_FORMAT_Binary);

	int	y	= bFlip ? Get_NY() - 1 : 0;
	int	dy	= bFlip ? -1 : 1;

	//-----------------------------------------------------
	if( File_Type == SG_DATATYPE_Bit )
	{
		int	nxBytes	= Get_NX() / 8 + 1;

		if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal )
		{
			for(int iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
			{
				Stream.Read(m_Values[y], sizeof(char), nxBytes);
			}
		}
		else
		{
			char	*Line	= (char *)SG_Malloc(nxBytes);

			for(int iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
			{
				Stream.Read(Line, sizeof(char), nxBytes);

				char	*pValue	= Line;

				for(int x=0; x<Get_NX(); pValue++)
				{
					for(int i=0; i<8 && x<Get_NX(); i++, x++)
					{
						Set_Value(x, y, (*pValue & m_Bitmask[i]) == 0 ? 0.0 : 1.0);
					}
				}
			}

			SG_Free(Line);
		}
	}

	//-----------------------------------------------------
	else
	{
		int	nValueBytes	= (int)SG_Data_Type_Get_Size(File_Type);
		int	nxBytes		= Get_NX() * nValueBytes;

		if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && !bSwapBytes )
		{
			for(int iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
			{
				Stream.Read(m_Values[y], sizeof(char), nxBytes);
			}
		}
		else
		{
			char	*Line	= (char *)SG_Malloc(nxBytes);

			for(int iy=0; iy<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(iy, Get_NY()); iy++, y+=dy)
			{
				Stream.Read(Line, sizeof(char), nxBytes);

				char	*pValue	= Line;

				for(int x=0; x<Get_NX(); x++, pValue+=nValueBytes)
				{
					if( bSwapBytes )
					{
						_Swap_Bytes(pValue, nValueBytes);
					}

					switch( File_Type )
					{
					case SG_DATATYPE_Byte  :	Set_Value(x, y, *(BYTE   *)pValue, false);	break;
					case SG_DATATYPE_Char  :	Set_Value(x, y, *(char   *)pValue, false);	break;
					case SG_DATATYPE_Word  :	Set_Value(x, y, *(WORD   *)pValue, false);	break;
					case SG_DATATYPE_Short :	Set_Value(x, y, *(short  *)pValue, false);	break;
					case SG_DATATYPE_DWord :	Set_Value(x, y, *(DWORD  *)pValue, false);	break;
					case SG_DATATYPE_Int   :	Set_Value(x, y, *(int    *)pValue, false);	break;
					case SG_DATATYPE_Float :	Set_Value(x, y, *(float  *)pValue, false);	break;
					case SG_DATATYPE_Double:	Set_Value(x, y, *(double *)pValue, false);	break;
					default:	break;
					}
				}
			}

			SG_Free(Line);
		}
	}

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Binary(CSG_File &Stream, int xA, int yA, int xN, int yN, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes)
{
	//-----------------------------------------------------
	if( !Stream.is_Open() || !m_System.is_Valid() || m_Type == SG_DATATYPE_Undefined )
	{
		return( false );
	}

	Set_File_Type(GRID_FILE_FORMAT_Binary);

	int	y	= bFlip ? yA + yN - 1 : yA;
	int	dy	= bFlip ? -1 : 1;

	//-----------------------------------------------------
	if( File_Type == SG_DATATYPE_Bit )
	{
		int	nxBytes	= xN / 8 + 1;

		if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && xA % 8 == 0 )
		{
			int	axBytes	= xA / 8;

			for(int iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
			{
				Stream.Write((char *)m_Values[y] + axBytes, sizeof(char), nxBytes);
			}
		}
		else
		{
			char	*Line	= (char *)SG_Malloc(nxBytes);

			for(int iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
			{
				char	*pValue	= Line;

				for(int ix=0, x=xA; ix<xN; pValue++)
				{
					for(int i=0; i<8 && ix<xN; i++, ix++, x++)
					{
						*pValue	= asChar(x, y) != 0.0 ? *pValue | m_Bitmask[i] : *pValue & (~m_Bitmask[i]);
					}
				}

				Stream.Write(Line, sizeof(char), nxBytes);
			}

			SG_Free(Line);
		}
	}

	//-----------------------------------------------------
	else
	{
		int	nValueBytes	= (int)SG_Data_Type_Get_Size(File_Type);
		int	nxBytes		= xN * nValueBytes;

		if( m_Type == File_Type && m_Memory_Type == GRID_MEMORY_Normal && !bSwapBytes )
		{
			int	axBytes	= xA * nValueBytes;

			for(int iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
			{
				Stream.Write((char *)m_Values[y] + axBytes, sizeof(char), nxBytes);
			}
		}
		else
		{
			char	*Line	= (char *)SG_Malloc(nxBytes);

			for(int iy=0; iy<yN && SG_UI_Process_Set_Progress(iy, yN); iy++, y+=dy)
			{
				char	*pValue	= Line;

				for(int ix=0, x=xA; ix<xN; ix++, x++, pValue+=nValueBytes)
				{
					switch( File_Type )
					{
					case SG_DATATYPE_Byte  :	*(BYTE   *)pValue	= asByte  (x, y, false);	break;
					case SG_DATATYPE_Char  :	*(char   *)pValue	= asChar  (x, y, false);	break;
					case SG_DATATYPE_Word  :	*(WORD   *)pValue	= asShort (x, y, false);	break;
					case SG_DATATYPE_Short :	*(short  *)pValue	= asShort (x, y, false);	break;
					case SG_DATATYPE_DWord :	*(DWORD  *)pValue	= asInt   (x, y, false);	break;
					case SG_DATATYPE_Int   :	*(int    *)pValue	= asInt   (x, y, false);	break;
					case SG_DATATYPE_Float :	*(float  *)pValue	= asFloat (x, y, false);	break;
					case SG_DATATYPE_Double:	*(double *)pValue	= asDouble(x, y, false);	break;
					default:	break;
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

	//-----------------------------------------------------
	SG_UI_Process_Set_Ready();

	return( true );
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
				fscanf(Stream.Get_Stream(), "%lf", &Value);

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

sLong SG_Grid_Cache_Check(CSG_Grid_System &m_System, int nValueBytes)
{
	if(	SG_Grid_Cache_Get_Automatic() && (m_System.Get_NCells() * nValueBytes) > SG_Grid_Cache_Get_Threshold() )
	{
		switch( SG_Grid_Cache_Get_Confirm() )
		{
		default:
			break;

		case 1:
			{
				CSG_String	s;

				s.Printf(SG_T("%s\n%s\n%s: %.2fMB"),
					_TL("Shall I activate file caching for new grid."),
					m_System.Get_Name(),
					_TL("Total memory size"),
					(m_System.Get_NCells() * nValueBytes) / (double)N_MEGABYTE_BYTES
				);

				if( SG_UI_Dlg_Continue(s, _TL("Activate Grid File Cache?")) )
				{
				//	Memory_Type	= GRID_MEMORY_Cache;

					return( SG_Grid_Cache_Get_Threshold() );
				}
			}
			break;

		case 2:
			{
				CSG_Parameters	p(NULL, _TL("Activate Grid File Cache?"), SG_T(""));

				p.Add_Value(
					NULL	, SG_T("BUFFERSIZE")	, _TL("Buffer Size [MB]"),
					SG_T(""),
					PARAMETER_TYPE_Double, SG_Grid_Cache_Get_Threshold_MB(), 0.0, true
				);

				if( SG_UI_Dlg_Parameters(&p, _TL("Activate Grid File Cache?")) )
				{
				//	Memory_Type	= GRID_MEMORY_Cache;

				//	Set_Buffer_Size((sLong)(p(SG_T("BUFFERSIZE"))->asDouble() * N_MEGABYTE_BYTES));

					return( (sLong)(p(SG_T("BUFFERSIZE"))->asDouble() * N_MEGABYTE_BYTES) );
				}
			}
			break;
		}
	}

	return( 0 );
}

//---------------------------------------------------------
bool CSG_Grid::_Load_Native(const CSG_String &File_Name, TSG_Grid_Memory_Type Memory_Type, bool bLoadData)
{
	//-----------------------------------------------------
	CSG_Grid_File_Info	Info;

	if( !Info.Create(File_Name) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Set_Name        (Info.m_Name);
	Set_Description (Info.m_Description);
	Set_Unit        (Info.m_Unit);
	Set_NoData_Value(Info.m_NoData);

	m_System		= Info.m_System;
	m_Type			= Info.m_Type;
	m_zScale		= Info.m_zScale;
	m_zOffset		= Info.m_zOffset;

	Get_Projection().Create(Info.m_Projection);

	//-----------------------------------------------------
	// Load Data...

	if( !bLoadData )
	{
		return( _Memory_Create(Memory_Type) );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !SG_Data_Type_is_Numeric(m_Type) )	// ASCII...
	{
		if(	Stream.Open(Info.m_Data_File                                , SG_FILE_R, false)
		||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T( "dat")), SG_FILE_R, false)
		||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat")), SG_FILE_R, false) )
		{
			Stream.Seek((long)Info.m_Offset);

			return( _Load_ASCII(Stream, Memory_Type, Info.m_bFlip) );
		}
	}

	//-----------------------------------------------------
	else	// Binary...
	{
		if( SG_Grid_Cache_Check(m_System, Get_nValueBytes()) > 0 )
		{
			Set_Buffer_Size(SG_Grid_Cache_Check(m_System, Get_nValueBytes()));

			if( _Cache_Create(Info.m_Data_File                                , m_Type, Info.m_Offset, Info.m_bSwapBytes, Info.m_bFlip)
			||	_Cache_Create(SG_File_Make_Path(NULL, File_Name, SG_T( "dat")), m_Type, Info.m_Offset, Info.m_bSwapBytes, Info.m_bFlip)
			||	_Cache_Create(SG_File_Make_Path(NULL, File_Name, SG_T("sdat")), m_Type, Info.m_Offset, Info.m_bSwapBytes, Info.m_bFlip) )
			{
				return( true );
			}

			Memory_Type	= GRID_MEMORY_Cache;
		}

		if( _Memory_Create(Memory_Type) )
		{
			if(	Stream.Open(Info.m_Data_File                                , SG_FILE_R, true)
			||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T( "dat")), SG_FILE_R, true)
			||	Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat")), SG_FILE_R, true) )
			{
				Stream.Seek((long)Info.m_Offset);

				return( _Load_Binary(Stream, m_Type, Info.m_bFlip, Info.m_bSwapBytes) );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Native(const CSG_String &File_Name, int xA, int yA, int xN, int yN, bool bBinary)
{
	CSG_Grid_File_Info	Info(*this);

	if(	Info.Save(File_Name, bBinary) )
	{
		CSG_File	Stream;

		if( Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat")), SG_FILE_W, true) )
		{
			if( bBinary )
			{
#ifdef WORDS_BIGENDIAN
				return( _Save_Binary(Stream, xA, yA, xN, yN, m_Type, false,  true) );
#else
				return( _Save_Binary(Stream, xA, yA, xN, yN, m_Type, false, false) );
#endif
			}
			else
			{
				return( _Save_ASCII (Stream, xA, yA, xN, yN) );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_Surfer(const CSG_String &File_Name, TSG_Grid_Memory_Type Memory_Type, bool bLoadData)
{
	if( !SG_File_Cmp_Extension(File_Name, SG_T("grd")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File_Name, SG_FILE_R, true) )
	{
		return( false );
	}

	char	Identifier[4];

	Stream.Read(Identifier, sizeof(char), 4);

	//-----------------------------------------------------
	if( !strncmp(Identifier, "DSBB", 4) )	// Binary...
	{
		short		nx, ny;
		double		d;
		TSG_Rect	r;

		Stream.Read(&nx    , sizeof(short ));
		Stream.Read(&ny    , sizeof(short ));
		Stream.Read(&r.xMin, sizeof(double));
		Stream.Read(&r.xMax, sizeof(double));
		Stream.Read(&r.yMin, sizeof(double));
		Stream.Read(&r.yMax, sizeof(double));
		Stream.Read(&d     , sizeof(double));	// zMin
		Stream.Read(&d     , sizeof(double));	// zMax

		d	= (r.xMax - r.xMin) / (nx - 1.0);
	//	d	= (r.yMax - r.yMin) / (ny - 1.0);	// we could proof for equal cellsize in direction of y...

		//-------------------------------------------------
		if( !Create(SG_DATATYPE_Float, nx, ny, d, r.xMin, r.yMin, Memory_Type) || Stream.is_EOF() )
		{
			return( false );
		}

		//-------------------------------------------------
		if( bLoadData )
		{
			float	*fLine	= (float *)SG_Malloc(Get_NX() * sizeof(float));

			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Read(fLine, sizeof(float), Get_NX());

				for(int x=0; x<Get_NX(); x++)
				{
					Set_Value(x, y, fLine[x]);
				}
			}

			SG_Free(fLine);
		}

		Get_MetaData().Add_Child("SURFER_GRID", "Surfer Grid (Binary)");
	}

	//-----------------------------------------------------
	else if( !strncmp(Identifier, "DSAA", 4) )	// ASCII...
	{
		int			nx, ny;
		double		d;
		TSG_Rect	r;

		fscanf(Stream.Get_Stream(), "%d  %d ", &nx    , &ny    );
		fscanf(Stream.Get_Stream(), "%lf %lf", &r.xMin, &r.xMax);
		fscanf(Stream.Get_Stream(), "%lf %lf", &r.yMin, &r.yMax);
		fscanf(Stream.Get_Stream(), "%lf %lf", &d     , &d     );

		d	= (r.xMax - r.xMin) / (nx - 1.0);
	//	d	= (r.yMax - r.yMin) / (ny - 1.0);	// we could proof for equal cellsize in direction of y...

		//-------------------------------------------------
		if( !Create(SG_DATATYPE_Float, nx, ny, d, r.xMin, r.yMin, Memory_Type) || Stream.is_EOF() )
		{
			return( false );
		}

		//-------------------------------------------------
		if( bLoadData )
		{
			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					fscanf(Stream.Get_Stream(), "%lf", &d);

					Set_Value(x, y, d);
				}
			}
		}

		Get_MetaData().Add_Child("SURFER_GRID", "Surfer Grid (ASCII)");
	}

	//-------------------------------------------------
	SG_UI_Process_Set_Ready();

	Set_File_Name(File_Name);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_File_Info::CSG_Grid_File_Info(void)
{
	_On_Construction();
}

void CSG_Grid_File_Info::_On_Construction(void)
{
	m_Name			.Clear();
	m_Description	.Clear();
	m_Unit			.Clear();
	m_System		.Assign(0.0, 0.0, 0.0, 0, 0);
	m_Type			= SG_DATATYPE_Float;	// defaults to float
	m_zScale		= 1.0;
	m_zOffset		= 0;
	m_NoData		= -99999.0;
	m_Data_File		.Clear();
	m_bFlip			= false;
	m_bSwapBytes	= false;
	m_Offset		= 0;
	m_Projection	.Destroy();
}

//---------------------------------------------------------
CSG_Grid_File_Info::CSG_Grid_File_Info(const CSG_Grid_File_Info &Info)
{
	Create(Info);
}

bool CSG_Grid_File_Info::Create(const CSG_Grid_File_Info &Info)
{
	m_Name			= Info.m_Name;
	m_Description	= Info.m_Description;
	m_Unit			= Info.m_Unit;
	m_System		= Info.m_System;
	m_Type			= Info.m_Type;
	m_zScale		= Info.m_zScale;
	m_zOffset		= Info.m_zOffset;
	m_NoData		= Info.m_NoData;
	m_Data_File		= Info.m_Data_File;
	m_bFlip			= Info.m_bFlip;
	m_bSwapBytes	= Info.m_bSwapBytes;
	m_Offset		= Info.m_Offset;
	m_Projection	= Info.m_Projection;

	return( true );
}

//---------------------------------------------------------
CSG_Grid_File_Info::CSG_Grid_File_Info(const CSG_Grid &Grid)
{
	Create(Grid);
}

bool CSG_Grid_File_Info::Create(const CSG_Grid &Grid)
{
	m_Name			= Grid.Get_Name();
	m_Description	= Grid.Get_Description();
	m_Unit			= Grid.Get_Unit();
	m_System		= Grid.Get_System();
	m_Type			= Grid.Get_Type();
	m_zScale		= Grid.Get_Scaling();
	m_zOffset		= Grid.Get_Offset();
	m_NoData		= Grid.Get_NoData_Value();
	m_Data_File		.Clear();
	m_bFlip			= false;
	m_bSwapBytes	= false;
	m_Offset		= 0;
	m_Projection	= Grid.Get_Projection();

	return( true );
}

//---------------------------------------------------------
CSG_Grid_File_Info::CSG_Grid_File_Info(const CSG_String &File_Name)
{
	Create(File_Name);
}

bool CSG_Grid_File_Info::Create(const CSG_String &File_Name)
{
	_On_Construction();

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(File_Name, SG_FILE_R, false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	sLong	NX = 0, NY = 0;
	double	Cellsize = 0.0, xMin = 0.0, yMin = 0.0;

	do
	{
		CSG_String	Value;

		switch( _Get_Key(Stream, Value) )
		{
		case GRID_FILE_KEY_NAME           :	m_Name        = Value;	break;
		case GRID_FILE_KEY_DESCRIPTION    :	m_Description = Value;	break;
		case GRID_FILE_KEY_UNITNAME       :	m_Unit        = Value;	break;

		case GRID_FILE_KEY_CELLCOUNT_X    :	NX            = Value.asInt   ();	break;
		case GRID_FILE_KEY_CELLCOUNT_Y    :	NY            = Value.asInt   ();	break;
		case GRID_FILE_KEY_POSITION_XMIN  :	xMin          = Value.asDouble();	break;
		case GRID_FILE_KEY_POSITION_YMIN  :	yMin          = Value.asDouble();	break;
		case GRID_FILE_KEY_CELLSIZE       :	Cellsize      = Value.asDouble();	break;

		case GRID_FILE_KEY_Z_FACTOR       :	m_zScale      = Value.asDouble();	break;
		case GRID_FILE_KEY_Z_OFFSET       :	m_zOffset     = Value.asDouble();	break;
		case GRID_FILE_KEY_NODATA_VALUE   :	m_NoData      = Value.asDouble();	break;

		case GRID_FILE_KEY_DATAFILE_OFFSET:	m_Offset      = Value.asInt   ();	break;
		case GRID_FILE_KEY_BYTEORDER_BIG  :	m_bSwapBytes  = Value.Find(GRID_FILE_KEY_TRUE) >= 0;	break;
		case GRID_FILE_KEY_TOPTOBOTTOM    :	m_bFlip       = Value.Find(GRID_FILE_KEY_TRUE) >= 0;	break;

		case GRID_FILE_KEY_DATAFILE_NAME:
			if( SG_File_Get_Path(Value).Length() > 0 )
			{
				m_Data_File	= Value;
			}
			else
			{
				m_Data_File	= SG_File_Make_Path(SG_File_Get_Path(File_Name), Value);
			}
			break;

		case GRID_FILE_KEY_DATAFORMAT:
			{
				for(int i=0; i<SG_DATATYPE_Undefined; i++)
				{
					if( Value.Find(gSG_Data_Type_Identifier[i]) >= 0 )
					{
						m_Type	= (TSG_Data_Type)i;

						break;
					}
				}
			}
			break;
		}
	}
	while( !Stream.is_EOF() );

	//-----------------------------------------------------
	m_Projection.Load(SG_File_Make_Path(NULL, File_Name, SG_T("prj")), SG_PROJ_FMT_WKT);

	if( !m_System.Assign(Cellsize, xMin, yMin, (int)NX, (int)NY) )
	{
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
int CSG_Grid_File_Info::_Get_Key(CSG_File &Stream, CSG_String &Value)
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_String &File_Name, bool bBinary)
{
	return( Save(File_Name, 0, 0, m_System.Get_NX(), m_System.Get_NY(), bBinary) );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_String &File_Name, int xStart, int yStart, int xCount, int yCount, bool bBinary)
{
	CSG_File	Stream;

	if(	Stream.Open(File_Name, SG_FILE_W, false) )
	{
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_NAME           ], m_Name       .c_str()   );
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_DESCRIPTION    ], m_Description.c_str()   );
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_UNITNAME       ], m_Unit       .c_str()   );
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_DATAFORMAT     ], bBinary ? gSG_Data_Type_Identifier[m_Type] : SG_T("ASCII") );
		Stream.Printf("%s\t= %d\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_DATAFILE_OFFSET], 0                       );
#ifdef WORDS_BIGENDIAN
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_BYTEORDER_BIG  ], true                    );
#else
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_BYTEORDER_BIG  ], false                   );
#endif
		Stream.Printf("%s\t= %s\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_TOPTOBOTTOM    ], GRID_FILE_KEY_FALSE     );
		Stream.Printf("%s\t= %.10f\n", gSG_Grid_File_Key_Names[GRID_FILE_KEY_POSITION_XMIN  ], xStart * m_System.Get_Cellsize() + m_System.Get_XMin() );
		Stream.Printf("%s\t= %.10f\n", gSG_Grid_File_Key_Names[GRID_FILE_KEY_POSITION_YMIN  ], yStart * m_System.Get_Cellsize() + m_System.Get_YMin() );
		Stream.Printf("%s\t= %d\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_CELLCOUNT_X    ], xCount                  );
		Stream.Printf("%s\t= %d\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_CELLCOUNT_Y    ], yCount                  );
		Stream.Printf("%s\t= %.10f\n", gSG_Grid_File_Key_Names[GRID_FILE_KEY_CELLSIZE       ], m_System.Get_Cellsize() );
		Stream.Printf("%s\t= %f\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_Z_FACTOR       ], m_zScale                );
		Stream.Printf("%s\t= %f\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_Z_OFFSET       ], m_zOffset               );
		Stream.Printf("%s\t= %f\n"   , gSG_Grid_File_Key_Names[GRID_FILE_KEY_NODATA_VALUE   ], m_NoData                );

		if( m_Projection.is_Okay() )
		{
			m_Projection.Save(SG_File_Make_Path(NULL, File_Name, SG_T("prj")), SG_PROJ_FMT_WKT);

			if( Stream.Open(SG_File_Make_Path(NULL, File_Name, SG_T("sdat")) + ".aux.xml", SG_FILE_W, false) )	// store srs information that is recognized by ArcGIS
			{
				Stream.Write("<PAMDataset>\n<SRS>");
				Stream.Write(m_Projection.Get_WKT());
				Stream.Write("</SRS>\n</PAMDataset>\n");
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_String &File_Name, const CSG_Grid &Grid, bool bBinary)
{
	return( Save(File_Name, Grid, 0, 0, Grid.Get_NX(), Grid.Get_NY(), bBinary) );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_String &File_Name, const CSG_Grid &Grid, int xStart, int yStart, int xCount, int yCount, bool bBinary)
{
	CSG_Grid_File_Info	Info(Grid);

	return( Info.Save(File_Name, xStart, yStart, xCount, yCount, bBinary) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
