/**********************************************************
 * Version $Id: flow_by_slope.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
//                   flow_by_slope.cpp                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#include "flow_by_slope.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_by_Slope::CFlow_by_Slope(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Slope Limited Flow Accumulation"));

	Set_Author	("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Flow accumulation is calculated as upslope contributing (catchment) area "
		"using the multiple flow direction approach of Freeman (1991). For this tool "
		"the approach has been modified to limit the flow portion routed through a cell "
		"depending on the local slope. If a cell is not inclined, no flow is routed "
		"through it at all. With increasing slopes the portion of flow routed through "
		"a cell becomes higher. Cells with slopes greater than a specified slope threshold "
		"route their entire accumulated flow downhill. "		 

		"\nReferences:\n"
		"- Freeman, G.T. (1991):"
		" Calculating catchment area with divergent flow based on a regular grid."
		" Computers and Geosciences, 17:413-22\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "WEIGHT"		, _TL("Weight"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "FLOW"		, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "SLOPE_MIN"	, _TL("Slope Minimum"),
		_TL("Assume a given minimum slope for each cell."),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "SLOPE_MAX"	, _TL("Slope Threshold"),
		_TL("Slope threshold, given as degree, above which flow transport is unlimited."),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 90.0, true
	);

	CSG_Parameter	*pNode	= Parameters.Add_Value(
		NULL	, "B_FLOW"		, _TL("Use Flow Threshold"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Range(
		pNode	, "T_FLOW"		, _TL("Flow Threshold"),
		_TL("Flow threshold, given as amount of cells, above which flow transport is unlimited. Ignored if range equals zero."),
		1.0, 100.0, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_by_Slope::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "B_FLOW") )
	{
		pParameters->Get_Parameter("T_FLOW")->Set_Enabled(pParameter->asBool());
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_by_Slope::On_Execute(void)
{
	m_Slope_Min	= Parameters("SLOPE_MIN")->asDouble() * M_DEG_TO_RAD;
	m_Slope_Max = Parameters("SLOPE_MAX")->asDouble() * M_DEG_TO_RAD;

	if( m_Slope_Max <= 0.0 )
	{
		Error_Set(_TL("slope threshold must not be zero!"));

		return( false );
	}

	if( Parameters("B_FLOW")->asBool() )
	{
		m_Flow_Min	= Parameters("T_FLOW")->asRange()->Get_LoVal() * Get_Cellarea();
		m_Flow_Max	= Parameters("T_FLOW")->asRange()->Get_HiVal() * Get_Cellarea();
	}
	else
	{
		m_Flow_Min	= m_Flow_Max	= 0.0;
	}

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM"   )->asGrid();
	m_pFlow		= Parameters("FLOW"  )->asGrid();

	m_pFlow->Assign(Get_Cellarea());

	if( Parameters("WEIGHT")->asGrid() )
	{
		m_pFlow->Multiply(*Parameters("WEIGHT")->asGrid());
	}

	DataObject_Set_Colors(m_pFlow, 11, SG_COLORS_WHITE_BLUE, false);

	if( !m_pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	//-----------------------------------------------------
	for(sLong i=0; i<Get_NCells() && Set_Progress_NCells(i); i++)
	{
		int	x, y;

		if( !m_pDEM->Get_Sorted(i, x, y, true) || m_pDEM->is_NoData(x, y) )
		{
			m_pFlow->Set_NoData(x, y);
		}
		else
		{
			Set_Area(x, y);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CFlow_by_Slope::Get_Fuzzy(double Value, double Minimum, double Maximum)
{
	return( Value <= Minimum ? 0.0 : Value >= Maximum ? 1.0 :
		0.5 * (1.0 - cos(M_PI * ((Value - Minimum) / (Maximum - Minimum))))
	);
}

//---------------------------------------------------------
inline bool CFlow_by_Slope::Get_Decay(int x, int y, double &Decay)
{
	double	d;

	if( !m_pDEM->Get_Gradient(x, y, d, Decay) )
	{
		return( false );
	}

	if( d < m_Slope_Min )
	{
		d	= m_Slope_Min;
	}

	Decay	= Get_Fuzzy(d, 0.0, m_Slope_Max);

	if( m_Flow_Min > 0.0 )
	{
		d	= Get_Fuzzy(m_pFlow->asDouble(x, y), m_Flow_Min, m_Flow_Max);

		switch( 1 )
		{
		default:
			Decay	= M_GET_MAX(Decay, d);
			break;

		case 1:
			Decay	= Decay + d - Decay * d;
			break;
		}
	}

	return( Decay > 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_by_Slope::Set_Area(int x, int y)
{
	double	Decay;

	if( Get_Decay(x, y, Decay) )
	{
		int		i, ix, iy;
		double	z, dzSum, dz[8];

		for(i=0, dzSum=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
		{
			if( m_pDEM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) && (dz[i] = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
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
			Decay	*= m_pFlow->asDouble(x, y) / dzSum;

			for(i=0; i<8; i++)
			{
				if( dz[i] > 0.0 )
				{
					m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), Decay * dz[i]);
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
