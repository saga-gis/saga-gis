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
//                  VIEW_Histogram.cpp                   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>
#include <wx/toolbar.h>
#include <wx/clipbrd.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "helper.h"
#include "dc_helper.h"
#include "res_dialogs.h"

#include "active.h"

#include "wksp_data_manager.h"
#include "wksp_layer_classify.h"
#include "wksp_grid.h"
#include "wksp_shapes.h"
#include "wksp_pointcloud.h"

#include "view_histogram.h"

//---------------------------------------------------------
#define IS_BAND_WISE_FIT(pLayer)	(m_pLayer->Get_Type() == WKSP_ITEM_Grids && m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_OVERLAY && m_pLayer->Get_Parameter("OVERLAY_STATISTICS")->asInt() != 0)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Histogram, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Histogram, CVIEW_Base)
	EVT_PAINT		(CVIEW_Histogram::On_Paint)
	EVT_SIZE		(CVIEW_Histogram::On_Size)

	EVT_MOTION		(CVIEW_Histogram::On_Mouse_Motion)
	EVT_LEFT_DOWN	(CVIEW_Histogram::On_Mouse_LDown)
	EVT_LEFT_UP		(CVIEW_Histogram::On_Mouse_LUp)
	EVT_RIGHT_DOWN	(CVIEW_Histogram::On_Mouse_RDown)

	EVT_MENU		(ID_CMD_HISTOGRAM_CUMULATIVE  , CVIEW_Histogram::On_Cumulative)
	EVT_MENU		(ID_CMD_HISTOGRAM_CLASS_COUNT , CVIEW_Histogram::On_ClassCount)
	EVT_MENU		(ID_CMD_HISTOGRAM_AS_TABLE    , CVIEW_Histogram::On_AsTable)
	EVT_MENU		(ID_CMD_HISTOGRAM_TO_CLIPBOARD, CVIEW_Histogram::On_ToClipboard)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Histogram::CVIEW_Histogram(CWKSP_Layer *pLayer)
	: CVIEW_Base(pLayer, ID_VIEW_HISTOGRAM, pLayer->Get_Name(), ID_IMG_WND_HISTOGRAM)
{
	SYS_Set_Color_BG_Window(this);

	m_pLayer		= pLayer;

	m_bCumulative	= false;
	m_bMouse_Down	= false;

	Do_Update();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Histogram::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_HISTOGRAM_CUMULATIVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_HISTOGRAM_CLASS_COUNT);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_HISTOGRAM_AS_TABLE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_HISTOGRAM_TO_CLIPBOARD);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Histogram::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_HISTOGRAM);

	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_HISTOGRAM_CUMULATIVE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_HISTOGRAM_AS_TABLE);
//	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_HISTOGRAM_TO_CLIPBOARD);

	CMD_ToolBar_Add(pToolBar, _TL("Histogram"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram::Do_Update(void)
{
	if( IS_BAND_WISE_FIT(pLayer) )
	{
		m_pLayer->Get_Classifier()->Set_Metric(0, 1, m_pLayer->Get_Value_Minimum(), m_pLayer->Get_Value_Maximum());
	}

	if( m_pLayer->Get_Classifier()->Histogram_Update() )
	{
		Refresh();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram::Draw(wxDC &dc, wxRect r)
{
	wxFont	Font;
	Font.SetFamily(wxFONTFAMILY_SWISS);
	dc.SetFont(Font);

	r	= Draw_Get_rDiagram(r);

	Draw_Histogram(dc, r);
	Draw_Frame    (dc, r);
}

//---------------------------------------------------------
void CVIEW_Histogram::Draw_Histogram(wxDC &dc, wxRect r)
{
	const CSG_Histogram	&Histogram	= m_pLayer->Get_Classifier()->Histogram_Get();

	if( Histogram.Get_Class_Count() > 0 && Histogram.Get_Element_Count() > 0 )
	{
		int		ax, ay, bx, by;
		double	dx, Value;

		wxColor	Color	= SYS_Get_Color(wxSYS_COLOUR_ACTIVECAPTION); // wxSYS_COLOUR_BTNSHADOW);

		dx	= (double)r.GetWidth() / (double)Histogram.Get_Class_Count();
		ay	= r.GetBottom();
		bx	= r.GetLeft();

		for(size_t iClass=0; iClass<Histogram.Get_Class_Count(); iClass++)
		{
			Value	= m_bCumulative
				? Histogram.Get_Cumulative(iClass) / (double)Histogram.Get_Element_Count  ()
				: Histogram.Get_Elements  (iClass) / (double)Histogram.Get_Element_Maximum();

			ax	= bx;
			bx	= r.GetLeft() + (int)(dx * (iClass + 1.0));
			by	= ay - (int)(r.GetHeight() * Value);

			if( m_pLayer->Get_Classifier()->Get_Mode() != CLASSIFY_OVERLAY )
			{
				Color	= m_pLayer->Get_Classifier()->Get_Class_Color(iClass);
			}

			Draw_FillRect(dc, Color, ax, ay, bx, by);
		}
	}
	else
	{
		Draw_Text(dc, TEXTALIGN_CENTER, r.GetLeft() + r.GetWidth() / 2, r.GetBottom() - r.GetHeight() / 2, _TL("invalid histogram"));
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::Draw_Frame(wxDC &dc, wxRect r)
{
	const int	FontSize	= 12;
	const int	Precision	= 3;

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	const CSG_Histogram	&Histogram	= m_pLayer->Get_Classifier()->Histogram_Get();

	if( Histogram.Get_Class_Count() < 1 || Histogram.Get_Element_Count() < 1 )
	{
		return;
	}

	//-----------------------------------------------------
	int	Maximum	= m_bCumulative
		? Histogram.Get_Element_Count  ()
		: Histogram.Get_Element_Maximum();

	if( Maximum > 0 )
	{
		Draw_Scale(dc, wxRect(r.GetLeft() - 20, r.GetTop(), 20, r.GetHeight()), 0, Maximum, false, false, false);
	}

	//-----------------------------------------------------
	wxFont	Font	= dc.GetFont();
	Font.SetPointSize((int)(0.7 * FontSize));
	dc.SetFont(Font);

	//-----------------------------------------------------
	if( m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_LUT )
	{
		double	dx	= r.GetWidth() / (double)Histogram.Get_Class_Count(), n;

		for(size_t iClass=0; iClass<Histogram.Get_Class_Count(); iClass++, n+=dx)
		{
			if( iClass == 0 || n > (FontSize + 5) )
			{
				n	= 0.0;

				int	ix	= r.GetLeft() + (int)(dx * (0.5 + iClass));
				dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);
				Draw_Text(dc, TEXTALIGN_TOPRIGHT, ix, r.GetBottom() + 7, 45.0,
					m_pLayer->Get_Classifier()->Get_Class_Name(iClass)
				);
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		double	dx	= r.GetWidth() / (double)Histogram.Get_Class_Count();

		if( dx < (FontSize + 5) )
		{
			dx	*= 1 + (int)((FontSize + 5) / dx);
		}

		double	dz	= dx / (double)r.GetWidth();

		for(int i=0, n=(int)(r.GetWidth()/dx); i<=n; i++)
		{
			int	ix	= r.GetLeft() + (int)(dx * i);
			dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);
			Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45.0,
				SG_Get_String(m_pLayer->Get_Classifier()->Get_RelativeToMetric(dz * i), -2).c_str()
			);
		}
	}
}

//---------------------------------------------------------
wxRect CVIEW_Histogram::Draw_Get_rDiagram(wxRect r)
{
	if( m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_LUT )
	{
		return(	wxRect(
			wxPoint(r.GetLeft()  + 30, r.GetTop()    +  10),
			wxPoint(r.GetRight() - 10, r.GetBottom() - 100)
		));
	}

	return(	wxRect(
		wxPoint(r.GetLeft()  + 30, r.GetTop()    + 10),
		wxPoint(r.GetRight() - 10, r.GetBottom() - 40)
	));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxRect		r(wxPoint(0, 0), GetClientSize());

	Draw_Edge(dc, EDGE_STYLE_SUNKEN, r);

	Draw(dc, r);
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Size(wxSizeEvent &WXUNUSED(event))
{
	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram::On_Mouse_Motion(wxMouseEvent &event)
{
	if( m_bMouse_Down )
	{
		wxClientDC	dc(this);
		wxRect		r(Draw_Get_rDiagram(wxRect(wxPoint(0, 0), GetClientSize())));
		dc.SetLogicalFunction(wxINVERT);

		dc.DrawRectangle(m_Mouse_Down.x, r.GetTop(), m_Mouse_Move.x - m_Mouse_Down.x, r.GetHeight());
		m_Mouse_Move	= event.GetPosition();
		dc.DrawRectangle(m_Mouse_Down.x, r.GetTop(), m_Mouse_Move.x - m_Mouse_Down.x, r.GetHeight());
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Mouse_LDown(wxMouseEvent &event)
{
	if( IS_BAND_WISE_FIT(pLayer) )
	{
		return;
	}

	switch( m_pLayer->Get_Classifier()->Get_Mode() )
	{
	case CLASSIFY_GRADUATED:
	case CLASSIFY_METRIC   :
	case CLASSIFY_SHADE    :
	case CLASSIFY_OVERLAY  :
		m_bMouse_Down	= true;
		m_Mouse_Move	= m_Mouse_Down	= event.GetPosition();

		CaptureMouse();

	default: break;
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Mouse_LUp(wxMouseEvent &event)
{
	if( m_bMouse_Down )
	{
		ReleaseMouse();

		m_bMouse_Down	= false;
		m_Mouse_Move	= event.GetPosition();

		wxRect	r(Draw_Get_rDiagram(wxRect(wxPoint(0, 0), GetClientSize())));

		CWKSP_Layer_Classify	*pClassify	= m_pLayer->Get_Classifier();

		double	Minimum	= pClassify->Get_RelativeToMetric((double)(m_Mouse_Down.x - r.GetLeft()) / (double)r.GetWidth());
		double	Maximum	= pClassify->Get_RelativeToMetric((double)(m_Mouse_Move.x - r.GetLeft()) / (double)r.GetWidth());

		m_pLayer->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(Minimum, Maximum);
		pClassify->Set_Metric(pClassify->Get_Metric_Mode(), pClassify->Get_Metric_Mode(), Minimum, Maximum);
		g_pACTIVE->Update(m_pLayer, false);
		m_pLayer->Update_Views();

	//	m_pLayer->Set_Color_Range(Minimum, Maximum);
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Mouse_RDown(wxMouseEvent &event)
{
	if( IS_BAND_WISE_FIT(pLayer) )
	{
		return;
	}

	switch( m_pLayer->Get_Classifier()->Get_Mode() )
	{
	case CLASSIFY_GRADUATED:
	case CLASSIFY_METRIC   :
	case CLASSIFY_SHADE    :
	case CLASSIFY_OVERLAY  :
		m_pLayer->Set_Color_Range(
			m_pLayer->Get_Value_Minimum(),
			m_pLayer->Get_Value_Maximum()
		);

	default: break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_HISTOGRAM_CUMULATIVE:
		event.Check(m_bCumulative);
		break;

	case ID_CMD_HISTOGRAM_CLASS_COUNT:
		event.Enable(
			m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_GRADUATED
		||	m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_SHADE
		||	m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_OVERLAY
		);
		break;

	default: break;
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Cumulative(wxCommandEvent &event)
{
	m_bCumulative	= !m_bCumulative;

	Refresh();
}

//---------------------------------------------------------
void CVIEW_Histogram::On_ClassCount(wxCommandEvent &event)
{
	int	nClasses	= m_pLayer->Get_Classifier()->Get_Class_Count();

	if( DLG_Get_Number(nClasses, _TL("Histogram"), _TL("Number of Classes")) && nClasses > 1 )
	{
		m_pLayer->Get_Classifier()->Set_Class_Count(nClasses);

		Refresh();
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_AsTable(wxCommandEvent &event)
{
	CWKSP_Layer_Classify	*pClassifier	= ((CWKSP_Layer *)m_pOwner)->Get_Classifier();

	if( pClassifier->Get_Class_Count() > 0 )
	{
		CSG_Data_Object	*pObject	= ((CWKSP_Layer *)m_pOwner)->Get_Object();

		CSG_Table	*pTable	= new CSG_Table;

		pTable->Set_Name("%s: %s", _TL("Histogram"), pObject->Get_Name());

		pTable->Add_Field(_TL("CLASS" ), SG_DATATYPE_Int   );
		pTable->Add_Field(_TL("AREA"  ), SG_DATATYPE_Double);
		pTable->Add_Field(_TL("COUNT" ), SG_DATATYPE_Long  );
		pTable->Add_Field(_TL("CUMUL" ), SG_DATATYPE_Long  );
		pTable->Add_Field(_TL("NAME"  ), SG_DATATYPE_String);
		pTable->Add_Field(_TL("MIN"   ), SG_DATATYPE_Double);
		pTable->Add_Field(_TL("CENTER"), SG_DATATYPE_Double);
		pTable->Add_Field(_TL("MAX"   ), SG_DATATYPE_Double);

		double	dArea	= pObject->asGrid() != NULL ? pObject->asGrid()->Get_Cellarea() : 1.0;

		for(int i=0; i<pClassifier->Get_Class_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, i + 1);
			pRecord->Set_Value(1, pClassifier->Histogram_Get().Get_Elements  (i) * dArea);
			pRecord->Set_Value(2, pClassifier->Histogram_Get().Get_Elements  (i));
			pRecord->Set_Value(3, pClassifier->Histogram_Get().Get_Cumulative(i));
			pRecord->Set_Value(4, pClassifier->Get_Class_Name                (i).wx_str());
			pRecord->Set_Value(5, pClassifier->Get_Class_Value_Minimum       (i));
			pRecord->Set_Value(6, pClassifier->Get_Class_Value_Center        (i));
			pRecord->Set_Value(7, pClassifier->Get_Class_Value_Maximum       (i));
		}

		g_pData->Add(pTable);
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_ToClipboard(wxCommandEvent &event)
{
	wxBitmap	BMP(GetSize());
	wxMemoryDC	dc;
	
	dc.SelectObject(BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	Draw(dc, wxRect(BMP.GetSize()));

	dc.SelectObject(wxNullBitmap);

	if( wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
		pBMP->SetBitmap(BMP);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
