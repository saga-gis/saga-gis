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

	Set_Author		(SG_T("O.Conrad (c) 2001-14"));

	Set_Description	(_TW(
		"This tool calculates overland flow distances to a channel network "
		"based on gridded digital elevation data and channel network information. "
		"The flow algorithm may be either Deterministic 8 (O'Callaghan & Mark 1984) "
		"or Multiple Flow Direction (Freeman 1991). Sediment Delivery Rates (SDR) "
		"according to Ali & De Boer (2010) can be computed optionally. "
		"\n\nReferences:\n"
		"- Ali, K. F., De Boer, D. H. (2010): Spatially distributed erosion and sediment yield modeling in the upper Indus River basin."
		" Water Resources Research, 46(8), W08504. doi:10.1029/2009WR008762\n"
		"- Freeman, G.T., 1991: Calculating catchment area with divergent flow based on a regular grid."
		" Computers and Geosciences, 17:413-22.\n"
		"- O'Callaghan, J.F., Mark, D.M., 1984: The extraction of drainage networks from digital elevation data."
		" Computer Vision, Graphics and Image Processing, 28:323-344.\n"
		"- Nobre, A.D., Cuartas, L.A., Hodnett, M., Renno, C.D., Rodrigues, G., Silveira, A., Waterloo, M., Saleska S. (2011): Height Above the Nearest Drainage - a hydrologically relevant new terrain model."
		" Journal of Hydrology, Vol. 404, Issues 1-2, pp. 13-29, ISSN 0022-1694, 10.1016/j.jhydrol.2011.03.051."
		" <a target=\"_blank\" href=\"http://www.sciencedirect.com/science/article/pii/S0022169411002599\">online</a>\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHANNELS"	, _TL("Channel Network"),
		_TW("A grid providing information about the channel network. It is assumed that no-data cells are not part "
		"of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "ROUTE"		, _TL("Preferred Routing"),
		_TL("Downhill flow is bound to preferred routing cells, where these are not no-data. Helps to model e.g. small ditches, that are not well represented in the elevation data."),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DISTANCE"	, _TL("Overland Flow Distance"),
		_TW("The overland flow distance in map units. "
		"It is assumed that the (vertical) elevation data use the same units as the (horizontal) grid coordinates."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTVERT"	, _TL("Vertical Overland Flow Distance"),
		_TL("This is the vertical component of the overland flow"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISTHORZ"	, _TL("Horizontal Overland Flow Distance"),
		_TL("This is the horizontal component of the overland flow"),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "TIME"		, _TL("Flow Travel Time"),
		_TL("flow travel time to channel expressed in hours based on Manning's Equation"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SDR"			, _TL("Sediment Yield Delivery Ratio"),
		_TL("This is the horizontal component of the overland flow"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "FIELDS"		, _TL("Fields"),
		_TL("If set, output is given about the number of fields a flow path visits downhill. For D8 only."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "PASSES"		, _TL("Fields Visited"),
		_TL("Number of fields a flow path visits downhill starting at a cell. For D8 only."),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Short
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Flow Algorithm"),
		_TL("Choose a flow routing algorithm that shall be used for the overland flow distance calculation:\n- D8\n- MFD"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("D8"),
			_TL("MFD")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "FLOW_B"		, _TL("Beta"),
		_TL("catchment specific parameter for sediment delivery ratio calculation"),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Grid_or_Const(
		NULL	, "FLOW_K"		, _TL("Manning-Strickler Coefficient"),
		_TL("Manning-Strickler coefficient for flow travel time estimation (reciprocal of Manning's Roughness Coefficient)"),
		20.0, 0.0, true
	);

	Parameters.Add_Grid_or_Const(
		NULL	, "FLOW_R"		, _TL("Flow Depth"),
		_TL("flow depth [m] for flow travel time estimation"),
		0.05, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CChannelNetwork_Distance::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Get_Parameter("FIELDS")->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("PASSES")->Set_Enabled(pParameter->asInt() == 0 && pParameters->Get_Parameter("FIELDS")->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FIELDS") )
	{
		pParameters->Get_Parameter("PASSES")->Set_Enabled(pParameter->is_Enabled() && pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TIME") || !SG_STR_CMP(pParameter->Get_Identifier(), "SDR") )
	{
		bool	bEnable	= pParameters->Get_Parameter("TIME")->asGrid() != NULL
					||	  pParameters->Get_Parameter("SDR" )->asGrid() != NULL;

		pParameters->Get_Parameter("FLOW_B")->Set_Enabled(bEnable);
		pParameters->Get_Parameter("FLOW_K")->Set_Enabled(bEnable);
		pParameters->Get_Parameter("FLOW_R")->Set_Enabled(bEnable);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Distance::On_Execute(void)
{
	CSG_Grid	*pChannels;

	//-----------------------------------------------------
	m_pDEM		= Parameters("ELEVATION")->asGrid();
	m_pRoute	= Parameters("ROUTE"    )->asGrid();
	pChannels	= Parameters("CHANNELS" )->asGrid();

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

	int	Method	= Parameters("METHOD"   )->asInt();

	//-----------------------------------------------------
	if( m_pDistance )	m_pDistance->Assign_NoData();
	if( m_pDistVert )	m_pDistVert->Assign_NoData();
	if( m_pDistHorz )	m_pDistHorz->Assign_NoData();
	if( m_pTime     )	m_pTime    ->Assign_NoData();
	if( m_pSDR      )	m_pSDR     ->Assign_NoData();

	switch( Method )
	{
	default:	Initialize_D8 ();	break;
	case  1:	Initialize_MFD();	break;
	}

	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;

		if( m_pDEM->Get_Sorted(n, x, y, false, true) && !(pChannels->is_NoData(x, y) && m_pDistance->is_NoData(x, y)) )
		{
			if( !pChannels->is_NoData(x, y) )
			{
				if( m_pDistance )	m_pDistance->Set_Value(x, y, 0.0);
				if( m_pDistVert )	m_pDistVert->Set_Value(x, y, 0.0);
				if( m_pDistHorz )	m_pDistHorz->Set_Value(x, y, 0.0);
				if( m_pTime     )	m_pTime    ->Set_Value(x, y, 0.0);
				if( m_pSDR      )	m_pSDR     ->Set_Value(x, y, 0.0);
				if( m_pFields   )	m_pPasses  ->Set_Value(x, y, 0.0);
			}

			switch( Method )
			{
			default:	Execute_D8 (x, y);	break;
			case  1:	Execute_MFD(x, y);	break;
			}
		}
	}

	//-----------------------------------------------------
	m_Dir.Destroy();

	for(int i=0; i<=8; i++)
	{
		m_Flow[i].Destroy();
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
inline double CChannelNetwork_Distance::Get_Travel_Time(int x, int y, int i)
{
	double	dz	= m_pDEM->asDouble(Get_xTo(i, x), Get_yTo(i, y)) - m_pDEM->asDouble(x, y);
	double	dx	= Get_Length(i);
	double	k	= m_pFlow_K && !m_pFlow_K->is_NoData(x, y) ? m_pFlow_K->asDouble(x, y) : m_Flow_K;
	double	R	= m_pFlow_R && !m_pFlow_R->is_NoData(x, y) ? m_pFlow_R->asDouble(x, y) : m_Flow_R;

	double	v	= k * pow(R, 2.0 / 3.0) * sqrt(dz / dx);	// [m / s], simplified Manning equation

	return( dx / (v * 3600.0) );	// return travel time in hours
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_D8(void)
{
	m_Dir.Create(*Get_System(), SG_DATATYPE_Char);

	if( (m_pFields = Parameters("FIELDS")->asGrid()) != NULL )
	{
		m_pPasses	= Parameters("PASSES")->asGrid();
		m_pPasses	->Set_NoData_Value(-1.0);
		m_pPasses	->Assign_NoData();
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Initialize_D8(x, y);
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_D8(int x, int y)
{
	int		i, iMax, iRoute;
	double	z, dz, dzMax, dzRoute;

	for(i=0, iMax=-1, dzMax=0.0, iRoute=-1, dzRoute=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( is_InGrid(ix, iy) && (dz = (z - m_pDEM->asDouble(ix, iy)) / Get_Length(i)) > 0.0 )
		{
			if( dz > dzMax )
			{
				iMax	= i;
				dzMax	= dz;
			}

			if( m_pRoute && !m_pRoute->is_NoData(ix, iy) && dz > dzRoute )
			{
				iRoute	= i;
				dzRoute	= dz;
			}
		}
	}

	m_Dir.Set_Value(x, y, iRoute >= 0 ? iRoute : iMax);
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_D8(int x, int y)
{
	int		nPasses	= m_pFields ? m_pPasses->asInt   (x, y) : 0;
	double	Field	= m_pFields ? m_pFields->asDouble(x, y) : 0;

	double	sz	= m_pDistVert->asDouble(x, y);
	double	sx	= m_pDistHorz->asDouble(x, y);
	double	sd	= m_pDistance->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xFrom(i, x);
		int	iy	= Get_yFrom(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && m_Dir.asInt(ix, iy) == i )
		{
			double	dz	= m_pDEM->asDouble(ix, iy) - m_pDEM->asDouble(x, y);
			double	dx	= Get_Length(i);

			if( m_pDistVert )	m_pDistVert->Set_Value(ix, iy, sz + dz);
			if( m_pDistHorz )	m_pDistHorz->Set_Value(ix, iy, sx + dx);
			if( m_pDistance )	m_pDistance->Set_Value(ix, iy, sd + sqrt(dz*dz + dx*dx));
			if( m_pTime     )	m_pTime    ->Set_Value(ix, iy, m_pTime->asDouble(x, y) + Get_Travel_Time(x, y, i));
			if( m_pSDR      )	m_pSDR     ->Set_Value(ix, iy, m_pSDR ->asDouble(x, y) + Get_Travel_Time(x, y, i));
			if( m_pFields   )	m_pPasses  ->Set_Value(ix, iy, Field != m_pFields->asDouble(ix, iy) ? nPasses + 1 : nPasses);
		}
	}

	if( m_pSDR )
	{
		m_pSDR->Set_Value(x, y, exp(-m_Flow_B * m_pSDR->asDouble(x, y)));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_MFD(void)
{
	m_pFields	= NULL;
	m_pPasses	= NULL;

	for(int i=0; i<=8; i++)
	{
		m_Flow[i].Create(*Get_System(), SG_DATATYPE_Float);
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Initialize_MFD(x, y);
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Initialize_MFD(int x, int y)
{
	const double	MFD_Convergence	= 1.1;

	double	Flow[8], dz, zSum = 0.0, z = m_pDEM->asDouble(x, y);

	if( m_pRoute )
	{
		for(int i=0, ix, iy; i<8; i++)
		{
			if( m_pDEM->is_InGrid(ix=Get_xTo(i, x), iy=Get_yTo(i, y)) && !m_pRoute->is_NoData(ix, iy) && (dz = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				zSum	+= (Flow[i]	= pow(dz / Get_Length(i), MFD_Convergence));
			}
			else
			{
				Flow[i]	= 0.0;
			}
		}
	}

	if( zSum == 0.0 )
	{
		for(int i=0, ix, iy; i<8; i++)
		{
			if( m_pDEM->is_InGrid(ix=Get_xTo(i, x), iy=Get_yTo(i, y)) && (dz = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				zSum	+= (Flow[i]	= pow(dz / Get_Length(i), MFD_Convergence));
			}
			else
			{
				Flow[i]	= 0.0;
			}
		}
	}

	if( zSum > 0.0 )
	{
		m_Flow[8].Set_Value(x, y, zSum);

		for(int i=0; i<8; i++)
		{
			if( Flow[i] > 0.0 )
			{
				m_Flow[i].Set_Value(x, y, Flow[i] / zSum);
			}
		}
	}
}

//---------------------------------------------------------
void CChannelNetwork_Distance::Execute_MFD(int x, int y)
{
	double	df	= m_Flow[8].asDouble(x, y);

	if( df > 0.0 )
	{
		if( m_pDistance )	m_pDistance->Mul_Value(x, y, 1.0 / df);
		if( m_pDistVert )	m_pDistVert->Mul_Value(x, y, 1.0 / df);
		if( m_pDistHorz )	m_pDistHorz->Mul_Value(x, y, 1.0 / df);
		if( m_pTime     )	m_pTime    ->Mul_Value(x, y, 1.0 / df);
		if( m_pSDR      )	m_pSDR     ->Mul_Value(x, y, 1.0 / df);
	}

	double	sz	= m_pDistVert->asDouble(x, y);
	double	sx	= m_pDistHorz->asDouble(x, y);
	double	sd	= m_pDistance->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int	ix	= Get_xTo(i, x);
		int	iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) && (df = m_Flow[(i + 4) % 8].asDouble(ix, iy)) > 0.0 )
		{
			double	dz	= m_pDEM->asDouble(ix, iy) - m_pDEM->asDouble(x, y);
			double	dx	= Get_Length(i);
			double	dt	= m_pTime || m_pSDR ? Get_Travel_Time(x, y, i) : 1.0;

			if( m_pDistance->is_NoData(ix, iy) )
			{
				m_Flow[8].Set_Value(ix, iy, df);

				if( m_pDistVert )	m_pDistVert->Set_Value(ix, iy, df * (sz + dz));
				if( m_pDistHorz )	m_pDistHorz->Set_Value(ix, iy, df * (sx + dx));
				if( m_pDistance )	m_pDistance->Set_Value(ix, iy, df * (sd + sqrt(dz*dz + dx*dx)));
				if( m_pTime     )	m_pTime    ->Set_Value(ix, iy, df * (m_pTime->asDouble(x, y) + dt));
				if( m_pSDR      )	m_pSDR     ->Set_Value(ix, iy, df * (m_pSDR ->asDouble(x, y) + dt));
			}
			else
			{
				m_Flow[8].Add_Value(ix, iy, df);

				if( m_pDistVert )	m_pDistVert->Add_Value(ix, iy, df * (sz + dz));
				if( m_pDistHorz )	m_pDistHorz->Add_Value(ix, iy, df * (sx + dx));
				if( m_pDistance )	m_pDistance->Add_Value(ix, iy, df * (sd + sqrt(dz*dz + dx*dx)));
				if( m_pTime     )	m_pTime    ->Add_Value(ix, iy, df * (m_pTime->asDouble(x, y) + dt));
				if( m_pSDR      )	m_pSDR     ->Add_Value(ix, iy, df * (m_pSDR ->asDouble(x, y) + dt));
			}
		}
	}

	if( m_pSDR )
	{
		m_pSDR->Set_Value(x, y, exp(-m_Flow_B * m_pSDR->asDouble(x, y)));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
