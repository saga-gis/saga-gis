
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
//                Gridding_Spline_MBA.cpp                //
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
#include "Gridding_Spline_MBA.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridding_Spline_MBA::CGridding_Spline_MBA(void)
	: CGridding_Spline_Base()
{
	Set_Name		(_TL("Multilevel B-Spline"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Multilevel B-spline algorithm for spatial interpolation of scattered data "
		"as proposed by Lee, Wolberg and Shin (1997).\n"
		"The algorithm makes use of a coarse-to-fine hierarchy of control lattices to "
		"generate a sequence of bicubic B-spline functions, whose sum approaches the "
		"desired interpolation function. Performance gains are realized by using "
		"B-spline refinement to reduce the sum of these functions into one equivalent "
		"B-spline function. "
		"\n\n"
		"The 'Maximum Level' determines the maximum size of the final B-spline matrix "
		"and increases exponential with each level. Where level=10 requires about 1mb "
		"level=12 needs about 16mb and level=14 about 256mb(!) of additional memory. "
	));

	Add_Reference(
		"Lee, S., Wolberg, G., Shin, S.Y.", "1997",
		"Scattered Data Interpolation with Multilevel B-Splines",
		"IEEE Transactions On Visualisation And Computer Graphics, Vol.3, No.3., p.228-244.",
		SG_T("https://www.researchgate.net/profile/George_Wolberg/publication/3410822_Scattered_Data_Interpolation_with_Multilevel_B-Splines/links/00b49518719ac9f08a000000/Scattered-Data-Interpolation-with-Multilevel-B-Splines.pdf"),
		SG_T("ResearchGate")
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Refinement"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("no"),
			_TL("yes")
		), 0
	);

	Parameters.Add_Double(
		"", "EPSILON"	, _TL("Threshold Error"),
		_TL(""),
		0.0001, 0.0, true
	);

	Parameters.Add_Int(
		"", "LEVEL_MAX"	, _TL("Maximum Level"),
		_TL(""),
		11, 1, true, 14, true
	);

	Parameters.Add_Bool(
		"", "UPDATE"	, _TL("Update View"),
		_TL(""),
		false
	)->Set_UseInCMD(false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGridding_Spline_MBA::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("UPDATE", pParameter->asInt() == 0);	// no performance gain with refinement!
	}

	return( CGridding_Spline_Base::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA::On_Execute(void)
{
	bool	bResult	= false;

	if( Initialize(m_Points, true, true) )
	{
		m_Epsilon	= Parameters("EPSILON")->asDouble();

		double	Cellsize	= M_GET_MAX(m_pGrid->Get_XRange(), m_pGrid->Get_YRange());

		switch( Parameters("METHOD")->asInt() )
		{
		case  0: bResult = _Set_MBA           (Cellsize); break;
		default: bResult = _Set_MBA_Refinement(Cellsize); break;
		}

		m_Points.Clear();

		Finalize(true);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA::_Set_MBA(double Cellsize)
{
	CSG_Grid	Phi;

	bool	bContinue	= true;

	int	Levels	= Parameters("LEVEL_MAX")->asInt();

	for(int Level=0; bContinue && Level<Levels && Process_Get_Okay(false); Level++, Cellsize/=2.)
	{
		bContinue	= BA_Set_Phi(Phi, Cellsize) && _Get_Difference(Phi, Level);

		BA_Set_Grid(Phi, Level > 0);

		if( Parameters("UPDATE")->asBool() )
		{
			DataObject_Update(m_pGrid, true);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA::_Set_MBA_Refinement(double Cellsize)
{
	CSG_Grid	Phi[2];

	bool	bContinue	= true;

	int	Levels	= Parameters("LEVEL_MAX")->asInt(), i = 0;

	for(int Level=0; bContinue && Level<Levels && Process_Get_Okay(false); Level++, Cellsize/=2.)
	{
		i	= Level % 2;

		bContinue	= BA_Set_Phi(Phi[i], Cellsize) && _Get_Difference(Phi[i], Level);

		_Set_MBA_Refinement(Phi[(i + 1) % 2], Phi[i]);
	}

	BA_Set_Grid(Phi[i]);

	return( true );
}

//---------------------------------------------------------
bool CGridding_Spline_MBA::_Set_MBA_Refinement(const CSG_Grid &Psi_0, CSG_Grid &Psi_1)
{
	if(	2 * (Psi_0.Get_NX() - 4) != (Psi_1.Get_NX() - 4)
	||	2 * (Psi_0.Get_NY() - 4) != (Psi_1.Get_NY() - 4) )
	{
		return( false );
	}

	#pragma omp parallel for
	for(int y=0; y<Psi_0.Get_NY(); y++)
	{
		int	yy	= 2 * y - 1;

		for(int x=0, xx=-1; x<Psi_0.Get_NX(); x++, xx+=2)
		{
			double	a[3][3];

			for(int iy=0, jy=y-1; iy<3; iy++, jy++)
			{
				for(int ix=0, jx=x-1; ix<3; ix++, jx++)
				{
					a[ix][iy]	= Psi_0.is_InGrid(jx, jy, false) ? Psi_0.asDouble(jx, jy) : 0.0;
				}
			}

			#define SET_PSI(x, y, z)	if( Psi_1.is_InGrid(x, y) ) { Psi_1.Add_Value(x, y, z); }

			SET_PSI(xx + 0, yy + 0,
				(  a[0][0] + a[0][2] + a[2][0] + a[2][2]
				+ (a[0][1] + a[1][0] + a[1][2] + a[2][1]) * 6.0
				+  a[1][1] * 36.0
				) / 64.0
			);

			SET_PSI(xx + 0, yy + 1,
				(  a[0][1] + a[0][2] + a[2][1] + a[2][2]
				+ (a[1][1] + a[1][2]) * 6.0
				) / 16.0
			);

			SET_PSI(xx + 1, yy + 0,
				(  a[1][0] + a[1][2] + a[2][0] + a[2][2]
				+ (a[1][1] + a[2][1]) * 6.0
				) / 16.0
			);

			SET_PSI(xx + 1, yy + 1,
				(  a[1][1] + a[1][2] + a[2][1] + a[2][2]
				) /  4.0
			);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGridding_Spline_MBA::_Get_Difference(const CSG_Grid &Phi, int Level)
{
	CSG_Simple_Statistics	Differences;

	for(int i=0; i<m_Points.Get_Count(); i++)
	{
		TSG_Point_Z	p	= m_Points[i];

		p.x	= (p.x - Phi.Get_XMin()) / Phi.Get_Cellsize();
		p.y	= (p.y - Phi.Get_YMin()) / Phi.Get_Cellsize();
		p.z	=  p.z - BA_Get_Phi(Phi, p.x, p.y);

		m_Points[i].z	= p.z;

		if( fabs(p.z) > m_Epsilon )
		{
			Differences	+= fabs(p.z);
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s:%d %s:%d %s:%f %s:%f",
		_TL("level"  ),      Level + 1,
		_TL("errors" ), (int)Differences.Get_Count  (),
		_TL("maximum"),      Differences.Get_Maximum(),
		_TL("mean"   ),      Differences.Get_Mean   ()
	);

	Process_Set_Text(CSG_String::Format("%s %d [%d]", _TL("Level"), Level + 1, (int)Differences.Get_Count()));

	return( Differences.Get_Maximum() > m_Epsilon );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGridding_Spline_MBA::BA_Get_B(int i, double d) const
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
bool CGridding_Spline_MBA::BA_Set_Phi(CSG_Grid &Phi, double Cellsize)
{
	int	n	= 4 + (int)(M_GET_MAX(m_pGrid->Get_XRange(), m_pGrid->Get_YRange()) / Cellsize);

	Phi.Create(SG_DATATYPE_Float, n, n, Cellsize, m_pGrid->Get_XMin(), m_pGrid->Get_YMin());

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

//---------------------------------------------------------
double CGridding_Spline_MBA::BA_Get_Phi(const CSG_Grid &Phi, double px, double py) const
{
	double	z	= 0.0;

	int	x	= (int)px;	px	-= x;
	int	y	= (int)py;	py	-= y;

	if(	x >= 0 && x < Phi.Get_NX() - 3 && y >= 0 && y < Phi.Get_NY() - 3 )
	{
		for(int iy=0; iy<4; iy++)
		{
			double	by	= BA_Get_B(iy, py);

			for(int ix=0; ix<4; ix++)
			{
				z	+= by * BA_Get_B(ix, px) * Phi.asDouble(x + ix, y + iy);
			}
		}
	}

	return( z );
}

//---------------------------------------------------------
void CGridding_Spline_MBA::BA_Set_Grid(const CSG_Grid &Phi, bool bAdd)
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
