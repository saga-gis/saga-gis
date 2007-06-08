
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   WKSP_Module.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
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
#include <wx/utils.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_parameters.h"

#include "wksp_data_manager.h"

#include "wksp_module_manager.h"
#include "wksp_module_menu.h"
#include "wksp_module.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module	*g_pModule	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Module::CWKSP_Module(CSG_Module *pModule, const wxChar *Menu_Path_default)
{
	m_pModule		= pModule;

	m_Menu_ID		= -1;

	//-----------------------------------------------------
	bool		bLibrary, bModule;
	const wxChar	*sModule	= m_pModule->Get_MenuPath();

	m_Menu_Path.Empty();

	if( sModule && *sModule && *(sModule + 1) == wxT(':') )
	{
		if( *sModule == wxT('A') || *sModule == wxT('a') )
		{
			sModule	+= 2;

			if( *sModule )
			{
				m_Menu_Path.Printf(wxT("%s"), sModule);
			}
		}
		else
		{
			sModule	+= 2;
		}
	}

	if( m_Menu_Path.Length() == 0 )	// Menu path is relative to default menu path...
	{
		bLibrary	= Menu_Path_default	&& *Menu_Path_default;
		bModule		= sModule			&& *sModule;

		if( bLibrary && bModule )
		{
			m_Menu_Path.Printf(wxT("%s|%s"), Menu_Path_default, sModule);
		}
		else if( bLibrary )
		{
			m_Menu_Path.Printf(wxT("%s"), Menu_Path_default);
		}
		else if( bModule )
		{
			m_Menu_Path.Printf(wxT("%s"), sModule);
		}
	}
}

//---------------------------------------------------------
CWKSP_Module::~CWKSP_Module(void)
{
	if( g_pModule == this )
	{
		if( g_pModule->is_Executing() )
		{
			PROCESS_Set_Okay(false);
		}

		if( m_pModule->is_Interactive() )
		{
			((CSG_Module_Interactive *)m_pModule)->Execute_Finish();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Module::Get_Name(void)
{
	return( m_pModule->is_Interactive()
		? wxString::Format(wxT("%s [%s]"), m_pModule->Get_Name(), LNG("interactive"))
		: wxString::Format(wxT("%s")     , m_pModule->Get_Name())
	);
}

wxString & CWKSP_Module::Get_File_Name(void)
{
	return( m_File_Name );
}

void CWKSP_Module::Set_File_Name(wxString File_Name)
{
	m_File_Name = File_Name;
}
//---------------------------------------------------------
wxString CWKSP_Module::Get_Description(void)
{
	bool		bFirst, bOptionals	= false;
	int			i;
	CSG_Parameter	*pParameter;
	wxString	s, sTmp;

	//-----------------------------------------------------
	s.Append(wxString::Format(wxT("%s: <b>%s</b>")	, LNG("Module"), m_pModule->Get_Name()));

	if( m_pModule->is_Interactive() )
	{
		s.Append(wxString::Format(wxT("<br>- %s -")	, LNG("interactive execution")));
	}

	if( m_pModule->Get_Author() && *(m_pModule->Get_Author()) )
	{
		s.Append(wxString::Format(wxT("<br>%s")		, m_pModule->Get_Author()));
	}

	if( m_Menu_Path.Length() > 0 )
	{
		sTmp	= m_Menu_Path;
		sTmp.Replace(wxT("|"), wxT(" <b>></b> "));
		s.Append(wxString::Format(wxT("<br>%s: <i>%s</i>"), LNG("Menu"), sTmp.c_str()));
	}

	if( m_pModule->Get_Description() && *(m_pModule->Get_Description()) )
	{
		s.Append(wxString::Format(wxT("<hr><b>%s</b><br>%s"), LNG("Description"), m_pModule->Get_Description()));
	}


	//-----------------------------------------------------
	s.Append(wxString::Format(wxT("<hr><b>%s</b><br>"), LNG("Parameters")));
	s.Append(wxString::Format(wxT("<table border=\"1\" width=\"100%%\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>"),
		LNG("Name"), LNG("Type"), LNG("Description")
	));

	for(i=0, bFirst=true; i<m_pModule->Get_Parameters()->Get_Count(); i++)
	{
		pParameter	= m_pModule->Get_Parameters()->Get_Parameter(i);

		if( pParameter->is_Input() )
		{
			if( bFirst )
			{
				bFirst	= false;
				s.Append(wxString::Format(wxT("<tr><th colspan=\"3\">%s</th></tr>"), LNG("Input")));
			}

			s.Append(wxString::Format(wxT("<tr><td>%s%s</td><td>%s</td><td>%s</td></tr>"),
				pParameter->Get_Name(),
				pParameter->is_Optional() ? wxT(" (*)") : wxT(" "),
				pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES).c_str(),
				pParameter->Get_Description()
			));
		}
	}

	for(i=0, bFirst=true; i<m_pModule->Get_Parameters()->Get_Count(); i++)
	{
		pParameter	= m_pModule->Get_Parameters()->Get_Parameter(i);

		if( pParameter->is_Output() )
		{
			if( bFirst )
			{
				bFirst	= false;
				s.Append(wxString::Format(wxT("<tr><th colspan=\"3\">%s</th></tr>"), LNG("Output")));
			}

			s.Append(wxString::Format(wxT("<tr><td>%s%s</td><td>%s</td><td>%s</td></tr>"),
				pParameter->Get_Name(),
				pParameter->is_Optional() ? wxT(" (*)") : wxT(""),
				pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES).c_str(),
				pParameter->Get_Description()
			));
		}
	}

	for(i=0, bFirst=true; i<m_pModule->Get_Parameters()->Get_Count(); i++)
	{
		pParameter	= m_pModule->Get_Parameters()->Get_Parameter(i);

		if( pParameter->is_Option() )
		{
			if( bFirst )
			{
				bFirst	= false;
				s.Append(wxString::Format(wxT("<tr><th colspan=\"3\">%s</th></tr>"), LNG("Options")));
			}

			s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td><td>%s</td></tr>"),
				pParameter->Get_Name(),
				pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES).c_str(),
				pParameter->Get_Description()
			));
		}
		else if( pParameter->is_Optional() )
		{
			bOptionals	= true;
		}
	}

	s.Append(wxT("</table>"));

	if( bOptionals )
	{
		s.Append(wxString::Format(wxT("(*) <i>%s</i>"), LNG("optional")));
	}

	s.Replace(wxT("\n"), wxT("<br>"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Module::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	pMenu->AppendCheckItem(Get_Menu_ID(), LNG("[CMD] Execute"), LNG("[HLP] Execute Module"));

	return( pMenu );
}

//---------------------------------------------------------
CSG_Parameters * CWKSP_Module::Get_Parameters(void)
{
	return( m_pModule->Get_Parameters() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		Execute(true);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module::Set_Menu_ID(int aMenu_ID)
{
	m_Menu_ID	= aMenu_ID;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Module::is_Interactive(void)
{
	return( m_pModule->is_Interactive() );
}

//---------------------------------------------------------
bool CWKSP_Module::is_Executing(void)
{
	return( m_pModule->is_Executing() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MSG_ADD(s)	MSG_General_Add(s, true, true);	MSG_Execution_Add(s, true, true);

//---------------------------------------------------------
bool CWKSP_Module::Execute(bool bDialog)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	if( g_pModule )
 	{
		if( g_pModule == this )
		{
			if( g_pModule->is_Executing() )
			{
				if( DLG_Message_Confirm(LNG("[MSG] Shall execution be stopped?"), LNG("[CAP] Module Execution")) )
				{
					PROCESS_Set_Okay(false);
				}
			}
			else if( m_pModule->is_Interactive() )
			{
				bResult		= ((CSG_Module_Interactive *)m_pModule)->Execute_Finish();
				g_pModule	= NULL;

				PROCESS_Set_Okay(true);

				MSG_ADD(bResult
					? LNG("[MSG] Interactive module execution has been stopped")
					: LNG("[MSG] Interactive module execution failed")
				);
			}
		}
		else
		{
			DLG_Message_Show(LNG("[ERR] Can't execute a module while another runs"), LNG("[CAP] Module Execution"));
		}
	}

	//-----------------------------------------------------
	else
	{
		g_pModule	= this;

		if( !bDialog || DLG_Parameters(m_pModule->Get_Parameters()) )
		{
			g_pModules->Get_Modules_Menu()->Set_Recent(this);

			g_pData->Check_Parameters(m_pModule->Get_Parameters());

			MSG_General_Add_Line();
			MSG_Execution_Add_Line();
			MSG_ADD(wxString::Format(wxT("%s: %s"), LNG("[MSG] Executing module"), m_pModule->Get_Name()));

			STATUSBAR_Set_Text(m_pModule->Get_Name());

			bResult		= m_pModule->Execute();

			g_pACTIVE->Get_Parameters()->Update_Parameters(m_pModule->Get_Parameters(), false);

			if( m_pModule->is_Interactive() )
			{
				MSG_ADD(bResult
					? LNG("[MSG] Interactive module execution has been started")
					: LNG("[MSG] Interactive module execution failed")
				);
			}
			else
			{
				MSG_ADD(bResult
					? LNG("[MSG] Module execution succeeded")
					: LNG("[MSG] Module execution failed")
				);
			}

			if( g_pModules && g_pModules->Do_Beep() )	{	Do_Beep();	}
		}

		if( !m_pModule->is_Interactive() || !bResult )
		{
			g_pModule	= NULL;
		}
	}

	//-----------------------------------------------------
	return( bResult );
}

//---------------------------------------------------------
bool CWKSP_Module::Execute(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode, int Keys)
{
	bool	bResult	= false;

	if( g_pModule == this && m_pModule->is_Interactive() && !m_pModule->is_Executing() )
	{
		bResult	= ((CSG_Module_Interactive *)m_pModule)->Execute_Position(ptWorld, Mode, Keys);

		PROCESS_Set_Okay();
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
