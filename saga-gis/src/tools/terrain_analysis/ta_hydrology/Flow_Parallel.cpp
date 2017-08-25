/**********************************************************
 * Version $Id: Flow_Parallel.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                   Flow_Parallel.cpp                   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow_Parallel.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Parallel::CFlow_Parallel(void)
{
	Set_Name		(_TL("Flow Accumulation (Top-Down)"));

	Set_Author		("O.Conrad (c) 2001-2016, Portions by T.Grabs (c) 2010");

	Set_Description	(_TW(
		"Top-down processing of cells for calculation of flow accumulation and related parameters. "
		"This set of algorithms processes a DEM downwards from the highest to the lowest cell.\n"
		"\n"
		"Flow routing methods provided by this tool:<ul>"
		"<li>Deterministic 8 (aka D8, O'Callaghan & Mark 1984)</li>"
		"<li>Braunschweiger Reliefmodell (Bauer et al. 1985)</li>"
		"<li>Rho 8 (Fairfield & Leymarie 1991)</li>"
		"<li>Multiple Flow Direction (Freeman 1991, Quinn et al. 1991)</li>"
		"<li>Deterministic Infinity (Tarboton 1997)</li>"
		"<li>Triangular Multiple Flow Direction (Seibert & McGlynn 2007</li>"
		"<li>Multiple Flow Direction based on Maximum Downslope Gradient (Qin et al. 2011)</li>"
		"</ul>"
	));

	Add_Reference("Bauer, J., Rohdenburg, H. & Bork, H.-R.", "1985",
		"Ein Digitales Reliefmodell als Vorraussetzung fuer ein deterministisches Modell der Wasser- und Stoff-Fluesse",
		"Landschaftsgenese und Landschaftsoekologie, H.10, p.1-15."
	);

	Add_Reference("Fairfield, J. & Leymarie, P.", "1991",
		"Drainage networks from grid digital elevation models",
		"Water Resources Research, 27:709-717."
	);

	Add_Reference("Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22."
	);

	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344."
	);

	Add_Reference("Qin, C. Z., Zhu, A. X., Pei, T., Li, B. L., Scholten, T., Behrens, T. & Zhou, C. H.", "2011",
		"An approach to computing topographic wetness index based on maximum downslope gradient",
		"Precision Agriculture, 12(1), 32-43.",
		SG_T("https://www.researchgate.net/profile/Cheng-Zhi_Qin/publication/225309245_An_approach_to_computing_topographic_wetness_index_based_on_maximum_downslope_gradient/links/0912f5019cb8cd1521000000.pdf"),
		SG_T("ResearchGate")
	);

	Add_Reference("Quinn, P.F., Beven, K.J., Chevallier, P. & Planchon, O.", "1991",
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models",
		"Hydrological Processes, 5:59-79.",
		SG_T("https://www.researchgate.net/profile/Olivier_Planchon/publication/32978462_The_Prediction_of_Hillslope_Flow_Paths_for_Distributed_Hydrological_Modeling_Using_Digital_Terrain_Model/links/0912f5130c356c86e6000000.pdf"),
		SG_T("ResearchGate")
	);

	Add_Reference("Seibert, J. & McGlynn, B.", "2007",
		"A new triangular multiple flow direction algorithm for computing upslope areas from gridded digital elevation models",
		"Water Resources Research, Vol. 43, W04501,<br>"
		"C++ implementation in SAGA by Thomas Grabs (c) 2007, contact: thomas.grabs@natgeo.su.se, jan.seibert@natgeo.su.se.",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1029/2006WR005128/full"),
		SG_T("Wiley")
	);

	Add_Reference("Tarboton, D.G.", "1997",
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",
		"Water Resources Research, Vol.33, No.2, p.309-319.",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1029/96WR03137/pdf"),
		SG_T("Wiley")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"FLOW_LENGTH"	, _TL("Flow Path Length"),
		_TL("average distance that a cell's accumulated flow travelled"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"LINEAR_VAL"	, _TL("Linear Flow Threshold Grid"),
		_TL("optional grid providing values to be compared with linear flow threshold instead of flow accumulation"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"LINEAR_DIR"	, _TL("Channel Direction"),
		_TL("use this for (linear) flow routing, if the value is a valid direction (0-7 = N, NE, E, SE, S, SW, W, NW)"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"WEIGHT_LOSS"	, _TL("Loss through Negative Weights"),
		_TL("when using weights without support for negative flow: output of the absolute amount of negative flow that occurred"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|",
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Braunschweiger Reliefmodell"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction"),
			_TL("Multiple Triangular Flow Directon"),
			_TL("Multiple Maximum Downslope Gradient Based Flow Directon")
		), 4
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"LINEAR_DO"		, _TL("Thresholded Linear Flow"),
		_TL("apply linear flow routing (D8) to all cells, having a flow accumulation greater than the specified threshold"),
		true
	);

	Parameters.Add_Int("LINEAR_DO",
		"LINEAR_MIN"	, _TL("Linear Flow Threshold"),
		_TL("flow accumulation threshold (cells) for linear flow routing"),
		500, 0, true
	);
	
	Parameters.Add_Double("",
		"CONVERGENCE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction Algorithm (Freeman 1991).\nApplies also to the Multiple Triangular Flow Directon Algorithm."),
		1.1, 0.0, true
	);

	Parameters.Add_Bool("",
		"NO_NEGATIVES"	, _TL("Prevent Negative Flow Accumulation"),
		_TL("when using weights: do not transport negative flow, set it to zero instead; useful e.g. when accumulating measures of water balance."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_Parallel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("CONVERGENCE", pParameter->asInt() == 4 || pParameter->asInt() == 5);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "LINEAR_DO") )
	{
		pParameters->Set_Enabled("LINEAR_MIN", pParameter->asBool());
		pParameters->Set_Enabled("LINEAR_VAL", pParameter->asBool());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "WEIGHTS") )
	{
		pParameters->Set_Enabled("NO_NEGATIVES", pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("WEIGHT_LOSS" , pParameter->asGrid() != NULL && Parameters("NO_NEGATIVES")->asBool());
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NO_NEGATIVES") )
	{
		pParameters->Set_Enabled("WEIGHT_LOSS" , pParameter->asBool());
	}

	return( CFlow::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::On_Initialize(void)
{
	m_pFlow_Length	= Parameters("FLOW_LENGTH")->asGrid();
	m_Converge		= Parameters("CONVERGENCE")->asDouble();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Parallel::Calculate(void)
{
	for(int y=0; y<Get_NY() && Set_Progress(y); y+=m_Step)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x+=m_Step)
		{
			Init_Cell(x, y);
		}
	}

	return( Set_Flow() );
}

//---------------------------------------------------------
bool CFlow_Parallel::Calculate(int x, int y)
{
	Init_Cell(x, y);

	return( Set_Flow() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Parallel::Set_Flow(void)
{
	//-----------------------------------------------------
	if( !m_pDTM->Set_Index() )
	{
		return( false );
	}

	//-----------------------------------------------------
	int Method	= Parameters("METHOD")->asInt();

	if( Method == 2 )
	{
		BRM_Init();
	}

	//-----------------------------------------------------
	double	dLinear	= Parameters("LINEAR_DO")->asBool() ? Parameters("LINEAR_MIN")->asDouble() : -1.0;

	CSG_Grid	*pLinear_Val	= Parameters("LINEAR_VAL")->asGrid();
	CSG_Grid	*pLinear_Dir	= Parameters("LINEAR_DIR")->asGrid();

	//-----------------------------------------------------
	bool	bNoNegatives	= m_pWeights ? Parameters("NO_NEGATIVES")->asBool() : false;

	CSG_Grid	*pLoss	= Parameters("WEIGHT_LOSS")->asGrid();

	if( bNoNegatives && pLoss )
	{
		pLoss->Assign_NoData();
	}

	//-----------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;

		if( m_pDTM->Get_Sorted(n, x, y) )
		{
			if( bNoNegatives && m_pFlow->asDouble(x, y) < 0.0 )
			{
				if( pLoss )
				{
					pLoss->Set_Value(x, y, fabs(m_pFlow->asDouble(x, y)));
				}

				m_pFlow->Set_Value(x, y, 0.0);
			}

			if( pLinear_Dir && !pLinear_Dir->is_NoData(x, y) )
			{
				Set_D8(x, y, pLinear_Dir->asInt(x, y));
			}
			else if( dLinear > 0.0 && dLinear <= (pLinear_Val && !pLinear_Val->is_NoData(x, y) ? pLinear_Val->asDouble(x, y) : m_pFlow->asDouble(x, y)) )
			{
				Set_D8(x, y, pLinear_Dir && !pLinear_Dir->is_NoData(x, y) ? pLinear_Dir->asInt(x, y) : -1);
			}
			else switch( Method )
			{
			case 0:	Set_D8    (x, y);	break;
			case 1:	Set_Rho8  (x, y);	break;
			case 2:	Set_BRM   (x, y);	break;
			case 3:	Set_DInf  (x, y);	break;
			case 4:	Set_MFD   (x, y);	break;
			case 5:	Set_MDInf (x, y);	break;
			case 6:	Set_MMDGFD(x, y);	break;
			}
		}
	}

	//-----------------------------------------------------
	if( m_pRoute )
	{
		for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			int		x, y;

			if( m_pDTM->Get_Sorted(n, x, y, false) )
			{
				Check_Route(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Check_Route(int x, int y)
{
	if( m_pRoute->asChar(x, y) <= 0 )
	{
		return;
	}

	//-----------------------------------------------------
	int		i, ix, iy;

	double	z	= m_pDTM->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		if( !m_pDTM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) || z > m_pDTM->asDouble(ix, iy) )
		{
			return;	// cell is no sink
		}
	}

	//-----------------------------------------------------
	i	= m_pRoute->asChar(x, y);

	ix	= Get_xTo(i, ix);
	iy	= Get_yTo(i, iy);

	//---------------------------------------------
	while( m_pDTM->is_InGrid(ix, iy) )
	{
		Add_Portion(x, y, ix, iy, i);

		if( (i = m_pRoute->asChar(ix, iy)) > 0 )
		{
			ix	= Get_xTo(i, ix);
			iy	= Get_yTo(i, iy);
		}
		else if( (i = m_pDTM->Get_Gradient_NeighborDir(ix, iy)) >= 0 )
		{
			ix	= Get_xTo(i, ix);
			iy	= Get_yTo(i, iy);
		}
		else
		{
			return;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//					Deterministic 8						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_D8(int x, int y, int Direction)
{
	Add_Fraction(x, y, Direction >= 0 ? Direction : m_pDTM->Get_Gradient_NeighborDir(x, y));
}


///////////////////////////////////////////////////////////
//														 //
//						Rho 8							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_Rho8(int x, int y)
{
	int		iMax	= -1;
	double	dMax, z	= m_pDTM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( !m_pDTM->is_InGrid(ix, iy) )
		{
			return;
		}
		else
		{
			double	d	= z - m_pDTM->asDouble(ix, iy);

			if( i % 2 == 1 )
			{
				d	/= 1.0 + rand() / (double)RAND_MAX;
			}

			if( iMax < 0 || dMax < d )
			{
				iMax	= i;
				dMax	= d;
			}
		}
	}

	if( iMax >= 0 )
	{
		Add_Fraction(x, y, iMax);
	}
}

/*void CFlow_Parallel::Set_Rho8(	int x, int y )
{
	int		Direction, ix, iy;

	double 	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	if( Aspect >= 0 )
	{
		Direction	= (int)(Aspect / M_PI_045);

		if( fmod(Aspect, M_PI_045) / M_PI_045 > rand() / (double)RAND_MAX )
		{
			Direction++;
		}

		Direction	%= 8;

		ix			= Get_xTo(Direction, x);
		iy			= Get_yTo(Direction, y);

		if( is_InGrid(ix, iy) && m_pDTM->asDouble(ix, iy) >= m_pDTM->asDouble(x, y) )
		{
			Direction	= Get_Direction_Lowest(m_pDTM, x, y);
		}

		Add_Fraction(x, y, Direction);
	}
}/**/


///////////////////////////////////////////////////////////
//														 //
//				Deterministic Infinity					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_DInf(int x, int y)
{
	double	s, a;

	if( m_pDTM->Get_Gradient(x, y, s, a) && a >= 0.0 )
	{
		int	i, ix, iy;

		i	= (int)(a / M_PI_045);
		a	= fmod (a , M_PI_045) / M_PI_045;
		s	= m_pDTM->asDouble(x, y);

		if( m_pDTM->is_InGrid(ix = Get_xTo(i + 0, x), iy = Get_yTo(i + 0, y)) && m_pDTM->asDouble(ix, iy) < s
		&&  m_pDTM->is_InGrid(ix = Get_xTo(i + 1, x), iy = Get_yTo(i + 1, y)) && m_pDTM->asDouble(ix, iy) < s )
		{
			Add_Fraction(x, y,  i         , 1.0 - a);
			Add_Fraction(x, y, (i + 1) % 8,       a);

			return;
		}
	}

	Set_D8(x, y);
}


///////////////////////////////////////////////////////////
//														 //
//				Multiple Flow Direction					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_MFD(int x, int y)
{
	int		i, ix, iy;
	double	z, d, dzSum, dz[8];

	//-----------------------------------------------------
	for(i=0, dzSum=0.0, z=m_pDTM->asDouble(x, y); i<8; i++)
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) )
		{
			d	= z - m_pDTM->asDouble(ix, iy);
		}
		else
		{
			ix	= Get_xTo(i + 4, x);
			iy	= Get_yTo(i + 4, y);

			if( m_pDTM->is_InGrid(ix, iy) )
			{
				d	= m_pDTM->asDouble(ix, iy) - z;
			}
			else
			{
				d	= 0.0;
			}
		}

		if( d > 0.0 )
		{
			dzSum	+= (dz[i] = pow(d / Get_Length(i), m_Converge));
		}
		else
		{
			dz[i]	= 0.0;
		}
	}

	//-----------------------------------------------------
	if( dzSum > 0.0 )
	{
		for(i=0; i<8; i++)
		{
			if( dz[i] > 0.0 )
			{
				Add_Fraction(x, y, i, dz[i] / dzSum);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//	Multiple Maximum Downslope Gradient Flow Directon	 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_MMDGFD(int x, int y)
{
	int		i, ix, iy;
	double	z, d, dzMax, dzSum, dz[8];

	//-----------------------------------------------------
	for(i=0, dzMax=0.0, z=m_pDTM->asDouble(x, y); i<8; i++)
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) )
		{
			d	= z - m_pDTM->asDouble(ix, iy);
		}
		else
		{
			ix	= Get_xTo(i + 4, x);
			iy	= Get_yTo(i + 4, y);

			if( m_pDTM->is_InGrid(ix, iy) )
			{
				d	= m_pDTM->asDouble(ix, iy) - z;
			}
			else
			{
				d	= 0.0;
			}
		}

		if( d > 0.0 )
		{
			dz[i]	= d / Get_Length(i);

			if( dzMax < dz[i] )
			{
				dzMax	= dz[i];
			}
		}
		else
		{
			dz[i]	= 0.0;
		}
	}

	//-----------------------------------------------------
	if( dzMax > 0.0 )
	{
		dzMax	= dzMax < 1.0 ? 8.9 * dzMax + 1.1 : 10.0;

		for(i=0, dzSum=0.0; i<8; i++)
		{
			if( dz[i] > 0.0 )
			{
				dzSum	+= (dz[i]	= pow(dz[i], dzMax));
			}
		}

		for(i=0; i<8; i++)
		{
			if( dz[i] > 0.0 )
			{
				Add_Fraction(x, y, i, dz[i] / dzSum);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//			Multiple Triangular Flow Direction			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Parallel::Set_MDInf(int x, int y)
{
	int		i, ix, iy;

	double	dz[8], s_facet[8], r_facet[8], valley[8], portion[8];

	bool	bInGrid[8];

	//-----------------------------------------------------
	double	z	= m_pDTM->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		s_facet[i]	= r_facet[i]	= -999.0;

		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);
		
		if( (bInGrid[i] = m_pDTM->is_InGrid(ix, iy)) )
		{
			dz[i]	= z - m_pDTM->asDouble(ix, iy);
		}
		else
		{
			dz[i]	= 0.0;
		}
	}

	//-----------------------------------------------------
	for(i=0; i<8; i++)
	{
		double	hs	= -999.;
		double	hr	= -999.;
		
		if( bInGrid[i] )
		{
			int	j	= i < 7 ? i + 1 : 0;
			
			if( bInGrid[j] )
			{			
				double	nx	= (dz[j] * Get_yTo(i) - dz[i] * Get_yTo(j)) * Get_Cellsize();			// vb-code:  nx = (z1 * yd(j) - z2 * yd(i)) * gridsize
/*ERROR?*/		double	ny	= (dz[i] * Get_xTo(j) - dz[j] * Get_xTo(i)) * Get_Cellsize();			// vb-code:  ny = (z1 * xd(j) - z2 * xd(i)) * gridsize
				double	nz	= (Get_xTo(i) * Get_yTo(j) - Get_xTo(j) * Get_yTo(i)) * Get_Cellarea();	// vb-code:  nz = (xd(j) * yd(i) - xd(i) * yd(j)) * gridsize ^ 2
				
				double	n_norm	= sqrt(nx*nx + ny*ny +nz*nz);

			/*	if( nx == 0.0 )
				{
					hr = (ny >= 0.0)? 0.0 : M_PI;
				} 
				else if( nx > 0.0 )
				{
					hr = M_PI_090 - atan(ny / nx);
				} 
				else
				{
					hr = M_PI_270 - atan(ny / nx);
				}	*/

				if( nx == 0.0 )
				{
					hr = (ny >= 0.0)? 0.0 : M_PI;
				} 
				else if( nx < 0.0 )
				{
					hr = M_PI_270 - atan(ny / nx);
				} 
				else
				{
					hr = M_PI_090 - atan(ny / nx);
				}

				hs	= -tan( acos( nz/n_norm ) );	// vb-code:  hs = -Tan(arccos(nz / betrag_n))
				
				// vb-code: If hr <= (i - 1) * PI / 4 Or hr >= i * PI / 4 Then
				//SHOULD IT BE LIKE THIS: (( hr <= i * M_PI_045 || hr >= j * M_PI_045 )  OR AS BELOW???
				if( hr < i * M_PI_045 || hr > (i+1) * M_PI_045 )
				{
					if( dz[i] > dz[j] )
					{
						hr	= i * M_PI_045;
						hs	= dz[i] / Get_Length(i);
					}
					else
					{
						hr	= j * M_PI_045;
						hs	= dz[j] / Get_Length(j);						
					}
				}				
			}
			else if( dz[i] > 0.0 )
			{
				hr	= i * M_PI_045;
				hs	= dz[i] / Get_Length(i);
			}
			
			s_facet[i]	= hs;
			r_facet[i]	= hr;
		}
	}
	
	//-----------------------------------------------------
	double	dzSum	= 0.0;
	
	for(i=0; i<8; i++)
	{		
		valley[i]	= 0.0;

		int	j	= i < 7 ? i + 1 : 0;
		
		if( s_facet[i] > 0.0 )
		{
			if( r_facet[i] > i * M_PI_045 && r_facet[i] < (i+1) * M_PI_045 )
			{
				valley[i] = s_facet[i];
			}
			else if( r_facet[i] == r_facet[j] )
			{
				valley[i] = s_facet[i];
			}
			else if( s_facet[j] == -999.0 && r_facet[i] == (i+1) * M_PI_045 )
			{
				valley[i] = s_facet[i];
			}
			else
			{
				j = i > 0 ? i - 1 : 7;

				if( s_facet[j] == -999.0 && r_facet[i] == i * M_PI_045 )
				{
					valley[i] = s_facet[i];
				}
			}
			
			valley[i] = pow(valley[i], m_Converge);
			dzSum += valley[i];
		} 
		
		portion[i] = 0.0;
	}

	//-----------------------------------------------------
	if( dzSum )
	{
		for(i=0; i<8; i++)
		{
			int	j	= i < 7 ? i + 1 : 0;

			if( i >= 7 && r_facet[i] == 0.0 )
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

		for(i=0; i<8; i++)
		{
			Add_Fraction(x, y, i, portion[i]);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//				Braunschweiger Reliefmodell				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow_BRM.h"

#define BRM_nint(x)		((int)(x >= 0 ? x + 0.5 : x - 0.5))


//---------------------------------------------------------
void CFlow_Parallel::Set_BRM(	int x, int y )
{
	int		Dir, QBinaer,
			ix[3], iy[3],
			nexp[6];

	double	QLinks, QMitte, QRecht,
			nnei[6];

	//-----------------------------------------------------
	if( is_InGrid(x, y, 1) ) // Rand !!!
	{
		Dir	= BRM_InitRZ(x,y,ix,iy);

		if( Dir >= 0 )
		{
			if( Dir % 2 )
			{
				BRM_GetDiago(Dir,x,y,ix,iy,nnei,nexp);
				BRM_QStreuung(4,1,nnei,nexp,QBinaer,QLinks,QMitte,QRecht);
			}
			else
			{
				BRM_GetOrtho(Dir,x,y,ix,iy,nnei,nexp);
				BRM_QStreuung(6,0,nnei,nexp,QBinaer,QLinks,QMitte,QRecht);
			}

			Add_Fraction(x,y,(Dir+1)%8,BRM_BitMtrx[0][QBinaer] ? QLinks : 0);
			Add_Fraction(x,y,(Dir+0)%8,BRM_BitMtrx[1][QBinaer] ? QMitte : 0);
			Add_Fraction(x,y,(Dir+7)%8,BRM_BitMtrx[2][QBinaer] ? QRecht : 0);
		}
	}
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_Init(void)
{
	int 	i;

	double	DXT	= Get_Cellsize()/2,
			DYT	= Get_Cellsize()/2;

	//-----------------------------------------------------
	BRM_kgexp[0]	= (int)(atan2(DXT     , Get_Cellsize()) * M_RAD_TO_DEG);
	BRM_kgexp[1]	= (int)(atan2(Get_Cellsize(), DYT     ) * M_RAD_TO_DEG) + 1;
	BRM_kgexp[2]	= (int)(atan2(Get_Cellsize(),-DYT     ) * M_RAD_TO_DEG);
	BRM_kgexp[3]	= (int)(atan2(DXT     ,-Get_Cellsize()) * M_RAD_TO_DEG) + 1;

	for(i=0; i<4; i++)
		BRM_kgexp[i+4]	= BRM_kgexp[i] + 180;

	//---BRM_idreh---------------------------------------------
	BRM_idreh[0]	= 180;
	BRM_idreh[1]	= 180 -	BRM_nint(atan2(Get_Cellsize(), Get_Cellsize()) * M_RAD_TO_DEG);
	BRM_idreh[2]	=  90;
	BRM_idreh[3]	=		BRM_nint(atan2(Get_Cellsize(), Get_Cellsize()) * M_RAD_TO_DEG);
	BRM_idreh[4]	=   0;

	for(i=1; i<4; i++)
		BRM_idreh[i+4]	= BRM_idreh[i] + 180;
}

//---------------------------------------------------------
int CFlow_Parallel::BRM_InitRZ(int x, int y, int ix[3], int iy[3])
{
	int		i, j, Dir;

	double	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	Aspect	*= M_RAD_TO_DEG;

	if( Aspect < 0 )
	{
		return( -1 );
	}

	//---Kategorisierte-Exposition-------------------------
	Dir	= 0;
	while( Aspect > BRM_kgexp[Dir] && Dir < 8 )
		Dir++;
	Dir	%=8;

    //---Finde-Die-3-ZielRasterZellen----------------------
	for(i=0; i<3; i++)	// zxy[]: 0=Recht, 1=Mitte, 2=Links
	{
		j		= (Dir + 7 + i) % 8;
		ix[2-i]	= Get_xTo(j,x);
		iy[2-i]	= Get_yTo(j,y);
	}

	return(Dir);
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_GetOrtho(int Dir, int x, int y, int ix[3], int iy[3], double nnei[6], int nexp[6])
{
	int		jx, jy, i,
			i0	= (Dir + 2) % 8,
			i4	= (Dir + 6) % 8;

	double	Slope, Aspect;

	for(i=0; i<3; i++)
	{
		jx		= ix[i];
		jy		= iy[i];

		Get_Gradient(jx, jy, Slope, Aspect);

		nnei[i]	= M_RAD_TO_DEG * Slope;
		nexp[i]	= (int)(M_RAD_TO_DEG * Aspect);
	}

	jx		= Get_xTo(i0,x);
	jy		= Get_yTo(i0,y);

	Get_Gradient(jx, jy, Slope, Aspect);

	nnei[3]	= M_RAD_TO_DEG * Slope;
	nexp[3]	= (int)(M_RAD_TO_DEG * Aspect);

	jx		= Get_xTo(i4,x);
	jy		= Get_yTo(i4,y);

	Get_Gradient(jx, jy, Slope, Aspect);

	nnei[5]	= M_RAD_TO_DEG * Slope;
	nexp[5]	= (int)(M_RAD_TO_DEG * Aspect);

	Get_Gradient(x, y, Slope, Aspect);

	nnei[4]	= M_RAD_TO_DEG * Slope;
	nexp[4]	= (int)(M_RAD_TO_DEG * Aspect);	//[jy][jx]) ????!!!!...;

	for(i=0; i<6; i++)
		if(nexp[i]<0)
			nexp[i]	= nexp[4];

	for(i=0; i<6; i++)
	{
		nexp[i]	+= BRM_idreh[Dir];

		if(nexp[i]>360)
			nexp[i]	-= 360;
	}
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_GetDiago(int Dir, int x, int y, int ix[3], int iy[3], double nnei[6], int nexp[6])
{
	int		i;

	double	Slope, Aspect;

	Get_Gradient( x   ,  y   , Slope, Aspect);
	nexp[0]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[0]	= M_RAD_TO_DEG * Slope;

	Get_Gradient(ix[0], iy[0], Slope, Aspect);
	nexp[1]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[1]	= M_RAD_TO_DEG * Slope;

	Get_Gradient(ix[2], iy[2], Slope, Aspect);
	nexp[2]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[2]	= M_RAD_TO_DEG * Slope;

	Get_Gradient(ix[1], iy[1], Slope, Aspect);
	nexp[3]	= (int)(M_RAD_TO_DEG * Aspect);
	nnei[3]	= M_RAD_TO_DEG * Slope;

	for(i=1; i<4; i++)
		if(nexp[i]<0)
			nexp[i]	= nexp[0];

	for(i=0; i<4; i++)
	{
		nexp[i]	+= BRM_idreh[Dir];

		if(nexp[i]>360)
			nexp[i]	-= 360;
	}
}

//---------------------------------------------------------
void CFlow_Parallel::BRM_QStreuung(int i64, int g64, double nnei[6], int nexp[6], int &QBinaer, double &QLinks, double &QMitte, double &QRecht)
{
	int		i, j, ix, iy,
			ALinks, AMitte=2, ARecht;

	double	x=1, y=1, sg=0, a,
			s[6], c[6];

	ALinks	= ARecht	= 0;
	QLinks	= QRecht	= 0.0;

	for(i=0; i<i64; i++)
		sg	+= nnei[i];

	sg	= i64 / sg;

	for(i=0; i<i64; i++)
	{
		a		= sg * nnei[i];
		s[i]	= a * -sin(nexp[i] * M_DEG_TO_RAD);
		c[i]	= a * -cos(nexp[i] * M_DEG_TO_RAD);
	}

    //---QLinks-ermitteln----------------------------------
	for(i=0; i<100; i++)
	{
		ix	= BRM_nint(x) - 1;
		iy	= BRM_nint(y) - 1;

		for(j=0; j<i64; j++)
		{
			a		= BRM_g[g64][j][ix][iy];
			x		+= s[j] * a;
			y		+= c[j] * a;
		}	 

		if(x<1)
		{
			ALinks	= 0;
			QLinks	= 0;
			break;
		}

		if( x>8.99 || y<1 )
		{
			ALinks	= 4;
			QLinks	= 1;			
			break;
		}

		if(y>8.95)
		{
			if(x<1.02)
			{
				ALinks	= 0;
				QLinks	= 0;
			}
			else
			{
				ALinks	= 4;
				QLinks	= (x - 1) / 8;

				if(i64==4)
				{
					if(QLinks<0.5)
						QLinks	= QLinks * (1.67 - QLinks * 1.078);
					else
						QLinks	= QLinks * 0.869 + 0.131;
				}
			}
			break;
		}
	}


    //---QRechts-ermitteln---------------------------------
	x	= 9;
	y	= 1;

	for(i=0; i<100; i++)
	{
		ix	= BRM_nint(x) - 1;
		iy	= BRM_nint(y) - 1;

		for(j=0; j<i64; j++)
		{
			a	= BRM_g[g64][j][ix][iy];
			x	+= s[j] * a;
			y	+= c[j] * a;
		}

		if(x>9)
		{
			ARecht	= 0;
			QRecht	= 0;
			break;
		}

		if( x<1.01 || y<1 )
		{
			ARecht	= 1;
			QRecht	= 1;
			break;
		}

		if(y>8.95)
		{
			if(x>8.98)
			{
				ARecht	= 0;
				QRecht	= 0;
			}
			else
			{
				ARecht	= 1;
				QRecht	= 1 - (x - 1) / 8;

				if(i64==4)					
				{
					if(QRecht<0.5)
						QRecht	= QRecht * (1.67 - QRecht * 1.078);
					else
						QRecht	= QRecht * 0.869 + 0.131;
				}
			}
			break;
		}
	}

	//---QMitte-ist-Rest-/-QBinaer'Bits-setzen-------------
	QMitte	= 1 - QLinks - QRecht;
	QBinaer	= ALinks + AMitte + ARecht;

	if(QMitte<=0.01)
	{
		a		= QLinks + QRecht;
		QLinks	/= a;
		QRecht	/= a;
		QMitte	= 0;
		QBinaer	-= 2;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
