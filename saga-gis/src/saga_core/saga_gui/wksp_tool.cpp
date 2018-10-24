/**********************************************************
 * Version $Id$
 *********************************************************/

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
	m_pTool	= pTool;
	m_Menu_ID	= -1;
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	//-----------------------------------------------------
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
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	pMenu->AppendCheckItem(Get_Menu_ID(), _TL("Execute"), _TL("Execute Tool"));

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_SAVE_SCRIPT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_SAVE_TO_CLIPBOARD);

	if( m_pTool->Get_Type() == TOOL_TYPE_Chain )
	{
		pMenu->AppendSeparator();

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_CHAIN_RELOAD);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_TOOL_CHAIN_EDIT);
	}

	return( pMenu );
}

//---------------------------------------------------------
CSG_Parameters * CWKSP_Tool::Get_Parameters(void)
{
	return( m_pTool->Get_Parameters() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
		_Save_to_Script();
		break;

	case ID_CMD_TOOL_SAVE_TO_CLIPBOARD:
		_Save_to_Clipboard();
		break;

	case ID_CMD_TOOL_CHAIN_RELOAD:
		if( m_pTool->Get_Type() == TOOL_TYPE_Chain
		&&  g_pTools->Open(m_pTool->Get_File_Name().c_str())
		&&  g_pACTIVE->Get_Active() == this )
		{
			g_pACTIVE->Set_Active(NULL);
			g_pACTIVE->Set_Active(this);
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Tool::Set_Menu_ID(int aMenu_ID)
{
	m_Menu_ID	= aMenu_ID;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
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
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), _TL("Tool Execution")) )
				{
					PROCESS_Set_Okay(false);
				}
			}
			else if( m_pTool->is_Interactive() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), _TL("Tool Execution")) )
				{
					bResult		= ((CSG_Tool_Interactive *)m_pTool)->Execute_Finish();
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

			bResult		= m_pTool->Execute();

			m_pTool->On_After_Execution();

			g_pACTIVE->Get_Parameters()->Update_Parameters(m_pTool->Get_Parameters(), false);

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/clipbrd.h>

//---------------------------------------------------------
void CWKSP_Tool::_Save_to_Clipboard(void)
{
	//-----------------------------------------------------
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
		CSG_String	s;

		switch( dlg.GetSelection() )
		{
		case 0:	s	= _Get_XML   (false);	break;	// Tool Chain
		case 1:	s	= _Get_XML   ( true);	break;	// Tool Chain with Header
		case 2:	s	= _Get_CMD   (false);	break;	// Command Line
		case 3:	s	= _Get_CMD   ( true);	break;	// Command Line with Header
		case 4:	s	= _Get_Python(false);	break;	// Python
		case 5:	s	= _Get_Python( true);	break;	// Python with Header
		}

		if( !s.is_Empty() && wxTheClipboard->Open() )
		{
			wxTheClipboard->SetData(new wxTextDataObject(s.c_str()));
			wxTheClipboard->Close();
		}
	}
}

//---------------------------------------------------------
void CWKSP_Tool::_Save_to_Script(void)
{
	wxString	FileName;

	if( DLG_Save(FileName, _TL("Create Script Command File"), "DOS Batch Script (*.bat)|*.bat|Bash Script (*.sh)|*.sh|Python Script (*.py)|*.py|SAGA Tool Chain (*.xml)|*.xml") )
	{
		CSG_String	Script;

		if( SG_File_Cmp_Extension(&FileName, "xml") )
		{
			Script	= _Get_XML(true);
		}

		if( SG_File_Cmp_Extension(&FileName, "bat") )
		{
			Script	= _Get_CMD(true, 0);
		}

		if( SG_File_Cmp_Extension(&FileName, "sh") )
		{
			Script	= _Get_CMD(true, 1);
		}

		if( SG_File_Cmp_Extension(&FileName, "py") )
		{
			Script	= _Get_Python(true);
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CWKSP_Tool::_Get_XML(bool bHeader)
{
	CSG_MetaData	Tool;	Tool.Set_Name("tool");

	Tool.Add_Property("library", m_pTool->Get_Library());
	Tool.Add_Property("tool"   , m_pTool->Get_ID     ());
	Tool.Add_Property("name"   , m_pTool->Get_Name   ());

	_Get_XML(Tool, m_pTool->Get_Parameters());

	for(int i=0; i<m_pTool->Get_Parameters_Count(); i++)
	{
		_Get_XML(Tool, m_pTool->Get_Parameters(i), m_pTool->Get_Parameters(i)->Get_Identifier() + '.');
	}

	if( !bHeader )
	{
		return( Tool.asText(1) );
	}
	else
	{
		CSG_MetaData	Tools;	Tools.Set_Name("toolchain");

		Tools.Add_Property("saga-version", SAGA_VERSION);

		Tools.Add_Child("group");
		Tools.Add_Child("identifier");
		Tools.Add_Child("name");
		Tools.Add_Child("author");
		Tools.Add_Child("description");
		Tools.Add_Child("menu");
		Tools.Add_Child("parameters");
		Tools.Add_Child("tools")->Add_Child(Tool);

		return( Tools.asText(1) );
	}
}

//---------------------------------------------------------
void CWKSP_Tool::_Get_XML(CSG_MetaData &Tool, CSG_Parameters *pParameters, const CSG_String &Prefix)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !p->is_Enabled() || p->is_Information() )
		{
			continue;
		}

		CSG_MetaData	*pChild	= NULL;

		switch( p->Get_Type() )
		{
		case PARAMETER_TYPE_Parameters  :
			_Get_XML(Tool, p->asParameters(), Prefix + p->Get_Identifier() + '.');
			break;

		case PARAMETER_TYPE_Bool        :
			pChild	= Tool.Add_Child("option", p->asBool() ? "true" : "false");
			break;

		case PARAMETER_TYPE_Int         :
		case PARAMETER_TYPE_Double      :
		case PARAMETER_TYPE_Degree      :
		case PARAMETER_TYPE_Date        :
		case PARAMETER_TYPE_Range       :
		case PARAMETER_TYPE_String      :
		case PARAMETER_TYPE_Text        :
		case PARAMETER_TYPE_FilePath    :
		case PARAMETER_TYPE_Choices     :
		case PARAMETER_TYPE_Table_Field :
		case PARAMETER_TYPE_Table_Fields:
			pChild	= Tool.Add_Child("option", p->asString());
			break;

		case PARAMETER_TYPE_Choice      :
			pChild	= Tool.Add_Child("option", p->asInt());
			break;

		case PARAMETER_TYPE_FixedTable  :
			pChild	= Tool.Add_Child("option");
			p->Serialize(*pChild, true);
			break;

		case PARAMETER_TYPE_Grid_System :
			if( p->Get_Children_Count() == 0 )
			{
				pChild	= Tool.Add_Child("option", p->asString());
			}
			break;

		default:
			if( p->is_Input() )
			{
				pChild	= Tool.Add_Child("input");
				pChild->Set_Content(p->is_Optional() ? "input_optional" : "input");
			}
			else if( p->is_Output() )
			{
				pChild	= Tool.Add_Child("output");
				pChild->Set_Content("output");
			}
			break;
		}

		if( pChild )
		{
		//	pChild->Add_Property("parms", pParameters->Get_Identifier());
		//	pChild->Add_Property("id"   , p          ->Get_Identifier());
			pChild->Add_Property("id", Prefix + p->Get_Identifier());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CWKSP_Tool::_Get_CMD(bool bHeader, int Type)
{
	CSG_String	s;

	if( Type != 0 && Type != 1 )	// default type ??
	{
#ifdef _SAGA_MSW
		Type	= 0;
#else
		Type	= 1;
#endif
	}

	//-----------------------------------------------------
	if( Type == 0 )	// DOS/Windows Batch Script
	{
		if( bHeader )
		{
			s	+= "@ECHO OFF\n\n";
			s	+= "REM SET SAGA_TLB=C:\\SAGA\\Tools\n";
			s	+= "REM SET PATH=%PATH%;C:\\SAGA\n\n";
			s	+= "REM Tool: ";
			s	+= m_pTool->Get_Name() + "\n\n";
		}

		s	+= "saga_cmd ";
		s	+= m_pTool->Get_Library() + " " + m_pTool->Get_ID();

		_Get_CMD(s, m_pTool->Get_Parameters());

		for(int i=0; i<m_pTool->Get_Parameters_Count(); i++)
		{
			_Get_CMD(s, m_pTool->Get_Parameters(i));
		}

		if( bHeader )
		{
			s	+= "\n\nPAUSE\n";
		}
	}

	//-----------------------------------------------------
	if( Type == 1 )	// Bash Shell Script
	{
		if( bHeader )
		{
			s	+= "#!/bin/bash\n\n";
			s	+= "# export SAGA_TLB=/usr/lib/saga\n\n";
			s	+= "# tool: ";
			s	+= m_pTool->Get_Name() + "\n\n";
		}

		s	+= "saga_cmd ";
		s	+= m_pTool->Get_Library() + " " + m_pTool->Get_ID();

		_Get_CMD(s, m_pTool->Get_Parameters());

		for(int i=0; i<m_pTool->Get_Parameters_Count(); i++)
		{
			_Get_CMD(s, m_pTool->Get_Parameters(i));
		}
	}

	return( s );
}

//---------------------------------------------------------
#include "wksp_data_manager.h"

#define GET_ID1(p)		(p->Get_Owner()->Get_Identifier().Length() > 0 \
						? CSG_String::Format("%s_%s", p->Get_Owner()->Get_Identifier().c_str(), p->Get_Identifier()) \
						: CSG_String::Format(p->Get_Identifier())).c_str()

#define GET_ID2(p, s)	CSG_String::Format("%s_%s", GET_ID1(p), s).c_str()

//---------------------------------------------------------
void CWKSP_Tool::_Get_CMD(CSG_String &Command, CSG_Parameters *pParameters)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !p->is_Enabled() || p->is_Information() || !p->do_UseInCMD() )
		{
			continue;
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool:
			Command	+= CSG_String::Format(" -%s=%d", GET_ID1(p), p->asBool() ? 1 : 0);
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_Table_Field:
			Command	+= CSG_String::Format(" -%s=%d", GET_ID1(p), p->asInt());
			break;

		case PARAMETER_TYPE_Choices     :
		case PARAMETER_TYPE_Table_Fields:
			if( p->asString() && *p->asString() )
				Command	+= CSG_String::Format(" -%s=%s", GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Command	+= CSG_String::Format(" -%s=%f", GET_ID1(p), p->asDouble());
			break;

		case PARAMETER_TYPE_Range:
			Command	+= CSG_String::Format(" -%s=%f", GET_ID2(p, SG_T("MIN")), p->asRange()->Get_Min());
			Command	+= CSG_String::Format(" -%s=%f", GET_ID2(p, SG_T("MAX")), p->asRange()->Get_Max());
			break;

		case PARAMETER_TYPE_Date:
		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_FilePath:
			Command	+= CSG_String::Format(" -%s=%s", GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable:
			Command	+= CSG_String::Format(" -%s=%s", GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Grid_System:
			if( p->Get_Children_Count() == 0 )
			{
				Command	+= CSG_String::Format(" -%s=%d", GET_ID2(p, SG_T("NX")), p->asGrid_System()->Get_NX());
				Command	+= CSG_String::Format(" -%s=%d", GET_ID2(p, SG_T("NY")), p->asGrid_System()->Get_NY());
				Command	+= CSG_String::Format(" -%s=%f", GET_ID2(p, SG_T( "X")), p->asGrid_System()->Get_XMin());
				Command	+= CSG_String::Format(" -%s=%f", GET_ID2(p, SG_T( "Y")), p->asGrid_System()->Get_YMin());
				Command	+= CSG_String::Format(" -%s=%f", GET_ID2(p, SG_T( "D")), p->asGrid_System()->Get_Cellsize());
			}
			break;

		case PARAMETER_TYPE_DataObject_Output:
		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Grids:
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
			Command	+= CSG_String::Format(" -%s=%s", GET_ID1(p), g_pData->Get(p->asDataObject()) && p->asDataObject()->Get_File_Name() ? p->asDataObject()->Get_File_Name() : SG_T("NULL"));
			break;

		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Grids_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
		case PARAMETER_TYPE_PointCloud_List:
			if( p->is_Input() )
			{
				Command	+= CSG_String::Format(" -%s=", GET_ID1(p));

				if( p->asList()->Get_Item_Count() == 0 )
				{
					Command	+= "NULL";
				}
				else
				{
					Command	+= SG_File_Exists(p->asList()->Get_Item(0)->Get_File_Name())
							 ? p->asList()->Get_Item(0)->Get_File_Name() : _TL("memory");

					for(int iObject=1; iObject<p->asList()->Get_Item_Count(); iObject++)
					{
						Command	+= ";";
						Command	+= SG_File_Exists(p->asList()->Get_Item(iObject)->Get_File_Name())
								 ? p->asList()->Get_Item(iObject)->Get_File_Name() : _TL("memory");
					}
				}
			}
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CWKSP_Tool::_Get_Python(bool bHeader)
{
	CSG_String	s;

	//-----------------------------------------------------
	if( bHeader )
	{
		s	+= "#! /usr/bin/env python\n";
		s	+= "\n";
		s	+= "import saga_api, sys, os\n";
		s	+= "\n";
		s	+= "##########################################\n";
	}

	//-----------------------------------------------------
	s	+= "def Run_SAGA_Tool(File):\n";
	s	+= "    #_____________________________________\n";
	s	+= "    # Provide your input dataset(s), here -as example- load a dataset from file.\n";
	s	+= "    # Using SAGA's central data manager instance for such jobs is an easy way to go...\n";
	s	+= "    Data = saga_api.SG_Get_Data_Manager().Add(File)\n";
	s	+= "    if Data == None or Data.is_Valid() == False:\n";
	s	+= "        print 'Failed to load dataset [' + File + ']'\n";
	s	+= "        return False\n";
	s	+= "\n";
	s	+= "    #_____________________________________\n";
	s	+= "    # request tool '" + m_pTool->Get_Name() + "'\n";
	s	+= "    Tool = saga_api.SG_Get_Tool_Library_Manager().Get_Tool('" + m_pTool->Get_Library() + "', '" + m_pTool->Get_ID() + "')\n";
	s	+= "\n";
	s	+= "    Parm = Tool.Get_Parameters()\n";

	if( m_pTool->Get_Type() == TOOL_TYPE_Grid )
	{
		s	+= "    Parm.Reset_Grid_System()\n";
	}

	//-------------------------------------------------
	_Get_Python(s, m_pTool->Get_Parameters());

	for(int iParameters=0; iParameters<m_pTool->Get_Parameters_Count(); iParameters++)
	{
		s	+= CSG_String::Format("\n    Parm = Tool.Get_Parameters(%d) # additional parameter list\n", iParameters);

		_Get_Python(s, m_pTool->Get_Parameters(iParameters));
	}

	//-------------------------------------------------
	s	+= "\n";
	s	+= "    if Tool.Execute() == False:\n";
	s	+= "        print 'Tool execution failed!'\n";
	s	+= "        return False\n";
	s	+= "\n";
	s	+= "    #_____________________________________\n";
	s	+= "    # Save results to file:\n";
	s	+= "    Path = os.path.split(File)[0] + os.sep\n";
	s	+= "    Parm = Tool.Get_Parameters()\n";

	for(int iParameter=0; iParameter<m_pTool->Get_Parameters()->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= m_pTool->Get_Parameters()->Get_Parameter(iParameter);

		if( p->is_Output() )
		{
			CSG_String	id(p->Get_Identifier()), ext;

			switch( p->Get_DataObject_Type() )
			{
			case SG_DATAOBJECT_TYPE_Grid      : ext = " + '.sg-grd-z'"; break;
			case SG_DATAOBJECT_TYPE_Grids     : ext = " + '.sg-gds-z'"; break;
			case SG_DATAOBJECT_TYPE_Table     : ext = " + '.txt'"     ; break;
			case SG_DATAOBJECT_TYPE_Shapes    : ext = " + '.geojson'" ; break;
			case SG_DATAOBJECT_TYPE_PointCloud: ext = " + '.sg-pts-z'"; break;
			case SG_DATAOBJECT_TYPE_TIN       : ext = " + '.geojson'" ; break;
			default                           : ext = ""              ; break;
			}

			if( p->is_DataObject() )
			{
				s	+= "    Parm('" +  id + "').asDataObject().Save(Path + Parm('" +  id + "').asDataObject().Get_Name()" + ext + ")\n";
			}
			else if( p->is_DataObject_List() )
			{
				s	+= "    List = Parm('" +  id + "').asList()\n";
				s	+= "    Name = Path + Parm('" +  id + "').Get_Name()\n";
				s	+= "    for i in range(0, List.Get_Data_Count()):\n";
				s	+= "        List.Get_Data(i).Save(Name + str(i)" + ext + ")\n";
			}
		}
	}

	s	+= "\n";
	s	+= "    #_____________________________________\n";
	s	+= "    saga_api.SG_Get_Data_Manager().Delete_All() # job is done, free memory resources\n";
	s	+= "\n";
	s	+= "    print 'Tool successfully executed!'\n";
	s	+= "    return True\n";
	s	+= "\n";

	//-----------------------------------------------------
	if( bHeader )
	{
		s	+= "\n";
		s	+= "##########################################\n";
		s	+= "if __name__ == '__main__':\n";
		s	+= "    #____________________________________\n";
		s	+= "    # The following will load all tools from the SAGA installation directory (adjust the path to your system)\n";
		s	+= "    # and if available the directory defined by the environment variable SAGA_TLB\n";
		s	+= "    saga_api.SG_UI_Msg_Lock(True) # avoid too much noise\n";
		s	+= "\n";
		s	+= "    if os.name == 'nt':    # Windows\n";
		s	+= "        saga_path = os.environ['SAGA_32'] # e.g. import SAGA installation directory from an environment variable\n";
		s	+= "        os.environ['PATH'] = os.environ['PATH'] + ';' + saga_path + '/dll' # library dependencies\n";
		s	+= "        saga_api.SG_Get_Tool_Library_Manager().Add_Directory(saga_path + '/tools', False)\n";
		s	+= "    else:                  # Linux\n";
		s	+= "        saga_api.SG_Get_Tool_Library_Manager().Add_Directory('/usr/local/lib/saga' , False)\n";
		s	+= "\n";
		s	+= "    saga_api.SG_Get_Tool_Library_Manager().Add_Directory(os.environ['SAGA_TLB'], False)\n";
		s	+= "    saga_api.SG_UI_Msg_Lock(False)\n";
		s	+= "\n";
		s	+= "    print 'Python - Version ' + sys.version\n";
		s	+= "    print saga_api.SAGA_API_Get_Version()\n";
		s	+= "    print 'number of loaded libraries: ' + str(saga_api.SG_Get_Tool_Library_Manager().Get_Count())\n";
		s	+= "    print\n";
		s	+= "\n";
		s	+= "    #____________________________________\n";
		s	+= "    print 'Usage: %s <in: filename>'\n";
		s	+= "    print 'This is a simple template for using a SAGA tool through Python.'\n";
		s	+= "    print 'Please edit the script to make it work properly before using it!'\n";
		s	+= "    sys.exit()\n";
		s	+= "    # For a single file based input it might look like following:\n";
		s	+= "    File = sys.argv[1]\n";
		s	+= "\n";
		s	+= "    #____________________________________\n";
		s	+= "    Run_SAGA_Tool(File)\n";
	}

	return( s );
}

//---------------------------------------------------------
void CWKSP_Tool::_Get_Python(CSG_String &Command, CSG_Parameters *pParameters, const CSG_String &Prefix)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !p->is_Enabled() || p->is_Information() || !p->do_UseInCMD() )
		{
			continue;
		}

		CSG_String	ID(p->Get_Identifier());

		if( !Prefix.is_Empty() )
		{
			ID.Prepend(Prefix + ".");
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool           :
			Command	+= CSG_String::Format("    Parm('%s').Set_Value('%s')\n", ID.c_str(), p->asBool() ? SG_T("true") : SG_T("false"));
			break;

		case PARAMETER_TYPE_Int            :
			Command	+= CSG_String::Format("    Parm('%s').Set_Value(%d)\n", ID.c_str(), p->asInt());
			break;

		case PARAMETER_TYPE_Choice         :
		case PARAMETER_TYPE_Choices        :
		case PARAMETER_TYPE_Table_Field    :
		case PARAMETER_TYPE_Table_Fields   :
			Command	+= CSG_String::Format("    Parm('%s').Set_Value('%s')\n", ID.c_str(), p->asString());
			break;

		case PARAMETER_TYPE_Double         :
		case PARAMETER_TYPE_Degree         :
			Command	+= CSG_String::Format("    Parm('%s').Set_Value(%f)\n", ID.c_str(), p->asDouble());
			break;

		case PARAMETER_TYPE_Range          :
			Command	+= CSG_String::Format("    Parm('%s').asRange().Set_Min(%f)\n", ID.c_str(), p->asRange()->Get_Min());
			Command	+= CSG_String::Format("    Parm('%s').asRange().Set_Max(%f)\n", ID.c_str(), p->asRange()->Get_Max());
			break;

		case PARAMETER_TYPE_Date           :
		case PARAMETER_TYPE_String         :
		case PARAMETER_TYPE_Text           :
		case PARAMETER_TYPE_FilePath       :
			Command	+= CSG_String::Format("    Parm('%s').Set_Value('%s')\n", ID.c_str(), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable     :
			Command	+= CSG_String::Format("    Parm('%s').Set_Value(saga_api.SG_Create_Table('table.txt'))\n", ID.c_str());
			break;

		case PARAMETER_TYPE_Grid_System    :
			if( p->Get_Children_Count() == 0 )
			{
				Command	+= CSG_String::Format("    Parm('%s').Set_Value(saga_api.CSG_Grid_System(%f, %f, %f, %d, %d))\n", ID.c_str(),
					p->asGrid_System()->Get_Cellsize(),
					p->asGrid_System()->Get_XMin(), p->asGrid_System()->Get_YMin(),
					p->asGrid_System()->Get_NX  (), p->asGrid_System()->Get_NY  ()
				);
			}
			break;

		case PARAMETER_TYPE_Grid           :
		case PARAMETER_TYPE_Grids          :
		case PARAMETER_TYPE_Table          :
		case PARAMETER_TYPE_Shapes         :
		case PARAMETER_TYPE_TIN            :

			if( p->is_Input() )
			{
				Command	+= CSG_String::Format("    Parm('%s').Set_Value('%s input%s')\n", ID.c_str(),
					SG_Get_DataObject_Name(p->Get_DataObject_Type()).c_str(), p->is_Optional() ? SG_T(", optional") : SG_T("")
				);
			}
			else if( p->is_Output() && p->is_Optional() )
			{
				Command	+= CSG_String::Format("    Parm('%s').Set_Value(saga_api.SG_Get_Create_Pointer()) # optional output, remove this line, if you don't want to create it\n", ID.c_str());
			}
			break;

		case PARAMETER_TYPE_Grid_List      :
		case PARAMETER_TYPE_Grids_List     :
		case PARAMETER_TYPE_Table_List     :
		case PARAMETER_TYPE_Shapes_List    :
		case PARAMETER_TYPE_TIN_List       :
		case PARAMETER_TYPE_PointCloud_List:
			if( p->is_Input() )
			{
				Command	+= CSG_String::Format("    Parm('%s').asList().Add_Item('%s input list%s')\n", ID.c_str(),
					SG_Get_DataObject_Name(p->Get_DataObject_Type()).c_str(), p->is_Optional() ? SG_T(", optional") : SG_T("")
				);
			}
			break;

		case PARAMETER_TYPE_Parameters     :
			_Get_Python(Command, p->asParameters(), ID);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
