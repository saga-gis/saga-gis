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
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//           Interpolation_InverseDistance.cpp           //
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
#include "Interpolation_InverseDistance.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CInterpolation_InverseDistance::CInterpolation_InverseDistance(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Inverse Distance Weighted"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Inverse distance grid interpolation from irregular distributed points."
	));

	pNode	= Parameters.Add_Choice(
		NULL	, "WEIGHTING"	, _TL("Distance Weighting"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("inverse distance to a power"),
			_TL("linearly decreasing within search radius"),
			_TL("exponential weighting scheme"),
			_TL("gaussian weighting scheme")
		), 0
	);

	Parameters.Add_Value(
		pNode	, "POWER"		, _TL("Inverse Distance Power"),
		_TL(""),
		PARAMETER_TYPE_Double	, 2.0
	);

	Parameters.Add_Value(
		pNode	, "BANDWIDTH"	, _TL("Exponential and Gaussian Weighting Bandwidth"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0, 0.0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "RANGE"		, _TL("Search Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("search radius (local)"),
			_TL("no search radius (global)")
		)
	);

	Parameters.Add_Value(
		pNode	, "RADIUS"		, _TL("Search Radius"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100.0
	);

	Parameters.Add_Choice(
		pNode	, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("all directions"),
			_TL("quadrants")
		)
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "POINTS"		, _TL("Number of Points"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("maximum number of points"),
			_TL("all points")
		)
	);

	Parameters.Add_Value(
		pNode	, "NPOINTS"		, _TL("Maximum Number of Points"),
		_TL(""),
		PARAMETER_TYPE_Int		, 10.0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_InverseDistance::On_Initialize(void)
{
	m_Weighting		= Parameters("WEIGHTING")	->asInt();
	m_Power			= Parameters("POWER")		->asDouble();
	m_Bandwidth		= Parameters("BANDWIDTH")	->asDouble();
	m_Mode			= Parameters("MODE")		->asInt();
	m_nPoints_Max	= Parameters("POINTS")		->asInt() == 0 ? Parameters("NPOINTS")->asInt()    : 0;
	m_Radius		= Parameters("RANGE")		->asInt() == 0 ? Parameters("RADIUS") ->asDouble() : 0.0;

	return( (m_nPoints_Max == 0 && m_Radius == 0.0) || Set_Search_Engine() );
}

//---------------------------------------------------------
inline double CInterpolation_InverseDistance::Get_Weight(double Distance)
{
	switch( m_Weighting )
	{
	default:	return( Distance > 0.0 ? pow(Distance, -m_Power) : -1.0 );
	case 1:		return( Distance < m_Radius ? (1.0 - Distance / m_Radius) : 0.0 );
	case 2:		return( exp(-Distance / m_Bandwidth) );
	case 3:		return( exp(-0.5 * SG_Get_Square(Distance / m_Bandwidth)) );
	}
}

//---------------------------------------------------------
bool CInterpolation_InverseDistance::Get_Value(double x, double y, double &z)
{
	double	w, ix, iy, iz;

	z	= 0.0;
	w	= 0.0;

	//-----------------------------------------------------
	if( m_nPoints_Max > 0 || m_Radius > 0.0 )	// using search engine
	{
		int		nPoints	= m_Search.Select_Nearest_Points(x, y, m_nPoints_Max, m_Radius, m_Mode == 0 ? -1 : 4);

		for(int iPoint=0; iPoint<nPoints; iPoint++)
		{
			if( m_Search.Get_Selected_Point(iPoint, ix, iy, iz) )
			{
				double	d	= Get_Weight(SG_Get_Distance(x, y, ix, iy));

				if( d <= 0.0 )
				{
					z	= iz;

					return( true );
				}

				z	+= d * iz;
				w	+= d;
			}
		}
	}

	//-----------------------------------------------------
	else										// without search engine
	{
		for(int iShape=0; iShape<m_pShapes->Get_Count() && Process_Get_Okay(); iShape++)
		{
			CSG_Shape	*pShape	= m_pShapes->Get_Shape(iShape);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point	p	= pShape->Get_Point(iPoint, iPart);
					double		d	= Get_Weight(SG_Get_Distance(x, y, p.x, p.y));

					if( d < 0.0 )
					{
						z	= pShape->asDouble(m_zField);

						return( true );
					}

					z	+= d * pShape->asDouble(m_zField);
					w	+= d;
				}
			}
		}
	}

	//-----------------------------------------------------
	if( w > 0.0 )
	{
		z	/= w;

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
