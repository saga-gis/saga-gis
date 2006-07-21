
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
//                 Copyright (C) 2003 by                 //
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
#include "Kriging_Universal_Global.h"


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

	Set_Author		(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TL(
		"Universal Kriging for grid interpolation from irregular sample points.\n"
		"This implementation does not use a maximum search radius. The weighting "
		"matrix is generated globally for all points."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	,"INTERPOL"		, _TL("Grid Interpolation"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
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
	int		i, j, n, nGrids;
	double	Lambda;

	//-----------------------------------------------------
	if(	(n = m_Points.Get_Count()) > 1 && (nGrids = m_pGrids->Get_Count()) > 0 )
	{
		for(i=0; i<n; i++)
		{
			m_G[i]	= Get_Weight(x - m_Points[i].x, y - m_Points[i].y);
		}

		m_G[n]	= 1.0;

		for(i=0, j=n+1; i<nGrids; i++, j++)
		{
			if( !m_pGrids->asGrid(i)->Get_Value(x, y, m_G[j], m_Interpolation, true) )
			{
				return( false );
			}
		}

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			for(j=0, Lambda=0.0; j<=n+nGrids; j++)
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
	int		i, j, n, iGrid, nGrids;

	//-----------------------------------------------------
	if( (nGrids = m_pGrids->Get_Count()) > 0 )
	{
		for(int iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
		{
			CShape	*pShape	= m_pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					m_Points.Add(
						pShape->Get_Point(iPoint, iPart).x,
						pShape->Get_Point(iPoint, iPart).y,
						pShape->Get_Record()->asDouble(m_zField)
					);
				}
			}
		}

		//-------------------------------------------------
		if( (n = m_Points.Get_Count()) > 1 )
		{
			m_G.Create(n + 1 + nGrids);
			m_W.Create(n + 1 + nGrids, n + 1 + nGrids);

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

				for(iGrid=0, j=n+1; iGrid<nGrids; iGrid++, j++)
				{
					m_W[i][j]	= m_W[j][i]	= m_pGrids->asGrid(iGrid)->Get_Value(
						m_Points[i].x, m_Points[i].y, m_Interpolation
					);
				}
			}

			for(i=n; i<=n+nGrids; i++)
			{
				for(j=n; j<=n+nGrids; j++)
				{
					m_W[i][j]	= 0.0;
				}
			}

			return( m_W.Set_Inverse() );
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
