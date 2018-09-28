/**********************************************************
 * Version $Id: successive_flow_routing.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               successive_flow_routing.cpp             //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "successive_flow_routing.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSuccessive_Flow_Routing::CSuccessive_Flow_Routing(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Successive Flow Routing"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Calculation of flow accumulation, aka upslope contributing area, "
		"with the multiple flow direction method after Freeman (1991)."
	));

	Add_Reference("Freeman, G.T.",
		"1991", "Calculating catchment area with divergent flow based on a rectangular grid",
		"Computers & Geosciences, 17, pp.413-422."
	);

	Add_Reference("Pelletier, J.D.",
		"2008", "Quantitative Modeling of Earth Surface Processes",
		"Cambridge, 295p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"FLOW"		, _TL("Flow"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int("",
		"ITERATIONS", _TL("Iterations"),
		_TL(""),
		100, 1, true
	);

	Parameters.Add_Double("",
		"RUNOFF"	, _TL("Runoff"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Double("",
		"MANNING"	, _TL("Manning's Roughness"),
		_TL(""),
		0.2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSuccessive_Flow_Routing::On_Execute(void)
{
	int			x, y, nIterations;
	double		Runoff;
	CSG_Grid	*pDEM, DEM, Slope;

	nIterations	= Parameters("ITERATIONS")->asInt();
	Runoff		= Parameters("RUNOFF"    )->asDouble();
	m_Manning	= Parameters("MANNING"   )->asDouble();

	pDEM		= Parameters("DEM"  )->asGrid();
	m_pFlow		= Parameters("FLOW" )->asGrid();

	m_pDEM		= &DEM;
	m_pSlope	= &Slope;

	DEM			.Create(*pDEM);
	Slope		.Create(*Get_System());

	CSG_Colors	Colors(11, SG_COLORS_WHITE_BLUE);
	Colors[0]	= SG_GET_RGB(255, 255, 200);
	DataObject_Set_Colors(m_pFlow, Colors);
	DataObject_Update(m_pFlow, SG_UI_DATAOBJECT_SHOW);

	Fill_Sinks();

	#pragma omp parallel for private(x, y)
	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Set_Channel_Slope(x, y);
		}
	}

	//-----------------------------------------------------
	for(int Iteration=1; Iteration<=nIterations && Process_Get_Okay(); Iteration++)
	{
		Process_Set_Text("%s: %d [%d]", _TL("Iteration"), Iteration, nIterations);

		SG_UI_Progress_Lock(true);

		Fill_Sinks();

		#pragma omp parallel for private(x, y)
		for(y=0; y<Get_NY(); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				m_pFlow->Set_Value(x, y, pow(Runoff, 1.66667) * m_pSlope->asDouble(x, y));
			}
		}

		for(int i=0; i<Get_NCells() && Process_Get_Okay(); i++)
		{
			if( m_pDEM->Get_Sorted(i, x, y) )
			{
				Set_Flow(x, y);
			}
		}

		#pragma omp parallel for private(x, y)
		for(y=0; y<Get_NY(); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( m_pDEM->is_NoData(x, y) )
				{
					m_pFlow->Set_NoData(x, y);
				}
				else
				{
					double	Depth	= pow(m_pFlow->asDouble(x, y) / m_pSlope->asDouble(x, y), 0.6);

					if( m_pDEM->asDouble(x, y) < pDEM->asDouble(x, y) + Depth )
					{
						m_pDEM->Set_Value(x, y, pDEM->asDouble(x, y) + Depth / nIterations);
					}

					m_pFlow->Set_Value(x, y, m_pDEM->asDouble(x, y) - pDEM->asDouble(x, y));
				}
			}
		}

		DataObject_Update(m_pFlow);

		SG_UI_Progress_Lock(false);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RUN_TOOL(LIBRARY, TOOL, CONDITION)	{\
	bool	bResult;\
	SG_RUN_TOOL(bResult, LIBRARY, TOOL, CONDITION)\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pTool->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)

void CSuccessive_Flow_Routing::Fill_Sinks(void)
{
	RUN_TOOL("ta_preprocessor", 2,	// CPit_Eliminator
			SET_PARAMETER("DEM"	, m_pDEM)
	)
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSuccessive_Flow_Routing::Set_Flow(int x, int y)
{
	if( !m_pDEM->is_NoData(x, y) )
	{
		int		i;
		double	z, dz[8], dzSum;

		for(i=0, dzSum=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && (dz[i] = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				dzSum	+= (dz[i] = pow(dz[i] / Get_Length(i), 1.1));
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		if( dzSum > 0.0 )
		{
			for(i=0, z=m_pFlow->asDouble(x, y)/dzSum; i<8; i++)
			{
				if( dz[i] > 0.0 )
				{
					m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), z * dz[i]);
				}
			}
		}
	}
}

//---------------------------------------------------------
void CSuccessive_Flow_Routing::Set_Channel_Slope(int x, int y)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		m_pSlope->Set_NoData(x, y);
	}
	else
	{
		int		i;
		double	z, dz, Slope;

		for(i=0, Slope=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && (dz = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > Slope )
			{
				Slope	= dz;
			}
		}

		m_pSlope->Set_Value(x, y, sqrt(Slope > 0.01 ? Slope : 0.01) * Get_Cellsize() / m_Manning);
	}
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
