
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library::CSG_Tool_Library(void)
{
	m_pInterface	= NULL;
	m_pLibrary		= NULL;
}

//---------------------------------------------------------
CSG_Tool_Library::CSG_Tool_Library(const CSG_String &File)
{
	m_pLibrary	= new wxDynamicLibrary(SG_File_Get_Path_Absolute(File).c_str(), wxDL_DEFAULT|wxDL_QUIET);

	if(	m_pLibrary->IsLoaded()
	&&	m_pLibrary->HasSymbol(SYMBOL_TLB_Get_Interface)
	&&	m_pLibrary->HasSymbol(SYMBOL_TLB_Initialize)
	&&	m_pLibrary->HasSymbol(SYMBOL_TLB_Finalize)
	&&	((TSG_PFNC_TLB_Initialize)m_pLibrary->GetSymbol(SYMBOL_TLB_Initialize))(File) )
	{
		m_pInterface	= ((TSG_PFNC_TLB_Get_Interface)m_pLibrary->GetSymbol(SYMBOL_TLB_Get_Interface))();

		if( m_pInterface->Get_Count() > 0 )
		{
			m_File_Name		= m_pInterface->Get_Info(TLB_INFO_File   );
			m_Library_Name	= m_pInterface->Get_Info(TLB_INFO_Library);

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
			TSG_PFNC_TLB_Finalize	TLB_Finalize	= (TSG_PFNC_TLB_Finalize)m_pLibrary->GetSymbol(SYMBOL_TLB_Finalize);

			TLB_Finalize();
		}

		delete(m_pLibrary);

		m_pLibrary	= NULL;
	}

	m_pInterface	= NULL;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
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
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Get_Tool(int Index, TSG_Tool_Type Type) const
{
	CSG_Tool	*pTool	= m_pInterface && Index >= 0 && Index < Get_Count() ? m_pInterface->Get_Tool(Index) : NULL;

	return(	pTool && (Type == TOOL_TYPE_Base || Type == pTool->Get_Type()) ? pTool : NULL );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Get_Tool(const char       *Name, TSG_Tool_Type Type) const	{	return( Get_Tool(CSG_String(Name), Type) );	}
CSG_Tool * CSG_Tool_Library::Get_Tool(const wchar_t    *Name, TSG_Tool_Type Type) const	{	return( Get_Tool(CSG_String(Name), Type) );	}
CSG_Tool * CSG_Tool_Library::Get_Tool(const CSG_String &Name, TSG_Tool_Type Type) const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool	*pTool	= Get_Tool(i, Type);

		if( pTool && (!pTool->Get_ID().Cmp(Name) || !pTool->Get_Name().Cmp(Name)) )
		{
			return( pTool );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
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
CSG_Tool * CSG_Tool_Library::Create_Tool(int Index, bool bWithGUI)
{
	return( m_pInterface ? m_pInterface->Create_Tool(Index, bWithGUI) : NULL );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Create_Tool(const char       *Name, bool bWithGUI)	{	return( Create_Tool(CSG_String(Name), bWithGUI) );	}
CSG_Tool * CSG_Tool_Library::Create_Tool(const wchar_t    *Name, bool bWithGUI)	{	return( Create_Tool(CSG_String(Name), bWithGUI) );	}
CSG_Tool * CSG_Tool_Library::Create_Tool(const CSG_String &Name, bool bWithGUI)
{
	int	Index;	return( Name.asInt(Index) ? Create_Tool(Index, bWithGUI) : NULL );
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
//														 //
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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library_Manager		g_Tool_Library_Manager;

//---------------------------------------------------------
CSG_Tool_Library_Manager &	SG_Get_Tool_Library_Manager	(void)
{
	return( g_Tool_Library_Manager );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library_Manager::CSG_Tool_Library_Manager(void)
{
	m_pLibraries	= NULL;
	m_nLibraries	= 0;

	if( this == &g_Tool_Library_Manager )
	{
		CSG_Random::Initialize();	// initialize with current time on startup
	}
}

//---------------------------------------------------------
CSG_Tool_Library_Manager::~CSG_Tool_Library_Manager(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const CSG_String &File)
{
	//-----------------------------------------------------
	if( !SG_File_Cmp_Extension(File, "mlb"  )
	&&	!SG_File_Cmp_Extension(File, "dll"  )
	&&	!SG_File_Cmp_Extension(File, "so"   )
	&&	!SG_File_Cmp_Extension(File, "dylib") )
	{
		return( _Add_Tool_Chain(File) );
	}

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Loading library"), File.c_str()), true);

	wxFileName	fn(File.c_str());

	for(int i=0; i<Get_Count(); i++)
	{
		if( fn == Get_Library(i)->Get_File_Name().c_str() )
		{
			SG_UI_Msg_Add(_TL("has already been loaded"), false);

			return( NULL );
		}
	}

	//-----------------------------------------------------
	CSG_Tool_Library	*pLibrary	= new CSG_Tool_Library(File);

	if( pLibrary->is_Valid() )
	{
		m_pLibraries	= (CSG_Tool_Library **)SG_Realloc(m_pLibraries, (m_nLibraries + 1) * sizeof(CSG_Tool_Library *));
		m_pLibraries[m_nLibraries++]	= pLibrary;

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( pLibrary );
	}

	delete(pLibrary);

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( NULL );
}

CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const char       *File)
{
	return( Add_Library(CSG_String(File)) );
}

CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const wchar_t    *File)
{
	return( Add_Library(CSG_String(File)) );
}

//---------------------------------------------------------
int CSG_Tool_Library_Manager::Add_Directory(const CSG_String &Directory, bool bOnlySubDirectories)
{
	int		nOpened	= 0;
	wxDir	Dir;

	if( Dir.Open(Directory.c_str()) )
	{
		wxString	FileName, DirName(Dir.GetName());

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

int CSG_Tool_Library_Manager::Add_Directory(const char       *Directory, bool bOnlySubDirectories)
{
	return( Add_Directory(CSG_String(Directory), bOnlySubDirectories) );
}

int CSG_Tool_Library_Manager::Add_Directory(const wchar_t    *Directory, bool bOnlySubDirectories)
{
	return( Add_Directory(CSG_String(Directory), bOnlySubDirectories) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::_Add_Tool_Chain(const CSG_String &File)
{
	//-----------------------------------------------------
	if( !SG_File_Cmp_Extension(File, "xml") )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	CSG_Tool_Chains	*pLibrary	= NULL;
	CSG_Tool_Chain	*pTool		= NULL;

	//-----------------------------------------------------
	{	// is tool chain already loaded ?
		wxFileName	fn(File.c_str());

		for(int iLibrary=0; !pTool && iLibrary<Get_Count(); iLibrary++)
		{
			if( Get_Library(iLibrary)->Get_Type() == TOOL_CHAINS )
			{
				for(int iTool=0; !pTool && iTool<Get_Library(iLibrary)->Get_Count(); iTool++)
				{
					if( fn == ((CSG_Tool_Chain *)Get_Library(iLibrary)->Get_Tool(iTool))->Get_File_Name().c_str() )
					{
						pLibrary	= (CSG_Tool_Chains *)Get_Library(iLibrary);
						pTool		= (CSG_Tool_Chain  *)Get_Library(iLibrary)->Get_Tool(iTool);
					}
				}
			}
		}

		if( pTool )	// ...then try to reload !
		{
			SG_UI_ProgressAndMsg_Lock(true);
			CSG_Tool_Chain	Tool(File);	// don't reset loaded tool in case reloading fails!!!
			SG_UI_ProgressAndMsg_Lock(false);

			if( Tool.is_Okay() )
			{
				pTool->Create(File);
			}

			return( pLibrary );
		}
	}

	//-----------------------------------------------------
	pTool	= new CSG_Tool_Chain(File);

	if( !pTool || !pTool->is_Okay() )
	{
		if( pTool )
		{
			delete(pTool);
		}

		return( NULL );
	}

	//-----------------------------------------------------
	CSG_String	Library	= pTool->Get_Library();	if( Library.is_Empty() )	Library	= "toolchains";

	for(int iLibrary=0; !pLibrary && iLibrary<Get_Count(); iLibrary++)
	{
		if( Get_Library(iLibrary)->Get_Type() == TOOL_CHAINS
		&&  Get_Library(iLibrary)->Get_Library_Name().Cmp(Library) == 0 )
		{
			pLibrary	= (CSG_Tool_Chains *)Get_Library(iLibrary);
		}
	}

	if( !pLibrary && (pLibrary = new CSG_Tool_Chains(pTool->Get_Library(), SG_File_Get_Path(File))) != NULL )
	{
		m_pLibraries	= (CSG_Tool_Library **)SG_Realloc(m_pLibraries, (m_nLibraries + 1) * sizeof(CSG_Tool_Library *));
		m_pLibraries[m_nLibraries++]	= pLibrary;
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
//														 //
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

		m_pLibraries	= NULL;
		m_nLibraries	= 0;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
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
			m_pLibraries[i]	= m_pLibraries[i + 1];
		}

		m_pLibraries	= (CSG_Tool_Library **)SG_Realloc(m_pLibraries, m_nLibraries * sizeof(CSG_Tool_Library *));

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const CSG_String &Name, bool bLibrary)	const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library	*pLibrary	= Get_Library(i);

		if( !Name.Cmp(bLibrary ? pLibrary->Get_Library_Name() : pLibrary->Get_Name()) )
		{
			return( pLibrary );
		}
	}

	return( NULL );
}

CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const char *Name, bool bLibrary)	const
{
	return( Get_Library(CSG_String(Name), bLibrary) );
}

CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const wchar_t *Name, bool bLibrary)	const
{
	return( Get_Library(CSG_String(Name), bLibrary) );
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
//														 //
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
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library	*pLibrary	= Get_Library(i);

		if( pLibrary->Get_Library_Name().Cmp(Library) == 0 )
		{
			CSG_Tool	*pTool	= pLibrary->Get_Tool(Name);

			if( pTool )
			{
				return( pTool );
			}
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const char       *Library, int              Index, bool bWithGUI)	const	{	return( Create_Tool(CSG_String(Library), Index, bWithGUI) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const wchar_t    *Library, int              Index, bool bWithGUI)	const	{	return( Create_Tool(CSG_String(Library), Index, bWithGUI) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const CSG_String &Library, int              Index, bool bWithGUI)	const
{
	return( Create_Tool(Library, CSG_String::Format("%d", Index), bWithGUI) );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const char       *Library, const char       *Name, bool bWithGUI)	const	{	return( Create_Tool(CSG_String(Library), CSG_String(Name), bWithGUI) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const wchar_t    *Library, const wchar_t    *Name, bool bWithGUI)	const	{	return( Create_Tool(CSG_String(Library), CSG_String(Name), bWithGUI) );	}
CSG_Tool * CSG_Tool_Library_Manager::Create_Tool(const CSG_String &Library, const CSG_String &Name, bool bWithGUI)	const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library	*pLibrary	= Get_Library(i);

		if( pLibrary->Get_Library_Name().Cmp(Library) == 0 )
		{
			CSG_Tool	*pTool	= pLibrary->Create_Tool(Name, bWithGUI);

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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
