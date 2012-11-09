/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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

	Set_Author		(SG_T("O.Conrad (c) 2002"));

	Set_Description	(_TW(
		"This module calculates the vertical distance to a channel network base level. "
		"The algorithm consists of two major steps:\n"
		" 1. Interpolation of a channel network base level elevation\n"
		" 2. Subtraction of this base level from the original elevations\n"
	));


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL, "ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "CHANNELS"	, _TL("Channel Network"),
		_TW("A grid providing information about the channel network. It is assumed that no-data cells are not part "
		"of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL, "DISTANCE"	, _TL("Vertical Distance to Channel Network"),
		_TL("The resulting grid gives the altitude above the channel network in the same units as the elevation data."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "BASELEVEL"	, _TL("Channel Network Base Level"),
		_TL("This optional grid output contains the interpolated channel network base level elevations."),
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL, "THRESHOLD"	, _TL("Tension Threshold [Percentage of Cell Size]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "NOUNDERGROUND", _TL("Keep Base Level below Surface"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Altitude::On_Execute(void)
{
	int			nCells, nCells_Start, iStep, nSteps;
	double		Threshold;

	//-----------------------------------------------------
	m_pDTM				= Parameters("ELEVATION")		->asGrid();
	m_pChannels			= Parameters("CHANNELS")		->asGrid();
	m_pDistance			= Parameters("DISTANCE")		->asGrid();
	m_bNoUnderground	= Parameters("NOUNDERGROUND")	->asBool();
	Threshold			= Parameters("THRESHOLD")		->asDouble() * Get_Cellsize();

	DataObject_Set_Colors(m_pDistance, 10, SG_COLORS_YELLOW_BLUE, true);

	//-----------------------------------------------------
	nCells			= Get_NX() > Get_NY() ? Get_NX() : Get_NY();
	for(nSteps=0; pow(2.0, nSteps + 1) < nCells; nSteps++);
	nCells_Start	= (int)pow(2.0, nSteps);

	//-----------------------------------------------------
	m_pDistance->Assign_NoData();

	m_Mask.Create(*Get_System(), SG_DATATYPE_Byte);
	m_Dist.Create(*Get_System());
	m_Dist.Set_NoData_Value_Range(m_pDTM->Get_NoData_Value(), m_pDTM->Get_NoData_hiValue());

	for(nCells=nCells_Start, iStep=1; nCells>0 && Process_Get_Okay(); nCells/=2, iStep++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%d [%d]"), iStep, nSteps + 1));

		Set_Surface(nCells);

		while( Threshold < Get_Change(nCells) && Process_Get_Okay() );
	}

	m_Mask.Destroy();
	m_Dist.Destroy();

	//-----------------------------------------------------
	CSG_Grid	*pBase	= Parameters("BASELEVEL")->asGrid();

	for(long n=0; n<Get_NCells(); n++)
	{
		if( m_pDistance->is_NoData(n) || m_pDTM->is_NoData(n) )
		{
			m_pDistance->Set_NoData(n);

			if( pBase )	{	pBase->Set_NoData(n);	}
		}
		else
		{
			double	z	= m_pDistance->asDouble(n);

			m_pDistance->Set_Value(n, m_pDTM->asDouble(n) - z);

			if( pBase )	{	pBase->Set_Value(n, z);	}
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
	m_Dist.Assign_NoData();
	m_Mask.Assign(0.0);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY(); y+=nCells)
	{
		int	ny	= y + nCells < Get_NY() ? y + nCells : Get_NY();

		for(int x=0; x<Get_NX(); x+=nCells)
		{
			int		nx	= x + nCells < Get_NX() ? x + nCells : Get_NX();

			int		i, ix, iy, nz;
			double	z;

			for(iy=y, nz=0, z=0.0; iy<ny; iy++)
			{
				for(ix=x; ix<nx; ix++)
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
				m_Mask.Set_Value(x, y, true);
				m_Dist.Set_Value(x, y, z / nz);
			}
			else
			{
				m_Mask.Set_Value(x, y, false);

				if( m_pDistance->is_NoData(x, y) )
				{
					for(i=0; i<8; i++)
					{
						ix	= x + nCells * Get_xTo(i);
						iy	= y + nCells * Get_yTo(i);

						if( m_pDistance->is_InGrid(ix, iy) )
						{
							z	+= m_pDistance->asDouble(ix, iy);
							nz	++;
						}
					}

					m_Dist.Set_Value(x, y, nz > 0 ? z / nz : m_pDTM->asDouble(x, y));
				}
				else
				{
					m_Dist.Set_Value(x, y, m_pDistance->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	m_pDistance->Assign(&m_Dist);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CChannelNetwork_Altitude::Get_Change(int nCells)
{
	int		x, y;
	double	d, dMax;

	//-----------------------------------------------------
	for(y=0; y<Get_NY(); y+=nCells)
	{
		for(x=0; x<Get_NX(); x+=nCells)
		{
			if( !m_Mask.asByte(x, y) )
			{
				m_Dist.Set_Value(x, y, Get_Change(nCells, x, y));
			}
		}
	}

	//-----------------------------------------------------
	for(y=0, dMax=0.0; y<Get_NY(); y+=nCells)
	{
		for(x=0; x<Get_NX(); x+=nCells)
		{
			if( !m_Mask.asByte(x, y) )
			{
				if( dMax < (d = fabs(m_Dist.asDouble(x, y) - m_pDistance->asDouble(x, y))) )
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
	int		i, ix, iy;
	double	n, d, dz;

	for(i=0, d=0.0, n=0.0; i<8; i++)
	{
		ix	= x + nCells * Get_xTo(i);
		iy	= y + nCells * Get_yTo(i);

		if( m_pDistance->is_InGrid(ix, iy) )
		{
			dz	= 1.0 / Get_UnitLength(i);
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

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Valley depth is calculated here  vertical distance to a channel network base level. "
		"The algorithm consists of two major steps:\n"
		" 1. Interpolation of a channel network base level elevation\n"
		" 2. Subtraction of this base level from the original elevations\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ELEVATION"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "VALLEY_DEPTH"	, _TL("Valley Depth"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "RIDGE_LEVEL"		, _TL("Ridge Level"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"		, _TL("Tension Threshold [Percentage of Cell Size]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "NOUNDERGROUND"	, _TL("Keep Ridge Level above Surface"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "ORDER"			, _TL("Ridge Detection Threshold"),
		_TL(""),
		PARAMETER_TYPE_Int, 4, 1, true, 7, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RUN_MODULE(LIBRARY, MODULE, CONDITION)	{\
	bool	bResult;\
	SG_RUN_MODULE(bResult, LIBRARY, MODULE, CONDITION)\
	if( !bResult ) return( false );\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pModule->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)

//---------------------------------------------------------
bool CValley_Depth::On_Execute(void)
{
	CSG_Grid	Inverse(*Get_System(), SG_DATATYPE_Float);
	CSG_Grid	Ridges (*Get_System(), SG_DATATYPE_Int);

	//-----------------------------------------------------
	RUN_MODULE("grid_tools"					, 19,	// grid orientation
			SET_PARAMETER("INPUT"			, Parameters("ELEVATION"))
		&&	SET_PARAMETER("RESULT"			, &Inverse)
		&&	SET_PARAMETER("METHOD"			, 3)	// invert
	)

	RUN_MODULE("ta_channels"				, 6,	// strahler order
			SET_PARAMETER("DEM"				, &Inverse)
		&&	SET_PARAMETER("STRAHLER"		, &Ridges)
	)

	Ridges.Set_NoData_Value_Range(0, Parameters("ORDER")->asInt());

	RUN_MODULE("ta_channels"				, 3,	// vertical channel network distance
			SET_PARAMETER("ELEVATION"		, &Inverse)
		&&	SET_PARAMETER("CHANNELS"		, &Ridges)
		&&	SET_PARAMETER("DISTANCE"		, Parameters("VALLEY_DEPTH"))
		&&	SET_PARAMETER("BASELEVEL"		, Parameters("RIDGE_LEVEL"))
		&&	SET_PARAMETER("THRESHOLD"		, Parameters("THRESHOLD"))
		&&	SET_PARAMETER("NOUNDERGROUND"	, Parameters("NOUNDERGROUND"))
	)

	//-----------------------------------------------------
	if( Parameters("RIDGE_LEVEL")->asGrid() )
	{
		Parameters("RIDGE_LEVEL")->asGrid()->Invert();
	}

	DataObject_Set_Colors(Parameters("VALLEY_DEPTH")->asGrid(), 10, SG_COLORS_YELLOW_BLUE, false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
