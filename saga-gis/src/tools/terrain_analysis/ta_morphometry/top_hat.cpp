/**********************************************************
 * Version $Id: top_hat.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

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
//                      top_hat.cpp                      //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//                University of Hamburg                  //
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
#include "top_hat.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTop_Hat::CTop_Hat(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Valley and Ridge Detection (Top Hat Approach)"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Calculating fuzzy valley and ridge class memberships using the Top Hat approach. "
		"Based on the AML script \'tophat\' by Jochen Schmidt, Landcare Research. "
		"\n"
		"\nReferences:\n"
		"Rodriguez, F., Maire, E., Courjault-Rad'e, P., Darrozes, J. (2002): "
		"The Black Top Hat function applied to a DEM: a tool to estimate recent incision in a mountainous watershed. "
		"(Estib`ere Watershed, Central Pyrenees). Geophysical Research Letters, 29(6), 9-1 - 9-4.\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(NULL, "DEM"       , _TL("Elevation"      ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "VALLEY"    , _TL("Valley Depth"   ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "HILL"      , _TL("Hill Height"    ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "VALLEY_IDX", _TL("Valley Index"   ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "HILL_IDX"  , _TL("Hill Index"     ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "SLOPE_IDX" , _TL("Hillslope Index"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Value(
		NULL	, "RADIUS_VALLEY"	, _TL("Valley Radius"),
		_TL("radius given in map units used to fill valleys"),
		PARAMETER_TYPE_Double, 1000.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "RADIUS_HILL"		, _TL("Hill Radius"),
		_TL("radius given in map units used to cut hills"),
		PARAMETER_TYPE_Double, 1000.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"		, _TL("Elevation Threshold"),
		_TL("elevation threshold used to identify hills/valleys"),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Slope Index"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("default"),
			_TL("alternative")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTop_Hat::On_Execute(void)
{
	int			y, Method;
	double		Threshold;

	CSG_Grid	*pDEM, *pValley, *pValley_Idx, *pHill, *pHill_Idx, *pSlope_Idx;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM"       )->asGrid();

	pValley		= Parameters("VALLEY"    )->asGrid();
	pValley_Idx	= Parameters("VALLEY_IDX")->asGrid();
	pHill		= Parameters("HILL"      )->asGrid();
	pHill_Idx	= Parameters("HILL_IDX"  )->asGrid();
	pSlope_Idx	= Parameters("SLOPE_IDX" )->asGrid();

	Threshold	= Parameters("THRESHOLD" )->asDouble();
	Method		= Parameters("METHOD"    )->asInt();

	//-----------------------------------------------------
	CSG_Grid_Cell_Addressor	rValley, rHill;

	if( !rValley.Set_Radius(Parameters("RADIUS_VALLEY")->asDouble() / Get_Cellsize()) )
	{
		Error_Set(_TL("could not initialize search engine for valleys"));

		return( false );
	}

	if( !rHill  .Set_Radius(Parameters("RADIUS_HILL"  )->asDouble() / Get_Cellsize()) )
	{
		Error_Set(_TL("could not initialize search engine for hills"));

		return( false );
	}

	//-----------------------------------------------------
	DataObject_Set_Colors(pValley    , 11, SG_COLORS_WHITE_BLUE);
	DataObject_Set_Colors(pValley_Idx, 11, SG_COLORS_WHITE_BLUE);
	DataObject_Set_Colors(pHill      , 11, SG_COLORS_WHITE_RED);
	DataObject_Set_Colors(pHill_Idx  , 11, SG_COLORS_WHITE_RED);
	DataObject_Set_Colors(pSlope_Idx , 11, SG_COLORS_WHITE_GREEN);

	//-----------------------------------------------------
	CSG_Grid	zMax(Get_System()), zMin(Get_System());

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bOkay;
			double	min, max;

			if( rValley.Get_Count() == rHill.Get_Count() )
			{
				bOkay	= Get_Focal_Extremes(x, y, *pDEM, rValley, min, max);
			}
			else
			{
				bOkay	=  Get_Focal_Minimum(x, y, *pDEM, rValley, min)
						&& Get_Focal_Maximum(x, y, *pDEM, rHill  , max);
			}

			if( bOkay )
			{
				zMin.Set_Value(x, y, min);
				zMax.Set_Value(x, y, max);
			}
			else
			{
				zMax.Set_NoData(x, y);
				zMin.Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	min, max;

			if( Get_Focal_Minimum(x, y, zMax, rValley, min)
			&&  Get_Focal_Maximum(x, y, zMin, rHill  , max) )
			{
				double	z		= pDEM->asDouble(x, y);

				// VALLEY = max(0, focalmin(focalmax(%IN%, CIRCLE, %VALRAD%), CIRCLE, %VALRAD%) - %IN% - %THRES%)	// TOP HAT: fill valleys
				double	zValley	= M_GET_MAX(0, min - z - Threshold);

				// HILL   = max(0, %IN% - focalmax(focalmin(%IN%, CIRCLE, %HILRAD%), CIRCLE, %HILRAD%) - %THRES%)	// TOP HAT: cut hills
				double	zHill	= M_GET_MAX(0, z - max - Threshold);

				if( pValley )	pValley    ->Set_Value(x, y, zValley);
				if( pHill   )	pHill      ->Set_Value(x, y, zHill);

				pValley_Idx->Set_Value(x, y, // valley index = con(VALLEY > 0 & HILL <= 0, 1, con(HILL > 0 & VALLEY <= 0, 0, con(VALLEY > 0 | HILL > 0, VALLEY / (HILL + VALLEY), 0)))
					zValley > 0 && zHill <= 0 ? 1 : (zHill > 0 && zValley <= 0 ? 0 : (zValley > 0 || zHill > 0 ? zValley / (zHill + zValley) : 0))
				);

				pHill_Idx  ->Set_Value(x, y, // hill index = con(VALLEY > 0 & HILL <= 0, 0, con(HILL > 0 & VALLEY <= 0, 1, con(VALLEY > 0 | HILL > 0, HILL / (HILL + VALLEY), 0)))
					zValley > 0 && zHill <= 0 ? 0 : (zHill > 0 && zValley <= 0 ? 1 : (zValley > 0 || zHill > 0 ? zHill   / (zHill + zValley) : 0))
				);

				if( pSlope_Idx )
				{
					if( Method == 0 )
					{
						pSlope_Idx->Set_Value(x, y, //	HILLSLOPE = con(VALLEY <= 0 & HILL <= 0, 1, 2 * min(VALLEY, HILL) / (HILL + VALLEY))
							zValley <= 0 && zHill <= 0 ? 1 : 2 * M_GET_MIN(zValley, zHill) / (zHill + zValley)
						);
					}
					else	// maybe better ?!
					{
						pSlope_Idx->Set_Value(x, y, // HILLSLOPE = con(VALLEY < %THRES% & hill < %THRES%, 1, sin(3.14152 * 0.5 * (1 - abs(HILLI - VALLI))))
							zValley < Threshold && zHill < Threshold ? 1 : sin(3.14152 * 0.5 * (1 - fabs(pHill_Idx->asDouble(x, y) - pValley_Idx->asDouble(x, y))))
						);
					}
				}
			}
			else
			{
				if( pValley     ) pValley    ->Set_NoData(x, y);
				if( pHill       ) pHill      ->Set_NoData(x, y);
				if( pValley_Idx ) pValley_Idx->Set_NoData(x, y);
				if( pHill_Idx   ) pHill_Idx  ->Set_NoData(x, y);
				if( pSlope_Idx  ) pSlope_Idx ->Set_NoData(x, y);
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
bool CTop_Hat::Get_Focal_Statistics(int x, int y, const CSG_Grid &g, const CSG_Grid_Cell_Addressor &r, CSG_Simple_Statistics &s)
{
	s.Invalidate();

	if( g.is_InGrid(x, y) )
	{
		for(int i=0, ix, iy; i<r.Get_Count(); i++)
		{
			if( g.is_InGrid(ix = r.Get_X(i, x), iy = r.Get_Y(i, y)) )
			{
				s.Add_Value(g.asDouble(ix, iy));
			}
		}
	}

	return( s.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CTop_Hat::Get_Focal_Minimum(int x, int y, const CSG_Grid &g, const CSG_Grid_Cell_Addressor &r, double &Minimum)
{
	CSG_Simple_Statistics	s;

	if( Get_Focal_Statistics(x, y, g, r, s) )
	{
		Minimum	= s.Get_Minimum();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CTop_Hat::Get_Focal_Maximum(int x, int y, const CSG_Grid &g, const CSG_Grid_Cell_Addressor &r, double &Maximum)
{
	CSG_Simple_Statistics	s;

	if( Get_Focal_Statistics(x, y, g, r, s) )
	{
		Maximum	= s.Get_Maximum();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CTop_Hat::Get_Focal_Extremes(int x, int y, const CSG_Grid &g, const CSG_Grid_Cell_Addressor &r, double &Minimum, double &Maximum)
{
	CSG_Simple_Statistics	s;

	if( Get_Focal_Statistics(x, y, g, r, s) )
	{
		Minimum	= s.Get_Minimum();
		Maximum	= s.Get_Maximum();

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
