
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Distance_Gradient.cpp                 //
//                                                       //
//                 Copyright (C) 2006 by                 //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "distance_gradient.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDistance_Gradient::CDistance_Gradient(void)
{
	Set_Name		(_TL("Downslope Distance Gradient"));

	Set_Author		(_TL("Copyrights (c) 2006 by Olaf Conrad"));

	Set_Description	(_TL(
		"Calculation of a new topographic index to quantify downslope controls on local drainage. "
		""
		"\n\n"
		"References:\n"
		"- Hjerdt, K.N., McDonnell, J.J., Seibert, J. Rodhe, A. (2004): "
		"  'A new topographic index to quantify downslope controls on local drainage', "
		"  Water Resources Research, 40\n"
		"\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRADIENT"	, _TL("Gradient"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "DIFFERENCE"	, _TL("Gradient Difference"),
		_TL("Difference to local gradient."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "DISTANCE"	, _TL("Vertical Distance"),
		"",
		PARAMETER_TYPE_Double	, 10.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		"",

		CSG_String::Format("%s|%s|%s|",
			_TL("distance"),
			_TL("gradient (tangens)"),
			_TL("gradient (degree)")
		), 2
	);
}

//---------------------------------------------------------
CDistance_Gradient::~CDistance_Gradient(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDistance_Gradient::On_Execute(void)
{
	int			x, y, Output;
	double		vDistance, hDistance, s, a;
	CSG_Grid	*pGradient, *pDifference;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM")			->asGrid();
	pGradient	= Parameters("GRADIENT")	->asGrid();
	pDifference	= Parameters("DIFFERENCE")	->asGrid();
	vDistance	= Parameters("DISTANCE")	->asDouble();
	Output		= Parameters("OUTPUT")		->asInt();

	//-----------------------------------------------------
	if( vDistance > 0.0 )
	{
		switch( Output )
		{
		case 0:	// distance
			DataObject_Set_Colors(pGradient, 100, SG_COLORS_WHITE_BLUE	, false);
			pGradient->Set_Unit("m");
			pGradient->Set_ZFactor(1.0);
			break;

		case 1:	// gradient (ratio)
			DataObject_Set_Colors(pGradient, 100, SG_COLORS_WHITE_BLUE	, true);
			pGradient->Set_Unit("");
			pGradient->Set_ZFactor(1.0);
			break;

		case 2:	// gradient (degree)
			DataObject_Set_Colors(pGradient, 100, SG_COLORS_YELLOW_RED	, false);
			pGradient->Set_Unit("°");
			pGradient->Set_ZFactor(M_RAD_TO_DEG);
			break;
		}

		if( pDifference )
		{
			DataObject_Set_Colors(pDifference, 100, SG_COLORS_RED_GREY_BLUE	, false);
			pDifference->Set_Unit("°");
			pDifference->Set_ZFactor(M_RAD_TO_DEG);
		}

		//-------------------------------------------------
		m_Dir.Create(m_pDEM, GRID_TYPE_Char);

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				m_Dir.Set_Value(x, y, m_pDEM->Get_Gradient_NeighborDir(x, y));
			}
		}

		//-------------------------------------------------
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( (hDistance = Get_hDistance(x, y, vDistance)) > 0.0 )
				{
					switch( Output )
					{
					case 0:	// distance
						pGradient->Set_Value(x, y, hDistance);
						break;

					case 1:	// gradient (tangens)
						pGradient->Set_Value(x, y, vDistance / hDistance);
						break;

					case 2:	// gradient (degree)
						pGradient->Set_Value(x, y, atan(vDistance / hDistance));
						break;
					}

					if( pDifference )
					{
						if( m_pDEM->Get_Gradient(x, y, s, a) )
							pDifference->Set_Value (x, y, s - atan(vDistance / hDistance));
						else
							pDifference->Set_NoData(x, y);
					}
				}
				else
				{
					pGradient->Set_NoData(x, y);

					if( pDifference )
						pDifference->Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
		m_Dir.Destroy();

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
double CDistance_Gradient::Get_hDistance(int x, int y, double vDistance)
{
	int		Dir;
	double	zStart, zStop, z, zLast, hDistance;

	hDistance	= 0.0;

	if( m_pDEM->is_InGrid(x, y) )
	{
		zStart	= z	= m_pDEM->asDouble(x, y);
		zStop	= zStart - vDistance;

		while( z > zStop && m_pDEM->is_InGrid(x, y) && (Dir = m_Dir.asInt(x, y)) >= 0 )//&& Process_Get_Okay(false) )
		{
			x		+= Get_xTo(Dir);
			y		+= Get_yTo(Dir);

			if( m_pDEM->is_InGrid(x, y) )
			{
				zLast	= z;
				z		= m_pDEM->asDouble(x, y);

				if( z < zStop )
				{
					hDistance	+= Get_Length(Dir) * (zStop - zLast) / (z - zLast);
				}
				else
				{
					hDistance	+= Get_Length(Dir);
				}
			}
			else
			{
				hDistance	+= Get_Length(Dir);
			}
		}

		if( !m_pDEM->is_InGrid(x, y) )
		{
			if( (z = zStart - z) > 0.0 )
			{
				hDistance	*= vDistance / z;
			}
			else
			{
				hDistance	= SG_Get_Length(m_pDEM->Get_XRange(), m_pDEM->Get_YRange());
			}
		}
	}

	return( hDistance );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
