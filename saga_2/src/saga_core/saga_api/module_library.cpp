
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
	&&	!(MLB_Get_Interface	= (TSG_PFNC_MLB_Get_Interface)	m_pLibrary->GetSymbol(SYMBOL_MLB_Get_Interface))
	&&	!(MLB_Initialize	= (TSG_PFNC_MLB_Initialize)		m_pLibrary->GetSymbol(SYMBOL_MLB_Initialize)   )
	&&	MLB_Initialize(m_File_Name) )
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
CSG_String CSG_Module_Library::Get_Name(void)
{
	return( Get_Info(MLB_INFO_Name) );
}

//---------------------------------------------------------
CSG_String CSG_Module_Library::Get_Description(void)
{
	CSG_String	s;

	s.Printf(
		"%s: <b>%s</b><br>%s: <i>%s</i><br>%s: <i>%s</i><br>%s: <i>%s</i><hr>%s",
		LNG("[CAP] Module Library")	, Get_Info(MLB_INFO_Name),
		LNG("[CAP] Author")			, Get_Info(MLB_INFO_Author),
		LNG("[CAP] Version")		, Get_Info(MLB_INFO_Version),
		LNG("[CAP] File")			, Get_File_Name().c_str(),
		Get_Info(MLB_INFO_Description)
	);

	s.Append(wxString::Format("<hr><b>%s:<ul>", LNG("[CAP] Modules")));

	for(int i=0; i<Get_Count(); i++)
	{
		s.Append(CSG_String::Format("<li>%s</li>", Get_Module(i)->Get_Name()));
	}

	s.Append("</ul>");

	s.Replace("\n", "<br>");

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Module_Library::is_Valid(void)
{
	return( Get_Count() > 0 );
}

//---------------------------------------------------------
const char * CSG_Module_Library::Get_Info(int Type)
{
	if( m_pInterface != NULL )
	{
		return( m_pInterface->Get_Info(Type) );
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
