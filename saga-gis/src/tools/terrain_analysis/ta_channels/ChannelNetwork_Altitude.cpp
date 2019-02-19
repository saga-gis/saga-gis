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
//              ChannelNetwork_Altitude.cpp              //
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
#include "ChannelNetwork_Altitude.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChannelNetwork_Altitude::CChannelNetwork_Altitude(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Vertical Distance to Channel Network"));

	Set_Author		("O.Conrad (c) 2002");

	Set_Description	(_TW(
		"This tool calculates the vertical distance to a channel network base level. "
		"The algorithm consists of two major steps:\n"
		" 1. Interpolation of a channel network base level elevation\n"
		" 2. Subtraction of this base level from the original elevations\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "CHANNELS"	, _TL("Channel Network"),
		_TL("A grid providing information about the channel network. It is assumed that no-data cells are not part of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DISTANCE"	, _TL("Vertical Distance to Channel Network"),
		_TL("The resulting grid gives the altitude above the channel network in the same units as the elevation data."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "BASELEVEL"	, _TL("Channel Network Base Level"),
		_TL("This optional grid output contains the interpolated channel network base level elevations."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "THRESHOLD"	, _TL("Tension Threshold"),
		_TL("Maximum change in elevation units (e.g. meter), iteration is stopped once maximum change reaches this threshold."),
		1.0, 0.0, true
	);

	Parameters.Add_Int(
		"", "MAXITER"	, _TL("Maximum Iterations"),
		_TL("Maximum number of iterations, ignored if set to zero"),
		0, 0, true
	);

	Parameters.Add_Bool(
		"", "NOUNDERGROUND", _TL("Keep Base Level below Surface"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Altitude::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDTM				= Parameters("ELEVATION"    )->asGrid();
	m_pChannels			= Parameters("CHANNELS"     )->asGrid();
	m_pDistance			= Parameters("DISTANCE"     )->asGrid();
	m_bNoUnderground	= Parameters("NOUNDERGROUND")->asBool();
	double	Threshold	= Parameters("THRESHOLD"    )->asDouble();
	int		maxIter		= Parameters("MAXITER"      )->asInt();

	//-----------------------------------------------------
	CSG_Colors	Colors(3);

	Colors.Set_Color(0, SG_GET_RGB(  0, 128,   0));
	Colors.Set_Color(1, SG_GET_RGB(255, 255,   0));
	Colors.Set_Color(2, SG_GET_RGB(128,  64,  64));

	DataObject_Set_Colors(m_pDistance, Colors);

	m_pDistance->Assign_NoData();

	m_Mask.Create(Get_System(), SG_DATATYPE_Byte);
	m_Dist.Create(Get_System());
//	m_Dist.Set_NoData_Value_Range(m_pDTM->Get_NoData_Value(), m_pDTM->Get_NoData_hiValue());
	m_Dist.Assign_NoData();

	//-----------------------------------------------------
	int	nSteps, nCells	= Get_NX() > Get_NY() ? Get_NX() : Get_NY();
	for(nSteps=0; pow(2.0, nSteps + 1)<nCells; nSteps++);
	nCells	= (int)pow(2.0, nSteps);

	//-----------------------------------------------------
	for(int iStep=1; nCells>0 && Process_Get_Okay(); nCells/=2, iStep++)
	{
		Process_Set_Text("%d [%d]", iStep, nSteps + 1);

		Set_Surface(nCells);

		int	Iteration	= 0;	double dMax;

		while( Threshold < (dMax = Get_Change(nCells)) && (maxIter < 1 || Iteration < maxIter) && Process_Get_Okay() )
		{
			Iteration++;
		}

		Message_Fmt("\n%s: %d; %s: %d; %s: %f",
			_TL("Level"         ), 2 + nSteps - iStep,
			_TL("Iterations"    ), 1 + Iteration,
			_TL("Maximum change"), dMax
		);
	}

	m_Mask.Destroy();
	m_Dist.Destroy();

	//-----------------------------------------------------
	CSG_Grid	*pBase	= Parameters("BASELEVEL")->asGrid();

	DataObject_Set_Colors(pBase, Colors);

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		if( m_pDistance->is_NoData(i) || m_pDTM->is_NoData(i) )
		{
			m_pDistance->Set_NoData(i);

			if( pBase )	{	pBase->Set_NoData(i);	}
		}
		else
		{
			double	z	= m_pDistance->asDouble(i);

			m_pDistance->Set_Value(i, m_pDTM->asDouble(i) - z);

			if( pBase )	{	pBase->Set_Value(i, z);	}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CChannelNetwork_Altitude::Set_Surface(int nCells)
{
	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y+=nCells)
	{
		int	ny	= y + nCells < Get_NY() ? y + nCells : Get_NY();

		for(int x=0; x<Get_NX(); x+=nCells)
		{
			int		nx	= x + nCells < Get_NX() ? x + nCells : Get_NX();

			double	z	= 0.0;	int	nz	= 0;

			{
				for(int iy=y; iy<ny; iy++) for(int ix=x; ix<nx; ix++)
				{
					if( m_pChannels->is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
					{
						z	+= m_pDTM->asDouble(ix, iy);
						nz	++;
					}
				}
			}

			//---------------------------------------------
			if( nz > 0 )
			{
				m_Mask.Set_Value(x, y, 1.);

				m_Dist.Set_Value(x, y, z / nz);
			}
			else
			{
				m_Mask.Set_Value(x, y, 0.);

				if( m_pDistance->is_NoData(x, y) )
				{
					for(int i=0; i<8; i++)
					{
						int	ix	= x + nCells * Get_xTo(i);
						int	iy	= y + nCells * Get_yTo(i);

						if( m_pDistance->is_InGrid(ix, iy) )
						{
							z	+= m_pDistance->asDouble(ix, iy);
							nz	++;
						}
					}

					m_Dist.Set_Value(x, y, nz > 0 ? z / nz : m_pDTM->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		if( !m_Dist.is_NoData(i) && m_pDistance->is_NoData(i) )
		{
			m_pDistance->Set_Value(i, m_Dist.asDouble(i));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CChannelNetwork_Altitude::Get_Change(int nCells)
{
	int		y;

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y+=nCells)
	{
		for(int x=0; x<Get_NX(); x+=nCells)
		{
			if( !m_Mask.asByte(x, y) )
			{
				m_Dist.Set_Value(x, y, Get_Change(nCells, x, y));
			}
		}
	}

	//-----------------------------------------------------
	double	dMax	= 0.0;

	for(y=0; y<Get_NY(); y+=nCells)
	{
		for(int x=0; x<Get_NX(); x+=nCells)
		{
			if( !m_Mask.asByte(x, y) )
			{
				double	d	= fabs(m_Dist.asDouble(x, y) - m_pDistance->asDouble(x, y));

				if( dMax < d )
				{
					dMax	= d;
				}

				m_pDistance->Set_Value(x, y, m_Dist.asDouble(x, y));
			}
		}
	}

	return( dMax );
}


//---------------------------------------------------------
double CChannelNetwork_Altitude::Get_Change(int nCells, int x, int y)
{
	double	d = 0.0, n = 0.0;

	for(int i=0; i<8; i++)
	{
		int	ix	= x + nCells * Get_xTo(i);
		int	iy	= y + nCells * Get_yTo(i);

		if( m_pDistance->is_InGrid(ix, iy) )
		{
			double	dz	= 1.0 / Get_UnitLength(i);

			d	+= dz * m_pDistance->asDouble(ix, iy);
			n	+= dz;
		}
	}

	if( n > 0.0 )
	{
		d	/= n;

		return( m_bNoUnderground && !m_pDTM->is_NoData(x, y) && d > m_pDTM->asDouble(x, y) ? m_pDTM->asDouble(x, y) : d );
	}

	return( m_pDistance->asDouble(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CValley_Depth::CValley_Depth(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Valley Depth"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"Valley depth is calculated as difference between the elevation and an interpolated ridge level. "
		"Ridge level interpolation uses the algorithm implemented in the 'Vertical Distance to Channel Network' tool. "
		"It performs the following steps:\n"
		" - Definition of ridge cells (using Strahler order on the inverted DEM).\n"
		" - Interpolation of the ridge level.\n"
		" - Subtraction of the original elevations from the ridge level.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ELEVATION"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "VALLEY_DEPTH"	, _TL("Valley Depth"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "RIDGE_LEVEL"	, _TL("Ridge Level"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(
		"", "THRESHOLD"		, _TL("Tension Threshold"),
		_TL("Maximum change in elevation units (e.g. meter), iteration is stopped once maximum change reaches this threshold."),
		1.0, 0.0, true
	);

	Parameters.Add_Int(
		"", "MAXITER"		, _TL("Maximum Iterations"),
		_TL("Maximum number of iterations, ignored if set to zero"),
		0, 0, true
	);

	Parameters.Add_Bool(
		"", "NOUNDERGROUND"	, _TL("Keep Ridge Level above Surface"),
		_TL(""),
		true
	);

	Parameters.Add_Int(
		"", "ORDER"			, _TL("Ridge Detection Threshold"),
		_TL(""),
		4, 1, true, 7, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CValley_Depth::On_Execute(void)
{
	CSG_Grid	*pDepth	= Parameters("VALLEY_DEPTH")->asGrid();

	//-----------------------------------------------------
	CSG_Grid	Inverse(*Parameters("ELEVATION")->asGrid());
	CSG_Grid	Ridges (Get_System(), SG_DATATYPE_Int);

	Inverse.Invert();

	SG_RUN_TOOL_ExitOnError("ta_channels"       , 6,	// strahler order
		   SG_TOOL_PARAMETER_SET("DEM"          , &Inverse)
		&& SG_TOOL_PARAMETER_SET("STRAHLER"     , &Ridges )
	)

	Ridges.Set_NoData_Value_Range(0, Parameters("ORDER")->asInt());

	SG_RUN_TOOL_ExitOnError("ta_channels"       , 3,	// vertical channel network distance
		   SG_TOOL_PARAMETER_SET("ELEVATION"    , &Inverse)
		&& SG_TOOL_PARAMETER_SET("CHANNELS"     , &Ridges )
		&& SG_TOOL_PARAMETER_SET("DISTANCE"     , pDepth  )
		&& SG_TOOL_PARAMETER_SET("THRESHOLD"    , Parameters("THRESHOLD"    ))
		&& SG_TOOL_PARAMETER_SET("MAXITER"      , Parameters("MAXITER"      ))
		&& SG_TOOL_PARAMETER_SET("NOUNDERGROUND", Parameters("NOUNDERGROUND"))
	)

	//-----------------------------------------------------
	CSG_Grid	*pRidge	= Parameters("RIDGE_LEVEL")->asGrid();

	if( pRidge )
	{
		pRidge->Assign(Parameters("ELEVATION")->asGrid());
		pRidge->Add(*pDepth);
	}

	pDepth->Set_Name(_TL("Valley Depth"));

	DataObject_Set_Colors(pDepth, 11, SG_COLORS_RAINBOW, false);
	DataObject_Set_Colors(pDepth, 11, SG_COLORS_RAINBOW, false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
