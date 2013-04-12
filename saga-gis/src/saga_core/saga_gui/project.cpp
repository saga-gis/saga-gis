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
//                     Project.cpp                       //
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
#include <wx/string.h>
#include <wx/filename.h>

#include <saga_api/saga_api.h>

#include "helper.h"

#include "res_dialogs.h"

#include "saga_frame.h"

#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"

#include "wksp_pointcloud_manager.h"
#include "wksp_pointcloud.h"

#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"
#include "wksp_layer.h"

#include "project.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Project::CWKSP_Project(void)
{
}

//---------------------------------------------------------
CWKSP_Project::~CWKSP_Project(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::Has_File_Name(void)
{
	return( wxFileExists(m_File_Name) );
}

//---------------------------------------------------------
bool CWKSP_Project::Clr_File_Name(void)
{
	m_File_Name.Clear();

	g_pSAGA_Frame->Set_Project_Name();

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Project::_Set_Project_Name(void)
{
	if( Has_File_Name() )
	{
		wxFileName	fn(m_File_Name);

		if( fn.GetFullName().CmpNoCase(wxT("saga_gui.cfg")) )
		{
			g_pSAGA_Frame->Set_Project_Name(m_File_Name);

			return( true );
		}

		Clr_File_Name();
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::Load(bool bAdd)
{
	wxString	FileName;

	return( DLG_Open(FileName, ID_DLG_PROJECT_OPEN) && _Load(FileName, bAdd, true) );
}

//---------------------------------------------------------
bool CWKSP_Project::Load(const wxString &FileName, bool bAdd, bool bUpdateMenu)
{
	return( _Load(FileName, bAdd, bUpdateMenu) );
}

//---------------------------------------------------------
bool CWKSP_Project::Save(void)
{
	wxString	FileName;

	return( DLG_Save(FileName, ID_DLG_PROJECT_SAVE) && _Save(FileName, true, true) );
}

//---------------------------------------------------------
bool CWKSP_Project::Save(bool bSaveAsOnError)
{
	if( Has_File_Name() )
	{
		return( _Save(m_File_Name, true, true) );
	}

	return( bSaveAsOnError ? Save() : false );
}

//---------------------------------------------------------
bool CWKSP_Project::Save(const wxString &FileName, bool bSaveModified)
{
	return( _Save(FileName, bSaveModified, false) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::_Load(const wxString &FileName, bool bAdd, bool bUpdateMenu)
{
	if( !bAdd && g_pData->Get_Count() > 0 )
	{
		switch( DLG_Message_YesNoCancel(_TL("Close all data sets"), _TL("Load Project")) )
		{
		case 0:
			if( !g_pData->Close(true) )
				return( false );
			break;

		case 2:
			return( false );
		}
	}

	//-------------------------------------------------
	MSG_General_Add_Line();
	MSG_General_Add(wxString::Format(wxT("%s: %s"), _TL("Load project"), FileName), true, true);

	//-------------------------------------------------
	bool			bSuccess	= false;

	CSG_MetaData	Project, *pNode;

	if( _Compatibility_Load_Data(FileName) )
	{
		bSuccess	= true;
	}
	else if( !wxFileExists(FileName) )
	{
		MSG_Error_Add(_TL("file does not exist.")				, true, true, SG_UI_MSG_STYLE_FAILURE);
	}
	else if( !Project.Load(&FileName) )
	{
		MSG_Error_Add(_TL("could not read project file.")		, true, true, SG_UI_MSG_STYLE_FAILURE);
	}
	else if( Project.Get_Name().Cmp(SG_T("SAGA_PROJECT")) )
	{
		MSG_Error_Add(_TL("invalid project file.")			, true, true, SG_UI_MSG_STYLE_FAILURE);
	}
	else if( (pNode = Project.Get_Child(SG_T("DATA"))) == NULL || pNode->Get_Children_Count() <= 0 )
	{
		MSG_Error_Add(_TL("no data entries in project file.")	, true, true, SG_UI_MSG_STYLE_FAILURE);
	}
	else
	{
		int			i;

		CSG_String	Version(Project.Get_Property("VERSION"));

		bSuccess	= true;

		//-------------------------------------------------
		g_pData->Get_Menu_Files()->Set_Update(false);

		for(i=0; i<pNode->Get_Children_Count(); i++)
		{
			_Load_Data(*pNode->Get_Child(i), SG_File_Get_Path(FileName).w_str(), true , Version);
		}

		for(i=0; i<pNode->Get_Children_Count(); i++)
		{
			_Load_Data(*pNode->Get_Child(i), SG_File_Get_Path(FileName).w_str(), false, Version);
		}

		g_pData->Get_Menu_Files()->Set_Update(true);

		//-------------------------------------------------
		if( (pNode = Project.Get_Child(SG_T("MAPS"))) != NULL && pNode->Get_Children_Count() > 0 )
		{
			for(int j=0; j<pNode->Get_Children_Count(); j++)
			{
				_Load_Map(*pNode->Get_Child(j), SG_File_Get_Path(FileName).w_str());
			}
		}
	}

	//-----------------------------------------------------
	if( bSuccess )
	{
		if( bUpdateMenu )
		{
			g_pData->Get_Menu_Files()->Recent_Add(DATAOBJECT_TYPE_Undefined, FileName);
		}

		MSG_General_Add(_TL("Project has been successfully loaded."), true, true, SG_UI_MSG_STYLE_SUCCESS);

		m_File_Name	= FileName;

		_Set_Project_Name();

		return( true );
	}

	if( bUpdateMenu )
	{
		g_pData->Get_Menu_Files()->Recent_Del(DATAOBJECT_TYPE_Undefined, FileName);
	}

	MSG_General_Add(_TL("Could not load project."), true, true, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Project::_Save(const wxString &FileName, bool bSaveModified, bool bUpdateMenu)
{
	int							i, j;
	wxString					ProjectDir, oldFileName(m_File_Name);
	CSG_MetaData				Project, *pNode;
	CWKSP_Table_Manager			*pTables;
	CWKSP_Shapes_Manager		*pShapes;
	CWKSP_TIN_Manager			*pTINs;
	CWKSP_PointCloud_Manager	*pPointClouds;
	CWKSP_Grid_Manager			*pGrids;

	//-----------------------------------------------------
	m_File_Name	= FileName;

	if( bSaveModified && !Save_Modified(g_pData) )
	{
		m_File_Name	= oldFileName;

		return( false );
	}

	//-----------------------------------------------------
	ProjectDir	= SG_File_Get_Path(FileName).w_str();

	Project.Set_Name    (SG_T("SAGA_PROJECT"));
	Project.Add_Property(SG_T("VERSION"), SAGA_VERSION);

	//-----------------------------------------------------
	pNode		= Project.Add_Child(SG_T("DATA"));

	if( (pTables = g_pData->Get_Tables()) != NULL )
	{
		for(i=0; i<pTables->Get_Count(); i++)
		{
			_Save_Data(*pNode, ProjectDir,
				pTables->Get_Data(i)->Get_Object(),
				pTables->Get_Data(i)->Get_Parameters()
			);
		}
	}

	if( (pTINs = g_pData->Get_TINs()) != NULL )
	{
		for(i=0; i<pTINs->Get_Count(); i++)
		{
			_Save_Data(*pNode, ProjectDir,
				pTINs->Get_Data(i)->Get_Object(),
				pTINs->Get_Data(i)->Get_Parameters()
			);
		}
	}

	if( (pPointClouds = g_pData->Get_PointClouds()) != NULL )
	{
		for(i=0; i<pPointClouds->Get_Count(); i++)
		{
			_Save_Data(*pNode, ProjectDir,
				pPointClouds->Get_Data(i)->Get_Object(),
				pPointClouds->Get_Data(i)->Get_Parameters()
			);
		}
	}

	if( (pShapes = g_pData->Get_Shapes()) != NULL )
	{
		for(j=0; j<pShapes->Get_Count(); j++)
		{
			for(i=0; i<pShapes->Get_Shapes_Type(j)->Get_Count(); i++)
			{
				_Save_Data(*pNode, ProjectDir,
					pShapes->Get_Shapes_Type(j)->Get_Data(i)->Get_Object(),
					pShapes->Get_Shapes_Type(j)->Get_Data(i)->Get_Parameters()
				);
			}
		}
	}

	if( (pGrids = g_pData->Get_Grids()) != NULL )
	{
		for(j=0; j<pGrids->Get_Count(); j++)
		{
			for(i=0; i<pGrids->Get_System(j)->Get_Count(); i++)
			{
				_Save_Data(*pNode, ProjectDir,
					pGrids->Get_System(j)->Get_Data(i)->Get_Object(),
					pGrids->Get_System(j)->Get_Data(i)->Get_Parameters()
				);
			}
		}
	}

	//-----------------------------------------------------
	if( g_pMaps->Get_Count() > 0 )
	{
		pNode		= Project.Add_Child(SG_T("MAPS"));

		for(i=0; i<g_pMaps->Get_Count(); i++)
		{
			_Save_Map(*pNode, ProjectDir, g_pMaps->Get_Map(i));
		}
	}

	//-----------------------------------------------------
	if( Project.Save(&FileName) )
	{
		m_File_Name	= FileName;

		if( bUpdateMenu )
			g_pData->Get_Menu_Files()->Recent_Add(DATAOBJECT_TYPE_Undefined, FileName);

		MSG_General_Add(_TL("Project has been saved."), true, true, SG_UI_MSG_STYLE_SUCCESS);

		_Set_Project_Name();

		PROGRESSBAR_Set_Position(0);

		return( true );
	}

	m_File_Name.Clear();

	if( bUpdateMenu )
		g_pData->Get_Menu_Files()->Recent_Del(DATAOBJECT_TYPE_Undefined, FileName);

	MSG_General_Add(_TL("Could not save project."), true, true, SG_UI_MSG_STYLE_FAILURE);

	PROGRESSBAR_Set_Position(0);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::_Load_Data(CSG_MetaData &Entry, const wxString &ProjectDir, bool bLoad, const CSG_String &Version)
{
	TSG_Data_Object_Type	Type;
	wxString				File;
	CWKSP_Base_Item			*pItem;

	if( Entry.Get_Name().Cmp(SG_T("DATASET")) )
	{
		return( false );
	}

	Type	= Entry.Cmp_Property(SG_T("type"), SG_T("GRID"))	? DATAOBJECT_TYPE_Grid
			: Entry.Cmp_Property(SG_T("type"), SG_T("TABLE"))	? DATAOBJECT_TYPE_Table
			: Entry.Cmp_Property(SG_T("type"), SG_T("SHAPES"))	? DATAOBJECT_TYPE_Shapes
			: Entry.Cmp_Property(SG_T("type"), SG_T("TIN"))		? DATAOBJECT_TYPE_TIN
			: Entry.Cmp_Property(SG_T("type"), SG_T("POINTS"))	? DATAOBJECT_TYPE_PointCloud
			: DATAOBJECT_TYPE_Undefined;

	if( Type == DATAOBJECT_TYPE_Undefined )
	{
		return( false );
	}

	if( !Entry.Get_Child(SG_T("FILE")) )
	{
		return( false );
	}

	File	= Get_FilePath_Absolute(ProjectDir, Entry.Get_Child(SG_T("FILE"))->Get_Content().c_str());

	if( !wxFileExists(File) )
	{
		return( false );
	}

	pItem	= bLoad ? g_pData->Open(File, Type) : _Get_byFileName(File);

	if(	!pItem )
	{
		return( false );
	}

	if( pItem->Get_Parameters() && Entry.Get_Child(SG_T("PARAMETERS")) )
	{
		pItem->Get_Parameters()->Serialize(*Entry.Get_Child(SG_T("PARAMETERS")), false);
		_Compatibility_Data((TSG_Data_Type)Type, pItem->Get_Parameters(), Version);
		pItem->Parameters_Changed();
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Project::_Save_Data(CSG_MetaData &Entry, const wxString &ProjectDir, CSG_Data_Object *pDataObject, CSG_Parameters *pParameters)
{
	if( !pDataObject || !pDataObject->Get_File_Name() || !wxFileExists(pDataObject->Get_File_Name()) )
	{
		return( false );
	}

	CSG_MetaData	*pEntry	= Entry.Add_Child(SG_T("DATASET"));

	switch( pDataObject->Get_ObjectType() )
	{
	default:	return( false );
	case DATAOBJECT_TYPE_Grid:			pEntry->Add_Property(SG_T("type"), SG_T("GRID"));	break;
	case DATAOBJECT_TYPE_Table:			pEntry->Add_Property(SG_T("type"), SG_T("TABLE"));	break;
	case DATAOBJECT_TYPE_Shapes:		pEntry->Add_Property(SG_T("type"), SG_T("SHAPES"));	break;
	case DATAOBJECT_TYPE_TIN:			pEntry->Add_Property(SG_T("type"), SG_T("TIN"));	break;
	case DATAOBJECT_TYPE_PointCloud:	pEntry->Add_Property(SG_T("type"), SG_T("POINTS"));	break;
	}

	pEntry->Add_Child(SG_T("FILE"), Get_FilePath_Relative(ProjectDir, pDataObject->Get_File_Name()).c_str());

	if( pParameters )
	{
		pParameters->DataObjects_Check(true);

		pParameters->Serialize(*pEntry->Add_Child(SG_T("PARAMETERS")), true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::_Load_Map(CSG_MetaData &Entry, const wxString &ProjectDir)
{
	TSG_Rect		r;
	CSG_MetaData	*pNode;
	CWKSP_Base_Item	*pItem;
	CWKSP_Map		*pMap;

	if( Entry.Get_Name().Cmp(SG_T("MAP")) )
	{
		return( false );
	}

	if(	!Entry.Get_Child(SG_T("XMIN")) || !Entry.Get_Child(SG_T("XMIN"))->Get_Content().asDouble(r.xMin)
	||	!Entry.Get_Child(SG_T("XMAX")) || !Entry.Get_Child(SG_T("XMAX"))->Get_Content().asDouble(r.xMax)
	||	!Entry.Get_Child(SG_T("YMIN")) || !Entry.Get_Child(SG_T("YMIN"))->Get_Content().asDouble(r.yMin)
	||	!Entry.Get_Child(SG_T("YMAX")) || !Entry.Get_Child(SG_T("YMAX"))->Get_Content().asDouble(r.yMax) )
	{
		return( false );
	}

	if( (pNode = Entry.Get_Child(SG_T("LAYERS"))) == NULL || pNode->Get_Children_Count() <= 0 )
	{
		return( false );
	}

	pMap	= NULL;

	for(int i=0; i<pNode->Get_Children_Count(); i++)
	{
		if(	(pItem = _Get_byFileName(Get_FilePath_Absolute(ProjectDir, pNode->Get_Child(i)->Get_Content().w_str()))) != NULL
		&&	(	pItem->Get_Type()	== WKSP_ITEM_Grid
			||	pItem->Get_Type()	== WKSP_ITEM_TIN
			||	pItem->Get_Type()	== WKSP_ITEM_PointCloud
			||	pItem->Get_Type()	== WKSP_ITEM_Shapes) )
		{
			if( pMap == NULL )
			{
				pMap	= new CWKSP_Map;
			}

			g_pMaps->Add((CWKSP_Layer *)pItem, pMap);
		}
	}

	if( !pMap )
	{
		return( false );
	}

	if( Entry.Get_Child(SG_T("NAME")) && Entry.Get_Child(SG_T("NAME"))->Get_Content().Length() > 0 )
	{
		pMap->Get_Parameters()->Get_Parameter("NAME")->Set_Value(Entry.Get_Child(SG_T("NAME"))->Get_Content());
		pMap->Parameters_Changed();
	}

	pMap->Set_Extent(r, true);
	pMap->View_Show(true);

	return( true );
}


//---------------------------------------------------------
bool CWKSP_Project::_Save_Map(CSG_MetaData &Entry, const wxString &ProjectDir, CWKSP_Map *pMap)
{
	if( !pMap )
	{
		return( false );
	}

	CSG_MetaData	*pEntry	= Entry.Add_Child(SG_T("MAP"));

	pEntry->Add_Child(SG_T("NAME"), pMap->Get_Name().c_str());
	pEntry->Add_Child(SG_T("XMIN"), pMap->Get_Extent().Get_XMin());
	pEntry->Add_Child(SG_T("XMAX"), pMap->Get_Extent().Get_XMax());
	pEntry->Add_Child(SG_T("YMIN"), pMap->Get_Extent().Get_YMin());
	pEntry->Add_Child(SG_T("YMAX"), pMap->Get_Extent().Get_YMax());

	pEntry	= pEntry->Add_Child(SG_T("LAYERS"));

	for(int i=pMap->Get_Count()-1; i>=0; i--)
	{
		if( pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_File_Name() != NULL )
		{
			pEntry->Add_Child(SG_T("FILE"), Get_FilePath_Relative(ProjectDir, pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_File_Name()).c_str());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Base_Item * CWKSP_Project::_Get_byFileName(const wxString &FileName)
{
	return( g_pData->Get(SG_Get_Data_Manager().Find(&FileName)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::Save_Modified(CWKSP_Base_Item *pItem, bool bSelections)
{
	CSG_Parameters	Parameters;

	Parameters.Create(this, _TL("Close and save modified data sets..."), _TL(""));
	Parameters.Add_Value(NULL, "SAVE_ALL", _TL("Save all"), _TL(""), PARAMETER_TYPE_Bool, false);

	_Modified_Get(&Parameters, pItem, bSelections);

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
int CWKSP_Project::_Modified_Changed(CSG_Parameter *pParameter, int Flags)
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
bool CWKSP_Project::_Modified_Get(CSG_Parameters *pParameters, CWKSP_Base_Item *pItem, bool bSelections)
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
				_Modified_Get(pParameters, ((CWKSP_Base_Manager *)pItem)->Get_Item(i), bSelections && !pItem->is_Selected());
			}
			break;

		//-------------------------------------------------
		case WKSP_ITEM_Table:
			if( !bSelections || pItem->is_Selected() )
			{
				_Modified_Get(pParameters, pItem, ((CWKSP_Table *)pItem)->Get_Table() );
			}
			break;

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Grid:
			if( !bSelections || pItem->is_Selected() )
			{
				_Modified_Get(pParameters, pItem, ((CWKSP_Layer *)pItem)->Get_Object());
			}
			break;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Project::_Modified_Get(CSG_Parameters *pParameters, CWKSP_Base_Item *pItem, CSG_Data_Object *pObject)
{
	if( pObject->is_Modified() )
	{
		wxString		sFilter, sExtension, sPath;

		switch( pItem->Get_Type() )
		{
		//-------------------------------------------------
		default:	return( false );
		case WKSP_ITEM_Table:		sExtension	= SG_T("txt" );	sFilter	= DLG_Get_FILE_Filter(ID_DLG_TABLES_SAVE);		break;
		case WKSP_ITEM_Shapes:		sExtension	= SG_T("shp" );	sFilter	= DLG_Get_FILE_Filter(ID_DLG_SHAPES_SAVE);		break;
		case WKSP_ITEM_TIN:			sExtension	= SG_T("shp" );	sFilter	= DLG_Get_FILE_Filter(ID_DLG_TIN_SAVE);			break;
		case WKSP_ITEM_PointCloud:	sExtension	= SG_T("spc" );	sFilter	= DLG_Get_FILE_Filter(ID_DLG_POINTCLOUD_SAVE);	break;
		case WKSP_ITEM_Grid:		sExtension	= SG_T("sgrd");	sFilter	= DLG_Get_FILE_Filter(ID_DLG_GRIDS_SAVE);		break;
		}

		sPath	= pObject->Get_File_Name();

		if( sPath.Length() == 0 )
		{
			sPath	= SG_File_Make_Path(SG_File_Get_Path(Get_File_Name()), pObject->Get_Name()).w_str();
			sPath	+= SG_T(".") + sExtension;
		}

		//-------------------------------------------------
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
			_TL("File"), SG_T(""), sFilter, sPath, true
		);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Project::_Modified_Save(CSG_Parameters *pParameters)
{
	for(int i=0, j=0; i<pParameters->Get_Count(); i++)
	{
		long long		Pointer;
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		if(	pParameter->Get_Type() == PARAMETER_TYPE_Bool && pParameter->asBool()
		&&	SG_SSCANF(pParameter->Get_Identifier(), SG_T("%lld"), (&Pointer)) == 1 )
		{
			CSG_Data_Object	*pObject	= (CSG_Data_Object *)Pointer;

			if(	SG_Get_Data_Manager().Exists(pObject) )
			{
				CSG_String		fPath;
				CSG_Parameter	*pPath	= pParameters->Get_Parameter(CSG_String::Format(SG_T("%d FILE"), (long)pObject));

				if(	pPath && pPath->asString() && pPath->asString()[0] )
				{
					fPath	= pPath->asString();
				}
				else
				{
					fPath	= SG_File_Make_Path(SG_File_Get_Path(Get_File_Name()), CSG_String::Format(SG_T("%02d_%s"), ++j, pObject->Get_Name()), NULL);
				}

				pObject->Save(fPath);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					v2.1 Compatibility					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::_Compatibility_Data(TSG_Data_Type Type, CSG_Parameters *pParameters, const CSG_String &Version)
{
	if( !pParameters )
	{
		return( false );
	}

	if( !Version.Cmp(SAGA_VERSION) )
	{
		return( true );
	}

	//-----------------------------------------------------
	if( Version.is_Empty() )
	{
		CSG_Parameter	*pParameter;

		if( Type == DATAOBJECT_TYPE_Grid )
		{
			if( (pParameter = pParameters->Get_Parameter("COLORS_TYPE")) != NULL )
			{
				if( pParameter->asInt() == 3 )
				{	// 0:Single >> 1:LUT >> 2:Discrete >> 3:Graduated >> 4:Shade >> 5:Overlay >> 6:RGB
					pParameter->Set_Value(6);	// RGB moved to position 6
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					v2.0 Compatibility					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DATA_ENTRIES_BEGIN	wxT("[DATA_ENTRIES_BEGIN]")
#define DATA_ENTRIES_END	wxT("[DATA_ENTRIES_END]")
#define DATA_ENTRY_BEGIN	wxT("[DATA_ENTRY_BEGIN]")
#define DATA_ENTRY_END		wxT("[DATA_ENTRY_END]")

//---------------------------------------------------------
#define MAP_ENTRIES_BEGIN	wxT("[MAP_ENTRIES_BEGIN]")
#define MAP_ENTRIES_END		wxT("[MAP_ENTRIES_END]")
#define MAP_ENTRY_BEGIN		wxT("[MAP_ENTRY_BEGIN]")
#define MAP_ENTRY_END		wxT("[MAP_ENTRY_END]")
#define MAP_ENTRY_NAME		wxT("[MAP_ENTRY_NAME]")

//---------------------------------------------------------
bool CWKSP_Project::_Compatibility_Load_Data(const wxString &FileName)
{
	CSG_String	sLine;
	CSG_File	Stream;

	if( !Stream.Open(&FileName, SG_FILE_R, true) )
	{
		return( false );
	}

	//-------------------------------------------------
	while( Stream.Read_Line(sLine) && sLine.Cmp(DATA_ENTRIES_BEGIN) );

	if( sLine.Cmp(DATA_ENTRIES_BEGIN) )
	{
		return( false );
	}

	g_pData->Get_Menu_Files()->Set_Update(false);
	while( _Compatibility_Load_Data(Stream, SG_File_Get_Path(FileName).w_str()) );
	g_pData->Get_Menu_Files()->Set_Update(true);

	//-------------------------------------------------
	while( Stream.Read_Line(sLine) && sLine.Cmp(MAP_ENTRIES_BEGIN) );

	if( !sLine.Cmp(MAP_ENTRIES_BEGIN) )
	{
		while( _Compatibility_Load_Map(Stream, SG_File_Get_Path(FileName).w_str()) );
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Project::_Compatibility_Load_Data(CSG_File &Stream, const wxString &ProjectDir)
{
	int				Type;
	CSG_String		sLine, sPath;
	CWKSP_Base_Item	*pItem;

	while( Stream.Read_Line(sLine) && sLine.Cmp(DATA_ENTRY_BEGIN) && sLine.Cmp(DATA_ENTRIES_END) );

	if( !sLine.Cmp(DATA_ENTRY_BEGIN) )
	{
		if(	Stream.Read_Line(sPath) && Stream.Read_Line(sLine) && sLine.asInt(Type) )
		{
			if( SG_File_Exists(sPath = Get_FilePath_Absolute(ProjectDir, sPath.w_str()).wc_str()) )
			{
				if(	(pItem = g_pData->Open(sPath.w_str(), Type)) != NULL )
				{
					if( pItem->Get_Parameters() )
					{
						pItem->Get_Parameters()->Serialize_Compatibility(Stream);
						_Compatibility_Data((TSG_Data_Type)Type, pItem->Get_Parameters(), "");
						pItem->Parameters_Changed();
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Project::_Compatibility_Load_Map(CSG_File &Stream, const wxString &ProjectDir)
{
	TSG_Rect		r;
	CSG_String		sLine, sName;
	CWKSP_Base_Item	*pItem;
	CWKSP_Map		*pMap;

	while( Stream.Read_Line(sLine) && sLine.Cmp(MAP_ENTRY_BEGIN) && sLine.Cmp(MAP_ENTRIES_END) );

	if( !sLine.Cmp(MAP_ENTRY_BEGIN) && Stream.Read_Line(sLine) )
	{
		if(	SG_SSCANF(sLine, SG_T("%lf %lf %lf %lf"), &r.xMin, &r.xMax, &r.yMin, &r.yMax) == 4 )
		{
			pMap	= NULL;

			while( Stream.Read_Line(sLine) && sLine.Cmp(MAP_ENTRY_END) )
			{
				if( !sLine.Cmp(MAP_ENTRY_NAME) )
				{
					Stream.Read_Line(sName);
				}
				else
				{
					sLine	= Get_FilePath_Absolute(ProjectDir, sLine.w_str()).wc_str();

					if(	(pItem = _Get_byFileName(sLine.c_str())) != NULL
					&&	(	pItem->Get_Type()	== WKSP_ITEM_Grid
						||	pItem->Get_Type()	== WKSP_ITEM_TIN
						||	pItem->Get_Type()	== WKSP_ITEM_PointCloud
						||	pItem->Get_Type()	== WKSP_ITEM_Shapes) )
					{
						if( pMap == NULL )
						{
							pMap	= new CWKSP_Map;
						}

						g_pMaps->Add((CWKSP_Layer *)pItem, pMap);
					}
				}
			}

			if( pMap )
			{
				if( sName.Length() > 0 )
				{
					pMap->Get_Parameters()->Get_Parameter("NAME")->Set_Value(sName);
					pMap->Parameters_Changed();
				}

				pMap->Set_Extent(r, true);
				pMap->View_Show(true);
			}
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
