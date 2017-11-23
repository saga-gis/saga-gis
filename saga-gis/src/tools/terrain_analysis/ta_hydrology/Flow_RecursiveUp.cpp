/**********************************************************
 * Version $Id: Flow_RecursiveUp.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                 Flow_RecursiveUp.cpp                  //
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
#include "Flow_RecursiveUp.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_RecursiveUp::CFlow_RecursiveUp(void)
{
	Set_Name		(_TL("Flow Accumulation (Recursive)"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Recursive upward processing of cells for calculation of flow accumulation "
		"and related parameters. This set of algorithms processes recursively all "
		"upwards connected cells until each cell of the DEM has been processed.\n"
		"\n"
		"Flow routing methods provided by this tool:<ul>"
		"<li>Deterministic 8 (aka D8, O'Callaghan & Mark 1984)</li>"
		"<li>Rho 8 (Fairfield & Leymarie 1991)</li>"
		"<li>Multiple Flow Direction (Freeman 1991, Quinn et al. 1991)</li>"
		"<li>Deterministic Infinity (Tarboton 1997)</li>"
		"</ul>"
	));

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

	Add_Reference("Quinn, P.F., Beven, K.J., Chevallier, P. & Planchon, O.", "1991",
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models",
		"Hydrological Processes, 5:59-79.",
		SG_T("https://www.researchgate.net/profile/Olivier_Planchon/publication/32978462_The_Prediction_of_Hillslope_Flow_Paths_for_Distributed_Hydrological_Modeling_Using_Digital_Terrain_Model/links/0912f5130c356c86e6000000.pdf"),
		SG_T("ResearchGate")
	);

	Add_Reference("Tarboton, D.G.", "1997",
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",
		"Water Resources Research, Vol.33, No.2, p.309-319.",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1029/96WR03137/pdf"),
		SG_T("Wiley")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"TARGETS"		, _TL("Target Areas"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"FLOW_LENGTH"	, _TL("Flow Path Length"),
		_TL("average distance that a cell's accumulated flow travelled"),
		PARAMETER_OUTPUT_OPTIONAL
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
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction")
		), 3
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"CONVERGENCE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction Algorithm (Freeman 1991)"),
		1.1, 0.0, true
	);

	Parameters.Add_Bool("",
		"NO_NEGATIVES"	, _TL("Prevent Negative Flow Accumulation"),
		_TL("when using weights: do not transport negative flow, set it to zero instead; useful e.g. when accumulating measures of water balance."),
		true
	);
	
	//-----------------------------------------------------
	m_Flow	= NULL;

	Parameters.Del_Parameter("STEP");	// is not in usage here
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_RecursiveUp::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("CONVERGENCE", pParameter->asInt() == 4 || pParameter->asInt() == 5);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "WEIGHTS") )
	{
		pParameters->Set_Enabled("NO_NEGATIVES", pParameter->asGrid() != NULL);
	}

	return( CFlow::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::On_Create(void)
{
	int	x, y;

	//-----------------------------------------------------
	On_Destroy();

	m_Flow		= (double ***)SG_Malloc(    Get_NY    () * sizeof(double **));
	double *p	= (double   *)SG_Malloc(8 * Get_NCells() * sizeof(double   ));

	for(y=0; y<Get_NY(); y++)
	{
		m_Flow[y]	= (double **)SG_Malloc( Get_NX    () * sizeof(double  *));

		for(x=0; x<Get_NX(); x++, p+=8)
		{
			m_Flow[y][x]	= p;
		}
	}

	//-----------------------------------------------------
	Lock_Create();

	int Method	= Parameters("METHOD")->asInt();

	memset(m_Flow[0][0], 0, 8 * Get_NCells() * sizeof(double) );

	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pRoute && m_pRoute->asChar(x, y) > 0 )
			{
				m_Flow[y][x][m_pRoute->asChar(x, y) % 8]	= 1.0;
			}
			else switch( Method )
			{
				case 0:	Set_D8  (x, y);	break;
				case 1:	Set_Rho8(x, y);	break;
				case 2:	Set_DInf(x, y);	break;
				case 3:	Set_MFD (x, y);	break;
			}
		}
	}
}

//---------------------------------------------------------
void CFlow_RecursiveUp::On_Destroy(void)
{
	if( m_Flow )
	{
		SG_Free(m_Flow[0][0]);

		for(int y=0; y<Get_NY(); y++)
		{
			SG_Free(m_Flow[y]);
		}

		SG_Free(m_Flow);

		m_Flow	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::On_Initialize(void)
{
	m_pFlow_Length	= Parameters("FLOW_LENGTH")->asGrid();
	m_Converge		= Parameters("CONVERGENCE")->asDouble();

	m_bNoNegatives	= m_pWeights ? Parameters("NO_NEGATIVES")->asBool() : false;
	m_pLoss			= Parameters("WEIGHT_LOSS")->asGrid();

	if( m_bNoNegatives && m_pLoss )
	{
		m_pLoss->Assign_NoData();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_RecursiveUp::Calculate(void)
{
	CSG_Grid	*pTargets	= Parameters("TARGETS")->asGrid();

	On_Create();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pTargets || !pTargets->is_NoData(x, y) )
			{
				Get_Flow(x, y);
			}
		}
	}

	On_Destroy();

	return( true );
}

//---------------------------------------------------------
bool CFlow_RecursiveUp::Calculate(int x, int y)
{
	On_Create();

	Get_Flow(x,y);

	On_Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Get_Flow(int x, int y)
{
	if( is_Locked(x, y) == false )
	{
		Lock_Set (x, y);
		Init_Cell(x, y);

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( is_InGrid(ix, iy) )
			{
				int		iDir	= (i + 4) % 8;
				double	iFlow	= m_Flow[iy][ix][iDir];

				if( iFlow > 0.0 )
				{
					Get_Flow    (ix, iy);
					Add_Fraction(ix, iy, iDir, iFlow);
				}
			}
		}

		if( m_bNoNegatives && m_pFlow->asDouble(x, y) < 0.0 )
		{
			if( m_pLoss )
			{
				m_pLoss->Set_Value(x, y, fabs(m_pFlow->asDouble(x, y)));
			}

			m_pFlow->Set_Value(x, y, 0.0);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_D8(int x, int y)
{
	int		Direction	= m_pDTM->Get_Gradient_NeighborDir(x,y);

	if( Direction >= 0 )
	{
		m_Flow[y][x][Direction % 8]	= 1.0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_Rho8(int x, int y)
{
	int		Direction;

	double 	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	Aspect	*= M_RAD_TO_DEG;

	if( Aspect >= 0 )
	{
		Direction	= (int)(Aspect / 45.0);

		if( fmod(Aspect,45) / 45.0 > rand() / (double)RAND_MAX )
			Direction++;

		Direction	%= 8;

		m_Flow[y][x][Direction]	= 1.0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_DInf(int x, int y)
{
	int		Direction;

	double 	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	Aspect	*= M_RAD_TO_DEG;

	if( Aspect >= 0 )
	{
		Direction	= (int)(Aspect / 45.0);
		Aspect		= fmod(Aspect,45) / 45.0;

		m_Flow[y][x][(Direction    ) % 8]	= 1 - Aspect;
		m_Flow[y][x][(Direction + 1) % 8]	=     Aspect;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_MFD(int x, int y)
{
	int		i;
	double	z, *dz, dzSum;

	for(i=0, dzSum=0.0, dz=m_Flow[y][x], z=m_pDTM->asDouble(x, y); i<8; i++)
	{
		int	ix	= Get_xTo(i,x);
		int	iy	= Get_yTo(i,y);

		if( is_InGrid(ix,iy) )
		{
			double	d	= z - m_pDTM->asDouble(ix,iy);

			if( d > 0 )
			{
				dzSum	+= dz[i]	= pow(d / Get_Length(i), m_Converge);
			}
		}
	}

	if( dzSum )
	{
		for(i=0; i<8; i++)
		{
			if( dz[i] > 0 )
			{
				dz[i]	/= dzSum;
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
