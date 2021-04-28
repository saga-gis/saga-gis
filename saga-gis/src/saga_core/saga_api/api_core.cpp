
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
//                     api_core.cpp                      //
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
#include <wx/utils.h>

#include "api_core.h"
#include "tool_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _OPENMP

#include <omp.h>

//---------------------------------------------------------
int		g_SG_OMP_Max_Num_Threads	= omp_get_num_procs();

//---------------------------------------------------------
void	SG_OMP_Set_Max_Num_Threads	(int iCores)
{
	if( iCores < 1 )
	{
		iCores	= 1;
	}
	else if( iCores > omp_get_num_procs() )
	{
		iCores	= omp_get_num_procs();
	}

	omp_set_num_threads(g_SG_OMP_Max_Num_Threads = iCores);
}

//---------------------------------------------------------
int		SG_OMP_Get_Max_Num_Threads	(void)
{
	return( g_SG_OMP_Max_Num_Threads );
}

//---------------------------------------------------------
int		SG_OMP_Get_Max_Num_Procs	(void)
{
	return( omp_get_num_procs() );
}

//---------------------------------------------------------
int		SG_OMP_Get_Thread_Num		(void)
{
	return( omp_get_thread_num() );
}

//---------------------------------------------------------
#else
void	SG_OMP_Set_Max_Num_Threads	(int iCores)	{}
int		SG_OMP_Get_Max_Num_Threads	(void)	{	return( 1 );	}
int		SG_OMP_Get_Max_Num_Procs	(void)	{	return( 1 );	}
int		SG_OMP_Get_Thread_Num		(void)	{	return( 0 );	}
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	SG_Data_Type_Get_Name	(TSG_Data_Type Type)
{
	switch( Type )
	{
	default                : return( _TL("undefined"                   ) );
	case SG_DATATYPE_Bit   : return( _TL("bit"                         ) );
	case SG_DATATYPE_Byte  : return( _TL("unsigned 1 byte integer"     ) );
	case SG_DATATYPE_Char  : return( _TL("signed 1 byte integer"       ) );
	case SG_DATATYPE_Word  : return( _TL("unsigned 2 byte integer"     ) );
	case SG_DATATYPE_Short : return( _TL("signed 2 byte integer"       ) );
	case SG_DATATYPE_DWord : return( _TL("unsigned 4 byte integer"     ) );
	case SG_DATATYPE_Int   : return( _TL("signed 4 byte integer"       ) );
	case SG_DATATYPE_ULong : return( _TL("unsigned 8 byte integer"     ) );
	case SG_DATATYPE_Long  : return( _TL("signed 8 byte integer"       ) );
	case SG_DATATYPE_Float : return( _TL("4 byte floating point number") );
	case SG_DATATYPE_Double: return( _TL("8 byte floating point number") );
	case SG_DATATYPE_String: return( _TL("string"                      ) );
	case SG_DATATYPE_Date  : return( _TL("date"                        ) );
	case SG_DATATYPE_Color : return( _TL("color"                       ) );
	case SG_DATATYPE_Binary: return( _TL("binary"                      ) );
	}
};

//---------------------------------------------------------
CSG_String	SG_Data_Type_Get_Identifier	(TSG_Data_Type Type)
{
	return( gSG_Data_Type_Identifier[Type] );
}

//---------------------------------------------------------
TSG_Data_Type SG_Data_Type_Get_Type(const CSG_String &Identifier)
{
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Bit   ]) )	return( SG_DATATYPE_Bit    );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Byte  ]) )	return( SG_DATATYPE_Byte   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Char  ]) )	return( SG_DATATYPE_Char   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Word  ]) )	return( SG_DATATYPE_Word   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Short ]) )	return( SG_DATATYPE_Short  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_DWord ]) )	return( SG_DATATYPE_DWord  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Int   ]) )	return( SG_DATATYPE_Int    );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_ULong ]) )	return( SG_DATATYPE_ULong  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Long  ]) )	return( SG_DATATYPE_Long   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Float ]) )	return( SG_DATATYPE_Float  );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Double]) )	return( SG_DATATYPE_Double );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_String]) )	return( SG_DATATYPE_String );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Date  ]) )	return( SG_DATATYPE_Date   );
	if( !Identifier.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Color ]) )	return( SG_DATATYPE_Color  );

	return( SG_DATATYPE_Undefined );
}

//---------------------------------------------------------
bool SG_Data_Type_is_Numeric(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit   :
	case SG_DATATYPE_Byte  :
	case SG_DATATYPE_Char  :
	case SG_DATATYPE_Word  :
	case SG_DATATYPE_Short :
	case SG_DATATYPE_DWord :
	case SG_DATATYPE_Int   :
	case SG_DATATYPE_ULong :
	case SG_DATATYPE_Long  :
	case SG_DATATYPE_Float :
	case SG_DATATYPE_Double:
		return( true );

	case SG_DATATYPE_String:
	case SG_DATATYPE_Date  :
	case SG_DATATYPE_Color :
	case SG_DATATYPE_Binary: default:
		return( false );
	}
}

//---------------------------------------------------------
bool SG_Data_Type_Range_Check(TSG_Data_Type Type, double &Value)
{
	double	min, max;

	switch( Type )
	{
		default:
		case SG_DATATYPE_Double: Value = (double)Value; return( true );
		case SG_DATATYPE_Float : Value = (float )Value; return( true );

		case SG_DATATYPE_Bit   : min =           0.; max =          1.; break;
		case SG_DATATYPE_Byte  : min =           0.; max =        255.; break;
		case SG_DATATYPE_Char  : min =        -128.; max =        127.; break;
		case SG_DATATYPE_Word  : min =           0.; max =      65535.; break;
		case SG_DATATYPE_Short : min =      -32768.; max =      32767.; break;
		case SG_DATATYPE_DWord : min =           0.; max = 4294967295.; break;
		case SG_DATATYPE_Int   : min = -2147483648.; max = 2147483647.; break;
	}

	if( Value < min )
	{
		Value	= min;
	}
	else if( Value > max )
	{
		Value	= max;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//                     Environment                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool SG_Initialize_Environment(bool bLibraries, bool bProjections, const SG_Char *Directory)
{
	SG_UI_ProgressAndMsg_Lock(true);

	//-----------------------------------------------------
	#ifdef _SAGA_MSW
	{
		wxString App_Path, Dll_Path, Path;

		if( Directory && SG_Dir_Exists(Directory) )
		{
			App_Path	= Directory;
		}
		else
		{
			App_Path	= SG_File_Get_Path(SG_UI_Get_Application_Path()).c_str();
		}

		Dll_Path	= App_Path + "\\dll";

		if( wxGetEnv("PATH", &Path) && !Path.IsEmpty() )
		{
			wxSetEnv("PATH", Dll_Path + ";" + Path);
		}
		else
		{
			wxSetEnv("PATH", Dll_Path);
		}

		wxSetEnv("GDAL_DRIVER_PATH", Dll_Path);
		wxSetEnv("GDAL_DATA"       , Dll_Path + "\\gdal-data");
		wxSetEnv("PROJ_LIB"        , Dll_Path + "\\proj-data");

		if( bLibraries )
		{
			SG_Get_Tool_Library_Manager().Add_Directory(SG_File_Make_Path(&App_Path, "tools"), false);
		}

		if( bProjections )
		{
			SG_Get_Projections().Load_Dictionary(SG_File_Make_Path(&App_Path, "saga_prj", "dic"));
			SG_Get_Projections().Load_DB        (SG_File_Make_Path(&App_Path, "saga_prj", "srs"));
		}
	}
	#else // #ifdef _SAGA_LINUX
	{
		if( bLibraries )
		{
			SG_Get_Tool_Library_Manager().Add_Directory(TOOLS_PATH);
			SG_Get_Tool_Library_Manager().Add_Directory(SG_File_Make_Path(SHARE_PATH, "toolchains"));	// look for tool chains
		}

		if( bProjections )
		{
			SG_Get_Projections().Load_Dictionary(SG_File_Make_Path(SHARE_PATH, "saga_prj", "dic"));
			SG_Get_Projections().Load_DB        (SG_File_Make_Path(SHARE_PATH, "saga_prj", "srs"));
		}
	}
	#endif
		
	//-----------------------------------------------------
	if( bLibraries )
	{
		wxString Path;

		if( wxGetEnv("SAGA_TLB", &Path) && SG_Dir_Exists(&Path) )
		{
			#ifdef _SAGA_MSW
				CSG_Strings	Paths = SG_String_Tokenize(&Path, ";" ); // colon (':') would split drive from paths!
			#else // #ifdef _SAGA_LINUX
				CSG_Strings	Paths = SG_String_Tokenize(&Path, ";:"); // colon (':') is more native to non-windows os than semi-colon (';'), we support both...
			#endif

			for(int i=0; i<Paths.Get_Count(); i++)
			{
				SG_Get_Tool_Library_Manager().Add_Directory(Paths[i]);
			}
		}
	}

	//-----------------------------------------------------
	SG_UI_ProgressAndMsg_Lock(false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
