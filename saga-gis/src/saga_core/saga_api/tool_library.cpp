
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
//                  tool_library.cpp                     //
//                                                       //
//          Copyright (C) 2006 by Olaf Conrad            //
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
#include <wx/dynlib.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/utils.h>

#include "tool_chain.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library::CSG_Tool_Library(void)
{
	m_pInterface = NULL;
	m_pLibrary   = NULL;
}

//---------------------------------------------------------
CSG_Tool_Library::CSG_Tool_Library(const CSG_String &File)
{
	m_pLibrary = new wxDynamicLibrary(SG_File_Get_Path_Absolute(File).c_str(), wxDL_DEFAULT|wxDL_QUIET);

	if(	m_pLibrary->IsLoaded()
	&&  m_pLibrary->HasSymbol(SYMBOL_TLB_Get_Interface)
	&&  m_pLibrary->HasSymbol(SYMBOL_TLB_Initialize)
	&&  m_pLibrary->HasSymbol(SYMBOL_TLB_Finalize)
	&&  ((TSG_PFNC_TLB_Initialize)m_pLibrary->GetSymbol(SYMBOL_TLB_Initialize))(File) )
	{
		m_pInterface = ((TSG_PFNC_TLB_Get_Interface)m_pLibrary->GetSymbol(SYMBOL_TLB_Get_Interface))();

		if( m_pInterface->Get_Count() > 0 )
		{
			m_File_Name    = m_pInterface->Get_Info(TLB_INFO_File   );
			m_Library_Name = m_pInterface->Get_Info(TLB_INFO_Library);

			return;	// success
		}
	}

	_Destroy();
}

//---------------------------------------------------------
CSG_Tool_Library::~CSG_Tool_Library(void)
{
	_Destroy();
}

//---------------------------------------------------------
bool CSG_Tool_Library::_Destroy(void)
{
	if( m_pLibrary )
	{
		if( m_pLibrary->IsLoaded() && m_pLibrary->HasSymbol(SYMBOL_TLB_Finalize) )
		{
			TSG_PFNC_TLB_Finalize TLB_Finalize = (TSG_PFNC_TLB_Finalize)m_pLibrary->GetSymbol(SYMBOL_TLB_Finalize);

			TLB_Finalize();
		}

		delete(m_pLibrary);

		m_pLibrary = NULL;
	}

	m_pInterface = NULL;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Library::Get_Info(int Type) const
{
	if( m_pInterface != NULL )
	{
		return( m_pInterface->Get_Info(Type) );
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Get_Tool(int Index, TSG_Tool_Type Type) const
{
	CSG_Tool *pTool = m_pInterface && Index >= 0 && Index < Get_Count() ? m_pInterface->Get_Tool(Index) : NULL;

	return(	pTool && (Type == TOOL_TYPE_Base || Type == pTool->Get_Type()) ? pTool : NULL );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Get_Tool(const char       *Name, TSG_Tool_Type Type) const	{	return( Get_Tool(CSG_String(Name), Type) );	}
CSG_Tool * CSG_Tool_Library::Get_Tool(const wchar_t    *Name, TSG_Tool_Type Type) const	{	return( Get_Tool(CSG_String(Name), Type) );	}
CSG_Tool * CSG_Tool_Library::Get_Tool(const CSG_String &Name, TSG_Tool_Type Type) const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool *pTool = Get_Tool(i, Type);

		if( pTool && (!pTool->Get_ID().Cmp(Name) || !pTool->Get_Name().Cmp(Name)) )
		{
			return( pTool );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Creates a new instance of the requested tool in addition
  * to the standard-wise created instance that you can request
  * with the 'Get_Tool() functions. Tools created with this
  * function are collected in a separate internal list and can
  * be removed from memory with the 'Delete_Tool()' function.
  * This way of tool creation is necessary, if you want to run
  * a tool simultaneously with different settings.
*/
//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Create_Tool(int Index, bool bWithGUI, bool bWithCMD)
{
	return( m_pInterface ? m_pInterface->Create_Tool(Index, bWithGUI, bWithCMD) : NULL );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Create_Tool(const char       *Name, bool bWithGUI, bool bWithCMD)	{	return( Create_Tool(CSG_String(Name), bWithGUI, bWithCMD) );	}
CSG_Tool * CSG_Tool_Library::Create_Tool(const wchar_t    *Name, bool bWithGUI, bool bWithCMD)	{	return( Create_Tool(CSG_String(Name), bWithGUI, bWithCMD) );	}
CSG_Tool * CSG_Tool_Library::Create_Tool(const CSG_String &Name, bool bWithGUI, bool bWithCMD)
{
	int Index; return( Name.asInt(Index) ? Create_Tool(Index, bWithGUI, bWithCMD) : NULL );
}

//---------------------------------------------------------
/**
  * Deletes a tool that has been created previously with a call
  * to the 'Create_Tool()' function.
*/
//---------------------------------------------------------
bool CSG_Tool_Library::Delete_Tool(CSG_Tool *pTool)
{
	return( m_pInterface ? m_pInterface->Delete_Tool(pTool) : false );
}

//---------------------------------------------------------
bool CSG_Tool_Library::Delete_Tools(void)
{
	return( m_pInterface ? m_pInterface->Delete_Tools() : false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Tool_Library::Get_Menu(int i) const
{
	if( Get_Tool(i) )
	{
		return( Get_Tool(i)->Get_MenuPath(true) );
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Tool_Library::Add_Reference(const CSG_String &Authors, const CSG_String &Year, const CSG_String &Title, const CSG_String &Where, const SG_Char *Link, const SG_Char *Link_Text)
{
	CSG_String Reference = Authors;

	Reference.Printf("<b>%s (%s):</b> %s. %s", Authors.c_str(), Year.c_str(), Title.c_str(), Where.c_str());

	if( Link && *Link )
	{
		Reference += CSG_String::Format(" <a href=\"%s\">%s</a>.", Link, Link_Text && *Link_Text ? Link_Text : Link);
	}

	if( !Reference.is_Empty() )
	{
		m_References += Reference;
	}

	m_References.Sort();
}

//---------------------------------------------------------
void CSG_Tool_Library::Add_Reference(const CSG_String &Link, const SG_Char *Link_Text)
{
	m_References += CSG_String::Format("<a href=\"%s\">%s</a>", Link.c_str(), Link_Text && *Link_Text ? Link_Text : Link.c_str());

	m_References.Sort();
}

//---------------------------------------------------------
void CSG_Tool_Library::Del_References(void)
{
	m_References.Clear();
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library_Manager g_Tool_Library_Manager;

//---------------------------------------------------------
CSG_Tool_Library_Manager &	SG_Get_Tool_Library_Manager	(void)
{
	return( g_Tool_Library_Manager );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library_Manager::CSG_Tool_Library_Manager(void)
{
	m_pLibraries = NULL;
	m_nLibraries = 0;

	if( this == &g_Tool_Library_Manager )
	{
		CSG_Random::Initialize(); // initialize with current time on startup
	}
}

//---------------------------------------------------------
CSG_Tool_Library_Manager::~CSG_Tool_Library_Manager(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Tool_Library_Manager::Get_Tool_Count(void)	const
{
	int nTools = 0;

	for(int i=0; i<m_nLibraries; i++)
	{
		nTools += m_pLibraries[i]->Get_Count();
	}

	return( nTools );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Add_Default_Libraries(bool bVerbose)
{
	if( bVerbose == false )
	{
		SG_UI_Msg_Lock(true);
	}

	//-----------------------------------------------------
	#if defined(_SAGA_MSW)
	{
		SG_Get_Tool_Library_Manager().Add_Directory(SG_File_Make_Path(SG_UI_Get_API_Path(), "tools"), false);
	}
	#elif defined(__WXMAC__)
	{
		if( SG_Get_Tool_Library_Manager().Add_Directory(SG_UI_Get_Application_Path(true) + "/../Tools", false) < 1 )
		{
			#ifdef TOOLS_PATH
				SG_Get_Tool_Library_Manager().Add_Directory(TOOLS_PATH);
			#endif

			#ifdef SHARE_PATH
				SG_Get_Tool_Library_Manager().Add_Directory(SG_File_Make_Path(SHARE_PATH, "toolchains")); // look for tool chains
			#endif
		}
	}
	#else // #if defined(_SAGA_LINUX)
	{
		#ifdef TOOLS_PATH
			SG_Get_Tool_Library_Manager().Add_Directory(TOOLS_PATH);
		#endif

		#ifdef SHARE_PATH
			SG_Get_Tool_Library_Manager().Add_Directory(SG_File_Make_Path(SHARE_PATH, "toolchains")); // look for tool chains
		#endif
	}
	#endif

	//-----------------------------------------------------
	CSG_String Paths;

	if( SG_Get_Environment("SAGA_TLB", &Paths) )
	{
		#if defined(_SAGA_MSW)
			CSG_Strings	Path = SG_String_Tokenize(Paths, ";" ); // colon (':') would split drive from paths!
		#else // #if defined(_SAGA_LINUX) || defined(__WXMAC__)
			CSG_Strings	Path = SG_String_Tokenize(Paths, ";:"); // colon (':') is more native to non-windows os than semi-colon (';'), we support both...
		#endif

		for(int i=0; i<Path.Get_Count(); i++)
		{
			SG_Get_Tool_Library_Manager().Add_Directory(Path[i]);
		}
	}

	//-----------------------------------------------------
	if( bVerbose == false )
	{
		SG_UI_Msg_Lock(false);
	}

	return( Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const char       *File) { return( Add_Library(CSG_String(File)) ); }
CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const wchar_t    *File) { return( Add_Library(CSG_String(File)) ); }
CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const CSG_String &File)
{
	if( SG_File_Exists(File) == false )
	{
		return( NULL );
	}

	if( SG_File_Cmp_Extension(File, "xml") )
	{
		return( _Add_Tool_Chain(File) );
	}

	if( SG_File_Cmp_Extension(File, "dll"  ) == false
	&&  SG_File_Cmp_Extension(File, "dylib") == false
	&&  SG_File_Cmp_Extension(File, "so"   ) == false )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	wxFileName FileName(File.c_str());

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Loading library"), File.c_str()), true);

	for(int i=0; i<Get_Count(); i++)
	{
		if( FileName == Get_Library(i)->Get_File_Name().c_str() )
		{
			SG_UI_Msg_Add(_TL("has already been loaded"), false);

			return( Get_Library(i) );
		}
	}

	//-----------------------------------------------------
	CSG_Tool_Library *pLibrary = new CSG_Tool_Library(File);

	if( pLibrary->is_Valid() )
	{
		m_pLibraries = (CSG_Tool_Library **)SG_Realloc(m_pLibraries, (m_nLibraries + 1) * sizeof(CSG_Tool_Library *));
		m_pLibraries[m_nLibraries++] = pLibrary;

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( pLibrary );
	}

	delete(pLibrary);

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( NULL );
}

//---------------------------------------------------------
int CSG_Tool_Library_Manager::Add_Directory(const char       *Directory, bool bOnlySubDirectories) { return( Add_Directory(CSG_String(Directory), bOnlySubDirectories) ); }
int CSG_Tool_Library_Manager::Add_Directory(const wchar_t    *Directory, bool bOnlySubDirectories) { return( Add_Directory(CSG_String(Directory), bOnlySubDirectories) ); }
int CSG_Tool_Library_Manager::Add_Directory(const CSG_String &Directory, bool bOnlySubDirectories)
{
	int nOpened = 0; wxDir Dir;

	if( Dir.Open(Directory.c_str()) )
	{
		wxString FileName, DirName(Dir.GetName());

		if( !bOnlySubDirectories && Dir.GetFirst(&FileName, wxEmptyString, wxDIR_FILES) )
		{
			do
			{	if( FileName.Find("saga_") < 0 && FileName.Find("wx") < 0 )
				if( Add_Library(SG_File_Make_Path(&DirName, &FileName)) )
				{
					nOpened++;
				}
			}
			while( Dir.GetNext(&FileName) );
		}

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_DIRS) )
		{
			do
			{
				if( FileName.CmpNoCase("dll") )
				{
					nOpened	+= Add_Directory(SG_File_Make_Path(&DirName, &FileName), false);
				}
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( nOpened );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::_Add_Tool_Chain(const CSG_String &File, bool bReload)
{
	if( !SG_File_Cmp_Extension(File, "xml") )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	CSG_Tool_Chains *pLibrary = NULL;
	CSG_Tool_Chain     *pTool = NULL;

	//-----------------------------------------------------
	wxFileName FileName(File.c_str()); // check if tool chain is already loaded ?

	for(int iLibrary=0; !pTool && iLibrary<Get_Count(); iLibrary++)
	{
		if( Get_Library(iLibrary)->Get_Type() == ESG_Library_Type::Chain )
		{
			for(int iTool=0; !pTool && iTool<Get_Library(iLibrary)->Get_Count(); iTool++)
			{
				if( FileName == ((CSG_Tool_Chain *)Get_Library(iLibrary)->Get_Tool(iTool))->Get_File_Name().c_str() )
				{
					pLibrary = (CSG_Tool_Chains *)Get_Library(iLibrary);
					pTool    = (CSG_Tool_Chain  *)Get_Library(iLibrary)->Get_Tool(iTool);
				}
			}
		}
	}

	if( pTool )
	{
		if( bReload ) // ...then try to reload !
		{
			SG_UI_ProgressAndMsg_Lock(true);
			CSG_Tool_Chain Tool(File); // don't reset loaded tool in case reloading fails!!!
			SG_UI_ProgressAndMsg_Lock(false);

			if( Tool.is_Okay() )
			{
				pTool->Create(File);
			}
		}

		return( pLibrary );
	}

	//-----------------------------------------------------
	pTool = new CSG_Tool_Chain(File);

	if( !pTool || !pTool->is_Okay() )
	{
		if( pTool )
		{
			delete(pTool);
		}

		return( NULL );
	}

	//-----------------------------------------------------
	CSG_String Library = pTool->Get_Library();

	for(int iLibrary=0; !pLibrary && iLibrary<Get_Count(); iLibrary++)
	{
		if( Get_Library(iLibrary)->Get_Type() == ESG_Library_Type::Chain
		&&  Get_Library(iLibrary)->Get_Library_Name().Cmp(Library) == 0 )
		{
			pLibrary = (CSG_Tool_Chains *)Get_Library(iLibrary);
		}
	}

	if( !pLibrary && (pLibrary = new CSG_Tool_Chains(pTool->Get_Library(), SG_File_Get_Path(File))) != NULL )
	{
		m_pLibraries = (CSG_Tool_Library **)SG_Realloc(m_pLibraries, (m_nLibraries + 1) * sizeof(CSG_Tool_Library *));
		m_pLibraries[m_nLibraries++] = pLibrary;
	}

	if( !pLibrary )	// this should never happen, but who knows...
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s %s: %s", _TL("ERROR"), _TL("tool chain library"), File.c_str()));

		delete(pTool);

		return( NULL );
	}

	pLibrary->Add_Tool(pTool);

	//-----------------------------------------------------
	return( pLibrary );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Destroy(void)
{
	if( m_pLibraries )
	{
		for(int i=0; i<Get_Count(); i++)
		{
//			#ifndef _SAGA_MSW
			if( !SG_UI_Get_Window_Main() && m_pLibraries[i]->m_pLibrary )
			{
				m_pLibraries[i]->m_pLibrary->Detach();
			}
//			#endif

			delete(m_pLibraries[i]);
		}

		SG_Free(m_pLibraries);

		m_pLibraries = NULL;
		m_nLibraries = 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Del_Library(CSG_Tool_Library *pLibrary)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pLibrary == Get_Library(i) )
		{
			return( Del_Library(i) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Del_Library(int i)
{
	if( i >= 0 && i < Get_Count() )
	{
		delete(m_pLibraries[i]);

		for(m_nLibraries--; i<m_nLibraries; i++)
		{
			m_pLibraries[i] = m_pLibraries[i + 1];
		}

		m_pLibraries = (CSG_Tool_Library **)SG_Realloc(m_pLibraries, m_nLibraries * sizeof(CSG_Tool_Library *));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::is_Loaded(CSG_Tool_Library *pLibrary) const
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( pLibrary == Get_Library(i) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const char       *Name, bool bLibrary, ESG_Library_Type Type) const { return( Get_Library(CSG_String(Name), bLibrary, Type) ); }
CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const wchar_t    *Name, bool bLibrary, ESG_Library_Type Type) const { return( Get_Library(CSG_String(Name), bLibrary, Type) ); }
CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const CSG_String &Name, bool bLibrary, ESG_Library_Type Type) const
{
	if( bLibrary )
	{
		SG_Get_Tool_Library_Manager()._Add_Library(Name);
	}

	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library *pLibrary = Get_Library(i);

		if( Type == ESG_Library_Type::Undefined || Type == pLibrary->Get_Type() )
		{
			if( !Name.Cmp(bLibrary ? pLibrary->Get_Library_Name() : pLibrary->Get_Name()) )
			{
				return( pLibrary );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::_Add_Library(const CSG_String &Library)
{
	int bOkay = false;

	SG_UI_ProgressAndMsg_Lock(true);

	//-----------------------------------------------------
	#if defined(_SAGA_MSW)
		if(  Add_Library       (         CSG_String::Format("%s\\tools\\%s.dll"      , SG_UI_Get_API_Path().c_str()            , Library.c_str())) ) { bOkay = true; }
		if( _Add_Library_Chains(Library, CSG_String::Format("%s\\tools\\toolchains"  , SG_UI_Get_API_Path().c_str()                             )) ) { bOkay = true; }
	#elif defined(__WXMAC__)
		if(  Add_Library       (         CSG_String::Format("%s/../Tools/lib%s.dylib", SG_UI_Get_Application_Path(true).c_str(), Library.c_str())) ) { bOkay = true; }
		if( _Add_Library_Chains(Library, CSG_String::Format("%s/../Tools"            , SG_UI_Get_Application_Path(true).c_str()                 )) ) { bOkay = true; }
		#ifdef TOOLS_PATH
		if(  Add_Library       (         CSG_String::Format("%s/lib%s.dylib"         , CSG_String(TOOLS_PATH).c_str()          , Library.c_str())) ) { bOkay = true; }
		#endif
		#ifdef SHARE_PATH
		if( _Add_Library_Chains(Library, CSG_String::Format("%s/toolchains"          , CSG_String(SHARE_PATH).c_str()                           )) ) { bOkay = true; }
		#endif
	#else // #if defined(_SAGA_LINUX)
		#ifdef TOOLS_PATH
		if(  Add_Library       (         CSG_String::Format("%s/lib%s.so"            , CSG_String(TOOLS_PATH).c_str()          , Library.c_str())) ) { bOkay = true; }
		#endif
		#ifdef SHARE_PATH
		if( _Add_Library_Chains(Library, CSG_String::Format("%s/toolchains"          , CSG_String(SHARE_PATH).c_str()                           )) ) { bOkay = true; }
		#endif
	#endif

	//-----------------------------------------------------
	CSG_String Paths;

	if( SG_Get_Environment("SAGA_TLB", &Paths) )
	{
		#if defined(_SAGA_MSW)
			const char *Format("%s\\%s.dll"    ); CSG_Strings Path = SG_String_Tokenize(Paths, ";" ); // colon (':') would split drive from paths!
		#elif defined(__WXMAC__)
			const char *Format("%s/lib%s.dylib"); CSG_Strings Path = SG_String_Tokenize(Paths, ";:"); // colon (':') is more native to non-windows os than semi-colon (';'), we support both...
		#else // #if defined(_SAGA_LINUX)
			const char *Format("%s/lib%s.so"   ); CSG_Strings Path = SG_String_Tokenize(Paths, ";:"); // colon (':') is more native to non-windows os than semi-colon (';'), we support both...
		#endif

		for(int i=0; i<Path.Get_Count(); i++)
		{
			if(  Add_Library       (CSG_String::Format(Format, Path[i].c_str(), Library.c_str())) ) { bOkay = true; }
			if( _Add_Library_Chains(Library                  , Path[i])                           ) { bOkay = true; }
		}
	}

	//-----------------------------------------------------
	SG_UI_ProgressAndMsg_Lock(false);

	return( bOkay );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::_Add_Library_Chains(const CSG_String &Library, const CSG_String &Directory)
{
	wxDir dir; wxString file; bool bOkay = false;

	if( dir.Open(Directory.c_str()) && dir.GetFirst(&file, wxEmptyString, wxDIR_FILES) )
	{
		do
		{
			CSG_String path(SG_File_Make_Path(Directory, &file)); CSG_Tool_Chain Tool(path);

			if( Tool.is_Okay() && Tool.Get_Library().Cmp(Library) == 0 && _Add_Tool_Chain(path, false) )
			{
				bOkay = true;
			}
		}
		while( dir.GetNext(&file) );
	}

	return( bOkay );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const char       *Library, int ID) const	{	return( Get_Tool(CSG_String(Library), ID) );	}
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const wchar_t    *Library, int ID) const	{	return( Get_Tool(CSG_String(Library), ID) );	}
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const CSG_String &Library, int ID) const
{
	return( Get_Tool(Library, CSG_String::Format("%d", ID)) );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const char       *Library, const char       *Name) const	{	return( Get_Tool(CSG_String(Library), CSG_String(Name)) );	}
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const wchar_t    *Library, const wchar_t    *Name) const	{	return( Get_Tool(CSG_String(Library), CSG_String(Name)) );	}
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const CSG_String &Library, const CSG_String &Name) const
{
	SG_Get_Tool_Library_Manager()._Add_Library(Library);

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Library(i)->Get_Library_Name().Cmp(Library) == 0 )
		{
			CSG_Tool *pTool = Get_Library(i)->Get_Tool(Name);

			if( pTool )
			{
				return( pTool );
			}
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const char       *Library, int              Index, bool bWithGUI, bool bWithCMD)	const	{	return( Create_Tool(CSG_String(Library), Index, bWithGUI, bWithCMD) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const wchar_t    *Library, int              Index, bool bWithGUI, bool bWithCMD)	const	{	return( Create_Tool(CSG_String(Library), Index, bWithGUI, bWithCMD) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const CSG_String &Library, int              Index, bool bWithGUI, bool bWithCMD)	const
{
	return( Create_Tool(Library, CSG_String::Format("%d", Index), bWithGUI, bWithCMD) );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const char       *Library, const char       *Name, bool bWithGUI, bool bWithCMD)	const	{	return( Create_Tool(CSG_String(Library), CSG_String(Name), bWithGUI, bWithCMD) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const wchar_t    *Library, const wchar_t    *Name, bool bWithGUI, bool bWithCMD)	const	{	return( Create_Tool(CSG_String(Library), CSG_String(Name), bWithGUI, bWithCMD) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const CSG_String &Library, const CSG_String &Name, bool bWithGUI, bool bWithCMD)	const
{
	SG_Get_Tool_Library_Manager()._Add_Library(Library);

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Library(i)->Get_Library_Name().Cmp(Library) == 0 )
		{
			CSG_Tool *pTool = Get_Library(i)->Create_Tool(Name, bWithGUI, bWithCMD);

			if( pTool )
			{
				return( pTool );
			}
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Delete_Tool(CSG_Tool *pTool) const
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Library(i)->Delete_Tool(pTool) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Tool_Library_Manager::Create_Python_ToolBox(const CSG_String &Destination, bool bClean, bool bName, bool bSingleFile) const
{
	const char *Header =
		"#! /usr/bin/env python\n\n"
		"'''\n"
		"Python Interface to SAGA Tools Library\n"
		"----------\n"
		"- Category : %s\n"
		"- Name     : %s\n"
		"- ID       : %s\n\n"
		"Description\n"
		"----------\n%s\n"
		"'''\n\n"
		"from PySAGA.helper import Tool_Wrapper\n\n";

	CSG_File Stream;

	if( bSingleFile )
	{
		if( SG_File_Exists(Destination) )
		{
			SG_File_Delete(Destination);
		}

		if( !Stream.Open(Destination, SG_FILE_W, false, SG_FILE_ENCODING_UTF8) )
		{
			SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("failed to create destination file"), Destination.c_str()));

			return( false );
		}

		Stream.Write("#! /usr/bin/env python\n\n'''\n");
		Stream.Write("Python Interface to SAGA Tools\n");
		Stream.Write("----------\n'''\n\n");
		Stream.Write("from PySAGA.helper import Tool_Wrapper\n\n");
	}
	else
	{
		if( !SG_Dir_Exists(Destination) && !SG_Dir_Create(Destination, true) )
		{
			SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("failed to create destination folder"), Destination.c_str()));

			return( false );
		}

		if( bClean )
		{
			CSG_Strings Files;

			SG_Dir_List_Files(Files, Destination);

			for(int i=0; i<Files.Get_Count(); i++)
			{
				SG_File_Delete(Files[i]);
			}
		}
	}

	SG_UI_Set_Application_Name("python");

	//-----------------------------------------------------
	// skip tool chains in 1st run...
	for(int iLibrary=0; iLibrary<Get_Count() && SG_UI_Process_Set_Progress(iLibrary, Get_Count()); iLibrary++)
	{
		CSG_Tool_Library *pLibrary = Get_Library(iLibrary);

		if( !pLibrary->Get_Category    ().Cmp("SAGA Development" ) // generally exclude certain categories/libraries
		||  !pLibrary->Get_Category    ().Cmp("Garden"           )
		||  !pLibrary->Get_Library_Name().Cmp("grid_calculus_bsl")
		||  !pLibrary->Get_Library_Name().Cmp("pj_geotrans"      )
		||  !pLibrary->Get_Library_Name().Cmp("vis_3d_viewer"    )
		||   pLibrary->Get_Type() == ESG_Library_Type::Chain     ) // exclude tool chains in 1st run
		{
			continue;
		}

		SG_UI_Process_Set_Text(CSG_String::Format("%s: %s", SG_T("Library"), pLibrary->Get_Library_Name().c_str()));

		if( !bSingleFile )
		{
			if( !Stream.Open(SG_File_Make_Path(Destination, pLibrary->Get_Library_Name(), "py"), SG_FILE_W, false, SG_FILE_ENCODING_UTF8) )
			{
				continue;
			}

			Stream.Printf(Header, pLibrary->Get_Category().c_str(), pLibrary->Get_Name().c_str(), pLibrary->Get_Library_Name().c_str(), SG_HTML_Tag_Replacer(pLibrary->Get_Description()).c_str());
		}

		for(int iTool=0; iTool<pLibrary->Get_Count(); iTool++)
		{
			CSG_Tool *pTool = pLibrary->Create_Tool(pLibrary->Get_Tool(iTool)->Get_ID(), false, false);

			if( pTool && !pTool->needs_GUI() && !pTool->is_Interactive() && pTool->Get_Parameters_Count() == 0 )
			{
				if( bName )
				{
					Stream.Write(pTool->Get_Script(CSG_Tool::Script_Format::Python_Wrapper_Func_Name, false));
				}

				Stream.Write(pTool->Get_Script(CSG_Tool::Script_Format::Python_Wrapper_Func_ID, false));
			}

			pLibrary->Delete_Tool(pTool);
		}
	}

	//-----------------------------------------------------
	// add tool chains in 2nd run...
	for(int iLibrary=0; iLibrary<Get_Count() && SG_UI_Process_Set_Progress(iLibrary, Get_Count()); iLibrary++)
	{
		CSG_Tool_Library *pLibrary = Get_Library(iLibrary);

		if( !pLibrary->Get_Category().Cmp("SAGA Development" ) // generally exclude certain categories
		||  !pLibrary->Get_Category().Cmp("Garden"           )
		||   pLibrary->Get_Type() != ESG_Library_Type::Chain ) // only process tool chains in 2nd run
		{
			continue;
		}

		SG_UI_Process_Set_Text(CSG_String::Format("%s: %s", SG_T("Library"), pLibrary->Get_Library_Name().c_str()));

		if( !bSingleFile )
		{
			CSG_String File(SG_File_Make_Path(Destination, pLibrary->Get_Library_Name(), "py"));

			if( SG_File_Exists(File) )
			{
				if( !Stream.Open(File, SG_FILE_RW, false, SG_FILE_ENCODING_UTF8) )
				{
					continue;
				}

				Stream.Seek_End();
			}
			else
			{
				if( !Stream.Open(File, SG_FILE_W, false, SG_FILE_ENCODING_UTF8) )
				{
					continue;
				}

				Stream.Printf(Header, pLibrary->Get_Category().c_str(), pLibrary->Get_Name().c_str(), pLibrary->Get_Library_Name().c_str(), SG_HTML_Tag_Replacer(pLibrary->Get_Description()).c_str());
			}
		}

		for(int iTool=0; iTool<pLibrary->Get_Count(); iTool++)
		{
			CSG_Tool *pTool = pLibrary->Create_Tool(pLibrary->Get_Tool(iTool)->Get_ID(), false, false);

			if( pTool && !pTool->needs_GUI() && !pTool->is_Interactive() && pTool->Get_Parameters_Count() == 0 )
			{
				if( bName )
				{
					Stream.Write(pTool->Get_Script(CSG_Tool::Script_Format::Python_Wrapper_Func_Name, false));
				}

				Stream.Write(pTool->Get_Script(CSG_Tool::Script_Format::Python_Wrapper_Func_ID, false));
			}

			pLibrary->Delete_Tool(pTool);
		}
	}

	//-----------------------------------------------------
	SG_UI_Set_Application_Name(); // reset

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
