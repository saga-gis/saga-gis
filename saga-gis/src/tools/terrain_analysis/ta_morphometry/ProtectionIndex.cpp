
/*******************************************************************************
    ProtectionIndex.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "ProtectionIndex.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CProtectionIndex::CProtectionIndex(void)
{
	Set_Name		(_TL("Morphometric Protection Index"));

	Set_Author		("V.Olaya (c) 2005");

	Set_Description(_TW(
        "This algorithm analyses the immediate surrounding of each cell "
		"up to an given distance and evaluates how the relief protects it. "
         "It is equivalent to the positive openness described in Yokoyama (2002). "
	));

	Add_Reference("Yokoyama, R., Shirasawa, M., & Pike, R. J.", "2002",
		"Visualizing Topography by Openness: A New Application of Image Processing to Digital Elevation Models",
		"Photogrammetric Engineering and Remote Sensing(68), No. 3, March 2002, pp. 257-266."
	);

	Parameters.Add_Grid("", "DEM"       , _TL("Elevation"       ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("", "PROTECTION", _TL("Protection Index"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Double("", "RADIUS", _TL("Radius"), _TL("The radius in map units"), 2000., 0., true);

}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProtectionIndex::On_Execute(void){

	m_pDEM = Parameters("DEM")->asGrid();

	CSG_Grid *pProtection = Parameters("PROTECTION")->asGrid();

	DataObject_Set_Colors(pProtection, 5, SG_COLORS_RED_GREY_BLUE, false);

	double Radius = Parameters("RADIUS")->asDouble();

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Protection;

			if( Get_Protection(x, y, Radius, Protection) )
			{
				pProtection->Set_Value(x, y, Protection);
			}
			else
			{
				pProtection->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProtectionIndex::Get_Protection(int x, int y, double Radius, double &Protection)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	double z = m_pDEM->asDouble(x, y);

	Protection = 0.;

	for(int i=0; i<8; i++)
	{
		double maxAngle = 0.; int ix = x, iy = y;

		for(double Distance=Get_Length(i); Distance<Radius; Distance+=Get_Length(i))
		{
			if( m_pDEM->is_InGrid(ix += Get_xTo(i), iy += Get_yTo(i)) )
			{
				double Angle = atan((m_pDEM->asDouble(ix, iy) - z) / Distance);

				if( maxAngle < Angle )
				{
					maxAngle = Angle;
				}
			}
			else if( !is_InGrid(ix, iy) )
			{
				break;
			}
		}

		Protection += maxAngle;
	}

	Protection /= 8.;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
