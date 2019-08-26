
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
//                kriging3d_ordinary.cpp                 //
//                                                       //
//                 Olaf Conrad (C) 2019                  //
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
#include "kriging3d_ordinary.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging3D_Ordinary::CKriging3D_Ordinary(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Ordinary Kriging (3D)"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Ordinary Kriging interpolation for 3-dimensional data points. "
		"Output will be a grid collection with evenly spaced Z-levels "
		"representing the 3rd dimension. "
	));

	//-----------------------------------------------------
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging3D_Ordinary::Get_Weights(const CSG_Matrix &Points, CSG_Matrix &W)
{
	int	n	= Points.Get_NRows();

	if( n < 1 || !W.Create(n + 1, n + 1) )
	{
		return( false );
	}

	for(int i=0; i<n; i++)
	{
		W[i][i]           = 0.;	// diagonal...
		W[i][n] = W[n][i] = 1.;	// edge...

		for(int j=i+1; j<n; j++)
		{
			W[i][j] = W[j][i] = Get_Weight(Points[i], Points[j]);
		}
	}

	W[n][n]	= 0.;

	return( W.Set_Inverse(m_Search.is_Okay(), n + 1) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging3D_Ordinary::Get_Value(double x, double y, double z, double &v, double &e)
{
	CSG_Matrix	__Points, __W;	double	**P, **W;	int	i, n = 0;

	if( !m_Search.is_Okay() )	// global
	{
		n	= m_Points.Get_NRows();
		P	= m_Points.Get_Data ();
		W	= m_W     .Get_Data ();
	}
	else if( Get_Points(x, y, z, __Points) && Get_Weights(__Points, __W) )	// local
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
	CSG_Vector	G(n + 1);

	for(i=0; i<n; i++)
	{
		G[i]	= Get_Weight(x, y, z, P[i][0], P[i][1], P[i][2]);
	}

	G[n]	= 1.;

	for(i=0, v=0., e=0.; i<n; i++)
	{
		double	Lambda	= 0.;

		for(int j=0; j<=n; j++)
		{
			Lambda	+= W[i][j] * G[j];
		}

		v	+= Lambda * P[i][3];
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
