
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
#include <wx/window.h>
#include <wx/scrolwin.h>

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

	virtual void					OnDraw				(wxDC &dc);


private:

	bool							m_bFitSize;

	int								m_nFields, *m_Fields, m_xField;

	double							m_xMin, m_xMax, m_yMin, m_yMax;

	CSG_Colors						m_Colors;

	CSG_Parameters					m_Parameters;

	CSG_Table						*m_pTable, m_Structure;

	wxSize							m_sDraw;

	wxFont							*m_pFont;

	wxImage							m_Image;


	void							On_Size				(wxSizeEvent  &event);
	void							On_Mouse_LDown		(wxMouseEvent &event);
	void							On_Mouse_RDown		(wxMouseEvent &event);

	void							_Destroy			(void);
	bool							_Create				(void);
	bool							_Initialize			(void);

	void							_Draw				(wxDC &dc, wxRect r);
	void							_Draw_Frame			(wxDC &dc, wxRect r, double dx, double dy);
	void							_Draw_Legend		(wxDC &dc, wxRect r);
	void							_Draw_Line			(wxDC &dc, wxRect r, double dx, double dy, int iField, bool bLine, bool bPoint);
	void							_Draw_Bars			(wxDC &dc, wxRect r, double dx, double dy, int iField);


	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_Table_Diagram_Control)

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

	m_pFont		= NULL;

	m_bFitSize	= false;

	Fit_Size();

	_Initialize();

	Set_Parameters();
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
			m_sDraw.x	= Size.x;
			m_sDraw.y	= Size.y;
		
			SetScrollbars(SCROLL_RATE, SCROLL_RATE, (m_sDraw.x + SCROLL_BAR_DX) / SCROLL_RATE, (m_sDraw.y + SCROLL_BAR_DY) / SCROLL_RATE);

			Refresh();
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
		const wxChar	*sField;
		int				iField;

		m_bFitSize	= m_Parameters("_DIAGRAM_FIT_SIZE")->asBool();

		iField		= m_Parameters("_DIAGRAM_X_FIELD")->asInt();
		sField		= m_Parameters("_DIAGRAM_X_FIELD")->asChoice()->Get_Item(iField);
		m_xField	= -1;

		for(iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( SG_STR_CMP(sField, m_pTable->Get_Field_Name(iField)) == 0 )
			{
				m_xMin	= m_pTable->Get_MinValue(iField);
				m_xMax	= m_pTable->Get_MaxValue(iField);

				if( m_xMin < m_xMax )
				{
					m_xField	= iField;
				}
				break;
			}
		}

		if( m_xField < 0 )
		{
			m_xMin	= 1;
			m_xMax	= 1 + m_pTable->Get_Record_Count();
		}

		//-------------------------------------------------
		for(iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if(	m_pTable->Get_Field_Type(iField) != TABLE_FIELDTYPE_String
			&&	m_Parameters(wxString::Format(wxT("FIELD_%d"), iField))->asBool() )
			{
				m_Fields			= (int *)SG_Realloc(m_Fields, (m_nFields + 1) * sizeof(int));
				m_Fields[m_nFields]	= iField;

				m_Colors.Set_Color(iField, m_Parameters(wxString::Format(wxT("COLOR_%d"), iField))->asColor());

				m_nFields++;

				if( m_nFields == 1 )
				{
					m_yMin	= m_pTable->Get_MinValue(iField);
					m_yMax	= m_pTable->Get_MaxValue(iField);
				}
				else
				{
					if( m_yMin	> m_pTable->Get_MinValue(iField) )
						m_yMin	= m_pTable->Get_MinValue(iField);

					if( m_yMax	< m_pTable->Get_MaxValue(iField) )
						m_yMax	= m_pTable->Get_MaxValue(iField);
				}
			}
		}

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
	if( m_pTable && m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Record_Count() > 0 )
	{
		CSG_String		sFields_All, sFields_Num;
		CSG_Parameter	*pNode, *pFields, *pColors;

		m_Structure.Create(m_pTable);

		m_Colors.Set_Count(m_pTable->Get_Field_Count());

		m_Parameters.Create(NULL, LNG("[CAP] Properties"), LNG(""));

		pFields	= m_Parameters.Add_Node(NULL, "NODE_FIELDS", LNG("[CAP] Attributes")	, LNG(""));
		pColors	= m_Parameters.Add_Node(NULL, "NODE_COLORS", LNG("[CAP] Colors")		, LNG(""));

		for(int iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( m_pTable->Get_Field_Type(iField) != TABLE_FIELDTYPE_String )
			{
				m_Parameters.Add_Value(
					pFields, wxString::Format(wxT("FIELD_%d"), iField), m_pTable->Get_Field_Name(iField),
					LNG("[CAP] Show"),
					PARAMETER_TYPE_Bool, false
				);

				m_Parameters.Add_Value(
					pColors, wxString::Format(wxT("COLOR_%d"), iField), m_pTable->Get_Field_Name(iField),
					LNG("[CAP] Color"),
					PARAMETER_TYPE_Color, m_Colors.Get_Color(iField)
				);

				sFields_Num.Append(m_pTable->Get_Field_Name(iField));
				sFields_Num.Append(wxT("|"));
			}

			sFields_All.Append(m_pTable->Get_Field_Name(iField));
			sFields_All.Append(wxT("|"));
		}

		//-------------------------------------------------
		pNode	= m_Parameters.Add_Node(
			NULL, "GENERAL", LNG("[CAP] General"),
			LNG("")
		);

		m_Parameters.Add_Choice(
			pNode, "_DIAGRAM_TYPE"		, LNG("[CAP] Display Type"),
			LNG(""),
			wxString::Format(wxT("%s|%s|%s|%s|"),
				LNG("Bars"),
				LNG("Lines and Points"),
				LNG("Lines"),
				LNG("Points")
			), 1
		);

		m_pFont	= m_Parameters.Add_Font(
			pNode, "_DIAGRAM_FONT"		, LNG("[CAP] Font"),
			LNG("")
		)->asFont();

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_LEGEND"	, LNG("[CAP] Legend"),
			LNG(""),
			PARAMETER_TYPE_Bool, true
		);

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_FIT_SIZE"	, LNG("[CAP] Fit Size to Window"),
			LNG(""),
			PARAMETER_TYPE_Bool, true
		);

		sFields_Num.Append(LNG("[CAP] [none]"));
		sFields_Num.Append(wxT("|"));

		//-------------------------------------------------
		pNode	= m_Parameters.Add_Node(
			m_Parameters("GENERAL"), "NODE_X", LNG("[CAP] X Axis"),
			LNG("")
		);

		m_Parameters.Add_Choice(
			pNode, "_DIAGRAM_X_FIELD"	, LNG("[CAP] Values"),
			LNG(""),
			sFields_Num, m_pTable->Get_Field_Count() + 1
		);

		sFields_All.Append(LNG("[CAP] [none]"));
		sFields_All.Append(wxT("|"));

		m_Parameters.Add_Choice(
			pNode, "_DIAGRAM_X_LABEL"	, LNG("[CAP] Label"),
			LNG(""),
			sFields_All, m_pTable->Get_Field_Count() + 1
		);

		//-------------------------------------------------
		pNode	= m_Parameters.Add_Node(
			m_Parameters("GENERAL"), "NODE_Y", LNG("[CAP] Y Axis"),
			LNG("")
		);

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_Y_MIN_FIX"	, LNG("[CAP] Fixed Minimum"),
			LNG(""),
			PARAMETER_TYPE_Bool, false
		);

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_Y_MIN_VAL"	, LNG("[CAP] Minimum Value"),
			LNG(""),
			PARAMETER_TYPE_Double, 0.0
		);

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_Y_MAX_FIX"	, LNG("[CAP] Fixed Maximum"),
			LNG(""),
			PARAMETER_TYPE_Bool, false
		);

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_Y_MAX_VAL"	, LNG("[CAP] Maximum Value"),
			LNG(""),
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
	if( m_pFont )
	{
		dc.SetFont(*m_pFont);
	}

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
			? (double)r.GetWidth()	/ (double)m_pTable->Get_Record_Count()
			: (double)r.GetWidth()	/ (m_xMax - m_xMin);
		dy	= (double)r.GetHeight()	/ (m_yMax - m_yMin);

		//-------------------------------------------------
		if( dx > 0.0 && dy > 0.0 )
		{
			_Draw_Frame(dc, r, dx, dy);

			for(iField=0; iField<m_nFields; iField++)
			{
				switch( m_Parameters("_DIAGRAM_TYPE")->asInt() )
				{
				default:
				case 0:	_Draw_Bars(dc, r, dx, dy, iField);					break;
				case 1:	_Draw_Line(dc, r, dx, dy, iField, true ,  true);	break;
				case 2:	_Draw_Line(dc, r, dx, dy, iField, true , false);	break;
				case 3:	_Draw_Line(dc, r, dx, dy, iField, false,  true);	break;
				}
			}

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
				LNG("[ERR] Invalid display size!")
			);
		}
	}
	else
	{
		Draw_Text(dc, TEXTALIGN_CENTER, rDC.GetLeft() + rDC.GetWidth() / 2, rDC.GetTop() + rDC.GetHeight() / 2,
			LNG("[ERR] Invalid data set!")
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
	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	//-----------------------------------------------------
	Font	= dc.GetFont();
	Font.SetPointSize((int)(0.7 * dyFont));
	dc.SetFont(Font);

	if( m_xField < 0 )
	{
		iLabel	= m_Parameters("_DIAGRAM_X_LABEL")->asInt();	if( iLabel >= m_pTable->Get_Field_Count() )	iLabel	= -1;
		iStep	= dx > dyFont ? 1 : (int)(1 + (dyFont + 5) / dx);

		for(iRecord=0; iRecord<m_pTable->Get_Record_Count(); iRecord+=iStep)
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
void CVIEW_Table_Diagram_Control::_Draw_Line(wxDC &dc, wxRect r, double dx, double dy, int iField, bool bLine, bool bPoint)
{
	int		iRecord, ix, iy, jx, jy;
	wxPen	Pen;
	wxBrush	Brush;

	iField	= m_Fields[iField];

	Pen		= wxPen  (Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxSOLID);
	dc.SetPen(Pen);

	Brush	= wxBrush(Get_Color_asWX(m_Colors.Get_Color(iField)), wxSOLID);
	dc.SetBrush(Brush);

	if( bLine && m_pTable->Get_Record_Count() > 1 )
	{
		ix	= m_xField < 0
			? r.GetLeft()	+ 0
			: r.GetLeft()	+ (int)(dx * (m_pTable->Get_Record_byIndex(0)->asDouble(m_xField) - m_xMin));
		iy	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(0)->asDouble(  iField) - m_yMin));

		for(iRecord=1; iRecord<m_pTable->Get_Record_Count(); iRecord++)
		{
			jx	= ix;
			jy	= iy;
			ix	= m_xField < 0
				? r.GetLeft()	+ (int)(dx * iRecord)
				: r.GetLeft()	+ (int)(dx * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(m_xField) - m_xMin));
			iy	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(  iField) - m_yMin));

			dc.DrawLine(jx, jy, ix, iy);
		}
	}

	if( bPoint )
	{
		for(iRecord=0; iRecord<m_pTable->Get_Record_Count(); iRecord++)
		{
			ix	= m_xField < 0
				? r.GetLeft()	+ (int)(dx * iRecord)
				: r.GetLeft()	+ (int)(dx * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(m_xField) - m_xMin));
			iy	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(  iField) - m_yMin));

			dc.DrawCircle(ix, iy, 2);
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram_Control::_Draw_Bars(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	int		iRecord, x, y, xa, xb, dxa, dxb;
	wxPen	Pen;

	dxb		= m_xField < 0 ? (int)(dx / m_nFields)			: iField + 1;
	dxa		= m_xField < 0 ? (int)(dx / m_nFields * iField)	: iField + 0;

	iField	= m_Fields[iField];

	Pen		= wxPen  (Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxSOLID);
	dc.SetPen(Pen);

	for(iRecord=0; iRecord<m_pTable->Get_Record_Count(); iRecord++)
	{
		xa	= dxa + r.GetLeft() + (m_xField < 0
			? (int)(dx * iRecord)
			: (int)(dx * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(m_xField) - m_xMin)));
		xb	= dxb + xa;

		y	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(iField) - m_yMin));

		for(x=xa; x<=xb; x++)
		{
			dc.DrawLine(x, r.GetBottom(), x, y);
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
	EVT_MENU			(ID_CMD_DIAGRAM_PARAMETERS	, CVIEW_Table_Diagram::On_Parameters)
	EVT_MENU			(ID_CMD_DIAGRAM_SIZE_FIT	, CVIEW_Table_Diagram::On_Size_Fit)
	EVT_MENU			(ID_CMD_DIAGRAM_SIZE_INC	, CVIEW_Table_Diagram::On_Size_Inc)
	EVT_MENU			(ID_CMD_DIAGRAM_SIZE_DEC	, CVIEW_Table_Diagram::On_Size_Dec)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Diagram::CVIEW_Table_Diagram(CWKSP_Table *pTable)
	: CVIEW_Base(ID_VIEW_TABLE_DIAGRAM, wxString::Format(wxT("%s [%s]"), LNG("[CAP] Diagram"), pTable->Get_Name().c_str()), ID_IMG_WND_DIAGRAM, CVIEW_Table_Diagram::_Create_Menu(), LNG("[CAP] Diagram"))
{
	SYS_Set_Color_BG_Window(this);

	m_pOwner	= pTable;

	m_pControl	= new CVIEW_Table_Diagram_Control(this, pTable);

	wxCommandEvent	dummy;

	On_Size_Fit(dummy);
}

//---------------------------------------------------------
CVIEW_Table_Diagram::~CVIEW_Table_Diagram(void)
{
	m_pOwner->View_Closes(this);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Table_Diagram::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_PARAMETERS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_FIT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_INC);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_SIZE_DEC);

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

	CMD_ToolBar_Add(pToolBar, LNG("[CAP] Diagram"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Diagram::Update_Diagram(void)
{
	return( m_pControl->Update_Diagram() );
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
