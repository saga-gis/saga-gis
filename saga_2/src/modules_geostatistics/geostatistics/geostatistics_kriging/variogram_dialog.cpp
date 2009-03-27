
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
	CVariogram_Diagram(wxWindow *pParent, CSG_Trend *pVariogram, CSG_Table *pVariances);

	CSG_Trend					*m_pVariogram;


	void						Set_Trend				(double Distance, bool bCumulative);


private:

	bool						m_bCumulative;

	double						m_Distance;

	CSG_Table					*m_pVariances;


	virtual void				On_Draw					(wxDC &dc, wxRect rDraw);

};

//---------------------------------------------------------
CVariogram_Diagram::CVariogram_Diagram(wxWindow *pParent, CSG_Trend *pVariogram, CSG_Table *pVariances)
	: CSGUI_Diagram(pParent)
{
	m_xName			= _TL("Distance");
	m_yName			= _TL("Semivariance");

	m_pVariogram	= pVariogram;
	m_pVariances	= pVariances;

	m_bCumulative	= false;
	m_Distance		= -1.0;

	//-----------------------------------------------------
	m_xMin	= m_yMin	= 0.0;
	m_xMax	= m_pVariances->Get_MaxValue(0);
	m_yMax	= (1.0 + 0.02) * m_pVariances->Get_MaxValue(m_pVariances->Get_MaxValue(1) > m_pVariances->Get_MaxValue(2) ? 1 : 2);
}

//---------------------------------------------------------
void CVariogram_Diagram::Set_Trend(double Distance, bool bCumulative)
{
	if( bCumulative != m_bCumulative || Distance < 0 || Distance != m_Distance )
	{
		m_bCumulative	= bCumulative;
		m_Distance		= Distance;

		m_pVariogram->Clr_Data();

		for(int i=0; i<m_pVariances->Get_Count(); i++)
		{
			if( (*m_pVariances)[i][0] <= Distance )
			{
				m_pVariogram->Add_Data((*m_pVariances)[i][0], (*m_pVariances)[i][m_bCumulative ? 2 : 1]);
			}
		}
	}
}

//---------------------------------------------------------
void CVariogram_Diagram::On_Draw(wxDC &dc, wxRect rDraw)
{
	if( m_pVariances->Get_Count() > 0 )
	{
		int		i, ix, iy, jx, jy, k;
		double	x, dx;

		//-------------------------------------------------
		if( m_pVariogram->Get_Data_Count() > 0 )
		{
			ix	= Get_xToScreen(m_pVariogram->Get_Data_XMax());
			dc.SetPen  (wxPen(wxColour(  0, 127,   0), 2));
			dc.DrawLine(ix, Get_yToScreen(m_yMin), ix, Get_yToScreen(m_yMax));
		}

		//-------------------------------------------------
		dc.SetPen  (wxColour(191, 191, 191));
		dc.SetBrush(wxColour(191, 191, 191));

		for(i=0, k=m_bCumulative ? 1 : 2; i<m_pVariances->Get_Count(); i++)
		{
			ix	= Get_xToScreen((*m_pVariances)[i][0]);
			iy	= Get_yToScreen((*m_pVariances)[i][k]);

			dc.DrawCircle(ix, iy, 1);
		}

		//-------------------------------------------------
		dc.SetPen  (wxColour(127, 127, 127));
		dc.SetBrush(wxColour(  0,   0,   0));

		for(i=0, k=m_bCumulative ? 2 : 1; i<m_pVariances->Get_Count(); i++)
		{
			ix	= Get_xToScreen((*m_pVariances)[i][0]);
			iy	= Get_yToScreen((*m_pVariances)[i][k]);

			dc.DrawCircle(ix, iy, 3);
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
	EVT_CHECKBOX	(wxID_ANY	, CVariogram_Dialog::On_Update_Control)
	EVT_TEXT_ENTER	(wxID_ANY	, CVariogram_Dialog::On_Update_Control)
	EVT_SLIDER		(wxID_ANY	, CVariogram_Dialog::On_Update_Control)
	EVT_CHOICE		(wxID_ANY	, CVariogram_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
CVariogram_Dialog::CVariogram_Dialog(CSG_Trend *pVariogram, CSG_Table *pVariances)
	: CSGUI_Dialog(_TL("Semi-Variogram"))
{
	//-----------------------------------------------------
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
	Formulas.Add(SG_T("n + (s - n) * (1 - exp(-(x / r)^2))"));
	Formulas.Add(SG_T("n+(s-n)*ifelse(x>r,1,3*x/(2*r)-x^3/(2*r^3))"));

	//-----------------------------------------------------
	Add_Button(_TL("Ok")		, wxID_OK);
	Add_Button(_TL("Cancel")	, wxID_CANCEL);

	Add_Spacer();
	m_pCumulative	= Add_CheckBox	(_TL("Cumulative Variance"), false);

	Add_Spacer();
	m_pFormulas		= Add_Choice	(_TL("Predefined Functions"), Formulas, 0);

	Add_Spacer();
	m_pDistance		= Add_Slider	(_TL("Function Fitting Range"), pVariances->Get_MaxValue(0), 0.0, pVariances->Get_MaxValue(0));

	Add_Spacer();
	m_pParameters	= Add_TextCtrl	(_TL("Function Parameters"), wxTE_MULTILINE|wxTE_READONLY);

	//-----------------------------------------------------
	Add_Output(
		m_pDiagram = new CVariogram_Diagram(this, pVariogram, pVariances),
		m_pFormula = new wxTextCtrl(this, wxID_ANY, pVariogram->Get_Formula(SG_TREND_STRING_Formula).c_str(), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER),
		1, 0
	);

	Fit_Function();
}

//---------------------------------------------------------
void CVariogram_Dialog::On_Update_Control(wxCommandEvent &WXUNUSED(event))
{
	Fit_Function();
}

void CVariogram_Dialog::On_Update_Choices(wxCommandEvent &WXUNUSED(event))
{
	m_pFormula->SetValue(m_pFormulas->GetStringSelection().c_str());

	Fit_Function();
}

//---------------------------------------------------------
void CVariogram_Dialog::Fit_Function(void)
{
	wxString	s;

	m_pDiagram->Set_Trend(m_pDistance->Get_Value(), m_pCumulative->GetValue());

	if(	!m_pDiagram->m_pVariogram->Set_Formula(m_pFormula->GetValue().c_str()) )
	{
		s	+= _TL("invalid formula !");
	}
	else if( !m_pDiagram->m_pVariogram->Get_Trend() )
	{
		s	+= _TL("function fitting failed !");
	}
	else
	{
		s	+= m_pDiagram->m_pVariogram->Get_Formula(SG_TREND_STRING_Function).c_str();
		s	+= wxString::Format(wxT("\n%s: %.*f")	, _TL("Fitting range")		, SG_Get_Significant_Decimals(m_pDistance->Get_Value()), m_pDistance->Get_Value());
		s	+= wxString::Format(wxT("\n%s: %d")		, _TL("Samples in range")	, m_pDiagram->m_pVariogram->Get_Data_Count());
		s	+= wxString::Format(wxT("\n%s: %.2f%")	, _TL("R2")					, m_pDiagram->m_pVariogram->Get_R2() * 100.0);
	}

	m_pParameters->SetValue(s);

	m_pDiagram->Refresh(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
