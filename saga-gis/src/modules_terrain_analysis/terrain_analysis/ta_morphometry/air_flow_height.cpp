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
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Air_Flow_Height.cpp                  //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    e-mail:     conrad@geowiss.uni-hamburg.de          //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "air_flow_height.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CAir_Flow_Height::CAir_Flow_Height(void)
{
	Set_Name		(_TL("Effective Air Flow Heights"));

	Set_Author		(SG_T("J.Boehner, O.Conrad (c) 2008"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Effective Air Flow Heights"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "DIRECT"		, _TL("Wind Direction (Degree)"),
		_TL(""),
		PARAMETER_TYPE_Double	, 315.0, 0.0, true, 360.0, true
	);

	Parameters.Add_Value(
		NULL	, "LEEFACT"		, _TL("Lee Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.5
	);

	Parameters.Add_Value(
		NULL	, "LUVFACT"		, _TL("Luv Factor"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);

	Parameters.Add_Value(
		NULL	, "MAXDIST"		, _TL("Maximum Distance (km)"),
		_TL(""),
		PARAMETER_TYPE_Double	, 300.0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CAir_Flow_Height::On_Execute(void)
{
	int		x, y;

	double	Direction, d, dLuvA, dLuvB, dLee, dx, dy, z;

	//-----------------------------------------------------
	m_pDEM			= Parameters("ELEVATION")	->asGrid();
	m_pAFH			= Parameters("RESULT")		->asGrid();

	Direction		= Parameters("DIRECT")		->asDouble() * M_DEG_TO_RAD;
	m_dLee			= Parameters("LEEFACT")		->asDouble();
	m_dLuv			= Parameters("LUVFACT")		->asDouble();
	m_Distance_Max	= Parameters("MAXDIST")		->asDouble() * 1000.0;

	//-----------------------------------------------------
	dx		= sin(Direction);
	dy		= cos(Direction);

	if( fabs(dx) > fabs(dy) )
	{
		dy	/= fabs(dx);
		dx	= dx < 0 ? -1 : 1;
	}
	else
	{
		dx	/= fabs(dy);
		dy	= dy < 0 ? -1 : 1;
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_NoData(x,y) )
			{
				m_pAFH->Set_NoData(x,y);
			}
			else
			{
				dLuvA	= Get_Sum(x, y,  dx,  dy, m_dLuv);
						  Get_Sum(x, y, -dx, -dy, m_dLuv, dLuvB, m_dLee, dLee);

				d		= dLuvA > dLuvB ? dLuvA - dLuvB : 0;
				z		= m_pDEM->asDouble(x, y);
				dLee	= 1.0 + (z - dLee) / (z + dLee);
				d		= d + z * dLee * dLee / 2.0;

				m_pAFH->Set_Value(x, y, d < 0.0 ? 0.0 : d);
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
double CAir_Flow_Height::Get_Sum(int x, int y, double dx, double dy, double Factor)
{
	double	ix, iy, Dist, Sum, dxy, Weight, SumWeight;

	dxy			= Get_Cellsize() * sqrt(dx*dx + dy*dy);
	Dist		= 0.0;
	SumWeight	= 0.0;
	Sum			= 0.0;

	for(ix=x+0.5, iy=y+0.5; ; )
	{
		Dist	+= dxy;

		ix		+= dx;
		iy		+= dy;

		x		= (int)ix;
		y		= (int)iy;

		if( !is_InGrid(x,y) || Dist > m_Distance_Max )
		{
			break;
		}
		else if( !m_pDEM->is_NoData(x,y) )
		{
			SumWeight	+= Weight	= pow(Dist, -Factor);
			Sum			+= Weight * m_pDEM->asDouble(x,y);
		}
	}

	if( SumWeight > 0 )
	{
		Sum		/= SumWeight;
	}

	return( Sum );
}

//---------------------------------------------------------
void CAir_Flow_Height::Get_Sum(int x, int y, double dx, double dy, double FactorA, double &SumA, double FactorB, double &SumB)
{
	double	ix, iy, Dist, dxy, WeightA, SumWeightA, WeightB, SumWeightB;

	dxy			= Get_Cellsize() * sqrt(dx*dx + dy*dy);
	Dist		= 0.0;
	SumWeightA	= 0.0;
	SumA		= 0.0;
	SumWeightB	= 0.0;
	SumB		= 0.0;

	for(ix=x+0.5, iy=y+0.5; ; )
	{
		Dist	+= dxy;

		ix		+= dx;
		iy		+= dy;

		x		= (int)ix;
		y		= (int)iy;

		if( !is_InGrid(x,y) || Dist > m_Distance_Max )
		{
			break;
		}
		else if( !m_pDEM->is_NoData(x,y) )
		{
			SumWeightA	+= WeightA	= pow(Dist, -FactorA);
			SumA		+= WeightA * m_pDEM->asDouble(x,y);

			SumWeightB	+= WeightB	= pow(Dist, -FactorB);
			SumB		+= WeightB * m_pDEM->asDouble(x,y);
		}
	}

	if( SumWeightA > 0 )
	{
		SumA		/= SumWeightA;
	}

	if( SumWeightB > 0 )
	{
		SumB		/= SumWeightB;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
