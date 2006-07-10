
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
IMPLEMENT_CLASS(CVIEW_Table_Diagram, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_Table_Diagram, CVIEW_Base)
	EVT_PAINT			(CVIEW_Table_Diagram::On_Paint)
	EVT_SIZE			(CVIEW_Table_Diagram::On_Size)

	EVT_MENU			(ID_CMD_DIAGRAM_PARAMETERS	, CVIEW_Table_Diagram::On_Parameters)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_Table_Diagram::CVIEW_Table_Diagram(CWKSP_Table *pTable)
	: CVIEW_Base(ID_VIEW_TABLE_DIAGRAM, wxString::Format("%s [%s]", LNG("[CAP] Diagram"), pTable->Get_Name().c_str()), ID_IMG_WND_DIAGRAM, CVIEW_Table_Diagram::_Create_Menu(), LNG("[CAP] Diagram"))
{
	SYS_Set_Color_BG_Window(this);

	m_pOwner	= pTable;
	m_pTable	= pTable->Get_Table();

	m_nFields	= 0;
	m_Fields	= NULL;

	m_pFont		= NULL;

	_Initialize();
	_DLG_Parameters();
}

//---------------------------------------------------------
CVIEW_Table_Diagram::~CVIEW_Table_Diagram(void)
{
	m_pOwner->View_Closes(this);

	_Destroy_Fields();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_Table_Diagram::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu();

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DIAGRAM_PARAMETERS);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_Table_Diagram::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_TABLE_DIAGRAM);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DIAGRAM_PARAMETERS);

	CMD_ToolBar_Add(pToolBar, LNG("[CAP] Diagram"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Size(wxSizeEvent &WXUNUSED(event))
{
	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Paint(wxPaintEvent &event)
{
	wxRect		r(wxPoint(0, 0), GetClientSize());
	wxPaintDC	dc(this);

	Draw_Edge(dc, EDGE_STYLE_SUNKEN, r);

	Draw(dc, r);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_Table_Diagram::Update_Diagram(void)
{
	if( m_Structure.is_Compatible(m_pTable, true) )
	{
		_Set_Fields();
	}
	else
	{
		_Initialize();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::_Destroy_Fields(void)
{
	if( m_nFields > 0 )
	{
		API_Free(m_Fields);
		m_Fields	= NULL;
		m_nFields	= 0;
	}
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram::_Set_Fields(void)
{
	_Destroy_Fields();

	if( m_pTable && m_pTable->Get_Field_Count() > 0 )
	{
		const char	*sField;
		int			iField;

		iField		= m_Parameters("_DIAGRAM_XFIELD")->asInt();
		sField		= m_Parameters("_DIAGRAM_XFIELD")->asChoice()->Get_Item(iField);
		m_xField	= -1;

		for(iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( strcmp(sField, m_pTable->Get_Field_Name(iField)) == 0 )
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
			&&	m_Parameters(wxString::Format("FIELD_%d", iField))->asBool() )
			{
				m_Fields			= (int *)API_Realloc(m_Fields, (m_nFields + 1) * sizeof(int));
				m_Fields[m_nFields]	= iField;

				m_Colors.Set_Color(iField, m_Parameters(wxString::Format("COLOR_%d", iField))->asColor());

				m_nFields++;

				if( m_nFields == 1 )
				{
					m_zMin	= m_pTable->Get_MinValue(iField);
					m_zMax	= m_pTable->Get_MaxValue(iField);
				}
				else
				{
					if( m_zMin	> m_pTable->Get_MinValue(iField) )
						m_zMin	= m_pTable->Get_MinValue(iField);

					if( m_zMax	< m_pTable->Get_MaxValue(iField) )
						m_zMax	= m_pTable->Get_MaxValue(iField);
				}
			}
		}
	}

	Refresh();

	return( m_nFields > 0 );
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::_Initialize(void)
{
	int			iField;
	CAPI_String	sFields_All, sFields_Num;
	CParameter	*pNode, *pFields, *pColors;

	//-----------------------------------------------------
	_Destroy_Fields();

	if( m_pTable && m_pTable->Get_Field_Count() > 0 && m_pTable->Get_Record_Count() > 0 )
	{
		m_Structure.Create(m_pTable);

		m_Colors.Set_Count(m_pTable->Get_Field_Count());

		m_Parameters.Create(NULL, LNG("[CAP] Properties"), "");

		pFields	= m_Parameters.Add_Node(NULL, "NODE_FIELDS", LNG("[CAP] Attributes"), "");
		pColors	= m_Parameters.Add_Node(NULL, "NODE_COLORS", LNG("[CAP] Colors")	, "");

		for(iField=0; iField<m_pTable->Get_Field_Count(); iField++)
		{
			if( m_pTable->Get_Field_Type(iField) != TABLE_FIELDTYPE_String )
			{
				m_Parameters.Add_Value(
					pFields, wxString::Format("FIELD_%d", iField), m_pTable->Get_Field_Name(iField),
					LNG("[CAP] Show"),
					PARAMETER_TYPE_Bool, false
				);

				m_Parameters.Add_Value(
					pColors, wxString::Format("COLOR_%d", iField), m_pTable->Get_Field_Name(iField),
					LNG("[CAP] Color"),
					PARAMETER_TYPE_Color, m_Colors.Get_Color(iField)
				);

				sFields_Num.Append(m_pTable->Get_Field_Name(iField));
				sFields_Num.Append("|");
			}

			sFields_All.Append(m_pTable->Get_Field_Name(iField));
			sFields_All.Append("|");
		}

		pNode	= m_Parameters.Add_Node(
			NULL, "GENERAL", LNG("[CAP] General"),
			""
		);

		m_Parameters.Add_Choice(
			pNode, "_DIAGRAM_TYPE"		, LNG("[CAP] Display Type"),
			"",
			wxString::Format("%s|%s|%s|%s|",
				LNG("Bars"),
				LNG("Lines and Points"),
				LNG("Lines"),
				LNG("Points")
			), 1
		);

		m_pFont	= m_Parameters.Add_Font(
			pNode, "_DIAGRAM_FONT"		, LNG("[CAP] Font"),
			""
		)->asFont();

		m_Parameters.Add_Value(
			pNode, "_DIAGRAM_LEGEND"	, LNG("[CAP] Legend"),
			"",
			PARAMETER_TYPE_Bool, true
		);

		sFields_Num.Append(LNG("[CAP] [none]"));
		sFields_Num.Append("|");

		m_Parameters.Add_Choice(
			pNode, "_DIAGRAM_XFIELD"	, LNG("[CAP] X Axis"),
			"",
			sFields_Num, m_pTable->Get_Field_Count() + 1
		);

		sFields_All.Append(LNG("[CAP] [none]"));
		sFields_All.Append("|");

		m_Parameters.Add_Choice(
			pNode, "_DIAGRAM_LABEL"		, LNG("[CAP] X Label"),
			"",
			sFields_All, m_pTable->Get_Field_Count() + 1
		);

		_Set_Fields();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::On_Parameters(wxCommandEvent &event)
{
	_DLG_Parameters();
}

//---------------------------------------------------------
bool CVIEW_Table_Diagram::_DLG_Parameters(void)
{
	if( DLG_Parameters(&m_Parameters) )
	{
		return( _Set_Fields() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::Draw(wxDC &dc, wxRect rDraw)
{
	if( m_pFont )
	{
		dc.SetFont(*m_pFont);
	}

	_Draw_Diagram(dc, rDraw);
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::_Draw_Diagram(wxDC &dc, wxRect rDC)
{
	if( m_nFields > 0 && m_zMax > m_zMin )
	{
		bool	bLegend	= m_Parameters("_DIAGRAM_LEGEND")->asBool();
		int		iField;
		double	dx, dy;
		wxRect	r;

		r	= bLegend
			? wxRect(	wxPoint(rDC.GetLeft()  +  80, rDC.GetTop()    + 10),
						wxPoint(rDC.GetRight() - 100, rDC.GetBottom() - 40))
			: wxRect(	wxPoint(rDC.GetLeft()  +  80, rDC.GetTop()    + 10),
						wxPoint(rDC.GetRight() -  10, rDC.GetBottom() - 40));

		dx	= m_xField < 0
			? (double)r.GetWidth()	/ (double)m_pTable->Get_Record_Count()
			: (double)r.GetWidth()	/ (m_xMax - m_xMin);
		dy	= (double)r.GetHeight()	/ (m_zMax - m_zMin);

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
void CVIEW_Table_Diagram::_Draw_Frame(wxDC &dc, wxRect r, double dx, double dy)
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
		iLabel	= m_Parameters("_DIAGRAM_LABEL")->asInt();	if( iLabel >= m_pTable->Get_Field_Count() )	iLabel	= -1;
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
				sLabel.Printf("%d", iRecord);
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

			sLabel.Printf("%.*f", Precision, z);

			Draw_Text(dc, TEXTALIGN_CENTERRIGHT, ix, r.GetBottom() + 7, 45.0, sLabel);
		}
	}

	//-----------------------------------------------------
	nSteps		= r.GetHeight()			/ (dyFont + 5);
	dzStep		= (double)r.GetHeight()	/ nSteps;
	dz			= (m_zMax - m_zMin)		/ nSteps;

	for(iStep=0, z=m_zMin; iStep<=nSteps; iStep++, z+=dz)
	{
		ix		= r.GetBottom()	- (int)(dzStep * iStep);

		dc.DrawLine(r.GetLeft(), ix, r.GetLeft() - 5, ix);

		Draw_Text(dc, TEXTALIGN_TOPRIGHT, r.GetLeft() - 7, ix - dyFont / 2, wxString::Format("%.*f", Precision, z));
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::_Draw_Legend(wxDC &dc, wxRect r)
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_Table_Diagram::_Draw_Line(wxDC &dc, wxRect r, double dx, double dy, int iField, bool bLine, bool bPoint)
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
		iy	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(0)->asDouble(  iField) - m_zMin));

		for(iRecord=1; iRecord<m_pTable->Get_Record_Count(); iRecord++)
		{
			jx	= ix;
			jy	= iy;
			ix	= m_xField < 0
				? r.GetLeft()	+ (int)(dx * iRecord)
				: r.GetLeft()	+ (int)(dx * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(m_xField) - m_xMin));
			iy	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(  iField) - m_zMin));

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
			iy	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(  iField) - m_zMin));

			dc.DrawCircle(ix, iy, 2);
		}
	}
}

//---------------------------------------------------------
void CVIEW_Table_Diagram::_Draw_Bars(wxDC &dc, wxRect r, double dx, double dy, int iField)
{
	int		iRecord, x, y, xa, xb, dxa, dxb;
	wxPen	Pen;

	dxb		= (int)(dx / m_nFields);
	dxa		= (int)(dx / m_nFields * iField);

	iField	= m_Fields[iField];

	Pen		= wxPen  (Get_Color_asWX(m_Colors.Get_Color(iField)), 0, wxSOLID);
	dc.SetPen(Pen);

	for(iRecord=0; iRecord<m_pTable->Get_Record_Count(); iRecord++)
	{
		xa	= dxa + r.GetLeft() + (int)(dx * iRecord);
		xb	= dxb + xa;

		y	= r.GetBottom()	- (int)(dy * (m_pTable->Get_Record_byIndex(iRecord)->asDouble(iField) - m_zMin));

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
