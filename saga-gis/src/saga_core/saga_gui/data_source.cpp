
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
//                   data_source.cpp                     //
//                                                       //
//          Copyright (C) 2011 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "data_source.h"
#include "data_source_files.h"
#include "data_source_odbc.h"
#include "data_source_pgsql.h"

#include "wksp_layer.h"
#include "wksp_map_layer.h"

#include "wksp_data_manager.h"
#include "wksp_data_layers.h"
#include "wksp_table.h"

#include "wksp_tool_library.h"
#include "wksp_tool.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_FILES	= 0,
	IMG_DATABASE,
	IMG_WEBSERVICE
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CData_Source	*g_pData_Source	= NULL;


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CData_Source, wxNotebook)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CData_Source::CData_Source(wxWindow *pParent)
	: wxNotebook(pParent, ID_WND_DATA_SOURCE, wxDefaultPosition, wxDefaultSize, wxNB_TOP|wxNB_MULTILINE, _TL("Data Sources"))
{
	g_pData_Source	= this;

	//-----------------------------------------------------
	AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));

	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_DATA_SOURCE_FILES     );
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_DATA_SOURCE_DATABASE  );
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_DATA_SOURCE_WEBSERVICE);

	//-----------------------------------------------------
	bool	bFiles;

	if( CONFIG_Read("/DATA", "SHOW_FILE_SOURCES", bFiles) && !bFiles )
	{
		m_pFiles	= NULL;
	}
	else
	{
		m_pFiles	= new CData_Source_Files(this);	m_pFiles->SetName(_TL("File System"));
	}

	m_pODBC		= new CData_Source_ODBC (this);	m_pODBC ->SetName(_TL("ODBC"));
	m_pPgSQL	= new CData_Source_PgSQL(this);	m_pPgSQL->SetName(_TL("PostgreSQL"));

#if defined(_SAGA_MSW)
	if( m_pFiles )	m_pFiles->Hide();
	if( m_pODBC  )	m_pODBC ->Hide();
	if( m_pPgSQL )	m_pPgSQL->Hide();
#endif
}

//---------------------------------------------------------
void CData_Source::Add_Pages(void)
{
	_Show_Page(m_pFiles);
	_Show_Page(m_pODBC );
	_Show_Page(m_pPgSQL);

	long	lValue;

	if( CONFIG_Read("/DATA/SOURCE", "TAB", lValue) )
	{
		SetSelection((size_t)lValue);
	}
}

//---------------------------------------------------------
CData_Source::~CData_Source(void)
{
	CONFIG_Write("/DATA/SOURCE", "TAB", (long)GetSelection());

	g_pData_Source		= NULL;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CData_Source::Autoconnect_DB(void)
{
	m_pPgSQL->Autoconnect();
}

//---------------------------------------------------------
bool CData_Source::Set_Data_Source(CWKSP_Base_Item *pItem)
{
	if( pItem == NULL )
	{
		return( true );
	}

	//-----------------------------------------------------
	if( m_pFiles )
	{
		switch( pItem->Get_Type() )
		{
		case WKSP_ITEM_Shapes      :
		case WKSP_ITEM_TIN         :
		case WKSP_ITEM_PointCloud  :
		case WKSP_ITEM_Grid        :
		case WKSP_ITEM_Grids       :
			m_pFiles->SetPath(((CWKSP_Layer        *)pItem)->Get_Object()->Get_File_Name(false));
			break;

		case WKSP_ITEM_Table       :
			m_pFiles->SetPath(((CWKSP_Table        *)pItem)->Get_Table()->Get_File_Name());
			break;

		case WKSP_ITEM_Map_Layer   :
			m_pFiles->SetPath(((CWKSP_Map_Layer    *)pItem)->Get_Layer()->Get_Object()->Get_File_Name(false));
			break;

		case WKSP_ITEM_Tool_Library:
		//	m_pFiles->SetPath(((CWKSP_Tool_Library *)pItem)->Get_File_Name());
			break;

		case WKSP_ITEM_Tool        :
		//	m_pFiles->SetPath(((CWKSP_Tool         *)pItem)->Get_File_Name());
			break;

		default:
			break;
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CData_Source::Update_Database(const wxString &Server)
{
	m_pODBC ->Update_Source(Server);
	m_pPgSQL->Update_Source(Server);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CData_Source::_Show_Page(wxWindow *pPage)
{
	if( !pPage )
	{
		return( false );
	}

	int		Image_ID	= -1;

	//-----------------------------------------------------
	if( pPage == m_pFiles )	Image_ID	= IMG_FILES;
	if( pPage == m_pODBC  )	Image_ID	= IMG_DATABASE;
	if( pPage == m_pPgSQL )	Image_ID	= IMG_DATABASE;

	//-----------------------------------------------------
	if( pPage )
	{
		for(int i=0; i<(int)GetPageCount(); i++)
		{
			if( GetPage(i) == pPage )
			{
				return( true );
			}
		}

		AddPage(pPage, pPage->GetName(), false, Image_ID);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CData_Source::_Hide_Page(wxWindow *pPage)
{
	for(int i=0; i<(int)GetPageCount(); i++)
	{
		if( GetPage(i) == pPage )
		{
			if( i == GetSelection() )
			{
				SetSelection(i - 1);
			}

			RemovePage(i);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
