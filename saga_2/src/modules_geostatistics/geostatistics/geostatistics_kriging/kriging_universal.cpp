
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
	Set_Name		(_TL("Universal Kriging (VF)"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

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

	Parameters.Add_Choice(
		NULL	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all directions"),
			_TL("quadrants")
		)
	);
}

//---------------------------------------------------------
CKriging_Universal::~CKriging_Universal(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::On_Initialise(void)
{
	m_pGrids		= Parameters("GRIDS")		->asGridList();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();

	m_Radius		= Parameters("MAXRADIUS")	->asDouble();

	m_nPoints_Min	= (int)Parameters("NPOINTS")->asRange()->Get_LoVal();
	m_nPoints_Max	= (int)Parameters("NPOINTS")->asRange()->Get_HiVal();

	m_Mode			= Parameters("MODE")		->asInt();

	//-----------------------------------------------------
	if( !m_Search.Create(m_pPoints, m_zField) )
	{
		SG_UI_Msg_Add(_TL("could not initialize point search engine"), true);

		return( false );
	}

	//-----------------------------------------------------
	int		nPoints_Max;

	switch( m_Mode )
	{
	default:	nPoints_Max	= m_nPoints_Max;		break;
	case 1:		nPoints_Max	= m_nPoints_Max * 4;	break;
	}

	m_Points.Set_Count	(nPoints_Max);
	m_G		.Create		(nPoints_Max + 1 + m_pGrids->Get_Count());
	m_W		.Create		(nPoints_Max + 1 + m_pGrids->Get_Count(), nPoints_Max + 1 + m_pGrids->Get_Count());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal::Get_Value(double x, double y, double &z, double &v)
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
int CKriging_Universal::Get_Weights(double x, double y)
{
	int		i, j, n, iGrid, nGrids	= m_pGrids->Get_Count();

	//-----------------------------------------------------
	switch( m_Mode )
	{
	default:	n	= m_Search.Select_Nearest_Points(x, y, m_nPoints_Max, m_Radius);	break;
	case 1:		n	= m_Search.Select_Nearest_Points(x, y, m_nPoints_Max, m_Radius, 4);	break;
	}

	//-----------------------------------------------------
	if( n >= m_nPoints_Min )
	{
		for(i=0; i<n; i++)
		{
			m_Search.Get_Selected_Point(i, m_Points[i].x, m_Points[i].y, m_Points[i].z);
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
