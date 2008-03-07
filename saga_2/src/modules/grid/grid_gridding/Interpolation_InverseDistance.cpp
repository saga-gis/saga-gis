
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
	Set_Name		(_TL("Inverse Distance"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Inverse distance to a power method for grid interpolation from irregular distributed points."
	));

	Parameters.Add_Value(
		NULL	, "POWER"		, _TL("Inverse Distance: Power"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Search Radius"),
		_TL(""),
		PARAMETER_TYPE_Double	, 100.0
	);

	Parameters.Add_Value(
		NULL	, "NPOINTS"		, _TL("Maximum Points"),
		_TL(""),
		PARAMETER_TYPE_Int		, 10.0
	);
}

//---------------------------------------------------------
CInterpolation_InverseDistance::~CInterpolation_InverseDistance(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CInterpolation_InverseDistance::On_Initialize(void)
{
	m_Power			= Parameters("POWER")	->asDouble();
	m_Radius		= Parameters("RADIUS")	->asDouble();
	m_nPoints_Max	= Parameters("NPOINTS")	->asInt();

	return( Set_Search_Engine() );
}

//---------------------------------------------------------
bool CInterpolation_InverseDistance::Get_Value(double x, double y, double &z)
{
	int			i, n;
	double		d, ds;
	TSG_Point	p;
	CSG_Shape		*pPoint;

	if( (n = m_Search.Select_Radius(x, y, m_Radius, true, m_nPoints_Max)) > 0 )
	{
		for(i=0, p.x=0, p.y=y, z=0.0, ds=0.0; i<n; i++)
		{
			if( (pPoint = m_Search.Get_Selected_Point(i)) != NULL )
			{
				d	= SG_Get_Distance(p, pPoint->Get_Point(0));

				if( d <= 0.0 )
				{
					z	= pPoint->Get_Record()->asDouble(m_zField);

					return( true );
				}

				d	= pow(d, -m_Power);

				z	+= d * pPoint->Get_Record()->asDouble(m_zField);
				ds	+= d;
			}
		}

		if( ds > 0.0 )
		{
			z	/= ds;

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
