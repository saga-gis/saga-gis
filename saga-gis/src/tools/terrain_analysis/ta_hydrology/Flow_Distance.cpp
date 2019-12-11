
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
//                   Flow_Distance.cpp                   //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Flow_Distance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Distance::CFlow_Distance(void)
{
	Set_Name		(_TL("Flow Path Length"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"This tool calculates the average flow path length starting from the seeds, "
		"that are given by the optional \'Seeds\' grid and optionally from cells "
		"without upslope contributing areas (i.e. summits, ridges). Seeds will be "
		"all grid cells, that are not \'no data\' values. If seeds are not given, "
		"only summits and ridges as given by the flow routing will be taken into "
		"account. Available flow routing methods are based on the \'Deterministic 8 "
		"(D8)\' (Callaghan and Mark 1984) and the \'Multiple Flow Direction (FD8)\' "
		"(Freeman 1991, Quinn et al. 1991) algorithms."
	));

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

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SEED"		, _TL("Seeds"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"LENGTH"	, _TL("Flow Path Length"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"SEEDS_ONLY", _TL("Seeds Only"),
		_TL("Use only seed cells as starting points.")
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Flow Routing Algorithm"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Deterministic 8 (D8)"),
			_TL("Multiple Flow Direction (FD8)")
		), 1
	);

	Parameters.Add_Double("",
		"CONVERGENCE"	, _TL("Convergence (FD8)"),
		_TL("Convergence factor for the \'Multiple Flow Direction\' algorithm (after Freeman 1991)"),
		1.1, 0.001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Distance::On_Execute(void)
{
	bool		bSeeds;
	int			x, y, Method;
	CSG_Grid	*pSeed;

	//-------------------------------------------------
	m_pDTM		= Parameters("ELEVATION"  )->asGrid();
	pSeed		= Parameters("SEED"       )->asGrid();
	m_pLength	= Parameters("LENGTH"     )->asGrid();

	m_Converge	= Parameters("CONVERGENCE")->asDouble();
	bSeeds		= Parameters("SEEDS_ONLY" )->asBool();
	Method		= Parameters("METHOD"     )	->asInt();

	m_pWeight	= SG_Create_Grid(m_pLength, SG_DATATYPE_Float);
	m_pWeight	->Assign(0.);
	m_pLength	->Assign(0.);

	if( !m_pDTM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		m_pDTM->Get_Sorted(n, x, y, true, false);

		if( pSeed && !pSeed->is_NoData(x, y) )
		{
			m_pLength->Set_Value(x, y, 0.0);
			m_pWeight->Set_Value(x, y, 0.0);
		}
		else if( m_pWeight->asDouble(x, y) > 0.0 )
		{
			m_pLength->Set_Value(x, y, m_pLength->asDouble(x, y) / m_pWeight->asDouble(x, y));
		}
		else if( bSeeds )
		{
			m_pLength->Set_NoData(x, y);

			continue;
		}

		switch( Method )
		{
		case  0: Set_Length_D8 (x, y); break;
		default: Set_Length_MFD(x, y); break;
		}
	}

	//-------------------------------------------------
	delete(m_pWeight);

	DataObject_Set_Colors(m_pLength, 11, SG_COLORS_WHITE_BLUE);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_Distance::Set_Length_D8(int x, int y)
{
	int		i, ix, iy;

	if( m_pDTM->is_InGrid(x, y) && (i = m_pDTM->Get_Gradient_NeighborDir(x, y, true)) >= 0 )
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) )
		{
			m_pLength->Add_Value(ix, iy, m_pLength->asDouble(x, y) + Get_Length(i));
			m_pWeight->Add_Value(ix, iy, 1.0);
		}
	}
}

//---------------------------------------------------------
void CFlow_Distance::Set_Length_MFD(int x, int y)
{
	int		i, ix, iy;
	double	z, d, dzSum, dz[8];

	if( m_pDTM->is_InGrid(x, y) )
	{
		z		= m_pDTM->asDouble(x, y);
		dzSum	= 0.0;

		for(i=0; i<8; i++)
		{
			ix		= Get_xTo(i, x);
			iy		= Get_yTo(i, y);

			if( m_pDTM->is_InGrid(ix, iy) && (d = z - m_pDTM->asDouble(ix, iy)) > 0.0 )
			{
				dz[i]	= pow(d / Get_Length(i), m_Converge);
				dzSum	+= dz[i];
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		if( dzSum > 0.0 )
		{
			d	 = m_pLength->asDouble(x, y);

			for(i=0; i<8; i++)
			{
				if( dz[i] > 0.0 )
				{
					ix		= Get_xTo(i, x);
					iy		= Get_yTo(i, y);

					dz[i]	/= dzSum;

					m_pLength->Add_Value(ix, iy, dz[i] * (d + Get_Length(i)));
					m_pWeight->Add_Value(ix, iy, dz[i]);
				}
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
