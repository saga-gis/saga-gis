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
//                     ACTIVE.cpp                        //
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
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "data_source.h"

#include "active.h"
#include "active_parameters.h"
#include "active_description.h"
#include "active_history.h"
#include "active_attributes.h"
#include "active_legend.h"
#include "active_HTMLExtraInfo.h"

#include "wksp_module.h"

#include "wksp_data_manager.h"
#include "wksp_data_layers.h"
#include "wksp_map_buttons.h"

#include "wksp_layer.h"
#include "wksp_map_layer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// #define ACTIVE_SHOW_ALL_PAGES


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_PARAMETERS	= 0,
	IMG_DESCRIPTION,
	IMG_HISTORY,
	IMG_ATTRIBUTES,
	IMG_LEGEND
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE	*g_pACTIVE	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CACTIVE, wxNotebook)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CACTIVE::CACTIVE(wxWindow *pParent)
	: wxNotebook(pParent, ID_WND_ACTIVE, wxDefaultPosition, wxDefaultSize, NOTEBOOK_STYLE, _TL("Object Properties"))
{
	g_pACTIVE		= this;

	m_pItem			= NULL;
	m_pLayer		= NULL;
	m_pObject		= NULL;

	//-----------------------------------------------------
	AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));

	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_PARAMETERS);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_DESCRIPTION);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_HISTORY);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_ATTRIBUTES);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_LEGEND);

	//-----------------------------------------------------
	m_pParameters	= new CACTIVE_Parameters	(this);	m_pParameters	->SetName(_TL("Settings"));
	m_pDescription	= new CACTIVE_Description	(this);	m_pDescription	->SetName(_TL("Description"));
	m_pHistory		= new CACTIVE_History		(this);	m_pHistory		->SetName(_TL("History"));
	m_pLegend		= new CACTIVE_Legend		(this);	m_pLegend		->SetName(_TL("Legend"));
	m_pAttributes	= new CACTIVE_Attributes	(this);	m_pAttributes	->SetName(_TL("Attributes"));

#if defined(_SAGA_MSW)
	m_pParameters	->Hide();
	m_pDescription	->Hide();
	m_pHistory		->Hide();
	m_pLegend		->Hide();
	m_pAttributes	->Hide();
#endif
}

//---------------------------------------------------------
void CACTIVE::Add_Pages(void)
{
	_Show_Page(m_pParameters);
	_Show_Page(m_pDescription);
#ifdef ACTIVE_SHOW_ALL_PAGES
	_Show_Page(m_pHistory);
	_Show_Page(m_pLegend);
	_Show_Page(m_pAttributes);
#endif
}

//---------------------------------------------------------
CACTIVE::~CACTIVE(void)
{
	g_pACTIVE		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE::Set_Active(CWKSP_Base_Item *pItem)
{
	if( pItem == m_pItem )
	{
		return( true );
	}

	if( g_pData_Source )
	{
		g_pData_Source->Set_Data_Source(pItem);
	}

	//-----------------------------------------------------
	CWKSP_Base_Item	*pLegend, *pHistory;

	m_pItem		= pItem;
	m_pLayer	= NULL;
	pLegend		= NULL;
	pHistory	= NULL;

	STATUSBAR_Set_Text(SG_T(""), STATUSBAR_VIEW_X);
	STATUSBAR_Set_Text(SG_T(""), STATUSBAR_VIEW_Y);
	STATUSBAR_Set_Text(SG_T(""), STATUSBAR_VIEW_Z);

	if( m_pParameters )
	{
		m_pParameters->Set_Parameters(m_pItem);
	}

	Update_Description();

	if( m_pItem == NULL )
	{
		STATUSBAR_Set_Text(wxT(""), STATUSBAR_ACTIVE);

		return( true );
	}

	STATUSBAR_Set_Text(m_pItem->Get_Name(), STATUSBAR_ACTIVE);

	//-----------------------------------------------------
	switch( m_pItem->Get_Type() )
	{
	default:
		break;

	case WKSP_ITEM_Map:
		pLegend		= m_pItem;
		break;

	case WKSP_ITEM_Map_Layer:
		pLegend		= pHistory	= m_pLayer	= ((CWKSP_Map_Layer *)m_pItem)->Get_Layer();
		break;

	case WKSP_ITEM_Table:
		pHistory	= m_pItem;
		break;

	case WKSP_ITEM_Shapes:
	case WKSP_ITEM_TIN:
	case WKSP_ITEM_PointCloud:
	case WKSP_ITEM_Grid:
		pLegend		= pHistory	= m_pLayer	= (CWKSP_Layer *)m_pItem;
		break;
	}

	//-----------------------------------------------------
	if( pLegend )
	{
		m_pLegend->Set_Item(pLegend);

		_Show_Page(m_pLegend);
	}
	else
	{
		_Hide_Page(m_pLegend);
	}

	if( pHistory )
	{
		m_pHistory->Set_Item(pHistory);

		_Show_Page(m_pHistory);
	}
	else
	{
		_Hide_Page(m_pHistory);
	}

	if( m_pLayer )
	{
		m_pAttributes->Set_Layer(m_pLayer);

		_Show_Page(m_pAttributes);
	}
	else
	{
		_Hide_Page(m_pAttributes);
	}

	//-----------------------------------------------------
	if( g_pData_Buttons )
	{
		g_pData_Buttons->Refresh(false);
	}

	if( g_pMap_Buttons )
	{
		g_pMap_Buttons->Refresh(false);
	}

	//-----------------------------------------------------
	if( m_pLayer )
	{
		m_pObject	= m_pLayer->Get_Object();

		if( SG_Get_Data_Manager().Exists(m_pObject) && m_pObject->Get_ObjectType() == DATAOBJECT_TYPE_Shapes && ((CSG_Shapes *)m_pObject)->Get_Selection_Count() > 0 )
		{
			g_pData->Update_Views(m_pObject);
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
bool CACTIVE::_Show_Page(wxWindow *pPage)
{
	int		Image_ID	= -1;

	//-----------------------------------------------------
	if( pPage == m_pParameters )	Image_ID	= IMG_PARAMETERS;
	if( pPage == m_pDescription )	Image_ID	= IMG_DESCRIPTION;
	if( pPage == m_pHistory )		Image_ID	= IMG_HISTORY;
	if( pPage == m_pLegend )		Image_ID	= IMG_LEGEND;
	if( pPage == m_pAttributes )	Image_ID	= IMG_ATTRIBUTES;

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
bool CACTIVE::_Hide_Page(wxWindow *pPage)
{
#ifndef ACTIVE_SHOW_ALL_PAGES
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
#endif

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE::Update_Description(void)
{
	if( m_pDescription == NULL )
	{
		return( false );
	}

	wxString	Description;

	//-----------------------------------------------------
	if( m_pItem == NULL )
	{
		Description	= _TL("No description available");
	}
	else switch( m_pItem->Get_Type() )
	{
	default:
		Description	= m_pItem->Get_Description();
		break;

	case WKSP_ITEM_Module:
		{
			wxFileName	FileName;

			FileName.Assign		(((CWKSP_Module *)m_pItem)->Get_File_Name());
			FileName.AppendDir	(FileName.GetName());
			FileName.SetName	(wxString::Format(wxT("%s_%02d"), FileName.GetName().c_str(), m_pItem->Get_Index()));

			FileName.SetExt		(wxT("html"));

			if( FileName.FileExists() && m_pDescription->LoadPage(FileName.GetFullPath()) )
			{
				return( true );
			}

			FileName.SetExt		(wxT("htm"));

			if( FileName.FileExists() && m_pDescription->LoadPage(FileName.GetFullPath()) )
			{
				return( true );
			}

			Description	= m_pItem->Get_Description();
		}
		break;
	}

	//-----------------------------------------------------
	if( Description.Length() <= 8192 )
	{
		Description.Replace(wxT("\n"), wxT("<br>"));
	}

	m_pDescription->SetPage(Description);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE::Update(CWKSP_Base_Item *pItem, bool bSave)
{
	if( m_pItem && m_pItem == pItem )
	{
		if( m_pParameters )
		{
			m_pParameters->Update_Parameters(pItem->Get_Parameters(), bSave);
		}

		if( !bSave )
		{
			Update_Description();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CACTIVE::Update_DataObjects(void)
{
	if( m_pParameters )
	{
		m_pParameters->Update_DataObjects();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
