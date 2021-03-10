
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
//                    WKSP_Tool.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/utils.h>
#include <wx/filename.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_parameters.h"

#include "wksp_data_manager.h"
#include "wksp_tool_manager.h"
#include "wksp_tool_library.h"
#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool	*g_pTool	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool::CWKSP_Tool(CSG_Tool *pTool, const wxString &Menu_Library)
{
	m_pTool		= pTool;
	m_Menu_ID	= -1;
	m_bCloseGUI	= false;
}

//---------------------------------------------------------
CWKSP_Tool::~CWKSP_Tool(void)
{
	if( g_pTool == this )
	{
		if( g_pTool->is_Executing() )
		{
			PROCESS_Set_Okay(false);
		}

		if( m_pTool->is_Interactive() )
		{
			((CSG_Tool_Interactive *)m_pTool)->Execute_Finish();
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(m_pTool);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Tool::Get_Name(void)
{
	return( m_pTool->is_Interactive()
		? wxString::Format("%s [%s]", m_pTool->Get_Name().c_str(), _TL("interactive"))
		: wxString::Format("%s"     , m_pTool->Get_Name().c_str())
	);
}

//---------------------------------------------------------
wxString CWKSP_Tool::Get_Description(void)
{
	if( !m_pTool->Get_File_Name().is_Empty() )
	{
		CSG_String	Lib_Name	= SG_File_Get_Name(m_pTool->Get_File_Name(), false);
		CSG_String	File_Path	= SG_File_Make_Path(SG_File_Get_Path(m_pTool->Get_File_Name()), Lib_Name, "");

		File_Path	= SG_File_Make_Path(File_Path, CSG_String::Format("%s_%02d", Lib_Name.c_str(), Get_Index()));

		CSG_String	Description;
		CSG_File	Stream;

		if( SG_File_Set_Extension(File_Path, "html") && Stream.Open(File_Path, SG_FILE_R) && Stream.Read(Description, Stream.Length()) )
		{
			return( Description.c_str() );
		}

		if( SG_File_Set_Extension(File_Path, "htm" ) && Stream.Open(File_Path, SG_FILE_R) && Stream.Read(Description, Stream.Length()) )
		{
			return( Description.c_str() );
		}
	}

	//-----------------------------------------------------
	wxString	Menu(m_pTool->Get_MenuPath(true).c_str()), Description;

	if( g_pTools->Get_Parameter("HELP_SOURCE")->asInt() == 1 )
	{
		Description	= Get_Online_Tool_Description(((CWKSP_Tool_Library *)Get_Manager())->Get_File_Name(), Get_Tool()->Get_ID().c_str());
	}

	return( m_pTool->Get_Summary(true, &Menu, &Description).c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Tool::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(Get_Name());

	pMenu->AppendCheckItem(Get_Menu_ID(), _TL("Execute"), _TL("Execute Tool"));

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_SAVE_SCRIPT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_SAVE_TO_CLIPBOARD);

	if( m_pTool->Get_Type() == TOOL_TYPE_Chain )
	{
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_CHAIN_RELOAD);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_CHAIN_EDIT);
	}

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_RELOAD);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);

	return( pMenu );
}

//---------------------------------------------------------
CSG_Parameters * CWKSP_Tool::Get_Parameters(void)
{
	return( m_pTool->Get_Parameters() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
		Execute(true);
		break;

	case ID_CMD_TOOL_SAVE_SCRIPT:
		Save_to_Script();
		break;

	case ID_CMD_TOOL_SAVE_TO_CLIPBOARD:
		Save_to_Clipboard();
		break;

	case ID_CMD_TOOL_CHAIN_RELOAD:
		if( m_pTool->Get_Type() == TOOL_TYPE_Chain
		&&  g_pTools->Open(m_pTool->Get_File_Name().c_str())
		&&  g_pActive->Get_Active() == this )
		{
			g_pActive->Set_Active(NULL);
			g_pActive->Set_Active(this);
		}
		break;

	case ID_CMD_TOOL_CHAIN_EDIT:
		if( m_pTool->Get_Type() == TOOL_TYPE_Chain )
		{
			Open_Application(m_pTool->Get_File_Name().c_str(), "txt");
		}
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool::Set_Menu_ID(int Menu_ID)
{
	m_Menu_ID	= Menu_ID;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool::is_Interactive(void)
{
	return( m_pTool->is_Interactive() );
}

//---------------------------------------------------------
bool CWKSP_Tool::is_Executing(void)
{
	return( m_pTool->is_Executing() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MSG_ADD(s)			MSG_General_Add  (s, true, true);\
							MSG_Execution_Add(s, true, true);

#define MSG_ADD2(b, s1, s2)	MSG_General_Add  (b ? s1 : s2, true, true, b ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE);\
							MSG_Execution_Add(b ? s1 : s2, true, true, b ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE);

//---------------------------------------------------------
bool CWKSP_Tool::Execute(bool bDialog)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	if( g_pTool )
 	{
		if( g_pTool == this )
		{
			if( g_pTool->is_Executing() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), m_pTool->Get_Name().c_str()) )
				{
					bResult	= true;

					PROCESS_Set_Okay(false);
				}
			}
			else if( m_pTool->is_Interactive() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), m_pTool->Get_Name().c_str()) )
				{
					bResult	= ((CSG_Tool_Interactive *)m_pTool)->Execute_Finish();

					g_pTool	= NULL;

					PROCESS_Set_Okay(true);

					MSG_ADD2(bResult,
						_TL("Interactive tool execution has been stopped"),
						_TL("Interactive tool execution failed")
					);
				}
			}
		}
		else
		{
			DLG_Message_Show(_TL("Can't execute a tool while another runs"), _TL("Tool Execution"));
		}
	}

	//-----------------------------------------------------
	else
	{
		g_pTool	= this;

		if( m_pTool->On_Before_Execution() && (!bDialog || DLG_Parameters(m_pTool->Get_Parameters(), "", m_pTool->Get_Summary(false).c_str())) )
		{
			g_pTools->Set_Recently_Used(this);

			MSG_General_Add_Line();
			MSG_Execution_Add_Line();
			MSG_ADD(wxString::Format("%s: %s", _TL("Executing tool"), m_pTool->Get_Name().c_str()));

			STATUSBAR_Set_Text(m_pTool->Get_Name().w_str());

			bResult		= m_pTool->Execute(true);

			m_pTool->On_After_Execution();

			g_pActive->Get_Parameters()->Update_Parameters(m_pTool->Get_Parameters(), false);

			if( m_pTool->is_Interactive() )
			{
				MSG_ADD2(bResult,
					_TL("Interactive tool execution has been started"),
					_TL("Interactive tool execution failed")
				);
			}
			else
			{
				MSG_ADD2(bResult,
					_TL("Tool execution succeeded"),
					_TL("Tool execution failed")
				);
			}

			if( g_pTools && g_pTools->Do_Beep() )	{	Do_Beep();	}
		}

		if( !m_pTool->is_Interactive() || !bResult )
		{
			g_pTool	= NULL;
		}

		if( m_bCloseGUI )
		{
			m_bCloseGUI	= false;

			MDI_Get_Frame()->Close();
		}
	}

	//-----------------------------------------------------
	return( bResult );
}

//---------------------------------------------------------
bool CWKSP_Tool::Execute(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode, int Keys)
{
	if( g_pTool == this && m_pTool->is_Interactive() )
	{
		return( ((CSG_Tool_Interactive *)m_pTool)->Execute_Position(ptWorld, Mode, Keys) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Tool::Finish(bool bDialog, bool bCloseGUI)
{
	if( Execute(bDialog) )
	{
		m_bCloseGUI	= bCloseGUI;

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
#include <wx/clipbrd.h>

//---------------------------------------------------------
void CWKSP_Tool::Save_to_Clipboard(void)
{
	wxArrayString	Choices;

	Choices.Add(_TL("Tool Chain"              ));
	Choices.Add(_TL("Tool Chain with Header"  ));
	Choices.Add(_TL("Command Line"            ));
	Choices.Add(_TL("Command Line with Header"));
	Choices.Add(_TL("Python"                  ));
	Choices.Add(_TL("Python with Header"      ));

	wxSingleChoiceDialog	dlg(MDI_Get_Top_Window(), _TL("Select Format"), _TL("Save to Clipboard"), Choices);

	if( dlg.ShowModal() == wxID_OK )
	{
		CSG_String	Script;

		switch( dlg.GetSelection() )
		{
		case 0:	Script = m_pTool->Get_Script(TOOL_SCRIPT_CHAIN    , false); break;	// Tool Chain
		case 1:	Script = m_pTool->Get_Script(TOOL_SCRIPT_CHAIN    ,  true); break;	// Tool Chain with Header
		#ifdef _SAGA_MSW
		case 2:	Script = m_pTool->Get_Script(TOOL_SCRIPT_CMD_BATCH, false); break;	// Command Line
		case 3:	Script = m_pTool->Get_Script(TOOL_SCRIPT_CMD_BATCH,  true); break;	// Command Line with Header
		#else
		case 2:	Script = m_pTool->Get_Script(TOOL_SCRIPT_CMD_SHELL, false); break;	// Command Line
		case 3:	Script = m_pTool->Get_Script(TOOL_SCRIPT_CMD_SHELL,  true); break;	// Command Line with Header
		#endif
		case 4:	Script = m_pTool->Get_Script(TOOL_SCRIPT_PYTHON   , false); break;	// Python
		case 5:	Script = m_pTool->Get_Script(TOOL_SCRIPT_PYTHON   ,  true); break;	// Python with Header
		}

		if( !Script.is_Empty() && wxTheClipboard->Open() )
		{
			wxTheClipboard->SetData(new wxTextDataObject(Script.c_str()));

			wxTheClipboard->Close();
		}
	}
}

//---------------------------------------------------------
void CWKSP_Tool::Save_to_Script(void)
{
	wxString	FileName;

	if( DLG_Save(FileName, _TL("Create Script Command File"), "DOS Batch Script (*.bat)|*.bat|Bash Script (*.sh)|*.sh|Python Script (*.py)|*.py|SAGA Tool Chain (*.xml)|*.xml") )
	{
		CSG_String	Script;

		if( SG_File_Cmp_Extension(&FileName, "xml") )
		{
			Script	= m_pTool->Get_Script(TOOL_SCRIPT_CHAIN    ,  true);
		}

		if( SG_File_Cmp_Extension(&FileName, "bat") )
		{
			Script	= m_pTool->Get_Script(TOOL_SCRIPT_CMD_BATCH,  true);
		}

		if( SG_File_Cmp_Extension(&FileName, "sh") )
		{
			Script	= m_pTool->Get_Script(TOOL_SCRIPT_CMD_SHELL, true);
		}

		if( SG_File_Cmp_Extension(&FileName, "py") )
		{
			Script	= m_pTool->Get_Script(TOOL_SCRIPT_PYTHON   , true);
		}

		//-------------------------------------------------
		CSG_File	File;

		if( !Script.is_Empty() && File.Open(&FileName, SG_FILE_W, false) )
		{
			File.Write(Script);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
