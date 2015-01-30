/**********************************************************
 * Version $Id: kriging_universal.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Kriging_Universal.cpp                 //
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
#include "kriging_universal.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Universal::CKriging_Universal(void)
	: CKriging_Universal_Global()
{
	//-----------------------------------------------------
	Set_Name		(_TL("Universal Kriging"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Universal Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
	m_Search.Create(&Parameters, Parameters.Add_Node(NULL, "NODE_SEARCH", _TL("Search Options"), _TL("")), 4);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::On_Initialize(void)
{
	m_pGrids		= Parameters("GRIDS"   )->asGridList();
	m_Interpolation	= Parameters("INTERPOL")->asInt();

	//-----------------------------------------------------
	if( m_Search.Do_Use_All(true) )	// global
	{
		return( CKriging_Universal_Global::On_Initialize() );
	}

	//-----------------------------------------------------
	if( !m_bLog )
	{
		return( m_Search.Initialize(m_pPoints, m_zField) );
	}
	else
	{
		m_Points.Create(SHAPE_TYPE_Point);
		m_Points.Add_Field("Z", SG_DATATYPE_Double);

		for(int iPoint=0; iPoint<m_pPoints->Get_Count() && Set_Progress(iPoint, m_pPoints->Get_Count()); iPoint++)
		{
			CSG_Shape	*pPoint	= m_pPoints->Get_Shape(iPoint);

			if( !pPoint->is_NoData(m_zField) )
			{
				m_Points.Add_Shape(pPoint, SHAPE_COPY_GEOM)->Set_Value(0, log(pPoint->asDouble(m_zField)));
			}
		}

		return( m_Search.Initialize(&m_Points, 0) );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CKriging_Universal::On_Finalize(void)
{
	m_Search.Finalize();
	m_Points.Destroy();

	return( CKriging_Universal_Global::On_Finalize() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKriging_Universal::Get_Weights(const TSG_Point &p, CSG_Matrix &W, CSG_Points_Z &Points)
{
	//-----------------------------------------------------
	if( m_Search.Get_Points(p, Points) )
	{
		int		i, j, k, n	= Points.Get_Count();

		int	nCoords	= m_bCoords ? 2 : 0;
		int	nGrids	= m_pGrids->Get_Count();

		W.Create(n + 1 + nGrids + nCoords, n + 1 + nGrids + nCoords);

		//-------------------------------------------------
		for(i=0; i<n; i++)
		{
			W[i][i]	= 0.0;				// diagonal...
			W[i][n]	= W[n][i]	= 1.0;	// edge...

			for(j=i+1; j<n; j++)
			{
				W[i][j]	= W[j][i]	= Get_Weight(Points[i], Points[j]);
			}

			for(k=0, j=n+1; k<nGrids; k++, j++)
			{
				W[i][j]	= W[j][i]	= m_pGrids->asGrid(k)->Get_Value(Points[i].x, Points[i].y, m_Interpolation);
			}

			for(k=0, j=n+nGrids+1; k<nCoords; k++, j++)
			{
				W[i][j]	= W[j][i]	= k == 0 ? Points[i].x : Points[i].y;
			}
		}

		for(i=n; i<=n+nGrids+nCoords; i++)
		{
			for(j=n; j<=n+nGrids+nCoords; j++)
			{
				W[i][j]	= 0.0;
			}
		}

		if( W.Set_Inverse(true, n + 1 + nGrids + nCoords) )
		{
			return( n );
		}
	}	

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Get_Value(const TSG_Point &p, double &z, double &v)
{
	//-----------------------------------------------------
	if( m_Search.Do_Use_All() )	// global
	{
		return( CKriging_Universal_Global::Get_Value(p, z, v) );
	}

	//-----------------------------------------------------
	int				i, j, n, nGrids, nCoords;
	CSG_Points_Z	Points;
	CSG_Matrix		W;

	//-----------------------------------------------------
	if(	(n = Get_Weights(p, W, Points)) > 1 )
	{
		nCoords	= m_bCoords ? 2 : 0;
		nGrids	= m_pGrids->Get_Count();

		CSG_Vector	G(n + 1 + nGrids + nCoords);

		for(i=0; i<n; i++)
		{
			G[i]	=	Get_Weight(p.x, p.y, Points[i].x, Points[i].y);
		}

		G[n]	= 1.0;

		for(i=0, j=n+1; i<nGrids; i++, j++)
		{
			if( !m_pGrids->asGrid(i)->Get_Value(p, G[j], m_Interpolation) )
			{
				return( false );
			}
		}

		if( m_bCoords )
		{
			G[n + 1 + nGrids]	= p.x;
			G[n + 2 + nGrids]	= p.y;
		}

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			double	Lambda	= 0.0;

			for(j=0; j<=n+nGrids+nCoords; j++)
			{
				Lambda	+= W[i][j] * G[j];
			}

			z	+= Lambda * Points[i].z;
			v	+= Lambda * G[i];
		}

		//-------------------------------------------------
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
