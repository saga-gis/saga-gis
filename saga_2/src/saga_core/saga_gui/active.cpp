
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

#include "active.h"
#include "active_parameters.h"
#include "active_description.h"
#include "active_attributes.h"
#include "active_legend.h"
#include "active_HTMLExtraInfo.h"

#include "wksp_module.h"

#include "wksp_data_layers.h"

#include "wksp_layer.h"
#include "wksp_map_layer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//#define ACTIVE_SHOW_ALL_PAGES


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
	IMG_LEGEND,
	IMG_ATTRIBUTES,
	IMG_HTMLEXTRAINFO
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
	: wxNotebook(pParent, ID_WND_ACTIVE, wxDefaultPosition, wxDefaultSize, NOTEBOOK_STYLE, LNG("[CAP] Object Properties"))
{
	g_pACTIVE		= this;

	m_pItem			= NULL;
	m_pLayer		= NULL;

	//-----------------------------------------------------
	AssignImageList(new wxImageList(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK, true, 0));

	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_PARAMETERS);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_DESCRIPTION);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_ATTRIBUTES);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_LEGEND);
	IMG_ADD_TO_NOTEBOOK(ID_IMG_NB_ACTIVE_HTMLEXTRAINFO);

	//-----------------------------------------------------
	m_pParameters		= NULL;
	m_pDescription		= NULL;
	m_pLegend			= NULL;
	m_pAttributes		= NULL;
	m_pHTMLExtraInfo	= NULL;
}

//---------------------------------------------------------
void CACTIVE::Add_Pages(void)
{
	_Add_Page(IMG_PARAMETERS);
	_Add_Page(IMG_DESCRIPTION);
#ifdef ACTIVE_SHOW_ALL_PAGES
	_Add_Page(IMG_LEGEND);
	_Add_Page(IMG_ATTRIBUTES);
	_Add_Page(IMG_HTMLEXTRAINFO);
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

	//-----------------------------------------------------
	CWKSP_Base_Item	*pLegend, *pHTML;

	m_pLayer	= NULL;
	pLegend		= NULL;
	pHTML		= NULL;

	//-----------------------------------------------------
	if( (m_pItem = pItem) != NULL )
	{
		switch( m_pItem->Get_Type() )
		{
		default:
			break;

		case WKSP_ITEM_Map:
			pLegend		= m_pItem;
			break;

		case WKSP_ITEM_Map_Layer:
			pLegend		= m_pLayer	= ((CWKSP_Map_Layer *)m_pItem)->Get_Layer();
			break;

		case WKSP_ITEM_Shapes:
#ifdef USE_HTMLINFO
			pHTML					= (CWKSP_Layer      *)m_pItem;
#endif
		case WKSP_ITEM_TIN:
		case WKSP_ITEM_Grid:
			pLegend		= m_pLayer	= (CWKSP_Layer      *)m_pItem;
			break;
		}
	}

	_Set_Description();

	//-----------------------------------------------------
	if( m_pParameters )
	{
		m_pParameters->Set_Parameters(m_pItem);
	}

	if( m_pLayer == NULL && m_pAttributes		!= NULL )
	{
#ifndef ACTIVE_SHOW_ALL_PAGES
		_Del_Page(IMG_ATTRIBUTES);
#endif
	}

	if( pLegend  == NULL && m_pLegend			!= NULL )
	{
		m_pLegend->Set_Item(NULL);

#ifndef ACTIVE_SHOW_ALL_PAGES
		_Del_Page(IMG_LEGEND);
#endif
	}

	if( pHTML    == NULL && m_pHTMLExtraInfo	!= NULL )
	{
#ifndef ACTIVE_SHOW_ALL_PAGES
		_Del_Page(IMG_HTMLEXTRAINFO);
#endif
	}

	//-----------------------------------------------------
	if( m_pLayer != NULL )
	{
		_Add_Page(IMG_ATTRIBUTES);

		m_pAttributes->Set_Layer(m_pLayer);
	}

	if( pLegend )
	{
		_Add_Page(IMG_LEGEND);

		m_pLegend->Set_Item(pLegend);
	}

	if( pHTML )
	{
		_Add_Page(IMG_HTMLEXTRAINFO);

		m_pHTMLExtraInfo->SetPage(wxT(""));
	}

	//-----------------------------------------------------
	if( g_pLayers )
	{
		g_pLayers->Refresh(false);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CACTIVE::_Add_Page(int PageID)
{
	wxString	Caption;
	wxWindow	*pPage	= NULL;

	//-----------------------------------------------------
	switch( PageID )
	{
	case IMG_PARAMETERS:
		if( m_pParameters != NULL )
			return( true );

		pPage	= m_pParameters		= new CACTIVE_Parameters	(this);
		Caption	= LNG("[CAP] Settings");
		break;

	case IMG_DESCRIPTION:
		if( m_pDescription != NULL )
			return( true );

		pPage	= m_pDescription	= new CACTIVE_Description	(this);
		Caption	= LNG("[CAP] Description");
		break;

	case IMG_ATTRIBUTES:
		if( m_pAttributes != NULL )
			return( true );

		pPage	= m_pAttributes		= new CACTIVE_Attributes	(this);
		Caption	= LNG("[CAP] Attributes");
		break;

	case IMG_LEGEND:
		if( m_pLegend != NULL )
			return( true );

		pPage	= m_pLegend			= new CACTIVE_Legend	    (this);
		Caption	= LNG("[CAP] Legend");
		break;

	case IMG_HTMLEXTRAINFO:
		if( m_pHTMLExtraInfo != NULL )
			return( true );

		pPage	= m_pHTMLExtraInfo	= new CACTIVE_HTMLExtraInfo (this);
		Caption	= LNG("[CAP] Info");
		break;
	}

	//-----------------------------------------------------
	if( pPage )
	{
		AddPage(pPage, Caption, false, PageID);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CACTIVE::_Del_Page(int PageID)
{
	wxWindow	*pPage	= NULL;

	//-----------------------------------------------------
	switch( PageID )
	{
	case IMG_PARAMETERS:
		pPage				= m_pParameters;
		m_pParameters		= NULL;
		break;

	case IMG_DESCRIPTION:
		pPage				= m_pDescription;
		m_pDescription		= NULL;
		break;

	case IMG_ATTRIBUTES:
		pPage				= m_pAttributes;
		m_pAttributes		= NULL;
		break;

	case IMG_LEGEND:
		pPage				= m_pLegend;
		m_pLegend			= NULL;
		break;

	case IMG_HTMLEXTRAINFO:
		pPage				= m_pHTMLExtraInfo;
		m_pHTMLExtraInfo	= NULL;
		break;
	}

	//-----------------------------------------------------
	if( pPage )
	{
		for(size_t i=0; i<GetPageCount(); i++)
		{
			if( GetPage(i) == pPage )
			{
				DeletePage(i);

				return( true );
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
bool CACTIVE::_Set_Description(void)
{
	if( m_pDescription )
	{
		if( m_pItem != NULL )
		{
			if( m_pItem->Get_Type() == WKSP_ITEM_Module )
			{
				wxFileName	FileName;

				FileName.Assign		(((CWKSP_Module *)m_pItem)->Get_File_Name());
				FileName.AppendDir	(FileName.GetName());
				FileName.SetName	(wxString::Format(wxT("%s_%02d"), FileName.GetName().c_str(), m_pItem->Get_Index()));

				FileName.SetExt		(wxT("html"));

				if( !FileName.FileExists() || !m_pDescription->LoadPage(FileName.GetFullPath()) )
				{
					FileName.SetExt		(wxT("htm"));

					if( !FileName.FileExists() || !m_pDescription->LoadPage(FileName.GetFullPath()) )
					{
						m_pDescription->SetPage(m_pItem->Get_Description());
					}
				}
			}
			else
			{
				m_pDescription->SetPage(m_pItem->Get_Description());
			}

			STATUSBAR_Set_Text(m_pItem->Get_Name(), STATUSBAR_ACTIVE);
		}
		else
		{
			m_pDescription->SetPage(LNG("[TXT] No description available"));

			STATUSBAR_Set_Text(wxT(""), STATUSBAR_ACTIVE);
		}

		return( true );
	}

	STATUSBAR_Set_Text(wxT(""), STATUSBAR_ACTIVE);

	return( false );
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
			_Set_Description();
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
