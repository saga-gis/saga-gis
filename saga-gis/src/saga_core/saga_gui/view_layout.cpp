
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
//                   VIEW_Layout.cpp                     //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "helper.h"

#include "wksp_map.h"

#include "view_ruler.h"

#include "view_layout.h"
#include "view_layout_info.h"
#include "view_layout_control.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Layout, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Layout, CVIEW_Base)
	EVT_SIZE			(CVIEW_Layout::On_Size)

	EVT_MENU			(ID_CMD_LAYOUT_PRINT_SETUP		, CVIEW_Layout::On_Print_Setup)
	EVT_MENU			(ID_CMD_LAYOUT_PAGE_SETUP		, CVIEW_Layout::On_Page_Setup)
	EVT_MENU			(ID_CMD_LAYOUT_PRINT			, CVIEW_Layout::On_Print)
	EVT_MENU			(ID_CMD_LAYOUT_PRINT_PREVIEW	, CVIEW_Layout::On_Print_Preview)
	EVT_MENU			(ID_CMD_LAYOUT_FIT_SCALE		, CVIEW_Layout::On_Fit_Scale)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Layout::CVIEW_Layout(CVIEW_Layout_Info *pInfo)
	: CVIEW_Base(ID_VIEW_LAYOUT, LNG("[CAP] Map-Layout"), ID_IMG_WND_LAYOUT, CVIEW_Layout::_Create_Menu(), LNG("[CAP] Map-Layout"))
{
	SYS_Set_Color_BG(this, wxSYS_COLOUR_3DFACE);

	m_pInfo		= pInfo;

	m_pRuler_X	= new CVIEW_Ruler(this, RULER_HORIZONTAL|RULER_EDGE_SUNKEN);
	m_pRuler_Y	= new CVIEW_Ruler(this, RULER_VERTICAL  |RULER_EDGE_SUNKEN);

	m_pControl	= new CVIEW_Layout_Control(this);

	SetTitle(wxString::Format(wxT("%s - %s"), pInfo->Get_Map()->Get_Name().c_str(), LNG("[CAP] Map-Layout")));
}

//---------------------------------------------------------
CVIEW_Layout::~CVIEW_Layout(void)
{
	m_pInfo->Get_Map()->View_Closes(this);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Layout::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_FIT_SCALE);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_PAGE_SETUP);
//	CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_PRINT_SETUP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_PRINT_PREVIEW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_LAYOUT_PRINT);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Layout::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_LAYOUT);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PAGE_SETUP);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PRINT_SETUP);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PRINT_PREVIEW);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_LAYOUT_PRINT);

	CMD_ToolBar_Add(pToolBar, LNG("[CAP] Map-Layout"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::On_Size(wxSizeEvent &event)
{
	int		A, B, d, dX, dY;

	A	= 1;
	B	= 20;
	d	= B - 4 * A;
	dX	= GetClientSize().x - B;
	dY	= GetClientSize().y - B;

	Freeze();

	m_pRuler_Y->SetSize(wxRect(A, B, d , dY));
	m_pRuler_X->SetSize(wxRect(B, A, dX, d ));
	m_pControl->SetSize(wxRect(B, B, dX, dY));
	m_pControl->Fit_To_Size(dX, dY);

	Thaw();

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::Ruler_Set_Position(int x, int y)
{
	m_pRuler_X->Set_Position(x);
	m_pRuler_Y->Set_Position(y);
}

//---------------------------------------------------------
void CVIEW_Layout::Ruler_Refresh(double xMin, double xMax, double yMin, double yMax)
{
	wxRect	r(m_pInfo->Get_Margins());

	m_pRuler_X->Set_Range_Core(r.GetLeft(), r.GetLeft() + r.GetWidth());
	m_pRuler_Y->Set_Range_Core(r.GetTop (), r.GetTop () + r.GetHeight());

	m_pRuler_X->Set_Range(xMin, xMax);
	m_pRuler_Y->Set_Range(yMin, yMax);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Layout::Refresh_Layout(void)
{
	return( m_pControl->Refresh_Layout() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Layout::On_Print_Setup(wxCommandEvent &event)
{
	if( m_pInfo->Setup_Print() )
	{
		m_pControl->Set_Dimensions();
	}
}

//---------------------------------------------------------
void CVIEW_Layout::On_Page_Setup(wxCommandEvent &event)
{
	if( m_pInfo->Setup_Page() )
	{
		m_pControl->Set_Dimensions();
	}
}

//---------------------------------------------------------
void CVIEW_Layout::On_Print(wxCommandEvent &event)
{
	m_pInfo->Print();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Print_Preview(wxCommandEvent &event)
{
	m_pInfo->Print_Preview();
}

//---------------------------------------------------------
void CVIEW_Layout::On_Fit_Scale(wxCommandEvent &event)
{
	m_pInfo->Fit_Scale();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
