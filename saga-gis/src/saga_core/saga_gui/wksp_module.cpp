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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include <wx/filename.h>

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_parameters.h"

#include "wksp_data_manager.h"

#include "wksp_module_manager.h"
#include "wksp_module_library.h"
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
CWKSP_Module::CWKSP_Module(CSG_Module *pModule, const wxString &Menu_Library)
{
	m_pModule	= pModule;
	m_Menu_ID	= -1;
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
		? wxString::Format(wxT("%s [%s]"), m_pModule->Get_Name().c_str(), _TL("interactive"))
		: wxString::Format(wxT("%s")     , m_pModule->Get_Name().c_str())
	);
}

//---------------------------------------------------------
wxString CWKSP_Module::Get_File_Name(void)
{
	return( m_pModule->Get_File_Name().c_str() );
}

//---------------------------------------------------------
wxString CWKSP_Module::Get_Description(void)
{
	//-----------------------------------------------------
	if( !Get_File_Name().IsEmpty() )
	{
		CSG_String	Description;
		CSG_File	Stream;

		wxFileName	FileName(Get_File_Name());
		FileName.AppendDir	(FileName.GetName());
		FileName.SetName	(wxString::Format("%s_%02d", FileName.GetName().c_str(), Get_Index()));

		FileName.SetExt		("html");

		if( Stream.Open(FileName.GetFullPath().wc_str(), SG_FILE_R) && Stream.Read(Description, Stream.Length()) )
		{
			return( Description.c_str() );
		}

		FileName.SetExt		("htm");

		if( Stream.Open(FileName.GetFullPath().wc_str(), SG_FILE_R) && Stream.Read(Description, Stream.Length()) )
		{
			return( Description.c_str() );
		}
	}

	//-----------------------------------------------------
	wxString	Menu(m_pModule->Get_MenuPath(true).c_str()), Description;

	if( g_pModules->Get_Parameter("HELP_SOURCE")->asInt() == 1 )
	{
		Description	= Get_Online_Module_Description(((CWKSP_Module_Library *)Get_Manager())->Get_File_Name(), Get_Module()->Get_ID().c_str());
	}

	return( m_pModule->Get_Summary(true, &Menu, &Description).c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Module::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	pMenu->AppendCheckItem(Get_Menu_ID(), _TL("Execute"), _TL("Execute Tool"));

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_SAVE_SCRIPT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_SAVE_TO_CLIPBOARD);

	if( m_pModule->Get_Type() == MODULE_TYPE_Chain )
	{
		pMenu->AppendSeparator();

		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_CHAIN_RELOAD);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_CHAIN_EDIT);
	}

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

	case ID_CMD_MODULES_SAVE_SCRIPT:
		_Save_to_Script();
		break;

	case ID_CMD_MODULES_SAVE_TO_CLIPBOARD:
		_Save_to_Clipboard();
		break;

	case ID_CMD_MODULES_CHAIN_RELOAD:
		if( m_pModule->Get_Type() == MODULE_TYPE_Chain
		&&  g_pModules->Open(m_pModule->Get_File_Name().c_str())
		&&  g_pACTIVE->Get_Active() == this )
		{
			g_pACTIVE->Set_Active(NULL);
			g_pACTIVE->Set_Active(this);
		}
		break;

	case ID_CMD_MODULES_CHAIN_EDIT:
		if( m_pModule->Get_Type() == MODULE_TYPE_Chain )
		{
			Open_Application(m_pModule->Get_File_Name().c_str(), "txt");
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
#define MSG_ADD(s)			MSG_General_Add  (s, true, true);\
							MSG_Execution_Add(s, true, true);

#define MSG_ADD2(b, s1, s2)	MSG_General_Add  (b ? s1 : s2, true, true, b ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE);\
							MSG_Execution_Add(b ? s1 : s2, true, true, b ? SG_UI_MSG_STYLE_SUCCESS : SG_UI_MSG_STYLE_FAILURE);

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
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), _TL("Tool Execution")) )
				{
					PROCESS_Set_Okay(false);
				}
			}
			else if( m_pModule->is_Interactive() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), _TL("Tool Execution")) )
				{
					bResult		= ((CSG_Module_Interactive *)m_pModule)->Execute_Finish();
					g_pModule	= NULL;

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
		g_pModule	= this;

		if( m_pModule->On_Before_Execution() && (!bDialog || DLG_Parameters(m_pModule->Get_Parameters())) )
		{
			g_pModules->Set_Recently_Used(this);

			MSG_General_Add_Line();
			MSG_Execution_Add_Line();
			MSG_ADD(wxString::Format(wxT("%s: %s"), _TL("Executing tool"), m_pModule->Get_Name().c_str()));

			STATUSBAR_Set_Text(m_pModule->Get_Name().w_str());

			bResult		= m_pModule->Execute();

			g_pACTIVE->Get_Parameters()->Update_Parameters(m_pModule->Get_Parameters(), false);

			if( m_pModule->is_Interactive() )
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
	if( g_pModule == this && m_pModule->is_Interactive() )
	{
		return( ((CSG_Module_Interactive *)m_pModule)->Execute_Position(ptWorld, Mode, Keys) );
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
void CWKSP_Module::_Save_to_Clipboard(void)
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
void CWKSP_Module::_Save_to_Script(void)
{
	wxString	FileName;

	if( DLG_Save(FileName, _TL("Create Script Command File"), SG_T("DOS Batch Script (*.bat)|*.bat|Bash Script (*.sh)|*.sh|Python Script (*.py)|*.py|SAGA Tool Chain (*.xml)|*.xml")) )
	{
		CSG_String	Script;

		if( SG_File_Cmp_Extension(FileName, SG_T("xml")) )
		{
			Script	= _Get_XML(true);
		}

		if( SG_File_Cmp_Extension(FileName, SG_T("bat")) )
		{
			Script	= _Get_CMD(true, 0);
		}

		if( SG_File_Cmp_Extension(FileName, SG_T("sh")) )
		{
			Script	= _Get_CMD(true, 1);
		}

		if( SG_File_Cmp_Extension(FileName, SG_T("py")) )
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
CSG_String CWKSP_Module::_Get_XML(bool bHeader)
{
	CSG_MetaData	Tool;	Tool.Set_Name("tool");

	Tool.Add_Property("library", m_pModule->Get_Library());
	Tool.Add_Property("module" , m_pModule->Get_ID     ());
	Tool.Add_Property("name"   , m_pModule->Get_Name   ());

	_Get_XML(Tool, m_pModule->Get_Parameters());

	for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
	{
		_Get_XML(Tool, m_pModule->Get_Parameters());
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
CSG_String CWKSP_Module::_Get_CMD(bool bHeader, int Type)
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
			s	+= "REM SET SAGA_MLB=C:\\SAGA\\Modules\n";
			s	+= "REM SET PATH=%PATH%;C:\\SAGA\n\n";
			s	+= "REM Tool: ";
			s	+= m_pModule->Get_Name() + "\n\n";
		}

		s	+= "saga_cmd ";
		s	+= m_pModule->Get_Library() + " " + m_pModule->Get_ID();

		_Get_CMD(s, m_pModule->Get_Parameters());

		for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
		{
			_Get_CMD(s, m_pModule->Get_Parameters(i));
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
			s	+= "# export SAGA_MLB=/usr/lib/saga\n\n";
			s	+= "# tool: ";
			s	+= m_pModule->Get_Name() + "\n\n";
		}

		s	+= "saga_cmd ";
		s	+= m_pModule->Get_Library() + " " + m_pModule->Get_ID();

		_Get_CMD(s, m_pModule->Get_Parameters());

		for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
		{
			_Get_CMD(s, m_pModule->Get_Parameters(i));
		}
	}

	return( s );
}

//---------------------------------------------------------
CSG_String CWKSP_Module::_Get_Python(bool bHeader)
{
	CSG_String	s;

	//-----------------------------------------------------
	if( bHeader )
	{
		#ifndef _SAGA_MSW
		s	+= "#! /usr/bin/env python\n";
		#endif
		s	+= "# Python script template for SAGA tool execution (automatically created, experimental)\n\n";
		s	+= "import saga_api, sys, os\n";
		s	+= "\n";
		s	+= "##########################################\n";
	}

	//-----------------------------------------------------
	s	+= "def Call_SAGA_Module(fDEM):            # pass your input file(s) here\n";
	s	+= "\n";
	s	+= "    # ------------------------------------\n";
	s	+= "    # initialize input dataset(s)\n";
	s	+= "    dem    = saga_api.SG_Get_Data_Manager().Add_Grid(unicode(fDEM))\n";
	s	+= "    if dem == None or dem.is_Valid() == 0:\n";
	s	+= "        print 'ERROR: loading grid [' + fDEM + ']'\n";
	s	+= "        return 0\n";
	s	+= "\n";
	s	+= "    # ------------------------------------\n";
	s	+= "    # initialize output dataset(s)\n";
	s	+= "    outgrid = saga_api.SG_Get_Data_Manager().Add_Grid(dem.Get_System())\n";
	s	+= "\n";
	s	+= "    # ------------------------------------\n";
	s	+= "    # call module: ";
	s	+= m_pModule->Get_Name() + "\n";
	s	+= "    Module = saga_api.SG_Get_Module_Library_Manager().Get_Module('";
	s	+= m_pModule->Get_Library() + "','" + m_pModule->Get_ID() + "')\n";

	if( m_pModule->Get_Type() == MODULE_TYPE_Grid )
	{
		s	+= "    Module.Get_Parameters().Get_Grid_System().Assign(dem.Get_System())\n";
	}

	s	+= "\n";
	s	+= "    Parms = Module.Get_Parameters() # default parameter list\n";

	//-------------------------------------------------
	_Get_Python(s, m_pModule->Get_Parameters());

	for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
	{
		s	+= "\n";
		s	+= CSG_String::Format("    Parms = Module.Get_Parameters(%d) # additional parameter list\n", i);

		_Get_Python(s, m_pModule->Get_Parameters(i));
	}

	//-------------------------------------------------
	s	+= "\n";
	s	+= "    if Module.Execute() == 0:\n";
	s	+= "        print 'Module execution failed!'\n";
	s	+= "        return 0\n";
	s	+= "\n";
	s	+= "    print\n";
	s	+= "    print 'The module has been executed.'\n";
	s	+= "    print 'Now you would like to save your output datasets, please edit the script to do so.'\n";
	s	+= "    return 0                           # remove this line once you have edited the script\n";
	s	+= "\n";
	s	+= "    # ------------------------------------\n";
	s	+= "    # save results\n";
	s	+= "    path   = os.path.split(fDEM)[0]\n";
	s	+= "    if path == '':\n";
	s	+= "        path = './'\n";
	s	+= "    outgrid.Save(saga_api.CSG_String(path + '/outgrid'))\n";
	s	+= "\n";
	s	+= "    print\n";
	s	+= "    print 'Module successfully executed!'\n";
	s	+= "    return 1\n";
	s	+= "\n";

	//-----------------------------------------------------
	if( bHeader )
	{
		s	+= "##########################################\n";
		s	+= "if __name__ == '__main__':\n";
		s	+= "    print 'Python - Version ' + sys.version\n";
		s	+= "    print saga_api.SAGA_API_Get_Version()\n";
		s	+= "    print\n";
		s	+= "    print 'Usage: %s <in: filename>'\n";
		s	+= "    print\n";
		s	+= "    print 'This is a simple template, please edit the script and add the necessary input and output file(s)!'\n";
		s	+= "    print 'We will exit the script for now.'\n";
		s	+= "    sys.exit()                         # remove this line once you have edited the script\n";
		s	+= "    # This might look like this:\n";
		s	+= "    # fDEM    = sys.argv[1]\n";
		s	+= "    # if os.path.split(fDEM)[0] == '':\n";
		s	+= "    #    fDEM    = './' + fDEM\n";
		s	+= "    fDEM = './../test_data/test.sgrd'  # remove this line once you have edited the script\n";
		s	+= "\n\n";
		s	+= "    saga_api.SG_UI_Msg_Lock(1)\n";
		s	+= "    if os.name == 'nt':    # Windows\n";
		s	+= "        os.environ['PATH'] = os.environ['PATH'] + ';' + os.environ['SAGA'] + '/bin/saga_vc_Win32/dll'\n";
		s	+= "        saga_api.SG_Get_Module_Library_Manager().Add_Directory(os.environ['SAGA'] + '/bin/saga_vc_Win32/modules', 0)\n";
		s	+= "    else:                  # Linux\n";
		s	+= "        saga_api.SG_Get_Module_Library_Manager().Add_Directory(os.environ['SAGA_MLB'], 0)\n";
		s	+= "    saga_api.SG_UI_Msg_Lock(0)\n";
		s	+= "\n";
		s	+= "    Call_SAGA_Module(fDEM)             # pass your input file(s) here\n";
	//	s	+= "    else:\n";
	//	s	+= "        in__grid    = saga_api.SG_Create_Grid(saga_api.CSG_String(sys.argv[1]))\n";
	//	s	+= "        out_grid    = saga_api.SG_Create_Grid(grid_in.Get_System())\n";
	//	s	+= "        in__shapes  = saga_api.SG_Create_Shapes(saga_api.CSG_String(sys.argv[3]))\n";
	//	s	+= "        out_shapes  = saga_api.SG_Create_Shapes()\n";
	//	s	+= "\n";
	//	s	+= "        if Call_SAGA_Module(in__grid, out_grid, in__shapes, out_shapes) != 0:\n";
	//	s	+= "            grid_out  .Save(saga_api.CSG_String(sys.argv[2]))\n";
	//	s	+= "            shapes_out.Save(saga_api.CSG_String(sys.argv[4]))\n";
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Module::_Get_XML(CSG_MetaData &Tool, CSG_Parameters *pParameters)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !p->is_Enabled() )
		{
			continue;
		}

		CSG_MetaData	*pChild	= NULL;

		switch( p->Get_Type() )
		{
		case PARAMETER_TYPE_Parameters  :
			_Get_XML(Tool, p->asParameters());
			break;

		case PARAMETER_TYPE_Bool        :
			pChild	= Tool.Add_Child("option", p->asBool() ? "true" : "false");
			break;

		case PARAMETER_TYPE_Int         :
		case PARAMETER_TYPE_Double      :
		case PARAMETER_TYPE_Degree      :
		case PARAMETER_TYPE_Range       :
		case PARAMETER_TYPE_String      :
		case PARAMETER_TYPE_Text        :
		case PARAMETER_TYPE_FilePath    :
		case PARAMETER_TYPE_Table_Field :
		case PARAMETER_TYPE_Table_Fields:
			pChild	= Tool.Add_Child("option", p->asString());
			break;

		case PARAMETER_TYPE_Choice      :
			pChild	= Tool.Add_Child("option", p->asInt());
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
			pChild->Add_Property("parms", pParameters->Get_Identifier());
			pChild->Add_Property("id"   , p          ->Get_Identifier());
		}
	}
}

//---------------------------------------------------------
#include "wksp_data_manager.h"

#define GET_ID1(p)		(p->Get_Owner()->Get_Identifier().Length() > 0 \
						? CSG_String::Format(SG_T("%s_%s"), p->Get_Owner()->Get_Identifier().c_str(), p->Get_Identifier()) \
						: CSG_String::Format(p->Get_Identifier())).c_str()

#define GET_ID2(p, s)	CSG_String::Format(SG_T("%s_%s"), GET_ID1(p), s).c_str()

//---------------------------------------------------------
void CWKSP_Module::_Get_CMD(CSG_String &Command, CSG_Parameters *pParameters)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !p->is_Enabled() )
		{
			continue;
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool:
			if( p->asBool() )
				Command	+= CSG_String::Format(SG_T(" -%s"), GET_ID1(p));
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_Table_Field:
			Command	+= CSG_String::Format(SG_T(" -%s=%d"), GET_ID1(p), p->asInt());
			break;

		case PARAMETER_TYPE_Table_Fields:
			if( p->asString() && *p->asString() )
				Command	+= CSG_String::Format(SG_T(" -%s=%s"), GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Command	+= CSG_String::Format(SG_T(" -%s=%f"), GET_ID1(p), p->asDouble());
			break;

		case PARAMETER_TYPE_Range:
			Command	+= CSG_String::Format(SG_T(" -%s=%f"), GET_ID2(p, SG_T("MIN")), p->asRange()->Get_LoVal());
			Command	+= CSG_String::Format(SG_T(" -%s=%f"), GET_ID2(p, SG_T("MAX")), p->asRange()->Get_HiVal());
			break;

		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_FilePath:
			Command	+= CSG_String::Format(SG_T(" -%s=%s"), GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable:
			Command	+= CSG_String::Format(SG_T(" -%s=%s"), GET_ID1(p), p->asString());
			break;

		case PARAMETER_TYPE_Grid_System:
			if( p->Get_Children_Count() == 0 )
			{
				Command	+= CSG_String::Format(SG_T(" -%s=%d"), GET_ID2(p, SG_T("NX")), p->asGrid_System()->Get_NX());
				Command	+= CSG_String::Format(SG_T(" -%s=%d"), GET_ID2(p, SG_T("NY")), p->asGrid_System()->Get_NY());
				Command	+= CSG_String::Format(SG_T(" -%s=%f"), GET_ID2(p, SG_T( "X")), p->asGrid_System()->Get_XMin());
				Command	+= CSG_String::Format(SG_T(" -%s=%f"), GET_ID2(p, SG_T( "Y")), p->asGrid_System()->Get_YMin());
				Command	+= CSG_String::Format(SG_T(" -%s=%f"), GET_ID2(p, SG_T( "D")), p->asGrid_System()->Get_Cellsize());
			}
			break;

		case PARAMETER_TYPE_DataObject_Output:
		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
			Command	+= CSG_String::Format(SG_T(" -%s=%s"), GET_ID1(p), g_pData->Get(p->asDataObject()) && p->asDataObject()->Get_File_Name() ? p->asDataObject()->Get_File_Name() : SG_T("NULL"));
			break;

		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
		case PARAMETER_TYPE_PointCloud_List:
			if( p->is_Input() )
			{
				Command	+= CSG_String::Format(SG_T(" -%s="), GET_ID1(p));

				if( p->asList()->Get_Count() == 0 )
				{
					Command	+= SG_T("NULL");
				}
				else
				{
					Command	+= SG_File_Exists(p->asList()->asDataObject(0)->Get_File_Name())
							 ? p->asList()->asDataObject(0)->Get_File_Name() : _TL("memory");

					for(int iObject=1; iObject<p->asList()->Get_Count(); iObject++)
					{
						Command	+= SG_T(";");
						Command	+= SG_File_Exists(p->asList()->asDataObject(iObject)->Get_File_Name())
								 ? p->asList()->asDataObject(iObject)->Get_File_Name() : _TL("memory");
					}
				}
			}
			break;
		}
	}
}

//---------------------------------------------------------
void CWKSP_Module::_Get_Python(CSG_String &Command, CSG_Parameters *pParameters)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		if( !p->is_Enabled() )
		{
			continue;
		}

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(%d)\n"), p->Get_Identifier(), p->asBool() ? 1 : 0);
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_Table_Field:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(%d)\n"), p->Get_Identifier(), p->asInt());
			break;

		case PARAMETER_TYPE_Table_Fields:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(%s)\n"), p->Get_Identifier(), p->asString());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(%f)\n"), p->Get_Identifier(), p->asDouble());
			break;

		case PARAMETER_TYPE_Range:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).asRange().Set_LoVal(%f)\n"), p->Get_Identifier(), p->asRange()->Get_LoVal());
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).asRange().Set_HiVal(%f)\n"), p->Get_Identifier(), p->asRange()->Get_HiVal());
			break;

		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_FilePath:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(%s)\n"), p->Get_Identifier(), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable:
			Command	+= CSG_String::Format(SG_T("#   Parms.Get(unicode('%s')).Set_Value(saga_api.SG_Create_Table('table.txt'))\n"), p->Get_Identifier());
			break;

		case PARAMETER_TYPE_Grid_System:
			if( p->Get_Children_Count() == 0 )
			{
				Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(saga_api.CSG_Grid_System(%f, %f, %f, %d, %d))\n"), p->Get_Identifier(),
					p->asGrid_System()->Get_Cellsize(),
					p->asGrid_System()->Get_XMin()	, p->asGrid_System()->Get_YMin(),
					p->asGrid_System()->Get_NX()	, p->asGrid_System()->Get_NY());
			}
			break;

		case PARAMETER_TYPE_Grid:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(use_variable_of_dataset_here) # %s %s grid\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_Table:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(use_variable_of_dataset_here) # %s %s table\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_Shapes:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(use_variable_of_dataset_here) # %s %s shapes\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_TIN:
			Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(use_variable_of_dataset_here) # %s %s TIN\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
		case PARAMETER_TYPE_PointCloud_List:
			if( p->is_Input() )
			{
				if( !p->is_Optional() )
					Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(use_variable_of_dataset_here) # data object list\n"), p->Get_Identifier());
				else
					Command	+= CSG_String::Format(SG_T("    Parms.Get(unicode('%s')).Set_Value(use_variable_of_dataset_here) # optional data object list\n"), p->Get_Identifier());
			}
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
