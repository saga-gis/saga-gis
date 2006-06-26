
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

#include <saga_api/saga_api.h>

#include "helper.h"

#include "res_dialogs.h"

#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"

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
#define DATA_ENTRIES_BEGIN	"[DATA_ENTRIES_BEGIN]"
#define DATA_ENTRIES_END	"[DATA_ENTRIES_END]"
#define DATA_ENTRY_BEGIN	"[DATA_ENTRY_BEGIN]"
#define DATA_ENTRY_END		"[DATA_ENTRY_END]"

//---------------------------------------------------------
#define MAP_ENTRIES_BEGIN	"[MAP_ENTRIES_BEGIN]"
#define MAP_ENTRIES_END		"[MAP_ENTRIES_END]"
#define MAP_ENTRY_BEGIN		"[MAP_ENTRY_BEGIN]"
#define MAP_ENTRY_END		"[MAP_ENTRY_END]"


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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::Load(bool bAdd)
{
	wxString	FileName;

	return( DLG_Open(FileName, ID_DLG_PROJECT_OPEN) && _Load(FileName, bAdd) );
}

//---------------------------------------------------------
bool CWKSP_Project::Load(const char *FileName, bool bAdd)
{
	return( _Load(FileName, bAdd) );
}

//---------------------------------------------------------
bool CWKSP_Project::Save(bool bSaveAsOnError)
{
	if( Has_File_Name() )
	{
		return( _Save(m_File_Name) );
	}

	return( bSaveAsOnError ? Save_As() : false );
}

//---------------------------------------------------------
bool CWKSP_Project::Save_As(void)
{
	wxString	FileName;

	return( DLG_Save(FileName, ID_DLG_PROJECT_SAVE) && _Save(FileName) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::_Load(const char *FileName, bool bAdd)
{
	bool		bSuccess;
	FILE		*Stream;
	CAPI_String	sLine, ProjectDir;

	//-----------------------------------------------------
	bSuccess	= false;

	MSG_General_Add(wxString::Format("%s: %s", LNG("[MSG] Load project"), FileName), true, true);

	//-----------------------------------------------------
	if( (Stream = fopen(FileName, "rb")) != NULL )
	{
		ProjectDir	= API_Extract_File_Path(FileName);

		while( API_Read_Line(Stream, sLine) && sLine.Cmp(DATA_ENTRIES_BEGIN) );

		if( !sLine.Cmp(DATA_ENTRIES_BEGIN) )
		{
			if( !bAdd )
			{
				g_pData->Close(false);
			}

			g_pData->Get_FileMenus()->Set_Update(false);
			while( _Load_Data(Stream, ProjectDir) );
			g_pData->Get_FileMenus()->Set_Update(true);

			bSuccess	= true;
		}

		//-------------------------------------------------
		while( API_Read_Line(Stream, sLine) && sLine.Cmp(MAP_ENTRIES_BEGIN) );

		if( !sLine.Cmp(MAP_ENTRIES_BEGIN) )
		{
			while( _Load_Map(Stream, ProjectDir) );
		}

		fclose(Stream);
	}

	//-----------------------------------------------------
	if( bSuccess )
	{
		m_File_Name	= FileName;

		g_pData->Get_FileMenus()->Recent_Add(DATAOBJECT_TYPE_Undefined, FileName);

		MSG_General_Add(LNG("[MSG] Project has been successfully loaded."), true, true);

		return( true );
	}
	else
	{
		g_pData->Get_FileMenus()->Recent_Del(DATAOBJECT_TYPE_Undefined, FileName);

		MSG_General_Add(LNG("[MSG] Could not load project."), true, true);

		return( false );
	}
}

//---------------------------------------------------------
bool CWKSP_Project::_Save(const char *FileName)
{
	int						i, j;
	CAPI_String				ProjectDir;
	FILE					*Stream;
	CWKSP_Table_Manager		*pTables;
	CWKSP_Shapes_Manager	*pShapes;
	CWKSP_TIN_Manager		*pTINs;
	CWKSP_Grid_Manager		*pGrids;

	//-----------------------------------------------------
	if( (Stream = fopen(FileName, "wb")) != NULL )
	{
		ProjectDir	= API_Extract_File_Path(FileName);

		Save_Modified(g_pData);

		fprintf(Stream, "\n%s\n", DATA_ENTRIES_BEGIN);

		//-------------------------------------------------
		if( (pTables = g_pData->Get_Tables()) != NULL )
		{
			for(i=0; i<pTables->Get_Count(); i++)
			{
				_Save_Data(Stream, ProjectDir,
					pTables->Get_Table(i)->Get_Table(),
					pTables->Get_Table(i)->Get_Parameters()
				);
			}
		}

		if( (pShapes = g_pData->Get_Shapes()) != NULL )
		{
			for(j=0; j<pShapes->Get_Count(); j++)
			{
				for(i=0; i<((CWKSP_Shapes_Type *)pShapes->Get_Item(j))->Get_Count(); i++)
				{
					_Save_Data(Stream, ProjectDir,
						((CWKSP_Shapes_Type *)pShapes->Get_Item(j))->Get_Shapes(i)->Get_Shapes(),
						((CWKSP_Shapes_Type *)pShapes->Get_Item(j))->Get_Shapes(i)->Get_Parameters()
					);
				}
			}
		}

		if( (pTINs = g_pData->Get_TINs()) != NULL )
		{
			for(i=0; i<pTINs->Get_Count(); i++)
			{
				_Save_Data(Stream, ProjectDir,
					pTINs->Get_TIN(i)->Get_TIN(),
					pTINs->Get_TIN(i)->Get_Parameters()
				);
			}
		}

		if( (pGrids = g_pData->Get_Grids()) != NULL )
		{
			for(j=0; j<pGrids->Get_Count(); j++)
			{
				for(i=0; i<pGrids->Get_System(j)->Get_Count(); i++)
				{
					_Save_Data(Stream, ProjectDir,
						pGrids->Get_System(j)->Get_Grid(i)->Get_Grid(),
						pGrids->Get_System(j)->Get_Grid(i)->Get_Parameters()
					);
				}
			}
		}

		fprintf(Stream, "%s\n", DATA_ENTRIES_END);

		//-------------------------------------------------
		if( g_pMaps->Get_Count() > 0 )
		{
			fprintf(Stream, "\n%s\n", MAP_ENTRIES_BEGIN);

			for(i=0; i<g_pMaps->Get_Count(); i++)
			{
				_Save_Map(Stream, ProjectDir, g_pMaps->Get_Map(i));
			}

			fprintf(Stream, "%s\n", MAP_ENTRIES_END);
		}

		//-------------------------------------------------
		fclose(Stream);

		m_File_Name	= FileName;

		g_pData->Get_FileMenus()->Recent_Add(DATAOBJECT_TYPE_Undefined, m_File_Name);

		MSG_General_Add(LNG("[MSG] Project has been saved."), true, true);

		return( true );
	}

	m_File_Name.Clear();

	MSG_General_Add(LNG("[MSG] Could not save project."), true, true);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::_Load_Data(FILE *Stream, const char *ProjectDir)
{
	int				Type;
	CAPI_String		sLine, sPath;
	CWKSP_Base_Item	*pItem;

	while( API_Read_Line(Stream, sLine) && sLine.Cmp(DATA_ENTRY_BEGIN) && sLine.Cmp(DATA_ENTRIES_END) );

	if( !sLine.Cmp(DATA_ENTRY_BEGIN) )
	{
		if(	API_Read_Line(Stream, sPath) && API_Read_Line(Stream, sLine) && sLine.asInt(Type) )
		{
			if( wxFileExists(sPath.c_str()) || wxFileExists((sPath = Get_FilePath_Absolute(ProjectDir, sPath)).c_str()) )
			{
				if(	(pItem = g_pData->Open(Type, sPath)) != NULL )
				{
					if( pItem->Get_Parameters() )
					{
						pItem->Get_Parameters()->Serialize(Stream, false);
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
bool CWKSP_Project::_Save_Data(FILE *Stream, const char *ProjectDir, CDataObject *pDataObject, CParameters *pParameters)
{
	if( pDataObject && pDataObject->Get_File_Path() != NULL )
	{
		fprintf(Stream, "\n");
		fprintf(Stream, "%s\n", DATA_ENTRY_BEGIN);

		fprintf(Stream, "%s\n", Get_FilePath_Relative(ProjectDir, pDataObject->Get_File_Path()).c_str());
		fprintf(Stream, "%d\n", pDataObject->Get_ObjectType());

		if( pParameters )
		{
			pParameters->Serialize(Stream, true);
		}

		fprintf(Stream, "%s\n", DATA_ENTRY_END);

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
bool CWKSP_Project::_Load_Map(FILE *Stream, const char *ProjectDir)
{
	TGEO_Rect		r;
	CAPI_String		sLine;
	CWKSP_Base_Item	*pItem;
	CWKSP_Map		*pMap;

	while( API_Read_Line(Stream, sLine) && sLine.Cmp(MAP_ENTRY_BEGIN) && sLine.Cmp(MAP_ENTRIES_END) );

	if( !sLine.Cmp(MAP_ENTRY_BEGIN) )
	{
		if(	API_Read_Line(Stream, sLine) && sscanf(sLine, "%lf %lf %lf %lf", &r.xMin, &r.xMax, &r.yMin, &r.yMax) == 4 )
		{
			pMap	= NULL;

			while( API_Read_Line(Stream, sLine) && sLine.Cmp(MAP_ENTRY_END) )
			{
				sLine	= Get_FilePath_Absolute(ProjectDir, sLine).c_str();

				if(	(pItem = _Get_byFileName(sLine.c_str())) != NULL
				&&	(	pItem->Get_Type()	== WKSP_ITEM_Grid
					||	pItem->Get_Type()	== WKSP_ITEM_TIN
					||	pItem->Get_Type()	== WKSP_ITEM_Shapes) )
				{
					if( pMap == NULL )
					{
						pMap	= new CWKSP_Map;
					}

					g_pMaps->Add((CWKSP_Layer *)pItem, pMap);
				}
			}

			if( pMap )
			{
				pMap->Set_Extent(r);
				pMap->View_Show(true);
			}
		}

		return( true );
	}

	return( false );
}


//---------------------------------------------------------
bool CWKSP_Project::_Save_Map(FILE *Stream, const char *ProjectDir, CWKSP_Map *pMap)
{
	if( pMap )
	{
		fprintf(Stream, "%s\n", MAP_ENTRY_BEGIN);

		fprintf(Stream, "%f %f %f %f\n",
			pMap->Get_Extent().Get_XMin(), pMap->Get_Extent().Get_XMax(),
			pMap->Get_Extent().Get_YMin(), pMap->Get_Extent().Get_YMax()
		);

		for(int i=pMap->Get_Count()-1; i>=0; i--)
		{
			if( pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_File_Path() != NULL )
			{
				fprintf(Stream, "%s\n", Get_FilePath_Relative(ProjectDir, pMap->Get_Layer(i)->Get_Layer()->Get_Object()->Get_File_Path()).c_str());
			}
		}

		fprintf(Stream, "%s\n", MAP_ENTRY_END);

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
CWKSP_Base_Item * CWKSP_Project::_Get_byFileName(wxString FileName)
{
	const char				*s;
	int						i, j;
	CWKSP_Table_Manager		*pTables;
	CWKSP_Shapes_Manager	*pShapes;
	CWKSP_TIN_Manager		*pTINs;
	CWKSP_Grid_Manager		*pGrids;

	//-----------------------------------------------------
	if( FileName.Length() > 0 )
	{
		if( (pTables = g_pData->Get_Tables()) != NULL )
		{
			for(i=0; i<pTables->Get_Count(); i++)
			{
				if( (s = pTables->Get_Table(i)->Get_Table()->Get_File_Path()) && !FileName.Cmp(s) )
				{
					return( pTables->Get_Table(i) );
				}
			}
		}

		if( (pShapes = g_pData->Get_Shapes()) != NULL )
		{
			for(j=0; j<pShapes->Get_Count(); j++)
			{
				for(i=0; i<((CWKSP_Shapes_Type *)pShapes->Get_Item(j))->Get_Count(); i++)
				{
					if( (s = ((CWKSP_Shapes_Type *)pShapes->Get_Item(j))->Get_Shapes(i)->Get_Shapes()->Get_File_Path()) && !FileName.Cmp(s) )
					{
						return( ((CWKSP_Shapes_Type *)pShapes->Get_Item(j))->Get_Shapes(i) );
					}
				}
			}
		}

		if( (pTINs = g_pData->Get_TINs()) != NULL )
		{
			for(i=0; i<pTINs->Get_Count(); i++)
			{
				if( (s = pTINs->Get_TIN(i)->Get_TIN()->Get_File_Path()) && !FileName.Cmp(s) )
				{
					return( pTINs->Get_TIN(i) );
				}
			}
		}

		if( (pGrids = g_pData->Get_Grids()) != NULL )
		{
			for(j=0; j<pGrids->Get_Count(); j++)
			{
				for(i=0; i<pGrids->Get_System(j)->Get_Count(); i++)
				{
					if( (s = pGrids->Get_System(j)->Get_Grid(i)->Get_Grid()->Get_File_Path()) && !FileName.Cmp(s) )
					{
						return( pGrids->Get_System(j)->Get_Grid(i) );
					}
				}
			}
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Project::Save_Modified(CWKSP_Base_Item *pItem)
{
	CParameters	Parameters;

	Parameters.Set_Name(LNG("[CAP] Save Modified Data Objects"));

	_Modified_Get(&Parameters, pItem);

	if( Parameters.Get_Count() > 0 )
	{
		if( !DLG_Parameters(&Parameters) )
		{
			return( false );
		}

		_Modified_Save(&Parameters);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Project::_Modified_Get(CParameters *pParameters, CWKSP_Base_Item *pItem)
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
		case WKSP_ITEM_Grid_Manager:
		case WKSP_ITEM_Grid_System:
			for(i=0; i<((CWKSP_Base_Manager *)pItem)->Get_Count(); i++)
			{
				_Modified_Get(pParameters, ((CWKSP_Base_Manager *)pItem)->Get_Item(i));
			}
			break;

		//-------------------------------------------------
		case WKSP_ITEM_Table:
			_Modified_Get(pParameters, pItem, ((CWKSP_Table *)pItem)->Get_Table() );
			break;

		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_Grid:
			_Modified_Get(pParameters, pItem, ((CWKSP_Layer *)pItem)->Get_Object());
			break;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Project::_Modified_Get(CParameters *pParameters, CWKSP_Base_Item *pItem, CDataObject *pObject)
{
	CParameter	*pNode, *pParent;
	wxString	sFilter;

	if( pObject->is_Modified() )
	{
		switch( pItem->Get_Type() )
		{
		//-------------------------------------------------
		default:	return( false );
		case WKSP_ITEM_Table:	sFilter	= DLG_Get_FILE_Filter(ID_DLG_TABLES_SAVE);	break;
		case WKSP_ITEM_Shapes:	sFilter	= DLG_Get_FILE_Filter(ID_DLG_SHAPES_SAVE);	break;
		case WKSP_ITEM_TIN:		sFilter	= DLG_Get_FILE_Filter(ID_DLG_TIN_SAVE);		break;
		case WKSP_ITEM_Grid:	sFilter	= DLG_Get_FILE_Filter(ID_DLG_GRIDS_SAVE);	break;
		}

		if( (pParent = pParameters->Get_Parameter(pItem->Get_Manager()->Get_Name())) == NULL )
		{
			pParent	= pParameters->Add_Node(NULL, pItem->Get_Manager()->Get_Name(), pItem->Get_Manager()->Get_Name(), "");
		}			

		//-------------------------------------------------
		pNode	= pParameters->Add_Node(
			pParent,
			wxString::Format("%d NODE", (int)pObject),
			pItem->Get_Name(), ""
		);

		pParameters->Add_Value(
			pNode,
			wxString::Format("%d"     , (int)pObject),
			LNG("[CAP] Save"), "", PARAMETER_TYPE_Bool, false
		);

		pParameters->Add_FilePath(
			pNode,
			wxString::Format("%d FILE", (int)pObject),
			LNG("[CAP] File"), "", sFilter, pObject->Get_File_Path(), true
		);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Project::_Modified_Save(CParameters *pParameters)
{
	CDataObject	*pObject;
	CParameter	*pParameter, *pPath	= NULL;

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		pParameter	= pParameters->Get_Parameter(i);

		if(	pParameter->Get_Type() == PARAMETER_TYPE_Bool
		&&	pParameter->asBool()

		&&	sscanf(pParameter->Get_Identifier(), "%d", (int *)(&pObject)) == 1
		&&	g_pData->Exists(pObject)

		&&	(pPath = pParameters->Get_Parameter(wxString::Format("%d FILE", (int)pObject))) != NULL
		&&	pPath->asString() != NULL
		&&	strlen(pPath->asString()) > 0 )
		{
			pObject->Save(pPath->asString());
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
