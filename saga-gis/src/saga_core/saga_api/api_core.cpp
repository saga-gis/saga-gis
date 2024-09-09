
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
#include <wx/app.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>

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
CSG_String	SG_Data_Type_Get_Name	(TSG_Data_Type Type, bool bShort)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit   : return(          _TL("bit"   ) );
	case SG_DATATYPE_Byte  : return( bShort ? _TL("uint8" ) : _TL("unsigned 1 byte integer"     ) );
	case SG_DATATYPE_Char  : return( bShort ? _TL("int8"  ) : _TL("signed 1 byte integer"       ) );
	case SG_DATATYPE_Word  : return( bShort ? _TL("uint16") : _TL("unsigned 2 byte integer"     ) );
	case SG_DATATYPE_Short : return( bShort ? _TL("int16" ) : _TL("signed 2 byte integer"       ) );
	case SG_DATATYPE_DWord : return( bShort ? _TL("uint32") : _TL("unsigned 4 byte integer"     ) );
	case SG_DATATYPE_Int   : return( bShort ? _TL("int32" ) : _TL("signed 4 byte integer"       ) );
	case SG_DATATYPE_ULong : return( bShort ? _TL("uint64") : _TL("unsigned 8 byte integer"     ) );
	case SG_DATATYPE_Long  : return( bShort ? _TL("int64" ) : _TL("signed 8 byte integer"       ) );
	case SG_DATATYPE_Float : return( bShort ? _TL("float" ) : _TL("4 byte floating point number") );
	case SG_DATATYPE_Double: return( bShort ? _TL("double") : _TL("8 byte floating point number") );
	case SG_DATATYPE_String: return(          _TL("string") );
	case SG_DATATYPE_Date  : return(          _TL("date"  ) );
	case SG_DATATYPE_Color : return(          _TL("color" ) );
	case SG_DATATYPE_Binary: return(          _TL("binary") );
	default                : return( bShort ? _TL("none"  ) : _TL("undefined"                   ) );
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
int	SG_Data_Type_Get_Flag(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_Bit   : return( SG_DATATYPES_Bit       );
	case SG_DATATYPE_Byte  : return( SG_DATATYPES_Byte      );
	case SG_DATATYPE_Char  : return( SG_DATATYPES_Char      );
	case SG_DATATYPE_Word  : return( SG_DATATYPES_Word      );
	case SG_DATATYPE_Short : return( SG_DATATYPES_Short     );
	case SG_DATATYPE_DWord : return( SG_DATATYPES_DWord     );
	case SG_DATATYPE_Int   : return( SG_DATATYPES_Int       );
	case SG_DATATYPE_ULong : return( SG_DATATYPES_ULong     );
	case SG_DATATYPE_Long  : return( SG_DATATYPES_Long      );
	case SG_DATATYPE_Float : return( SG_DATATYPES_Float     );
	case SG_DATATYPE_Double: return( SG_DATATYPES_Double    );
	case SG_DATATYPE_String: return( SG_DATATYPES_String    );
	case SG_DATATYPE_Date  : return( SG_DATATYPES_Date      );
	case SG_DATATYPE_Color : return( SG_DATATYPES_Color     );
	case SG_DATATYPE_Binary: return( SG_DATATYPES_Binary    );
	default                : return( SG_DATATYPES_Undefined );
	}
};

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
/**
* This class is for internal use only. It has a single instance
* (g_App_Initialize) that is used to make a call to wxInitialize()
* when SG_Initialize_Environment() is called with bInitializeWX = true
* (default) to ensure wxWidgets is fully working, even if SAGA API
* is used outside of SAGA's GUI or CMD (i.e. from Python).
* You might want to call SG_Uninitialize_Environment() before you
* exit your application to invoke wxUninitialize(),
* what is suggested to be done by wxWidgets documentation.
*/ 
class CSG_App_Initialize
{
public:
	CSG_App_Initialize(void)
	{
		m_Initialized = 0;
	}

	virtual ~CSG_App_Initialize(void)
	{}

	bool	Initialize		(void)
	{
		if( wxInitialize() )
		{
			m_Initialized++;

			return( true );
		}

		return( false );
	}

	bool	Uninitialize	(void)
	{
		if( m_Initialized > 0 )
		{
			wxUninitialize();

			m_Initialized--;

			return( true );
		}

		return( false );
	}

	bool	Uninitialize	(bool bAll)
	{
		if( bAll )
		{
			while( m_Initialized > 0 )
			{
				Uninitialize();
			}
		}
		else
		{
			Uninitialize();
		}

		return( true );
	}


private:

	int		m_Initialized;

};

//---------------------------------------------------------
CSG_App_Initialize	g_App_Initialize;


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String	g_SAGA_API_Path;

CSG_String	SG_UI_Get_API_Path	(void)
{
	if( g_SAGA_API_Path.is_Empty() && SG_UI_Get_Application_Name().Find("saga") == 0 )
	{
		g_SAGA_API_Path = SG_UI_Get_Application_Path(true);
	}

	return( g_SAGA_API_Path );
}

//---------------------------------------------------------
CSG_String	SG_UI_Get_Application_Path	(bool bPathOnly)
{
	static CSG_String App_Path;

	if( App_Path.is_Empty() )
	{
		App_Path = wxStandardPaths::Get().GetExecutablePath().wc_str();
	}

	CSG_String Path(App_Path);

	if( bPathOnly )
	{
		Path = SG_File_Get_Path(App_Path);
	}

	return( SG_File_Get_Path_Absolute(Path) );
}

//---------------------------------------------------------
CSG_String	g_App_Name;

CSG_String	SG_UI_Get_Application_Name	(void)
{
	if( g_App_Name.is_Empty() )
	{
		g_App_Name = wxApp::GetInstance()->GetAppName().wc_str();
	}

	return( g_App_Name );
}

CSG_String	SG_UI_Set_Application_Name	(const CSG_String &Name)
{
	CSG_String oldName(g_App_Name);

	g_App_Name = Name;

	return( oldName );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool SG_Add_Dll_Paths(const wxString &Directory, wxString &Paths)
{
	wxDir Dir(Directory);

	if( Dir.IsOpened() )
	{
		wxString Path;

		if( Dir.GetFirst(&Path, "*.dll", wxDIR_HIDDEN|wxDIR_FILES) )
		{
			if( !Path.IsEmpty() )
			{
				Paths += ";";
			}

			Paths += Directory;
		}

		if( Dir.GetFirst(&Path, "gdal_netCDF.dll", wxDIR_HIDDEN|wxDIR_FILES) )
		{
			wxSetEnv("GDAL_DRIVER_PATH", Directory);
		}

		if( Dir.GetFirst(&Path, "pdalcpp.dll", wxDIR_HIDDEN | wxDIR_FILES) )
		{
			wxSetEnv("PDAL_DRIVER_PATH", Directory);
		}

		if( Dir.GetFirst(&Path, wxEmptyString, wxDIR_HIDDEN|wxDIR_DIRS) )
		{
			do
			{
				wxString SubDir(Directory + "\\" + Path);

				if( !Path.CmpNoCase("gdal-plugins") ) { wxSetEnv("GDAL_DRIVER_PATH", SubDir); } else
				if( !Path.CmpNoCase("gdal-data"   ) ) { wxSetEnv("GDAL_DATA"       , SubDir); } else
				if( !Path.CmpNoCase("proj-data"   ) ) { wxSetEnv("PROJ_LIB"        , SubDir); } else
				{
					SG_Add_Dll_Paths(SubDir, Paths);
				}
			}
			while( Dir.GetNext(&Path) );
		}
	}

	return( !Paths.IsEmpty() );
}

//---------------------------------------------------------
bool SG_Initialize_Environment(bool bLibraries, bool bProjections, const SG_Char *SAGA_Path, bool bInitializeWX)
{
	#ifdef _DEBUG
	SG_UI_Console_Print_StdOut("\n!!! Loading SAGA API => DEBUG !!!");
	#endif

	if( bInitializeWX )
	{
		g_App_Initialize.Initialize();
	}

	//-----------------------------------------------------
	static bool bInitialized = false;

	if( bInitialized == false )
	{
		bInitialized = true;

		SG_UI_ProgressAndMsg_Lock(true);

		#if defined(_SAGA_MSW)
		{
			wxString App_Path, Dll_Paths, System_Paths;

			if( SAGA_Path && SG_Dir_Exists(SAGA_Path) )
			{
				App_Path = SAGA_Path;
			}
			else
			{
				App_Path = SG_UI_Get_Application_Path(true).c_str();
			}

			g_SAGA_API_Path = &App_Path;

			wxGetEnv("PATH", &System_Paths);

			if( SG_Add_Dll_Paths(App_Path + "\\dll", Dll_Paths) )
			{
				Dll_Paths += ';' + App_Path + ';' + System_Paths;

				wxSetEnv("PATH", Dll_Paths);
			}
		}
		#elif defined(__WXMAC__)
		{
			CSG_String App_Path(SG_UI_Get_Application_Path(true));

			if( SG_Dir_Exists(App_Path + "/proj-data") ) { wxSetEnv("PROJ_LIB" , wxString::Format("%s/proj-data", App_Path.c_str())); }
			if( SG_Dir_Exists(App_Path + "/gdal-data") ) { wxSetEnv("GDAL_DATA", wxString::Format("%s/gdal-data", App_Path.c_str())); }
		}
		#else // #if defined(_SAGA_LINUX)
		{
		}
		#endif

		SG_Get_Projections().Create(bProjections);

		SG_UI_ProgressAndMsg_Lock(false);
	}

	//-----------------------------------------------------
	if( bLibraries )
	{
		SG_UI_ProgressAndMsg_Lock(true);

		SG_Get_Tool_Library_Manager().Add_Default_Libraries();

		SG_UI_ProgressAndMsg_Lock(false);
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool SG_Uninitialize_Environment(void)
{
	g_App_Initialize.Uninitialize(true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
