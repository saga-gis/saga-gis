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
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Kriging_Ordinary_Global.cpp              //
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
#include "kriging_ordinary_global.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Ordinary_Global::CKriging_Ordinary_Global(void)
	: CKriging_Base()
{
	Set_Name		(_TL("Ordinary Kriging (Global)"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Ordinary Kriging for grid interpolation from irregular sample points. "
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
bool CKriging_Ordinary_Global::On_Initialize(void)
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
		m_W.Create(n + 1, n + 1);

		for(i=0; i<n; i++)
		{
			m_W[i][i]	= 0.0;				// diagonal...
			m_W[i][n]	= m_W[n][i]	= 1.0;	// edge...

			for(j=i+1; j<n; j++)
			{
				m_W[i][j]	= m_W[j][i]	= Get_Weight(m_Points[i], m_Points[j]);
			}
		}

		m_W[n][n]	= 0.0;

		return( m_W.Set_Inverse(false) );
	}

	return( false );
}

//---------------------------------------------------------
bool CKriging_Ordinary_Global::On_Finalize(void)
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
bool CKriging_Ordinary_Global::Get_Value(const TSG_Point &p, double &z, double &v)
{
	int		i, j, n;

	//-----------------------------------------------------
	if(	(n = m_Points.Get_Count()) > 0 )
	{
		CSG_Vector	G(n + 1);

		for(i=0; i<n; i++)
		{
			G[i]	= Get_Weight(p.x, p.y, m_Points[i].x, m_Points[i].y);
		}

		G[n]	= 1.0;

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			double	Lambda	= 0.0;

			for(j=0; j<=n; j++)
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
