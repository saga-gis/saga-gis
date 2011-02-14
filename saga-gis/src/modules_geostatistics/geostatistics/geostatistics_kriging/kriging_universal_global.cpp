
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
	Set_Name		(_TL("Universal Kriging (VF, Global)"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

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

//---------------------------------------------------------
CKriging_Universal_Global::~CKriging_Universal_Global(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal_Global::On_Initialise(void)
{
	m_pGrids		= Parameters("GRIDS")		->asGridList();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();
	m_bCoords		= Parameters("COORDS")		->asBool();

	return( Get_Weights() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal_Global::Get_Value(double x, double y, double &z, double &v)
{
	int		i, j, n, nGrids, nCoords;
	double	Lambda;

	//-----------------------------------------------------
	if(	(n = m_Points.Get_Count()) > 1 )
	{
		nCoords	= m_bCoords ? 2 : 0;
		nGrids	= m_pGrids->Get_Count();

		for(i=0; i<n; i++)
		{
			if( !m_bBlock )
			{
				m_G[i]	=	Get_Weight(x - m_Points[i].x, y - m_Points[i].y);
			}
			else
			{
				m_G[i]	= (	Get_Weight((x          ) - m_Points[i].x, (y          ) - m_Points[i].y)
						+	Get_Weight((x + m_Block) - m_Points[i].x, (y + m_Block) - m_Points[i].y)
						+	Get_Weight((x + m_Block) - m_Points[i].x, (y - m_Block) - m_Points[i].y)
						+	Get_Weight((x - m_Block) - m_Points[i].x, (y + m_Block) - m_Points[i].y)
						+	Get_Weight((x - m_Block) - m_Points[i].x, (y - m_Block) - m_Points[i].y) ) / 5.0;
			}
		}

		m_G[n]	= 1.0;

		for(i=0, j=n+1; i<nGrids; i++, j++)
		{
			if( !m_pGrids->asGrid(i)->Get_Value(x, y, m_G[j], m_Interpolation, true) )
			{
				return( false );
			}
		}

		for(i=0, j=n+1+nGrids; i<nCoords; i++, j++)
		{
			m_G[j]	= i == 0 ? x : y;
		}

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			for(j=0, Lambda=0.0; j<=n+nGrids+nCoords; j++)
			{
				Lambda	+= m_W[i][j] * m_G[j];
			}

			z	+= Lambda * m_Points[i].z;
			v	+= Lambda * m_G[i];
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
bool CKriging_Universal_Global::Get_Weights(void)
{
	int		i, j, k, n, nGrids, nCoords;

	//-----------------------------------------------------
	nCoords	= m_bCoords ? 2 : 0;
	nGrids	= m_pGrids->Get_Count();

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
				m_Points.Add(pPoint->Get_Point(0).x, pPoint->Get_Point(0).y, pPoint->asDouble(m_zField));
			}
		}
	}

	//-----------------------------------------------------
	if( (n = m_Points.Get_Count()) > 1 )
	{
		m_G.Create(n + 1 + nGrids + nCoords);
		m_W.Create(n + 1 + nGrids + nCoords, n + 1 + nGrids + nCoords);

		for(i=0; i<n; i++)
		{
			m_W[i][i]	= 0.0;				// diagonal...
			m_W[i][n]	= m_W[n][i]	= 1.0;	// edge...

			for(j=i+1; j<n; j++)
			{
				m_W[i][j]	= m_W[j][i]	= Get_Weight(
					m_Points[i].x - m_Points[j].x,
					m_Points[i].y - m_Points[j].y
				);
			}

			for(k=0, j=n+1; k<nGrids; k++, j++)
			{
				m_W[i][j]	= m_W[j][i]	= m_pGrids->asGrid(k)->Get_Value(
					m_Points[i].x, m_Points[i].y, m_Interpolation
				);
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

		return( m_W.Set_Inverse() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
