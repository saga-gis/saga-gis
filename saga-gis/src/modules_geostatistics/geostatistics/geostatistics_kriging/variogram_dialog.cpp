/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	int					i, j, k, n;
	double				z, lagDistance;
	TSG_Point			p;
	CSG_Vector			Count, Variance;
	CSG_Table_Record	*pRecord;
	CSG_Shape			*pPoint;

	//-----------------------------------------------------
	if( nSkip < 1 )
	{
		nSkip		= 1;
	}

	if( maxDistance <= 0.0 || maxDistance > SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange()) )
	{
		maxDistance	= SG_Get_Length(pPoints->Get_Extent().Get_XRange(), pPoints->Get_Extent().Get_YRange());	// bounding box' diagonal
	}

	lagDistance	= maxDistance / nClasses;

	Count		.Create(nClasses);
	Variance	.Create(nClasses);

	//-----------------------------------------------------
	for(i=0, n=0; i<pPoints->Get_Count()-nSkip && SG_UI_Process_Set_Progress(n, SG_Get_Square(pPoints->Get_Count()/nSkip)/2); i+=nSkip)
	{
		pPoint	= pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(Attribute) )
		{
			p	= pPoint->Get_Point(0);
			z	= bLog ? log(pPoint->asDouble(Attribute)) : pPoint->asDouble(Attribute);

			for(j=i+nSkip; j<pPoints->Get_Count(); j+=nSkip, n++)
			{
				pPoint	= pPoints->Get_Shape(j);

				if( !pPoint->is_NoData(Attribute) )
				{
					k		= (int)(SG_Get_Distance(p, pPoint->Get_Point(0)) / lagDistance);

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

	pVariogram->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Variogram"), pPoints->Get_Name()));

	pVariogram->Add_Field(_TL("Class")		, SG_DATATYPE_Int);		// FIELD_CLASS
	pVariogram->Add_Field(_TL("Distance")	, SG_DATATYPE_Double);	// FIELD_DISTANCE
	pVariogram->Add_Field(_TL("Count")		, SG_DATATYPE_Int);		// FIELD_COUNT
	pVariogram->Add_Field(_TL("Variance")	, SG_DATATYPE_Double);	// FIELD_VAR_EXP
	pVariogram->Add_Field(_TL("Var.cum.")	, SG_DATATYPE_Double);	// FIELD_VAR_CUM
	pVariogram->Add_Field(_TL("Model")		, SG_DATATYPE_Double);	// FIELD_VAR_MODEL

	for(i=0, z=0.0, n=0; i<nClasses; i++)
	{
		if( Count[i] > 0 )
		{
			n	+= (int)Count[i];
			z	+= Variance[i];

			pRecord	= pVariogram->Add_Record();
			pRecord->Set_Value(FIELD_CLASS		, (i + 1));
			pRecord->Set_Value(FIELD_DISTANCE	, (i + 1) * lagDistance);
			pRecord->Set_Value(FIELD_COUNT		, Count[i]);
			pRecord->Set_Value(FIELD_VAR_EXP	, 0.5 * Variance[i] / Count[i]);
			pRecord->Set_Value(FIELD_VAR_CUM	, 0.5 * z / n);
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

		double	x, y, z;

		if( nSkip < 1 )	{	nSkip	= 1;	}

		for(int iPoint=0; iPoint<pPoints->Get_Count() && ::SG_UI_Process_Set_Progress(iPoint, pPoints->Get_Count()); iPoint+=nSkip)
		{
			TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

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


	bool						m_bPairs;


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
	if( m_pVariogram->Get_Count() > 0 )
	{
		int		i, ix, iy, jx, jy;
		double	x, dx;

		//-------------------------------------------------
		if( m_pModel->Get_Data_Count() > 0 )
		{
			ix	= Get_xToScreen(m_pModel->Get_Data_XMax());
			dc.SetPen  (wxPen(wxColour(  0, 127,   0), 2));
			dc.DrawLine(ix, Get_yToScreen(m_yMin), ix, Get_yToScreen(m_yMax));
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
			dc.SetPen(wxPen(*wxRED, 2));

			dx	= (m_xMax - m_xMin) / (double)rDraw.GetWidth();

			ix	= Get_xToScreen(m_xMin);
			iy	= Get_yToScreen(m_pModel->Get_Value(m_xMin));

			for(x=m_xMin+dx; x<=m_xMax; x+=dx)
			{
				jx	= ix;
				jy	= iy;
				ix	= Get_xToScreen(x);
				iy	= Get_yToScreen(m_pModel->Get_Value(x));

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
	m_Attribute		= 0;
	m_pVariogram	= NULL;
	m_pModel		= NULL;
	m_Distance		= -1;

	//-----------------------------------------------------
	wxArrayString	Formulas;

	Formulas.Empty();
	Formulas.Add(SG_T("a + b * x"));												// 1st order polynom (linear)
	Formulas.Add(SG_T("a + b * x + c * x^2"));										// 2nd order polynom (quadric)
	Formulas.Add(SG_T("a + b * x + c * x^2 + d * x^3"));							// 3rd order polynom (cubic)
	Formulas.Add(SG_T("a + b * x + c * x^2 + d * x^3 + e * x^4"));					// 4th order polynom
	Formulas.Add(SG_T("a + b * sqrt(c + x)"));										// square root
	Formulas.Add(SG_T("a + b * ln(x)"));											// logarithmic
	Formulas.Add(SG_T("a + b * x^c"));												// exponential
	Formulas.Add(SG_T("a + b * (1 - exp(-(x / b)^2))"));							// gaussian
	Formulas.Add(SG_T("a + b * ifelse(x > c, 1, 1.5 * x / c - 0.5 * x^3 / c^3)"));	// spherical

	//-----------------------------------------------------
	Add_Button(_TL("Ok")		, wxID_OK);
	Add_Button(_TL("Cancel")	, wxID_CANCEL);

	Add_Spacer();
	m_pSettings		= Add_Button	(_TL("Settings"), wxID_ANY);

	Add_Spacer();
	m_pPairs		= Add_CheckBox	(_TL("Number of Pairs"), false);

	Add_Spacer();
	m_pFormulas		= Add_Choice	(_TL("Predefined Functions"), Formulas, 0);

	Add_Spacer();
	m_pDistance		= Add_Slider	(_TL("Function Fitting Range"), 1, 0, 1);

	Add_Spacer();
	m_pParameters	= Add_TextCtrl	(_TL("Function Parameters"), wxTE_MULTILINE|wxTE_READONLY);

	//-----------------------------------------------------
	Add_Output(
		m_pDiagram = new CVariogram_Diagram(this),
		m_pFormula = new wxTextCtrl(this, wxID_ANY, SG_T("a + b * x"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER),
		1, 0
	);

	//-----------------------------------------------------
	m_Settings.Set_Name(_TL("Variogram Settings"));

	m_Settings.Add_Value (NULL, "SKIP"		, _TL("Skip")				, _TL(""), PARAMETER_TYPE_Int   , 1,   1, true);
	m_Settings.Add_Value (NULL, "LAGDIST"	, _TL("Lag Distance")		, _TL(""), PARAMETER_TYPE_Double, 1, 0.0, true);
	m_Settings.Add_Value (NULL, "MAXDIST"	, _TL("Maximum Distance")	, _TL(""), PARAMETER_TYPE_Double, 1, 0.0, true);
	m_Settings.Add_String(NULL, "MODEL"		, _TL("Model")				, _TL(""), SG_T("a + b * x"));
}

//---------------------------------------------------------
bool CVariogram_Dialog::Execute(CSG_Shapes *pPoints, int Attribute, bool bLog, CSG_Table *pVariogram, CSG_Trend *pModel)
{
	if( m_pPoints != pPoints )
	{
		m_pPoints	= pPoints;
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
	m_pFormula->SetValue(m_pFormulas->GetStringSelection().c_str());

	Set_Model();
}

//---------------------------------------------------------
void CVariogram_Dialog::On_Button(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pSettings )
	{
		if( SG_UI_Dlg_Parameters(&m_Settings, m_Settings.Get_Name()) )
		{
			m_pFormula ->SetValue (m_Settings("MODEL")  ->asString());

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

		m_pModel->Get_Trend();
	}

	//-----------------------------------------------------
	wxString	s;

	if(	!m_pModel->Set_Formula(m_pFormula->GetValue().c_str()) )
	{
		s	+= m_pModel->Get_Error().w_str();
	}
	else if( !m_pModel->Get_Trend() )
	{
		s	+= _TL("function fitting failed !");
	}
	else
	{
		s	+= m_pModel->Get_Formula(SG_TREND_STRING_Function).w_str();
		s	+= wxString::Format(wxT("\n%s:\t%.2f%%"), _TL("Determination")		, m_pModel->Get_R2() * 100.0);
		s	+= wxString::Format(wxT("\n%s:\t%.*f")	, _TL("Fitting range")		, SG_Get_Significant_Decimals(m_pDistance->Get_Value()), m_pDistance->Get_Value());
		s	+= wxString::Format(wxT("\n%s:\t%d")	, _TL("Samples in range")	, m_pModel->Get_Data_Count());
		s	+= wxString::Format(wxT("\n%s:\t%d")	, _TL("Lag Classes")		, m_pVariogram->Get_Count());
		s	+= wxString::Format(wxT("\n%s:\t%.2f")	, _TL("Lag Distance")		, m_Settings("LAGDIST")->asDouble());
		s	+= wxString::Format(wxT("\n%s:\t%.2f")	, _TL("Maximum Distance")	, m_Settings("MAXDIST")->asDouble());

		m_Settings("MODEL")->Set_Value(m_pModel->Get_Formula(SG_TREND_STRING_Formula));
	}

	m_pParameters->SetValue(s);

	m_pDiagram->m_bPairs	= m_pPairs->GetValue();

	m_pDiagram->Refresh(true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
