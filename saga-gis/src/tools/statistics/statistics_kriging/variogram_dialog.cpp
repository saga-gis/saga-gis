/**********************************************************
 * Version $Id: variogram_dialog.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include <wx/dcmemory.h>

#include "variogram_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Variogram::CSG_Variogram(void)
{}

//---------------------------------------------------------
bool CSG_Variogram::Calculate(CSG_Shapes *pPoints, int Attribute, bool bLog, CSG_Table *pVariogram, int nClasses, double maxDistance, int nSkip)
{
	int		i, n;

	//-----------------------------------------------------
	if( nSkip < 1 )
	{
		nSkip		= 1;
	}

	if( maxDistance <= 0.0 || maxDistance > SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange()) )
	{
		maxDistance	= SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange());	// bounding box' diagonal
	}

	double	lagDistance	= maxDistance / nClasses;

	CSG_Vector	Count   (nClasses);
	CSG_Vector	Variance(nClasses);

	//-----------------------------------------------------
	for(i=0, n=0; i<pPoints->Get_Count()-nSkip && SG_UI_Process_Set_Progress(n, SG_Get_Square(pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Attribute) )
		{
			TSG_Point	p	= pPoint->Get_Point(0);
			double		z	= bLog ? log(pPoint->asDouble(Attribute)) : pPoint->asDouble(Attribute);

			for(int j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Attribute) )
				{
					int	k	= (int)(SG_Get_Distance(p, pPoint->Get_Point(0)) / lagDistance);

					if( k < nClasses )
					{
						Count	[k]	++;
						Variance[k]	+= SG_Get_Square((bLog ? log(pPoint->asDouble(Attribute)) : pPoint->asDouble(Attribute)) - z);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	pVariogram->Destroy();

	pVariogram->Set_Name(CSG_String::Format("%s [%s]", _TL("Variogram"), pPoints->Get_Name()));

	pVariogram->Add_Field(_TL("Class"   ), SG_DATATYPE_Int   );	// FIELD_CLASS
	pVariogram->Add_Field(_TL("Distance"), SG_DATATYPE_Double);	// FIELD_DISTANCE
	pVariogram->Add_Field(_TL("Count"   ), SG_DATATYPE_Int   );	// FIELD_COUNT
	pVariogram->Add_Field(_TL("Variance"), SG_DATATYPE_Double);	// FIELD_VAR_EXP
	pVariogram->Add_Field(_TL("Var.cum."), SG_DATATYPE_Double);	// FIELD_VAR_CUM
	pVariogram->Add_Field(_TL("Model"   ), SG_DATATYPE_Double);	// FIELD_VAR_MODEL

	double	z	= 0.0;

	for(i=0, n=0; i<nClasses; i++)
	{
		if( Count[i] > 0 )
		{
			n	+= (int)Count[i];
			z	+= Variance[i];

			CSG_Table_Record	*pRecord	= pVariogram->Add_Record();

			pRecord->Set_Value(FIELD_CLASS   , (i + 1));
			pRecord->Set_Value(FIELD_DISTANCE, (i + 1) * lagDistance);
			pRecord->Set_Value(FIELD_COUNT   , Count[i]);
			pRecord->Set_Value(FIELD_VAR_EXP , 0.5 * Variance[i] / Count[i]);
			pRecord->Set_Value(FIELD_VAR_CUM , 0.5 * z / n);
		}
	}

	//-----------------------------------------------------
	return( SG_UI_Process_Get_Okay() );
}

//---------------------------------------------------------
double CSG_Variogram::Get_Lag_Distance(CSG_Shapes *pPoints, int Method, int nSkip)
{
	if( Method == 0 )
	{
		CSG_PRQuadTree			QT(pPoints, 0);
		CSG_Simple_Statistics	s;

		if( nSkip < 1 )	{	nSkip	= 1;	}

		for(int iPoint=0; iPoint<pPoints->Get_Count() && ::SG_UI_Process_Set_Progress(iPoint, pPoints->Get_Count()); iPoint+=nSkip)
		{
			TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

			double	x, y, z;

			if( QT.Select_Nearest_Points(p.x, p.y, 2) && QT.Get_Selected_Point(1, x, y, z) && (x != p.x || y != p.y) )
			{
				s.Add_Value(SG_Get_Distance(x, y, p.x, p.y));
			}
		}

		if( s.Get_Count() > 0 && s.Get_Mean() > 0.0 )
		{
			return( s.Get_Mean() );
		}
	}

	return( 0.25 * sqrt((pPoints->Get_Extent().Get_XRange() * pPoints->Get_Extent().Get_YRange()) / pPoints->Get_Count()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVariogram_Diagram : public CSGDI_Diagram
{
public:
	CVariogram_Diagram(wxWindow *pParent);


	bool						m_bPairs, m_bErrors;


	void						Initialize				(CSG_Trend *pModel, CSG_Table *pVariogram);

	void						Set_Variogram			(void);


private:

	CSG_Table					*m_pVariogram;

	CSG_Trend					*m_pModel;


	virtual void				On_Draw					(wxDC &dc, wxRect rDraw);

};

//---------------------------------------------------------
CVariogram_Diagram::CVariogram_Diagram(wxWindow *pParent)
	: CSGDI_Diagram(pParent)
{
	m_xName			= _TL("Distance");
	m_yName			= _TL("Variance");

	m_pModel		= NULL;
	m_pVariogram	= NULL;

	m_bPairs		= false;
	m_bErrors		= false;
}

//---------------------------------------------------------
void CVariogram_Diagram::Initialize(CSG_Trend *pModel, CSG_Table *pVariogram)
{
	m_pModel		= pModel;
	m_pVariogram	= pVariogram;
}

//---------------------------------------------------------
void CVariogram_Diagram::Set_Variogram(void)
{
	m_xMin	= m_yMin	= 0.0;
	m_xMax	= (1.0 + 0.00) * m_pVariogram->Get_Maximum(CSG_Variogram::FIELD_DISTANCE);
	m_yMax	= (1.0 + 0.02) * m_pVariogram->Get_Maximum(CSG_Variogram::FIELD_VAR_EXP );
}

//---------------------------------------------------------
void CVariogram_Diagram::On_Draw(wxDC &dc, wxRect rDraw)
{
	m_bErrors	= false;

	if( m_pVariogram->Get_Count() > 0 && rDraw.GetWidth() > 0 && rDraw.GetHeight() > 0 )
	{
		int		i, ix, iy, jx, jy;
		double	x, dx;

		//-------------------------------------------------
		if( m_pModel->Get_Data_Count() > 0 && !dc.IsKindOf(wxCLASSINFO(wxMemoryDC)) )
		{
			ix	= Get_xToScreen(m_pModel->Get_Data_XMax());
			
			if( ix < rDraw.GetRight() )
			{
				dc.SetPen  (wxPen(wxColour(  0, 127,   0), 2));
				dc.DrawLine(ix, Get_yToScreen(m_yMin), ix, Get_yToScreen(m_yMax));
			}
		}

		//-------------------------------------------------
		if( m_bPairs && m_pVariogram->Get_Maximum(CSG_Variogram::FIELD_COUNT) > 0 )
		{
			double	dScale	= m_yMax / m_pVariogram->Get_Maximum(CSG_Variogram::FIELD_COUNT);

			dc.SetPen  (wxColour(191, 191, 191));
			dc.SetBrush(wxColour(191, 191, 191));

			for(i=0; i<m_pVariogram->Get_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= m_pVariogram->Get_Record(i);

				ix	= Get_xToScreen(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE));
				iy	= Get_yToScreen(pRecord->asDouble(CSG_Variogram::FIELD_COUNT   ) * dScale);

				dc.DrawCircle(ix, iy, 3);
			}
		}

		//-------------------------------------------------
		dc.SetPen  (wxColour(127, 127, 127));
		dc.SetBrush(wxColour(  0,   0,   0));

		for(i=0; i<m_pVariogram->Get_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= m_pVariogram->Get_Record(i);

			ix	= Get_xToScreen(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE));
			iy	= Get_yToScreen(pRecord->asDouble(CSG_Variogram::FIELD_VAR_EXP ));

			dc.DrawCircle(ix, iy, 3);
		}

		//-------------------------------------------------
		if( m_pModel->is_Okay() )
		{
			dx	= (m_xMax - m_xMin) / (double)rDraw.GetWidth();

			ix	= Get_xToScreen(m_xMin);
			iy	= Get_yToScreen(m_pModel->Get_Value(m_xMin));

			int	yMin	= Get_yToScreen(m_yMin);
			int	yMax	= yMin;

			if( iy > yMin )
			{
				iy	= yMin;
			}

			for(x=m_xMin+dx; x<=m_xMax; x+=dx)
			{
				jx	= ix;	ix	= Get_xToScreen(x);
				jy	= iy;	iy	= Get_yToScreen(m_pModel->Get_Value(x));

				if( yMax >= iy )
				{
					yMax	= iy;

					dc.SetPen(wxPen(*wxBLUE, 2));
				}
				else
				{
					dc.SetPen(wxPen(*wxRED , 2));

					if( iy > yMin )
					{
						iy	= yMin;
					}

					m_bErrors	= true;
				}

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
BEGIN_EVENT_TABLE(CVariogram_Dialog, CSGDI_Dialog)
	EVT_BUTTON		(wxID_ANY	, CVariogram_Dialog::On_Button)
	EVT_CHECKBOX	(wxID_ANY	, CVariogram_Dialog::On_Update_Control)
	EVT_TEXT_ENTER	(wxID_ANY	, CVariogram_Dialog::On_Update_Control)
	EVT_SLIDER		(wxID_ANY	, CVariogram_Dialog::On_Update_Control)
	EVT_CHOICE		(wxID_ANY	, CVariogram_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
CVariogram_Dialog::CVariogram_Dialog(void)
	: CSGDI_Dialog(_TL("Variogram"))
{
	m_pPoints		= NULL;
	m_nPoints		= 0;
	m_Attribute		= 0;
	m_pVariogram	= NULL;
	m_pModel		= NULL;
	m_Distance		= -1;

	//-----------------------------------------------------
	wxArrayString	Formulas;

	Formulas.Add("linear"            );
	Formulas.Add("linear (no nugget)");
	Formulas.Add("square root"       );
	Formulas.Add("logarithmic"       );
	Formulas.Add("exponential"       );
	Formulas.Add("gaussian"          );
	Formulas.Add("spherical"         );

	//-----------------------------------------------------
	Add_Button(_TL("Ok"    ), wxID_OK);
	Add_Button(_TL("Cancel"), wxID_CANCEL);

	Add_Spacer(); m_pSettings   = Add_Button  (_TL("Settings"              ), wxID_ANY);
	Add_Spacer(); m_pPairs      = Add_CheckBox(_TL("Number of Pairs"       ), false);
	Add_Spacer(); m_pFormulas   = Add_Choice  (_TL("Predefined Functions"  ), Formulas, 0);
	Add_Spacer(); m_pDistance   = Add_Slider  (_TL("Function Fitting Range"), 1, 0, 1);
	Add_Spacer(); m_pSummary    = Add_TextCtrl(_TL("Summary"               ), wxTE_MULTILINE|wxTE_READONLY);

	//-----------------------------------------------------
	Add_Output(
		m_pDiagram = new CVariogram_Diagram(this),
		m_pFormula = new wxTextCtrl(this, wxID_ANY, Get_Formula(0), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER),
		1, 0
	);

	//-----------------------------------------------------
	m_Settings.Set_Name(_TL("Variogram Settings"));

	m_Settings.Add_Int   ("", "SKIP"   , _TL("Skip"            ), _TL(""), 1,   1, true);
	m_Settings.Add_Double("", "LAGDIST", _TL("Lag Distance"    ), _TL(""), 1, 0.0, true);
	m_Settings.Add_Double("", "MAXDIST", _TL("Maximum Distance"), _TL(""), 1, 0.0, true);
	m_Settings.Add_String("", "MODEL"  , _TL("Model"           ), _TL(""), &Formulas[0]);
}

//---------------------------------------------------------
bool CVariogram_Dialog::Execute(CSG_Shapes *pPoints, int Attribute, bool bLog, CSG_Table *pVariogram, CSG_Trend *pModel)
{
	if( m_pPoints != pPoints || m_nPoints != pPoints->Get_Count() || !m_Extent.is_Equal(pPoints->Get_Extent()) )
	{
		m_pPoints	= pPoints;
		m_nPoints	= pPoints->Get_Count();
		m_Extent	= pPoints->Get_Extent();
		m_Distance	= -1;

		int	nSkip	= 1 + m_pPoints->Get_Count() / 10000;

		m_Settings("SKIP"   )->Set_Value(nSkip);
		m_Settings("LAGDIST")->Set_Value(CSG_Variogram::Get_Lag_Distance(m_pPoints, 0, nSkip));
		m_Settings("MAXDIST")->Set_Value(0.5 * sqrt(SG_Get_Square(m_pPoints->Get_Extent().Get_XRange()) + SG_Get_Square(m_pPoints->Get_Extent().Get_YRange())));
	}

	m_Attribute		= Attribute;
	m_bLog			= bLog;
	m_pVariogram	= pVariogram;
	m_pModel		= pModel;
	m_Distance		= -1.0;

	m_pDiagram->Initialize(m_pModel, m_pVariogram);

	//-----------------------------------------------------
	Set_Variogram();

	return( ShowModal() == wxID_OK && m_pModel && m_pModel->is_Okay() );
}

//---------------------------------------------------------
void CVariogram_Dialog::On_Update_Control(wxCommandEvent &WXUNUSED(event))
{
	Set_Model();
}

void CVariogram_Dialog::On_Update_Choices(wxCommandEvent &WXUNUSED(event))
{
	m_pFormula->SetValue(Get_Formula(m_pFormulas->GetSelection()));

	Set_Model();
}

//---------------------------------------------------------
void CVariogram_Dialog::On_Button(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pSettings )
	{
		if( SG_UI_Dlg_Parameters(&m_Settings, m_Settings.Get_Name()) )
		{
			m_pFormula->SetValue(m_Settings("MODEL")->asString());

			Set_Variogram();
		}
	}
	else
	{
		event.Skip();
	}
}

//---------------------------------------------------------
void CVariogram_Dialog::Set_Variogram(void)
{
	double	lagDist	= m_Settings("LAGDIST")->asDouble();
	double	maxDist	= m_Settings("MAXDIST")->asDouble();

	if( lagDist > 0.0 )
	{
		double	Diagonal	= SG_Get_Length(m_pPoints->Get_Extent().Get_XRange(), m_pPoints->Get_Extent().Get_YRange());	// bounding box's diagonal

		if( maxDist <= 0.0 || maxDist > Diagonal )
		{
			m_Settings("MAXDIST")->Set_Value(maxDist = Diagonal);
		}

		CSG_Variogram::Calculate(m_pPoints, m_Attribute, m_bLog, m_pVariogram,
			1 + (int)(0.5 + maxDist / lagDist), maxDist, m_Settings("SKIP")->asInt()
		);

		m_pDistance->Set_Range(0.0, m_pVariogram->Get_Maximum(CSG_Variogram::FIELD_DISTANCE));
		m_pDistance->Set_Value(m_pVariogram->Get_Maximum(CSG_Variogram::FIELD_DISTANCE));

		m_pDiagram->Set_Variogram();

		Set_Model();
	}
}

//---------------------------------------------------------
void CVariogram_Dialog::Set_Model(void)
{
	//-----------------------------------------------------
	if( m_Distance < 0 || m_Distance != m_pDistance->Get_Value() )
	{
		m_Distance	= m_pDistance->Get_Value();

		m_pModel->Clr_Data();

		for(int i=0; i<m_pVariogram->Get_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= m_pVariogram->Get_Record(i);

			if( pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE) <= m_Distance )
			{
				m_pModel->Add_Data(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE), pRecord->asDouble(CSG_Variogram::FIELD_VAR_EXP));
			}
		}
	}

	//-----------------------------------------------------
	if(	!m_pModel->Set_Formula(Get_Formula()) )
	{
		m_pSummary->SetValue(m_pModel->Get_Error().c_str());
	}
	else
	{
		m_pModel->Init_Parameter('r', 0.5 * m_Distance);

		if( !m_pModel->Get_Trend() )
		{
			m_pSummary->SetValue(_TL("function fitting failed !"));
		}
		else
		{
			wxString	s(m_pModel->Get_Formula(SG_TREND_STRING_Function).c_str());

			if( m_pDiagram->m_bErrors )
			{
				s	+= wxString::Format("\n%s: %s\n", _TL("Warning"), _TL("Function returns negative and/or decreasing values."));
			}

			s	+= wxString::Format("\n%s:\t%.2f%%", _TL("Determination"   ), m_pModel->Get_R2() * 100.0);
			s	+= wxString::Format("\n%s:\t%.*f"  , _TL("Fitting range"   ), SG_Get_Significant_Decimals(m_pDistance->Get_Value()), m_pDistance->Get_Value());
			s	+= wxString::Format("\n%s:\t%d"    , _TL("Samples in range"), m_pModel->Get_Data_Count());
			s	+= wxString::Format("\n%s:\t%d"    , _TL("Lag Classes"     ), m_pVariogram->Get_Count());
			s	+= wxString::Format("\n%s:\t%.2f"  , _TL("Lag Distance"    ), m_Settings("LAGDIST")->asDouble());
			s	+= wxString::Format("\n%s:\t%.2f"  , _TL("Maximum Distance"), m_Settings("MAXDIST")->asDouble());

			m_pSummary->SetValue(s);

			m_Settings("MODEL")->Set_Value(m_pFormula->GetValue().wx_str());
		}
	}

	m_pDiagram->m_bPairs	= m_pPairs->GetValue();

	m_pDiagram->Refresh(true);

	SG_UI_Process_Set_Okay();
}

//---------------------------------------------------------
const char * CVariogram_Dialog::Get_Formula(int Index)
{
	switch( Index )
	{
	default: return( "a + b * x"                                                                    );	// linear
	case  1: return( "b * x"                                                                        );	// linear (no nugget)
	case  2: return( "a + b * sqrt(x)"                                                              );	// square root
	case  3: return( "a + b * ln(1 + x)"                                                            );	// logarithmic
	case  4: return( "n + (s - n) * (1 - exp(-(x / r)); n=n; s=s; r=r"                              );	// exponential
	case  5: return( "n + (s - n) * (1 - exp(-(x / r)^2)); n=n; s=s; r=r"                           );	// gaussian
	case  6: return( "n + (s - n) * ifelse(x > r, 1, 1.5 * x / r - 0.5 * x^3 / r^3); n=n; s=s; r=r" );	// spherical
	}
}

//---------------------------------------------------------
CSG_String CVariogram_Dialog::Get_Formula(void)
{
	CSG_String_Tokenizer	Tokens(m_pFormula->GetValue().wx_str(), ";");

	CSG_String	Formula;

	if( Tokens.Get_Tokens_Count() > 0 )
	{
		Formula	= Tokens.Get_Next_Token();

		while( Tokens.Has_More_Tokens() )
		{
			CSG_String	Token	= Tokens.Get_Next_Token();

			CSG_String	val	= Token. AfterFirst('='); val.Trim(true); val.Trim(false);
			CSG_String	var	= Token.BeforeFirst('='); var.Trim(true); var.Trim(false);

			if( var.Length() == 1 && val.Length() > 0 )
			{
				CSG_String	tmp;

				for(int i=0, n=Formula.Length()-1; i<Formula.Length(); i++)
				{
					if( Formula[i] == var[0]
					&& !(i > 0 && isalpha(Formula[i - 1]))
					&& !(i < n && isalpha(Formula[i + 1])) )
					{
						tmp	+= val;
					}
					else
					{
						tmp	+= Formula[i];
					}
				}

				Formula	= tmp;
			}
		}
	}

	return( Formula );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
