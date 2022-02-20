
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     ta_hydrology                      //
//                                                       //
//                 Olaf Conrad (C) 2018                  //
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
#include "flow_accumulation_mp.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Accumulation_MP::CFlow_Accumulation_MP(void)
{
	Set_Name		(_TL("Flow Accumulation (Parallelizable)"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"A simple implementation of a parallelizable flow accumulation algorithn."
	));

	Add_Reference("Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22.",
		SG_T("https://doi.org/10.1016/0098-3004(91)90048-I"), SG_T("doi:10.1016/0098-3004(91)90048-I")
	);

	Add_Reference("O'Callaghan, J.F. & Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344.",
		SG_T("https://doi.org/10.1016/S0734-189X(84)80011-0"), SG_T("doi:10.1016/S0734-189X(84)80011-0")
	);

	Add_Reference("Tarboton, D.G.", "1997",
		"A new method for the determination of flow directions and upslope areas in grid digital elevation models",
		"Water Resources Research, Vol.33, No.2, p.309-319.",
		SG_T("https://doi.org/10.1029/96WR03137"), SG_T("doi:10.1029/96WR03137")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"FLOW"		, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int("",
		"UPDATE"	, _TL("Update Frequency"),
		_TL("if zero no updates will be done"),
		0, 0, true
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			SG_T("D8"),
			SG_T("Dinfinity"),
			SG_T("MFD")
		), 2
	);

	Parameters.Add_Double("",
		"CONVERGENCE"	, _TL("Convergence"),
		_TL(""),
		1.1, 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_Accumulation_MP::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("CONVERGENCE", pParameter->asInt() == 2);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Accumulation_MP::On_Execute(void)
{
	if( !Initialize() )
	{
		Error_Set(_TL("initialization failed"));

		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	bool	bChanged;	int	nPasses	= 1;

	int	Update	= Parameters("UPDATE")->asInt();

	do
	{
		Process_Set_Text(CSG_String::Format("%s %d", _TL("pass"), nPasses++));

		bChanged	= false;

		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( Get_Flow(x, y) )
				{
					bChanged	= true;
				}
			}
		}

		if( Update > 0 && nPasses % Update == 0 )
		{
			DataObject_Update(m_pFlow);
		}
	}
	while( bChanged && Process_Get_Okay() );

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("number of passes"), nPasses);

	DataObject_Set_Colors   (m_pFlow, 11, SG_COLORS_WHITE_BLUE);
	DataObject_Set_Parameter(m_pFlow, "METRIC_SCALE_MODE",   1);	// increasing geometrical intervals
	DataObject_Set_Parameter(m_pFlow, "METRIC_SCALE_LOG" , 100);	// Geometrical Interval Factor

	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Accumulation_MP::Initialize(void)
{
	m_pDEM	= Parameters("DEM" )->asGrid();
	m_pFlow	= Parameters("FLOW")->asGrid();

	m_pFlow->Assign(0.0);
	m_pFlow->Set_NoData_Value(0.0);

	//-----------------------------------------------------
	for(int i=0; i<8; i++)
	{
		if( !m_Flow[i].Create(Get_System()) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	double	c	= Parameters("CONVERGENCE")->asDouble();

	int	Method	= Parameters("METHOD")->asInt();

	#ifndef _DEBUG
	#pragma omp parallel for
	#endif
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				switch( Method )
				{
				case  0: Set_D8  (x, y   ); break;
				case  1: Set_Dinf(x, y   ); break;
				default: Set_MFD (x, y, c); break;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CFlow_Accumulation_MP::Finalize(void)
{
	for(int i=0; i<8; i++)
	{
		m_Flow[i].Destroy();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Accumulation_MP::Set_D8(int x, int y)
{
	int	i	= m_pDEM->Get_Gradient_NeighborDir(x, y);

	if( i < 0 )
	{
		return( false );
	}

	m_Flow[i].Set_Value(x, y, 1.);

	return( true );
}

//---------------------------------------------------------
bool CFlow_Accumulation_MP::Set_Dinf(int x, int y)
{
	double	s, a;

	if( m_pDEM->Get_Gradient(x, y, s, a) && a >= 0.0 )
	{
		int	ix, iy, i[2];
		
		i[0] = ((int)(a / M_PI_045)); i[1] = (i[0] + 1);

		if( m_pDEM->is_InGrid(ix = Get_xTo(i[0], x), iy = Get_yTo(i[0], y)) && m_pDEM->asDouble(ix, iy) < m_pDEM->asDouble(x, y)
		&&  m_pDEM->is_InGrid(ix = Get_xTo(i[1], x), iy = Get_yTo(i[1], y)) && m_pDEM->asDouble(ix, iy) < m_pDEM->asDouble(x, y) )
		{
			double	d	= fmod(a, M_PI_045) / M_PI_045;

			m_Flow[i[0] % 8].Set_Value(x, y, 1 - d);
			m_Flow[i[1] % 8].Set_Value(x, y,     d);

			return( true );
		}
	}

	return( Set_D8(x, y) );
}

//---------------------------------------------------------
bool CFlow_Accumulation_MP::Set_MFD(int x, int y, double Convergence)
{
	//-----------------------------------------------------
	int	i;	double	dz[8], dzSum = 0.0, z = m_pDEM->asDouble(x, y);

	for(i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && (dz[i] = m_pDEM->asDouble(ix, iy)) < z )
		{
			dzSum	+= (dz[i] = pow((z - dz[i]) / Get_Length(i), Convergence));
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
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				m_Flow[i].Set_Value(x, y, dz[i] / dzSum);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Accumulation_MP::Get_Flow(int x, int y)
{
	if( !m_pFlow->is_NoData(x, y) || m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	Flow	= Get_Cellarea();

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xFrom(i, x);
		int	iy	= Get_yFrom(i, y);

		if( m_Flow[i].is_InGrid(ix, iy) && m_Flow[i].asDouble(ix, iy) > 0.0 )
		{
			if( m_pFlow->is_NoData(ix, iy) )
			{
				return( false );
			}

			Flow	+= m_Flow[i].asDouble(ix, iy) * m_pFlow->asDouble(ix, iy);
		}
	}

	m_pFlow->Set_Value(x, y, Flow);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
