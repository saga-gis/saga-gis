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
	: wxNotebook(pParent, ID_WND_ACTIVE, wxDefaultPosition, wxDefaultSize, wxNB_TOP|wxNB_MULTILINE, _TL("Properties"))
{
	g_pACTIVE		= this;

	m_pItem			= NULL;

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
CWKSP_Data_Item * CACTIVE::Get_Active_Data_Item(void)
{
	if( m_pItem && m_pItem->GetId().IsOk() )
	{
		switch( m_pItem->Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Map_Layer:
			return( ((CWKSP_Map_Layer *)m_pItem)->Get_Layer() );

		case WKSP_ITEM_Table:
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_PointCloud:
		case WKSP_ITEM_Shapes:
		case WKSP_ITEM_Grid:
			return( (CWKSP_Data_Item *)m_pItem );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Layer * CACTIVE::Get_Active_Layer(void)
{
	CWKSP_Data_Item	*pItem	= Get_Active_Data_Item();

	if( pItem && pItem->Get_Type() != WKSP_ITEM_Table )
	{
		return( (CWKSP_Layer *)pItem );
	}

	return( NULL );
}

//---------------------------------------------------------
CWKSP_Map * CACTIVE::Get_Active_Map(void)
{
	if( m_pItem && m_pItem->GetId().IsOk() && m_pItem->Get_Type() == WKSP_ITEM_Map )
	{
		return( (CWKSP_Map *)m_pItem );
	}

	return( NULL );
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
		STATUSBAR_Set_Text(SG_T(""), STATUSBAR_ACTIVE);

		if( g_pSAGA_Frame   )	g_pSAGA_Frame->Set_Pane_Caption(this, _TL("Properties"));

		if( g_pData_Buttons )	g_pData_Buttons->Refresh();
		if( g_pMap_Buttons  )	g_pMap_Buttons ->Refresh();

		_Hide_Page(m_pHistory);
		_Hide_Page(m_pLegend);
		_Hide_Page(m_pAttributes);

		SendSizeEvent();

		return( true );
	}

	//-----------------------------------------------------
	STATUSBAR_Set_Text(m_pItem->Get_Name(), STATUSBAR_ACTIVE);

	if( g_pSAGA_Frame )	g_pSAGA_Frame->Set_Pane_Caption(this, wxString(_TL("Properties")) + ": " + m_pItem->Get_Name());

	//-----------------------------------------------------
	if( Get_Active_Data_Item() )
	{	_Show_Page(m_pHistory   );	}	else	{	_Hide_Page(m_pHistory   );	}

	if( Get_Active_Layer() || Get_Active_Map() )
	{	_Show_Page(m_pLegend    );	}	else	{	_Hide_Page(m_pLegend    );	}

	if( Get_Active_Layer() )
	{	_Show_Page(m_pAttributes);	}	else	{	_Hide_Page(m_pAttributes);	}

	//-----------------------------------------------------
	if( g_pData_Buttons )	g_pData_Buttons->Refresh(false);
	if( g_pMap_Buttons  )	g_pMap_Buttons ->Refresh(false);

	if( g_pData_Source  )	g_pData_Source->Set_Data_Source(m_pItem);

	//-----------------------------------------------------
	CSG_Data_Object	*pObject	= Get_Active_Data_Item() ? Get_Active_Data_Item()->Get_Object() : NULL;

	if( SG_Get_Data_Manager().Exists(pObject) &&
	(	(pObject->Get_ObjectType() == DATAOBJECT_TYPE_Table      && ((CSG_Table      *)pObject)->Get_Selection_Count() > 0)
	||	(pObject->Get_ObjectType() == DATAOBJECT_TYPE_TIN        && ((CSG_Shapes     *)pObject)->Get_Selection_Count() > 0)
	||	(pObject->Get_ObjectType() == DATAOBJECT_TYPE_PointCloud && ((CSG_PointCloud *)pObject)->Get_Selection_Count() > 0)
	||	(pObject->Get_ObjectType() == DATAOBJECT_TYPE_Shapes     && ((CSG_Shapes     *)pObject)->Get_Selection_Count() > 0)) )
	{
		g_pData->Update_Views(pObject);
	}

	SendSizeEvent();

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
	if( pPage == m_pParameters  )	Image_ID	= IMG_PARAMETERS;
	if( pPage == m_pDescription )	Image_ID	= IMG_DESCRIPTION;
	if( pPage == m_pHistory     )	Image_ID	= IMG_HISTORY;
	if( pPage == m_pLegend      )	Image_ID	= IMG_LEGEND;
	if( pPage == m_pAttributes  )	Image_ID	= IMG_ATTRIBUTES;

	//-----------------------------------------------------
	if( pPage == m_pHistory     )	m_pHistory   ->Set_Item(Get_Active_Data_Item());
	if( pPage == m_pLegend      )	m_pLegend    ->Set_Item(Get_Active_Layer() ? Get_Active_Layer() : Get_Active_Map() ? m_pItem : NULL);
	if( pPage == m_pAttributes  )	m_pAttributes->Set_Item(Get_Active_Layer());

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
#ifdef ACTIVE_SHOW_ALL_PAGES
	return( true );
#endif

	//-----------------------------------------------------
	if( pPage == m_pHistory     )	m_pHistory   ->Set_Item(NULL);
	if( pPage == m_pLegend      )	m_pLegend    ->Set_Item(NULL);
	if( pPage == m_pAttributes  )	m_pAttributes->Set_Item(NULL);

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
		if( !((CWKSP_Module *)m_pItem)->Get_File_Name().IsEmpty() )
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
		}

		Description	= m_pItem->Get_Description();

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
