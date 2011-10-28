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
//                 Kriging_Ordinary.cpp                  //
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
#include "kriging_ordinary.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Ordinary::CKriging_Ordinary(void)
	: CKriging_Ordinary_Global()
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Ordinary Kriging (VF)"));

	Set_Author		(SG_T("O.Conrad (c) 2008"));

	Set_Description	(_TW(
		"Ordinary Kriging for grid interpolation from irregular sample points."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Choice(
		NULL	, "GLOBAL"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("local"),
			_TL("global")
		)
	);

	Parameters.Add_Value(
		pNode	, "MAXRADIUS"	, _TL("Maximum Search Radius (map units)"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1000.0, 0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "ALL_POINTS"	, _TL("Number of Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("maximum number of points"),
			_TL("all points in search distance")
		)
	);

	Parameters.Add_Value(
		pNode	, "NPOINTS_MIN"	, _TL("Minimum"),
		_TL(""),
		PARAMETER_TYPE_Int, 4, 1, true
	);

	Parameters.Add_Value(
		pNode	, "NPOINTS_MAX"	, _TL("Maximum"),
		_TL(""),
		PARAMETER_TYPE_Int, 20, 1, true
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
CKriging_Ordinary::~CKriging_Ordinary(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Ordinary::On_Initialise(void)
{
	m_Radius		= Parameters("GLOBAL")->asBool() ? 0.0 : Parameters("MAXRADIUS")->asDouble();

	m_nPoints_Min	= Parameters("NPOINTS_MIN")	->asInt();
	m_nPoints_Max	= Parameters("ALL_POINTS")	->asBool() ? m_pPoints->Get_Count()
					: Parameters("NPOINTS_MAX")	->asInt();

	m_Mode			= Parameters("MODE")->asInt();

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
	m_G		.Create		(nPoints_Max + 1);
	m_W		.Create		(nPoints_Max + 1, nPoints_Max + 1);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Ordinary::Get_Value(double x, double y, double &z, double &v)
{
	int		i, j, n;
	double	Lambda;

	//-----------------------------------------------------
	if(	(n = Get_Weights(x, y)) > 0 )
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

		//-------------------------------------------------
		for(i=0, z=0.0, v=0.0; i<n; i++)
		{
			for(j=0, Lambda=0.0; j<=n; j++)
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
int CKriging_Ordinary::Get_Weights(double x, double y)
{
	int		i, j, n;

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

			if( m_bLog )
			{
				m_Points[i].z	= log(m_Points[i].z);
			}
		}

		//-------------------------------------------------
		for(i=0; i<n; i++)
		{
			m_W[i][i]	= 0.0;				// diagonale...
			m_W[i][n]	= m_W[n][i]	= 1.0;	// edge...

			for(j=i+1; j<n; j++)
			{
				m_W[i][j]	= m_W[j][i]	= Get_Weight(
					m_Points[i].x - m_Points[j].x,
					m_Points[i].y - m_Points[j].y
				);
			}
		}

		m_W[n][n]	= 0.0;

		if( m_W.Set_Inverse(true, 1 + n) )
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
