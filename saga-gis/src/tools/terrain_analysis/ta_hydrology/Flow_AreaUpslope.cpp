
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Flow_AreaUpslope.cpp                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow_AreaUpslope.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_AreaUpslope::CFlow_AreaUpslope(void)
{
	m_pDEM		= NULL;
	m_pRoute	= NULL;
	m_pFlow		= NULL;
}

//---------------------------------------------------------
CFlow_AreaUpslope::~CFlow_AreaUpslope(void)
{
	Finalise();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CFlow_AreaUpslope::Get_Description(void)
{
	return( _TW(
		"This tool allows you to specify target cells, for which the "
		"upslope contributing area shall be identified. The result will "
		"give for each cell the percentage of its flow that reaches the "
		"target cell(s)."
	));
}

//---------------------------------------------------------
CSG_String CFlow_AreaUpslope::Get_Methods(void)
{
	return( CSG_String::Format("%s|%s|%s|%s|%s",
		_TL("Deterministic 8"),
		_TL("Deterministic Infinity"),
		_TL("Multiple Flow Direction"),
		_TL("Multiple Triangular Flow Direction"),
		_TL("Multiple Maximum Downslope Gradient Based Flow Direction")
	));
}

//---------------------------------------------------------
#define ADD_REFERENCES	{\
	Add_Reference("Freeman, G.T.", "1991",\
		"Calculating catchment area with divergent flow based on a regular grid",\
		"Computers and Geosciences, 17:413-22."\
	);\
\
	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",\
		"The extraction of drainage networks from digital elevation data",\
		"Computer Vision, Graphics and Image Processing, 28:323-344."\
	);\
\
	Add_Reference("Qin, C. Z., Zhu, A. X., Pei, T., Li, B. L., Scholten, T., Behrens, T. & Zhou, C. H.", "2011",\
		"An approach to computing topographic wetness index based on maximum downslope gradient",\
		"Precision Agriculture, 12(1), 32-43.",\
		SG_T("https://www.researchgate.net/profile/Cheng-Zhi_Qin/publication/225309245_An_approach_to_computing_topographic_wetness_index_based_on_maximum_downslope_gradient/links/0912f5019cb8cd1521000000.pdf"),\
		SG_T("ResearchGate")\
	);\
\
	Add_Reference("Quinn, P.F., Beven, K.J., Chevallier, P. & Planchon, O.", "1991",\
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models",\
		"Hydrological Processes, 5:59-79.",\
		SG_T("https://www.researchgate.net/profile/Olivier_Planchon/publication/32978462_The_Prediction_of_Hillslope_Flow_Paths_for_Distributed_Hydrological_Modeling_Using_Digital_Terrain_Model/links/0912f5130c356c86e6000000.pdf"),\
		SG_T("ResearchGate")\
	);\
\
	Add_Reference("Seibert, J. & McGlynn, B.", "2007",\
		"A new triangular multiple flow direction algorithm for computing upslope areas from gridded digital elevation models",\
		"Water Resources Research, Vol. 43, W04501,<br>"\
		"C++ implementation in SAGA by Thomas Grabs (c) 2007, contact: thomas.grabs@natgeo.su.se, jan.seibert@natgeo.su.se.",\
		SG_T("http://onlinelibrary.wiley.com/doi/10.1029/2006WR005128/full"), SG_T("Wiley")\
	);\
\
	Add_Reference("Tarboton, D.G.", "1997",\
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",\
		"Water Resources Research, Vol.33, No.2, p.309-319.",\
		SG_T("http://onlinelibrary.wiley.com/doi/10.1029/96WR03137/pdf"),\
		SG_T("Wiley")\
	);\
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaUpslope::Initialise(int Method, CSG_Grid *pDTM, CSG_Grid *pRoute, CSG_Grid *pFlow, double MFD_Converge, bool MFD_bContour)
{
	Finalise();

	if( pDTM && pDTM->is_Valid() && pFlow && pFlow->is_Valid() && pFlow->Get_System() == pDTM->Get_System() )
	{
		m_Method		= Method;
		m_MFD_Converge	= MFD_Converge;
		m_MFD_bContour	= MFD_bContour;
		m_pDEM			= pDTM;
		m_pFlow			= pFlow;
		m_pFlow->Set_NoData_Value(0.);

		if( pRoute && pRoute->is_Valid() && pRoute->Get_System() == pDTM->Get_System() )
		{
			m_pRoute	= pRoute;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Finalise(void)
{
	m_pDEM		= NULL;
	m_pRoute	= NULL;
	m_pFlow		= NULL;

	return( true );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Add_Target(int x, int y)
{
	if( m_pFlow && m_pFlow->is_InGrid(x, y, false) )
	{
		m_pFlow->Set_Value(x, y, 100.);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Clr_Target(void)
{
	if( m_pFlow )
	{
		m_pFlow->Assign(0.);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Get_Area(int x, int y)
{
	return( Clr_Target() && Add_Target(x, y) && Get_Area() );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Get_Area(void)
{
	if( !m_pDEM || !m_pFlow || !m_pDEM->Set_Index() )
	{
		return( false );
	}

	for(sLong i=0; i<m_pDEM->Get_NCells() && SG_UI_Process_Set_Progress((double)i, (double)m_pDEM->Get_NCells()); i++)
	{
		int	x, y;

		if( m_pDEM->Get_Sorted(i, x, y, false) && m_pFlow->asDouble(x, y) <= 0. )
		{
			Set_Value(x, y);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_Value(int x, int y)
{
	int	i	= m_pRoute ? m_pRoute->asChar(x, y) : -1;

	if( i >= 0 )
	{
		int	ix	= CSG_Grid_System::Get_xTo(i, x);
		int	iy	= CSG_Grid_System::Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy, true) && m_pFlow->asDouble(ix, iy) > 0. )
		{
			m_pFlow->Set_Value(x, y, m_pFlow->asDouble(ix, iy));
		}
	}
	else if( !m_pDEM->is_NoData(x, y) )
	{
		switch( m_Method )
		{
		default: Set_D8    (x, y); break;
		case  1: Set_DInf  (x, y); break;
		case  2: Set_MFD   (x, y); break;
		case  3: Set_MDInf (x, y); break;
		case  4: Set_MMDGFD(x, y); break;
		}
	}
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_D8(int x, int y)
{
	int	i	= m_pDEM->Get_Gradient_NeighborDir(x, y);

	if( i >= 0 )
	{
		int	ix	= CSG_Grid_System::Get_xTo(i, x);
		int	iy	= CSG_Grid_System::Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && m_pFlow->asDouble(ix, iy) > 0. )
		{
			m_pFlow->Set_Value(x, y, m_pFlow->asDouble(ix, iy));
		}
	}
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_DInf(int x, int y)
{
	double	Slope, Aspect;

	if( m_pDEM->Get_Gradient(x, y, Slope, Aspect) )	// && Aspect >= 0. )
	{
		int	i	= (int)(Aspect / M_PI_045);

		int	ix	= CSG_Grid_System::Get_xTo(i    , x);
		int	iy	= CSG_Grid_System::Get_yTo(i    , y);

		int	jx	= CSG_Grid_System::Get_xTo(i + 1, x);
		int	jy	= CSG_Grid_System::Get_yTo(i + 1, y);

		if(	m_pDEM->is_InGrid(ix, iy) && m_pDEM->asDouble(ix, iy) < m_pDEM->asDouble(x, y)
		&&	m_pDEM->is_InGrid(jx, jy) && m_pDEM->asDouble(jx, jy) < m_pDEM->asDouble(x, y) )
		{
			Aspect	= fmod(Aspect,  M_PI_045) / M_PI_045;

			double	Flow	=
				m_pFlow->asDouble(ix, iy) * (1. - Aspect) +
				m_pFlow->asDouble(jx, jy) * (     Aspect);

			if( Flow > 0. )
			{
				m_pFlow->Set_Value(x, y, Flow);
			}

			return;
		}
	}

	Set_D8(x, y);
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_MFD(int x, int y)
{
	double	dz[8], dzSum = 0., z = m_pDEM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= CSG_Grid_System::Get_xTo(i, x);
		int	iy	= CSG_Grid_System::Get_yTo(i, y);

		dz[i]	= m_pDEM->is_InGrid(ix, iy) ? z - m_pDEM->asDouble(ix, iy) : 0.;

		if( dz[i] > 0. )
		{
			dzSum	+= (dz[i] = pow(dz[i] / m_pDEM->Get_System().Get_Length(i), m_MFD_Converge) * (m_MFD_bContour && i % 2 ? sqrt(2.) / 2. : 1.));

			if( m_pFlow->asDouble(ix, iy) > 0. )
			{
				dz[i] *= m_pFlow->asDouble(ix, iy);
			}
			else
			{
				dz[i]  = 0.;
			}
		}
	}

	if( dzSum > 0. )
	{
		double	Flow	= 0.;

		for(int i=0; i<8; i++)
		{
			if( dz[i] > 0. )
			{
				Flow	+= dz[i] / dzSum;
			}
		}

		if( Flow > 0. )
		{
			m_pFlow->Set_Value(x, y, Flow);
		}
	}
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_MMDGFD(int x, int y)
{
	double	dz[8], dzMax = 0., z = m_pDEM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= CSG_Grid_System::Get_xTo(i, x);
		int	iy	= CSG_Grid_System::Get_yTo(i, y);

		dz[i]	= m_pDEM->is_InGrid(ix, iy) ? z - m_pDEM->asDouble(ix, iy) : 0.;

		if( dz[i] > 0. )
		{
			dz[i]	/= m_pDEM->Get_System().Get_Length(i);

			if( dzMax < dz[i] )
			{
				dzMax	= dz[i];
			}
		}
	}

	//-----------------------------------------------------
	if( dzMax > 0. )
	{
		dzMax	= dzMax < 1. ? 8.9 * dzMax + 1.1 : 10.;

		double	dzSum	= 0.;

		for(int i=0; i<8; i++)
		{
			if( dz[i] > 0. )
			{
				dzSum	+= (dz[i] = pow(dz[i], dzMax) * (m_MFD_bContour && i % 2 ? sqrt(2.) / 2. : 1.));
			}
		}

		double	Flow	= 0.;

		for(int i=0; i<8; i++)
		{
			int	ix	= CSG_Grid_System::Get_xTo(i, x);
			int	iy	= CSG_Grid_System::Get_yTo(i, y);

			if( dz[i] > 0. && m_pFlow->asDouble(ix, iy) > 0. )
			{
				Flow	+= dz[i] / dzSum;
			}
		}

		if( Flow > 0. )
		{
			m_pFlow->Set_Value(x, y, Flow);
		}
	}
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_MDInf(int x, int y)
{
	bool	bInGrid[8];

	double	dz[8], s_facet[8], r_facet[8], valley[8], portion[8];

	double	z	= m_pDEM->asDouble(x, y);

	//-----------------------------------------------------
	for(int i=0; i<8; i++)
	{
		s_facet[i]	= r_facet[i]	= -999.;

		int	ix	= CSG_Grid_System::Get_xTo(i, x);
		int	iy	= CSG_Grid_System::Get_yTo(i, y);

		if( (bInGrid[i] = m_pDEM->is_InGrid(ix, iy)) )
		{
			dz[i]	= z - m_pDEM->asDouble(ix, iy);
		}
		else
		{
			dz[i]	= 0.;
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<8; i++)
	{
		double	hs	= -999.;
		double	hr	= -999.;

		if( bInGrid[i] )
		{
			int	j	= i < 7 ? i + 1 : 0;

			if( bInGrid[j] )
			{			
				double	nx	= (dz[j] * CSG_Grid_System::Get_yTo(i) - dz[i] * CSG_Grid_System::Get_yTo(j)) * m_pDEM->Get_System().Get_Cellsize();			// vb-code:  nx = (z1 * yd(j) - z2 * yd(i)) * gridsize
				double	ny	= (dz[i] * CSG_Grid_System::Get_xTo(j) - dz[j] * CSG_Grid_System::Get_xTo(i)) * m_pDEM->Get_System().Get_Cellsize();			// vb-code:  ny = (z1 * xd(j) - z2 * xd(i)) * gridsize
				double	nz	= (CSG_Grid_System::Get_xTo(i) * CSG_Grid_System::Get_yTo(j) - CSG_Grid_System::Get_xTo(j) * CSG_Grid_System::Get_yTo(i)) * m_pDEM->Get_System().Get_Cellarea();	// vb-code:  nz = (xd(j) * yd(i) - xd(i) * yd(j)) * gridsize ^ 2

				double	n_norm	= sqrt(nx*nx + ny*ny +nz*nz);

				if( nx == 0. )
				{
					hr = (ny >= 0.)? 0. : M_PI;
				} 
				else if( nx < 0. )
				{
					hr = M_PI_270 - atan(ny / nx);
				} 
				else
				{
					hr = M_PI_090 - atan(ny / nx);
				}

				hs	= -tan( acos( nz/n_norm ) );	// vb-code:  hs = -Tan(arccos(nz / betrag_n))

				if( hr < i * M_PI_045 || hr > (i+1) * M_PI_045 )
				{
					if( dz[i] > dz[j] )
					{
						hr	= i * M_PI_045;
						hs	= dz[i] / m_pDEM->Get_System().Get_Length(i);
					}
					else
					{
						hr	= j * M_PI_045;
						hs	= dz[j] / m_pDEM->Get_System().Get_Length(j);
					}
				}				
			}
			else if( dz[i] > 0. )
			{
				hr	= i * M_PI_045;
				hs	= dz[i] / m_pDEM->Get_System().Get_Length(i);
			}

			s_facet[i]	= hs;
			r_facet[i]	= hr;
		}
	}

	//-----------------------------------------------------
	double	dzSum	= 0.;

	for(int i=0; i<8; i++)
	{		
		valley[i]	= 0.;

		int	j	= i < 7 ? i + 1 : 0;

		if( s_facet[i] > 0. )
		{
			if( r_facet[i] > i * M_PI_045 && r_facet[i] < (i+1) * M_PI_045 )
			{
				valley[i] = s_facet[i];
			}
			else if( r_facet[i] == r_facet[j] )
			{
				valley[i] = s_facet[i];
			}
			else if( s_facet[j] == -999. && r_facet[i] == (i+1) * M_PI_045 )
			{
				valley[i] = s_facet[i];
			}
			else
			{
				j = i > 0 ? i - 1 : 7;

				if( s_facet[j] == -999. && r_facet[i] == i * M_PI_045 )
				{
					valley[i] = s_facet[i];
				}
			}

			valley[i] = pow(valley[i], m_MFD_Converge);
			dzSum += valley[i];
		} 

		portion[i] = 0.;
	}

	//-----------------------------------------------------
	if( dzSum )
	{
		for(int i=0; i<8; i++)
		{
			int	j	= i < 7 ? i + 1 : 0;

			if( i >= 7 && r_facet[i] == 0. )
			{
				r_facet[i]	= M_PI_360;
			}

			if( valley[i] )
			{
				valley[i]	/= dzSum;

				portion[i]	+= valley[i] * ((i+1) * M_PI_045 - r_facet[i]) / M_PI_045;	// vb-code: portion(i) = portion(i) + valley(i) * (i * PI / 4 - r_facet(i)) / (PI / 4)
				portion[j]	+= valley[i] * (r_facet[i] - (i  ) * M_PI_045) / M_PI_045;	// vb-code: portion(j) = portion(j) + valley(i) * (r_facet(i) - (i - 1) * PI / 4) / (PI / 4)
			}
		}

		double	Flow	= 0.;

		for(int i=0; i<8; i++)
		{
			if( portion[i] > 0. )
			{
				int	ix	= CSG_Grid_System::Get_xTo(i, x);
				int	iy	= CSG_Grid_System::Get_yTo(i, y);

				if( m_pFlow->is_InGrid(ix, iy, false) && m_pFlow->asDouble(ix, iy) > 0. )
				{
					Flow	+= m_pFlow->asDouble(ix, iy) * portion[i];
				}
			}
		}

		if( Flow > 0. )
		{
			m_pFlow->Set_Value(x, y, Flow);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_AreaUpslope_Interactive::CFlow_AreaUpslope_Interactive(void)
{
	Set_Name		(_TL("Upslope Area"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(CSG_String::Format("%s\n_______\n\n%s", m_Calculator.Get_Description().c_str(),
		_TL("Interactive version (left mouse clicks will trigger the calculation for the selected cell).")
	));

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SINKROUTE"	, _TL("Sink Routes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"AREA"		, _TL("Upslope Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		m_Calculator.Get_Methods(), 2
	);

	Parameters.Add_Double("",
		"CONVERGE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction Algorithm (Freeman 1991).\nApplies also to the Multiple Triangular Flow Direction Algorithm."),
		1.1, 0.001, true
	);

	Parameters.Add_Bool("",
		"MFD_CONTOUR", _TL("Contour Length"),
		_TL("Include (pseudo) contour length as additional weighting factor in multiple flow direction routing, reduces flow to diagonal neighbour cells by a factor of 0.71 (s. Quinn et al. 1991 for details)."),
		false
	);

	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_AreaUpslope_Interactive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("CONVERGE"   , pParameter->asInt() == 2 || pParameter->asInt() == 3);
		pParameters->Set_Enabled("MFD_CONTOUR", pParameter->asInt() == 2 || pParameter->asInt() == 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaUpslope_Interactive::On_Execute(void)
{
	if( m_Calculator.Initialise(
		Parameters("METHOD"     )->asInt   (),
		Parameters("ELEVATION"  )->asGrid  (),
		Parameters("SINKROUTE"  )->asGrid  (),
		Parameters("AREA"       )->asGrid  (),
		Parameters("CONVERGE"   )->asDouble(),
		Parameters("MFD_CONTOUR")->asBool  ()) )
	{
		DataObject_Set_Colors(Parameters("AREA")->asGrid(), 11, SG_COLORS_WHITE_BLUE);
		DataObject_Update    (Parameters("AREA")->asGrid(), SG_UI_DATAOBJECT_SHOW_MAP);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope_Interactive::On_Execute_Finish(void)
{
	return( m_Calculator.Finalise() );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	case TOOL_INTERACTIVE_LDOWN:
		m_Calculator.Clr_Target();
		m_Calculator.Add_Target(Get_xGrid(), Get_yGrid());
		break;

	case TOOL_INTERACTIVE_MOVE_LDOWN:
		m_Calculator.Add_Target(Get_xGrid(), Get_yGrid());
		break;

	case TOOL_INTERACTIVE_LUP:
		m_Calculator.Add_Target(Get_xGrid(), Get_yGrid());
		m_Calculator.Get_Area();
		DataObject_Update(Parameters("AREA")->asGrid(), 0., 100.);
		break;
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_AreaUpslope_Area::CFlow_AreaUpslope_Area(void)
{
	Set_Name		(_TL("Upslope Area"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(CSG_String::Format("%s\n_______\n\n%s", m_Calculator.Get_Description().c_str(),
		_TW("This version uses all valid cells (not \'no data\' values) of a given target grid to determine the contributing area. "
			"In case no target grid is provided as input, the specified x/y coordinates are used as target point.")
	));

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"TARGET"	, _TL("Target Area"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"TARGET_PT_X", _TL("Target X coordinate"),
		_TL("The x-coordinate of the target point in world coordinates [map units]"),
		0.
	);

	Parameters.Add_Double("",
		"TARGET_PT_Y", _TL("Target Y coordinate"),
		_TL("The y-coordinate of the target point in world coordinates [map units]"),
		0.
	);

	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SINKROUTE"	, _TL("Sink Routes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"AREA"		, _TL("Upslope Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		m_Calculator.Get_Methods(), 2
	);

	Parameters.Add_Double("",
		"CONVERGE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction algorithm"),
		1.1, 0.001, true
	);

	Parameters.Add_Bool("",
		"MFD_CONTOUR"	, _TL("Contour Length"),
		_TL("Include (pseudo) contour length as additional weighting factor in multiple flow direction routing, reduces flow to diagonal neighbour cells by a factor of 0.71 (s. Quinn et al. 1991 for details)."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_AreaUpslope_Area::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TARGET") )
	{
		pParameters->Set_Enabled("TARGET_PT_X", pParameter->asPointer() == NULL);
		pParameters->Set_Enabled("TARGET_PT_Y", pParameter->asPointer() == NULL);
	}

	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("CONVERGE", pParameter->asInt() == 2 || pParameter->asInt() == 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope_Area::On_Execute(void)
{
	bool	bResult	= false;

	if( m_Calculator.Initialise(
		Parameters("METHOD"     )->asInt   (),
		Parameters("ELEVATION"  )->asGrid  (),
		Parameters("SINKROUTE"  )->asGrid  (),
		Parameters("AREA"       )->asGrid  (),
		Parameters("CONVERGE"   )->asDouble(),
		Parameters("MFD_CONTOUR")->asBool  ()) )
	{
		m_Calculator.Clr_Target();

		CSG_Grid	*pTarget	= Parameters("TARGET")->asGrid();

		if( pTarget != NULL )
		{
			for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( !pTarget->is_NoData(x, y) && m_Calculator.Add_Target(x, y) )
					{
						bResult	= true;
					}
				}
			}
		}
		else
		{
			int	x, y;

			Parameters("ELEVATION")->asGrid()->Get_System().Get_World_to_Grid(x, y,
				Parameters("TARGET_PT_X")->asDouble(),
				Parameters("TARGET_PT_Y")->asDouble()
			);

			if( m_Calculator.Add_Target(x, y) )
			{
				bResult	= true;
			}
			else
			{
				SG_UI_Msg_Add_Error(_TL("Coordinates of target point outside of DEM!"));
			}
		}

		if( bResult )
		{
			m_Calculator.Get_Area();

			DataObject_Set_Colors(Parameters("AREA")->asGrid(), 11, SG_COLORS_WHITE_BLUE);
		}
	}

	//-----------------------------------------------------
	m_Calculator.Finalise();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
