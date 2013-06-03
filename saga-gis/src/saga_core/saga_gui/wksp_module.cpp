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
#include "wksp_module_library.h"
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
CWKSP_Module::CWKSP_Module(CSG_Module *pModule, const wxString &Menu_Library)
{
	m_pModule	= pModule;
	m_Menu_ID	= -1;
	m_Menu_Path	.Clear();

	//-----------------------------------------------------
	wxString	Menu_Module	= m_pModule->Get_MenuPath().w_str();

	if( Menu_Module.Length() > 2 && Menu_Module[1] == ':' )
	{
		if( Menu_Module[0] == 'A' || Menu_Module[0] == 'a' )		// absolute menu path, overwrites library's default menu path
		{
			m_Menu_Path.Printf(wxT("%s"), Menu_Module.AfterFirst(':'));
		}
		else //if( Menu_Module[0] == 'R' || Menu_Module[0] == 'r' )	// menu path explicitly declared as relative to library's default menu path
		{
			Menu_Module	= Menu_Module.AfterFirst(':');
		}
	}

	//-----------------------------------------------------
	if( m_Menu_Path.Length() == 0 )	// menu path is relative to library's default menu
	{
		if( Menu_Library.Length() > 0 && Menu_Module.Length() > 0 )
		{
			m_Menu_Path.Printf(wxT("%s|%s"), Menu_Library, Menu_Module);
		}
		else if( Menu_Library.Length() > 0 )
		{
			m_Menu_Path.Printf(wxT("%s"), Menu_Library);
		}
		else if( Menu_Module.Length() > 0 )
		{
			m_Menu_Path.Printf(wxT("%s"), Menu_Module);
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
		? wxString::Format(wxT("%s [%s]"), m_pModule->Get_Name().c_str(), _TL("interactive"))
		: wxString::Format(wxT("%s")     , m_pModule->Get_Name().c_str())
	);
}

//---------------------------------------------------------
void CWKSP_Module::Set_File_Name(const wxString &File_Name)
{
	m_File_Name = File_Name;
}

//---------------------------------------------------------
wxString CWKSP_Module::Get_Description(void)
{
	wxString	Description;

	if( g_pModules->Get_Parameters()->Get_Parameter("HELP_SOURCE")->asInt() == 1 )
	{
		Description	= Get_Online_Module_Description(((CWKSP_Module_Library *)Get_Manager())->Get_File_Name(), Get_Module()->Get_ID());
	}

	return( m_pModule->Get_Summary(true, &m_Menu_Path, &Description).c_str() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Module::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(Get_Name());

	pMenu->AppendCheckItem(Get_Menu_ID(), _TL("Execute"), _TL("Execute Module"));

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MODULES_SAVE_SCRIPT);

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
		_Save_Script();
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
void CWKSP_Module::_Save_Script(void)
{
	wxString	FileName;

	if( DLG_Save(FileName, _TL("Create Script Command File"), SG_T("DOS Batch Script (*.bat)|*.bat|Python Script (*.py)|*.py")) )
	{
		CSG_File	File;
		CSG_String	Command;

		if(      SG_File_Cmp_Extension(FileName, SG_T("bat")) )
		{
			Command	+= SG_T("@ECHO OFF\n\n");

			Command	+= SG_T("REM SET SAGA_MLB=C:\\SAGA\\Modules\n");
			Command	+= SG_T("REM SET PATH=%PATH%;C:\\SAGA\n\n");

			Command	+= SG_T("saga_cmd ");

			Command	+= SG_File_Get_Name(((CWKSP_Module_Library *)Get_Manager())->Get_File_Name(), false);

			Command	+= SG_T(" \"");
			Command	+= m_pModule->Get_Name();
			Command	+= SG_T("\"");

			_Save_Script_CMD(Command, m_pModule->Get_Parameters());

			for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
			{
				_Save_Script_CMD(Command, m_pModule->Get_Parameters(i));
			}

			Command	+= SG_T("\n\nPAUSE\n");
		}
		else if( SG_File_Cmp_Extension(FileName, SG_T("py" )) )
		{
			Command	+= SG_T("# Python script template for SAGA module execution (automatically created, experimental)\n\n");
			Command	+= SG_T("import saga_api, sys, os\n");
			Command	+= SG_T("\n");
			Command	+= SG_T("def Call_SAGA_Module(in__grid, out_grid, in__shapes, out_shapes):\n");
			Command	+= SG_T("    print saga_api.SAGA_API_Get_Version()\n");
			Command	+= SG_T("\n");
			Command	+= SG_T("    Library = saga_api.CSG_Module_Library()\n");
			Command	+= SG_T("    if Library.Create(saga_api.CSG_String('");
			Command	+= ((CWKSP_Module_Library *)Get_Manager())->Get_File_Name().wc_str();
			Command	+= SG_T("')) == 0:\n");
			Command	+= SG_T("        print 'unable to load SAGA module library'\n");
			Command	+= SG_T("        return 0\n");
			Command	+= SG_T("\n");

			switch( m_pModule->Get_Type() )
			{
			default:
				Command	+= CSG_String::Format(SG_T("    Module = Library.Get_Module('%s')\n")		, m_pModule->Get_Name().c_str());
				break;

			case MODULE_TYPE_Grid:
				Command	+= CSG_String::Format(SG_T("    Module = Library.Get_Module_Grid('%s')\n")	, m_pModule->Get_Name().c_str());
				Command	+= SG_T("    Module.Get_System().Assign(in__grid.Get_System())\n");
				break;
			}

			Command	+= SG_T("\n");
			Command	+= SG_T("    Parms = Module.Get_Parameters() # default parameter list\n");
			_Save_Script_Python(Command, m_pModule->Get_Parameters());

			for(int i=0; i<m_pModule->Get_Parameters_Count(); i++)
			{
				Command	+= SG_T("\n");
				Command	+= CSG_String::Format(SG_T("    Parms = Module.Get_Parameters(%d) # additional parameter list\n"), i);
				_Save_Script_Python(Command, m_pModule->Get_Parameters(i));
			}

			Command	+= SG_T("\n");
			Command	+= SG_T("    if Module.Execute() == 0:\n");
			Command	+= SG_T("        print 'module execution failed'\n");
			Command	+= SG_T("        return 0\n");
			Command	+= SG_T("    print 'module successfully executed'\n");
			Command	+= SG_T("    return 1\n");
			Command	+= SG_T("\n");
			Command	+= SG_T("\n");
			Command	+= SG_T("if __name__ == '__main__':\n");
			Command	+= SG_T("    if len( sys.argv ) != 4:\n");
			Command	+= SG_T("        print 'Usage: this_script.py <in: gridfile> <out: gridfile> <in: shapefile> <out: shapefile>'\n");
			Command	+= SG_T("\n");
			Command	+= SG_T("    else:\n");
			Command	+= SG_T("        in__grid    = saga_api.SG_Create_Grid(saga_api.CSG_String(sys.argv[1]))\n");
			Command	+= SG_T("        out_grid    = saga_api.SG_Create_Grid(grid_in.Get_System())\n");
			Command	+= SG_T("        in__shapes  = saga_api.SG_Create_Shapes(saga_api.CSG_String(sys.argv[3]))\n");
			Command	+= SG_T("        out_shapes  = saga_api.SG_Create_Shapes()\n");
			Command	+= SG_T("\n");
			Command	+= SG_T("        if Call_SAGA_Module(in__grid, out_grid, in__shapes, out_shapes) != 0:\n");
			Command	+= SG_T("            grid_out  .Save(saga_api.CSG_String(sys.argv[2]))\n");
			Command	+= SG_T("            shapes_out.Save(saga_api.CSG_String(sys.argv[4]))\n");
		}

		if( File.Open(&FileName, SG_FILE_W, false) && Command.Length() > 0 )
		{
			File.Write(Command);
		}
	}
}

//---------------------------------------------------------
#define GET_ID1(p)		(p->Get_Owner()->Get_Identifier().Length() > 0 \
						? CSG_String::Format(SG_T("%s_%s"), p->Get_Owner()->Get_Identifier().c_str(), p->Get_Identifier()) \
						: CSG_String::Format(p->Get_Identifier())).c_str()

#define GET_ID2(p, s)	CSG_String::Format(SG_T("%s_%s"), GET_ID1(p), s).c_str()

//---------------------------------------------------------
void CWKSP_Module::_Save_Script_CMD(CSG_String &Command, CSG_Parameters *pParameters)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

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
			if( p->asString() != '\0' )
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
			Command	+= CSG_String::Format(SG_T(" -%s=%s"), GET_ID1(p), p->asDataObject() && p->asDataObject()->Get_File_Name() ? p->asDataObject()->Get_File_Name() : SG_T("NULL"));
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
void CWKSP_Module::_Save_Script_Python(CSG_String &Command, CSG_Parameters *pParameters)
{
	for(int iParameter=0; iParameter<pParameters->Get_Count(); iParameter++)
	{
		CSG_Parameter	*p	= pParameters->Get_Parameter(iParameter);

		switch( p->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Bool:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(%d)\n"), p->Get_Identifier(), p->asBool() ? 1 : 0);
			break;

		case PARAMETER_TYPE_Int:
		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_Table_Field:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(%d)\n"), p->Get_Identifier(), p->asInt());
			break;

		case PARAMETER_TYPE_Table_Fields:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(%s)\n"), p->Get_Identifier(), p->asString());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(%f)\n"), p->Get_Identifier(), p->asDouble());
			break;

		case PARAMETER_TYPE_Range:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').asRange().Set_LoVal(%f)\n"), p->Get_Identifier(), p->asRange()->Get_LoVal());
			Command	+= CSG_String::Format(SG_T("    Parms('%s').asRange().Set_HiVal(%f)\n"), p->Get_Identifier(), p->asRange()->Get_HiVal());
			break;

		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_FilePath:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(%s)\n"), p->Get_Identifier(), p->asString());
			break;

		case PARAMETER_TYPE_FixedTable:
			Command	+= CSG_String::Format(SG_T("#   Parms('%s').Set_Value(saga_api.SG_Create_Table('table.txt'))\n"), p->Get_Identifier());
			break;

		case PARAMETER_TYPE_Grid_System:
			if( p->Get_Children_Count() == 0 )
			{
				Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(saga_api.CSG_Grid_System(%f, %f, %f, %d, %d))\n"), p->Get_Identifier(),
					p->asGrid_System()->Get_Cellsize(),
					p->asGrid_System()->Get_XMin()	, p->asGrid_System()->Get_YMin(),
					p->asGrid_System()->Get_NX()	, p->asGrid_System()->Get_NY());
			}
			break;

		case PARAMETER_TYPE_Grid:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(0) # %s %s grid\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_Table:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(0) # %s %s table\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_Shapes:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(0) # %s %s shapes\n"), p->Get_Identifier(),
				p->is_Input()    ? SG_T("input")    : SG_T("output"), p->is_Optional() ? SG_T("optional") : SG_T("NOT optional")
			);
			break;

		case PARAMETER_TYPE_TIN:
			Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(0) # %s %s TIN\n"), p->Get_Identifier(),
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
					Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(0) # data object list\n"), p->Get_Identifier());
				else
					Command	+= CSG_String::Format(SG_T("    Parms('%s').Set_Value(0) # optional data object list\n"), p->Get_Identifier());
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
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), _TL("Module Execution")) )
				{
					PROCESS_Set_Okay(false);
				}
			}
			else if( m_pModule->is_Interactive() )
			{
				if( !bDialog || DLG_Message_Confirm(_TL("Shall execution be stopped?"), _TL("Module Execution")) )
				{
					bResult		= ((CSG_Module_Interactive *)m_pModule)->Execute_Finish();
					g_pModule	= NULL;

					PROCESS_Set_Okay(true);

					MSG_ADD2(bResult,
						_TL("Interactive module execution has been stopped"),
						_TL("Interactive module execution failed")
					);
				}
			}
		}
		else
		{
			DLG_Message_Show(_TL("Can't execute a module while another runs"), _TL("Module Execution"));
		}
	}

	//-----------------------------------------------------
	else
	{
		g_pModule	= this;

		if( m_pModule->On_Before_Execution() && (!bDialog || DLG_Parameters(m_pModule->Get_Parameters())) )
		{
			g_pModules->Get_Menu_Modules()->Set_Recent(this);

			MSG_General_Add_Line();
			MSG_Execution_Add_Line();
			MSG_ADD(wxString::Format(wxT("%s: %s"), _TL("Executing module"), m_pModule->Get_Name().c_str()));

			STATUSBAR_Set_Text(m_pModule->Get_Name().w_str());

			bResult		= m_pModule->Execute();

			g_pACTIVE->Get_Parameters()->Update_Parameters(m_pModule->Get_Parameters(), false);

			if( m_pModule->is_Interactive() )
			{
				MSG_ADD2(bResult,
					_TL("Interactive module execution has been started"),
					_TL("Interactive module execution failed")
				);
			}
			else
			{
				MSG_ADD2(bResult,
					_TL("Module execution succeeded"),
					_TL("Module execution failed")
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
