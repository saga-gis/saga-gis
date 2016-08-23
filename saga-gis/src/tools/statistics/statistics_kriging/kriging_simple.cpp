/**********************************************************
 * Version $Id: kriging_simple.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
bool CKriging_Simple::Get_Weights(const CSG_Points_Z &Points, CSG_Matrix &W)
{
	int	n	= Points.Get_Count();

	if( n > 0 )
	{
		int	n	= Points.Get_Count();

		W.Create(n, n);

		for(int i=0; i<n; i++)
		{
			W[i][i]	= 0.0;				// diagonal...

			for(int j=i+1; j<n; j++)
			{
				W[i][j]	= W[j][i]	= Get_Weight(Points.Get_X(i), Points.Get_Y(i), Points.Get_X(j), Points.Get_Y(j));
			}
		}

		return( W.Set_Inverse(!m_Search.Do_Use_All(), n) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Simple::Get_Value(const TSG_Point &p, double &z, double &v)
{
	//-----------------------------------------------------
	int				i, n;
	double			**W;
	CSG_Matrix		_W;
	CSG_Points_Z	_Data, *pData;

	if( m_Search.Do_Use_All() )	// global
	{
		pData	= &m_Data;
		W		= m_W.Get_Data();
	}
	else if( m_Search.Get_Points(p, _Data) && Get_Weights(_Data, _W) )	// local
	{
		pData	= &_Data;
		W		= _W.Get_Data();
	}
	else
	{
		return( false );
	}

	//-----------------------------------------------------
	if(	(n = pData->Get_Count()) > 0 )
	{
		CSG_Vector	G(n);

		for(i=0; i<n; i++)
		{
			G[i]	= Get_Weight(p, pData->Get_Point(i));
		}

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			double	Lambda	= 0.0;

			for(int j=0; j<n; j++)
			{
				Lambda	+= W[i][j] * G[j];
			}

			z	+= Lambda * pData->Get_Z(i);
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
