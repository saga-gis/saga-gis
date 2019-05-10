
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Calculates B-spline functions for chosen level of detail. "
		"This tool serves as the basis for the 'Multilevel B-spline Interpolation' "
		"and is not suited as is for spatial data interpolation from "
		"scattered data. "
	));

	Add_Reference(
		"Lee, S., Wolberg, G., Shin, S.Y.", "1997",
		"Scattered Data Interpolation with Multilevel B-Splines",
		"IEEE Transactions On Visualisation And Computer Graphics, Vol.3, No.3., p.228-244.",
		SG_T("https://www.researchgate.net/profile/George_Wolberg/publication/3410822_Scattered_Data_Interpolation_with_Multilevel_B-Splines/links/00b49518719ac9f08a000000/Scattered-Data-Interpolation-with-Multilevel-B-Splines.pdf"),
		SG_T("ResearchGate")
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "LEVEL"		, _TL("Range"),
		_TL("B-spline range expressed as number of cells."),
		1, 0.001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_BA::On_Execute(void)
{
	bool	bResult	= false;

	if( Initialise(m_Points, true) )
	{
		double	Cellsize	= m_pGrid->Get_Cellsize() * Parameters("LEVEL")->asDouble();

		CSG_Grid	Phi;

		if( BA_Set_Phi(Phi, Cellsize) )
		{
			BA_Set_Grid(Phi);

			bResult	= true;
		}
	}

	m_Points.Clear();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGridding_Spline_BA::BA_Get_B(int i, double d) const
{
	switch( i )
	{
	case  0: d = 1. - d; return( d*d*d / 6. );

	case  1: return( ( 3. * d*d*d - 6. * d*d + 4.) / 6. );

	case  2: return( (-3. * d*d*d + 3. * d*d + 3. * d + 1.) / 6. );

	case  3: return( d*d*d / 6. );

	default: return( 0. );
	}
}

//---------------------------------------------------------
bool CGridding_Spline_BA::BA_Set_Phi(CSG_Grid &Phi, double Cellsize)
{
	int	nx	= (int)((m_pGrid->Get_XRange()) / Cellsize);
	int	ny	= (int)((m_pGrid->Get_YRange()) / Cellsize);

	Phi.Create(SG_DATATYPE_Float, nx + 4, ny + 4, Cellsize, m_pGrid->Get_XMin(), m_pGrid->Get_YMin());

	CSG_Grid	Delta(Phi.Get_System());

	//-----------------------------------------------------
	for(int i=0; i<m_Points.Get_Count(); i++)
	{
		TSG_Point_Z	p	= m_Points[i];

		int	x	= (int)(p.x	= (p.x - Phi.Get_XMin()) / Phi.Get_Cellsize());
		int	y	= (int)(p.y	= (p.y - Phi.Get_YMin()) / Phi.Get_Cellsize());

		if(	x >= 0 && x < Phi.Get_NX() - 3 && y >= 0 && y < Phi.Get_NY() - 3 )
		{
			int	iy;	double	W[4][4], SW2	= 0.0;

			for(iy=0; iy<4; iy++)	// compute W[k,l] and Sum[a=0-3, b=0-3](W²[a,b])
			{
				double	wy	= BA_Get_B(iy, p.y - y);

				for(int ix=0; ix<4; ix++)
				{
					SW2	+= SG_Get_Square(W[iy][ix] = wy * BA_Get_B(ix, p.x - x));
				}
			}

			if( SW2 > 0.0 )
			{
				p.z	/= SW2;

				for(iy=0; iy<4; iy++)
				{
					for(int ix=0; ix<4; ix++)
					{
						double	wxy	= W[iy][ix];

						Delta.Add_Value(x + ix, y + iy, wxy*wxy*wxy * p.z); // numerator
						Phi  .Add_Value(x + ix, y + iy, wxy*wxy          ); // denominator
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Phi.Get_NY(); y++)
	{
		for(int x=0; x<Phi.Get_NX(); x++)
		{
			double	z	=  Phi.asDouble(x, y);

			if( z != 0. )
			{
				Phi.Set_Value(x, y, Delta.asDouble(x, y) / z);
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
double CGridding_Spline_BA::BA_Get_Phi(const CSG_Grid &Phi, double px, double py) const
{
	double	z	= 0.0;

	int	x	= (int)px;
	int	y	= (int)py;

	if(	x >= 0 && x < Phi.Get_NX() - 3 && y >= 0 && y < Phi.Get_NY() - 3 )
	{
		for(int iy=0; iy<4; iy++)
		{
			double	by	= BA_Get_B(iy, py - y);

			for(int ix=0; ix<4; ix++)
			{
				z	+= by * BA_Get_B(ix, px - x) * Phi.asDouble(x + ix, y + iy);
			}
		}
	}

	return( z );
}

//---------------------------------------------------------
void CGridding_Spline_BA::BA_Set_Grid(const CSG_Grid &Phi, bool bAdd)
{
	double	d	= m_pGrid->Get_Cellsize() / Phi.Get_Cellsize();

	#pragma omp parallel for
	for(int y=0; y<m_pGrid->Get_NY(); y++)
	{
		double	py	= d * y;

		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			double	px	= d * x;

			if( bAdd )
			{	m_pGrid->Add_Value(x, y, BA_Get_Phi(Phi, px, py));	}
			else
			{	m_pGrid->Set_Value(x, y, BA_Get_Phi(Phi, px, py));	}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
