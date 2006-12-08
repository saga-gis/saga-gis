
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Gridding_Spline_BA.cpp                //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Gridding_Spline_BA.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_BA::CGridding_Spline_BA(void)
	: CGridding_Spline_Base()
{
	Set_Name		(_TL("B-Spline Approximation"));

	Set_Author		(_TL("Copyrights (c) 2006 by Olaf Conrad"));

	Set_Description	(_TW(
		"Calculates B-spline functions for choosen level of detail. "
		"This module serves as basis for the 'Multilevel B-Spline Interpolation' "
		"and is not suited as it is for spatial data interpolation from "
		"scattered data. "

		"\n\n"
		"Reference:\n"
		" - Lee, S., Wolberg, G., Shin, S.Y. (1997):"
		" 'Scattered Data Interpolation with Multilevel B-Splines',"
		" IEEE Transactions On Visualisation And Computer Graphics, Vol.3, No.3\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "LEVEL"		, _TL("Resolution"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1, 0.001, true
	);
}

//---------------------------------------------------------
CGridding_Spline_BA::~CGridding_Spline_BA(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_BA::On_Execute(void)
{
	bool	bResult	= false;
	int		nx, ny;
	double	d;
	CSG_Grid	Phi;

	if( Initialise(m_Points, true) )
	{
		d		= m_pGrid->Get_Cellsize() * Parameters("LEVEL")->asDouble();
		nx		= (int)((m_pGrid->Get_XRange()) / d);
		ny		= (int)((m_pGrid->Get_YRange()) / d);
		Phi.Create(GRID_TYPE_Float, nx + 4, ny + 4, d, m_pGrid->Get_XMin(), m_pGrid->Get_YMin());

		BA_Get_Phi	(Phi);
		BA_Set_Grid	(Phi);

		bResult	= true;
	}

	m_Points.Clear();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGridding_Spline_BA::BA_Set_Grid(CSG_Grid &Phi, bool bAdd)
{
	int		ix, iy;
	double	x, y, d	= m_pGrid->Get_Cellsize() / Phi.Get_Cellsize();

	for(iy=0, y=0.0; iy<m_pGrid->Get_NY() && Set_Progress(iy, m_pGrid->Get_NY()); iy++, y+=d)
	{
		for(ix=0, x=0.0; ix<m_pGrid->Get_NX(); ix++, x+=d)
		{
			if( bAdd )
			{
				m_pGrid->Add_Value(ix, iy, BA_Get_Value(x, y, Phi));
			}
			else
			{
				m_pGrid->Set_Value(ix, iy, BA_Get_Value(x, y, Phi));
			}
		}
	}
}

//---------------------------------------------------------
inline double CGridding_Spline_BA::BA_Get_Value(double x, double y, CSG_Grid &Phi)
{
	int		_x, _y, ix, iy;
	double	z	= 0.0, bx[4], by;

	if(	(_x = (int)x) >= 0 && _x < Phi.Get_NX() - 3
	&&	(_y = (int)y) >= 0 && _y < Phi.Get_NY() - 3 )
	{
		x	-= _x;
		y	-= _y;

		for(ix=0; ix<4; ix++)
		{
			bx[ix]	= BA_Get_B(ix, x);
		}

		for(iy=0; iy<4; iy++)
		{
			by	= BA_Get_B(iy, y);

			for(ix=0; ix<4; ix++)
			{
				z	+= by * bx[ix] * Phi.asDouble(_x + ix, _y + iy);
			}
		}
	}

	return( z );
}

//---------------------------------------------------------
bool CGridding_Spline_BA::BA_Get_Phi(CSG_Grid &Phi)
{
	int		iPoint, _x, _y, ix, iy;
	double	x, y, z, dx, dy, wxy, wy, SW2, W[4][4];
	CSG_Grid	Delta;

	//-----------------------------------------------------
	Phi		.Assign(0.0);
	Delta	.Create(Phi.Get_System());

	//-----------------------------------------------------
	for(iPoint=0; iPoint<m_Points.Get_Count() && Set_Progress(iPoint, m_Points.Get_Count()); iPoint++)
	{
		x	= (m_Points[iPoint].x - Phi.Get_XMin()) / Phi.Get_Cellsize();
		y	= (m_Points[iPoint].y - Phi.Get_YMin()) / Phi.Get_Cellsize();
		z	=  m_Points[iPoint].z;

		if(	(_x = (int)x) >= 0 && _x < Phi.Get_NX() - 3
		&&	(_y = (int)y) >= 0 && _y < Phi.Get_NY() - 3 )
		{
			dx	= x - _x;
			dy	= y - _y;

			for(iy=0, SW2=0.0; iy<4; iy++)	// compute W[k,l] and Sum[a=0-3, b=0-3](W²[a,b])
			{
				wy	= BA_Get_B(iy, dy);

				for(ix=0; ix<4; ix++)
				{
					wxy	= W[iy][ix]	= wy * BA_Get_B(ix, dx);

					SW2	+= wxy*wxy;
				}
			}

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					wxy	= W[iy][ix];

					Delta.Add_Value(_x + ix, _y + iy, wxy*wxy * ((wxy * z) / SW2));	// Numerator
					Phi  .Add_Value(_x + ix, _y + iy, wxy*wxy);						// Denominator
				}
			}
		}
	}

	//-----------------------------------------------------
	for(iy=0; iy<Phi.Get_NY(); iy++)
	{
		for(ix=0; ix<Phi.Get_NX(); ix++)
		{
			if( (z = Phi.asDouble(ix, iy)) != 0.0 )
			{
				Phi.Set_Value(ix, iy, Delta.asDouble(ix, iy) / z);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
inline double CGridding_Spline_BA::BA_Get_B(int i, double d)
{
	switch( i )
	{
	case 0:
		d	= 1.0 - d;
		return( d*d*d / 6.0 );

	case 1:	
		return( ( 3.0 * d*d*d - 6.0 * d*d + 4.0) / 6.0 );

	case 2:
		return( (-3.0 * d*d*d + 3.0 * d*d + 3.0 * d + 1.0) / 6.0 );

	case 3:
		return( d*d*d / 6.0 );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
