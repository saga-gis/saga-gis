
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

#include "module_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if defined(_SAGA_MSW)

	#define ENV_LIB_PATH	"PATH"
	#define ENV_LIB_SEPA	';'

#elif defined(_SAGA_LINUX)

	#define ENV_LIB_PATH	"LD_LIBRARY_PATH"
	#define ENV_LIB_SEPA	':'

#else

	#define ENV_LIB_PATH	"PATH"
	#define ENV_LIB_SEPA	';'

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
CSG_Module_Library::CSG_Module_Library(const char *File_Name)
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
bool CSG_Module_Library::Create(const char *File_Name)
{
	Destroy();

	TSG_PFNC_MLB_Initialize		MLB_Initialize;
	TSG_PFNC_MLB_Get_Interface	MLB_Get_Interface;

	wxString	sPath;
	wxFileName	fName(File_Name);

	fName.MakeAbsolute();
	m_File_Name		= fName.GetFullPath();

	//-----------------------------------------------------
	if( wxGetEnv(ENV_LIB_PATH, &sPath) && sPath.Length() > 0 )
	{
		wxSetEnv(ENV_LIB_PATH, CSG_String::Format("%s%c%s", sPath.c_str(), ENV_LIB_SEPA, SG_File_Get_Path(m_File_Name).c_str()));
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
const char * CSG_Module_Library::Get_Info(int Type)
{
	if( m_pInterface != NULL )
	{
		return( m_pInterface->Get_Info(Type) );
	}

	return( "" );
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
			"%s: <b>%s</b><br>%s: <i>%s</i><br>%s: <i>%s</i><br>%s: <i>%s</i><hr>%s",
			LNG("[CAP] Module Library")	, Get_Info(MLB_INFO_Name),
			LNG("[CAP] Author")			, Get_Info(MLB_INFO_Author),
			LNG("[CAP] Version")		, Get_Info(MLB_INFO_Version),
			LNG("[CAP] File")			, Get_File_Name().c_str(),
			Get_Info(MLB_INFO_Description)
		);

		s.Append(CSG_String::Format("<hr><b>%s:<ul>", LNG("[CAP] Modules")));

		for(int i=0; i<Get_Count(); i++)
		{
			s.Append(CSG_String::Format("<li>%s</li>", Get_Module(i)->Get_Name()));
		}

		s.Append("</ul>");

		s.Replace("\n", "<br>");
	}
	else
	{
		s.Printf(
			"%s: %s\n%s: %s\n%s: %s\n%s: %s\n\n%s",
			LNG("[CAP] Module Library")	, Get_Info(MLB_INFO_Name),
			LNG("[CAP] Author")			, Get_Info(MLB_INFO_Author),
			LNG("[CAP] Version")		, Get_Info(MLB_INFO_Version),
			LNG("[CAP] File")			, Get_File_Name().c_str(),
			Get_Info(MLB_INFO_Description)
		);

		s.Append(CSG_String::Format("\n\n%s:\n", LNG("[CAP] Modules")));

		for(int i=0; i<Get_Count(); i++)
		{
			s.Append(CSG_String::Format("- %s\n", Get_Module(i)->Get_Name()));
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
CSG_Module_Grid * CSG_Module_Library::Get_Module_Grid(int i)
{
	CSG_Module	*pModule	= Get_Module(i);

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

//---------------------------------------------------------
CSG_Module_Grid_Interactive * CSG_Module_Library::Get_Module_Grid_I(int i)
{
	CSG_Module	*pModule	= Get_Module(i);

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

	if( Get_Module(i) != NULL )
	{
		bool		bAbsolute	= false;
		const char	*sModule	= Get_Module(i)->Get_MenuPath();

		if( sModule != NULL && strlen(sModule) > 1 && sModule[1] == ':' )
		{
			bAbsolute	= sModule[0] == 'A' || sModule[0] == 'a';
			sModule		+= 2;
		}

		if( bAbsolute )	// menu path is relative to top menu...
		{
			if( sModule != NULL && strlen(sModule) > 0 )
			{
				sMenu.Printf("%s", sModule);
			}
		}
		else			// menu path is relative to library menu...
		{
			const char	*sLibrary	= Get_Info(MLB_INFO_Menu_Path);

			if( sModule != NULL && strlen(sModule) > 0 )
			{
				if( sLibrary != NULL && strlen(sLibrary) > 0 )
				{
					sMenu.Printf("%s|%s", sLibrary, sModule);
				}
				else
				{
					sMenu.Printf("%s", sModule);
				}
			}
			else if( sLibrary != NULL && strlen(sLibrary) > 0 )
			{
				sMenu.Printf("%s", sLibrary);
			}
		}

		if( sMenu.Length() > 0 )
		{
			sMenu.Append("|");
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
