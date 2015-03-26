/**********************************************************
 * Version $Id: diffuse_pollution_risk.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               diffuse_pollution_risk.cpp              //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                     Cosima Berger                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Cosima Berger                          //
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
#include "diffuse_pollution_risk.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDiffuse_Pollution_Risk::CDiffuse_Pollution_Risk(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Diffuse Pollution Risk"));

	Set_Author		("Cosima Berger, Olaf Conrad (c) 2014");

	Set_Description	(_TW(
		"Diffuse Pollution Risk Mapping.\n"
		"This tool tries to reproduce in parts the methodology of the "
		"<a target=\"_blank\" href=\"http://www.scimap.org.uk/\">SCIMAP - Diffuse Pollution Risk Mapping - Framework</a>.\n"
		"\nReferences:\n"
		"Lane, S.N.; Brookes, C.J.; Kirkby, M.J.; Holden, J. (2004): "
		"A network-index-based version of TOPMODEL for use with high-resolution digital topographic data. "
		"In: Hydrological processes. Vol. 18, S. 191-201.\n"
		"\n"
		"Milledge, D.G.; Lane, N.S.; Heathwait, A.L.; Reaney, S.M. (2012): "
		"A monte carlo approach to the invers problem of diffuse pollution risk in agricultural catchments. "
		"In: Science of the Total Environment. Vol. 433, S. 434-449.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"				, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHANNEL"			, _TL("Channel Network"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_or_Const(
		NULL	, "WEIGHT"			, _TL("Land Cover Weights"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Grid_or_Const(
		NULL	, "RAIN"			, _TL("Rainfall"),
		_TL(""),
		500.0, 0.0, true
	);

	Parameters.Add_Grid(
		NULL	, "DELIVERY"		, _TL("Delivery Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "RISK_POINT"		, _TL("Locational Risk"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "RISK_DIFFUSE"	, _TL("Diffuse Pollution Risk"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Flow Direction Algorithm"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("single"),
			_TL("multiple")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "CHANNEL_START"	, _TL("Channel Initiation Threshold"),
		_TL("minimum number of upslope contributing cells to start a channel"),
		PARAMETER_TYPE_Int, 150, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDiffuse_Pollution_Risk::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "CHANNEL") )
	{
		pParameters->Set_Enabled("CHANNEL_START", pParameter->asGrid() == NULL);
	}

	return( CSG_Module_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiffuse_Pollution_Risk::On_Execute(void)
{	
	//-----------------------------------------------------
	m_pDEM			= Parameters("DEM"         )->asGrid();
	m_pDelivery		= Parameters("DELIVERY"    )->asGrid();
	m_pRisk_Point	= Parameters("RISK_POINT"  )->asGrid();
	m_pRisk_Diffuse	= Parameters("RISK_DIFFUSE")->asGrid();
	m_bSingle		= Parameters("METHOD"      )->asInt() == 0;

	DataObject_Set_Colors(m_pDelivery    , 11, SG_COLORS_RED_GREY_GREEN, true);
	DataObject_Set_Colors(m_pRisk_Point  , 11, SG_COLORS_RED_GREY_GREEN, true);
	DataObject_Set_Colors(m_pRisk_Diffuse, 11, SG_COLORS_RED_GREY_GREEN, true);

	//-----------------------------------------------------
	bool	bResult	= false;

	if( !Set_Flow() )
	{
		Error_Set(_TL("initialization failed"));
	}
	else if( !Set_Delivery_Index() )
	{
		Error_Set(_TL("delivery index calculation failed"));
	}
	else if( !Get_Risk_Diffuse() )
	{
		Error_Set(_TL("diffuse pollution risk calculation failed"));
	}
	else
	{
		bResult	= true;
	}

	//-----------------------------------------------------
	m_FlowDir.Destroy();
	m_RainAcc.Destroy();
	m_TWI    .Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiffuse_Pollution_Risk::Get_Flow_Proportions(int x, int y, double Proportion[8])
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		double Sum = 0.0;

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && m_pDEM->asDouble(x, y) > m_pDEM->asDouble(ix, iy) )
			{
				Sum	+= (Proportion[i]	= ((m_pDEM->asDouble(x, y) - m_pDEM->asDouble(ix, iy)) / Get_Length(i)));
			}
			else
			{
				Proportion[i]	= 0.0;
			}
		}

		if( Sum > 0.0 )
		{
			for(int i=0; i<8; i++)
			{
				if( Proportion[i] > 0.0 )
				{
					Proportion[i]	/= Sum;
				}
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiffuse_Pollution_Risk::Set_Flow(void)
{
	Process_Set_Text(_TL("initialization"));

	CSG_Grid	*pWeight	= Parameters("WEIGHT")->asGrid  ();
	double		  Weight	= Parameters("WEIGHT")->asDouble();

	CSG_Grid	*pRain		= Parameters("RAIN"  )->asGrid  ();
	double		  Rain		= Parameters("RAIN"  )->asDouble();

	m_FlowDir.Create(*Get_System(), SG_DATATYPE_Char);
	m_RainAcc.Create(*Get_System());
	m_TWI    .Create(*Get_System());

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;

		if( !m_pDEM->Get_Sorted(n, x, y, true) || (pRain && pRain->is_NoData(x, y)) || !Set_Flow(x, y, pRain ? pRain->asDouble(x, y) : Rain) )
		{
			m_FlowDir     .Set_NoData(x, y);
			m_RainAcc     .Set_NoData(x, y);
			m_TWI         .Set_NoData(x, y);
			m_pRisk_Point->Set_NoData(x, y);
		}
		else
		{
			double	s, a;

			m_pDEM->Get_Gradient(x, y, s, a);
			
			s	= tan(s);											// tangens of slope
			a	= (fabs(sin(a)) + fabs(cos(a))) * Get_Cellsize();	// flow width

			double	SCA	= m_RainAcc.asDouble(x, y) / a;				// rain * specific catchment area

			m_TWI.Set_Value(x, y, log(SCA / (s < M_ALMOST_ZERO ? M_ALMOST_ZERO : s)));

			if( pWeight && pWeight->is_NoData(x, y) )
			{
				m_pRisk_Point->Set_NoData(x, y);
			}
			else
			{
				m_pRisk_Point->Set_Value(x, y, SCA * s * (pWeight ? pWeight->asDouble(x, y) : Weight));	// Point Scale Risk Calculation according to Milledge et al. 2012
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CDiffuse_Pollution_Risk::Set_Flow(int x, int y, double Rain)
{
	//-----------------------------------------------------
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	double	d[8];

	m_FlowDir.Set_Value(x, y, m_pDEM->Get_Gradient_NeighborDir(x, y));
	m_RainAcc.Set_Value(x, y, Rain = Rain * Get_Cellarea() + m_RainAcc.asDouble(x, y));

	//-----------------------------------------------------
	if( m_bSingle )
	{
		if( Get_System()->Get_Neighbor_Pos(m_FlowDir.asInt(x, y), x, y, x, y) && m_pDEM->is_InGrid(x, y) )
		{
			m_RainAcc.Add_Value(x, y, Rain);
		}
	}
	else if( Get_Flow_Proportions(x, y, d) )
	{
		for(int i=0; i<8; i++)
		{
			if( d[i] > 0.0 )
			{
				m_RainAcc.Add_Value(Get_xTo(i, x), Get_yTo(i, y), Rain * d[i]);
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
// Calculation according to Lane et al. 2004, p. 198.
//---------------------------------------------------------
bool CDiffuse_Pollution_Risk::Set_Delivery_Index(void)
{
	//-----------------------------------------------------
	CSG_Grid	Channel, *pChannel	= Parameters("CHANNEL")->asGrid();

	if( !pChannel )	// no channel network is provided, so create a tempory channel network out of the box!
	{
		Process_Set_Text(_TL("Channel Network"));

		int	Threshold	= Parameters("CHANNEL_START")->asInt();

		pChannel	= &Channel;	Channel.Create(*Get_System(), SG_DATATYPE_Word);	Channel.Assign(0.0);	Channel.Set_NoData_Value(0.0);

		for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
		{
			int		x, y, ix, iy;

			if( m_pDEM->Get_Sorted(n, x, y, true) )
			{
				if( Get_System()->Get_Neighbor_Pos(m_FlowDir.asInt(x, y), x, y, ix, iy) && m_FlowDir.is_InGrid(ix, iy) )
				{
					Channel.Add_Value(ix, iy, 1.0 + Channel.asDouble(x, y));
				}

				Channel.Set_Value(x, y, Channel.asInt(x, y) > Threshold ? 1.0 : 0.0);
			}
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Delivery Index"));

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y, ix, iy;

		double	TWI_min	= -1.0;

		if( m_pDEM->Get_Sorted(n, x, y, false) )	// bottom up
		{
			if( m_TWI.is_InGrid(x, y) )
			{
				TWI_min	= m_TWI.asDouble(x, y);
			}

			if( m_FlowDir.is_InGrid(x, y) && Get_System()->Get_Neighbor_Pos(m_FlowDir.asInt(x, y), x, y, ix, iy) && m_TWI.is_InGrid(ix, iy) )
			{
				if( TWI_min < 0.0 || TWI_min > m_TWI.asDouble(ix, iy) )
				{
					TWI_min	= m_TWI.asDouble(ix, iy);
				}
			}
		}

		if( TWI_min < 0.0 )
		{
			m_pDelivery->Set_NoData(x, y);
		}
		else
		{
			m_pDelivery->Set_Value(x, y, TWI_min);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Calculation according to Milledge et al. 2012
//---------------------------------------------------------
bool CDiffuse_Pollution_Risk::Get_Risk_Diffuse(void)
{
	Process_Set_Text(_TL("Difuse Pollution Risk"));

	m_pRisk_Diffuse->Assign(0.0);

	//-----------------------------------------------------
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int		x, y;

		if( !m_pDEM->Get_Sorted(n, x, y, true) || m_pDelivery->is_NoData(x, y) || m_pRisk_Point->is_NoData(x, y) || m_RainAcc.asDouble(x, y) <= 0.0 )
		{
			m_pRisk_Diffuse->Set_NoData(x, y);
		}
		else
		{
			double d[8], Risk;

			m_pRisk_Point->Mul_Value(x, y, m_pDelivery->asDouble(x, y));				// locational risk = generation risk * connection risk
			Risk	= m_pRisk_Diffuse->asDouble(x, y) + m_pRisk_Point->asDouble(x, y);	// risk load = sum of upslope locational risk
			m_pRisk_Diffuse->Set_Value(x, y, Risk / m_RainAcc.asDouble(x, y));			// risk concentration = risk load / sum of upslope rain

			if( m_bSingle )
			{
				int		i	= !m_FlowDir.is_NoData(x, y) ? m_FlowDir.asInt(x, y) : -1;

				if( i > 0 && m_pDEM->is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
				{
					m_pRisk_Diffuse->Add_Value(Get_xTo(i, x), Get_yTo(i, y), Risk);
				}
			}
			else if( Get_Flow_Proportions(x, y, d) )
			{
				for(int i=0; i<8; i++)
				{
					if( d[i] > 0.0 )
					{
						m_pRisk_Diffuse->Add_Value(Get_xTo(i, x), Get_yTo(i, y), Risk * d[i]);
					}
				}
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
