
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
#define MIN_SIZE		100

#define SCROLL_RATE		5

#define SCROLL_BAR_DX	wxSystemSettings::GetMetric(wxSYS_VSCROLL_X)
#define SCROLL_BAR_DY	wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Table_Diagram_Control : public wxScrolledWindow
{
public:
	CVIEW_Table_Diagram_Control(wxWindow *pParent, CWKSP_Table *pTable);
	virtual ~CVIEW_Table_Diagram_Control(void);

	bool							Update_Diagram		(void);

	bool							Set_Size			(const wxSize &Size);
	bool							Set_Size			(double Zoom, wxPoint Center);
	bool							Fit_Size			(void);
	bool							Inc_Size			(void);
	bool							Dec_Size			(void);

	bool							Set_Parameters		(void);

	void							SaveToClipboard		(void);

	virtual void					OnDraw				(wxDC &dc);


private:

	bool							m_bFitSize;

	int								m_nFields, *m_Fields, m_xField;

	double							m_xMin, m_xMax, m_yMin, m_yMax, m_yScale;

	CSG_Colors						m_Colors;

	CSG_Parameters					m_Parameters;

	CSG_Table						*m_pTable, m_Structure;

	wxSize							m_sDraw;


	void							On_Size				(wxSizeEvent  &event);
	void							On_Mouse_LDown		(wxMouseEvent &event);
	void							On_Mouse_RDown		(wxMouseEvent &event);

	static int						_On_Parameter_Changed	(CSG_Parameter *pParameter, int Flags);

	void							_Destroy			(void);
	bool							_Create				(void);
	bool							_Initialize			(void);

	int								_Get_Field_By_Name	(const CSG_String &sField);

	void							_Draw				(wxDC &dc, wxRect r);
	void							_Draw_Frame			(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Legend		(wxDC &dc, wxRect r);
	void							_Draw_Points		(wxDC &dc, wxRect r, double dx, double dy, int iField);
	void							_Draw_Lines			(wxDC &dc, wxRect r, double dx, double dy, int iField);
	void							_Draw_Bars			(wxDC &dc, wxRect r, double dx, double dy, int iField);


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
	EVT_SIZE			(CVIEW_Table_Diagram_Control::On_Size)
	EVT_LEFT_DOWN		(CVIEW_Table_Diagram_Control::On_Mouse_LDown)
	EVT_RIGHT_DOWN		(CVIEW_Table_Diagram_Control::On_Mouse_RDown)
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

	m_nFields	= 0;
	m_Fields	= NULL;

	m_bFitSize	= false;

	Fit_Size();

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
	if( DLG_Parameters(&m_Parameters) )
	{
		return( _Create() );
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Update_Diagram(void)
{
	return( m_Structure.is_Compatible(m_pTable, true) ? _Create() : _Initialize() );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Set_Size(const wxSize &Size)
{
	if( Size.x >= MIN_SIZE && Size.y >= MIN_SIZE )
	{
		if( Size.x != m_sDraw.x || Size.y != m_sDraw.y )
		{
			bool	bFitSize	= m_bFitSize;

			m_bFitSize	= false;

			m_sDraw.x	= Size.x;
			m_sDraw.y	= Size.y;
		
			SetScrollbars(SCROLL_RATE, SCROLL_RATE, (m_sDraw.x + SCROLL_BAR_DX) / SCROLL_RATE, (m_sDraw.y + SCROLL_BAR_DY) / SCROLL_RATE);

			Refresh(false);

			m_bFitSize	= bFitSize;
		}

		return( true );
	}

	return( false );
}

bool CVIEW_Table_Diagram_Control::Set_Size(double Zoom, wxPoint Center)
{
	if( Zoom > 0.0 )
	{
		int		x, y;

		GetViewStart(&x, &y);

		x	= (int)((Zoom * (x * SCROLL_RATE + Center.x) - GetClientSize().x / 2) / SCROLL_RATE);
		y	= (int)((Zoom * (y * SCROLL_RATE + Center.y) - GetClientSize().y / 2) / SCROLL_RATE);

		if( Set_Size(wxSize((int)(m_sDraw.x * Zoom), (int)(m_sDraw.y * Zoom))) )
		{
			Scroll(x, y);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Fit_Size(void)
{
	wxSize	Size(GetParent()->GetClientSize());

	Size.x	-= 2 * SCROLL_BAR_DX;
	Size.y	-= 2 * SCROLL_BAR_DY;

	return( Set_Size(Size) );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Inc_Size(void)
{
	return( Set_Size(1.2, wxPoint(GetClientSize().x / 2, GetClientSize().y / 2)) );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::Dec_Size(void)
{
	return( Set_Size(1.0 / 1.2, wxPoint(GetClientSize().x / 2, GetClientSize().y / 2)) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Size(wxSizeEvent &WXUNUSED(event))
{
	if( m_bFitSize )
	{
		Fit_Size();
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Mouse_LDown(wxMouseEvent &event)
{
	Set_Size(1.2, event.GetPosition());
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::On_Mouse_RDown(wxMouseEvent &event)
{
	Set_Size(1.0 / 1.2, event.GetPosition());
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::SaveToClipboard(void)
{
	Set_Buisy_Cursor(true);

	wxBitmap	BMP;
	wxMemoryDC	dc;

	BMP.Create(m_sDraw.x, m_sDraw.y);
	dc.SelectObject(BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	_Draw(dc, wxRect(0, 0, m_sDraw.x, m_sDraw.y));

	dc.SelectObject(wxNullBitmap);

	if( wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
		pBMP->SetBitmap(BMP);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();
	}

	Set_Buisy_Cursor(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CVIEW_Table_Diagram_Control::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	CSG_Parameters	*pParameters	= pParameter ? pParameter->Get_Parameters() : NULL;

	if( !pParameters )
	{
		return( 0 );
	}

	CSG_String	s(pParameter->Get_Identifier());

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if( s.Find("FIELD_") == 0 )
		{
			s.Replace("FIELD_", "COLOR_");

			if( pParameters->Get_Parameter(s) )
			{
				pParameters->Set_Enabled(s, pParameter->asBool());
			}
		}

		if( !s.Cmp("TYPE") )
		{
			pParameters->Set_Enabled("NODE_POINTS"  , pParameter->asInt() >= 2);
		}

		if( !s.Cmp("POINTS_COLOR_FIELD") )
		{
			pParameters->Set_Enabled("POINTS_COLORS", pParameter->asInt() < pParameter->asChoice()->Get_Count() - 1);
		}

		if( !s.Cmp("X_FIELD") )
		{
			bool	bNone	= pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1;

			pParameters->Set_Enabled("Y_SCALE_TO_X", bNone == false);
			pParameters->Set_Enabled("Y_MAX_FIX"   , bNone || (*pParameters)("Y_SCALE_TO_X")->asBool() == false);
		}

		if( !s.Cmp("Y_SCALE_TO_X") )
		{
			pParameters->Set_Enabled("Y_SCALE_RATIO", pParameter->asBool() ==  true);
			pParameters->Set_Enabled("Y_MAX_FIX"    , pParameter->asBool() == false);
		}

		if( !s.Cmp("Y_MIN_FIX") )
		{
			pParameters->Set_Enabled("Y_MIN_VAL"    , pParameter->asBool());
		}

		if( !s.Cmp("Y_MAX_FIX") )
		{
			pParameters->Set_Enabled("Y_MAX_VAL"    , pParameter->asBool());
		}

		if( !s.Cmp("X_FIELD") )
		{
			pParameters->Set_Enabled("X_LABEL"      , pParameter->asInt() >= pParameter->asChoice()->Get_Count() - 1);
		}
	}

	//-----------------------------------------------------
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Destroy(void)
{
	if( m_nFields > 0 )
	{
		SG_Free(m_Fields);

		m_Fields	= NULL;
		m_nFields	= 0;
	}
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::_Create(void)
{
	_Destroy();

	//-----------------------------------------------------
	if( m_pTable && m_pTable->Get_Field_Count() > 0 )
	{
		m_bFitSize	= m_Parameters("FIT_SIZE")->asBool();

		m_xField	= _Get_Field_By_Name(m_Parameters("X_FIELD")->asString());

		if( m_xField < 0 || m_pTable->Get_Range(m_xField) <= 0.0 )
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
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if(	m_pTable->Get_Field_Type(iField) != SG_DATATYPE_String
			&&	m_Parameters(CSG_String::Format("FIELD_%d", iField))->asBool() )
			{
				m_Fields			= (int *)SG_Realloc(m_Fields, (m_nFields + 1) * sizeof(int));
				m_Fields[m_nFields]	= iField;

				m_Colors.Set_Color(iField, m_Parameters(CSG_String::Format("COLOR_%d", iField))->asColor());

				m_nFields++;

				if( m_nFields == 1 )
				{
					m_yMin	= m_pTable->Get_Minimum(iField);
					m_yMax	= m_pTable->Get_Maximum(iField);
				}
				else
				{
					if( m_yMin	> m_pTable->Get_Minimum(iField) )
						m_yMin	= m_pTable->Get_Minimum(iField);

					if( m_yMax	< m_pTable->Get_Maximum(iField) )
						m_yMax	= m_pTable->Get_Maximum(iField);
				}
			}
		}

		//-------------------------------------------------
		if( m_Parameters("Y_MIN_FIX")->asBool() )
		{
			m_yMin	= m_Parameters("Y_MIN_VAL")->asDouble();
		}

		m_yScale	= m_Parameters("Y_SCALE_TO_X")->asBool() ? m_Parameters("Y_SCALE_RATIO")->asDouble() : 0.0;

		if( m_yScale > 0.0 )
		{
			m_yMax	= m_yMin + (m_xMax - m_xMin) / m_yScale;
		}
		else if( m_Parameters("Y_MAX_FIX")->asBool() )
		{
			m_yMax	= m_Parameters("Y_MAX_VAL")->asDouble();
		}
	}

	Refresh(false);

	return( m_nFields > 0 );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::_Initialize(void)
{
	_Destroy();

	//-----------------------------------------------------
	if( m_pTable && m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Count() > 0 )
	{
		CSG_String	sFields_All, sFields_Num; int nFields_Num = 0;

		m_Structure.Create(m_pTable);

		m_Colors.Set_Count(m_pTable->Get_Field_Count());

		m_Parameters.Create(_TL("Properties"));
		m_Parameters.Set_Callback_On_Parameter_Changed(_On_Parameter_Changed);

		m_Parameters.Add_Node("", "NODE_GENERAL", _TL("General"   ), _TL(""));
		m_Parameters.Add_Node("", "NODE_POINTS" , _TL("Points"    ), _TL(""));
		m_Parameters.Add_Node("", "NODE_X"      , _TL("X Axis"    ), _TL(""));
		m_Parameters.Add_Node("", "NODE_Y"      , _TL("Y Axis"    ), _TL(""));
		m_Parameters.Add_Node("", "NODE_FIELDS" , _TL("Attributes"), _TL(""));

		//-------------------------------------------------
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( m_pTable->Get_Field_Type(iField) != SG_DATATYPE_String )
			{
				CSG_String	ID; ID.Printf("FIELD_%d", iField);

				m_Parameters.Add_Bool ("NODE_FIELDS", ID, m_pTable->Get_Field_Name(iField), _TL("Show"), false);
				m_Parameters.Add_Color(ID, CSG_String::Format("COLOR_%d", iField), "", _TL("Color"), m_Colors.Get_Color(iField));

				sFields_Num	+= CSG_String::Format("%s|", m_pTable->Get_Field_Name(iField));
				nFields_Num	++;
			}

			sFields_All	+= CSG_String::Format("%s|", m_pTable->Get_Field_Name(iField));
		}

		sFields_Num	+= CSG_String::Format("<%s>", _TL("none"));
		sFields_All	+= CSG_String::Format("<%s>", _TL("none"));

		//-------------------------------------------------
		m_Parameters.Add_Choice("NODE_GENERAL",
			"TYPE"		, _TL("Type"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("Bars"),
				_TL("Lines"),
				_TL("Lines and Points"),
				_TL("Points")
			), 1
		);

		m_Parameters.Add_Font("NODE_GENERAL",
			"FONT"		, _TL("Font"),
			_TL("")
		);

		m_Parameters.Add_Bool("NODE_GENERAL",
			"LEGEND"	, _TL("Legend"),
			_TL(""),
			true
		);

		m_Parameters.Add_Bool("NODE_GENERAL",
			"FIT_SIZE"	, _TL("Fit Size to Window"),
			_TL(""),
			true
		);

		//-------------------------------------------------
		m_Parameters.Add_Int("NODE_POINTS",
			"POINTS_SIZE"	, _TL("Size"),
			_TL(""),
			2, 1, true
		);

		m_Parameters.Add_Bool("NODE_POINTS",
			"POINTS_OUTLINE", _TL("Outline"),
			_TL(""),
			false
		);

		m_Parameters.Add_Choice("NODE_POINTS",
			"POINTS_COLOR_FIELD", _TL("Color by Attribute"),
			_TL(""),
			sFields_Num, nFields_Num
		);

		m_Parameters.Add_Colors("NODE_POINTS",
			"POINTS_COLORS"	, _TL("Colors"),
			_TL("")
		);

		//-------------------------------------------------
		m_Parameters.Add_Choice("NODE_X",
			"X_FIELD"	, _TL("Values"),
			_TL(""),
			sFields_Num, nFields_Num
		);

		m_Parameters.Add_Choice("NODE_X",
			"X_LABEL"	, _TL("Label"),
			_TL(""),
			sFields_All, m_pTable->Get_Field_Count()
		);

		//-------------------------------------------------
		m_Parameters.Add_Bool("NODE_Y",
			"Y_SCALE_TO_X"	, _TL("Scale to X"),
			_TL(""),
			false
		);

		m_Parameters.Add_Double("Y_SCALE_TO_X",
			"Y_SCALE_RATIO"	, _TL("Ratio"),
			_TL(""),
			1.0, 0.0, true
		);

		m_Parameters.Add_Bool("NODE_Y",
			"Y_MIN_FIX"	, _TL("Fixed Minimum"),
			_TL(""),
			false
		);

		m_Parameters.Add_Double("Y_MIN_FIX",
			"Y_MIN_VAL"	, _TL("Value"),
			_TL(""),
			0.0
		);

		m_Parameters.Add_Bool("NODE_Y",
			"Y_MAX_FIX"	, _TL("Fixed Maximum"),
			_TL(""),
			false
		);

		m_Parameters.Add_Double("Y_MAX_FIX",
			"Y_MAX_VAL"	, _TL("Value"),
			_TL(""),
			1000.0
		);

		return( _Create() );
	}

	return( false );
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
void CVIEW_Table_Diagram_Control::OnDraw(wxDC &dc)
{
	wxBitmap	bmp(m_sDraw);

	wxMemoryDC	dcMem;

	dcMem.SelectObject(bmp);
	dcMem.SetBackground(*wxWHITE_BRUSH);
	dcMem.Clear();

	_Draw(dcMem, m_sDraw);

	dcMem.SelectObject(wxNullBitmap);

	dc.DrawBitmap(bmp, 0, 0);
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
	if( m_nFields < 1 )
	{
		DO_ERROR(_TL("invalid data set!"));
	}

	//-----------------------------------------------------
	bool	bLegend	= m_Parameters("LEGEND")->asBool();

	wxRect	r(
		wxPoint(rDC.GetLeft () +  80, rDC.GetTop   () + 10), bLegend ?
		wxPoint(rDC.GetRight() - 100, rDC.GetBottom() - 40) :
		wxPoint(rDC.GetRight() -  10, rDC.GetBottom() - 50)
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

		if( m_yScale > 0.0 )
		{
			dy	= dx * m_yScale;
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
	int	Type	= m_Parameters("TYPE")->asInt();

	for(int iField=0; iField<m_nFields; iField++)
	{
		switch( Type )
		{
		default: _Draw_Bars  (dc, r, dx, dy, iField); break;	// Bars
		case  1: _Draw_Lines (dc, r, dx, dy, iField); break;	// Lines
		case  2: _Draw_Lines (dc, r, dx, dy, iField);
		         _Draw_Points(dc, r, dx, dy, iField); break;	// Lines and Points
		case  3: _Draw_Points(dc, r, dx, dy, iField); break;	// Points
		case  4: _Draw_Points(dc, r, dx, dy, iField); break;	// Points with Colour Attribute
		}
	}

	//-----------------------------------------------------
	_Draw_Frame(dc, r, dx, dy);

	if( bLegend )
	{
		r	= wxRect(
			wxPoint(r  .GetRight(), rDC.GetTop   ()),
			wxPoint(rDC.GetRight(), rDC.GetBottom())
		);

		_Draw_Legend(dc, r);
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Frame(wxDC &dc, wxRect r, double dx, double dy)
{
	const int	dyFont		= 12,
				Precision	= 3;

	int			ix, iRecord, iStep, nSteps, iLabel;
	double		z, dz, dzStep;
	wxFont		Font;
	wxString	sLabel;

	//-----------------------------------------------------
	dc.SetPen(*wxBLACK);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	//-----------------------------------------------------
	Font	= dc.GetFont();
	Font.SetPointSize((int)(0.7 * dyFont));
	dc.SetFont(Font);

	if( m_xField < 0 )
	{
		iLabel	= m_Parameters("X_LABEL")->asInt();	if( iLabel >= m_pTable->Get_Field_Count() )	iLabel	= -1;
		iStep	= dx > dyFont ? 1 : (int)(1 + (dyFont + 5) / dx);

		for(iRecord=0; iRecord<m_pTable->Get_Count(); iRecord+=iStep)
		{
			ix	= r.GetLeft() + (int)(dx * iRecord);

			dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);

			if( iLabel >= 0 )
			{
				sLabel.Printf(m_pTable->Get_Record_byIndex(iRecord)->asString(iLabel));
			}
			else
			{
				sLabel.Printf("%d", iRecord);
			}

			Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45.0, sLabel);
		}
	}
	else
	{
		nSteps	=         r.GetWidth() / (dyFont + 5);
		dzStep	= (double)r.GetWidth() / nSteps;
		dz		=    (m_xMax - m_xMin) / nSteps;

		for(iStep=0, z=m_xMin; iStep<=nSteps; iStep++, z+=dz)
		{
			ix		= r.GetLeft()	+ (int)(dzStep * iStep);

			dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);

			if( m_pTable->Get_Field_Type(m_xField) == SG_DATATYPE_Date )
			{
				wxDateTime	Date(z);	// Julian Day Number

				sLabel	= Date.Format("%Y-%m-%d");
			}
			else
			{
				sLabel.Printf("%.*f", Precision, z);
			}

			Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45.0, sLabel);
		}
	}

	//-----------------------------------------------------
	nSteps	=         r.GetHeight() / (dyFont + 5);
	dzStep	= (double)r.GetHeight() / nSteps;
	dz		=     (m_yMax - m_yMin) / nSteps;

	for(iStep=0, z=m_yMin; iStep<=nSteps; iStep++, z+=dz)
	{
		ix		= r.GetBottom()	- (int)(dzStep * iStep);

		dc.DrawLine(r.GetLeft(), ix, r.GetLeft() - 5, ix);

		Draw_Text(dc, TEXTALIGN_TOPRIGHT, r.GetLeft() - 7, ix - dyFont / 2, wxString::Format("%.*f", Precision, z));
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Legend(wxDC &dc, wxRect r)
{
	const int	dyFont	= 12;
	const int	dyBox	= dyFont + 4;

	int	iField	= r.GetTop() + (r.GetHeight() / 2);

	r	= wxRect(
		wxPoint(r.GetLeft () + 5, iField - (m_nFields * dyBox) / 2),
		wxPoint(r.GetRight()    , iField + (m_nFields * dyBox) / 2)
	);

	wxRect	rBox(r.GetLeft(), r.GetTop(), 20, dyBox);

	wxFont	Font(dc.GetFont());
	Font.SetPointSize(dyFont);
	dc.SetFont(Font);

	dc.SetPen(*wxBLACK_PEN);

	//-----------------------------------------------------
	for(iField=0; iField<m_nFields; iField++)
	{
		rBox.SetY(r.GetTop() + iField * dyBox);

		wxBrush	Brush(dc.GetBrush());
		Brush.SetColour(Get_Color_asWX(m_Colors.Get_Color(m_Fields[iField])));
		dc.SetBrush(Brush);
		dc.DrawRectangle(rBox.GetX(), rBox.GetY(), rBox.GetWidth(), rBox.GetHeight());
		dc.DrawText(m_pTable->Get_Field_Name(m_Fields[iField]), rBox.GetRight() + 5, rBox.GetTop());
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DRAW_GET_XPOS	(r.GetLeft  () + (int)(dx * (m_xField >= 0 ? (pRecord->asDouble(m_xField) - m_xMin) : (double)iRecord)))
#define DRAW_GET_YPOS	(r.GetBottom() - (int)(dy * (                (pRecord->asDouble(  iField) - m_yMin)                  )))

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Points(wxDC &dc, wxRect r, double dx, double dy, int iField)
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
void CVIEW_Table_Diagram_Control::_Draw_Lines(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	if( m_pTable->Get_Count() > 1 )
	{
		iField	= m_Fields[iField];

		dc.SetPen  (wxPen  (Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxPENSTYLE_SOLID));
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
void CVIEW_Table_Diagram_Control::_Draw_Bars(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	int	dxa	= m_xField < 0 ? (int)(dx / m_nFields * iField)	: iField + 0;
	int	dxb	= m_xField < 0 ? (int)(dx / m_nFields)			: iField + 1;

	iField	= m_Fields[iField];

	dc.SetPen(wxPen(Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxPENSTYLE_SOLID));

	for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record_byIndex(iRecord);

		if( !pRecord->is_NoData(iField) && (m_pTable->Get_Selection_Count() < 1 || pRecord->is_Selected()) )
		{
			int	x	= DRAW_GET_XPOS + dxa;
			int	y	= DRAW_GET_YPOS;

			for(int xb=x+dxb; x<=xb; x++)
			{
				dc.DrawLine(x, r.GetBottom(), x, y);
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
	EVT_SIZE			(CVIEW_Table_Diagram::On_Size)
	EVT_KEY_DOWN		(CVIEW_Table_Diagram::On_Key_Down)

	EVT_MENU			(ID_CMD_DIAGRAM_PARAMETERS		, CVIEW_Table_Diagram::On_Parameters)
	EVT_MENU			(ID_CMD_DIAGRAM_SIZE_FIT		, CVIEW_Table_Diagram::On_Size_Fit)
	EVT_MENU			(ID_CMD_DIAGRAM_SIZE_INC		, CVIEW_Table_Diagram::On_Size_Inc)
	EVT_MENU			(ID_CMD_DIAGRAM_SIZE_DEC		, CVIEW_Table_Diagram::On_Size_Dec)
	EVT_MENU			(ID_CMD_DIAGRAM_TO_CLIPBOARD	, CVIEW_Table_Diagram::On_SaveToClipboard)
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
	m_pControl->SetSize(GetClientSize());

	if( m_pControl->Set_Parameters() )
	{
		m_pControl->Fit_Size();

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
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_FIT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_INC);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_DEC);
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

	switch( event.GetKeyCode() )
	{
	default:
		event.Skip();
		break;

	case WXK_UP:
		m_pControl->Inc_Size();
		break;

	case WXK_DOWN:
		m_pControl->Dec_Size();
		break;

	case 'C':
		if( event.GetModifiers() == wxMOD_CONTROL )
		{
			m_pControl->SaveToClipboard();
		}
		break;
	}
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
	m_pControl->Inc_Size();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Size_Dec(wxCommandEvent &event)
{
	m_pControl->Dec_Size();
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_SaveToClipboard(wxCommandEvent &event)
{
	m_pControl->SaveToClipboard();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
