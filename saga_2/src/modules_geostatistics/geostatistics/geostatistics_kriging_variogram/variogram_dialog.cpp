
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//            geostatistics_kriging_variogram            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Variogram_Dialog.cpp                  //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgui_diagram.h"

#include "variogram_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVariogram_Diagram : public CSGUI_Diagram
{
public:
	CVariogram_Diagram(wxWindow *pParent, CSG_Trend *pVariogram);
	virtual ~CVariogram_Diagram(void)	{}


	CSG_Trend					*m_pVariogram;


private:

	virtual void				On_Draw					(wxDC &dc, wxRect rDraw);


	DECLARE_EVENT_TABLE()

};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVariogram_Diagram, CSGUI_Diagram)
END_EVENT_TABLE()

//---------------------------------------------------------
CVariogram_Diagram::CVariogram_Diagram(wxWindow *pParent, CSG_Trend *pVariogram)
	: CSGUI_Diagram(pParent)
{
	m_pVariogram	= pVariogram;
	m_xName			= _TL("Distance"); 
	m_yName			= _TL("Semi-Variance"); 

	if( m_pVariogram->Get_Data_Count() > 1 )
	{
		m_xMin	= m_xMax	= m_pVariogram->Get_Data_X(0);
		m_yMin	= m_yMax	= m_pVariogram->Get_Data_Y(0);

		for(int i=0; i<m_pVariogram->Get_Data_Count(); i++)
		{
			double	d;

			d	= m_pVariogram->Get_Data_X(i);
			if( d < m_xMin )	m_xMin	= d;	else if( d > m_xMax )	m_xMax	= d;

			d	= m_pVariogram->Get_Data_Y(i);
			if( d < m_yMin )	m_yMin	= d;	else if( d > m_yMax )	m_yMax	= d;
		}

		m_xMin	= m_yMin	= 0.0;
		m_xMax	+= 0.02 * m_xMax;
		m_yMax	+= 0.02 * m_yMax;
	}
}

//---------------------------------------------------------
void CVariogram_Diagram::On_Draw(wxDC &dc, wxRect rDraw)
{
	if( m_pVariogram->Get_Data_Count() > 1 )
	{
		int		i, ix, iy, jx, jy;
		double	x, dx;

		//-------------------------------------------------
		dc.SetPen  (*wxWHITE_PEN);
		dc.SetBrush(*wxBLACK_BRUSH);

		for(i=0; i<m_pVariogram->Get_Data_Count(); i++)
		{
			ix	= Get_xToScreen(m_pVariogram->Get_Data_X(i));
			iy	= Get_yToScreen(m_pVariogram->Get_Data_Y(i));

			dc.DrawCircle(ix, iy, 4);
		}

		//-------------------------------------------------
		if( m_pVariogram->is_Okay() )
		{
			dc.SetPen(wxPen(*wxRED, 2));

			dx	= (m_xMax - m_xMin) / (double)rDraw.GetWidth();

			ix	= Get_xToScreen(m_xMin);
			iy	= Get_yToScreen(m_pVariogram->Get_Value(m_xMin));

			for(x=m_xMin+dx; x<=m_xMax; x+=dx)
			{
				jx	= ix;
				jy	= iy;
				ix	= Get_xToScreen(x);
				iy	= Get_yToScreen(m_pVariogram->Get_Value(x));

				dc.DrawLine(jx, jy, ix, iy);
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
BEGIN_EVENT_TABLE(CVariogram_Dialog, CSGUI_Dialog)
	EVT_CHOICE		(wxID_ANY	, CVariogram_Dialog::On_Update_Choices)
	EVT_TEXT_ENTER	(wxID_ANY	, CVariogram_Dialog::On_Update_Text)
END_EVENT_TABLE()

//---------------------------------------------------------
CVariogram_Dialog::CVariogram_Dialog(CSG_Trend *pVariogram)
	: CSGUI_Dialog(_TL("Semi-Variogram"))
{
	wxArrayString	Formulas;

	Formulas.Empty();
	Formulas.Add(SG_T("a + b * x"));
	Formulas.Add(SG_T("a + b * x + c * x^2"));
	Formulas.Add(SG_T("a + b * x + c * x^2 + d * x^3"));
	Formulas.Add(SG_T("a + b * x + c * x^2 + d * x^3 + e * x^4"));
	Formulas.Add(SG_T("a + b * ln(x)"));
	Formulas.Add(SG_T("a + b * x^c"));
	Formulas.Add(SG_T("a + b / x"));
	Formulas.Add(SG_T("a + b * sqrt(c + x)"));
	Formulas.Add(SG_T("a * (1 - exp(-(abs(x) / b)^2))"));

	//-----------------------------------------------------
	Add_Button(_TL("Ok")		, wxID_OK);
	Add_Button(_TL("Cancel")	, wxID_CANCEL);

	Add_Spacer();

	m_pFormulas		= Add_Choice(_TL("Predefined Functions"), Formulas, 0);

	Add_Spacer();

	m_pParameters	= Add_TextCtrl(_TL("Function Parameters"), wxTE_MULTILINE|wxTE_READONLY);

	Add_Output(
		m_pDiagram = new CVariogram_Diagram(this, pVariogram),
		m_pFormula = new wxTextCtrl(this, wxID_ANY, Formulas[0], wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER),
		1, 0
	);

	//-----------------------------------------------------
	Fit_Function();
}

//---------------------------------------------------------
CVariogram_Dialog::~CVariogram_Dialog(void)
{}

//---------------------------------------------------------
void CVariogram_Dialog::On_Update_Choices(wxCommandEvent &WXUNUSED(event))
{
	m_pFormula->SetValue(m_pFormulas->GetStringSelection().c_str());

	Fit_Function();
}

//---------------------------------------------------------
void CVariogram_Dialog::On_Update_Text(wxCommandEvent &WXUNUSED(event))
{
	Fit_Function();
}

//---------------------------------------------------------
void CVariogram_Dialog::Fit_Function(void)
{
	if(	!m_pDiagram->m_pVariogram->Set_Formula(m_pFormula->GetValue().c_str()) )
	{
		m_pParameters->SetValue(_TL("Invalid formula !!!"));
	}
	else if( !m_pDiagram->m_pVariogram->Get_Trend() )
	{
		m_pParameters->SetValue(_TL("Function fitting failed !!!"));
	}
	else
	{
		m_pParameters->SetValue(m_pDiagram->m_pVariogram->Get_Formula().c_str());
		m_pDiagram->Refresh();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
