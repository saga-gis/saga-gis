
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
		SG_T("https://www.uni-frankfurt.de/81804377/Friedrich_1996_FGA21.pdf"), SG_T("online")
	);

	Add_Reference("Friedrich, K.", "1998",
		"Multivariate distance methods for geomorphographic relief classification",
		"In: Heinecke, H., Eckelmann, W., Thomasson, A., Jones, J., Montanarella, L., Buckley, B. (eds.): "
		"Land Information Systems - Developments for planning the sustainable use of land resources. "
		"European Soil Bureau - Research Report 4, EUR 17729 EN, Office for official publications of the European Communities, Ispra, pp. 259-266.",
		SG_T("https://esdac.jrc.ec.europa.eu/ESDB_Archive/eusoils_docs/esb_rr/n04_land_information_systems/contents.html"), SG_T("online")
	//	SG_T("https://esdac.jrc.ec.europa.eu/ESDB_Archive/eusoils_docs/esb_rr/n04_land_information_systems/4_4.doc"), SG_T("online")
	);

	Add_Reference("Moeller, M., Volk, M., Friedrich, K., Lymburner, L.", "2008",
		"Placing soil-genesis and transport processes into a landscape context: A multiscale terrain-analysis approach",
		"Journal of Plant Nutrition and Soil Science, 171, pp. 419-430.",
		SG_T("https://doi.org/10.1002/jpln.200625039"), SG_T("doi:10.1002/jpln.200625039")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

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
	CSG_Grid *pDEM  = Parameters("DEM"   )->asGrid();
	CSG_Grid *pHRel = Parameters("HREL"  )->asGrid();
	CSG_Grid *pMBI  = Parameters("MBI"   )->asGrid();

	double TSlope   = Parameters("TSLOPE")->asDouble();
	double TCurve   = Parameters("TCURVE")->asDouble();
	double THRel    = Parameters("THREL" )->asDouble();

	DataObject_Set_Colors(pMBI, 11, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Slope, Curve;

			if( (!pHRel || !pHRel->is_NoData(x, y)) && Get_Morphometry(x, y, pDEM, Slope, Curve) )
			{
				Slope	= Get_Transformed(Slope, TSlope);
				Curve	= Get_Transformed(Curve, TCurve);

				if( pHRel )
				{
					double	HRel	= Get_Transformed(pHRel->asDouble(x, y), THRel);

					pMBI->Set_Value(x, y, Curve < 0.
						? Curve * (1. - Slope) * (1. - HRel)
						: Curve * (1. + Slope) * (1. + HRel)
					);
				}
				else
				{
					pMBI->Set_Value(x, y, Curve < 0.
						? Curve * (1. - Slope)
						: Curve * (1. + Slope)
					);
				}
			}
			else
			{
				pMBI->Set_NoData(x, y);
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
	t	+= fabs(x); return( t > 0. ? x / t : 0. );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMass_Balance_Index::Get_Morphometry(int x, int y, CSG_Grid *pDEM, double &Slope, double &Curve)
{
	if( !pDEM->is_InGrid(x, y) )
	{
		Slope	= 0.;
		Curve	= 0.;

		return( false );
	}

	//-----------------------------------------------------
	double	zm[8], z = pDEM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int ix = Get_xTo(i, x);
		int iy = Get_yTo(i, y);

		if( pDEM->is_InGrid(ix, iy) )
		{
			zm[i]	= pDEM->asDouble(ix, iy) - z;
		}
		else
		{
			ix = Get_xFrom(i, x);
			iy = Get_yFrom(i, y);

			if( pDEM->is_InGrid(ix, iy) )
			{
				zm[i]	= z - pDEM->asDouble(ix, iy);
			}
			else
			{
				zm[i]	= 0.;
			}
		}
	}

	//-----------------------------------------------------
	double D = ((zm[0] + zm[4]) / 2.) / Get_Cellarea();
	double E = ((zm[2] + zm[6]) / 2.) / Get_Cellarea();
//	double F =  (zm[5] - zm[7] - zm[3] + zm[1]) / (4. * Get_Cellarea());
	double G =  (zm[0] - zm[4]) / (2. * Get_Cellsize());
	double H =  (zm[2] - zm[6]) / (2. * Get_Cellsize());

	Slope	= atan(sqrt(G*G + H*H));
	Curve	= -2. * (E + D);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
