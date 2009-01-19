
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    flow_width.cpp                     //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "flow_width.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Width::CFlow_Width(void)
{
	Set_Name		(_TL("Flow Width"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Flow width and specific catchment area (SCA) calculation.\n"
		"\n"
		"References:\n"
		"Gruber, S., Peckham, S. (2008): Land-Surface Parameters and Objects in Hydrology. "
		"In: Hengl, T. and Reuter, H.I. [Eds.]: Geomorphometry: Concepts, Software, Applications. "
		"Developments in Soil Science, Elsevier, 33:293-308.\n"
		"\n"
		"Quinn, P.F., Beven, K.J., Chevallier, P., Planchon, O. (1991): "
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models. "
		"Hydrological Processes, 5:59-79\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "WIDTH"	, _TL("Flow Width"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "TCA"		, _TL("Total Catchment Area (TCA)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SCA"		, _TL("Specific Catchment Area (SCA)"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Deterministic 8"),
			_TL("Multiple Flow Direction (Quinn et al. 1991)"),
			_TL("Aspect")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Width::On_Execute(void)
{
	int			x, y, Method;
	double		Width;
	CSG_Grid	*pWidth, *pTCA, *pSCA;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM")		->asGrid();
	pWidth		= Parameters("WIDTH")	->asGrid();
	pTCA		= Parameters("TCA")		->asGrid();
	pSCA		= Parameters("SCA")		->asGrid();
	Method		= Parameters("METHOD")	->asInt();

	if( pTCA )
	{
		if( pSCA == NULL )
		{
			Parameters("SCA")->Set_Value(pSCA = SG_Create_Grid(*Get_System(), GRID_TYPE_Float));
		}

		pSCA->Set_Name(_TL("Specific Catchment Area"));
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			switch( Method )
			{
			case 0:				// Deterministic 8
				Width	= Get_D8	(x, y);
				break;

			case 1:				// Multiple Flow Direction (Quinn et al. 1991)
				Width	= Get_MFD	(x, y);
				break;

			case 2:	default:	// Aspect
				Width	= Get_Aspect(x, y);
				break;
			}

			//---------------------------------------------
			if( Width > 0.0 )
			{
				pWidth->Set_Value(x, y, Width);
			}
			else
			{
				pWidth->Set_NoData(x, y);
			}

			if( pTCA && pSCA )
			{
				if( Width > 0.0 && !pTCA->is_NoData(x, y) )
				{
					pSCA->Set_Value(x, y, pTCA->asDouble(x, y) / Width);
				}
				else
				{
					pSCA->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CFlow_Width::Get_D8(int x, int y)
{
	int		Direction;

	if( (Direction = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 )
	{
		return( Get_Length(Direction) );
	}

	return( -1.0 );
}

//---------------------------------------------------------
inline double CFlow_Width::Get_MFD(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) )
	{
		double	Width	= 0.0;
		double	z		= m_pDEM->asDouble(x, y);

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy, true) && z > m_pDEM->asDouble(ix, iy) )
			{
				Width	+= 0.5 * Get_Cellsize() / Get_System()->Get_UnitLength(i);
			}
		}

		return( Width );
	}

	return( -1.0 );
}

//---------------------------------------------------------
inline double CFlow_Width::Get_Aspect(int x, int y)
{
	double	Slope, Aspect;

	if( m_pDEM->Get_Gradient(x, y, Slope, Aspect) )
	{
		return( (fabs(sin(Aspect)) + fabs(cos(Aspect))) * Get_Cellsize() );
	}

	return( -1.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
