
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
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
#include "tool_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::On_Reload(void)
{
	return( Create(Get_File_Name(false)) );
}

//---------------------------------------------------------
bool CSG_Grid::On_Delete(void)
{
	CSG_String	FileName	= Get_File_Name(true);

	SG_File_Set_Extension(FileName, "sg-grd-z"); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "sg-grd"  ); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "sgrd"    ); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "sdat"    ); SG_File_Delete(FileName); SG_File_Delete(FileName + ".aux.xml");
	SG_File_Set_Extension(FileName, "mgrd"    ); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "dgm"     ); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "dat"     ); SG_File_Delete(FileName);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static TSG_Grid_File_Format	gSG_Grid_File_Format_Default	= GRID_FILE_FORMAT_Binary_old;

//---------------------------------------------------------
bool					SG_Grid_Set_File_Format_Default	(int Format)
{
	switch( Format )
	{
	case GRID_FILE_FORMAT_Binary_old:
	case GRID_FILE_FORMAT_Binary    :
	case GRID_FILE_FORMAT_Compressed:
	case GRID_FILE_FORMAT_ASCII     :
	case GRID_FILE_FORMAT_GeoTIFF   :
		gSG_Grid_File_Format_Default	= (TSG_Grid_File_Format)Format;
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
TSG_Grid_File_Format	SG_Grid_Get_File_Format_Default	(void)
{
	return( gSG_Grid_File_Format_Default );
}

//---------------------------------------------------------
CSG_String				SG_Grid_Get_File_Extension_Default	(void)
{
	switch( gSG_Grid_File_Format_Default )
	{
	default:
	case GRID_FILE_FORMAT_Compressed:	return( "sg-grd-z" );
	case GRID_FILE_FORMAT_Binary    :	return( "sg-grd"   );
	case GRID_FILE_FORMAT_Binary_old:	return( "sgrd"     );
	case GRID_FILE_FORMAT_GeoTIFF   :	return( "tif"      );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Save(const CSG_String &FileName, int Format)
{
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Saving grid"), FileName.c_str()), true);

	//-----------------------------------------------------
	if( Format == GRID_FILE_FORMAT_Undefined )
	{
		Format	= gSG_Grid_File_Format_Default;

		if( SG_File_Cmp_Extension(FileName, "sg-grd-z") )	Format	= GRID_FILE_FORMAT_Compressed;
		if( SG_File_Cmp_Extension(FileName, "sg-grd"  ) )	Format	= GRID_FILE_FORMAT_Binary    ;
		if( SG_File_Cmp_Extension(FileName, "sgrd"    ) )	Format	= GRID_FILE_FORMAT_Binary_old;
		if( SG_File_Cmp_Extension(FileName, "tif"     ) )	Format	= GRID_FILE_FORMAT_GeoTIFF   ;
	}

	//-----------------------------------------------------
	bool	bResult	= false;

	switch( Format )
	{
	default:
		bResult = _Save_Native(FileName, (TSG_Grid_File_Format)Format);
		break;

	case GRID_FILE_FORMAT_Compressed:
		bResult = _Save_Compressed(FileName);
		break;

	case GRID_FILE_FORMAT_GeoTIFF:
		SG_RUN_TOOL(bResult, "io_gdal", 2,	// Export GeoTIFF
			    SG_TOOL_PARAMLIST_ADD("GRIDS", this)
			&&	SG_TOOL_PARAMETER_SET("FILE" , FileName)
		);
		break;
	}

	SG_UI_Process_Set_Ready();

	//-----------------------------------------------------
	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(FileName, true);

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_External(const CSG_String &FileName, bool bCached, bool bLoadData)
{
	bool	bResult	= false;

	CSG_Data_Manager	Data;

	CSG_Tool	*pTool	= NULL;

	SG_UI_Msg_Lock(true);

	//-----------------------------------------------------
	// Image Import

	if( (	SG_File_Cmp_Extension(FileName, "bmp")
		||	SG_File_Cmp_Extension(FileName, "gif")
		||	SG_File_Cmp_Extension(FileName, "jpg")
		||	SG_File_Cmp_Extension(FileName, "png")
		||	SG_File_Cmp_Extension(FileName, "pcx") )
	&&  !bResult && (pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_grid_image", 1)) != NULL )
	{
		pTool->Settings_Push(&Data);

		bResult	=  pTool->Set_Parameter("FILE", FileName)
				&& pTool->Execute();

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
	}

	//-----------------------------------------------------
	// GDAL Import

	if( !bResult && (pTool = SG_Get_Tool_Library_Manager().Create_Tool("io_gdal", 0)) != NULL )
	{
		pTool->Settings_Push(&Data);

		bResult	=  pTool->Set_Parameter("FILES"   , FileName)
				&& pTool->Set_Parameter("MULTIPLE", 0       )	// output as single grid(s)
				&& pTool->Execute();

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
	}

	SG_UI_Msg_Lock(false);

	//-----------------------------------------------------
	if( bResult && Data.Get_Grid_System(0) && Data.Get_Grid_System(0)->Get(0) && Data.Get_Grid_System(0)->Get(0)->is_Valid() )
	{
		CSG_Grid	*pGrid	= (CSG_Grid *)Data.Get_Grid_System(0)->Get(0);

		if( pGrid->is_Cached() )
		{
			return( Create(*pGrid) );
		}

		Set_File_Name(FileName, false);

		Set_Name			(pGrid->Get_Name());
		Set_Description		(pGrid->Get_Description());

		m_System			= pGrid->m_System;
		m_Type				= pGrid->m_Type;
		m_Values			= pGrid->m_Values;	pGrid->m_Values	= NULL;	// take ownership of data array

		m_zOffset			= pGrid->m_zOffset;
		m_zScale			= pGrid->m_zScale;
		m_Unit				= pGrid->m_Unit;

		m_nBytes_Value		= pGrid->m_nBytes_Value;
		m_nBytes_Line		= pGrid->m_nBytes_Line;

		Get_MetaData  ()	= pGrid->Get_MetaData  ();
		Get_Projection()	= pGrid->Get_Projection();

		Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_Value(true));
		
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::_Load_PGSQL(const CSG_String &FileName, bool bCached, bool bLoadData)
{
	bool	bResult	= false;

	if( FileName.BeforeFirst(':').Cmp("PGSQL") == 0 )	// database source
	{
		CSG_String	s(FileName);

		s	= s.AfterFirst(':');	CSG_String	Host  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Port  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	DBName(s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Table (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	rid   (s.BeforeFirst(':').AfterFirst('='));

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("db_pgsql", 0, true);	// CGet_Connections

		if(	pTool != NULL )
		{
			SG_UI_ProgressAndMsg_Lock(true);

			//---------------------------------------------
			CSG_Table	Connections;
			CSG_String	Connection	= DBName + " [" + Host + ":" + Port + "]";

			pTool->Set_Manager(NULL);
			pTool->On_Before_Execution();

			if( SG_TOOL_PARAMETER_SET("CONNECTIONS", &Connections) && pTool->Execute() )	// CGet_Connections
			{
				for(int i=0; !bResult && i<Connections.Get_Count(); i++)
				{
					if( !Connection.Cmp(Connections[i].asString(0)) )
					{
						bResult	= true;
					}
				}
			}

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			//---------------------------------------------
			if( bResult && (bResult = (pTool = SG_Get_Tool_Library_Manager().Create_Tool("db_pgsql", 33, true)) != NULL) == true )	// CPGIS_Raster_Load_Band
			{
				pTool->Set_Manager(NULL);
				pTool->On_Before_Execution();

				bResult	=  SG_TOOL_PARAMETER_SET("CONNECTION", Connection)
						&& SG_TOOL_PARAMETER_SET("TABLES"    , Table)
						&& SG_TOOL_PARAMETER_SET("RID"       , rid)
						&& SG_TOOL_PARAMETER_SET("GRID"      , this)
						&& pTool->Execute();

				SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
			}

			SG_UI_ProgressAndMsg_Lock(false);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//						Native							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_Native(const CSG_String &FileName, bool bCached, bool bLoadData)
{
	CSG_Grid_File_Info	Info;

	if( !Info.Create(FileName) )
	{
		return( false );
	}

	Set_File_Name(FileName, true);

	Set_Name        (Info.m_Name);
	Set_Description (Info.m_Description);
	Set_Unit        (Info.m_Unit);

	Set_NoData_Value_Range(Info.m_NoData[0], Info.m_NoData[1]);

	m_System		= Info.m_System;
	m_Type			= Info.m_Type;
	m_zScale		= Info.m_zScale;
	m_zOffset		= Info.m_zOffset;

	m_nBytes_Value	= SG_Data_Type_Get_Size(m_Type);
	m_nBytes_Line	= m_Type == SG_DATATYPE_Bit ? 1 + Get_NX() / 8 : Get_NX() * m_nBytes_Value;

	Get_Projection().Load(SG_File_Make_Path("", FileName, "prj"), SG_PROJ_FMT_WKT);

	if( !bLoadData )
	{
		return( _Memory_Create(bCached) );
	}

	Load_MetaData(FileName);

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !SG_Data_Type_is_Numeric(m_Type) )	// ASCII...
	{
		if(	Stream.Open(Info.m_Data_File                       , SG_FILE_R, false)
		||	Stream.Open(SG_File_Make_Path("", FileName,  "dat"), SG_FILE_R, false)
		||	Stream.Open(SG_File_Make_Path("", FileName, "sdat"), SG_FILE_R, false) )
		{
			Stream.Seek(Info.m_Offset);

			return( _Load_ASCII(Stream, bCached, Info.m_bFlip) );
		}
	}

	//-----------------------------------------------------
	else	// Binary...
	{
		if(	bCached || _Cache_Check() )
		{
			if( _Cache_Create(Info.m_Data_File                       , m_Type, Info.m_Offset, Info.m_bSwapBytes, Info.m_bFlip)
			||	_Cache_Create(SG_File_Make_Path("", FileName,  "dat"), m_Type, Info.m_Offset, Info.m_bSwapBytes, Info.m_bFlip)
			||	_Cache_Create(SG_File_Make_Path("", FileName, "sdat"), m_Type, Info.m_Offset, Info.m_bSwapBytes, Info.m_bFlip) )
			{
				return( true );
			}
		}

		m_Cache_File	= Info.m_Data_File;
		m_Cache_Offset	= Info.m_Offset;
		m_Cache_bSwap	= Info.m_bSwapBytes;
		m_Cache_bFlip	= Info.m_bFlip;

		if( _Memory_Create(bCached) )
		{
			if(	Stream.Open(Info.m_Data_File                       , SG_FILE_R, true)
			||	Stream.Open(SG_File_Make_Path("", FileName,  "dat"), SG_FILE_R, true)
			||	Stream.Open(SG_File_Make_Path("", FileName, "sdat"), SG_FILE_R, true) )
			{
				Stream.Seek(Info.m_Offset);

				return( _Load_Binary(Stream, m_Type, Info.m_bFlip, Info.m_bSwapBytes) );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Native(const CSG_String &_FileName, TSG_Grid_File_Format Format)
{
#ifdef WORDS_BIGENDIAN
	bool	bBigEndian	= true;
#else
	bool	bBigEndian	= false;
#endif

	CSG_String	FileName(_FileName);

	bool	bBinary;

	if( Format == GRID_FILE_FORMAT_ASCII )
	{
		bBinary	= false;
	}
	else if( Format == GRID_FILE_FORMAT_Binary_old )
	{
		bBinary	= true;	SG_File_Set_Extension(FileName, "sgrd");
	}
	else
	{
		bBinary	= true;	SG_File_Set_Extension(FileName, "sg-grd");
	}

	CSG_Grid_File_Info	Info(*this);

	if(	Info.Save(FileName, bBinary) )
	{
		CSG_File	Stream(SG_File_Make_Path("", FileName, "sdat"), SG_FILE_W, true);

		if( bBinary ? _Save_Binary(Stream, m_Type, false, bBigEndian) : _Save_ASCII (Stream) )
		{
			Save_MetaData(FileName);

			Get_Projection().Save(SG_File_Make_Path("", FileName, "prj"), SG_PROJ_FMT_WKT);

			Info.Save_AUX_XML(SG_File_Make_Path("", FileName, "sdat"));

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Compressed						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_Compressed(const CSG_String &_FileName, bool bCached, bool bLoadData)
{
	Set_File_Name(_FileName, true);

	CSG_File_Zip	Stream(_FileName, SG_FILE_R);

	if( !Stream.is_Reading() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String	FileName(SG_File_Get_Name(_FileName, false) + ".");

	if( !Stream.Get_File(FileName + "sgrd"  )
	&&  !Stream.Get_File(FileName + "sg-grd") )
	{
		FileName.Clear();

		for(size_t i=0; i<Stream.Get_File_Count(); i++)
		{
			if( SG_File_Cmp_Extension(Stream.Get_File_Name(i), "sgrd"  )
			||  SG_File_Cmp_Extension(Stream.Get_File_Name(i), "sg-grd") )
			{
				FileName	= SG_File_Get_Name(Stream.Get_File_Name(i), false) + ".";
				Stream.Get_File(Stream.Get_File_Name(i));
				break;
			}
		}

		if( FileName.is_Empty() )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	CSG_Grid_File_Info	Info;

	if( !Info.Create(Stream) )
	{
		return( false );
	}

	Set_Name        (Info.m_Name);
	Set_Description (Info.m_Description);
	Set_Unit        (Info.m_Unit);

	Set_NoData_Value_Range(Info.m_NoData[0], Info.m_NoData[1]);

	m_System		= Info.m_System;
	m_Type			= Info.m_Type;
	m_zScale		= Info.m_zScale;
	m_zOffset		= Info.m_zOffset;

	m_nBytes_Value	= SG_Data_Type_Get_Size(m_Type);
	m_nBytes_Line	= m_Type == SG_DATATYPE_Bit ? 1 + Get_NX() / 8 : Get_NX() * m_nBytes_Value;

	if( Stream.Get_File(FileName + "prj") )
	{
		Get_Projection().Load(Stream, SG_PROJ_FMT_WKT);
	}

	if( !bLoadData )
	{
		return( _Memory_Create(bCached) );
	}

	if( Stream.Get_File(FileName + "mgrd") )
	{
		Load_MetaData(Stream);
	}

	//-----------------------------------------------------
	if( _Cache_Check() )
	{
		bCached	= true;
	}

	return( Stream.Get_File(FileName + "sdat") && _Memory_Create(bCached)
		&& _Load_Binary(Stream, m_Type, Info.m_bFlip, Info.m_bSwapBytes)
	);
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Compressed(const CSG_String &_FileName)
{
#ifdef WORDS_BIGENDIAN
	bool	bBigEndian	= true;
#else
	bool	bBigEndian	= false;
#endif

	CSG_File_Zip	Stream(_FileName, SG_FILE_W);

	if( Stream.is_Writing() )
	{
		CSG_String	FileName(SG_File_Get_Name(_FileName, false) + ".");

		CSG_Grid_File_Info	Info(*this);

		if( Stream.Add_File(FileName + "sgrd") && Info.Save(Stream, true)
		&&  Stream.Add_File(FileName + "sdat") && _Save_Binary(Stream, m_Type, false, bBigEndian) )
		{
			Stream.Add_File(FileName + "mgrd"        ); Save_MetaData(Stream);
			Stream.Add_File(FileName + "prj"         ); Get_Projection().Save(Stream, SG_PROJ_FMT_WKT);
			Stream.Add_File(FileName + "sdat.aux.xml"); Info.Save_AUX_XML(Stream);

			return( true );
		}
	}

	return( false );
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

	//-----------------------------------------------------
	if( File_Type == SG_DATATYPE_Bit )
	{
		int	nLineBytes	= Get_NX() / 8 + 1;

		if( m_Type == File_Type && !is_Cached() )
		{
			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Read(m_Values[bFlip ? Get_NY() - y - 1 : y], sizeof(char), nLineBytes);
			}
		}
		else
		{
			CSG_Array	Line(1, nLineBytes);

			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Read(Line.Get_Array(), nLineBytes);

				char	*pValue	= (char *)Line.Get_Array();

				for(int x=0, yy=bFlip ? Get_NY()-y-1 : y; x<Get_NX(); pValue++)
				{
					for(int i=0; i<8 && x<Get_NX(); i++, x++)
					{
						Set_Value(x, yy, (*pValue & m_Bitmask[i]) == 0 ? 0.0 : 1.0);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		int	nValueBytes	= (int)SG_Data_Type_Get_Size(File_Type);
		int	nLineBytes	= Get_NX() * nValueBytes;

		if( m_Type == File_Type && !is_Cached() && !bSwapBytes )
		{
			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Read(m_Values[bFlip ? Get_NY() - y - 1 : y], nLineBytes);
			}
		}
		else
		{
			CSG_Array	Line(1, nLineBytes);

			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Read(Line.Get_Array(), nLineBytes);

				char	*pValue	= (char *)Line.Get_Array();

				for(int x=0, yy=bFlip ? Get_NY()-y-1 : y; x<Get_NX(); x++, pValue+=nValueBytes)
				{
					if( bSwapBytes )
					{
						_Swap_Bytes(pValue, nValueBytes);
					}

					switch( File_Type )
					{
					case SG_DATATYPE_Byte  : Set_Value(x, yy, *(BYTE   *)pValue, false); break;
					case SG_DATATYPE_Char  : Set_Value(x, yy, *(char   *)pValue, false); break;
					case SG_DATATYPE_Word  : Set_Value(x, yy, *(WORD   *)pValue, false); break;
					case SG_DATATYPE_Short : Set_Value(x, yy, *(short  *)pValue, false); break;
					case SG_DATATYPE_DWord : Set_Value(x, yy, *(DWORD  *)pValue, false); break;
					case SG_DATATYPE_Int   : Set_Value(x, yy, *(int    *)pValue, false); break;
					case SG_DATATYPE_Float : Set_Value(x, yy, *(float  *)pValue, false); break;
					case SG_DATATYPE_Double: Set_Value(x, yy, *(double *)pValue, false); break;
					default:	break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_Binary(CSG_File &Stream, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes)
{
	//-----------------------------------------------------
	if( !Stream.is_Writing() || !is_Valid() )
	{
		return( false );
	}

	Set_File_Type(GRID_FILE_FORMAT_Binary);

	//-----------------------------------------------------
	if( File_Type == SG_DATATYPE_Bit )
	{
		int	nLineBytes	= Get_NX() / 8 + 1;

		if( m_Type == File_Type && !is_Cached() )
		{
			for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Write((char *)m_Values[bFlip ? Get_NY() - y - 1 : y], sizeof(char), nLineBytes);
			}
		}
		else
		{
			CSG_Array	Line(1, nLineBytes);

			for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				char	*pValue	= (char *)Line.Get_Array();

				for(int x=0, yy=bFlip ? Get_NY()-y-1 : y; x<Get_NX(); pValue++)
				{
					for(int i=0; i<8 && x<Get_NX(); i++, x++)
					{
						*pValue	= asChar(x, yy) != 0.0 ? *pValue | m_Bitmask[i] : *pValue & (~m_Bitmask[i]);
					}
				}

				Stream.Write(Line.Get_Array(), nLineBytes);
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		int	nValueBytes	= (int)SG_Data_Type_Get_Size(File_Type);
		int	nLineBytes	= Get_NX() * nValueBytes;

		if( m_Type == File_Type && !is_Cached() && !bSwapBytes )
		{
			for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Write((char *)m_Values[bFlip ? Get_NY() - y - 1 : y], nLineBytes);
			}
		}
		else
		{
			CSG_Array	Line(1, nLineBytes);

			for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				char	*pValue	= (char *)Line.Get_Array();

				for(int x=0, yy=bFlip ? Get_NY()-y-1 : y; x<Get_NX(); x++, pValue+=nValueBytes)
				{
					switch( File_Type )
					{
					case SG_DATATYPE_Byte  : *(BYTE   *)pValue = asByte  (x, yy, false); break;
					case SG_DATATYPE_Char  : *(char   *)pValue = asChar  (x, yy, false); break;
					case SG_DATATYPE_Word  : *(WORD   *)pValue = asShort (x, yy, false); break;
					case SG_DATATYPE_Short : *(short  *)pValue = asShort (x, yy, false); break;
					case SG_DATATYPE_DWord : *(DWORD  *)pValue = asInt   (x, yy, false); break;
					case SG_DATATYPE_Int   : *(int    *)pValue = asInt   (x, yy, false); break;
					case SG_DATATYPE_Float : *(float  *)pValue = asFloat (x, yy, false); break;
					case SG_DATATYPE_Double: *(double *)pValue = asDouble(x, yy, false); break;
					default:	break;
					}

					if( bSwapBytes )
					{
						_Swap_Bytes(pValue, nValueBytes);
					}
				}

				Stream.Write(Line.Get_Array(), nLineBytes);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//							ASCII						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_ASCII(CSG_File &Stream, bool bCached, bool bFlip)
{
	if( !Stream.is_Reading() || !_Memory_Create(bCached) )
	{
		return( false );
	}

	Set_File_Type(GRID_FILE_FORMAT_ASCII);

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(int x=0, yy=bFlip ? Get_NY()-y-1 : y; x<Get_NX(); x++)
		{
			Set_Value(x, yy, Stream.Scan_Double());
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid::_Save_ASCII(CSG_File &Stream, bool bFlip)
{
	if( !Stream.is_Writing() || !is_Valid() )
	{
		return( false );
	}

	Set_File_Type(GRID_FILE_FORMAT_ASCII);

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(int x=0, yy=bFlip ? Get_NY()-y-1 : y; x<Get_NX(); x++)
		{
			Stream.Printf("%lf ", asDouble(x, yy));
		}

		Stream.Printf("\n");
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Load_Surfer(const CSG_String &FileName, bool bCached, bool bLoadData)
{
	const float	NoData	= 1.70141e38f;

	if( !SG_File_Cmp_Extension(FileName, "grd") )
	{
		return( false );
	}

	Set_File_Name(FileName, true);

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(FileName, SG_FILE_R, true) )
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
		if( !Create(SG_DATATYPE_Float, nx, ny, d, r.xMin, r.yMin, bCached) || Stream.is_EOF() )
		{
			return( false );
		}

		//-------------------------------------------------
		if( bLoadData )
		{
			CSG_Array	Line(sizeof(float), Get_NX());	float *Values = (float *)Line.Get_Array();

			for(int y=0; y<Get_NY() && !Stream.is_EOF() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				Stream.Read(Values, sizeof(float), Get_NX());

				for(int x=0; x<Get_NX(); x++)
				{
					if( Values[x] == NoData )
					{
						Set_NoData(x, y);
					}
					else
					{
						Set_Value(x, y, Values[x]);
					}
				}
			}
		}

		Get_MetaData().Add_Child("SURFER_GRID", "Surfer Grid (Binary)");
	}

	//-----------------------------------------------------
	else if( !strncmp(Identifier, "DSAA", 4) )	// ASCII...
	{
		int		nx		= Stream.Scan_Int   ();
		int		ny		= Stream.Scan_Int   ();
		double	xMin	= Stream.Scan_Double();
		double	xMax	= Stream.Scan_Double();
		double	yMin	= Stream.Scan_Double(); Stream.Scan_Double();
	//	double	yMax	= Stream.Scan_Double();
		double	dx		= Stream.Scan_Double(); Stream.Scan_Double();
	//	double	dy		= Stream.Scan_Double();

		dx	= (xMax - xMin) / (nx - 1.0);
	//	dy	= (yMax - yMin) / (ny - 1.0);	// we could proof for equal cellsize in direction of y...

		//-------------------------------------------------
		if( !Create(SG_DATATYPE_Float, nx, ny, dx, xMin, yMin, bCached) || Stream.is_EOF() )
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
					double	Value;

					if( Stream.Scan(Value) && Value != NoData )
					{
						Set_Value(x, y, Value);
					}
					else
					{
						Set_NoData(x, y);
					}
				}
			}
		}

		Get_MetaData().Add_Child("SURFER_GRID", "Surfer Grid (ASCII)");
	}

	//-------------------------------------------------
	SG_UI_Process_Set_Ready();

	Set_File_Name(FileName);
	Load_MetaData(FileName);

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
	m_NoData[0]		= -99999.0;
	m_NoData[1]		= -99999.0;
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
	m_NoData[0]		= Info.m_NoData[0];
	m_NoData[1]		= Info.m_NoData[1];
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
	m_NoData[0]		= Grid.Get_NoData_Value();
	m_NoData[1]		= Grid.Get_NoData_Value(true);
	m_Data_File		.Clear();
	m_bFlip			= false;
	m_bSwapBytes	= false;
	m_Offset		= 0;
	m_Projection	= Grid.Get_Projection();

	return( true );
}

//---------------------------------------------------------
CSG_Grid_File_Info::CSG_Grid_File_Info(const CSG_String &FileName)
{
	Create(FileName);
}

bool CSG_Grid_File_Info::Create(const CSG_String &FileName)
{
	if( !SG_File_Cmp_Extension(FileName, "sg-grd-z") )
	{
		if( SG_File_Cmp_Extension(FileName, "sgrd")
		||  SG_File_Cmp_Extension(FileName, "sg-grd") )
		{
			CSG_File	Stream(FileName, SG_FILE_R, false);

			return( Create(Stream) );
		}

		return( false );
	}

	//-----------------------------------------------------
	CSG_File_Zip	Stream(FileName, SG_FILE_R);

	if( Stream.is_Reading() )
	{
		CSG_String	File(SG_File_Get_Name(FileName, false) + ".");

		if( !Stream.Get_File(File + "sgrd"  )
		&&  !Stream.Get_File(File + "sg-grd") )
		{
			for(size_t i=0; i<Stream.Get_File_Count(); i++)
			{
				if( SG_File_Cmp_Extension(Stream.Get_File_Name(i), "sgrd"  )
				||  SG_File_Cmp_Extension(Stream.Get_File_Name(i), "sg-grd") )
				{
					Stream.Get_File(Stream.Get_File_Name(i));
					break;
				}
			}
		}

		return( Create(Stream) );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
CSG_Grid_File_Info::CSG_Grid_File_Info(CSG_File &Stream)
{
	Create(Stream);
}

bool CSG_Grid_File_Info::Create(CSG_File &Stream)
{
	_On_Construction();

	//-----------------------------------------------------
	if( !Stream.is_Reading() )
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
		case GRID_FILE_KEY_NODATA_VALUE   :	m_NoData[0]   = Value.asDouble();
			Value = Value.AfterFirst(';');  m_NoData[1]   = Value.is_Empty() ? m_NoData[0] : Value.asDouble();	break;

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
				m_Data_File	= SG_File_Make_Path(SG_File_Get_Path(Stream.Get_File_Name()), Value);
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
	return( m_System.Assign(Cellsize, xMin, yMin, (int)NX, (int)NY) );
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
bool CSG_Grid_File_Info::Save(const CSG_String &FileName, bool bBinary)
{
	return( Save(CSG_File(FileName, SG_FILE_W, false), bBinary) );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_File &Stream, bool bBinary)
{
	if(	!Stream.is_Writing() )
	{
		return( false );
	}

#define GRID_FILE_PRINT(Key, Val)	{ CSG_String s(gSG_Grid_File_Key_Names[Key]); s += "\t= " + Val + "\n"; Stream.Write(s); }

	GRID_FILE_PRINT(GRID_FILE_KEY_NAME           , CSG_String::Format("%s"   , m_Name       .c_str()   ));
	GRID_FILE_PRINT(GRID_FILE_KEY_DESCRIPTION    , CSG_String::Format("%s"   , m_Description.c_str()   ));
	GRID_FILE_PRINT(GRID_FILE_KEY_UNITNAME       , CSG_String::Format("%s"   , m_Unit       .c_str()   ));
	GRID_FILE_PRINT(GRID_FILE_KEY_DATAFORMAT     , CSG_String::Format("%s"   , CSG_String(bBinary ? gSG_Data_Type_Identifier[m_Type] : "ASCII").c_str()));
	GRID_FILE_PRINT(GRID_FILE_KEY_DATAFILE_OFFSET, CSG_String::Format("%d"   , 0                       ));
#ifdef WORDS_BIGENDIAN
	GRID_FILE_PRINT(GRID_FILE_KEY_BYTEORDER_BIG  , CSG_String::Format("%s"   , GRID_FILE_KEY_TRUE      ));
#else
	GRID_FILE_PRINT(GRID_FILE_KEY_BYTEORDER_BIG  , CSG_String::Format("%s"   , GRID_FILE_KEY_FALSE     ));
#endif
	GRID_FILE_PRINT(GRID_FILE_KEY_TOPTOBOTTOM    , CSG_String::Format("%s"   , GRID_FILE_KEY_FALSE     ));
	GRID_FILE_PRINT(GRID_FILE_KEY_POSITION_XMIN  , CSG_String::Format("%.*f" , CSG_Grid_System::Get_Precision(), m_System.Get_XMin()));
	GRID_FILE_PRINT(GRID_FILE_KEY_POSITION_YMIN  , CSG_String::Format("%.*f" , CSG_Grid_System::Get_Precision(), m_System.Get_YMin()));
	GRID_FILE_PRINT(GRID_FILE_KEY_CELLCOUNT_X    , CSG_String::Format("%d"   , m_System.Get_NX()       ));
	GRID_FILE_PRINT(GRID_FILE_KEY_CELLCOUNT_Y    , CSG_String::Format("%d"   , m_System.Get_NY()       ));
	GRID_FILE_PRINT(GRID_FILE_KEY_CELLSIZE       , CSG_String::Format("%.*f" , CSG_Grid_System::Get_Precision(), m_System.Get_Cellsize()));
	GRID_FILE_PRINT(GRID_FILE_KEY_Z_FACTOR       , CSG_String::Format("%f"   , m_zScale                ));
	GRID_FILE_PRINT(GRID_FILE_KEY_Z_OFFSET       , CSG_String::Format("%f"   , m_zOffset               ));
	GRID_FILE_PRINT(GRID_FILE_KEY_NODATA_VALUE   , CSG_String::Format("%f;%f", m_NoData[0], m_NoData[1]));

	return( true );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_String &FileName, const CSG_Grid &Grid, bool bBinary)
{
	CSG_Grid_File_Info	Info(Grid);

	return( Info.Save(FileName, bBinary) );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save(const CSG_File &Stream, const CSG_Grid &Grid, bool bBinary)
{
	CSG_Grid_File_Info	Info(Grid);

	return( Info.Save(Stream, bBinary) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save_AUX_XML(const CSG_String &FileName)
{
	CSG_File	Stream;
	
	return( m_Projection.is_Okay() && Stream.Open(FileName + ".aux.xml", SG_FILE_W, false) && Save_AUX_XML(Stream) );
}

//---------------------------------------------------------
bool CSG_Grid_File_Info::Save_AUX_XML(CSG_File &Stream)
{
	if( m_Projection.is_Okay() && Stream.is_Writing() )	// store srs information that is recognized by ArcGIS
	{
		Stream.Write("<PAMDataset>\n<SRS>");
		Stream.Write(m_Projection.Get_WKT());
		Stream.Write("</SRS>\n</PAMDataset>\n");

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
