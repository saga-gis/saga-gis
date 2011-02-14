
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMass_Balance_Index::CMass_Balance_Index(void)
{
	Set_Name		(_TL("Mass Balance Index"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"\n"
		"References:\n"
		"\n"
		"Friedrich, K. (1996): "
		"Digitale Reliefgliederungsverfahren zur Ableitung bodenkundlich relevanter Flaecheneinheiten. "
		"Frankfurter Geowissenschaftliche Arbeiten D 21, Frankfurt/M., "
		"<a href=\"http://user.uni-frankfurt.de/~relief/fga21/\">online</a>.\n"
		"\n"
		"Friedrich, K. (1998): "
		"Multivariate distance methods for geomorphographic relief classification. "
		"in Heinecke, H., Eckelmann, W., Thomasson, A., Jones, J., Montanarella, L., Buckley, B. (eds.): "
		"Land Inforamtion Systems - Developments for planning the sustainable use of land resources. "
		"European Soil Bureau - Research Report 4, EUR 17729 EN, Office for oficial publications of the European Communities, Ispra, pp. 259-266, "
		"<a href=\"http://eusoils.jrc.it/ESDB_Archive/eusoils_docs/esb_rr/n04_land_information_systems/contents.html\">online</a>.\n"
		"\n"
		"Moeller, M., Volk, M., Friedrich, K., Lymburner, L. (2008): "
		"Placing soil-genesis and transport processes into a landscape context: A multiscale terrain-analysis approach. "
		"Journal of Plant Nutrition and Soil Science, 171, pp. 419-430, DOI: 10.1002/jpln.200625039\n"
		"\n"
	));

	//-----------------------------------------------------
#ifdef MBI_LEVEL_2
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);
#else
	Parameters.Add_Grid(
		NULL	, "SLOPE"		, _TL("Slope"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CURVE"		, _TL("Curvature"),
		_TL(""),
		PARAMETER_INPUT
	);
#endif
	Parameters.Add_Grid(
		NULL	, "HREL"		, _TL("Vertical Distance to Channel Network"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "MBI"			, _TL("Mass Balance Index"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "TSLOPE"		, _TL("T Slope"),
		_TL(""),
		PARAMETER_TYPE_Double	, 15.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "TCURVE"		, _TL("T Curvature"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.01, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "THREL"		, _TL("T Vertical Distance to Channel Network"),
		_TL(""),
		PARAMETER_TYPE_Double	, 15.0, 0.0, true
	);
}

//---------------------------------------------------------
CMass_Balance_Index::~CMass_Balance_Index(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMass_Balance_Index::On_Execute(void)
{
	int			x, y;
	double		TSlope, TCurve, THRel, fSlope, fCurve, fHRel;

	//-----------------------------------------------------
#ifdef MBI_LEVEL_2
	CSG_Grid	*pDEM		= Parameters("DEM")		->asGrid();
#else
	CSG_Grid	*pSlope		= Parameters("SLOPE")	->asGrid();
	CSG_Grid	*pCurve		= Parameters("CURVE")	->asGrid();
#endif
	CSG_Grid	*pHRel		= Parameters("HREL")	->asGrid();
	CSG_Grid	*pMBI		= Parameters("MBI")		->asGrid();

	TSlope		= Parameters("TSLOPE")	->asDouble();
	TCurve		= Parameters("TCURVE")	->asDouble();
	THRel		= Parameters("THREL")	->asDouble();

	//-----------------------------------------------------
	DataObject_Set_Colors(pMBI, 100, SG_COLORS_RED_GREY_BLUE, true);

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

					pMBI	->Set_Value(x, y, fCurve < 0.0
						? fCurve * (1.0 - fSlope) * (1.0 - fHRel)
						: fCurve * (1.0 + fSlope) * (1.0 + fHRel)
					);
				}
				else
				{
					fSlope	= Get_Transformed(fSlope, TSlope);
					fCurve	= Get_Transformed(fCurve, TCurve);

					pMBI	->Set_Value(x, y, fCurve < 0.0
						? fCurve * (1.0 - fSlope)
						: fCurve * (1.0 + fSlope)
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CMass_Balance_Index::Get_Transformed(double x, double t)
{
	t	+= fabs(x);

	return( t > 0.0 ? x / t : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
					zm[i]	= 0.0;
				}
			}
		}

		D		= ((zm[0] + zm[4]) / 2.0) / pDEM->Get_Cellarea();
		E		= ((zm[2] + zm[6]) / 2.0) / pDEM->Get_Cellarea();
	//	F		=  (zm[5] - zm[7] - zm[3] + zm[1]) / (4.0 * pDEM->Get_Cellarea());
		G		=  (zm[0] - zm[4]) / (2.0 * pDEM->Get_Cellsize());
        H		=  (zm[2] - zm[6]) / (2.0 * pDEM->Get_Cellsize());

		Slope	= atan(sqrt(G*G + H*H));
		Curve	= -2.0 * (E + D);

		return( true );
	}

	Slope	= 0.0;
	Curve	= 0.0;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
