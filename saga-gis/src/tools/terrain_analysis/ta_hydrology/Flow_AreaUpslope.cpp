/**********************************************************
 * Version $Id: Flow_AreaUpslope.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
#include "Flow_AreaUpslope.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_AreaUpslope::CFlow_AreaUpslope(void)
{
	m_pDTM		= NULL;
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CFlow_AreaUpslope::Get_Description(void)
{
	return(
		_TW(
			"This tool allows you to specify target cells, "
			"for which the upslope contributing area shall be identified. "
			"The result will give "
			"for each cell the percentage of its flow that reaches the target cell(s).\n\n"

			"References:\n\n"

			"Deterministic 8\n"
			"- O'Callaghan, J.F. / Mark, D.M. (1984):\n"
			"    'The extraction of drainage networks from digital elevation data',\n"
			"    Computer Vision, Graphics and Image Processing, 28:323-344\n\n"

			"Deterministic Infinity:\n"
			"- Tarboton, D.G. (1997):\n"
			"    'A new method for the determination of flow directions and upslope areas in grid digital elevation models',\n"
			"    Water Resources Research, Vol.33, No.2, p.309-319\n\n"

			"Multiple Flow Direction:\n"
			"- Freeman, G.T. (1991):\n"
			"    'Calculating catchment area with divergent flow based on a regular grid',\n"
			"    Computers and Geosciences, 17:413-22\n\n"

			"- Quinn, P.F. / Beven, K.J. / Chevallier, P. / Planchon, O. (1991):\n"
			"    'The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models',\n"
			"    Hydrological Processes, 5:59-79\n\n"
		)
	);
}

//---------------------------------------------------------
CSG_String CFlow_AreaUpslope::Get_Methods(void)
{
	return( CSG_String::Format(SG_T("%s|%s|%s|"),
		_TL("Deterministic 8"),
		_TL("Deterministic Infinity"),
		_TL("Multiple Flow Direction")
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_AreaUpslope::Initialise(int Method, CSG_Grid *pDTM, CSG_Grid *pRoute, CSG_Grid *pFlow, double MFD_Converge)
{
	Finalise();

	if( pDTM && pDTM->is_Valid() && pFlow && pFlow->is_Valid() && pFlow->Get_System() == pDTM->Get_System() )
	{
		m_Method		= Method;
		m_pDTM			= pDTM;
		m_pFlow			= pFlow;
		m_MFD_Converge	= MFD_Converge;

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
	m_pDTM		= NULL;
	m_pRoute	= NULL;
	m_pFlow		= NULL;

	return( true );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Add_Target(int x, int y)
{
	if( m_pFlow && m_pFlow->is_InGrid(x, y, false) )
	{
		m_pFlow->Set_Value(x, y, 100.0);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFlow_AreaUpslope::Clr_Target(void)
{
	if( m_pFlow )
	{
		m_pFlow->Assign(0.0);

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
	if( !m_pDTM || !m_pFlow || !m_pDTM->Set_Index() )
	{
		return( false );
	}

	sLong	i;
	int		x, y;

	for(i=0; i<m_pDTM->Get_NCells() && SG_UI_Process_Set_Progress((double)i, (double)m_pDTM->Get_NCells()); i++)
	{
		if( m_pDTM->Get_Sorted(i, x, y, false) &&  m_pFlow->asDouble(x, y) > 0.0 )
		{
			break;
		}
	}

	for(i++; i<m_pDTM->Get_NCells() && SG_UI_Process_Set_Progress((double)i, (double)m_pDTM->Get_NCells()); i++)
	{
		if( m_pDTM->Get_Sorted(i, x, y, false) && m_pFlow->asDouble(x, y) <= 0.0 )
		{
			Set_Value(x, y);
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
void CFlow_AreaUpslope::Set_Value(int x, int y)
{
	int		i;

	if( m_pRoute && (i = m_pRoute->asChar(x, y)) >= 0 )
	{
		int		ix, iy;
		double	Flow;

		ix	= m_pDTM->Get_System().Get_xTo(i, x);
		iy	= m_pDTM->Get_System().Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy, true) && (Flow = m_pFlow->asDouble(ix, iy)) > 0.0 )
		{
			m_pFlow->Set_Value(x, y, Flow);
		}
	}
	else if( !m_pDTM->is_NoData(x, y) )
	{
		switch( m_Method )
		{
		case 0:	Set_D8		(x, y);	break;
		case 1:	Set_DInf	(x, y);	break;
		case 2:	Set_MFD		(x, y);	break;
		}
	}
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_D8(int x, int y)
{
	int		i;

	if( (i = m_pDTM->Get_Gradient_NeighborDir(x, y, true)) >= 0 )
	{
		int		ix, iy;
		double	Flow;

		ix	= m_pDTM->Get_System().Get_xTo(i, x);
		iy	= m_pDTM->Get_System().Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy, true) && (Flow = m_pFlow->asDouble(ix, iy)) > 0.0 )
		{
			m_pFlow->Set_Value(x, y, Flow);
		}
	}
}

//---------------------------------------------------------
void CFlow_AreaUpslope::Set_DInf(int x, int y)
{
	double	Slope, Aspect;

	if( m_pDTM->Get_Gradient(x, y, Slope, Aspect) )	// && Aspect >= 0.0 )
	{
		int		i, ix, iy, jx, jy;
		double	Flow;

		i		= (int)(Aspect / M_PI_045);
		ix		= m_pDTM->Get_System().Get_xTo(i, x);
		iy		= m_pDTM->Get_System().Get_yTo(i, y);

		i++;
		jx		= m_pDTM->Get_System().Get_xTo(i, x);
		jy		= m_pDTM->Get_System().Get_yTo(i, y);

		if(	m_pDTM->is_InGrid(ix, iy) && m_pDTM->asDouble(ix, iy) < m_pDTM->asDouble(x, y)
		&&	m_pDTM->is_InGrid(jx, jy) && m_pDTM->asDouble(jx, jy) < m_pDTM->asDouble(x, y) )
		{
			Aspect	= fmod(Aspect,  M_PI_045) / M_PI_045;

			Flow	= m_pFlow->asDouble(ix, iy) * (1.0 - Aspect)
					+ m_pFlow->asDouble(jx, jy) * (      Aspect);

			if( Flow > 0.0 )
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
	int		i, ix, iy;
	double	z, d, f, dzSum, dz[8];

	for(i=0, dzSum=0.0, z=m_pDTM->asDouble(x, y); i<8; i++)
	{
		dz[i]	= 0.0;

		ix		= m_pDTM->Get_System().Get_xTo(i, x);
		iy		= m_pDTM->Get_System().Get_yTo(i, y);

		if( m_pDTM->is_InGrid(ix, iy) && (d = z - m_pDTM->asDouble(ix, iy)) > 0.0 )
		{
			dzSum	+= (d = pow(d / m_pDTM->Get_System().Get_Length(i), m_MFD_Converge));

			if( (f = m_pFlow->asDouble(ix, iy)) > 0.0 )
			{
				dz[i]	= d * f;
			}
		}
	}

	if( dzSum > 0.0 )
	{
		for(i=0, d=0.0; i<8; i++)
		{
			if( dz[i] > 0.0 )
			{
				d	+= dz[i] / dzSum;
			}
		}

		m_pFlow->Set_Value(x, y, d);
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
	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(CSG_String::Format(SG_T("%s_______\n%s"), m_Calculator.Get_Description().c_str(),
		_TL("Interactive version (left mouse clicks will trigger the calculation for the selected cell).")
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Sink Routes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "AREA"		, _TL("Upslope Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		m_Calculator.Get_Methods(), 2
	);

	Parameters.Add_Value(
		NULL	, "CONVERGE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction algorithm"),
		PARAMETER_TYPE_Double	, 1.1, 0.001, true
	);
}

//---------------------------------------------------------
CFlow_AreaUpslope_Interactive::~CFlow_AreaUpslope_Interactive(void)
{}

//---------------------------------------------------------
bool CFlow_AreaUpslope_Interactive::On_Execute(void)
{
	if( m_Calculator.Initialise(
		Parameters("METHOD")	->asInt(),
		Parameters("ELEVATION")	->asGrid(),
		Parameters("SINKROUTE")	->asGrid(),
		Parameters("AREA")		->asGrid(),
		Parameters("CONVERGE")	->asDouble()	) )
	{
		DataObject_Set_Colors(Parameters("AREA")->asGrid(), 100, SG_COLORS_WHITE_BLUE);

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
	if(	Mode == TOOL_INTERACTIVE_LDOWN && m_Calculator.Get_Area(Get_xGrid(), Get_yGrid()) )
	{
		DataObject_Update(Parameters("AREA")->asGrid(), 0.0, 100.0, true);

		return( true );
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
	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(CSG_String::Format(SG_T("%s_______\n\n%s"), m_Calculator.Get_Description().c_str(),
		_TW("This version uses all valid cells (not \'no data\' values) of a given target grid to determine the contributing area. "
			"In case no target grid is provided as input, the specified x/y coordinates are used as target point.")
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "TARGET"		, _TL("Target Area"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "TARGET_PT_X"	, _TL("Target X coordinate"),
		_TL("The x-coordinate of the target point in world coordinates [map units]"),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		NULL	, "TARGET_PT_Y"	, _TL("Target Y coordinate"),
		_TL("The y-coordinate of the target point in world coordinates [map units]"),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Sink Routes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "AREA"		, _TL("Upslope Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		m_Calculator.Get_Methods(), 2
	);

	Parameters.Add_Value(
		NULL	, "CONVERGE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction algorithm"),
		PARAMETER_TYPE_Double	, 1.1, 0.001, true
	);
}

//---------------------------------------------------------
CFlow_AreaUpslope_Area::~CFlow_AreaUpslope_Area(void)
{}

//---------------------------------------------------------
bool CFlow_AreaUpslope_Area::On_Execute(void)
{
	bool	bResult	= false;

	//-----------------------------------------------------
	if( m_Calculator.Initialise(
		Parameters("METHOD")	->asInt(),
		Parameters("ELEVATION")	->asGrid(),
		Parameters("SINKROUTE")	->asGrid(),
		Parameters("AREA")		->asGrid(),
		Parameters("CONVERGE")	->asDouble()	) )
	{
		if( m_Calculator.Clr_Target() )
		{
			int		x, y;
			CSG_Grid	*pTarget	= Parameters("TARGET")->asGrid();

			if( pTarget != NULL )
			{
				for(y=0; y<Get_NY() && Set_Progress(y); y++)
				{
					for(x=0; x<Get_NX(); x++)
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
				Parameters("ELEVATION")->asGrid()->Get_System().Get_World_to_Grid(x, y, Parameters("TARGET_PT_X")->asDouble(), Parameters("TARGET_PT_Y")->asDouble());

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

				DataObject_Set_Colors(Parameters("AREA")->asGrid(), 100, SG_COLORS_WHITE_BLUE);
			}
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
