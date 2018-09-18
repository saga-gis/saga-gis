/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              ChannelNetwork_Distance.cpp              //
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
#include "ChannelNetwork_Distance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChannelNetwork_Distance::CChannelNetwork_Distance(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Overland Flow Distance to Channel Network"));

	Set_Author		("O.Conrad (c) 2001-14");

	Set_Description	(_TW(
		"This tool calculates overland flow distances to a channel network "
		"based on gridded digital elevation data and channel network information. "
		"The flow algorithm may be either Deterministic 8 (O'Callaghan & Mark 1984) "
		"or Multiple Flow Direction (Freeman 1991). Sediment Delivery Rates (SDR) "
		"according to Ali & De Boer (2010) can be computed optionally. "
	));

	Add_Reference(
		"Ali, K. F., De Boer, D. H.", "2010",
		"Spatially distributed erosion and sediment yield modeling in the upper Indus River basin",
		"Water Resources Research, 46(8), W08504. doi:10.1029/2009WR008762"
	);

	Add_Reference(
		"Freeman, G.T.", "1991",
		"Calculating catchment area with divergent flow based on a regular grid",
		"Computers and Geosciences, 17:413-22."
	);

	Add_Reference(
		"O'Callaghan, J.F., Mark, D.M.", "1984",
		"The extraction of drainage networks from digital elevation data",
		"Computer Vision, Graphics and Image Processing, 28:323-344."
	);

	Add_Reference(
		"Nobre, A.D., Cuartas, L.A., Hodnett, M., Renno, C.D., Rodrigues, G., Silveira, A., Waterloo, M., Saleska S.", "2011",
		"Height Above the Nearest Drainage - a hydrologically relevant new terrain model",
		"Journal of Hydrology, Vol. 404, Issues 1-2, pp. 13-29, ISSN 0022-1694, 10.1016/j.jhydrol.2011.03.051.",
		SG_T("http://www.sciencedirect.com/science/article/pii/S0022169411002599")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CHANNELS"	, _TL("Channel Network"),
		_TW("A grid providing information about the channel network. It is assumed that no-data cells are not part "
		"of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"ROUTE"		, _TL("Preferred Routing"),
		_TL("Downhill flow is bound to preferred routing cells, where these are not no-data. Helps to model e.g. small ditches, that are not well represented in the elevation data."),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DISTANCE"	, _TL("Overland Flow Distance"),
		_TW("The overland flow distance in map units. "
		"It is assumed that the (vertical) elevation data use the same units as the (horizontal) grid coordinates."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"DISTVERT"	, _TL("Vertical Overland Flow Distance"),
		_TL("This is the vertical component of the overland flow"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"DISTHORZ"	, _TL("Horizontal Overland Flow Distance"),
		_TL("This is the horizontal component of the overland flow"),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"TIME"		, _TL("Flow Travel Time"),
		_TL("flow travel time to channel expressed in hours based on Manning's Equation"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"SDR"		, _TL("Sediment Yield Delivery Ratio"),
		_TL("This is the horizontal component of the overland flow"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"FIELDS"	, _TL("Fields"),
		_TL("If set, output is given about the number of fields a flow path visits downhill. For D8 only."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"PASSES"	, _TL("Fields Visited"),
		_TL("Number of fields a flow path visits downhill starting at a cell. For D8 only."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"	, _TL("Flow Algorithm"),
		_TL("Choose a flow routing algorithm that shall be used for the overland flow distance calculation:\n- D8\n- MFD"),
		CSG_String::Format("%s|%s|",
			_TL("D8"),
			_TL("MFD")
		), 1
	);

	Parameters.Add_Bool("",
		"BOUNDARY"	, _TL("Boundary Cells"),
		_TL("Take cells at the boundary of the DEM as channel."),
		false
	);

	Parameters.Add_Double("SDR",
		"FLOW_B"	, _TL("Beta"),
		_TL("catchment specific parameter for sediment delivery ratio calculation"),
		1.0, 0.0, true
	);

	Parameters.Add_Grid_or_Const("",
		"FLOW_K"	, _TL("Manning-Strickler Coefficient"),
		_TL("Manning-Strickler coefficient for flow travel time estimation (reciprocal of Manning's Roughness Coefficient)"),
		20.0, 0.0, true
	);

	Parameters.Add_Grid_or_Const("",
		"FLOW_R"	, _TL("Flow Depth"),
		_TL("flow depth [m] for flow travel time estimation"),
		0.05, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CChannelNetwork_Distance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("FIELDS", pParameter->asInt() == 0);
		pParameters->Set_Enabled("PASSES", pParameter->asInt() == 0 && (*pParameters)("FIELDS")->asPointer() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FIELDS") )
	{
		pParameters->Set_Enabled("PASSES", pParameter->is_Enabled() && pParameter->asPointer() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TIME") )
	{
		pParameters->Set_Enabled("FLOW_K", pParameter->asDataObject() != NULL);
		pParameters->Set_Enabled("FLOW_R", pParameter->asDataObject() != NULL);
		pParameters->Set_Enabled("SDR"   , pParameter->asDataObject() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SDR") )
	{
		pParameters->Set_Enabled("FLOW_B", pParameter->asDataObject() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Distance::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("ELEVATION")->asGrid();
	m_pChannels	= Parameters("CHANNELS" )->asGrid();
	m_pRoute	= Parameters("ROUTE"    )->asGrid();

	m_pDistance	= Parameters("DISTANCE" )->asGrid();
	m_pDistVert	= Parameters("DISTVERT" )->asGrid();
	m_pDistHorz	= Parameters("DISTHORZ" )->asGrid();

	m_pTime		= Parameters("TIME"     )->asGrid();
	m_pSDR		= Parameters("SDR"      )->asGrid();

	m_Flow_B	= Parameters("FLOW_B"   )->asDouble();
	m_Flow_K	= Parameters("FLOW_K"   )->asDouble();
	m_Flow_R	= Parameters("FLOW_R"   )->asDouble();
	m_pFlow_K	= Parameters("FLOW_K"   )->asGrid();
	m_pFlow_R	= Parameters("FLOW_R"   )->asGrid();

	m_pFields	= Parameters("FIELDS"   )->asGrid();
	m_pPasses	= Parameters("PASSES"   )->asGrid();

	//-----------------------------------------------------
	m_pDistance->Assign_NoData();
	m_pDistVert->Assign_NoData();
	m_pDistHorz->Assign_NoData();

	if( m_pTime )
	{
		m_pTime->Assign_NoData();
		m_pTime->Set_Unit("hours");
	}

	if( m_pSDR  )
	{
		m_pSDR ->Assign_NoData();
	}

	if( m_pFields && m_pPasses )
	{
		m_pPasses->Set_NoData_Value(-1.0);
		m_pPasses->Assign_NoData();
	}

	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	int  Method		= Parameters("METHOD")->asInt();
	bool bBoundary	= Parameters("BOUNDARY")->asBool();

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;

		if( m_pDEM->Get_Sorted(n, x, y, false) )	// ascending, only valid dem cells
		{
			if( is_Channel(x, y, bBoundary) )
			{
				m_pDistance->Set_Value(x, y, 0.0);
				m_pDistVert->Set_Value(x, y, 0.0);
				m_pDistHorz->Set_Value(x, y, 0.0);

				if( m_pTime   ) m_pTime  ->Set_Value(x, y, 0.0);
				if( m_pFields ) m_pPasses->Set_Value(x, y, 0.0);
			}
			else // if( m_pDEM->is_InGrid(x, y) )
			{
				switch( Method )
				{
				default: Set_D8 (x, y); break;
				case  1: Set_MFD(x, y); break;
				}

				if( m_pSDR && !m_pTime->is_NoData(x, y) )
				{
					m_pSDR->Set_Value(x, y, exp(-m_Flow_B * m_pTime->asDouble(x, y)));
				}
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
inline bool CChannelNetwork_Distance::is_Channel(int x, int y, bool bBoundary)
{
	if( !m_pChannels->is_NoData(x, y) )
	{
		return( true );
	}

	if( bBoundary )
	{
		for(int i=0; i<8; i++)
		{
			if( !m_pDEM->is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
			{
				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CChannelNetwork_Distance::Get_Travel_Time(int x, int y, int Direction)
{
	int	ix = Get_xTo(Direction, x), iy = Get_yTo(Direction, y);

	double	v, k, R, dz, dx;

	dz	= m_pDEM->is_InGrid(ix, iy) ? m_pDEM->asDouble(x, y) - m_pDEM->asDouble(ix, iy) : 0.1;
	dx	= Get_Length(Direction);

//	k	= m_pFlow_K && !m_pFlow_K->is_NoData(x, y) ? m_pFlow_K->asDouble(x, y) : m_Flow_K;
	k	= !m_pFlow_K || (m_pFlow_K->is_NoData(x, y) && m_pFlow_K->is_NoData(ix, iy)) ? m_Flow_K
		: m_pFlow_K->is_NoData( x,  y) ? m_pFlow_K->asDouble(ix, iy)
		: m_pFlow_K->is_NoData(ix, iy) ? m_pFlow_K->asDouble( x,  y)
		: (m_pFlow_K->asDouble(x, y) + m_pFlow_K->asDouble(ix, iy)) / 2.0;

//	R	= m_pFlow_R && !m_pFlow_R->is_NoData(x, y) ? m_pFlow_R->asDouble(x, y) : m_Flow_R;
	R	= !m_pFlow_R || (m_pFlow_R->is_NoData(x, y) && m_pFlow_R->is_NoData(ix, iy)) ? m_Flow_R
		: m_pFlow_R->is_NoData( x,  y) ? m_pFlow_R->asDouble(ix, iy)
		: m_pFlow_R->is_NoData(ix, iy) ? m_pFlow_R->asDouble( x,  y)
		: (m_pFlow_R->asDouble(x, y) + m_pFlow_R->asDouble(ix, iy)) / 2.0;

	v	= k * pow(R, 2.0 / 3.0) * sqrt(dz / dx);	// [m / s], simplified Manning equation

	return( dx / (v * 3600.0) );	// return travel time in hours
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Distance::Get_D8(int x, int y, int &Direction)
{
	double	dz, dzMax = 0.0, z = m_pDEM->asDouble(x, y);

	Direction	= -1;

	if( m_pRoute )
	{
		for(int i=0; i<8; i++)
		{
			int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && !m_pRoute->is_NoData(ix, iy) && (dz = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > dzMax )
			{
				Direction	= i; dzMax	= dz;
			}
		}

		if( Direction >= 0 )
		{
			return( true );
		}
	}

	for(int i=0; i<8; i++)
	{
		int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && !m_pDistance->is_NoData(ix, iy) && (dz = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > dzMax )
		{
			Direction	= i; dzMax	= dz;
		}
	}

	return( Direction >= 0 );
}

//---------------------------------------------------------
bool CChannelNetwork_Distance::Set_D8(int x, int y)
{
	int	Direction;

	if( !Get_D8(x, y, Direction) )
	{
		return( false );
	}

	int	ix = Get_xTo(Direction, x), iy = Get_yTo(Direction, y);

	double	dz	= m_pDEM->asDouble(x, y) - m_pDEM->asDouble(ix, iy);
	double	dx	= Get_Length(Direction);

	m_pDistance->Set_Value(x, y, m_pDistance->asDouble(ix, iy) + sqrt(dz*dz + dx*dx));
	m_pDistVert->Set_Value(x, y, m_pDistVert->asDouble(ix, iy) + dz);
	m_pDistHorz->Set_Value(x, y, m_pDistHorz->asDouble(ix, iy) + dx);

	if( m_pTime )
	{
		double	dt	= Get_Travel_Time(x, y, Direction);

		m_pTime->Set_Value(x, y, m_pTime->asDouble(ix, iy) + dt);
	}

	if( m_pFields )
	{
		int	Crossed	= m_pFields->asDouble(ix, iy) == m_pFields->asDouble(x, y) ? 0 : 1;

		m_pPasses->Set_Value(x, y, m_pPasses->asInt(ix, iy) + Crossed);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Distance::Get_MFD(int x, int y, CSG_Vector &Flow)
{
	const double	MFD_Convergence	= 1.1;

	double	dz, Sum = 0.0, z = m_pDEM->asDouble(x, y);

	if( m_pRoute )
	{
		for(int i=0; i<8; i++)
		{
			int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && !m_pRoute->is_NoData(ix, iy) && (dz = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				Sum	+= (Flow[i]	= pow(dz / Get_Length(i), MFD_Convergence));
			}
		}

		if( Sum > 0.0 )
		{
			Flow	*= 1. / Sum;

			return( true );
		}
	}

	for(int i=0; i<8; i++)
	{
		int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && !m_pDistance->is_NoData(ix, iy) && (dz = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
		{
			Sum	+= (Flow[i]	= pow(dz / Get_Length(i), MFD_Convergence));
		}
	}

	if( Sum > 0.0 )
	{
		Flow	*= 1. / Sum;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CChannelNetwork_Distance::Set_MFD(int x, int y)
{
	CSG_Vector	Flow(8);

	if( !Get_MFD(x, y, Flow) )
	{
		return( false );
	}

	double	Distance = 0.0, DistVert = 0.0, DistHorz = 0.0, Time = 0.0, SDR = 0.0;

	double	z = m_pDEM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		if( Flow[i] > 0.0 )
		{
			int	ix = Get_xTo(i, x), iy = Get_yTo(i, y);

			double	dz	= z - m_pDEM->asDouble(ix, iy);
			double	dx	= Get_Length(i);

			Distance += Flow[i] * (m_pDistance->asDouble(ix, iy) + sqrt(dz*dz + dx*dx));
			DistVert += Flow[i] * (m_pDistVert->asDouble(ix, iy) + dz);
			DistHorz += Flow[i] * (m_pDistHorz->asDouble(ix, iy) + dx);

			if( m_pTime )
			{
				double	dt	= Get_Travel_Time(x, y, i);

				Time += Flow[i] * (m_pTime->asDouble(ix, iy) + dt);
			}
		}
	}

	if( Distance > 0.0 )
	{
		m_pDistance->Set_Value(x, y, Distance);
		m_pDistVert->Set_Value(x, y, DistVert);
		m_pDistHorz->Set_Value(x, y, DistHorz);

		if( m_pTime )
		{
			m_pTime->Set_Value(x, y, Time);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
