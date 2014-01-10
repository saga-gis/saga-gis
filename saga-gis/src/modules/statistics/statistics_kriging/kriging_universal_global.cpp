/**********************************************************
 * Version $Id: kriging_universal_global.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//             Kriging_Universal_Global.cpp              //
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
#include "kriging_universal_global.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Universal_Global::CKriging_Universal_Global(void)
	: CKriging_Base()
{
	Set_Name		(_TL("Universal Kriging (Global)"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Universal Kriging for grid interpolation from irregular sample points.\n"
		"This implementation does not use a maximum search radius. The weighting "
		"matrix is generated globally for all points."
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Node(
		NULL	, "NODE_UK"		, _TL("Universal Kriging"),
		_TL("")
	);

	Parameters.Add_Grid_List(
		pNode	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Choice(
		pNode	,"INTERPOL"		, _TL("Grid Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Value(
		pNode	, "COORDS"		, _TL("Coordinates"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal_Global::On_Initialize(void)
{
	m_pGrids		= Parameters("GRIDS"   )->asGridList();
	m_Interpolation	= Parameters("INTERPOL")->asInt();
	m_bCoords		= Parameters("COORDS"  )->asBool();

	//-----------------------------------------------------
	int		i, j, k, n, nGrids, nCoords;

	nCoords	= m_bCoords ? 2 : 0;
	nGrids	= m_pGrids->Get_Count();

	//-----------------------------------------------------
	m_Points.Clear();

	for(i=0; i<m_pPoints->Get_Count(); i++)
	{
		CSG_Shape	*pPoint	= m_pPoints->Get_Shape(i);

		if( !pPoint->is_NoData(m_zField) )
		{
			bool		bAdd;

			for(j=0, bAdd=true; j<nGrids && bAdd; j++)
			{
				if( !m_pGrids->asGrid(j)->is_InGrid_byPos(pPoint->Get_Point(0)) )
				{
					bAdd	= false;
				}
			}

			if( bAdd )
			{
				m_Points.Add(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, m_bLog ? log(pPoint->asDouble(m_zField)) : pPoint->asDouble(m_zField));
			}
		}
	}

	//-----------------------------------------------------
	if( (n = m_Points.Get_Count()) > 1 )
	{
		m_W.Create(n + 1 + nGrids + nCoords, n + 1 + nGrids + nCoords);

		for(i=0; i<n; i++)
		{
			m_W[i][i]	= 0.0;				// diagonal...
			m_W[i][n]	= m_W[n][i]	= 1.0;	// edge...

			for(j=i+1; j<n; j++)
			{
				m_W[i][j]	= m_W[j][i]	= Get_Weight(m_Points[i], m_Points[j]);
			}

			for(k=0, j=n+1; k<nGrids; k++, j++)
			{
				m_W[i][j]	= m_W[j][i]	= m_pGrids->asGrid(k)->Get_Value(m_Points[i].x, m_Points[i].y, m_Interpolation);
			}

			for(k=0, j=n+nGrids+1; k<nCoords; k++, j++)
			{
				m_W[i][j]	= m_W[j][i]	= k == 0 ? m_Points[i].x : m_Points[i].y;
			}
		}

		for(i=n; i<=n+nGrids+nCoords; i++)
		{
			for(j=n; j<=n+nGrids+nCoords; j++)
			{
				m_W[i][j]	= 0.0;
			}
		}

		return( m_W.Set_Inverse(false) );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CKriging_Universal_Global::On_Finalize(void)
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
bool CKriging_Universal_Global::Get_Value(const TSG_Point &p, double &z, double &v)
{
	int		i, j, n, nGrids, nCoords;

	//-----------------------------------------------------
	if(	(n = m_Points.Get_Count()) > 1 )
	{
		nCoords	= m_bCoords ? 2 : 0;
		nGrids	= m_pGrids->Get_Count();

		CSG_Vector	G(n + 1 + nGrids + nCoords);

		for(i=0; i<n; i++)
		{
			G[i]	=	Get_Weight(p.x, p.y, m_Points[i].x, m_Points[i].y);
		}

		G[n]	= 1.0;

		for(i=0, j=n+1; i<nGrids; i++, j++)
		{
			if( !m_pGrids->asGrid(i)->Get_Value(p, G[j], m_Interpolation, true) )
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
