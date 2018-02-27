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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
CSG_Tool_Library::CSG_Tool_Library(const CSG_String &FileName)
{
	m_pLibrary	= new wxDynamicLibrary(SG_File_Get_Path_Absolute(FileName).c_str(), wxDL_DEFAULT|wxDL_QUIET);

	if(	m_pLibrary->IsLoaded()
	&&	m_pLibrary->HasSymbol(SYMBOL_TLB_Get_Interface)
	&&	m_pLibrary->HasSymbol(SYMBOL_TLB_Initialize)
	&&	m_pLibrary->HasSymbol(SYMBOL_TLB_Finalize)
	&&	((TSG_PFNC_TLB_Initialize)m_pLibrary->GetSymbol(SYMBOL_TLB_Initialize))(FileName) )
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
	CSG_Tool	*pTool	= Index >= 0 && Index < Get_Count() ? m_pInterface->Get_Tool(Index) : NULL;

	return(	pTool && (Type == TOOL_TYPE_Base || Type == pTool->Get_Type()) ? pTool : NULL );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library::Get_Tool(const CSG_String &Tool, TSG_Tool_Type Type) const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool	*pTool	= Get_Tool(i, Type);

		if( pTool && (!pTool->Get_ID().Cmp(Tool) || !pTool->Get_Name().Cmp(Tool)) )
		{
			return( pTool );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Tool_Grid * CSG_Tool_Library::Get_Tool_Grid(int Index) const
{	return( (CSG_Tool_Grid *)Get_Tool(Index , TOOL_TYPE_Grid) );	}

CSG_Tool_Grid * CSG_Tool_Library::Get_Tool_Grid(const CSG_String &Tool) const
{	return( (CSG_Tool_Grid *)Get_Tool(Tool, TOOL_TYPE_Grid) );	}

//---------------------------------------------------------
CSG_Tool_Interactive * CSG_Tool_Library::Get_Tool_Interactive(int Index) const
{	return( (CSG_Tool_Interactive *)Get_Tool(Index , TOOL_TYPE_Interactive) );	}

CSG_Tool_Interactive * CSG_Tool_Library::Get_Tool_Interactive(const CSG_String &Tool) const
{	return( (CSG_Tool_Interactive *)Get_Tool(Tool, TOOL_TYPE_Interactive) );	}

//---------------------------------------------------------
CSG_Tool_Grid_Interactive * CSG_Tool_Library::Get_Tool_Grid_Interactive(int Index) const
{	return( (CSG_Tool_Grid_Interactive *)Get_Tool(Index , TOOL_TYPE_Grid_Interactive) );	}

CSG_Tool_Grid_Interactive * CSG_Tool_Library::Get_Tool_Grid_Interactive(const CSG_String &Tool) const
{	return( (CSG_Tool_Grid_Interactive *)Get_Tool(Tool, TOOL_TYPE_Grid_Interactive) );	}


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
CSG_Tool_Library * CSG_Tool_Library_Manager::Add_Library(const SG_Char *FileName)
{
	//-----------------------------------------------------
	if( !SG_File_Cmp_Extension(FileName, SG_T("mlb"  ))
	&&	!SG_File_Cmp_Extension(FileName, SG_T("dll"  ))
	&&	!SG_File_Cmp_Extension(FileName, SG_T("so"   ))
	&&	!SG_File_Cmp_Extension(FileName, SG_T("dylib")) )
	{
		return( _Add_Tool_Chain(FileName) );
	}

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Load library"), FileName), true);

	wxFileName	fn(FileName);

	for(int i=0; i<Get_Count(); i++)
	{
		if( fn == Get_Library(i)->Get_File_Name().c_str() )
		{
			SG_UI_Msg_Add(_TL("has already been loaded"), false);

			return( NULL );
		}
	}

	//-----------------------------------------------------
	CSG_Tool_Library	*pLibrary	= new CSG_Tool_Library(FileName);

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

//---------------------------------------------------------
int CSG_Tool_Library_Manager::Add_Directory(const SG_Char *Directory, bool bOnlySubDirectories)
{
	int		nOpened	= 0;
	wxDir	Dir;

	if( Dir.Open(Directory) )
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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Tool_Library * CSG_Tool_Library_Manager::_Add_Tool_Chain(const SG_Char *FileName)
{
	//-----------------------------------------------------
	if( !SG_File_Cmp_Extension(FileName, SG_T("xml" )) )
	{
		return( NULL );
	}

	//-----------------------------------------------------
	CSG_Tool_Chains	*pLibrary	= NULL;
	CSG_Tool_Chain	*pTool		= NULL;

	//-----------------------------------------------------
	{	// is tool chain already loaded ?
		wxFileName	fn(FileName);

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
			CSG_Tool_Chain	Tool(FileName);	// don't reset loaded tool in case reloading fails!!!
			SG_UI_ProgressAndMsg_Lock(false);

			if( Tool.is_Okay() )
			{
				pTool->Create(FileName);
			}

			return( pLibrary );
		}
	}

	//-----------------------------------------------------
	pTool	= new CSG_Tool_Chain(FileName);

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

	if( !pLibrary && (pLibrary = new CSG_Tool_Chains(pTool->Get_Library(), SG_File_Get_Path(FileName))) != NULL )
	{
		m_pLibraries	= (CSG_Tool_Library **)SG_Realloc(m_pLibraries, (m_nLibraries + 1) * sizeof(CSG_Tool_Library *));
		m_pLibraries[m_nLibraries++]	= pLibrary;
	}

	if( !pLibrary )	// this should never happen, but who knows...
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s %s: %s", _TL("ERROR"), _TL("tool chain library"), FileName));

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
CSG_Tool_Library * CSG_Tool_Library_Manager::Get_Library(const SG_Char *Name, bool bLibrary) const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library	*pLibrary	= Get_Library(i);

		if( !SG_STR_CMP(Name, bLibrary ? pLibrary->Get_Library_Name() : pLibrary->Get_Name()) )
		{
			return( pLibrary );
		}
	}

	return( NULL );
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
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const CSG_String &Library, int ID)	const
{
	return( Get_Tool(Library, CSG_String::Format("%d", ID)) );
}

//---------------------------------------------------------
CSG_Tool * CSG_Tool_Library_Manager::Get_Tool(const CSG_String &Library, const CSG_String &Tool)	const
{
	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Tool_Library	*pLibrary	= Get_Library(i);

		if( pLibrary->Get_Library_Name().Cmp(Library) == 0 )
		{
			CSG_Tool	*pTool	= pLibrary->Get_Tool(Tool);

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
