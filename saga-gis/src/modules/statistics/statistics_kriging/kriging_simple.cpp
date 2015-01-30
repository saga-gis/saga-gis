/**********************************************************
 * Version $Id: kriging_simple.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                  kriging_simple.cpp                   //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//                University of Hamburg                  //
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
#include "kriging_simple.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Simple::CKriging_Simple(void)
	: CKriging_Simple_Global()
{
	//-----------------------------------------------------
	Set_Name		(_TL("Simple Kriging"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Simple Kriging for grid interpolation from irregular sample points."
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
bool CKriging_Simple::On_Initialize(void)
{
	//-----------------------------------------------------
	if( m_Search.Do_Use_All(true) )	// global
	{
		return( CKriging_Simple_Global::On_Initialize() );
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
bool CKriging_Simple::On_Finalize(void)
{
	m_Search.Finalize();
	m_Points.Destroy();

	return( CKriging_Simple_Global::On_Finalize() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CKriging_Simple::Get_Weights(const TSG_Point &p, CSG_Matrix &W, CSG_Points_Z &Points)
{
	if( m_Search.Get_Points(p, Points) )
	{
		int	n	= Points.Get_Count();

		W.Create(n, n);

		for(int i=0; i<n; i++)
		{
			W[i][i]	= 0.0;				// diagonal...

			for(int j=i+1; j<n; j++)
			{
				W[i][j]	= W[j][i]	= Get_Weight(Points[i], Points[j]);
			}
		}

		if( W.Set_Inverse(true, n) )
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
bool CKriging_Simple::Get_Value(const TSG_Point &p, double &z, double &v)
{
	//-----------------------------------------------------
	if( m_Search.Do_Use_All() )	// global
	{
		return( CKriging_Simple_Global::Get_Value(p, z, v) );
	}

	//-----------------------------------------------------
	int				i, j, n;
	CSG_Points_Z	Points;
	CSG_Matrix		W;

	//-----------------------------------------------------
	if(	(n = Get_Weights(p, W, Points)) > 0 )
	{
		CSG_Vector	G(n);

		for(i=0; i<n; i++)
		{
			G[i]	=	Get_Weight(p.x, p.y, Points[i].x, Points[i].y);
		}

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			double	Lambda	= 0.0;

			for(j=0; j<n; j++)
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
