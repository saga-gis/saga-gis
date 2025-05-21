
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
CWKSP_Tool *g_pTool = NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Tool::CWKSP_Tool(CSG_Tool *pTool, const wxString &Menu_Library)
{
	m_pTool     = pTool;
	m_Menu_ID   = -1;
	m_bCloseGUI = false;
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
		CSG_String Lib_Name, File_Path, Description; CSG_File Stream;

		Lib_Name  = SG_File_Get_Name(m_pTool->Get_File_Name(), false);
		File_Path = SG_File_Make_Path(SG_File_Get_Path(m_pTool->Get_File_Name()), Lib_Name, "");
		File_Path = SG_File_Make_Path(File_Path, CSG_String::Format("%s_%02d", Lib_Name.c_str(), Get_Index()));

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
	wxString Description;

	if( g_pTools->Get_Parameter("HELP_SOURCE")->asInt() == 1 )
	{
		Description	= Get_Online_Tool_Description(((CWKSP_Tool_Library *)Get_Manager())->Get_File_Name(), Get_Tool()->Get_ID().c_str());
	}

	return( m_pTool->Get_Summary(true, m_pTool->Get_MenuPath(true), &Description).c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Tool::Get_Menu(void)
{
	wxMenu *pMenu = new wxMenu(Get_Name());

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
	m_Menu_ID = Menu_ID;
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
bool CWKSP_Tool::Set_Projection(const CSG_Projection &Projection)
{
	if( g_pTool == this && m_pTool->is_Interactive() ) // applies only to interactive tools
	{
		return( ((CSG_Tool_Interactive *)m_pTool)->Set_Projection(Projection) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Tool::Execute(bool bDialog)
{
	if( g_pTool )
 	{
		if( g_pTool != this )
		{
			DLG_Message_Show(_TL("Can't execute a tool while another runs"), _TL("Tool Execution"));
		}
		else // if( g_pTool == this )
		{
			if( g_pTool->is_Executing() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), m_pTool->Get_Name().c_str()) )
				{
					PROCESS_Set_Okay(false);

					return( true );
				}
			}
			else if( m_pTool->is_Interactive() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), m_pTool->Get_Name().c_str()) )
				{
					g_pTool = NULL;

					return( ((CSG_Tool_Interactive *)m_pTool)->Execute_Finish() );
				}
			}
		}

		return( false );
	}

	//-----------------------------------------------------
	bool bResult = false; g_pTool = this;

	if( m_pTool->On_Before_Execution() )
	{
		if( !bDialog || DLG_Parameters(m_pTool->Get_Parameters(), "", m_pTool->Get_Summary(false).c_str()) )
		{
			#ifndef __WXMAC__
			g_pTools->Add_Recent(this);
			#endif

			MSG_General_Add_Line(); MSG_Execution_Add_Line(); STATUSBAR_Set_Text(m_pTool->Get_Name().w_str());

			bResult	= m_pTool->Execute(true);

			SG_UI_ProgressAndMsg_Reset();

			if( g_pTools && g_pTools->Do_Beep() )
			{
				Do_Beep();
			}
		}

		m_pTool->On_After_Execution();

		g_pActive->Get_Parameters()->Update_Parameters(m_pTool->Get_Parameters(), false);

		#ifdef __WXMAC__
		if( g_pTools->Get_Parameter("LIST_RECENT") && g_pTools->Get_Parameter("LIST_RECENT")->asBool() )
		{
			g_pTools->Add_Recent(this);
		}
		#endif
	}

	if( !bResult || !m_pTool->is_Interactive() )
	{
		g_pTool = NULL;
	}

	if( m_bCloseGUI )
	{
		m_bCloseGUI = false;

		MDI_Get_Frame()->Close();
	}

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
		m_bCloseGUI = bCloseGUI;

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
void CWKSP_Tool::Save_to_Script(void)
{
	wxString FileName;

	if( DLG_Save(FileName, _TL("Create Script Command File"), "DOS Batch Script (*.bat)|*.bat|Bash Script (*.sh)|*.sh|Python Script (*.py)|*.py|SAGA Tool Chain (*.xml)|*.xml") )
	{
		CSG_String Script;

		if( SG_File_Cmp_Extension(&FileName, "xml") ) { Script = m_pTool->Get_Script(CSG_Tool::Script_Format::Toolchain,  true); }
		if( SG_File_Cmp_Extension(&FileName, "bat") ) { Script = m_pTool->Get_Script(CSG_Tool::Script_Format::CMD_Batch,  true); }
		if( SG_File_Cmp_Extension(&FileName, "sh" ) ) { Script = m_pTool->Get_Script(CSG_Tool::Script_Format::CMD_Shell , true); }
		if( SG_File_Cmp_Extension(&FileName, "py" ) ) { Script = m_pTool->Get_Script(CSG_Tool::Script_Format::Python    , true); }

		CSG_File File;

		if( !Script.is_Empty() && File.Open(FileName, SG_FILE_W, false) )
		{
			File.Write(Script);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Tool::On_Clipboard_Format_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( pParameter && pParameter->Get_Parameters() && (Flags & PARAMETER_CHECK_ENABLE) )
	{
		CSG_Parameters &Parameters = *pParameter->Get_Parameters();

		Parameters.Set_Enabled("CMD"    , Parameters["TYPE"].asInt() == 0);
		Parameters.Set_Enabled("ARGS"   , Parameters["TYPE"].asInt() != 2 || (Parameters["TYPE"].asInt() == 2 && Parameters["PYTHON"].asInt() != 1));
		Parameters.Set_Enabled("WRAP"   , Parameters["TYPE"].asInt() == 0 || (Parameters["TYPE"].asInt() == 2 && Parameters["PYTHON"].asInt() == 0));
		Parameters.Set_Enabled("PYTHON" , Parameters["TYPE"].asInt() == 2);
		Parameters.Set_Enabled("PY_FUNC",                                    (Parameters["TYPE"].asInt() == 2 && Parameters["PYTHON"].asInt() != 2));

		CSG_String Arguments;

		if( Parameters["TYPE"].asInt() == 2 && Parameters["PYTHON"].asInt() == 0 )
		{
			Arguments.Printf("%s|", _TL("pure function call"));
		}

		Arguments += CSG_String::Format("%s|%s", _TL("non-default settings"), _TL("all settings"));

		Parameters["ARGS"].asChoice()->Set_Items(Arguments);

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
#include <wx/clipbrd.h>

//---------------------------------------------------------
void CWKSP_Tool::Save_to_Clipboard(void)
{
	static CSG_Parameters Options;

	if( !Options.Get_Count() )
	{
		Options.Create(_TL("Copy Script Tool Call to Clipboard"), _TL(""), SG_T("SCRIPT_FORMAT"));

		Options.Add_Choice("",
			"TYPE"   , _TL("Script Type"),
			_TL(""),
			CSG_String::Format("%s|%s|%s",
				_TL("Command Line"),
				_TL("Tool Chain"),
				_TL("Python")
			)
		);

		Options.Add_Choice("TYPE",
			"CMD"    , _TL("Command Line"),
			_TL(""),
			CSG_String::Format("%s|%s",
				_TL("Batch"),
				_TL("Shell")
			#ifdef _SAGA_MSW
			), 0
			#else
			), 1
			#endif
		);

		Options.Add_Choice("TYPE",
			"PYTHON" , _TL("Python Format"),
			_TL(""),
			CSG_String::Format("%s|%s|%s",
				_TL("Wrapper Function Call"),
				_TL("Wrapper Function"),
				_TL("Tool Settings and Call")
			)
		);

		Options.Add_Choice("PYTHON",
			"PY_FUNC", _TL("Function Format"),
			_TL(""),
			CSG_String::Format("%s|%s",
				_TL("name"),
				_TL("library + identifier")
			), 0
		);

		Options.Add_Bool("",
			"HEADER" , _TL("Header"),
			_TL(""),
			true
		);

		Options.Add_Choice("",
			"ARGS"   , _TL("Arguments"),
			_TL(""),
			CSG_String::Format("%s|%s",
				_TL("non-default settings"),
				_TL("all settings")
			), 0
		);

		Options.Add_Bool("ARGS",
			"WRAP"   , _TL("Line Breaks"),
			_TL(""),
			true
		);

		Options.Set_Callback_On_Parameter_Changed(On_Clipboard_Format_Changed);

		On_Clipboard_Format_Changed(Options(0), PARAMETER_CHECK_ENABLE);
	}

	//-----------------------------------------------------
	if( DLG_Parameters(&Options, _TL("Copy Script Tool Call to Clipboard")) )
	{
		CSG_String Script;

		switch( Options["TYPE"].asInt() )
		{
		case  0: // Command Line
			Script = m_pTool->Get_Script(Options["CMD"].asInt() == 0 ?
				CSG_Tool::Script_Format::CMD_Batch : CSG_Tool::Script_Format::CMD_Shell,
				Options["HEADER"].asBool(), Options["ARGS"].asInt(), Options["WRAP"].asBool()
			);
			break;

		case  1: // Tool Chain
			Script = m_pTool->Get_Script(CSG_Tool::Script_Format::Toolchain,
				Options["HEADER"].asBool(), Options["ARGS"].asInt()
			);
			break;

		case  2: // Python
			switch( Options["PYTHON"].asInt() )
			{
			case  0: // Wrapper Function Call
				Script = m_pTool->Get_Script(Options["PY_FUNC"].asInt() == 0 ?
					CSG_Tool::Script_Format::Python_Wrapper_Call_Name : CSG_Tool::Script_Format::Python_Wrapper_Call_ID,
					Options["HEADER"].asBool(), Options["ARGS"].asInt() - 1, Options["WRAP"].asBool()
				);
				break;

			case  1: // Wrapper Function
				Script = m_pTool->Get_Script(Options["PY_FUNC"].asInt() == 0 ?
					CSG_Tool::Script_Format::Python_Wrapper_Func_Name : CSG_Tool::Script_Format::Python_Wrapper_Func_ID,
					Options["HEADER"].asBool(), 1, false
				);
				break;

			case  2: // Tool Settings and Call
				Script = m_pTool->Get_Script(
					CSG_Tool::Script_Format::Python,
					Options["HEADER"].asBool(), Options["ARGS"].asInt()
				);
				break;
			}
		}

		if( !Script.is_Empty() && wxTheClipboard->Open() )
		{
			wxTheClipboard->SetData(new wxTextDataObject(Script.c_str()));

			wxTheClipboard->Close();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
