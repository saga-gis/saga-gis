/**********************************************************
 * Version $Id: flow_width.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Name		(_TL("Flow Width and Specific Catchment Area"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Flow width and specific catchment area (SCA) calculation. "
		"SCA calculation needs total catchment area (TCA) as input, "
		"which can be calculated with one of the flow accumulation tools. "
	));

	Add_Reference(
		"Gruber, S., Peckham, S.", "2008",
		"Land-Surface Parameters and Objects in Hydrology",
		"In: Hengl, T. and Reuter, H.I. [Eds.]: Geomorphometry: Concepts, Software, Applications. Developments in Soil Science, Elsevier, 33:293-308.",
		SG_T("https://www.elsevier.com/books/geomorphometry/hengl/978-0-12-374345-9")
	);

	Add_Reference(
		"Quinn, P.F., Beven, K.J., Chevallier, P., Planchon, O.", "1991",
		"The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models",
		"Hydrological Processes, 5:59-79",
		SG_T("http://onlinelibrary.wiley.com/doi/10.1002/hyp.3360050106/full")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"WIDTH"	, _TL("Flow Width"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"TCA"	, _TL("Total Catchment Area (TCA)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"SCA"	, _TL("Specific Catchment Area (SCA)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD", _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Deterministic 8"),
			_TL("Multiple Flow Direction (Quinn et al. 1991)"),
			_TL("Aspect")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_Width::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TCA") )
	{
		pParameters->Set_Enabled("SCA", pParameter->asGrid() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Width::On_Execute(void)
{
	CSG_Grid	*pWidth, *pTCA, *pSCA;

	//-----------------------------------------------------
	m_pDEM	= Parameters("DEM"  )->asGrid();
	pWidth	= Parameters("WIDTH")->asGrid();
	pTCA	= Parameters("TCA"  )->asGrid();
	pSCA	= Parameters("SCA"  )->asGrid();

	if( !pTCA )
	{
		pSCA	= NULL;
	}

	DataObject_Set_Colors(pSCA, 11, SG_COLORS_YELLOW_BLUE);

	int	Method	= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Width;

			switch( Method )
			{
			case  0: Width = Get_D8    (x, y); break;	// Deterministic 8
			case  1: Width = Get_MFD   (x, y); break;	// Multiple Flow Direction (Quinn et al. 1991)
			default: Width = Get_Aspect(x, y); break;	// Aspect
			}

			if( Width > 0.0 )
			{
				pWidth->Set_Value(x, y, Width);

				if( pSCA )
				{
					if( !pTCA->is_NoData(x, y) )
					{
						pSCA->Set_Value(x, y, pTCA->asDouble(x, y) / Width);
					}
					else
					{
						pSCA->Set_NoData(x, y);
					}
				}
			}
			else
			{
				pWidth->Set_NoData(x, y);

				if( pSCA )
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
