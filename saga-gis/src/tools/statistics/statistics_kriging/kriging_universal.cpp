
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
//                 kriging_universal.cpp                 //
//                                                       //
//                 Olaf Conrad (C) 2008                  //
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
#include "kriging_universal.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Universal::CKriging_Universal(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Universal Kriging"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Universal Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"NODE_UK"	, _TL("Universal Kriging"),
		_TL("")
	);

	Parameters.Add_Grid_List("NODE_UK",
		"PREDICTORS", _TL("Predictors"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Choice("NODE_UK",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("NODE_UK",
		"COORDS"	, _TL("Coordinates"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Init_Points(CSG_Shapes *pPoints, int Field, bool bLog)
{
	m_pPredictors	= Parameters("PREDICTORS")->asGridList();

	m_bCoords	= Parameters("COORDS")->asBool();

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: m_Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: m_Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: m_Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: m_Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	m_Points.Create(3, pPoints->Get_Count());

	int	n	= 0;

	for(int i=0; i<pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(i);

		bool	bOkay	= !pPoint->is_NoData(Field);	// for better performance, make sure all predictors supply a value now

		for(int j=0; bOkay && j<m_pPredictors->Get_Grid_Count(); j++)
		{
			bOkay	= m_pPredictors->Get_Grid(j)->is_InGrid_byPos(pPoint->Get_Point(0));
		}

		if( bOkay )
		{
			m_Points[n][0]	= pPoint->Get_Point(0).x;
			m_Points[n][1]	= pPoint->Get_Point(0).y;
			m_Points[n][2]	= bLog ? log(pPoint->asDouble(Field)) : pPoint->asDouble(Field);

			n++;
		}
	}

	if( n < 2 )
	{
		return( false );
	}

	m_Points.Set_Rows(n);	// resize if there are no-data values

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Get_Weights(const CSG_Matrix &Points, CSG_Matrix &W)
{
	int	i, j, k, n	= Points.Get_NRows();

	int	nCoords	= m_bCoords ? 2 : 0;
	int	nGrids	= m_pPredictors->Get_Grid_Count();

	if( n < 1 || !W.Create(n + 1 + nGrids + nCoords, n + 1 + nGrids + nCoords) )
	{
		return( false );
	}

	for(i=0; i<n; i++)
	{
		W[i][i]           = 0.;	// diagonal...
		W[i][n] = W[n][i] = 1.;	// edge...

		for(j=i+1; j<n; j++)
		{
			W[i][j] = W[j][i] = Get_Weight(Points[i], Points[j]);
		}

		for(k=0, j=n+1; k<nGrids; k++, j++)
		{
			W[i][j] = W[j][i] = m_pPredictors->Get_Grid(k)->Get_Value(Points[i][0], Points[i][1], m_Resampling);
		}

		for(k=0, j=n+nGrids+1; k<nCoords; k++, j++)
		{
			W[i][j] = W[j][i] = k == 0 ? Points[i][0] : Points[i][1];
		}
	}

	for(i=n; i<=n+nGrids+nCoords; i++)
	{
		for(j=n; j<=n+nGrids+nCoords; j++)
		{
			W[i][j] = 0.;
		}
	}

	return( W.Set_Inverse(m_Search.is_Okay(), n + 1 + nGrids + nCoords) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Get_Value(double x, double y, double &v, double &e)
{
	CSG_Matrix	__Points, __W;	double	**P, **W;	int	i, j, n = 0;

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
	int	nCoords	= m_bCoords ? 2 : 0;
	int	nGrids	= m_pPredictors->Get_Grid_Count();

	CSG_Vector	G(n + 1 + nGrids + nCoords);

	for(i=0; i<n; i++)
	{
		G[i]	= Get_Weight(x, y, P[i][0], P[i][1]);
	}

	G[n]	= 1.;

	for(i=0, j=n+1; i<nGrids; i++, j++)
	{
		if( !m_pPredictors->Get_Grid(i)->Get_Value(x, y, G[j], m_Resampling) )
		{
			return( false );
		}
	}

	if( m_bCoords )
	{
		G[n + 1 + nGrids]	= x;
		G[n + 2 + nGrids]	= y;
	}

	for(i=0, v=0., e=0.; i<n; i++)
	{
		double	Lambda	= 0.;

		for(j=0; j<=n+nGrids+nCoords; j++)
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
