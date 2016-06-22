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
#include <wx/window.h>
#include <wx/scrolwin.h>
#include <wx/clipbrd.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

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
	CVIEW_Table_Diagram_Control(wxWindow *pParent, class CWKSP_Table *pTable);
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

	double							m_xMin, m_xMax, m_yMin, m_yMax;

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

			Refresh();

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
	if( pParameter && pParameter->Get_Owner() )
	{
		CSG_Parameters	*pParameters	= pParameter->Get_Owner();

		CSG_String	s(pParameter->Get_Identifier());

		if( s.Find(wxT("FIELD_")) == 0 )
		{
			s.Replace(wxT("FIELD_"), wxT("COLOR_"));

			if( pParameters->Get_Parameter(s) )
			{
				pParameters->Get_Parameter(s)->Set_Enabled(pParameter->asBool());
			}
		}

		if( !s.Cmp(wxT("_DIAGRAM_TYPE")) )
		{
			pParameters->Get_Parameter("NODE_POINTS")->Set_Enabled(pParameter->asInt() >= 2);
		}

		if( !s.Cmp(wxT("_DIAGRAM_Y_MIN_FIX")) )
		{
			pParameters->Get_Parameter("_DIAGRAM_Y_MIN_VAL")->Set_Enabled(pParameter->asBool());
		}

		if( !s.Cmp(wxT("_DIAGRAM_Y_MAX_FIX")) )
		{
			pParameters->Get_Parameter("_DIAGRAM_Y_MAX_VAL")->Set_Enabled(pParameter->asBool());
		}
	}

	return( 0 );
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
		m_bFitSize	= m_Parameters("_DIAGRAM_FIT_SIZE")->asBool();

		m_xField	= _Get_Field_By_Name(m_Parameters("_DIAGRAM_X_FIELD")->asString());

		if( m_xField >= 0 && m_pTable->Get_Range(m_xField) > 0.0 )
		{
			m_xMin		= m_pTable->Get_Minimum(m_xField);
			m_xMax		= m_pTable->Get_Maximum(m_xField);
		}
		else
		{
			m_xMin		= 1;
			m_xMax		= 1 + m_pTable->Get_Count();
			m_xField	= -1;
		}

		//-------------------------------------------------
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if(	m_pTable->Get_Field_Type(iField) != SG_DATATYPE_String
			&&	m_Parameters(CSG_String::Format(SG_T("FIELD_%d"), iField))->asBool() )
			{
				m_Fields			= (int *)SG_Realloc(m_Fields, (m_nFields + 1) * sizeof(int));
				m_Fields[m_nFields]	= iField;

				m_Colors.Set_Color(iField, m_Parameters(CSG_String::Format(SG_T("COLOR_%d"), iField))->asColor());

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
		if( m_Parameters("_DIAGRAM_Y_MIN_FIX")->asBool() )
		{
			m_yMin	= m_Parameters("_DIAGRAM_Y_MIN_VAL")->asDouble();
		}

		if( m_Parameters("_DIAGRAM_Y_MAX_FIX")->asBool() )
		{
			m_yMax	= m_Parameters("_DIAGRAM_Y_MAX_VAL")->asDouble();
		}
	}

	Refresh();

	return( m_nFields > 0 );
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram_Control::_Initialize(void)
{
	_Destroy();

	//-----------------------------------------------------
	if( m_pTable && m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Count() > 0 )
	{
		CSG_String		sFields_All, sFields_Num;
		CSG_Parameter	*pNode;

		m_Structure.Create(m_pTable);

		m_Colors.Set_Count(m_pTable->Get_Field_Count());

		m_Parameters.Create(NULL, _TL("Properties"), _TL(""));
		m_Parameters.Set_Callback_On_Parameter_Changed(_On_Parameter_Changed);

		CSG_Parameter	*pGeneral	= m_Parameters.Add_Node(NULL	, "NODE_GENERAL"	, _TL("General")		, _TL(""));
		CSG_Parameter	*pPoints	= m_Parameters.Add_Node(NULL	, "NODE_POINTS"		, _TL("Points")		, _TL(""));
		CSG_Parameter	*pXAxis		= m_Parameters.Add_Node(NULL	, "NODE_X"			, _TL("X Axis")		, _TL(""));
		CSG_Parameter	*pYAxis		= m_Parameters.Add_Node(NULL	, "NODE_Y"			, _TL("Y Axis")		, _TL(""));
		CSG_Parameter	*pFields	= m_Parameters.Add_Node(NULL	, "NODE_FIELDS"		, _TL("Attributes")	, _TL(""));

		//-------------------------------------------------
		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( m_pTable->Get_Field_Type(iField) != SG_DATATYPE_String )
			{
				pNode	= m_Parameters.Add_Value(
					pFields, CSG_String::Format(SG_T("FIELD_%d"), iField), m_pTable->Get_Field_Name(iField),
					_TL("Show"),
					PARAMETER_TYPE_Bool, false
				);

				m_Parameters.Add_Value(
					pNode, CSG_String::Format(SG_T("COLOR_%d"), iField), SG_T(""),
					_TL("Color"),
					PARAMETER_TYPE_Color, m_Colors.Get_Color(iField)
				);

				sFields_Num	+= m_pTable->Get_Field_Name(iField) + CSG_String(SG_T("|"));
			}

			sFields_All	+= m_pTable->Get_Field_Name(iField) + CSG_String(SG_T("|"));
		}

		sFields_Num	+= _TL("<none>") + CSG_String(SG_T("|"));
		sFields_All	+= _TL("<none>") + CSG_String(SG_T("|"));

		//-------------------------------------------------
		m_Parameters.Add_Choice(
			pGeneral	, "_DIAGRAM_TYPE"		, _TL("Display Type"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|%s|"),
				_TL("Bars"),
				_TL("Lines"),
				_TL("Lines and Points"),
				_TL("Points")
			), 1
		);

		m_Parameters.Add_Font(
			pGeneral	, "_DIAGRAM_FONT"		, _TL("Font"),
			_TL("")
		);

		m_Parameters.Add_Value(
			pGeneral	, "_DIAGRAM_LEGEND"		, _TL("Legend"),
			_TL(""),
			PARAMETER_TYPE_Bool, true
		);

		m_Parameters.Add_Value(
			pGeneral	, "_DIAGRAM_FIT_SIZE"	, _TL("Fit Size to Window"),
			_TL(""),
			PARAMETER_TYPE_Bool, true
		);

		//-------------------------------------------------
		m_Parameters.Add_Value(
			pPoints		, "_POINTS_SIZE"		, _TL("Size"),
			_TL(""),
			PARAMETER_TYPE_Int, 2, 1, true
		);

		m_Parameters.Add_Value(
			pPoints		, "_POINTS_OUTLINE"		, _TL("Outline"),
			_TL(""),
			PARAMETER_TYPE_Bool, false
		);

		m_Parameters.Add_Choice(
			pPoints		, "_POINTS_COLOR_FIELD"	, _TL("Color by Attribute"),
			_TL(""),
			sFields_Num, m_pTable->Get_Field_Count()
		);

		m_Parameters.Add_Colors(
			pPoints		, "_POINTS_COLORS"		, _TL("Colors"),
			_TL("")
		);

		//-------------------------------------------------
		m_Parameters.Add_Choice(
			pXAxis		, "_DIAGRAM_X_FIELD"	, _TL("Values"),
			_TL(""),
			sFields_Num, m_pTable->Get_Field_Count() + 1
		);

		m_Parameters.Add_Choice(
			pXAxis		, "_DIAGRAM_X_LABEL"	, _TL("Label"),
			_TL(""),
			sFields_All, m_pTable->Get_Field_Count() + 1
		);

		//-------------------------------------------------
		pNode	= m_Parameters.Add_Value(
			pYAxis		, "_DIAGRAM_Y_MIN_FIX"	, _TL("Fixed Minimum"),
			_TL(""),
			PARAMETER_TYPE_Bool, false
		);

		m_Parameters.Add_Value(
			pNode		, "_DIAGRAM_Y_MIN_VAL"	, _TL("Value"),
			_TL(""),
			PARAMETER_TYPE_Double, 0.0
		);

		pNode	= m_Parameters.Add_Value(
			pYAxis		, "_DIAGRAM_Y_MAX_FIX"	, _TL("Fixed Maximum"),
			_TL(""),
			PARAMETER_TYPE_Bool, false
		);

		m_Parameters.Add_Value(
			pNode		, "_DIAGRAM_Y_MAX_VAL"	, _TL("Value"),
			_TL(""),
			PARAMETER_TYPE_Double, 1000.0
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
	_Draw(dc, wxRect(0, 0, m_sDraw.x, m_sDraw.y));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw(wxDC &dc, wxRect rDC)
{
	dc.SetFont(Get_Font(m_Parameters("_DIAGRAM_FONT")));

	if( m_nFields > 0 && m_yMax > m_yMin )
	{
		bool	bLegend	= m_Parameters("_DIAGRAM_LEGEND")->asBool();
		int		iField;
		double	dx, dy;
		wxRect	r;

		r	= bLegend
			? wxRect(	wxPoint(rDC.GetLeft()  +  80, rDC.GetTop()    + 10),
						wxPoint(rDC.GetRight() - 100, rDC.GetBottom() - 40))
			: wxRect(	wxPoint(rDC.GetLeft()  +  80, rDC.GetTop()    + 10),
						wxPoint(rDC.GetRight() -  10, rDC.GetBottom() - 50));

		dx	= m_xField < 0
			? (double)r.GetWidth()	/ (double)m_pTable->Get_Count()
			: (double)r.GetWidth()	/ (m_xMax - m_xMin);
		dy	= (double)r.GetHeight()	/ (m_yMax - m_yMin);

		//-------------------------------------------------
		if( dx > 0.0 && dy > 0.0 )
		{
			for(iField=0; iField<m_nFields; iField++)
			{
				switch( m_Parameters("_DIAGRAM_TYPE")->asInt() )
				{
				default:
				case 0:	_Draw_Bars  (dc, r, dx, dy, iField);	break;	// Bars
				case 1:	_Draw_Lines (dc, r, dx, dy, iField);	break;	// Lines
				case 2:	_Draw_Lines (dc, r, dx, dy, iField);
						_Draw_Points(dc, r, dx, dy, iField);	break;	// Lines and Points
				case 3:	_Draw_Points(dc, r, dx, dy, iField);	break;	// Points
				case 4:	_Draw_Points(dc, r, dx, dy, iField);	break;	// Points with Colour Attribute
				}
			}

			_Draw_Frame(dc, r, dx, dy);

			if( bLegend )
			{
				r	= wxRect(	wxPoint(r  .GetRight(), rDC.GetTop()),
								wxPoint(rDC.GetRight(), rDC.GetBottom()));

				_Draw_Legend(dc, r);
			}
		}
		else
		{
			Draw_Text(dc, TEXTALIGN_CENTER, rDC.GetLeft() + rDC.GetWidth() / 2, rDC.GetTop() + rDC.GetHeight() / 2,
				_TL("Invalid display size!")
			);
		}
	}
	else
	{
		Draw_Text(dc, TEXTALIGN_CENTER, rDC.GetLeft() + rDC.GetWidth() / 2, rDC.GetTop() + rDC.GetHeight() / 2,
			_TL("Invalid data set!")
		);
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
		iLabel	= m_Parameters("_DIAGRAM_X_LABEL")->asInt();	if( iLabel >= m_pTable->Get_Field_Count() )	iLabel	= -1;
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
				sLabel.Printf(wxT("%d"), iRecord);
			}

			Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45.0, sLabel);
		}
	}
	else
	{
		nSteps		= r.GetWidth()			/ (dyFont + 5);
		dzStep		= (double)r.GetWidth()	/ nSteps;
		dz			= (m_xMax - m_xMin)		/ nSteps;

		for(iStep=0, z=m_xMin; iStep<=nSteps; iStep++, z+=dz)
		{
			ix		= r.GetLeft()	+ (int)(dzStep * iStep);

			dc.DrawLine(ix, r.GetBottom(), ix, r.GetBottom() + 5);

			sLabel.Printf(wxT("%.*f"), Precision, z);

			Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45.0, sLabel);
		}
	}

	//-----------------------------------------------------
	nSteps		= r.GetHeight()			/ (dyFont + 5);
	dzStep		= (double)r.GetHeight()	/ nSteps;
	dz			= (m_yMax - m_yMin)		/ nSteps;

	for(iStep=0, z=m_yMin; iStep<=nSteps; iStep++, z+=dz)
	{
		ix		= r.GetBottom()	- (int)(dzStep * iStep);

		dc.DrawLine(r.GetLeft(), ix, r.GetLeft() - 5, ix);

		Draw_Text(dc, TEXTALIGN_TOPRIGHT, r.GetLeft() - 7, ix - dyFont / 2, wxString::Format(wxT("%.*f"), Precision, z));
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Legend(wxDC &dc, wxRect r)
{
	const int	dyFont	= 12,
				dyBox	= dyFont + 4;

	int		iField;
	wxRect	rBox;
	wxBrush	Brush;
	wxFont	Font;

	//-----------------------------------------------------
	iField	= r.GetTop() + (r.GetHeight() / 2);
	r		= wxRect(
				wxPoint(r.GetLeft() + 5, iField - (m_nFields * dyBox) / 2),
				wxPoint(r.GetRight()   , iField + (m_nFields * dyBox) / 2)
			);

	rBox	= wxRect(r.GetLeft(), r.GetTop(), 20, dyBox);

	Font	= dc.GetFont();
	Font.SetPointSize(dyFont);
	dc.SetFont(Font);

	dc.SetPen(*wxBLACK_PEN);

	//-----------------------------------------------------
	for(iField=0; iField<m_nFields; iField++)
	{
		rBox.SetY(r.GetTop() + iField * dyBox);

		Brush	= dc.GetBrush();
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
#define DRAW_GET_XPOS(i)	(r.GetLeft()   + (int)(dx * (m_xField >= 0 ? (m_pTable->Get_Record_byIndex(i)->asDouble(m_xField) - m_xMin) : (double)i)))
#define DRAW_GET_YPOS(i)	(r.GetBottom() - (int)(dy * (                (m_pTable->Get_Record_byIndex(i)->asDouble(  iField) - m_yMin)                  )))

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Points(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	bool	bOutline	= m_Parameters("_POINTS_OUTLINE")		->asBool();
	int		Size		= m_Parameters("_POINTS_SIZE")			->asInt();
	int		zField		= _Get_Field_By_Name(m_Parameters("_POINTS_COLOR_FIELD")->asString());

	iField	= m_Fields[iField];

	if( zField < 0 )
	{
		dc.SetPen  (wxPen  (bOutline ? *wxBLACK : Get_Color_asWX(m_Colors.Get_Color(iField))));
		dc.SetBrush(wxBrush(                      Get_Color_asWX(m_Colors.Get_Color(iField))));

		for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
		{
			if( !m_pTable->Get_Record_byIndex(iRecord)->is_NoData(iField) )
			{
				dc.DrawCircle(DRAW_GET_XPOS(iRecord), DRAW_GET_YPOS(iRecord), Size);
			}
		}
	}
	else
	{
		if( bOutline )
		{
			dc.SetPen(wxPen(Get_Color_asWX(m_Colors.Get_Color(iField))));
		}

		CSG_Colors	*pColors	= m_Parameters("_POINTS_COLORS")->asColors();
		double		zMin		= m_pTable->Get_Minimum(zField);
		double		dz			= pColors->Get_Count() / m_pTable->Get_Range(zField);

		for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
		{
			if( !m_pTable->Get_Record_byIndex(iRecord)->is_NoData(iField) )
			{
				int			iz	= (int)(dz * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(zField) - zMin));
				wxColour	ic	= Get_Color_asWX(pColors->Get_Color(iz < 0 ? 0 : (iz >= 255 ? 255 : iz)));

				if( !bOutline )
				{
					dc.SetPen(wxPen(ic));
				}

				dc.SetBrush(wxBrush(ic));

				dc.DrawCircle(DRAW_GET_XPOS(iRecord), DRAW_GET_YPOS(iRecord), Size);
			}
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Lines(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	if( m_pTable->Get_Count() >= 2 )
	{
		int		ix, iy, jx, jy;

		iField	= m_Fields[iField];

		dc.SetPen  (wxPen  (Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxPENSTYLE_SOLID));
		dc.SetBrush(wxBrush(Get_Color_asWX(m_Colors.Get_Color(iField)), wxBRUSHSTYLE_SOLID));

		ix	= DRAW_GET_XPOS(0);
		iy	= DRAW_GET_YPOS(0);

		for(int iRecord=1; iRecord<m_pTable->Get_Count(); iRecord++)
		{
			if( !m_pTable->Get_Record_byIndex(iRecord)->is_NoData(iField) )
			{
				jx	= ix;	ix	= DRAW_GET_XPOS(iRecord);
				jy	= iy;	iy	= DRAW_GET_YPOS(iRecord);

				dc.DrawLine(jx, jy, ix, iy);
			}
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Bars(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	int		x, y, xa, xb, dxa, dxb;

	dxb		= m_xField < 0 ? (int)(dx / m_nFields)			: iField + 1;
	dxa		= m_xField < 0 ? (int)(dx / m_nFields * iField)	: iField + 0;

	iField	= m_Fields[iField];

	dc.SetPen(wxPen(Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxPENSTYLE_SOLID));

	for(int iRecord=0; iRecord<m_pTable->Get_Count(); iRecord++)
	{
		if( !m_pTable->Get_Record_byIndex(iRecord)->is_NoData(iField) )
		{
			xa	= DRAW_GET_XPOS(iRecord) + dxa;	xb	= dxb + xa;
			y	= DRAW_GET_YPOS(iRecord);

			for(x=xa; x<=xb; x++)
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

	EVT_MENU			(ID_CMD_MAPS_SAVE_TO_CLIPBOARD	, CVIEW_Table_Diagram::On_SaveToClipboard)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Diagram::CVIEW_Table_Diagram(CWKSP_Table *pTable)
	: CVIEW_Base(pTable, ID_VIEW_TABLE_DIAGRAM, wxString::Format(wxT("%s [%s]"), _TL("Diagram"), pTable->Get_Name().c_str()), ID_IMG_WND_DIAGRAM, false)
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
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_SAVE_TO_CLIPBOARD);

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
