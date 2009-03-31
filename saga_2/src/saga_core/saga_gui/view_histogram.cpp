
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
#include <wx/toolbar.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp_data_manager.h"
#include "wksp_layer_classify.h"
#include "wksp_grid.h"
#include "wksp_shapes.h"

#include "view_histogram.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Histogram, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Histogram, CVIEW_Base)
	EVT_MENU			(ID_CMD_HISTOGRAM_CUMULATIVE	, CVIEW_Histogram::On_Cumulative)
	EVT_UPDATE_UI		(ID_CMD_HISTOGRAM_CUMULATIVE	, CVIEW_Histogram::On_Cumulative_UI)
	EVT_MENU			(ID_CMD_HISTOGRAM_AS_TABLE		, CVIEW_Histogram::On_AsTable)
END_EVENT_TABLE()

//---------------------------------------------------------
CVIEW_Histogram::CVIEW_Histogram(CWKSP_Layer *pLayer)
	: CVIEW_Base(ID_VIEW_HISTOGRAM, pLayer->Get_Name(), ID_IMG_WND_HISTOGRAM, CVIEW_Histogram::_Create_Menu(), LNG("[CAP] Histogram"))
{
	m_pLayer	= pLayer;
	m_pControl	= new CVIEW_Histogram_Control(this, pLayer);
}

//---------------------------------------------------------
CVIEW_Histogram::~CVIEW_Histogram(void)
{
	m_pLayer->View_Closes(this);
}

//---------------------------------------------------------
wxMenu * CVIEW_Histogram::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu();

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_HISTOGRAM_CUMULATIVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_HISTOGRAM_AS_TABLE);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Histogram::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_HISTOGRAM);

	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_HISTOGRAM_CUMULATIVE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_HISTOGRAM_AS_TABLE);

	CMD_ToolBar_Add(pToolBar, LNG("[CAP] Histogram"));

	return( pToolBar );
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		break;

	case ID_CMD_HISTOGRAM_CUMULATIVE:
		On_Cumulative_UI(event);
		break;
	}
}

//---------------------------------------------------------
void CVIEW_Histogram::On_Cumulative(wxCommandEvent &event)
{
	m_pControl->Set_Cumulative(!m_pControl->Get_Cumulative());
}

void CVIEW_Histogram::On_Cumulative_UI(wxUpdateUIEvent &event)
{
	event.Check(m_pControl->Get_Cumulative());
}

//---------------------------------------------------------
void CVIEW_Histogram::On_AsTable(wxCommandEvent &event)
{
	int					i, n;
	double				dArea	= m_pLayer->Get_Type() == WKSP_ITEM_Grid ? ((CSG_Grid *)m_pLayer->Get_Object())->Get_Cellarea() : 1.0;
	CSG_Table			*pTable;
	CSG_Table_Record	*pRecord;

	if( (n = m_pLayer->Get_Classifier()->Get_Class_Count()) > 0 )
	{
		pTable	= new CSG_Table;

		pTable->Set_Name(wxString::Format(wxT("%s: %s"), LNG("[CAP] Histogram"), m_pLayer->Get_Name().c_str()));

		pTable->Add_Field(LNG("CLASS")	, TABLE_FIELDTYPE_Int);
		pTable->Add_Field(LNG("COUNT")	, TABLE_FIELDTYPE_Int);
		pTable->Add_Field(LNG("AREA")	, TABLE_FIELDTYPE_Double);
		pTable->Add_Field(LNG("NAME")	, TABLE_FIELDTYPE_String);

		for(i=0; i<n; i++)
		{
			pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, i + 1);
			pRecord->Set_Value(1, m_pLayer->Get_Classifier()->Histogram_Get_Count(i, false));
			pRecord->Set_Value(2, m_pLayer->Get_Classifier()->Histogram_Get_Count(i, false) * dArea);
			pRecord->Set_Value(3, m_pLayer->Get_Classifier()->Get_Class_Name(i).c_str());
		}

		g_pData->Add(pTable);
	}
}

//---------------------------------------------------------
bool CVIEW_Histogram::Update_Histogram(void)
{
	return( m_pControl->Update_Histogram() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Histogram_Control, wxScrolledWindow);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Histogram_Control, wxScrolledWindow)
	EVT_PAINT			(CVIEW_Histogram_Control::On_Paint)
	EVT_SIZE			(CVIEW_Histogram_Control::On_Size)
	EVT_MOTION			(CVIEW_Histogram_Control::On_Mouse_Motion)
	EVT_LEFT_DOWN		(CVIEW_Histogram_Control::On_Mouse_LDown)
	EVT_LEFT_UP			(CVIEW_Histogram_Control::On_Mouse_LUp)
	EVT_RIGHT_DOWN		(CVIEW_Histogram_Control::On_Mouse_RDown)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Histogram_Control::CVIEW_Histogram_Control(wxWindow *pParent, CWKSP_Layer *pLayer)
	: wxScrolledWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
	SYS_Set_Color_BG_Window(this);

	m_pLayer		= pLayer;

	m_bCumulative	= false;
	m_bMouse_Down	= false;

	Update_Histogram();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Histogram_Control::Update_Histogram(void)
{
	if( m_pLayer->Get_Classifier()->Histogram_Update() )
	{
		Refresh();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::Set_Cumulative(bool bOn)
{
	if( m_bCumulative != bOn )
	{
		m_bCumulative	= bOn;

		Refresh();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram_Control::On_Mouse_Motion(wxMouseEvent &event)
{
	if( m_bMouse_Down )
	{
		wxClientDC	dc(this);
		wxRect		r(_Draw_Get_rDiagram(wxRect(wxPoint(0, 0), GetClientSize())));
		dc.SetLogicalFunction(wxINVERT);

		dc.DrawRectangle(m_Mouse_Down.x, r.GetTop(), m_Mouse_Move.x - m_Mouse_Down.x, r.GetHeight());
		m_Mouse_Move	= event.GetPosition();
		dc.DrawRectangle(m_Mouse_Down.x, r.GetTop(), m_Mouse_Move.x - m_Mouse_Down.x, r.GetHeight());
	}
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	if(	m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_METRIC
	||	m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_SHADE )
	{
		m_bMouse_Down	= true;
		m_Mouse_Move	= m_Mouse_Down	= event.GetPosition();

		CaptureMouse();
	}
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::On_Mouse_LUp(wxMouseEvent &event)
{
	if( m_bMouse_Down )
	{
		ReleaseMouse();

		m_bMouse_Down	= false;
		m_Mouse_Move	= event.GetPosition();

		wxRect	r(_Draw_Get_rDiagram(wxRect(wxPoint(0, 0), GetClientSize())));
		double	zFactor	= m_pLayer->Get_Type() == WKSP_ITEM_Grid ? ((CWKSP_Grid *)m_pLayer)->Get_Grid()->Get_ZFactor() : 1.0;

		m_pLayer->Set_Color_Range(
			zFactor * m_pLayer->Get_Classifier()->Get_RelativeToMetric(
				(double)(m_Mouse_Down.x - r.GetLeft()) / (double)r.GetWidth()),
			zFactor * m_pLayer->Get_Classifier()->Get_RelativeToMetric(
				(double)(m_Mouse_Move.x - r.GetLeft()) / (double)r.GetWidth())
		);
	}
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	if(	m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_METRIC
	||	m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_SHADE )
	{
		switch( m_pLayer->Get_Type() )
		{
		default:
			return;

		case WKSP_ITEM_Grid:
			m_pLayer->Set_Color_Range(
				((CWKSP_Grid *)m_pLayer)->Get_Grid()->Get_ZMin(true),
				((CWKSP_Grid *)m_pLayer)->Get_Grid()->Get_ZMax(true)
			);
			break;

		case WKSP_ITEM_Shapes:
			m_pLayer->Set_Color_Range(
				((CWKSP_Shapes *)m_pLayer)->Get_Shapes()->Get_MinValue(m_pLayer->Get_Parameters()->Get_Parameter("COLORS_ATTRIB")->asInt()),
				((CWKSP_Shapes *)m_pLayer)->Get_Shapes()->Get_MaxValue(m_pLayer->Get_Parameters()->Get_Parameter("COLORS_ATTRIB")->asInt())
			);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram_Control::On_Size(wxSizeEvent &event)
{
	Refresh();

	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxRect		r(wxPoint(0, 0), GetClientSize());

	Draw_Edge(dc, EDGE_STYLE_SUNKEN, r);

	_Draw(dc, r);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Histogram_Control::_Draw(wxDC &dc, wxRect rDraw)
{
	wxRect		r(_Draw_Get_rDiagram(rDraw));
	wxFont		Font;

	Font.SetFamily(wxSWISS);
	dc.SetFont(Font);

	_Draw_Histogram	(dc, r);
	_Draw_Frame		(dc, r);
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::_Draw_Histogram(wxDC &dc, wxRect r)
{
	int		nClasses	= m_pLayer->Get_Classifier()->Get_Class_Count();

	if( nClasses > 1 )
	{
		int		ax, ay, bx, by;
		double	dx, Value;

		dx	= (double)r.GetWidth() / (double)nClasses;
		ay	= r.GetBottom();
		bx	= r.GetLeft();

		for(int iClass=0; iClass<nClasses; iClass++)
		{
			Value	= m_bCumulative
					? m_pLayer->Get_Classifier()->Histogram_Get_Cumulative(iClass)
					: m_pLayer->Get_Classifier()->Histogram_Get_Count     (iClass);

			ax	= bx;
			bx	= r.GetLeft() + (int)(dx * (iClass + 1.0));
			by	= ay - (int)(r.GetHeight() * Value);

			Draw_FillRect(dc, m_pLayer->Get_Classifier()->Get_Class_Color(iClass), ax, ay, bx, by);
		}
	}
	else
	{
		Draw_Text(dc, TEXTALIGN_CENTER, r.GetLeft() + r.GetWidth() / 2, r.GetBottom() - r.GetHeight() / 2, LNG("no histogram for unclassified data"));
	}
}

//---------------------------------------------------------
void CVIEW_Histogram_Control::_Draw_Frame(wxDC &dc, wxRect r)
{
	const int	dyFont		= 12,
				Precision	= 3;

	int		iPixel, iStep, nSteps, Maximum, nClasses;
	double	dPixel, dPixelFont, dz, dArea	= m_pLayer->Get_Type() == WKSP_ITEM_Grid ? ((CSG_Grid *)m_pLayer->Get_Object())->Get_Cellarea() : 1.0;
	wxFont	Font;

	//-----------------------------------------------------
	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	Maximum	= m_bCumulative
			? m_pLayer->Get_Classifier()->Histogram_Get_Total()
			: m_pLayer->Get_Classifier()->Histogram_Get_Maximum();

	if( Maximum > 0 )
	{
		Font	= dc.GetFont();
		Font.SetPointSize((int)(0.7 * dyFont));
		dc.SetFont(Font);

		//-------------------------------------------------
		dPixelFont	= dyFont;

		if( (dPixel = r.GetHeight() / (double)Maximum) < dPixelFont )
		{
			dPixel	= dPixel * (1 + (int)(dPixelFont / dPixel));
		}

		nSteps	= (int)(r.GetHeight() / dPixel);
		dz		= Maximum * dPixel / (double)r.GetHeight();

		for(iStep=0; iStep<=nSteps; iStep++)
		{
			iPixel	= r.GetBottom()	- (int)(dPixel * iStep);
			dc.DrawLine(r.GetLeft(), iPixel, r.GetLeft() - 5, iPixel);

			if( 1 )
				Draw_Text(dc, TEXTALIGN_CENTERRIGHT, r.GetLeft() - 7, iPixel,
					wxString::Format(wxT("%d"), (int)(iStep * dz))
				);
			else
				Draw_Text(dc, TEXTALIGN_CENTERRIGHT, r.GetLeft() - 7, iPixel,
					wxString::Format(wxT("%.2f"), iStep * dz * dArea)
				);
		}

		//-------------------------------------------------
		nClasses	= m_pLayer->Get_Classifier()->Get_Class_Count();
		dPixelFont	= dyFont + 5;

		if( (dPixel = r.GetWidth() / (double)nClasses) < dPixelFont )
		{
			dPixel	= dPixel * (1 + (int)(dPixelFont / dPixel));
		}

		nSteps	= (int)(r.GetWidth() / dPixel);
		dz		= dPixel / (double)r.GetWidth();

		if( m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_LUT )
		{
			for(iStep=0; iStep<nSteps; iStep++)
			{
				iPixel	= r.GetLeft() + (int)(dPixel * iStep);
				dc.DrawLine(iPixel, r.GetBottom(), iPixel, r.GetBottom() + 5);
				Draw_Text(dc, TEXTALIGN_TOPRIGHT, iPixel, r.GetBottom() + 7, 45.0,
					m_pLayer->Get_Classifier()->Get_Class_Name((int)(nClasses * iStep * dz))
				);
			}
		}
		else
		{
			double	zFactor	= m_pLayer->Get_Type() == WKSP_ITEM_Grid ? ((CWKSP_Grid *)m_pLayer)->Get_Grid()->Get_ZFactor() : 1.0;

			for(iStep=0; iStep<=nSteps; iStep++)
			{
				iPixel	= r.GetLeft() + (int)(dPixel * iStep);
				dc.DrawLine(iPixel, r.GetBottom(), iPixel, r.GetBottom() + 5);
				Draw_Text(dc, TEXTALIGN_CENTERRIGHT, iPixel, r.GetBottom() + 7, 45.0,
					wxString::Format(wxT("%.*f"), Precision, zFactor * m_pLayer->Get_Classifier()->Get_RelativeToMetric(iStep * dz))
				);
			}
		}
	}
}

//---------------------------------------------------------
wxRect CVIEW_Histogram_Control::_Draw_Get_rDiagram(wxRect r)
{
	if( m_pLayer->Get_Classifier()->Get_Mode() == CLASSIFY_LUT )
	{
		return(	wxRect(
			wxPoint(r.GetLeft()  + 50, r.GetTop()    +  10),
			wxPoint(r.GetRight() - 10, r.GetBottom() - 100)
		));
	}

	return(	wxRect(
		wxPoint(r.GetLeft()  + 50, r.GetTop()    + 10),
		wxPoint(r.GetRight() - 10, r.GetBottom() - 40)
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
