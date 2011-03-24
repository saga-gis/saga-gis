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
//                  module_library.cpp                   //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/utils.h>

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if defined(_SAGA_MSW)

	#define ENV_LIB_PATH	SG_T("PATH")
	#define ENV_LIB_SEPA	SG_T(';')

#elif defined(_SAGA_LINUX)

	#define ENV_LIB_PATH	SG_T("LD_LIBRARY_PATH")
	#define ENV_LIB_SEPA	SG_T(':')

#else

	#define ENV_LIB_PATH	SG_T("PATH")
	#define ENV_LIB_SEPA	SG_T(';')

#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module_Library::CSG_Module_Library(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Module_Library::CSG_Module_Library(const CSG_String &File_Name)
{
	_On_Construction();

	Create(File_Name);
}

//---------------------------------------------------------
CSG_Module_Library::~CSG_Module_Library(void)
{
	Destroy();

	delete(m_pLibrary);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Module_Library::_On_Construction(void)
{
	m_pLibrary		= new wxDynamicLibrary;

	m_pInterface	= NULL;
}

//---------------------------------------------------------
bool CSG_Module_Library::Create(const CSG_String &File_Name)
{
	Destroy();

	TSG_PFNC_MLB_Initialize		MLB_Initialize;
	TSG_PFNC_MLB_Get_Interface	MLB_Get_Interface;

	wxString	sPath;
	wxFileName	fName(File_Name.c_str());

	fName.MakeAbsolute();
	m_File_Name		= fName.GetFullPath();

	//-----------------------------------------------------
	if( wxGetEnv(ENV_LIB_PATH, &sPath) && sPath.Length() > 0 )
	{
		wxSetEnv(ENV_LIB_PATH, CSG_String::Format(SG_T("%s%c%s"), sPath.c_str(), ENV_LIB_SEPA, SG_File_Get_Path(m_File_Name).c_str()));
	}
	else
	{
		wxSetEnv(ENV_LIB_PATH, SG_File_Get_Path(m_File_Name).c_str());
	}

	//-----------------------------------------------------
	if(	m_pLibrary->Load(m_File_Name.c_str())
	&&	(MLB_Get_Interface	= (TSG_PFNC_MLB_Get_Interface)	m_pLibrary->GetSymbol(SYMBOL_MLB_Get_Interface)) != NULL
	&&	(MLB_Initialize		= (TSG_PFNC_MLB_Initialize)		m_pLibrary->GetSymbol(SYMBOL_MLB_Initialize)   ) != NULL
	&&	 MLB_Initialize(m_File_Name) )
	{
		m_pInterface	= MLB_Get_Interface();
	}

	//-----------------------------------------------------
	if( sPath.Length() > 0 )
	{
		wxSetEnv(ENV_LIB_PATH, sPath);
	}
	else
	{
		wxUnsetEnv(ENV_LIB_PATH);
	}

	//-----------------------------------------------------
	if( Get_Count() > 0 )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( Get_Module(i) )
			{
				Get_Module(i)->Set_Managed(false);
			}
		}

		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
bool CSG_Module_Library::Destroy(void)
{
	if( m_pLibrary->IsLoaded() )
	{
		TSG_PFNC_MLB_Finalize	MLB_Finalize	= (TSG_PFNC_MLB_Finalize)m_pLibrary->GetSymbol(SYMBOL_MLB_Finalize);

		if(	MLB_Finalize )
		{
			MLB_Finalize();
		}

		m_pLibrary->Unload();
	}

	m_pInterface	= NULL;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Module_Library::Get_Info(int Type)
{
	if( m_pInterface != NULL )
	{
		return( m_pInterface->Get_Info(Type) );
	}

	return( SG_T("") );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Name(void)
{
	return( Get_Info(MLB_INFO_Name) );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Description(void)
{
	return( Get_Info(MLB_INFO_Description) );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Author(void)
{
	return( Get_Info(MLB_INFO_Author) );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Version(void)
{
	return( Get_Info(MLB_INFO_Version) );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Menu(void)
{
	return( Get_Info(MLB_INFO_Menu_Path) );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Summary(bool bHTML)
{
	CSG_String	s;

	if( bHTML )
	{
		s.Printf(
			SG_T("%s: <b>%s</b><br>%s: <i>%s</i><br>%s: <i>%s</i><br>%s: <i>%s</i><hr>%s"),
			LNG("[CAP] Module Library")	, Get_Info(MLB_INFO_Name),
			LNG("[CAP] Author")			, Get_Info(MLB_INFO_Author),
			LNG("[CAP] Version")		, Get_Info(MLB_INFO_Version),
			LNG("[CAP] File")			, Get_File_Name().c_str(),
			Get_Info(MLB_INFO_Description)
		);

		s.Append(CSG_String::Format(SG_T("<hr><b>%s:<ul>"), LNG("[CAP] Modules")));

		for(int i=0; i<Get_Count(); i++)
		{
			if( Get_Module(i) )
			{
				s.Append(CSG_String::Format(SG_T("<li>%s</li>"), Get_Module(i)->Get_Name()));
			}
		}

		s.Append(SG_T("</ul>"));

		s.Replace(SG_T("\n"), SG_T("<br>"));
	}
	else
	{
		s.Printf(
			SG_T("%s: %s\n%s: %s\n%s: %s\n%s: %s\n\n%s"),
			LNG("[CAP] Module Library")	, Get_Info(MLB_INFO_Name),
			LNG("[CAP] Author")			, Get_Info(MLB_INFO_Author),
			LNG("[CAP] Version")		, Get_Info(MLB_INFO_Version),
			LNG("[CAP] File")			, Get_File_Name().c_str(),
			Get_Info(MLB_INFO_Description)
		);

		s.Append(CSG_String::Format(SG_T("\n\n%s:\n"), LNG("[CAP] Modules")));

		for(int i=0; i<Get_Count(); i++)
		{
			if( Get_Module(i) )
			{
				s.Append(CSG_String::Format(SG_T("- %s\n"), Get_Module(i)->Get_Name()));
			}
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Module * CSG_Module_Library::Get_Module(const SG_Char *Name)
{
	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Module(i) && !SG_STR_CMP(Name, Get_Module(i)->Get_Name()) )
		{
			return( Get_Module(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CSG_Module_Grid * CSG_Module_Library::Get_Module_Grid(int i)
{
	CSG_Module	*pModule	= Get_Module(i);

	return( pModule && pModule->Get_Type() == MODULE_TYPE_Grid
		? (CSG_Module_Grid *)pModule : NULL
	);
}

CSG_Module_Grid * CSG_Module_Library::Get_Module_Grid(const SG_Char *Name)
{
	CSG_Module	*pModule	= Get_Module(Name);

	return( pModule && pModule->Get_Type() == MODULE_TYPE_Grid
		? (CSG_Module_Grid *)pModule : NULL
	);
}

//---------------------------------------------------------
CSG_Module_Interactive * CSG_Module_Library::Get_Module_I(int i)
{
	CSG_Module	*pModule	= Get_Module(i);

	return( pModule && pModule->Get_Type() == MODULE_TYPE_Interactive
		? (CSG_Module_Interactive *)pModule : NULL
	);
}

CSG_Module_Interactive * CSG_Module_Library::Get_Module_I(const SG_Char *Name)
{
	CSG_Module	*pModule	= Get_Module(Name);

	return( pModule && pModule->Get_Type() == MODULE_TYPE_Interactive
		? (CSG_Module_Interactive *)pModule : NULL
	);
}

//---------------------------------------------------------
CSG_Module_Grid_Interactive * CSG_Module_Library::Get_Module_Grid_I(int i)
{
	CSG_Module	*pModule	= Get_Module(i);

	return( pModule && pModule->Get_Type() == MODULE_TYPE_Grid_Interactive
		? (CSG_Module_Grid_Interactive *)pModule : NULL
	);
}

CSG_Module_Grid_Interactive * CSG_Module_Library::Get_Module_Grid_I(const SG_Char *Name)
{
	CSG_Module	*pModule	= Get_Module(Name);

	return( pModule && pModule->Get_Type() == MODULE_TYPE_Grid_Interactive
		? (CSG_Module_Grid_Interactive *)pModule : NULL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Menu(int i)
{
	CSG_String	sMenu;

	if( Get_Module(i) )
	{
		bool		bAbsolute	= false;
		const SG_Char	*sModule	= Get_Module(i)->Get_MenuPath();

		if( sModule && *sModule && *(sModule + 1) == ':' )
		{
			bAbsolute	= sModule[0] == SG_T('A') || sModule[0] == SG_T('a');
			sModule		+= 2;
		}

		if( bAbsolute )	// menu path is relative to top menu...
		{
			if( sModule && *sModule )
			{
				sMenu.Printf(SG_T("%s"), sModule);
			}
		}
		else			// menu path is relative to library menu...
		{
			const SG_Char	*sLibrary	= Get_Info(MLB_INFO_Menu_Path);

			if( sModule && *sModule )
			{
				if( sLibrary && *sLibrary )
				{
					sMenu.Printf(SG_T("%s|%s"), sLibrary, sModule);
				}
				else
				{
					sMenu.Printf(SG_T("%s"), sModule);
				}
			}
			else if( sLibrary && *sLibrary )
			{
				sMenu.Printf(SG_T("%s"), sLibrary);
			}
		}

		if( sMenu.Length() > 0 )
		{
			sMenu.Append(SG_T("|"));
		}

		sMenu.Append(Get_Info(MLB_INFO_Name));
	}

	return( sMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
