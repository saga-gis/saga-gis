
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Calculation of a new topographic index to quantify downslope controls on local drainage. "
	));

	Add_Reference("Hjerdt, K.N., McDonnell, J.J., Seibert, J. Rodhe, A.", "2004",
		"A new topographic index to quantify downslope controls on local drainage",
		"Water Resources Research, 40."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "GRADIENT"	, _TL("Gradient"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "DIFFERENCE", _TL("Gradient Difference"),
		_TL("Difference to local gradient."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(
		"", "DISTANCE"	, _TL("Vertical Distance"),
		_TL(""),
		10., 0., true
	);

	Parameters.Add_Choice(
		"", "OUTPUT"	, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("distance"),
			_TL("gradient (tangens)"),
			_TL("gradient (degree)")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDistance_Gradient::On_Execute(void)
{
	int			Output;
	double		vDistance;
	CSG_Grid	*pGradient, *pDifference;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM"       )->asGrid();
	pGradient	= Parameters("GRADIENT"  )->asGrid();
	pDifference	= Parameters("DIFFERENCE")->asGrid();
	vDistance	= Parameters("DISTANCE"  )->asDouble();
	Output		= Parameters("OUTPUT"    )->asInt();

	//-----------------------------------------------------
	if( vDistance > 0. )
	{
		switch( Output )
		{
		case 0:	// distance
			DataObject_Set_Colors(pGradient, 11, SG_COLORS_YELLOW_BLUE, false);
			pGradient->Set_Unit(_TL("m"));
			break;

		case 1:	// gradient (ratio)
			DataObject_Set_Colors(pGradient, 11, SG_COLORS_YELLOW_BLUE,  true);
			pGradient->Set_Unit(_TL(""));
			break;

		case 2:	// gradient (degree)
			DataObject_Set_Colors(pGradient, 11, SG_COLORS_RED_GREEN  ,  true);
			pGradient->Set_Unit(_TL("radians"));
			break;
		}

		if( pDifference )
		{
			DataObject_Set_Colors(pDifference, 11, SG_COLORS_RED_GREY_BLUE, false);
			pDifference->Set_Unit(_TL("radians"));
		}

		//-------------------------------------------------
		m_Dir.Create(m_pDEM, SG_DATATYPE_Char);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				m_Dir.Set_Value(x, y, m_pDEM->Get_Gradient_NeighborDir(x, y));
			}
		}

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	hDistance	= Get_hDistance(x, y, vDistance);

				if( hDistance > 0. )
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
						double	Slope, Aspect;

						if( m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
						{
							pDifference->Set_Value (x, y, Slope - atan(vDistance / hDistance));
						}
						else
						{
							pDifference->Set_NoData(x, y);
						}
					}
				}
				else
				{
					pGradient->Set_NoData(x, y);

					if( pDifference )
					{
						pDifference->Set_NoData(x, y);
					}
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CDistance_Gradient::Get_hDistance(int x, int y, double vDistance)
{
	int		Dir;
	double	zStart, zStop, z, zLast, hDistance;

	hDistance	= 0.;

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
			if( (z = zStart - z) > 0. )
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
