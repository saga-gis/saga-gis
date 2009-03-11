
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

	Set_Author		(SG_T("O. Conrad (c) 2003"));

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

	Parameters.Add_Choice(
		NULL	, "MODE"		, _TL("Search Mode"),
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
bool CInterpolation_InverseDistance::On_Initialize(void)
{
	m_Power			= Parameters("POWER")	->asDouble();
	m_Radius		= Parameters("RADIUS")	->asDouble();
	m_nPoints_Max	= Parameters("NPOINTS")	->asInt();
	m_Mode			= Parameters("MODE")	->asInt();

	return( Set_Search_Engine() );
}

//---------------------------------------------------------
bool CInterpolation_InverseDistance::Get_Value(double x, double y, double &z)
{
	int			i, n;
	double		ix, iy, iz, d, w;

	switch( m_Mode )
	{
	case 0:	n	= m_Search.Select_Nearest_Points(x, y, m_nPoints_Max, m_Radius);	break;
	case 1:	n	= m_Search.Select_Nearest_Points(x, y, m_nPoints_Max, m_Radius, 4);	break;
	}

	for(i=0, z=0.0, w=0.0; i<n; i++)
	{
		if( m_Search.Get_Selected_Point(i, ix, iy, iz) )
		{
			d	= SG_Get_Distance(x, y, ix, iy);

			if( d <= 0.0 )
			{
				z	= iz;

				return( true );
			}

			d	= pow(d, -m_Power);

			z	+= d * iz;
			w	+= d;
		}
	}

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
