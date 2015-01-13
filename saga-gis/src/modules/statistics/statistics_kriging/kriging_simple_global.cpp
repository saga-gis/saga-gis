/**********************************************************
 * Version $Id: kriging_simple_global.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//               kriging_simple_global.cpp               //
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
#include "kriging_simple_global.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Simple_Global::CKriging_Simple_Global(void)
	: CKriging_Base()
{
	Set_Name		(_TL("Simple Kriging (Global)"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Simple Kriging for grid interpolation from irregular sample points. "
		"This implementation does not use a maximum search radius. The weighting "
		"matrix is generated once globally for all points."
	));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Simple_Global::On_Initialize(void)
{
	int		i, j, n;

	//-----------------------------------------------------
	m_Points.Clear();

	for(i=0; i<m_pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(m_zField) )
		{
			m_Points.Add(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, m_bLog ? log(pPoint->asDouble(m_zField)) : pPoint->asDouble(m_zField));
		}
	}

	//-----------------------------------------------------
	if( (n = m_Points.Get_Count()) > 1 )
	{
		m_W.Create(n, n);

		for(i=0; i<n; i++)
		{
			m_W[i][i]	= 0.0;				// diagonal...

			for(j=i+1; j<n; j++)
			{
				m_W[i][j]	= m_W[j][i]	= Get_Weight(m_Points[i], m_Points[j]);
			}
		}

		return( m_W.Set_Inverse(false) );
	}

	return( false );
}

//---------------------------------------------------------
bool CKriging_Simple_Global::On_Finalize(void)
{
	m_Points.Clear();
	m_W.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Simple_Global::Get_Value(const TSG_Point &p, double &z, double &v)
{
	int		i, j, n;

	//-----------------------------------------------------
	if(	(n = m_Points.Get_Count()) > 0 )
	{
		CSG_Vector	G(n);

		for(i=0; i<n; i++)
		{
			G[i]	= Get_Weight(p.x, p.y, m_Points[i].x, m_Points[i].y);
		}

		G[n]	= 1.0;

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			double	Lambda	= 0.0;

			for(j=0; j<n; j++)
			{
				Lambda	+= m_W[i][j] * G[j];
			}

			z	+= Lambda * m_Points[i].z;
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
