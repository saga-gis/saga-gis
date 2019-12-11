
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
//                 mass_balance_index.cpp                //
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
#include "mass_balance_index.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MBI_LEVEL_2


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMass_Balance_Index::CMass_Balance_Index(void)
{
	Set_Name		(_TL("Mass Balance Index"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"A mass balance index. "
	));

	Add_Reference("Friedrich, K.", "1996",
		"Digitale Reliefgliederungsverfahren zur Ableitung bodenkundlich relevanter Flaecheneinheiten",
		"Frankfurter Geowissenschaftliche Arbeiten D 21, Frankfurt/M.",
		SG_T("http://user.uni-frankfurt.de/~relief/fga21/"), SG_T("online")
	);

	Add_Reference("Friedrich, K.", "1998",
		"Multivariate distance methods for geomorphographic relief classification",
		"In: Heinecke, H., Eckelmann, W., Thomasson, A., Jones, J., Montanarella, L., Buckley, B. (eds.): "
		"Land Inforamtion Systems - Developments for planning the sustainable use of land resources. "
		"European Soil Bureau - Research Report 4, EUR 17729 EN, Office for oficial publications of the European Communities, Ispra, pp. 259-266.",
		SG_T("http://eusoils.jrc.it/ESDB_Archive/eusoils_docs/esb_rr/n04_land_information_systems/contents.html"), SG_T("online")
	);

	Add_Reference("Moeller, M., Volk, M., Friedrich, K., Lymburner, L.", "2008",
		"Placing soil-genesis and transport processes into a landscape context: A multiscale terrain-analysis approach",
		"Journal of Plant Nutrition and Soil Science, 171, pp. 419-430, DOI: 10.1002/jpln.200625039."
	);

	//-----------------------------------------------------
#ifdef MBI_LEVEL_2
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);
#else
	Parameters.Add_Grid(
		"", "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "CURVE"		, _TL("Curvature"),
		_TL(""),
		PARAMETER_INPUT
	);
#endif
	Parameters.Add_Grid(
		"", "HREL"		, _TL("Vertical Distance to Channel Network"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "MBI"		, _TL("Mass Balance Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		"", "TSLOPE"	, _TL("T Slope"),
		_TL(""),
		15., 0., true
	);

	Parameters.Add_Double(
		"", "TCURVE"	, _TL("T Curvature"),
		_TL(""),
		0.01, 0., true
	);

	Parameters.Add_Double(
		"", "THREL"		, _TL("T Vertical Distance to Channel Network"),
		_TL(""),
		15., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMass_Balance_Index::On_Execute(void)
{
	int			x, y;
	double		TSlope, TCurve, THRel, fSlope, fCurve, fHRel;

	//-----------------------------------------------------
#ifdef MBI_LEVEL_2
	CSG_Grid	*pDEM		= Parameters("DEM"  )->asGrid();
#else
	CSG_Grid	*pSlope		= Parameters("SLOPE")->asGrid();
	CSG_Grid	*pCurve		= Parameters("CURVE")->asGrid();
#endif
	CSG_Grid	*pHRel		= Parameters("HREL" )->asGrid();
	CSG_Grid	*pMBI		= Parameters("MBI"  )->asGrid();

	TSlope		= Parameters("TSLOPE")->asDouble();
	TCurve		= Parameters("TCURVE")->asDouble();
	THRel		= Parameters("THREL" )->asDouble();

	//-----------------------------------------------------
	DataObject_Set_Colors(pMBI, 11, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
#ifdef MBI_LEVEL_2
			if( (pHRel == NULL || !pHRel->is_NoData(x, y)) && Get_Morphometry(x, y, pDEM, fSlope, fCurve) )
			{
#else
			if( (pHRel == NULL || !pHRel->is_NoData(x, y)) && !pSlope->is_NoData(x, y) && pCurve->is_NoData(x, y) )
			{
				fSlope	= pSlope->asDouble(x, y);
				fCurve	= pCurve->asDouble(x, y);
#endif
				if( pHRel )
				{
					fHRel	= pHRel	->asDouble(x, y);

					fSlope	= Get_Transformed(fSlope, TSlope);
					fCurve	= Get_Transformed(fCurve, TCurve);
					fHRel	= Get_Transformed(fHRel	, THRel);

					pMBI	->Set_Value(x, y, fCurve < 0.
						? fCurve * (1. - fSlope) * (1. - fHRel)
						: fCurve * (1. + fSlope) * (1. + fHRel)
					);
				}
				else
				{
					fSlope	= Get_Transformed(fSlope, TSlope);
					fCurve	= Get_Transformed(fCurve, TCurve);

					pMBI	->Set_Value(x, y, fCurve < 0.
						? fCurve * (1. - fSlope)
						: fCurve * (1. + fSlope)
					);
				}
			}
			else
			{
				pMBI	->Set_NoData(x, y);
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
inline double CMass_Balance_Index::Get_Transformed(double x, double t)
{
	t	+= fabs(x);

	return( t > 0. ? x / t : 0. );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMass_Balance_Index::Get_Morphometry(int x, int y, CSG_Grid *pDEM, double &Slope, double &Curve)
{
	int		i, ix, iy;
	double	z, zm[8], G, H, E, D;

	if( pDEM->is_InGrid(x, y) )
	{
		z		= pDEM->asDouble(x, y);

		for(i=0; i<8; i++)
		{
			ix		= pDEM->Get_System().Get_xTo(i, x);
			iy		= pDEM->Get_System().Get_yTo(i, y);

			if( is_InGrid(ix, iy) )
			{
				zm[i]	= pDEM->asDouble(ix, iy) - z;
			}
			else
			{
				ix		= pDEM->Get_System().Get_xFrom(i, x);
				iy		= pDEM->Get_System().Get_yFrom(i, y);

				if( is_InGrid(ix, iy) )
				{
					zm[i]	= z - pDEM->asDouble(ix, iy);
				}
				else
				{
					zm[i]	= 0.;
				}
			}
		}

		D		= ((zm[0] + zm[4]) / 2.) / pDEM->Get_Cellarea();
		E		= ((zm[2] + zm[6]) / 2.) / pDEM->Get_Cellarea();
	//	F		=  (zm[5] - zm[7] - zm[3] + zm[1]) / (4. * pDEM->Get_Cellarea());
		G		=  (zm[0] - zm[4]) / (2. * pDEM->Get_Cellsize());
        H		=  (zm[2] - zm[6]) / (2. * pDEM->Get_Cellsize());

		Slope	= atan(sqrt(G*G + H*H));
		Curve	= -2. * (E + D);

		return( true );
	}

	Slope	= 0.;
	Curve	= 0.;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
