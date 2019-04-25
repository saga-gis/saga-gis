
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
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>

#include <saga_api/saga_api.h>

#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "data_source.h"

#include "saga_frame.h"

#include "active.h"
#include "active_parameters.h"
#include "active_description.h"
#include "active_history.h"
#include "active_legend.h"
#include "active_attributes.h"
#include "active_info.h"

#include "wksp_tool.h"

#include "wksp_data_manager.h"
#include "wksp_data_layers.h"
#include "wksp_map_buttons.h"

#include "wksp_layer.h"
#include "wksp_shapes.h"
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
	IMG_LEGEND,
	IMG_ATTRIBUTES,
	IMG_INFO
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive	*g_pActive	= NULL;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive, wxNotebook)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive::CActive(wxWindow *pParent)
	: wxNotebook(pParent, ID_WND_ACTIVE, wxDefaultPosition, wxDefaultSize, wxNB_TOP|wxNB_MULTILINE, _TL("Properties"))
{
	g_pActive		= this;

	m_pItem			= NULL;

	//-----------------------------------------------------
	AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));

	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_PARAMETERS );
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_DESCRIPTION);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_HISTORY    );
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_LEGEND     );
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_ATTRIBUTES );
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_INFO       );

	//-----------------------------------------------------
	m_pParameters	= new CActive_Parameters (this); m_pParameters ->SetName(_TL("Settings"   ));
	m_pDescription	= new CActive_Description(this); m_pDescription->SetName(_TL("Description"));
	m_pHistory		= new CActive_History    (this); m_pHistory    ->SetName(_TL("History"    ));
	m_pLegend		= new CActive_Legend     (this); m_pLegend     ->SetName(_TL("Legend"     ));
	m_pAttributes	= new CActive_Attributes (this); m_pAttributes ->SetName(_TL("Attributes" ));
	m_pInfo			= new CActive_Info       (this); m_pInfo       ->SetName(_TL("Information"));

#if defined(_SAGA_MSW)
	m_pParameters ->Hide();
	m_pDescription->Hide();
	m_pHistory    ->Hide();
	m_pLegend     ->Hide();
	m_pAttributes ->Hide();
	m_pInfo       ->Hide();
#endif
}

//---------------------------------------------------------
void CActive::Add_Pages(void)
{
	_Show_Page(m_pParameters );
	_Show_Page(m_pDescription);
#ifdef ACTIVE_SHOW_ALL_PAGES
	_Show_Page(m_pHistory    );
	_Show_Page(m_pLegend     );
	_Show_Page(m_pAttributes );
	_Show_Page(m_pInfo       );
#endif
}

//---------------------------------------------------------
CActive::~CActive(void)
{
	g_pActive	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Data_Item * CActive::Get_Active_Data_Item(void)
{
	if( m_pItem && m_pItem->GetId().IsOk() )
	{
		switch( m_pItem->Get_Type() )
		{
		case WKSP_ITEM_Table     :
		case WKSP_ITEM_Shapes    :
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_TIN       :
		case WKSP_ITEM_Grid      :
		case WKSP_ITEM_Grids     : return( (CWKSP_Data_Item  *)m_pItem );

		case WKSP_ITEM_Map_Layer : return( ((CWKSP_Map_Layer *)m_pItem)->Get_Layer() );

		default: break;
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Layer * CActive::Get_Active_Layer(void)
{
	CWKSP_Data_Item	*pItem	= Get_Active_Data_Item();

	if( pItem && pItem->Get_Type() != WKSP_ITEM_Table )
	{
		return( (CWKSP_Layer *)pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Shapes * CActive::Get_Active_Shapes(bool bWithInfo)
{
	CWKSP_Layer	*pLayer	= Get_Active_Layer();

	if( pLayer && pLayer->Get_Type() == WKSP_ITEM_Shapes )
	{
		CWKSP_Shapes	*pShapes	= (CWKSP_Shapes *)pLayer;

		if( !bWithInfo || pShapes->Get_Field_Info() >= 0 )
		{
			return( pShapes );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Map * CActive::Get_Active_Map(void)
{
	if( m_pItem && m_pItem->GetId().IsOk() && m_pItem->Get_Type() == WKSP_ITEM_Map )
	{
		return( (CWKSP_Map *)m_pItem );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CActive::Set_Active(CWKSP_Base_Item *pItem)
{
	if( pItem == m_pItem )
	{
		return( true );
	}

	//-----------------------------------------------------
	m_pItem		= pItem;

	if( m_pParameters )	m_pParameters->Set_Parameters(m_pItem);

	Update_Description();

	STATUSBAR_Set_Text(SG_T(""), STATUSBAR_VIEW_X);
	STATUSBAR_Set_Text(SG_T(""), STATUSBAR_VIEW_Y);
	STATUSBAR_Set_Text(SG_T(""), STATUSBAR_VIEW_Z);

	//-----------------------------------------------------
	if( m_pItem == NULL )
	{
		if( g_pSAGA_Frame   )	g_pSAGA_Frame->Set_Pane_Caption(this, _TL("Properties"));

		if( g_pData_Buttons )	g_pData_Buttons->Refresh();
		if( g_pMap_Buttons  )	g_pMap_Buttons ->Refresh();

		_Hide_Page(m_pHistory   );
		_Hide_Page(m_pLegend    );
		_Hide_Page(m_pAttributes);
		_Hide_Page(m_pInfo      );

		SendSizeEvent();

		return( true );
	}

	//-----------------------------------------------------
	if( g_pSAGA_Frame )	g_pSAGA_Frame->Set_Pane_Caption(this, wxString(_TL("Properties")) + ": " + m_pItem->Get_Name());

	//-----------------------------------------------------
	_Show_Page(m_pHistory   , Get_Active_Data_Item () != NULL);
	_Show_Page(m_pLegend    , Get_Active_Layer     () != NULL || Get_Active_Map() != NULL);
	_Show_Page(m_pAttributes, Get_Active_Layer     () != NULL);
	_Show_Page(m_pInfo      , Get_Active_Shapes(true) != NULL);

	//-----------------------------------------------------
	if( g_pData_Buttons )	g_pData_Buttons->Refresh(false);
	if( g_pMap_Buttons  )	g_pMap_Buttons ->Refresh(false);

	if( g_pData_Source  )	g_pData_Source->Set_Data_Source(m_pItem);

	//-----------------------------------------------------
	CSG_Data_Object	*pObject	= Get_Active_Data_Item() ? Get_Active_Data_Item()->Get_Object() : NULL;

	if( SG_Get_Data_Manager().Exists(pObject) &&
	(	(pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table      && ((CSG_Table      *)pObject)->Get_Selection_Count() > 0)
	||	(pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN        && ((CSG_Shapes     *)pObject)->Get_Selection_Count() > 0)
	||	(pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud && ((CSG_PointCloud *)pObject)->Get_Selection_Count() > 0)
	||	(pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes     && ((CSG_Shapes     *)pObject)->Get_Selection_Count() > 0)) )
	{
		g_pData->Update_Views(pObject);
	}

	SendSizeEvent();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CActive::_Show_Page(wxWindow *pPage, bool bShow)
{
	return( bShow ? _Show_Page(pPage) : _Hide_Page(pPage) );
}

//---------------------------------------------------------
bool CActive::_Show_Page(wxWindow *pPage)
{
	int		Image_ID	= -1;

	//-----------------------------------------------------
	if( pPage == m_pParameters  )	Image_ID	= IMG_PARAMETERS;
	if( pPage == m_pDescription )	Image_ID	= IMG_DESCRIPTION;
	if( pPage == m_pHistory     )	Image_ID	= IMG_HISTORY;
	if( pPage == m_pLegend      )	Image_ID	= IMG_LEGEND;
	if( pPage == m_pAttributes  )	Image_ID	= IMG_ATTRIBUTES;
	if( pPage == m_pInfo        )	Image_ID	= IMG_INFO;

	//-----------------------------------------------------
	if( pPage == m_pHistory     )	m_pHistory   ->Set_Item(Get_Active_Data_Item());
	if( pPage == m_pLegend      )	m_pLegend    ->Set_Item(Get_Active_Layer() ? Get_Active_Layer() : Get_Active_Map() ? m_pItem : NULL);
	if( pPage == m_pAttributes  )	m_pAttributes->Set_Item(Get_Active_Layer());
	if( pPage == m_pInfo        )	m_pInfo      ->Set_Item(Get_Active_Layer());

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
bool CActive::_Hide_Page(wxWindow *pPage)
{
#ifdef ACTIVE_SHOW_ALL_PAGES
	return( true );
#endif

	//-----------------------------------------------------
	if( pPage == m_pHistory     )	m_pHistory   ->Set_Item(NULL);
	if( pPage == m_pLegend      )	m_pLegend    ->Set_Item(NULL);
	if( pPage == m_pAttributes  )	m_pAttributes->Set_Item(NULL);
	if( pPage == m_pInfo        )	m_pInfo      ->Set_Item(NULL);

	//-----------------------------------------------------
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
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CActive::Update(CWKSP_Base_Item *pItem, bool bSave)
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

			m_pHistory->Set_Item(pItem);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CActive::Update_DataObjects(void)
{
	if( m_pParameters )
	{
		m_pParameters->Update_DataObjects();
	}

	return( true );
}

//---------------------------------------------------------
bool CActive::Update_Description(void)
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
	else
	{
		Description	= m_pItem->Get_Description();
	}

	//-----------------------------------------------------
	if( Description.Length() <= 8192 )
	{
		Description.Replace("\n", "<br>");
	}

	m_pDescription->SetPage(Description);

	return( true );
}

//---------------------------------------------------------
bool CActive::Update_Attributes(bool bSave)
{
	if( bSave )
	{
		m_pAttributes->Save_Changes(true);
	}
	else
	{
		m_pAttributes->Set_Attributes();
	}

	return( true );
}

//---------------------------------------------------------
bool CActive::Update_Info(void)
{
	_Show_Page(m_pInfo, Get_Active_Shapes(true) != NULL);

//	m_pInfo->Set_Info();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
