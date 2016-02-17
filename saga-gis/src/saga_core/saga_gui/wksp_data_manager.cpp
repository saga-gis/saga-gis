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
//                 WKSP_Data_Manager.cpp                 //
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
#ifdef _SAGA_LINUX
#include <stdlib.h>
#endif

#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "saga.h"

#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"
#include "project.h"

#include "wksp_data_control.h"
#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"
#include "wksp_data_layers.h"

#include "wksp_layer.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"
#include "wksp_shapes_manager.h"
#include "wksp_tin_manager.h"
#include "wksp_pointcloud_manager.h"
#include "wksp_grid_manager.h"

#include "wksp_map_manager.h"

#include "wksp_module.h"

#include "active.h"
#include "active_parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Manager	*g_pData	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Manager::CWKSP_Data_Manager(void)
{
	g_pData			= this;

	m_pTables		= NULL;
	m_pShapes		= NULL;
	m_pTINs			= NULL;
	m_pPointClouds	= NULL;
	m_pGrids		= NULL;

	m_pProject		= new CWKSP_Project;
	m_pMenu_Files	= new CWKSP_Data_Menu_Files;

	//-----------------------------------------------------
	CSG_Parameter	*pNode, *pNode_1, *pNode_2;

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_GENERAL", _TL("General"), _TL(""));

	m_Parameters.Add_Choice(
		pNode	, "PROJECT_START"			, _TL("Startup Project"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("empty"),
			_TL("last state"),
			_TL("always ask what to do")
		), 2
	);

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "PROJECT_MAP_ARRANGE"		, _TL("Map Window Arrangement"),
		_TL("initial map window arrangement after a project is loaded"),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Cascade"),
			_TL("Tile Horizontally"),
			_TL("Tile Vertically")
		), 2
	);

#ifndef _SAGA_MSW
	pNode_1->Set_Enabled(false);
#endif

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "PROJECT_DB_REOPEN"		, _TL("Reopen Database Connections"),
		_TL("Reopen PostgreSQL database connections. Warning: if set to true account information including unencrypted paswords for automatic connection will be stored."),
		CSG_String::Format("%s|%s|",
			_TL("no"),
			_TL("yes")
		), 0
	);

	m_Parameters.Add_Value(
		pNode	, "SHOW_FILE_SOURCES"		, _TL("Show Data File Sources"),
		_TL("Show data sources tab for file system. Disabling might speed up start-up. Changes take effect after restart."),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		pNode	, "NUMBERING"				, _TL("Numbering of Data Sets"),
		_TL("Leading zeros for data set numbering. Set to -1 for not using numbers at all."),
		PARAMETER_TYPE_Int, m_Numbering = 2, -1, true
	);

	m_Parameters.Add_Value(
		pNode	, "HISTORY_DEPTH"			, _TL("History Depth"),
		_TL("Depth to which data history is stored. Set -1 keeps all history entries (default), 0 switches history option off."),
		PARAMETER_TYPE_Int, SG_Get_History_Depth(), -1, true
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_THUMBNAILS", _TL("Thumbnails"), _TL(""));

	m_Parameters.Add_Value(
		pNode	, "THUMBNAIL_SIZE"		, _TL("Thumbnail Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 75, 10, true
	);

	m_Parameters.Add_Value(
		pNode	, "THUMBNAIL_CATEGORY"	, _TL("Show Categories"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Value(
		pNode	, "THUMBNAIL_SELCOLOR"	, _TL("Selection Color"),
		_TL(""),
		PARAMETER_TYPE_Color, Get_Color_asInt(SYS_Get_Color(wxSYS_COLOUR_BTNSHADOW))
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_GRID", _TL("Grids"), _TL(""));

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Node(pNode, "NODE_GRID_DISPLAY", _TL("Display"), _TL(""));

	pNode_2	= m_Parameters.Add_Choice(
		pNode_1	, "GRID_COLORS_FIT"			, _TL("Histogram Stretch"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Minimum/Maximum"),
			_TL("Standard Deviation"),
			_TL("Percentile")
		), 1
	);

	m_Parameters.Add_Value(
		pNode_2	, "GRID_COLORS_FIT_STDDEV"	, _TL("Standard Deviation"),
		_TL("Multiple of Standard Deviation used as default for histogram stretch."),
		PARAMETER_TYPE_Double, 2.0, 0.01, true
	);

	m_Parameters.Add_Value(
		pNode_2	, "GRID_COLORS_FIT_PCTL"	, _TL("Percentile"),
		_TL("Percentile used as default for histogram stretch."),
		PARAMETER_TYPE_Double, 2.0, 0.0, true, 50.0, true
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Node(pNode, "NODE_GRID_SELECTION", _TL("Selection"), _TL(""));

	m_Parameters.Add_Value(
		pNode_1	, "GRID_SELECT_MAX"			, _TL("Maximum Selection"),
		_TL("Maximum number of rows/columns in selection of grid cells."),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Node(pNode, "NODE_GRID_CACHE", _TL("File Caching"), _TL(""));

	m_Parameters.Add_Value(
		pNode_1	, "GRID_CACHE_AUTO"		, _TL("Automatic"),
		_TL("Activate file caching automatically, if memory size exceeds the threshold value."),
		PARAMETER_TYPE_Bool, SG_Grid_Cache_Get_Automatic()
	);

	m_Parameters.Add_Value(
		pNode_1	, "GRID_CACHE_THRSHLD"	, _TL("Threshold for automatic mode [MB]"),
		_TL(""),
		PARAMETER_TYPE_Double, SG_Grid_Cache_Get_Threshold_MB(), 0.0, true
	);

	m_Parameters.Add_Choice(
		pNode_1	, "GRID_CACHE_CONFIRM"	, _TL("Confirm file caching"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("do not confirm"),
			_TL("confirm"),
			_TL("confirm with options")
		), SG_Grid_Cache_Get_Confirm()
	);

	m_Parameters.Add_FilePath(
		pNode_1	, "GRID_CACHE_TMPDIR"	, _TL("Temporary files"),
		_TL("Directory, where temporary cache files shall be saved."),
		NULL, SG_Grid_Cache_Get_Directory(), true, true
	);

	//-----------------------------------------------------
	CONFIG_Read("/DATA", &m_Parameters);

	SG_Grid_Cache_Set_Directory   (m_Parameters("GRID_CACHE_TMPDIR" )->asString());
	SG_Grid_Cache_Set_Automatic   (m_Parameters("GRID_CACHE_AUTO"   )->asBool  ());
	SG_Grid_Cache_Set_Threshold_MB(m_Parameters("GRID_CACHE_THRSHLD")->asDouble());
	SG_Grid_Cache_Set_Confirm     (m_Parameters("GRID_CACHE_CONFIRM")->asInt   ());

	SG_Set_History_Depth(m_Parameters("HISTORY_DEPTH")->asInt());

	m_Numbering	= m_Parameters("NUMBERING")->asInt();
}

//---------------------------------------------------------
CWKSP_Data_Manager::~CWKSP_Data_Manager(void)
{
	CONFIG_Write("/DATA", &m_Parameters);

	delete(m_pProject);
	delete(m_pMenu_Files);

	g_pData	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxFileName Get_SAGA_GUI_CFG(void)
{
#ifdef _SAGA_LINUX
//	wxFileName	fProject(wxString(getenv( "HOME"), wxConvFile ), wxT("saga_gui"), wxT("cfg"));
	CSG_String	sHome(getenv("HOME"));
	wxFileName	fProject(sHome.c_str(), "saga_gui", "cfg");
#else
	wxFileName	fProject(g_pSAGA->Get_App_Path(), "saga_gui", "cfg");

	if(	( fProject.FileExists() && (!fProject.IsFileReadable() || !fProject.IsFileWritable()))
	||	(!fProject.FileExists() && (!fProject.IsDirReadable () || !fProject.IsDirWritable ())) )
	{
		fProject.Assign(wxGetHomeDir(), "saga_gui", "cfg");
	}
#endif

	fProject.Normalize();

	return( fProject );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Initialise(void)
{
	//-----------------------------------------------------
	if( m_pProject->Has_File_Name() )	{	return( m_pProject->Load(false) );	}
	//-----------------------------------------------------

	//-----------------------------------------------------
	wxFileName	fProject, fLastState	= Get_SAGA_GUI_CFG();

	switch( m_Parameters("PROJECT_START")->asInt() )
	{
	case 0:	// empty
		return( true );

	case 1:	// last state
		return( m_pProject->Load(fLastState.GetFullPath(), false, false) );

	case 2:	// always ask what to do
		{
			wxArrayString	Projects;

			Projects.Add(wxString::Format("[%s]", _TL("empty")));

			if( fLastState.FileExists() )
			{
				Projects.Add(wxString::Format("[%s]", _TL("last state")));
			}

			m_pMenu_Files->Recent_Get(DATAOBJECT_TYPE_Undefined, Projects, true);

			wxSingleChoiceDialog	dlg(MDI_Get_Top_Window(), _TL("Startup Project"), _TL("Select Startup Project"), Projects);

			if( Projects.Count() <= 1 || dlg.ShowModal() != wxID_OK || dlg.GetSelection() == 0 )
			{	// empty
				return( true );
			}

			if( fLastState.FileExists() && dlg.GetSelection() == 1 )
			{	// last state
				return( m_pProject->Load(fLastState.GetFullPath(), false, false) );
			}
			else
			{	// recently opened project
				return( m_pProject->Load(dlg.GetStringSelection(), false, true) );
			}
		}
	}

	//-----------------------------------------------------
	//wxString	FileName	= Get_FilePath_Absolute(g_pSAGA->Get_App_Path(), fProject.GetFullPath());

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Finalise(void)
{
	wxFileName	fProject	= Get_SAGA_GUI_CFG();

	if( Get_Count() > 0 )
	{	// last state
		m_pProject->Save(fProject.GetFullPath(), false);

		if( fProject.GetPath().Find(g_pSAGA->Get_App_Path()) == 0 )
		{
			fProject.MakeRelativeTo(g_pSAGA->Get_App_Path());
		}
	}
	else if( fProject.FileExists() )
	{
		wxRemoveFile(fProject.GetFullPath());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Data_Manager::Get_Name(void)
{
	return( _TL("Data") );
}

//---------------------------------------------------------
wxString CWKSP_Data_Manager::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Data"));

	s	+= "<table border=\"0\">";

	DESC_ADD_INT(_TL("Number of Data Sets"), Get_Count());

	if( m_pProject->Has_File_Name() )
	{
		DESC_ADD_STR(_TL("Project File"), m_pProject->Get_File_Name());
	}

	DESC_ADD_INT(_TL("Tables"      ), Get_Tables     () ? Get_Tables     ()->Get_Count      () : 0);
	DESC_ADD_INT(_TL("Shapes"      ), Get_Shapes     () ? Get_Shapes     ()->Get_Items_Count() : 0);
	DESC_ADD_INT(_TL("TIN"         ), Get_TINs       () ? Get_TINs       ()->Get_Count      () : 0);
	DESC_ADD_INT(_TL("Point Clouds"), Get_PointClouds() ? Get_PointClouds()->Get_Count      () : 0);
	DESC_ADD_INT(_TL("Grid Systems"), Get_Grids      () ? Get_Grids      ()->Get_Count      () : 0);
	DESC_ADD_INT(_TL("Grids"       ), Get_Grids      () ? Get_Grids      ()->Get_Items_Count() : 0);

	s	+= wxT("</table>");

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Data_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(_TL("Data"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_OPEN);
//	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_OPEN_ADD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_CLOSE);

	if( Get_Count() > 0 )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_SAVE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_SAVE_AS);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_COPY);
	//	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_COPY_DB);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SEARCH);
	}

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::On_Command(int Cmd_ID)
{
	//-----------------------------------------------------
	if( Open_CMD(Cmd_ID) )
	{
		return( true );
	}

	if( m_pTables && Cmd_ID >= ID_CMD_TABLES_FIRST && Cmd_ID <= ID_CMD_TABLES_LAST && m_pTables->On_Command(Cmd_ID) )
	{
		return( true );
	}

	if( m_pShapes && Cmd_ID >= ID_CMD_SHAPES_FIRST && Cmd_ID <= ID_CMD_SHAPES_LAST && m_pShapes->On_Command(Cmd_ID) )
	{
		return( true );
	}

	if( m_pTINs   && Cmd_ID >= ID_CMD_TIN_FIRST    && Cmd_ID <= ID_CMD_TIN_LAST    && m_pTINs  ->On_Command(Cmd_ID) )
	{
		return( true );
	}

	if( m_pPointClouds && Cmd_ID >= ID_CMD_POINTCLOUD_FIRST && Cmd_ID <= ID_CMD_POINTCLOUD_LAST && m_pPointClouds->On_Command(Cmd_ID) )
	{
		return( true );
	}

	if( m_pGrids  && Cmd_ID >= ID_CMD_GRIDS_FIRST  && Cmd_ID <= ID_CMD_GRIDS_LAST  && m_pGrids ->On_Command(Cmd_ID) )
	{
		return( true );
	}

	//-----------------------------------------------------
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Manager::On_Command(Cmd_ID) );

	//-----------------------------------------------------
	case ID_CMD_DATA_PROJECT_OPEN:		m_pProject->Load(false);			break;
	case ID_CMD_DATA_PROJECT_OPEN_ADD:	m_pProject->Load(true);				break;
	case ID_CMD_DATA_PROJECT_BROWSE:	Open_Browser();						break;
	case ID_CMD_DATA_PROJECT_CLOSE:		Close(false);						break;
	case ID_CMD_DATA_PROJECT_SAVE:		m_pProject->Save(true);				break;
	case ID_CMD_DATA_PROJECT_SAVE_AS:	m_pProject->Save();					break;
	case ID_CMD_DATA_PROJECT_COPY:		m_pProject->Copy();					break;
	case ID_CMD_DATA_PROJECT_COPY_DB:	m_pProject->CopyToDB();				break;

	//-----------------------------------------------------
	case ID_CMD_TABLES_OPEN:			Open(DATAOBJECT_TYPE_Table);		break;
	case ID_CMD_SHAPES_OPEN:			Open(DATAOBJECT_TYPE_Shapes);		break;
	case ID_CMD_TIN_OPEN:				Open(DATAOBJECT_TYPE_TIN);			break;
	case ID_CMD_POINTCLOUD_OPEN:		Open(DATAOBJECT_TYPE_PointCloud);	break;
	case ID_CMD_GRIDS_OPEN:				Open(DATAOBJECT_TYPE_Grid);			break;

	//-----------------------------------------------------
	case ID_CMD_WKSP_ITEM_RETURN:
		break;

	case ID_CMD_WKSP_ITEM_CLOSE:
		Close(false);
		break;
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Manager::On_Command_UI(event) );

	case ID_CMD_DATA_PROJECT_CLOSE:
		event.Enable(Get_Count() > 0 && g_pModule == NULL);
		break;

	case ID_CMD_WKSP_ITEM_CLOSE:
		event.Enable(Get_Count() > 0 && g_pModule == NULL);
		break;

	case ID_CMD_DATA_PROJECT_SAVE:
		event.Enable(Get_Count() > 0 && m_pProject->Has_File_Name() );
		break;

	case ID_CMD_DATA_PROJECT_SAVE_AS:
		event.Enable(Get_Count() > 0);
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
CSG_Parameters * CWKSP_Data_Manager::Get_Parameters(void)
{
	if( m_Sel_Parms[0].Get_Count() > 0 )
	{
		return( &m_Sel_Parms[0] );
	}

	return( m_Parameters.Get_Count() > 0 ? &m_Parameters : NULL );
}

//---------------------------------------------------------
void CWKSP_Data_Manager::Parameters_Changed(void)
{
	if( MultiSelect_Update() )
	{
		return;
	}

	SG_Grid_Cache_Set_Directory   (m_Parameters("GRID_CACHE_TMPDIR" )->asString());
	SG_Grid_Cache_Set_Automatic   (m_Parameters("GRID_CACHE_AUTO"   )->asBool  ());
	SG_Grid_Cache_Set_Threshold_MB(m_Parameters("GRID_CACHE_THRSHLD")->asDouble());
	SG_Grid_Cache_Set_Confirm     (m_Parameters("GRID_CACHE_CONFIRM")->asInt   ());

	SG_Set_History_Depth(m_Parameters("HISTORY_DEPTH")->asInt());

	m_Numbering	= m_Parameters("NUMBERING")->asInt();

	g_pData_Buttons->Update_Buttons();

	CWKSP_Base_Manager::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Data_Manager::Open(const wxString &File, int DataType)
{
	CSG_Data_Object	*pObject	= NULL;

//	SG_Get_Data_Manager().Add(File, DataType);

	switch( DataType )
	{
	case DATAOBJECT_TYPE_Table:			pObject	= SG_Create_Table		(&File);	break;
	case DATAOBJECT_TYPE_Shapes:		pObject	= SG_Create_Shapes		(&File);	break;
	case DATAOBJECT_TYPE_TIN:			pObject	= SG_Create_TIN			(&File);	break;
	case DATAOBJECT_TYPE_PointCloud:	pObject	= SG_Create_PointCloud	(&File);	break;
	case DATAOBJECT_TYPE_Grid:			pObject	= SG_Create_Grid		(&File);	break;
	}

	if( pObject )
	{
		CWKSP_Data_Item	*pItem;

		if( pObject->is_Valid() && (pItem = Add(pObject)) != NULL )
		{
			m_pMenu_Files->Recent_Add(pObject->Get_ObjectType(), File);

			SG_Get_Data_Manager().Add(pObject);

			return( (CWKSP_Base_Item *)pItem );
		}

		delete(pObject);
	}

	m_pMenu_Files->Recent_Del(DataType, File);

	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open(const wxString &File)
{
	if( SG_File_Cmp_Extension(File, SG_T("sprj")) )
	{
		return( m_pProject->Load(File, false, true) );
	}

	if( SG_File_Cmp_Extension(File, SG_T("txt" ))
	||	SG_File_Cmp_Extension(File, SG_T("csv" ))
	||	SG_File_Cmp_Extension(File, SG_T("dbf" )) )
	{
		return( Open(File, DATAOBJECT_TYPE_Table     ) != NULL );
	}

	if( SG_File_Cmp_Extension(File, SG_T("shp" )) )
	{
		return( Open(File, DATAOBJECT_TYPE_Shapes    ) != NULL );
	}

	if( SG_File_Cmp_Extension(File, SG_T("spc" )) )
	{
		return( Open(File, DATAOBJECT_TYPE_PointCloud) != NULL );
	}

	if(	SG_File_Cmp_Extension(File, SG_T("sgrd"))
	||	SG_File_Cmp_Extension(File, SG_T("dgm" ))
	||	SG_File_Cmp_Extension(File, SG_T("grd" )) )
	{
		return( Open(File, DATAOBJECT_TYPE_Grid      ) != NULL );
	}

	return( SG_Get_Data_Manager().Add(&File) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open(int DataType)
{
	bool	bResult	= false;

	int				ID;
	wxArrayString	Files;

	//-----------------------------------------------------
	switch( DataType )
	{
	default:	return( false );
	case DATAOBJECT_TYPE_Table:			ID	= ID_DLG_TABLES_OPEN;		break;
	case DATAOBJECT_TYPE_Shapes:		ID	= ID_DLG_SHAPES_OPEN;		break;
	case DATAOBJECT_TYPE_TIN:			ID	= ID_DLG_TIN_OPEN;			break;
	case DATAOBJECT_TYPE_PointCloud:	ID	= ID_DLG_POINTCLOUD_OPEN;	break;
	case DATAOBJECT_TYPE_Grid:			ID	= ID_DLG_GRIDS_OPEN;		break;
	}

	//-----------------------------------------------------
	if( DLG_Open(Files, ID) )
	{
		MSG_General_Add_Line();

		for(size_t i=0; i<Files.GetCount(); i++)
		{
			if( Open(Files[i], DataType) )
			{
				bResult	= true;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open_CMD(int Cmd_ID)
{
	return( m_pMenu_Files->Recent_Open(Cmd_ID) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open_Browser(void)
{
	//-----------------------------------------------------
	wxString	Directory	= wxDirSelector(_TL("Search for Projects"));

	if( Directory.IsEmpty() )
	{
		return( false );
	}

	//-----------------------------------------------------
	wxArrayString	Projects;

	Open_Browser(Projects, Directory);

	if( Projects.Count() == 0 )
	{
		wxMessageBox(_TL("No projects in directory"), _TL("Search for Projects"), wxOK|wxICON_EXCLAMATION);

		return( false );
	}

	//-----------------------------------------------------
	wxSingleChoiceDialog	dlg(MDI_Get_Top_Window(), _TL("Open Project"), _TL("Search for Projects"), Projects);

	return( dlg.ShowModal() == wxID_OK && Open(dlg.GetStringSelection()) );
}

//---------------------------------------------------------
#include <wx/dir.h>

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open_Browser(wxArrayString &Projects, const wxString &Directory)
{
	wxDir	Dir;

	if( Dir.Open(Directory) )
	{
		wxString	FileName;

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_FILES) )
		{
			do
			{
				wxFileName	fn(Dir.GetName(), FileName);

				if( !fn.GetExt().CmpNoCase("sprj") )
				{
					Projects.Add(fn.GetFullPath());
				}
			}
			while( Dir.GetNext(&FileName) );
		}

		if( Dir.GetFirst(&FileName, wxEmptyString, wxDIR_DIRS) )
		{
			do
			{
				wxFileName	fn(Dir.GetName(), FileName);

				Open_Browser(Projects, fn.GetFullPath());
			}
			while( Dir.GetNext(&FileName) );
		}
	}

	return( Projects.Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Save_Modified(CWKSP_Base_Item *pItem, bool bSelections)
{
	CSG_Parameters	Parameters(this, _TL("Save Modified Data"), _TL(""));

	Parameters.Add_Value(NULL, "SAVE_ALL", _TL("Save all"), _TL(""), PARAMETER_TYPE_Bool, false);

	wxFileName	Directory(m_pProject->Get_File_Name());

	if( !Directory.DirExists() )
	{
		Directory.AssignDir(wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + "saga");

		Directory.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	}

	_Modified_Get(&Parameters, pItem ? pItem : this, Directory.GetPath(), bSelections);

	if( Parameters.Get_Count() > 1 )
	{
		Parameters.Set_Callback_On_Parameter_Changed(&_Modified_Changed);

		if( !DLG_Parameters(&Parameters) )
		{
			return( false );
		}

		_Modified_Save(&Parameters);
	}

	return( true );
}

//---------------------------------------------------------
int CWKSP_Data_Manager::_Modified_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( !pParameter || !pParameter->Get_Owner() || !pParameter->Get_Owner()->Get_Owner() )
	{
		return( 0 );
	}

	CSG_Parameters	*pParameters	= pParameter->Get_Owner();

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SAVE_ALL")) )
	{
		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter	*pFile	= pParameters->Get_Parameter(i);

			if( pFile->Get_Type() == PARAMETER_TYPE_Bool )
			{
				pFile->Set_Value(pParameter->asBool());

				for(int j=0; j<pFile->Get_Children_Count(); j++)
				{
					pFile->Get_Child(j)->Set_Enabled(pParameter->asBool());
				}
			}
		}
	}

	else if( pParameter->Get_Type() == PARAMETER_TYPE_Bool )
	{
		if( !pParameter->asBool() && pParameters->Get_Parameter("SAVE_ALL") )
		{
			pParameters->Get_Parameter("SAVE_ALL")->Set_Value(0);
		}

		for(int j=0; j<pParameter->Get_Children_Count(); j++)
		{
			pParameter->Get_Child(j)->Set_Enabled(pParameter->asBool());
		}
	}

	return( 0 );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::_Modified_Get(CSG_Parameters *pParameters, CWKSP_Base_Item *pItem, const wxString &Directory, bool bSelections)
{
	int		i;

	if( pItem && pParameters )
	{
		switch( pItem->Get_Type() )
		{
		default:
			break;

		//-------------------------------------------------
		case WKSP_ITEM_Data_Manager:
		case WKSP_ITEM_Table_Manager:
		case WKSP_ITEM_Shapes_Manager:
		case WKSP_ITEM_Shapes_Type:
		case WKSP_ITEM_TIN_Manager:
		case WKSP_ITEM_PointCloud_Manager:
		case WKSP_ITEM_Grid_Manager:
		case WKSP_ITEM_Grid_System:
			for(i=0; i<((CWKSP_Base_Manager *)pItem)->Get_Count(); i++)
			{
				_Modified_Get(pParameters, ((CWKSP_Base_Manager *)pItem)->Get_Item(i), Directory, bSelections && !pItem->is_Selected());
			}
			break;

		//-------------------------------------------------
		case WKSP_ITEM_Table:
			if( !bSelections || pItem->is_Selected() )
			{
				_Modified_Get(pParameters, pItem, Directory, ((CWKSP_Table *)pItem)->Get_Table() );
			}
			break;

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Grid:
			if( !bSelections || pItem->is_Selected() )
			{
				_Modified_Get(pParameters, pItem, Directory, ((CWKSP_Layer *)pItem)->Get_Object());
			}
			break;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::_Modified_Get(CSG_Parameters *pParameters, CWKSP_Base_Item *pItem, const wxString &Directory, CSG_Data_Object *pObject)
{
	if( !pObject->is_Modified() )
	{
		return( false );
	}

	//-----------------------------------------------------
	wxString	Filter, Extension;

	switch( pItem->Get_Type() )
	{
	default:	return( false );
	case WKSP_ITEM_Table:		Extension	= "txt" ;	Filter	= DLG_Get_FILE_Filter(ID_DLG_TABLES_SAVE    );	break;
	case WKSP_ITEM_Shapes:		Extension	= "shp" ;	Filter	= DLG_Get_FILE_Filter(ID_DLG_SHAPES_SAVE    );	break;
	case WKSP_ITEM_TIN:			Extension	= "shp" ;	Filter	= DLG_Get_FILE_Filter(ID_DLG_TIN_SAVE       );	break;
	case WKSP_ITEM_PointCloud:	Extension	= "spc" ;	Filter	= DLG_Get_FILE_Filter(ID_DLG_POINTCLOUD_SAVE);	break;
	case WKSP_ITEM_Grid:		Extension	= "sgrd";	Filter	= DLG_Get_FILE_Filter(ID_DLG_GRIDS_SAVE     );	break;
	}

	//-----------------------------------------------------
	wxFileName	Path(pObject->Get_File_Name());

	if( !Path.FileExists() )
	{
		wxString	Name(pObject->Get_Name());

		Name.Replace(".", "-");
		Name.Replace(":", "-");

		Path.SetPath(Directory);
		Path.SetExt (Extension);

		for(int i=0, bOkay=false; !bOkay; i++)
		{
			if( i == 0 )
			{
				Path.SetName(Name);
			}
			else
			{
				Path.SetName(Name + wxString::Format("_%d", i));
			}

			bOkay	= !Path.FileExists();

			for(int j=0; bOkay && j<pParameters->Get_Count(); j++)
			{
				CSG_Parameter	*p	= pParameters->Get_Parameter(j);

				if( p->Get_Type() == PARAMETER_TYPE_FilePath && Path == p->asString() )
				{
					bOkay	= false;
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	if( (pNode = pParameters->Get_Parameter(CSG_String::Format(SG_T("%d"), (long)pItem->Get_Manager()))) == NULL )
	{
		pNode	= pParameters->Add_Node(NULL, CSG_String::Format(SG_T("%d"), (long)pItem->Get_Manager()), pItem->Get_Manager()->Get_Name().wx_str(), SG_T(""));
	}			

	pNode	= pParameters->Add_Value(
		pNode, CSG_String::Format(SG_T("%d")     , (long)pObject),
		pItem->Get_Name().wx_str(), SG_T(""), PARAMETER_TYPE_Bool, false
	);

	pParameters->Add_FilePath(
		pNode, CSG_String::Format(SG_T("%d FILE"), (long)pObject),
		_TL("File"), SG_T(""), Filter, Path.GetFullPath(), true
	);

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::_Modified_Save(CSG_Parameters *pParameters)
{
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		long long		Pointer;
		CSG_Data_Object	*pObject;
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		if(	pParameter->Get_Type() == PARAMETER_TYPE_Bool && pParameter->asBool()
		&&  SG_SSCANF(pParameter->Get_Identifier(), SG_T("%lld"), (&Pointer)) == 1
		&&  SG_Get_Data_Manager().Exists(pObject = (CSG_Data_Object *)Pointer) )
		{
			pParameter	= pParameters->Get_Parameter(CSG_String::Format(SG_T("%d FILE"), (long)pObject));

			if(	pParameter && pParameter->asString() && pParameter->asString()[0] )
			{
				pObject->Save(pParameter->asString());
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Save_Modified_Sel(void)
{
	return( Save_Modified(this, true) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Close(bool bSilent)
{
	if( Get_Count() == 0 )
	{
		m_pProject->Clr_File_Name();

		return( true );
	}

	if( (bSilent || DLG_Message_Confirm(_TL("Close all data sets"), _TL("Close"))) && Save_Modified(this) )
	{
		m_pProject->Clr_File_Name();

		g_pACTIVE->Get_Parameters()->Restore_Parameters();
		g_pMaps->Close(true);

		return( g_pData_Ctrl->Close(true) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_MANAGER(pManager, Class)	if( !pManager && bAdd ) Add_Item(pManager = new Class); return( pManager );

CWKSP_Base_Manager * CWKSP_Data_Manager::Get_Manager(TSG_Data_Object_Type Type, bool bAdd)
{
	switch( Type )
	{
	default:	return( NULL );

	case DATAOBJECT_TYPE_Table:      GET_MANAGER(m_pTables     , CWKSP_Table_Manager);
	case DATAOBJECT_TYPE_TIN:        GET_MANAGER(m_pTINs       , CWKSP_TIN_Manager);
	case DATAOBJECT_TYPE_PointCloud: GET_MANAGER(m_pPointClouds, CWKSP_PointCloud_Manager);
	case DATAOBJECT_TYPE_Shapes:     GET_MANAGER(m_pShapes     , CWKSP_Shapes_Manager);
	case DATAOBJECT_TYPE_Grid:       GET_MANAGER(m_pGrids      , CWKSP_Grid_Manager);
	}
}

//---------------------------------------------------------
CWKSP_Data_Item * CWKSP_Data_Manager::Get(CSG_Data_Object *pObject)
{
	if( pObject && pObject != DATAOBJECT_CREATE && Get_Manager(pObject->Get_ObjectType()) )
	{
		switch( pObject->Get_ObjectType() )
		{
		default:	return( NULL );

		case DATAOBJECT_TYPE_Table:      return( (CWKSP_Data_Item *)m_pTables     ->Get_Data((CSG_Table      *)pObject) );
		case DATAOBJECT_TYPE_TIN:        return( (CWKSP_Data_Item *)m_pTINs       ->Get_Data((CSG_TIN        *)pObject) );
		case DATAOBJECT_TYPE_PointCloud: return( (CWKSP_Data_Item *)m_pPointClouds->Get_Data((CSG_PointCloud *)pObject) );
		case DATAOBJECT_TYPE_Shapes:     return( (CWKSP_Data_Item *)m_pShapes     ->Get_Data((CSG_Shapes     *)pObject) );
		case DATAOBJECT_TYPE_Grid:       return( (CWKSP_Data_Item *)m_pGrids      ->Get_Data((CSG_Grid       *)pObject) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Data_Item * CWKSP_Data_Manager::Add(CSG_Data_Object *pObject)
{
	if( SG_Get_Data_Manager().Add(pObject) && Get_Manager(pObject->Get_ObjectType(), true) )
	{
		switch( pObject->Get_ObjectType() )
		{
		default:	return( NULL );

		case DATAOBJECT_TYPE_Table:      return( (CWKSP_Data_Item *)m_pTables     ->Add_Data((CSG_Table      *)pObject) );
		case DATAOBJECT_TYPE_TIN:        return( (CWKSP_Data_Item *)m_pTINs       ->Add_Data((CSG_TIN        *)pObject) );
		case DATAOBJECT_TYPE_PointCloud: return( (CWKSP_Data_Item *)m_pPointClouds->Add_Data((CSG_PointCloud *)pObject) );
		case DATAOBJECT_TYPE_Shapes:     return( (CWKSP_Data_Item *)m_pShapes     ->Add_Data((CSG_Shapes     *)pObject) );
		case DATAOBJECT_TYPE_Grid:       return( (CWKSP_Data_Item *)m_pGrids      ->Add_Data((CSG_Grid       *)pObject) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Layer * CWKSP_Data_Manager::Get_Layer(CSG_Data_Object *pObject)
{
	if( pObject && pObject != DATAOBJECT_CREATE && Get_Manager(pObject->Get_ObjectType()) )
	{
		switch( pObject->Get_ObjectType() )
		{
		default:	return( NULL );

		case DATAOBJECT_TYPE_TIN:        return( (CWKSP_Layer *)m_pTINs       ->Get_Data((CSG_TIN        *)pObject) );
		case DATAOBJECT_TYPE_PointCloud: return( (CWKSP_Layer *)m_pPointClouds->Get_Data((CSG_PointCloud *)pObject) );
		case DATAOBJECT_TYPE_Shapes:     return( (CWKSP_Layer *)m_pShapes     ->Get_Data((CSG_Shapes     *)pObject) );
		case DATAOBJECT_TYPE_Grid:       return( (CWKSP_Layer *)m_pGrids      ->Get_Data((CSG_Grid       *)pObject) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
void CWKSP_Data_Manager::Del_Manager(CWKSP_Base_Item *pItem)
{
	if( pItem == m_pTables      )	m_pTables		= NULL;
	if( pItem == m_pTINs        )	m_pTINs			= NULL;
	if( pItem == m_pPointClouds )	m_pPointClouds	= NULL;
	if( pItem == m_pShapes      )	m_pShapes		= NULL;
	if( pItem == m_pGrids       )	m_pGrids		= NULL;

	if( Get_Count() == 0 )
	{
		m_pProject->Clr_File_Name();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Update(CSG_Data_Object *pObject, CSG_Parameters *pParameters)
{
	CWKSP_Data_Item	*pItem	= Get(pObject);

	return( pItem && pItem->DataObject_Changed(pParameters) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Update_Views(CSG_Data_Object *pObject)
{
	CWKSP_Data_Item	*pItem	= Get(pObject);

	return( pItem && pItem->Update_Views() );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Show(CSG_Data_Object *pObject, int Flags)
{
	CWKSP_Data_Item	*pItem	= Get(pObject);

	return( pItem && pItem->Show(Flags) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::asImage(CSG_Data_Object *pObject, CSG_Grid *pImage)
{
	CWKSP_Layer	*pLayer	= Get_Layer(pObject);

	return( pLayer && pLayer->asImage(pImage) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Get_Colors(CSG_Data_Object *pObject, CSG_Colors *pColors)
{
	CWKSP_Layer	*pLayer	= Get_Layer(pObject);

	return( pLayer && pLayer->Get_Colors(pColors) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Set_Colors(CSG_Data_Object *pObject, CSG_Colors *pColors)
{
	CWKSP_Layer	*pLayer	= Get_Layer(pObject);

	return( pLayer && pLayer->Set_Colors(pColors) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Get_Parameters(CSG_Data_Object *pObject, CSG_Parameters *pParameters)
{
	CWKSP_Data_Item	*pItem	= pParameters ? Get(pObject) : NULL;

	return( pItem && pParameters->Assign(pItem->Get_Parameters()) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Set_Parameters(CSG_Data_Object *pObject, CSG_Parameters *pParameters)
{
	CWKSP_Data_Item	*pItem	= pParameters ? Get(pObject) : NULL;

	if( pItem && pItem->Get_Parameters()->Assign_Values(pParameters) )
	{
		pItem->Parameters_Changed();

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
bool CWKSP_Data_Manager::MultiSelect_Check(void)
{
	wxArrayTreeItemIds	IDs;

	if( Get_Control()->GetSelections(IDs) > 1 )
	{
		TWKSP_Item	Type	= WKSP_ITEM_Undefined;

		for(size_t iID=0; iID<IDs.Count(); iID++)
		{
			CWKSP_Base_Item	*pItem	= (CWKSP_Base_Item *)Get_Control()->GetItemData(IDs[iID]);

			switch( pItem->Get_Type() )
			{
			default                  :	pItem	= NULL;	break;
			case WKSP_ITEM_Table     :
			case WKSP_ITEM_Shapes    :
			case WKSP_ITEM_TIN       :
			case WKSP_ITEM_PointCloud:
			case WKSP_ITEM_Grid      :	break;
			}

			//---------------------------------------------
			if( pItem )
			{
				if( Type == WKSP_ITEM_Undefined )
				{
					Type	= pItem->Get_Type();

					m_Sel_Parms[0].Assign(pItem->Get_Parameters());

					m_Sel_Items.Clear();

					m_Sel_Items.Add(IDs[iID]);
				}
				else if( Type == pItem->Get_Type() )
				{
					m_Sel_Items.Add(IDs[iID]);

					for(int i=m_Sel_Parms[0].Get_Count()-1; i>=0; i--)
					{
						if( !m_Sel_Parms[0][i].is_Compatible(pItem->Get_Parameters()->Get_Parameter(i)) )
						{
							m_Sel_Parms[0].Del_Parameter(i);
						}
						else if( !m_Sel_Parms[0][i].is_Value_Equal(pItem->Get_Parameters()->Get_Parameter(i)) )
						{
							if( m_Sel_Parms[0][i].is_DataObject() )
							{
								m_Sel_Parms[0][i].Set_Value(DATAOBJECT_NOTSET);
							}
							else switch( m_Sel_Parms[0][i].Get_Type() )
							{
							default:
								m_Sel_Parms[0][i].Restore_Default();
								break;

							case PARAMETER_TYPE_String:
							case PARAMETER_TYPE_Text:
							case PARAMETER_TYPE_FilePath:
								m_Sel_Parms[0][i].Set_Value(CSG_String(""));
								break;
							}
						}
					}
				}
				else //  Type != pItem->Get_Type()
				{
					IDs.Clear();
				}
			}
		}

		//-------------------------------------------------
		if( IDs.Count() > 0 )
		{
			m_Sel_Parms[1]	= m_Sel_Parms[0];

			if( g_pACTIVE->Get_Active() == this )
			{
				g_pACTIVE->Get_Parameters()->Update_Parameters(&m_Sel_Parms[0], false);
			//	g_pACTIVE->Get_Parameters()->Set_Parameters(this);
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	m_Sel_Items.Clear();

	m_Sel_Parms[0].Destroy();
	m_Sel_Parms[1].Destroy();

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::MultiSelect_Update(void)
{
	if( m_Sel_Items.Count() == 0 )
	{
		return( false );
	}

	for(int i=m_Sel_Parms[0].Get_Count()-1; i>=0; i--)	// remove unchanged parameters from backup list (:= 1)
	{
		if( m_Sel_Parms[0][i].is_Value_Equal(&m_Sel_Parms[1][i]) )
		{
			m_Sel_Parms[0].Del_Parameter(i);
		}
	}

	for(size_t iID=0; iID<m_Sel_Items.Count(); iID++)	// assign changed values to selected data set parameters
	{
		CWKSP_Data_Item	*pItem	= (CWKSP_Data_Item *)Get_Control()->GetItemData(m_Sel_Items[iID]);

		pItem->Get_Parameters()->Assign_Values(&m_Sel_Parms[0]);

		pItem->Parameters_Changed();
	}

	m_Sel_Parms[1].Assign_Values(&m_Sel_Parms[0]);	// restore from backup list with updated values
	m_Sel_Parms[0].Assign       (&m_Sel_Parms[1]);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
