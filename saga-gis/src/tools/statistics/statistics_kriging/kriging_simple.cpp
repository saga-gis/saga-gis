
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  statistics_kriging                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  kriging_simple.cpp                   //
//                                                       //
//                 Olaf Conrad (C) 2015                  //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "kriging_simple.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Simple::CKriging_Simple(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Simple Kriging"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Simple Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Simple::Get_Weights(const CSG_Matrix &Points, CSG_Matrix &W)
{
	int	n	= Points.Get_NRows();

	if( n < 1 || !W.Create(n, n) )
	{
		return( false );
	}

	for(int i=0; i<n; i++)
	{
		W[i][i] = 0.;	// diagonal...

		for(int j=i+1; j<n; j++)
		{
			W[i][j] = W[j][i] = Get_Weight(Points[i], Points[j]);
		}
	}

	return( W.Set_Inverse(m_Search.is_Okay(), n) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Simple::Get_Value(double x, double y, double &v, double &e)
{
	CSG_Matrix	__Points, __W;	double	**P, **W;	int	i, n = 0;

	if( !m_Search.is_Okay() )	// global
	{
		n	= m_Points.Get_NRows();
		P	= m_Points.Get_Data ();
		W	= m_W     .Get_Data ();
	}
	else if( Get_Points(x, y, __Points) && Get_Weights(__Points, __W) )	// local
	{
		n	= __Points.Get_NRows();
		P	= __Points.Get_Data ();
		W	= __W     .Get_Data ();
	}

	if( n < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Vector	G(n);

	for(i=0; i<n; i++)
	{
		G[i]	= Get_Weight(x, y, P[i][0], P[i][1]);
	}

	for(i=0, v=0., e=0.; i<n; i++)
	{
		double	Lambda	= 0.;

		for(int j=0; j<n; j++)
		{
			Lambda	+= W[i][j] * G[j];
		}

		v	+= Lambda * P[i][2];
		e	+= Lambda * G[i];
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
