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
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Gridding_Spline_MBA_Grid.cpp             //
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
#include "Gridding_Spline_MBA_Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_MBA_Grid::CGridding_Spline_MBA_Grid(void)
	: CGridding_Spline_Base(true)
{
	Set_Name		(_TL("Multilevel B-Spline Interpolation (from Grid)"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Multilevel B-spline algorithm for spatial interpolation of scattered data "
		"as proposed by Lee, Wolberg and Shin (1997). "
		"The algorithm makes use of a coarse-to-fine hierarchy of control lattices to "
		"generate a sequence of bicubic B-spline functions, whose sum approaches the "
		"desired interpolation function. Large performance gains are realized by using "
		"B-spline refinement to reduce the sum of these functions into one equivalent "
		"B-spline function. "
		"\n\n"
		"The 'Maximum Level' determines the maximum size of the final B-spline matrix "
		"and increases exponential with each level. Where level=10 requires about 1mb "
		"level=12 needs about 16mb and level=14 about 256mb(!) of additional memory. "
		"\n\n"
		"Reference:\n"
		" - Lee, S., Wolberg, G., Shin, S.Y. (1997):"
		" 'Scattered Data Interpolation with Multilevel B-Splines',"
		" IEEE Transactions On Visualisation And Computer Graphics, Vol.3, No.3\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("without B-spline refinement"),
			_TL("with B-spline refinement")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "EPSILON"		, _TL("Threshold Error"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.0001, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "LEVEL_MAX"	, _TL("Maximum Level"),
		_TL(""),
		PARAMETER_TYPE_Int		, 11, 1, true, 14, true
	);

	Parameters.Add_Value(
		NULL	, "UPDATE"		, _TL("Update View"),
		_TL(""),
		PARAMETER_TYPE_Bool		, false
	);

	Parameters.Add_Choice(
		Parameters("TARGET")	, "DATATYPE"	, _TL("Data Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("same as input grid"),
			_TL("floating point")
		), 1
	);
}

//---------------------------------------------------------
CGridding_Spline_MBA_Grid::~CGridding_Spline_MBA_Grid(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA_Grid::On_Execute(void)
{
	bool	bResult	= false;

	if( Initialise() )
	{
		if( Parameters("DATATYPE")->asInt() == 0 )
		{
			m_Points.Create(*Parameters("GRID")->asGrid());
		}
		else
		{
			m_Points.Create(Parameters("GRID")->asGrid());
			m_Points.Assign(Parameters("GRID")->asGrid());
		}

		m_Epsilon	= Parameters("EPSILON"  )->asDouble();
		m_Level_Max	= Parameters("LEVEL_MAX")->asInt   ();
		m_bUpdate	= Parameters("UPDATE"   )->asBool  ();

		double	dCell	= m_pGrid->Get_XRange() > m_pGrid->Get_YRange() ? m_pGrid->Get_XRange() : m_pGrid->Get_YRange();

		switch( Parameters("METHOD") ? Parameters("METHOD")->asInt() : 0 )
		{
		case 0:	// without B-spline refinement
			bResult	= _Set_MBA				(dCell);
			break;

		case 1:	// with B-spline refinement
			bResult	= _Set_MBA_Refinement	(dCell);
			break;
		}

		m_Points.Destroy();
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA_Grid::_Set_MBA(double dCell)
{
	bool		bContinue;
	int			nCells;
	CSG_Grid	Phi;

	for(bContinue=true, nCells=1; bContinue; nCells*=2, dCell/=2.0)
	{
		bContinue	= _Get_Phi(Phi, dCell, nCells);

		BA_Set_Grid	(Phi, nCells > 1);

		if( m_bUpdate )
		{
			DataObject_Update(m_pGrid, true);
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
bool CGridding_Spline_MBA_Grid::_Set_MBA_Refinement(double dCell)
{
	bool		bContinue;
	int			nCells;
	CSG_Grid	A, B, *Phi, *Psi, *pTmp;

	for(bContinue=true, Psi=&A, Phi=&B, nCells=1; bContinue; nCells*=2, dCell/=2.0)
	{
		bContinue	= _Get_Phi(*Phi, dCell, nCells);

		if( nCells > 1 )
		{
			_Set_MBA_Refinement(Psi, Phi);
		}

		pTmp	= Phi;	Phi	= Psi;	Psi	= pTmp;

		if( m_bUpdate )
		{
			BA_Set_Grid(*Psi);	DataObject_Update(m_pGrid, true);
		}
	}

	BA_Set_Grid(*Psi);

	return( true );
}

//---------------------------------------------------------
#define SET_PSI(x, y, z)	if( (x) >= 0 && (x) < Psi_B->Get_NX() && (y) >= 0 && (y) < Psi_B->Get_NY() )	Psi_B->Add_Value(x, y, z);

//---------------------------------------------------------
bool CGridding_Spline_MBA_Grid::_Set_MBA_Refinement(CSG_Grid *Psi_A, CSG_Grid *Psi_B)
{
	if(	Psi_A && Psi_B
	&&	2 * (Psi_A->Get_NX() - 4) == (Psi_B->Get_NX() - 4)
	&&	2 * (Psi_A->Get_NY() - 4) == (Psi_B->Get_NY() - 4) )
	{
		int		ax, ay, bx, by;
		double	a[3][3];

		for(ay=0, by=-1; ay<Psi_A->Get_NY() && Set_Progress(ay, Psi_A->Get_NY()); ay++, by+=2)
		{
			for(ax=0, bx=-1; ax<Psi_A->Get_NX(); ax++, bx+=2)
			{
				for(int iy=0, jy=ay-1; iy<3; iy++, jy++)
				{
					for(int ix=0, jx=ax-1; ix<3; ix++, jx++)
					{
						a[ix][iy]	= jx < 0 || jx >= Psi_A->Get_NX() || jy < 0 || jy >= Psi_A->Get_NY() ? 0.0 : Psi_A->asDouble(jx, jy);
					}
				}

				SET_PSI(bx + 0, by + 0,
					(  a[0][0] + a[0][2] + a[2][0] + a[2][2]
					+ (a[0][1] + a[1][0] + a[1][2] + a[2][1]) * 6.0
					+  a[1][1] * 36.0
					) / 64.0
				);

				SET_PSI(bx + 0, by + 1,
					(  a[0][1] + a[0][2] + a[2][1] + a[2][2]
					+ (a[1][1] + a[1][2]) * 6.0
					) / 16.0
				);

				SET_PSI(bx + 1, by + 0,
					(  a[1][0] + a[1][2] + a[2][0] + a[2][2]
					+ (a[1][1] + a[2][1]) * 6.0
					) / 16.0
				);

				SET_PSI(bx + 1, by + 1,
					(  a[1][1] + a[1][2] + a[2][1] + a[2][2]
					) /  4.0
				);
			}
		}

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
bool CGridding_Spline_MBA_Grid::_Get_Phi(CSG_Grid &Phi, double dCell, int nCells)
{
	Phi.Create	(SG_DATATYPE_Float, nCells + 4, nCells + 4, dCell, m_pGrid->Get_XMin(), m_pGrid->Get_YMin());
	BA_Get_Phi	(Phi);

	return( _Get_Difference(Phi) );
}

//---------------------------------------------------------
bool CGridding_Spline_MBA_Grid::_Get_Difference(CSG_Grid &Phi)
{
	int				xPoint, yPoint, nErrors;
	double			x, y, z, zMax, zMean;
	TSG_Point_Z	p;
	CSG_String		s;

	//-----------------------------------------------------
	for(yPoint=0, p.y=m_Points.Get_YMin(), zMax=0.0, nErrors=0, zMean=0.0; yPoint<m_Points.Get_NY() && Set_Progress(yPoint, m_Points.Get_NY()); yPoint++, p.y+=m_Points.Get_Cellsize())
	{
		for(xPoint=0, p.x=m_Points.Get_XMin(); xPoint<m_Points.Get_NX(); xPoint++, p.x+=m_Points.Get_Cellsize())
		{
			if( !m_Points.is_NoData(xPoint, yPoint) )
			{
				x	= (p.x - Phi.Get_XMin()) / Phi.Get_Cellsize();
				y	= (p.y - Phi.Get_YMin()) / Phi.Get_Cellsize();
				z	= m_Points.asDouble(xPoint, yPoint) - BA_Get_Value(x, y, Phi);

				m_Points.Set_Value(xPoint, yPoint, z);

				if( (z = fabs(z)) > m_Epsilon )
				{
					nErrors	++;
					zMean	+= fabs(z);

					if( fabs(z) > zMax )
					{
						zMax	= fabs(z);
					}
				}
				else
				{
				//	m_Points.Set_Value(xPoint, yPoint, 0.0);
					m_Points.Set_NoData(xPoint, yPoint);
				}
			}
		}
	}

	if( nErrors > 0 )
	{
		zMean	/= nErrors;
	}

	//-----------------------------------------------------
	int	i	= 1 + (int)(0.5 + log(Phi.Get_NX() - 4.0) / log(2.0));

	s.Printf(SG_T("%s:%d, %s:%d, %s:%f, %s:%f"),
		_TL("level"), i,
		_TL("error"), nErrors,
		_TL("max")	, zMax,
		_TL("mean")	, zMean
	);

	Process_Set_Text(s);
	Message_Add     (s);

	return( zMax >= m_Epsilon && i < m_Level_Max && Process_Get_Okay(false) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGridding_Spline_MBA_Grid::BA_Set_Grid(CSG_Grid &Phi, bool bAdd)
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
double CGridding_Spline_MBA_Grid::BA_Get_Value(double x, double y, CSG_Grid &Phi)
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
bool CGridding_Spline_MBA_Grid::BA_Get_Phi(CSG_Grid &Phi)
{
	int			xPoint, yPoint, _x, _y, ix, iy;
	double		x, y, z, dx, dy, wxy, wy, SW2, W[4][4];
	TSG_Point	p;
	CSG_Grid	Delta;

	//-----------------------------------------------------
	Phi		.Assign(0.0);
	Delta	.Create(Phi.Get_System());

	//-----------------------------------------------------
	for(yPoint=0, p.y=m_Points.Get_YMin(); yPoint<m_Points.Get_NY() && Set_Progress(yPoint, m_Points.Get_NY()); yPoint++, p.y+=m_Points.Get_Cellsize())
	{
		for(xPoint=0, p.x=m_Points.Get_XMin(); xPoint<m_Points.Get_NX(); xPoint++, p.x+=m_Points.Get_Cellsize())
		{
			if( !m_Points.is_NoData(xPoint, yPoint) )
			{
				x	= (p.x - Phi.Get_XMin()) / Phi.Get_Cellsize();
				y	= (p.y - Phi.Get_YMin()) / Phi.Get_Cellsize();
				z	= m_Points.asDouble(xPoint, yPoint);

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
inline double CGridding_Spline_MBA_Grid::BA_Get_B(int i, double d)
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
