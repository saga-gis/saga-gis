
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
// $Id: wksp_data_manager.cpp,v 1.16 2007-02-26 17:48:37 oconrad Exp $

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

#include "wksp_table_manager.h"
#include "wksp_shapes_manager.h"
#include "wksp_tin_manager.h"
#include "wksp_grid_manager.h"

#include "wksp_map_manager.h"

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
	m_pTables		= NULL;
	m_pShapes		= NULL;
	m_pTINs			= NULL;
	m_pGrids		= NULL;

	m_pProject		= new CWKSP_Project;
	m_pMenu_Files	= new CWKSP_Data_Menu_Files;

	g_pData			= this;

	//-----------------------------------------------------
	bool		bValue;
	long		lValue;
	wxString	sValue;

	if( CONFIG_Read(wxT("/DATA/GRIDS"), wxT("CACHE_TMP_DIR")	, sValue) )
	{
		SG_Grid_Cache_Set_Directory(sValue);
	}

	if( CONFIG_Read(wxT("/DATA/GRIDS"), wxT("CACHE_AUTO")		, bValue) )
	{
		SG_Grid_Cache_Set_Automatic(bValue);
	}

	if( CONFIG_Read(wxT("/DATA/GRIDS"), wxT("CACHE_THRESHOLD")	, lValue) )
	{
		SG_Grid_Cache_Set_Threshold(lValue);
	}

	if( CONFIG_Read(wxT("/DATA/GRIDS"), wxT("CACHE_CONFIRM")	, lValue) )
	{
		SG_Grid_Cache_Set_Confirm  (lValue);
	}

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	m_Parameters.Create(this, LNG(""), LNG(""));

	pNode	= m_Parameters.Add_Node(NULL, "NODE_GRID_MEM", LNG("Grid File Caching"), LNG(""));

	m_Parameters.Add_FilePath(
		pNode	, "GRID_MEM_CACHE_TMPDIR"	, LNG("Temporary files"),
		LNG("Directory, where temporary cache files shall be saved."),
		NULL, SG_Grid_Cache_Get_Directory(), true, true
	);

	m_Parameters.Add_Value(
		pNode	, "GRID_MEM_CACHE_AUTO"		, LNG("Automatic mode"),
		LNG("Activate file caching automatically, if memory size exceeds the threshold value."),
		PARAMETER_TYPE_Bool, SG_Grid_Cache_Get_Automatic()
	);

	m_Parameters.Add_Value(
		pNode	, "GRID_MEM_CACHE_THRSHLD"	, LNG("Threshold for automatic mode [MB]"),
		LNG(""),
		PARAMETER_TYPE_Double, SG_Grid_Cache_Get_Threshold_MB(), 0.0, true
	);

	m_Parameters.Add_Choice(
		pNode	, "GRID_MEM_CACHE_CONFIRM"	, LNG("Confirm file caching"),
		LNG(""),
		wxString::Format(wxT("%s|%s|%s|"),
			LNG("do not confirm"),
			LNG("confirm"),
			LNG("confirm with options")
		),
		SG_Grid_Cache_Get_Confirm()
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(NULL, "NODE_GENERAL", LNG("General"), LNG(""));

	if( CONFIG_Read(wxT("/DATA"), wxT("PROJECT_START")			, lValue) == false )
	{
		lValue	= 2;
	}

	m_Parameters.Add_Choice(
		pNode	, "PROJECT_START"			, LNG("Start Project"),
		LNG(""),
		wxString::Format(wxT("%s|%s|%s|"),
			LNG("empty"),
			LNG("last opened"),
			LNG("automatically save and load")
		), lValue
	);

	if( CONFIG_Read(wxT("/DATA"), wxT("START_LOGO")				, lValue) == false )
	{
		lValue	= 1;
	}

	m_Parameters.Add_Choice(
		pNode	, "START_LOGO"				, LNG("Show Logo at Start Up"),
		LNG(""),

		CSG_String::Format(wxT("%s|%s|%s|%s|"),
			LNG("do not show"),
			LNG("only during start up phase"),
			LNG("20 seconds"),
			LNG("until user closes it")
		), lValue
	);
}

//---------------------------------------------------------
CWKSP_Data_Manager::~CWKSP_Data_Manager(void)
{
	g_pData	= NULL;

	delete(m_pProject);
	delete(m_pMenu_Files);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Initialise(void)
{
	wxString	FileName;

	if( m_pProject->Has_File_Name() )
	{
		return( m_pProject->Load(false) );
	}
	else
	{
		return( CONFIG_Read(wxT("/DATA"), wxT("PROJECT_FILE"), FileName) && wxFileExists(FileName) && m_pProject->Load(FileName, false, false) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Finalise(void)
{
	//-----------------------------------------------------
	CONFIG_Write(wxT("/DATA/GRIDS")	, wxT("CACHE_TMP_DIR")	,		SG_Grid_Cache_Get_Directory());
	CONFIG_Write(wxT("/DATA/GRIDS")	, wxT("CACHE_AUTO")		,		SG_Grid_Cache_Get_Automatic());
	CONFIG_Write(wxT("/DATA/GRIDS")	, wxT("CACHE_THRESHOLD"), (long)SG_Grid_Cache_Get_Threshold());
	CONFIG_Write(wxT("/DATA/GRIDS")	, wxT("CACHE_CONFIRM")	, (long)SG_Grid_Cache_Get_Confirm  ());

	//-----------------------------------------------------
#ifdef _SAGA_LINUX
	wxFileName	fProject( wxString( getenv( "HOME" ), wxConvFile ), wxT( "saga_gui" ), wxT( "cfg" ) );
#else
	wxFileName	fProject(g_pSAGA->Get_App_Path(), wxT("saga_gui"), wxT("cfg"));
#endif

	CONFIG_Write(wxT("/DATA")		, wxT("PROJECT_START")	, (long)m_Parameters("PROJECT_START")	->asInt());
	CONFIG_Write(wxT("/DATA")		, wxT("START_LOGO")		, (long)m_Parameters("START_LOGO")		->asInt());

	if( Get_Count() == 0 )
	{
		wxRemoveFile(fProject.GetFullPath());
		CONFIG_Write(wxT("/DATA"), wxT("PROJECT_FILE"), LNG(""));
	}
	else switch( m_Parameters("PROJECT_START")->asInt() )
	{
	case 0:	// empty
		wxRemoveFile(fProject.GetFullPath());
		CONFIG_Write(wxT("/DATA"), wxT("PROJECT_FILE"), LNG(""));
		break;

	case 1:	// last opened
		wxRemoveFile(fProject.GetFullPath());
		CONFIG_Write(wxT("/DATA"), wxT("PROJECT_FILE"), m_pProject->Get_File_Name());
		break;

	case 2:	// automatically save and load		
		m_pProject->Save(fProject.GetFullPath(), false);
		CONFIG_Write(wxT("/DATA"), wxT("PROJECT_FILE"), m_pProject->Get_File_Name());
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
wxString CWKSP_Data_Manager::Get_Name(void)
{
	return( LNG("[CAP] Data") );
}

//---------------------------------------------------------
wxString CWKSP_Data_Manager::Get_Description(void)
{
	wxString	s;

	s.Printf(wxT("<b>%s</b><br>"), LNG("[CAP] Data"));

	if( Get_Count() <= 0 )
	{
		s.Append(LNG("[TXT] No data loaded."));
	}
	else
	{
		if( m_pProject->Has_File_Name() )
		{
			s.Append(wxString::Format(wxT("%s: %s<br>"), LNG("[CAP] Project File"), m_pProject->Get_File_Name()));
		}

		if( Get_Tables() )
		{
			s.Append(wxString::Format(wxT("%s: %d<br>"), LNG("[CAP] Tables"), Get_Tables()->Get_Count()));
		}

		if( Get_Shapes() )
		{
			s.Append(wxString::Format(wxT("%s: %d<br>"), LNG("[CAP] Shapes"), Get_Shapes()->Get_Items_Count()));
		}

		if( Get_TINs() )
		{
			s.Append(wxString::Format(wxT("%s: %d<br>"), LNG("[CAP] TIN"), Get_TINs()->Get_Count()));
		}

		if( Get_Grids() )
		{
			s.Append(wxString::Format(wxT("%s: %d<br>"), LNG("[CAP] Grid Systems"), Get_Grids()->Get_Count()));
			s.Append(wxString::Format(wxT("%s: %d<br>"), LNG("[CAP] Grids"), Get_Grids()->Get_Items_Count()));
		}
	}

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Data_Manager::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(LNG("[CAP] Data"));

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);

	pMenu->AppendSeparator();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_OPEN);
//	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_OPEN_ADD);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECT_SAVE_AS);

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
	case ID_CMD_DATA_PROJECT_OPEN:		m_pProject->Load(false);		break;
	case ID_CMD_DATA_PROJECT_OPEN_ADD:	m_pProject->Load(true);			break;
	case ID_CMD_DATA_PROJECT_SAVE:		m_pProject->Save(true);			break;
	case ID_CMD_DATA_PROJECT_SAVE_AS:	m_pProject->Save();				break;

	//-----------------------------------------------------
	case ID_CMD_TABLES_OPEN:			Open(DATAOBJECT_TYPE_Table);	break;
	case ID_CMD_SHAPES_OPEN:			Open(DATAOBJECT_TYPE_Shapes);	break;
	case ID_CMD_TIN_OPEN:				Open(DATAOBJECT_TYPE_TIN);		break;
	case ID_CMD_GRIDS_OPEN:				Open(DATAOBJECT_TYPE_Grid);		break;

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

	case ID_CMD_WKSP_ITEM_CLOSE:
		event.Enable(Get_Count() > 0);
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
void CWKSP_Data_Manager::Parameters_Changed(void)
{
	SG_Grid_Cache_Set_Automatic		(m_Parameters("GRID_MEM_CACHE_AUTO")	->asBool());
	SG_Grid_Cache_Set_Threshold_MB	(m_Parameters("GRID_MEM_CACHE_THRSHLD")	->asDouble());
	SG_Grid_Cache_Set_Confirm		(m_Parameters("GRID_MEM_CACHE_CONFIRM")	->asInt());
	SG_Grid_Cache_Set_Directory		(m_Parameters("GRID_MEM_CACHE_TMPDIR")	->asString());

	CWKSP_Base_Manager::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Check_Parameters(CSG_Parameters *pParameters)
{
	if( pParameters )
	{
		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			Check_Parameter(pParameters->Get_Parameter(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Check_Parameter(CSG_Parameter *pParameter)
{
	bool	bResult	= true;
	int		i, DataObject_Type;

	if( pParameter )
	{
		switch( pParameter->Get_Type() )
		{
		default:							DataObject_Type	= -1;						break;
		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Grid_List:		DataObject_Type	= DATAOBJECT_TYPE_Grid;		break;
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Table_List:		DataObject_Type	= DATAOBJECT_TYPE_Table;	break;
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_Shapes_List:	DataObject_Type	= DATAOBJECT_TYPE_Shapes;	break;
		case PARAMETER_TYPE_TIN:
		case PARAMETER_TYPE_TIN_List:		DataObject_Type	= DATAOBJECT_TYPE_TIN;		break;
		}

		//-------------------------------------------------
		switch( pParameter->Get_Type() )
		{
		default:
	    	break;

		case PARAMETER_TYPE_Grid_System:
			if( !Exists(pParameter->asGrid_System()) )
			{
				bResult	= false;
				pParameter->Set_Value((void *)NULL);
			}
			break;

		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
			if(	pParameter->asDataObject() != DATAOBJECT_NOTSET
			&&	pParameter->asDataObject() != DATAOBJECT_CREATE
			&&	!Exists(pParameter->asDataObject(), DataObject_Type) )
			{
				bResult	= false;
				pParameter->Set_Value(DATAOBJECT_NOTSET);
			}
			break;

		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
			for(i=pParameter->asList()->Get_Count()-1; i>=0; i--)
			{
				if( !Exists(pParameter->asList()->asDataObject(i), DataObject_Type) )
				{
					bResult	= false;
					pParameter->asList()->Del_Item(i);
				}
			}
			break;
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open_CMD(int Cmd_ID)
{
	return( m_pMenu_Files->Recent_Open(Cmd_ID) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open(const wxChar *File_Name)
{
	if( SG_File_Cmp_Extension(File_Name, wxT("txt"))
	||	SG_File_Cmp_Extension(File_Name, wxT("dbf")) )
	{
		return( Open(DATAOBJECT_TYPE_Table , File_Name) != NULL );
	}

	if( SG_File_Cmp_Extension(File_Name, wxT("shp")) )
	{
		return( Open(DATAOBJECT_TYPE_Shapes, File_Name) != NULL );
	}

	if(	SG_File_Cmp_Extension(File_Name, wxT("sgrd"))
	||	SG_File_Cmp_Extension(File_Name, wxT("dgm"))
	||	SG_File_Cmp_Extension(File_Name, wxT("grd")) )
	{
		return( Open(DATAOBJECT_TYPE_Grid  , File_Name) != NULL );
	}

	if( SG_File_Cmp_Extension(File_Name, wxT("sprj")) )
	{
		return( m_pProject->Load(File_Name, false, true) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Open(int DataType)
{
	int				ID;
	wxArrayString	File_Paths;

	switch( DataType )
	{
	default:	return( false );
	case DATAOBJECT_TYPE_Table:		ID	= ID_DLG_TABLES_OPEN;	break;
	case DATAOBJECT_TYPE_Shapes:	ID	= ID_DLG_SHAPES_OPEN;	break;
	case DATAOBJECT_TYPE_TIN:		ID	= ID_DLG_TIN_OPEN;		break;
	case DATAOBJECT_TYPE_Grid:		ID	= ID_DLG_GRIDS_OPEN;	break;
	}

	if( DLG_Open(File_Paths, ID) )
	{
		MSG_General_Add_Line();

		for(size_t i=0; i<File_Paths.GetCount(); i++)
		{
			Open(DataType, File_Paths[i]);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Data_Manager::Open(int DataType, const wxChar *FileName)
{
	CSG_Data_Object		*pObject;
	CWKSP_Base_Item	*pItem;

	switch( DataType )
	{
	default:
		pObject	= NULL;
		break;

	case DATAOBJECT_TYPE_Table:
		pObject	= new CSG_Table (FileName);
		break;

	case DATAOBJECT_TYPE_Shapes:
		pObject	= new CSG_Shapes(FileName);
		break;

	case DATAOBJECT_TYPE_TIN:
		pObject	= new CSG_TIN   (FileName);
		break;

	case DATAOBJECT_TYPE_Grid:
		pObject	= new CSG_Grid  (FileName);
		break;
	}

	PROCESS_Set_Okay();

	if( pObject )
	{
		if( pObject->is_Valid() && (pItem = Add(pObject)) != NULL )
		{
			m_pMenu_Files->Recent_Add(DataType, FileName);

			return( pItem );
		}

		delete(pObject);
	}

	m_pMenu_Files->Recent_Del(DataType, FileName);

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Exists(CSG_Grid_System *pSystem)
{
	return( m_pGrids && m_pGrids->Exists(pSystem) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Exists(CSG_Data_Object *pObject, int Type)
{
	switch( Type )
	{
	case DATAOBJECT_TYPE_Grid:
		return( m_pGrids  && m_pGrids ->Exists((CSG_Grid   *)pObject) );

	case DATAOBJECT_TYPE_Table:
		return( m_pTables && m_pTables->Exists((CSG_Table  *)pObject) );

	case DATAOBJECT_TYPE_Shapes:
		return( m_pShapes && m_pShapes->Exists((CSG_Shapes *)pObject) );

	case DATAOBJECT_TYPE_TIN:
		return( m_pTINs   && m_pTINs  ->Exists((CSG_TIN    *)pObject) );
	}

	return(	Exists(pObject, DATAOBJECT_TYPE_Table)
		||	Exists(pObject, DATAOBJECT_TYPE_Shapes)
		||	Exists(pObject, DATAOBJECT_TYPE_TIN)
		||	Exists(pObject, DATAOBJECT_TYPE_Grid)
	);
}

//---------------------------------------------------------
CSG_Data_Object * CWKSP_Data_Manager::Get_byFileName(const wxChar *File_Name, int Type)
{
	switch( Type )
	{
	case DATAOBJECT_TYPE_Grid:
		return( !m_pGrids  ? NULL : m_pGrids ->Get_byFileName(File_Name) );

	case DATAOBJECT_TYPE_Table:
		return( !m_pTables ? NULL : m_pTables->Get_byFileName(File_Name) );

	case DATAOBJECT_TYPE_Shapes:
		return( !m_pShapes ? NULL : m_pShapes->Get_byFileName(File_Name) );

	case DATAOBJECT_TYPE_TIN:
		return( !m_pTINs   ? NULL : m_pTINs  ->Get_byFileName(File_Name) );
	}

	CSG_Data_Object	*pObject;

	if( (pObject = Get_byFileName(File_Name, DATAOBJECT_TYPE_Table))  != NULL )	return( pObject );
	if( (pObject = Get_byFileName(File_Name, DATAOBJECT_TYPE_Shapes)) != NULL )	return( pObject );
	if( (pObject = Get_byFileName(File_Name, DATAOBJECT_TYPE_TIN))    != NULL )	return( pObject );
	if( (pObject = Get_byFileName(File_Name, DATAOBJECT_TYPE_Grid))   != NULL )	return( pObject );

	return(	NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Save_Modified(CWKSP_Base_Item *pItem)
{
	return( m_pProject->Save_Modified(pItem) );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Close(bool bSilent)
{
	if( Get_Count() == 0 )
	{
		Finalise();

		return( true );
	}
	else if( (bSilent || DLG_Message_Confirm(LNG("[TXT] Close all data sets"), LNG("[CAP] Close"))) && Save_Modified(this) )
	{
		Finalise();

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
CWKSP_Base_Item * CWKSP_Data_Manager::Add(CSG_Data_Object *pObject)
{
	if( pObject && _Get_Manager(pObject->Get_ObjectType()) )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( (CWKSP_Base_Item *)m_pGrids ->Add((CSG_Grid   *)pObject) );

		case DATAOBJECT_TYPE_Table:
			return( (CWKSP_Base_Item *)m_pTables->Add((CSG_Table  *)pObject) );

		case DATAOBJECT_TYPE_Shapes:
			return( (CWKSP_Base_Item *)m_pShapes->Add((CSG_Shapes *)pObject) );

		case DATAOBJECT_TYPE_TIN:
			return( (CWKSP_Base_Item *)m_pTINs  ->Add((CSG_TIN    *)pObject) );
			
		default:
			return( NULL );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
void CWKSP_Data_Manager::Del_Manager(CWKSP_Base_Item *pItem)
{
	if( pItem == m_pGrids )
	{
		m_pGrids	= NULL;
	}
	else if( pItem == m_pTables )
	{
		m_pTables	= NULL;
	}
	else if( pItem == m_pShapes )
	{
		m_pShapes	= NULL;
	}
	else if( pItem == m_pTINs )
	{
		m_pTINs		= NULL;
	}
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::_Get_Manager(int DataType)
{
	switch( DataType )
	{
	default:
		return( false );

	case DATAOBJECT_TYPE_Grid:
		if( !m_pGrids )
		{
			Add_Item(m_pGrids   = new CWKSP_Grid_Manager);
		}

		return( m_pGrids  != NULL );

	case DATAOBJECT_TYPE_Table:
		if( !m_pTables )
		{
			Add_Item(m_pTables  = new CWKSP_Table_Manager);
		}

		return( m_pTables != NULL );

	case DATAOBJECT_TYPE_Shapes:
		if( !m_pShapes )
		{
			Add_Item(m_pShapes = new CWKSP_Shapes_Manager);
		}

		return( m_pShapes != NULL );

	case DATAOBJECT_TYPE_TIN:
		if( !m_pTINs )
		{
			Add_Item(m_pTINs   = new CWKSP_TIN_Manager);
		}

		return( m_pTINs   != NULL );
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
	if( pObject )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( m_pGrids  && m_pGrids ->Update((CSG_Grid   *)pObject, pParameters) );

		case DATAOBJECT_TYPE_Table:
			return( m_pTables && m_pTables->Update((CSG_Table  *)pObject, pParameters) );

		case DATAOBJECT_TYPE_Shapes:
			return( m_pShapes && m_pShapes->Update((CSG_Shapes *)pObject, pParameters) );

		case DATAOBJECT_TYPE_TIN:
			return( m_pTINs   && m_pTINs  ->Update((CSG_TIN    *)pObject, pParameters) );

		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Update_Views(CSG_Data_Object *pObject)
{
	if( pObject )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( m_pGrids  && m_pGrids ->Update_Views((CSG_Grid   *)pObject) );

		case DATAOBJECT_TYPE_Table:
			return( m_pTables && m_pTables->Update_Views((CSG_Table  *)pObject) );

		case DATAOBJECT_TYPE_Shapes:
			return( m_pShapes && m_pShapes->Update_Views((CSG_Shapes *)pObject) );

		case DATAOBJECT_TYPE_TIN:
			return( m_pTINs   && m_pTINs  ->Update_Views((CSG_TIN    *)pObject) );

		default:
			break;
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Data_Manager::Show(CSG_Data_Object *pObject)
{
	if( pObject )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( m_pGrids  && m_pGrids ->Show((CSG_Grid   *)pObject) );

		case DATAOBJECT_TYPE_Table:
			return( m_pTables && m_pTables->Show((CSG_Table  *)pObject) );

		case DATAOBJECT_TYPE_Shapes:
			return( m_pShapes && m_pShapes->Show((CSG_Shapes *)pObject) );

		case DATAOBJECT_TYPE_TIN:
			return( m_pTINs   && m_pTINs  ->Show((CSG_TIN    *)pObject) );

		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::asImage(CSG_Data_Object *pObject, CSG_Grid *pImage)
{
	if( pObject )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( m_pGrids  && m_pGrids ->asImage((CSG_Grid   *)pObject, pImage) );

		case DATAOBJECT_TYPE_Shapes:
			return( m_pShapes && m_pShapes->asImage((CSG_Shapes *)pObject, pImage) );

		case DATAOBJECT_TYPE_TIN:
			return( m_pTINs   && m_pTINs  ->asImage((CSG_TIN    *)pObject, pImage) );

		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Get_Colors(CSG_Data_Object *pObject, CSG_Colors *pColors)
{
	if( pObject && pColors )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( m_pGrids ->Get_Colors((CSG_Grid   *)pObject, pColors) );

		case DATAOBJECT_TYPE_Shapes:
			return( m_pShapes->Get_Colors((CSG_Shapes *)pObject, pColors) );

		case DATAOBJECT_TYPE_TIN:
			return( m_pTINs  ->Get_Colors((CSG_TIN    *)pObject, pColors) );

		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Set_Colors(CSG_Data_Object *pObject, CSG_Colors *pColors)
{
	if( pObject && pColors )
	{
		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( m_pGrids ->Set_Colors((CSG_Grid   *)pObject, pColors) );

		case DATAOBJECT_TYPE_Shapes:
			return( m_pShapes->Set_Colors((CSG_Shapes *)pObject, pColors) );

		case DATAOBJECT_TYPE_TIN:
			return( m_pTINs  ->Set_Colors((CSG_TIN    *)pObject, pColors) );

		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Get_Parameters(CSG_Data_Object *pObject, CSG_Parameters *pParameters)
{
	if( pObject && pParameters )
	{
		CWKSP_Base_Item	*pItem;

		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			pItem	= (CWKSP_Base_Item *)m_pGrids ->Get_Grid  ((CSG_Grid   *)pObject);
			break;

		case DATAOBJECT_TYPE_Shapes:
			pItem	= (CWKSP_Base_Item *)m_pShapes->Get_Shapes((CSG_Shapes *)pObject);
			break;

		case DATAOBJECT_TYPE_TIN:
			pItem	= (CWKSP_Base_Item *)m_pTINs  ->Get_TIN   ((CSG_TIN    *)pObject);
			break;

		case DATAOBJECT_TYPE_Table:
			pItem	= (CWKSP_Base_Item *)m_pTables->Get_Table ((CSG_Table  *)pObject);
			break;

		default:
			pItem	= NULL;
			break;
		}

		if( pItem && pItem->Get_Parameters() )
		{
			return( pParameters->Assign(pItem->Get_Parameters()) != 0 );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Data_Manager::Set_Parameters(CSG_Data_Object *pObject, CSG_Parameters *pParameters)
{
	if( pObject && pParameters )
	{
		CWKSP_Base_Item	*pItem;

		switch( pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			pItem	= (CWKSP_Base_Item *)m_pGrids ->Get_Grid  ((CSG_Grid   *)pObject);
			break;

		case DATAOBJECT_TYPE_Shapes:
			pItem	= (CWKSP_Base_Item *)m_pShapes->Get_Shapes((CSG_Shapes *)pObject);
			break;

		case DATAOBJECT_TYPE_TIN:
			pItem	= (CWKSP_Base_Item *)m_pTINs  ->Get_TIN   ((CSG_TIN    *)pObject);
			break;

		case DATAOBJECT_TYPE_Table:
			pItem	= (CWKSP_Base_Item *)m_pTables->Get_Table ((CSG_Table  *)pObject);
			break;

		default:
			pItem	= NULL;
			break;
		}

		if( pItem && pItem->Get_Parameters() )
		{
			if( pItem->Get_Parameters()->Assign_Values(pParameters) != 0 )
			{
				pItem->Parameters_Changed();
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_grid_system.h"
#include "wksp_grid.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"
#include "wksp_table.h"

//---------------------------------------------------------
bool CWKSP_Data_Manager::Get_DataObject_List(CSG_Parameters *pParameters)
{
	if( pParameters )
	{
		int					i, j;
		CSG_String			s;
		CWKSP_Shapes_Type	*pShapes;

		pParameters->Destroy();

		for(i=0; i<Get_Grids()->Get_Count(); i++)
		{
			for(j=0; j<Get_Grids()->Get_System(i)->Get_Count(); j++)
			{
				s.Printf(wxT("GRID_%03_%03"), i, j);
				pParameters->Add_Grid(NULL, s, s, LNG(""), PARAMETER_INPUT)
					->Set_Value(Get_Grids()->Get_System(i)->Get_Grid(j)->Get_Grid());
			}
		}

		if( (pShapes = Get_Shapes()->Get_Shapes_Type(SHAPE_TYPE_Point)) != NULL )
		{
			for(i=0; i<pShapes->Get_Count(); i++)
			{
				s.Printf(wxT("POINT_%03"), i);
				pParameters->Add_Shapes(NULL, s, s, LNG(""), PARAMETER_INPUT)
					->Set_Value(pShapes->Get_Shapes(i)->Get_Shapes());
			}
		}

		if( (pShapes = Get_Shapes()->Get_Shapes_Type(SHAPE_TYPE_Points)) != NULL )
		{
			for(i=0; i<pShapes->Get_Count(); i++)
			{
				s.Printf(wxT("POINTS_%03"), i);
				pParameters->Add_Shapes(NULL, s, s, LNG(""), PARAMETER_INPUT)
					->Set_Value(pShapes->Get_Shapes(i)->Get_Shapes());
			}
		}

		if( (pShapes = Get_Shapes()->Get_Shapes_Type(SHAPE_TYPE_Line)) != NULL )
		{
			for(i=0; i<pShapes->Get_Count(); i++)
			{
				s.Printf(wxT("LINE_%03"), i);
				pParameters->Add_Shapes(NULL, s, s, LNG(""), PARAMETER_INPUT)
					->Set_Value(pShapes->Get_Shapes(i)->Get_Shapes());
			}
		}

		if( (pShapes = Get_Shapes()->Get_Shapes_Type(SHAPE_TYPE_Polygon)) != NULL )
		{
			for(i=0; i<pShapes->Get_Count(); i++)
			{
				s.Printf(wxT("POLYGON_%03"), i);
				pParameters->Add_Shapes(NULL, s, s, LNG(""), PARAMETER_INPUT)
					->Set_Value(pShapes->Get_Shapes(i)->Get_Shapes());
			}
		}

		for(i=0; i<Get_Tables()->Get_Count(); i++)
		{
			s.Printf(wxT("TABLE_%03"), i);
			pParameters->Add_Table(NULL, s, s, LNG(""), PARAMETER_INPUT)
				->Set_Value(Get_Tables()->Get_Table(i)->Get_Table());
		}

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
