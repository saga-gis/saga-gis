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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Universal Kriging"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Universal Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
	CSG_Parameter	*pSearch	= Parameters.Add_Node(
		NULL	, "NODE_SEARCH"			, _TL("Search Options"),
		_TL("")
	);

	pNode	= Parameters.Add_Choice(
		pSearch	, "SEARCH_RANGE"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("local"),
			_TL("global")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_RADIUS"		, _TL("Maximum Search Distance"),
		_TL("local maximum search distance given in map units"),
		PARAMETER_TYPE_Double	, 1000.0, 0, true
	);

	pNode	= Parameters.Add_Choice(
		pSearch	, "SEARCH_POINTS_ALL"	, _TL("Number of Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("maximum number of nearest points"),
			_TL("all points within search distance")
		)
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_POINTS_MIN"	, _TL("Minimum"),
		_TL("minimum number of points to use"),
		PARAMETER_TYPE_Int, 4, 1, true
	);

	Parameters.Add_Value(
		pNode	, "SEARCH_POINTS_MAX"	, _TL("Maximum"),
		_TL("maximum number of nearest points"),
		PARAMETER_TYPE_Int, 20, 1, true
	);

	Parameters.Add_Choice(
		pNode	, "SEARCH_DIRECTION"	, _TL("Search Direction"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all directions"),
			_TL("quadrants")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::On_Initialize(void)
{
	m_pGrids		= Parameters("GRIDS"            )->asGridList();
	m_Interpolation	= Parameters("INTERPOL"         )->asInt();

	m_nPoints_Min	= Parameters("SEARCH_POINTS_MIN")->asInt   ();
	m_nPoints_Max	= Parameters("SEARCH_POINTS_ALL")->asInt   () == 0
					? Parameters("SEARCH_POINTS_MAX")->asInt   () : 0;
	m_Radius		= Parameters("SEARCH_RANGE"     )->asInt   () == 0
					? Parameters("SEARCH_RADIUS"    )->asDouble() : 0.0;
	m_Direction		= Parameters("SEARCH_DIRECTION" )->asInt   () == 0 ? -1 : 4;

	//-----------------------------------------------------
	if( m_nPoints_Max <= 0 && m_Radius <= 0 )	// global
	{
		return( CKriging_Universal_Global::On_Execute() );
	}

	//-----------------------------------------------------
	m_Search.Create(m_pPoints->Get_Extent());

	for(int iPoint=0; iPoint<m_pPoints->Get_Count() && Set_Progress(iPoint, m_pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint	= m_pPoints->Get_Shape(iPoint);

		if( !pPoint->is_NoData(m_zField) )
		{
			bool		bAdd	= true;

			for(int iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				if( !m_pGrids->asGrid(iGrid)->is_InGrid_byPos(pPoint->Get_Point(0)) )
				{
					bAdd	= false;
				}
			}

			if( bAdd )
			{
				m_Search.Add_Point(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, m_bLog ? log(pPoint->asDouble(m_zField)) : pPoint->asDouble(m_zField));
			}
		}
	}

	if( !m_Search.is_Okay() )
	{
		SG_UI_Msg_Add(_TL("could not initialize point search engine"), true);

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CKriging_Universal::On_Finalize(void)
{
	m_Search.Destroy();

	return( true );
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
	int		n	= m_Search.Get_Nearest_Points(Points, p, m_nPoints_Max, m_Radius, m_Direction);

	if( n >= m_nPoints_Min )
	{
		int		i, j, k;

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
