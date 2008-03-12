
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
//                 _Kriging_Universal.cpp                //
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
#include "_kriging_universal.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C_Kriging_Universal::C_Kriging_Universal(void)
	: C_Kriging_Universal_Global()
{
	Set_Name		(_TL("Universal Kriging (VF)"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Universal Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "MAXRADIUS"	, _TL("Maximum Search Radius (map units)"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1000.0, 0, true
	);

	Parameters.Add_Range(
		NULL	, "NPOINTS"		, _TL("Min./Max. Number of m_Points"),
		_TL(""), 4, 20, 1, true
	);
}

//---------------------------------------------------------
C_Kriging_Universal::~C_Kriging_Universal(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C_Kriging_Universal::On_Initialise(void)
{
	m_pGrids		= Parameters("GRIDS")		->asGridList();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();

	m_Radius		= Parameters("MAXRADIUS")	->asDouble();

	m_nPoints_Min	= (int)Parameters("NPOINTS")->asRange()->Get_LoVal();
	m_nPoints_Max	= (int)Parameters("NPOINTS")->asRange()->Get_HiVal();

	//-----------------------------------------------------
	if( m_Search.Create(m_pShapes) )
	{
		m_Points.Set_Count	(m_nPoints_Max);
		m_G		.Create		(m_nPoints_Max + 1 + m_pGrids->Get_Count());
		m_W		.Create		(m_nPoints_Max + 1 + m_pGrids->Get_Count(), m_nPoints_Max + 1 + m_pGrids->Get_Count());

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
bool C_Kriging_Universal::Get_Value(double x, double y, double &z, double &v)
{
	int		i, j, n, nGrids;
	double	Lambda;

	//-----------------------------------------------------
	if(	(n = Get_Weights(x, y)) > 0 && (nGrids = m_pGrids->Get_Count()) > 0 )
	{
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
			if( !m_pGrids->asGrid(i)->Get_Value(x, y, m_G[j], m_Interpolation) )
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
int C_Kriging_Universal::Get_Weights(double x, double y)
{
	int		i, j, n, iGrid, nGrids;

	//-----------------------------------------------------
	if( (n = m_Search.Select_Radius(x, y, m_Radius, true, m_nPoints_Max)) >= m_nPoints_Min && (nGrids = m_pGrids->Get_Count()) > 0 )
	{
		for(i=0; i<n; i++)
		{
			CSG_Shape *pPoint	= m_Search.Get_Selected_Point(i);
			m_Points[i].x	= pPoint->Get_Point(0).x;
			m_Points[i].y	= pPoint->Get_Point(0).y;
			m_Points[i].z	= pPoint->Get_Record()->asDouble(m_zField);
		}

		//-------------------------------------------------
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

		if( m_W.Set_Inverse(true, n + 1 + nGrids) )
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
