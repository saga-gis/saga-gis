
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
//                VIEW_Table_Diagram.cpp                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/scrolwin.h>
#include <wx/clipbrd.h>

#include <saga_gdi/sgdi_helper.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"
#include "res_dialogs.h"

#include "helper.h"

#include "wksp_table.h"

#include "view_table_diagram.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Table_Diagram_Control : public wxScrolledWindow
{
	friend CVIEW_Table_Diagram;

public:
	CVIEW_Table_Diagram_Control(wxWindow *pParent, CWKSP_Table *pTable);
	virtual ~CVIEW_Table_Diagram_Control(void);

	bool							Update_Diagram		(void);

	bool							Set_Size			(const wxSize &Size);
	bool							Fit_Size			(void);

	bool							Set_Zoom			(double Zoom);
	bool							Set_Zoom			(double Zoom, wxPoint Center);

	bool							Set_Parameters		(void);

	void							SaveToClipboard		(void);

	virtual void					OnDraw				(wxDC &dc);

	void							On_Size				(wxSizeEvent  &event);
	void							On_Mouse_LDown		(wxMouseEvent &event);
	void							On_Mouse_RDown		(wxMouseEvent &event);
	void							On_Mouse_Wheel		(wxMouseEvent &event);
	void							On_Key_Down			(wxKeyEvent   &event);


private:

	int								m_xField;

	double							m_xMin, m_xMax, m_yMin, m_yMax, m_yScale;

	CSG_Array_Int					m_Fields;

	CSG_Colors						m_Colors;

	CSG_Parameters					m_Parameters;

	CSG_Table						*m_pTable, m_Structure;

	wxSize							m_Size;


	void							_Destroy				(void);
	bool							_Create					(void);

	bool							_Initialize				(void);

	static int						_On_Parameter_Changed	(CSG_Parameter *pParameter, int Flags);

	int								_Get_Field_By_Name		(const CSG_String &sField);

	void							_Draw					(wxDC &dc, wxRect r);
	void							_Draw_Frame				(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Legend			(wxDC &dc, wxRect r);
	void							_Draw_Points			(wxDC &dc, wxRect r, double dx, double dy, int iField, double Offset, double Scale);
	void							_Draw_Lines				(wxDC &dc, wxRect r, double dx, double dy, int iField, double Offset, double Scale);
	void							_Draw_Bars				(wxDC &dc, wxRect r, double dx, double dy, int iField, double Offset, double Scale);


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Table_Diagram_Control)
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Table_Diagram_Control, wxScrolledWindow);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Table_Diagram_Control, wxScrolledWindow)
	EVT_SIZE      (CVIEW_Table_Diagram_Control::On_Size)
	EVT_LEFT_DOWN (CVIEW_Table_Diagram_Control::On_Mouse_LDown)
	EVT_RIGHT_DOWN(CVIEW_Table_Diagram_Control::On_Mouse_RDown)
	EVT_MOUSEWHEEL(CVIEW_Table_Diagram_Control::On_Mouse_Wheel)
	EVT_KEY_DOWN  (CVIEW_Table_Diagram_Control::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Diagram_Control::CVIEW_Table_Diagram_Control(wxWindow *pParent, CWKSP_Table *pTable)
	: wxScrolledWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE)
{
	SYS_Set_Color_BG_Window(this);

	m_pTable	= pTable->Get_Table();

	_Initialize();
}

//---------------------------------------------------------
CVIEW_Table_Diagram_Control::~CVIEW_Table_Diagram_Control(void)
{
	_Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Set_Parameters(void)
{
	double	Ratio	= m_Parameters("FIX_RATIO")->asBool() ? m_Parameters("RATIO")->asDouble() : 0.;

	if( DLG_Parameters(&m_Parameters) && _Create() )
	{
		if( ((Ratio == 0.) == m_Parameters("FIX_RATIO")->asBool()) || ((Ratio != 0.) && Ratio != m_Parameters("RATIO")->asDouble()) )
		{
			Fit_Size();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Update_Diagram(void)
{
	return( m_Structure.is_Compatible(m_pTable, true) ? _Create() : _Initialize() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MIN_SIZE		100

#define SCROLL_RATE		5

#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Fit_Size(void)
{
	wxSize	Size(GetParent()->GetClientSize());

	Size.x	-= (int)(1.5 * SCROLL_BAR_DX);
	Size.y	-= (int)(1.5 * SCROLL_BAR_DY);

	if( m_Parameters("FIX_RATIO")->asBool() )
	{
		double	Ratio	= m_Parameters("RATIO")->asDouble();

		if( Size.x / (double)Size.y > Ratio )
		{
			Size.x = (int)(Size.y * Ratio);
		}
		else
		{
			Size.y = (int)(Size.x / Ratio);
		}
	}

	return( Set_Size(Size) );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Set_Size(const wxSize &Size)
{
	static bool bSizing = false;

	if( !bSizing && Size.x >= MIN_SIZE && Size.y >= MIN_SIZE )
	{
		if( Size.x != m_Size.x || Size.y != m_Size.y )
		{
			bSizing	= true;

			m_Size	= Size;
		
			SetScrollbars(SCROLL_RATE, SCROLL_RATE,
				(m_Size.x + SCROLL_BAR_DX) / SCROLL_RATE,
				(m_Size.y + SCROLL_BAR_DY) / SCROLL_RATE
			);

			Refresh(false);

			bSizing	= false;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Set_Zoom(double Zoom)
{
	return( Set_Zoom(Zoom, wxPoint(GetClientSize().x / 2, GetClientSize().y / 2)) );
}

bool CVIEW_Table_Diagram_Control::Set_Zoom(double Zoom, wxPoint Center)
{
	if( Zoom > 0. )
	{
		int x, y; GetViewStart(&x, &y);

		x	= (int)((Zoom * (x * SCROLL_RATE + Center.x) - GetClientSize().x / 2) / SCROLL_RATE);
		y	= (int)((Zoom * (y * SCROLL_RATE + Center.y) - GetClientSize().y / 2) / SCROLL_RATE);

		if( Set_Size(wxSize((int)(m_Size.x * Zoom), (int)(m_Size.y * Zoom))) )
		{
			Scroll(x, y);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Size(wxSizeEvent &WXUNUSED(event))
{
	if( m_Parameters("FIT_SIZE") && m_Parameters("FIT_SIZE")->asBool() )
	{
		Fit_Size();
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	event.Skip();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	wxMenu	Menu;

	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DIAGRAM_TO_CLIPBOARD);
	Menu.AppendSeparator();
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DIAGRAM_SIZE_FIT);
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DIAGRAM_SIZE_INC);
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DIAGRAM_SIZE_DEC);
	CMD_Menu_Add_Item(&Menu,  true, ID_CMD_DIAGRAM_LEGEND);
	Menu.AppendSeparator();
	CMD_Menu_Add_Item(&Menu, false, ID_CMD_DIAGRAM_PARAMETERS);

	PopupMenu(&Menu, event.GetPosition());
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Mouse_Wheel(wxMouseEvent &event)
{
	Set_Zoom(event.GetWheelRotation() > 0 ? 1.2 : 1. / 1.2, event.GetPosition());
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default: event.Skip(); break;

	case WXK_PAGEDOWN: Set_Zoom(     1.2); break;
	case WXK_PAGEUP  : Set_Zoom(1. / 1.2); break;

	case WXK_DOWN : { wxPoint p(GetViewStart()); Scroll(p.x, p.y + SCROLL_RATE); } break;
	case WXK_UP   : { wxPoint p(GetViewStart()); Scroll(p.x, p.y - SCROLL_RATE); } break;

	case WXK_LEFT : { wxPoint p(GetViewStart()); Scroll(p.x - SCROLL_RATE, p.y); } break;
	case WXK_RIGHT: { wxPoint p(GetViewStart()); Scroll(p.x + SCROLL_RATE, p.y); } break;

	case 'C':
		if( event.GetModifiers() == wxMOD_CONTROL )
		{
			SaveToClipboard();
		}
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::OnDraw(wxDC &dc)
{
	wxBitmap	Bmp(m_Size);
	wxMemoryDC	dc_Bmp(Bmp);

	dc_Bmp.SetBackground(*wxWHITE_BRUSH);
	dc_Bmp.Clear();

	_Draw(dc_Bmp, m_Size);

	dc_Bmp.SelectObject(wxNullBitmap);

	dc.DrawBitmap(Bmp, 0, 0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::SaveToClipboard(void)
{
	Set_Buisy_Cursor(true);

	wxBitmap	Bmp(m_Size);
	wxMemoryDC	dc_Bmp(Bmp);

	dc_Bmp.SetBackground(*wxWHITE_BRUSH);
	dc_Bmp.Clear();

	_Draw(dc_Bmp, wxRect(m_Size));

	dc_Bmp.SelectObject(wxNullBitmap);

	if( wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBmp	= new wxBitmapDataObject;
		pBmp->SetBitmap(Bmp);
		wxTheClipboard->SetData(pBmp);
		wxTheClipboard->Close();
	}

	Set_Buisy_Cursor(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Destroy(void)
{
	m_Fields.Destroy();
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::_Create(void)
{
	_Destroy();

	//-----------------------------------------------------
	if( m_pTable && m_pTable->Get_Field_Count() > 0 )
	{
		m_xField	= _Get_Field_By_Name(m_Parameters("X_FIELD")->asString());

		if( m_xField < 0 || m_pTable->Get_Range(m_xField) <= 0. )
		{
			m_xMin		= 1;
			m_xMax		= 1 + m_pTable->Get_Count();
			m_xField	= -1;
		}
		else if( m_pTable->Get_Field_Type(m_xField) == SG_DATATYPE_Date )
		{
			m_xMin		= m_pTable->Get_Minimum(m_xField);
			m_xMax		= m_pTable->Get_Maximum(m_xField);
		}
		else
		{
			m_xMin		= m_pTable->Get_Minimum(m_xField);
			m_xMax		= m_pTable->Get_Maximum(m_xField);
		}

		//-------------------------------------------------
		bool	bScaling	= m_Parameters("Y_SCALING")->asBool();

		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( SG_Data_Type_is_Numeric(m_pTable->Get_Field_Type(iField))
			&&  m_Parameters(CSG_String::Format("FIELD_%d", iField))->asBool() )
			{
				m_Fields	+= iField;

				m_Colors.Set_Color(iField, m_Parameters(CSG_String::Format("COLOR_%d" , iField))->asColor());

				double Offset = bScaling ? m_Parameters(CSG_String::Format("OFFSET_%d", iField))->asDouble() : 0.;
				double Scale  = bScaling ? m_Parameters(CSG_String::Format("SCALE_%d" , iField))->asDouble() : 1.;

				double yMin = Scale * m_pTable->Get_Minimum(iField) + Offset;
				double yMax = Scale * m_pTable->Get_Maximum(iField) + Offset;

				if( yMin > yMax ) { double y = yMin; yMin = yMax; yMax = y; }

				if( m_Fields.Get_Size() == 1 )
				{
					m_yMin	= yMin;
					m_yMax	= yMax;
				}
				else
				{
					if( m_yMin	> yMin )
						m_yMin	= yMin;

					if( m_yMax	< yMax )
						m_yMax	= yMax;
				}
			}
		}

		//-------------------------------------------------
		if( m_Parameters("Y_MIN_FIX")->asBool() )
		{
			m_yMin	= m_Parameters("Y_MIN_VAL")->asDouble();
		}

		m_yScale	= m_Parameters("Y_SCALE_TO_X")->asBool() ? m_Parameters("Y_SCALE_RATIO")->asDouble() : 0.;

		if( m_yScale > 0. )
		{
			m_yMax	= m_yMin + (m_xMax - m_xMin) / m_yScale;
		}
		else if( m_Parameters("Y_MAX_FIX")->asBool() )
		{
			m_yMax	= m_Parameters("Y_MAX_VAL")->asDouble();
		}
	}

	if( !m_Size.GetWidth() || !m_Size.GetHeight() )
	{
		SetSize(GetParent()->GetClientSize());
	}

	Refresh(false);

	return( m_Fields.Get_Size() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::_Initialize(void)
{
	_Destroy();

	if( !m_pTable || m_pTable->Get_Field_Count() < 1 || m_pTable->Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	#define CHART_TYPES _TL("bars"), _TL("lines"), _TL("points"), _TL("points connected with lines"), _TL("select a chart type for each attribute")

	CSG_String	sFields_All, sFields_Num; int nFields_Num = 0;

	m_Structure.Create(m_pTable);

	m_Colors.Set_Count(m_pTable->Get_Field_Count());

	m_Parameters.Create(_TL("Properties"));
	m_Parameters.Set_Callback_On_Parameter_Changed(_On_Parameter_Changed);

	//-----------------------------------------------------
	m_Parameters.Add_Bool("", "SHOW_FIELDS", _TL("Show/Hide All Attributes"), _TL("Show or hide all attributes."));

	for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
	{
		if( SG_Data_Type_is_Numeric(m_pTable->Get_Field_Type(iField)) )
		{
			CSG_String	ID; ID.Printf("FIELD_%d", iField);

			m_Parameters.Add_Bool("SHOW_FIELDS", ID, m_pTable->Get_Field_Name(iField), _TL("Show"), false);
			m_Parameters.Add_Color (ID, CSG_String::Format("COLOR_%d" , iField), _TL("Color" ), "", m_Colors.Get_Color(iField));
			m_Parameters.Add_Choice(ID, CSG_String::Format("TYPE_%d"  , iField), _TL("Type"  ), "", CSG_String::Format("%s|%s|%s|%s", CHART_TYPES), 1);
			m_Parameters.Add_Double(ID, CSG_String::Format("OFFSET_%d", iField), _TL("Offset"), "", 0.);
			m_Parameters.Add_Double(ID, CSG_String::Format("SCALE_%d" , iField), _TL("Scale" ), "", 1.);
		}

		if( SG_Data_Type_is_Numeric(m_pTable->Get_Field_Type(iField))
		||  m_pTable->Get_Field_Type(iField) == SG_DATATYPE_Date )
		{
			sFields_Num	+= CSG_String::Format("%s|", m_pTable->Get_Field_Name(iField));
			nFields_Num	++;
		}

		sFields_All	+= CSG_String::Format("%s|", m_pTable->Get_Field_Name(iField));
	}

	sFields_Num	+= CSG_String::Format("<%s>", _TL("none"));
	sFields_All	+= CSG_String::Format("<%s>", _TL("none"));

	//-----------------------------------------------------
	m_Parameters.Add_Node  (""            , "NODE_X"            , _TL("X Axis"            ), _TL(""));
	m_Parameters.Add_Choice("NODE_X"      , "X_FIELD"           , _TL("Values"            ), _TL(""), sFields_Num, nFields_Num);
	m_Parameters.Add_Choice("NODE_X"      , "X_LABEL"           , _TL("Label"             ), _TL(""), sFields_All, m_pTable->Get_Field_Count());

	//-----------------------------------------------------
	m_Parameters.Add_Node  (""            , "NODE_Y"            , _TL("Y Axis"            ), _TL(""));
	m_Parameters.Add_Bool  ("NODE_Y"      , "Y_SCALE_TO_X"      , _TL("Scale"             ), _TL("Scale Y axis to match X axis by a factor (ratio)."), false);
	m_Parameters.Add_Double("Y_SCALE_TO_X", "Y_SCALE_RATIO"     , _TL("Ratio"             ), _TL(""), 1., 0., true);
	m_Parameters.Add_Bool  ("NODE_Y"      , "Y_MIN_FIX"	        , _TL("Fix Minimum"       ), _TL(""), false);
	m_Parameters.Add_Double("Y_MIN_FIX"   , "Y_MIN_VAL"	        , _TL("Minimum"           ), _TL(""), 0.);
	m_Parameters.Add_Bool  ("NODE_Y"      , "Y_MAX_FIX"	        , _TL("Fix Maximum"       ), _TL(""), false);
	m_Parameters.Add_Double("Y_MAX_FIX"   , "Y_MAX_VAL"         , _TL("Maximum"           ), _TL(""), 1000.);
	m_Parameters.Add_Bool  ("NODE_Y"      , "Y_SCALING"         , _TL("Edit Scaling"      ), _TL("Edit offset and scaling for each attribute."), false);

	//-----------------------------------------------------
	m_Parameters.Add_Font  (""            , "FONT"              , _TL("Font"              ), _TL(""));

	m_Parameters.Add_Bool  (""            , "LEGEND"            , _TL("Legend"            ), _TL(""), true);
	m_Parameters.Add_Int   ("LEGEND"      , "LEGEND_WIDTH"      , _TL("Width"             ), _TL("Percent"), 15, 0, true, 50, true);

	//-----------------------------------------------------
	m_Parameters.Add_Node  (""            , "NODE_CHART"        , _TL("Chart Settings"    ), _TL(""));

	m_Parameters.Add_Choice("NODE_CHART"  , "TYPE"              , _TL("Chart Type"        ), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s", CHART_TYPES), 4);

//	m_Parameters.Add_Node  ("NODE_CHART"  , "NODE_BARS"         , _TL("Bars"              ), _TL(""));
//	m_Parameters.Add_Choice("NODE_BARS"   , "BARS_OFFSET"       , _TL("Offset"            ), _TL(""), CSG_String::Format("%s|%s|%s", _TL("origin"), _TL("bottom"), _TL("top")), 0);

	m_Parameters.Add_Node  ("NODE_CHART"  , "NODE_LINES"        , _TL("Lines"             ), _TL(""));
	m_Parameters.Add_Int   ("NODE_LINES"  , "LINES_SIZE"        , _TL("Size"              ), _TL(""), 1, 1, true);

	m_Parameters.Add_Node  ("NODE_CHART"  , "NODE_POINTS"       , _TL("Points"            ), _TL(""));
	m_Parameters.Add_Int   ("NODE_POINTS" , "POINTS_SIZE"       , _TL("Size"              ), _TL(""), 2, 1, true);
	m_Parameters.Add_Bool  ("NODE_POINTS" , "POINTS_OUTLINE"    , _TL("Outline"           ), _TL(""), false);
	m_Parameters.Add_Choice("NODE_POINTS" , "POINTS_COLOR_FIELD", _TL("Color by Attribute"), _TL(""), sFields_Num, nFields_Num);
	m_Parameters.Add_Colors("NODE_POINTS" , "POINTS_COLORS"     , _TL("Colors"            ), _TL(""));

	//-----------------------------------------------------
	m_Parameters.Add_Node  (""            , "NODE_FRAME"        , _TL("Frame"             ), _TL(""));
	m_Parameters.Add_Choice("NODE_FRAME"  , "FRAME_FULL"        , _TL("Draw Frame"        ), _TL(""), CSG_String::Format("%s|%s", _TL("all sides"), _TL("left/bottom")), 1);
	m_Parameters.Add_Bool  ("NODE_FRAME"  , "AXES_ORIGINS"      , _TL("Show Origins"      ), _TL(""), false);
	m_Parameters.Add_Bool  ("NODE_FRAME"  , "FIT_SIZE"          , _TL("Fit Size to Window"), _TL("Fit diagram's frame size to the parent window, when its size has changed."), true);
	m_Parameters.Add_Bool  ("NODE_FRAME"  , "FIX_RATIO"         , _TL("Fix Ratio"         ), _TL("Use a fix width to height ratio."), false);
	m_Parameters.Add_Double("FIX_RATIO"   , "RATIO"             , _TL("Ratio"             ), _TL(""), (1. + sqrt(5.)) / 2., 0.01, true, 100., true);

	m_Parameters.Add_Node  (""            , "NODE_MARGINS"      , _TL("Frame Margins"     ), _TL(""));
	m_Parameters.Add_Int   ("NODE_MARGINS", "MARGIN_LEFT"       , _TL("Left"              ), _TL("Pixels"), 50, 0, true);
	m_Parameters.Add_Int   ("NODE_MARGINS", "MARGIN_RIGHT"      , _TL("Right"             ), _TL("Pixels"), 10, 0, true);
	m_Parameters.Add_Int   ("NODE_MARGINS", "MARGIN_TOP"        , _TL("Top"               ), _TL("Pixels"), 10, 0, true);
	m_Parameters.Add_Int   ("NODE_MARGINS", "MARGIN_BOTTOM"     , _TL("Bottom"            ), _TL("Pixels"), 50, 0, true);

	return( _Create() && Fit_Size() );
}

//---------------------------------------------------------
int CVIEW_Table_Diagram_Control::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	CSG_Parameters	*pParameters	= pParameter ? pParameter->Get_Parameters() : NULL;

	if( !pParameters )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( pParameter->Cmp_Identifier("SHOW_FIELDS") )
		{
			bool bDifferent = (*pParameters)("TYPE")->asInt() >= 4;

			for(int i=0; i<pParameter->Get_Children_Count(); i++)
			{
				pParameter->Get_Child(i)->Set_Value                (pParameter->asBool());
				pParameter->Get_Child(i)->Get_Child(0)->Set_Enabled(pParameter->asBool());
				pParameter->Get_Child(i)->Get_Child(1)->Set_Enabled(pParameter->asBool() && bDifferent);
			}
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if( pParameter->Cmp_Identifier("TYPE") )
		{
		//	pParameters->Set_Enabled("NODE_BARS"    , pParameter->asInt() >= 4 || pParameter->asInt() == 0                            );
			pParameters->Set_Enabled("NODE_LINES"   , pParameter->asInt() >= 4 || pParameter->asInt() == 1 || pParameter->asInt() == 3);
			pParameters->Set_Enabled("NODE_POINTS"  , pParameter->asInt() >= 4 || pParameter->asInt() == 2 || pParameter->asInt() == 3);

			CSG_Parameter *pFields = (*pParameters)("SHOW_FIELDS");

			for(int i=0; i<pFields->Get_Children_Count(); i++)
			{
				CSG_Parameter *pField = pFields->Get_Child(i);

				pField->Get_Child(1)->Set_Enabled(pField->asBool() && pParameter->asInt() >= 4);
			}
		}

		if( pParameter->Cmp_Identifier("Y_SCALING") )
		{
			CSG_Parameter *pFields = (*pParameters)("SHOW_FIELDS");

			for(int i=0; i<pFields->Get_Children_Count(); i++)
			{
				CSG_Parameter *pField = pFields->Get_Child(i);

				pField->Get_Child(2)->Set_Enabled(pField->asBool() && pParameter->asBool());
				pField->Get_Child(3)->Set_Enabled(pField->asBool() && pParameter->asBool());
			}
		}

		if( pParameter->Cmp_Identifier("POINTS_COLOR_FIELD") )
		{
			pParameters->Set_Enabled("POINTS_COLORS", pParameter->asInt() < pParameter->asChoice()->Get_Count() - 1);
		}

		if( pParameter->Cmp_Identifier("LEGEND") )
		{
			pParameters->Set_Enabled("LEGEND_WIDTH", pParameter->asBool());
		}

		if( pParameter->Cmp_Identifier("FIX_RATIO") )
		{
			pParameters->Set_Enabled("RATIO", pParameter->asBool());
		}

		if( pParameter->Cmp_Identifier("X_FIELD") )
		{
			bool	bNone	= pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1;

			pParameters->Set_Enabled("Y_SCALE_TO_X", bNone == false);
			pParameters->Set_Enabled("Y_MAX_FIX"   , bNone || (*pParameters)("Y_SCALE_TO_X")->asBool() == false);
		}

		if( pParameter->Cmp_Identifier("Y_SCALE_TO_X") )
		{
			pParameters->Set_Enabled("Y_SCALE_RATIO", pParameter->asBool() ==  true);
			pParameters->Set_Enabled("Y_MAX_FIX"    , pParameter->asBool() == false);
		}

		if( pParameter->Cmp_Identifier("Y_MIN_FIX") )
		{
			pParameters->Set_Enabled("Y_MIN_VAL"    , pParameter->asBool());
		}

		if( pParameter->Cmp_Identifier("Y_MAX_FIX") )
		{
			pParameters->Set_Enabled("Y_MAX_VAL"    , pParameter->asBool());
		}

		if( pParameter->Cmp_Identifier("X_FIELD") )
		{
			pParameters->Set_Enabled("X_LABEL"      , pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1);
		}

		CSG_String	ID(pParameter->Get_Identifier());

		if( ID.Find("FIELD_") == 0 )
		{
			pParameters->Set_Enabled("COLOR_"  + ID.AfterFirst('_'), pParameter->asBool());
			pParameters->Set_Enabled("TYPE_"   + ID.AfterFirst('_'), pParameter->asBool() && (*pParameters)("TYPE")->asInt() >= 4);
			pParameters->Set_Enabled("OFFSET_" + ID.AfterFirst('_'), pParameter->asBool() && (*pParameters)("Y_SCALING")->asBool());
			pParameters->Set_Enabled("SCALE_"  + ID.AfterFirst('_'), pParameter->asBool() && (*pParameters)("Y_SCALING")->asBool());
		}
	}

	//-----------------------------------------------------
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVIEW_Table_Diagram_Control::_Get_Field_By_Name(const CSG_String &sField)
{
	if( m_pTable )
	{
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( sField.Cmp(m_pTable->Get_Field_Name(iField)) == 0 )
			{
				return( iField );
			}
		}
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw(wxDC &dc, wxRect rDC)
{
	#define DO_ERROR(s)	Draw_Text(dc, TEXTALIGN_CENTER, rDC.GetLeft() + rDC.GetWidth() / 2, rDC.GetTop() + rDC.GetHeight() / 2, s); return;

	dc.SetFont(Get_Font(m_Parameters("FONT")));

	//-----------------------------------------------------
	if( m_Fields.Get_Size() < 1 )
	{
		DO_ERROR(_TL("invalid data set!"));
	}

	//-----------------------------------------------------
	int	 wLegend = m_Parameters("LEGEND")->asBool() ? (int)(rDC.GetWidth () * 0.01 * m_Parameters("LEGEND_WIDTH")->asInt()) : 0;

	wxRect	r(wxPoint(
		rDC.GetLeft  () + m_Parameters("MARGIN_LEFT"  )->asInt(),
		rDC.GetTop   () + m_Parameters("MARGIN_TOP"   )->asInt()), wxPoint(
		rDC.GetRight () - m_Parameters("MARGIN_RIGHT" )->asInt() - wLegend,
		rDC.GetBottom() - m_Parameters("MARGIN_BOTTOM")->asInt())
	);

	//-----------------------------------------------------
	double	dy, dx;

	if( m_xField < 0 )
	{
		dx	= r.GetWidth () / (double)m_pTable->Get_Count();
		dy	= r.GetHeight() / (m_yMax - m_yMin);
	}
	else if( m_xMin < m_xMax )
	{
		dx	= r.GetWidth () / (m_xMax - m_xMin);

		if( m_yScale > 0. )
		{
			dy	= dx * m_yScale; m_yMax = m_yMin + r.GetHeight() / dy;
		}
		else if( m_yMin < m_yMax )
		{
			dy	= r.GetHeight() / (m_yMax - m_yMin);
		}
		else
		{
			DO_ERROR(_TL("invalid y range!"));
		}
	}
	else
	{
		DO_ERROR(_TL("invalid x range!"));
	}

	//-----------------------------------------------------
	int Type = m_Parameters("TYPE")->asInt(); bool bDifferent = Type >= 4, bScaling = m_Parameters("Y_SCALING")->asBool();

	for(size_t iField=0; iField<m_Fields.Get_Size(); iField++)
	{
		if( bDifferent )
		{
			CSG_Parameter *pParameter = m_Parameters(CSG_String::Format("TYPE_%d", m_Fields[iField]));

			Type = pParameter ? pParameter->asInt() : -1;
		}

		double Offset = bScaling ? m_Parameters(CSG_String::Format("OFFSET_%d", m_Fields[iField]))->asDouble() : 0.;
		double Scale  = bScaling ? m_Parameters(CSG_String::Format("SCALE_%d" , m_Fields[iField]))->asDouble() : 1.;

		switch( Type )
		{
		case  0: _Draw_Bars  (dc, r, dx, dy, iField, Offset, Scale); break; // bars
		default: _Draw_Lines (dc, r, dx, dy, iField, Offset, Scale); break; // lines
		case  2: _Draw_Points(dc, r, dx, dy, iField, Offset, Scale); break; // points
		case  3: _Draw_Lines (dc, r, dx, dy, iField, Offset, Scale);        // points connected with lines
		         _Draw_Points(dc, r, dx, dy, iField, Offset, Scale); break;
		}
	}

	//-----------------------------------------------------
	_Draw_Frame(dc, r, dx, dy);

	if( wLegend > 0 )
	{
		r.SetLeft(rDC.GetRight() - wLegend);
		r.SetWidth(wLegend);

		_Draw_Legend(dc, r);
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Frame(wxDC &dc, wxRect r, double dx, double dy)
{
	const int	dyFont	= 12;

	//-----------------------------------------------------
	dc.SetPen(*wxBLACK);

	if( m_Parameters("FRAME_FULL")->asInt() == 0 )
	{
		Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);
	}
	else
	{
		Draw_Edge(dc, EDGE_STYLE_SIMPLE, r.GetLeft(), r.GetBottom(), r.GetLeft (), r.GetTop   ());
		Draw_Edge(dc, EDGE_STYLE_SIMPLE, r.GetLeft(), r.GetBottom(), r.GetRight(), r.GetBottom());
	}

	if( m_Parameters("AXES_ORIGINS")->asBool() )
	{
		if( m_xField >= 0 && m_xMin < 0 && 0 < m_xMax )
		{
			int x = r.GetLeft() + (int)(dx * (0 - m_xMin));

			dc.DrawLine(x, r.GetBottom(), x, r.GetTop());
		}

		if( m_yMin < 0 && 0 < m_yMax )
		{
			int y = r.GetBottom() - (int)(dy * (0 - m_yMin));

			dc.DrawLine(r.GetLeft(), y, r.GetRight(), y);
		}
	}

	//-----------------------------------------------------
	wxFont	Font(dc.GetFont());
	Font.SetPointSize((int)(0.7 * dyFont));
	dc.SetFont(Font);

	//-----------------------------------------------------
	Draw_Scale(dc, wxRect(r.GetLeft() - 20, r.GetTop(), 20, r.GetHeight()),	// Y Axis
		m_yMin, m_yMax,
		false, false, false
	);

	//-----------------------------------------------------
	if( m_xField >= 0 )	// numeric values
	{
		if( m_pTable->Get_Field_Type(m_xField) != SG_DATATYPE_Date )
		{
			Draw_Scale(dc, wxRect(r.GetLeft(), r.GetBottom(), r.GetWidth(), 20),
				m_xMin, m_xMax,
				true , true , true
			);
		}
		else // if( m_pTable->Get_Field_Type(m_xField) == SG_DATATYPE_Date )
		{
			double dStep  = 10 + dyFont;
			int    nSteps = r.GetWidth() / dStep;
			double dDay   = (m_xMax - m_xMin) / nSteps;

			for(int iStep=0; iStep<=nSteps; iStep++)
			{
				int ix = r.GetLeft() + (int)(dStep * iStep);

				dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);	// tic

				wxDateTime	Date(m_xMin + dDay * iStep);	// Julian Day Number

				Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45., Date.Format("%Y-%m-%d"));
			}
		}
	}
	else
	{
		int	iLabel	= m_Parameters("X_LABEL")->asInt();

		if( iLabel < 0 || iLabel >= m_pTable->Get_Field_Count() )
		{
			Draw_Scale(dc, wxRect(r.GetLeft(), r.GetBottom(), r.GetWidth(), 20),
				1, m_pTable->Get_Count(),
				true , true , true
			);
		}
		else
		{
			int	iStep	= dx > dyFont ? 1 : (int)(1 + (10 + dyFont) / dx);

			for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord+=iStep)
			{
				int	ix	= r.GetLeft() + (int)(dx * iRecord);

				dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);

				CSG_Table_Record *pRecord = m_pTable->Get_Record_byIndex(iRecord);

				Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45., pRecord->asString(iLabel));
			}
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Legend(wxDC &dc, wxRect r)
{
	int	dyFont	= (int)(0.08 * r.GetWidth());
	int	dyBox	= dyFont + 4;
	int	dxBox	= dyFont * 2;

	const int minMargin = 10;

	r.SetTopLeft(wxPoint(r.GetLeft() + minMargin, r.GetBottom() - m_Fields.Get_Size() * dyBox));

	wxFont	Font(dc.GetFont());
	Font.SetPointSize(dyFont);
	dc.SetFont(Font);

	dc.SetPen(*wxBLACK_PEN);

	//-----------------------------------------------------
	for(size_t iField=0; iField<m_Fields.Get_Size(); iField++)
	{
		wxRect	rBox(r.GetLeft(), r.GetTop() + iField * dyBox, dxBox, dyBox);
		wxBrush	Brush(dc.GetBrush());
		Brush.SetColour(Get_Color_asWX(m_Colors.Get_Color(m_Fields[iField])));
		dc.SetBrush(Brush);
		dc.DrawRectangle(rBox.GetX(), rBox.GetY() + 4, rBox.GetWidth(), rBox.GetHeight() - 4);
		dc.DrawText(m_pTable->Get_Field_Name(m_Fields[iField]), rBox.GetRight() + 5, rBox.GetTop());
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DRAW_GET_XPOS	(r.GetLeft  () + (int)(dx * (m_xField >= 0 ? (pRecord->asDouble(m_xField) - m_xMin) : (double)iRecord)))
#define DRAW_GET_YPOS	(r.GetBottom() - (int)(dy * ((Scale * pRecord->asDouble(iField) + Offset) - m_yMin)))

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Points(wxDC &dc, wxRect r, double dx, double dy, int iField, double Offset, double Scale)
{
	bool	bOutline	= m_Parameters("POINTS_OUTLINE")->asBool();
	int		Size		= m_Parameters("POINTS_SIZE"   )->asInt();
	int		zField		= _Get_Field_By_Name(m_Parameters("POINTS_COLOR_FIELD")->asString());

	iField	= m_Fields[iField];

	if( zField < 0 )
	{
		dc.SetPen  (wxPen  (bOutline ? *wxBLACK : Get_Color_asWX(m_Colors.Get_Color(iField))));
		dc.SetBrush(wxBrush(                      Get_Color_asWX(m_Colors.Get_Color(iField))));

		for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

			if( !pRecord->is_NoData(iField) && (m_pTable->Get_Selection_Count() < 1 || pRecord->is_Selected()) )
			{
				dc.DrawCircle(DRAW_GET_XPOS, DRAW_GET_YPOS, Size);
			}
		}
	}
	else
	{
		if( bOutline )
		{
			dc.SetPen(wxPen(Get_Color_asWX(m_Colors.Get_Color(iField))));
		}

		CSG_Colors	*pColors	= m_Parameters("POINTS_COLORS")->asColors();
		double		zMin		= m_pTable->Get_Minimum(zField);
		double		dz			= pColors->Get_Count() / m_pTable->Get_Range(zField);

		for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

			if( !pRecord->is_NoData(iField) && (m_pTable->Get_Selection_Count() < 1 || pRecord->is_Selected()) )
			{
				int			iz	= (int)(dz * (pRecord->asDouble(zField) - zMin));
				wxColour	ic	= Get_Color_asWX(pColors->Get_Color(iz < 0 ? 0 : (iz >= 255 ? 255 : iz)));

				if( !bOutline )
				{
					dc.SetPen(wxPen(ic));
				}

				dc.SetBrush(wxBrush(ic));

				dc.DrawCircle(DRAW_GET_XPOS, DRAW_GET_YPOS, Size);
			}
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Lines(wxDC &dc, wxRect r, double dx, double dy, int iField, double Offset, double Scale)
{
	if( m_pTable->Get_Count() > 1 )
	{
		iField	= m_Fields[iField];

		int Size = m_Parameters("LINES_SIZE")->asInt();

		dc.SetPen  (wxPen  (Get_Color_asWX(m_Colors.Get_Color(iField)), Size, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(Get_Color_asWX(m_Colors.Get_Color(iField)), wxBRUSHSTYLE_SOLID));

		for(int iRecord=0, bLast=0, xLast, yLast; iRecord<m_pTable->Get_Count(); iRecord++)
		{
			CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

			if( !pRecord->is_NoData(iField) && (m_pTable->Get_Selection_Count() < 1 || pRecord->is_Selected()) )
			{
				int	x	= DRAW_GET_XPOS;
				int	y	= DRAW_GET_YPOS;

				if( bLast )
				{
					dc.DrawLine(xLast, yLast, x, y);
				}

				xLast	= x;
				yLast	= y;
				bLast	= 1;
			}
			else
			{
				bLast	= 0;
			}
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Bars(wxDC &dc, wxRect r, double dx, double dy, int iField, double Offset, double Scale)
{
	int	dxa	= m_xField < 0 ? (int)(dx / m_Fields.Get_Size() * iField) : iField + 0;
	int	dxb	= m_xField < 0 ? (int)(dx / m_Fields.Get_Size()         ) : iField + 1;

	int yOffset = r.GetBottom() - (int)(dy * (Offset - m_yMin));

	iField	= m_Fields[iField];

	dc.SetPen(wxPen(Get_Color_asWX(m_Colors.Get_Color(iField)), 1, wxPENSTYLE_SOLID));

	for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

		if( !pRecord->is_NoData(iField) && (m_pTable->Get_Selection_Count() < 1 || pRecord->is_Selected()) )
		{
			int	x	= DRAW_GET_XPOS + dxa;
			int	y	= DRAW_GET_YPOS;

			for(int xb=x+dxb; x<=xb; x++)
			{
				dc.DrawLine(x, yOffset, x, y);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_Table_Diagram, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Table_Diagram, CVIEW_Base)
	EVT_SIZE    (CVIEW_Table_Diagram::On_Size)
	EVT_KEY_DOWN(CVIEW_Table_Diagram::On_Key_Down)

	EVT_MENU(ID_CMD_DIAGRAM_PARAMETERS  , CVIEW_Table_Diagram::On_Parameters)
	EVT_MENU(ID_CMD_DIAGRAM_SIZE_FIT    , CVIEW_Table_Diagram::On_Size_Fit)
	EVT_MENU(ID_CMD_DIAGRAM_SIZE_INC    , CVIEW_Table_Diagram::On_Size_Inc)
	EVT_MENU(ID_CMD_DIAGRAM_SIZE_DEC    , CVIEW_Table_Diagram::On_Size_Dec)
	EVT_MENU(ID_CMD_DIAGRAM_LEGEND      , CVIEW_Table_Diagram::On_Legend)
	EVT_MENU(ID_CMD_DIAGRAM_TO_CLIPBOARD, CVIEW_Table_Diagram::On_SaveToClipboard)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Diagram::CVIEW_Table_Diagram(CWKSP_Table *pTable)
	: CVIEW_Base(pTable, ID_VIEW_TABLE_DIAGRAM, wxString::Format("%s [%s]", _TL("Diagram"), pTable->Get_Name().c_str()), ID_IMG_WND_DIAGRAM, false)
{
	SYS_Set_Color_BG_Window(this);

	m_pControl	= new CVIEW_Table_Diagram_Control(this, pTable);

	if( m_pControl->Set_Parameters() )
	{
		Do_Show();
	}
	else
	{
		Destroy();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Table_Diagram::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_PARAMETERS);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_FIT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_INC);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_DEC);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_DIAGRAM_LEGEND);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_TO_CLIPBOARD);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Table_Diagram::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_TABLE_DIAGRAM);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DIAGRAM_PARAMETERS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DIAGRAM_SIZE_FIT);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DIAGRAM_SIZE_INC);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DIAGRAM_SIZE_DEC);

	CMD_ToolBar_Add(pToolBar, _TL("Diagram"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::Do_Update(void)
{
	m_pControl->Update_Diagram();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Size(wxSizeEvent &event)
{
	m_pControl->SetSize(GetClientSize());
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Key_Down(wxKeyEvent &event)
{
	m_pControl->On_Key_Down(event);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Parameters(wxCommandEvent &event)
{
	m_pControl->Set_Parameters();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Size_Fit(wxCommandEvent &event)
{
	m_pControl->Fit_Size();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Size_Inc(wxCommandEvent &event)
{
	m_pControl->Set_Zoom(1.2);
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Size_Dec(wxCommandEvent &event)
{
	m_pControl->Set_Zoom(1. / 1.2);
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Legend(wxCommandEvent &event)
{
	m_pControl->m_Parameters("LEGEND")->Set_Value(!m_pControl->m_Parameters("LEGEND")->asBool());

	m_pControl->Refresh();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_SaveToClipboard(wxCommandEvent &event)
{
	m_pControl->SaveToClipboard();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	case ID_CMD_DIAGRAM_LEGEND:
		event.Check(m_pControl->m_Parameters("LEGEND")->asBool());
		break;

	//-----------------------------------------------------
	default:
		break;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
