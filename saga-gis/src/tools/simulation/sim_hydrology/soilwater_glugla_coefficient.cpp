
///////////////////////////////////////////////////////////
//                                                       //
//                        SAGA                           //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Tool Library                       //
//                    sim_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//           soilwater_glugla_coefficient.cpp            //
//                                                       //
//              M.Bock, O.Conrad (c) 2022                //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 3 of the     //
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
#include "soilwater_glugla_coefficient.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoilWater_Glugla_Coefficient::CSoilWater_Glugla_Coefficient(void)
{
	Set_Name	(_TL("Glugla Coefficient"));

	Set_Author	("M.Bock (scilands), O.Conrad (c) 2022");

	Set_Description	(_TW(
		"Derivation of Glugla coefficient and, optionally, air capacitiy "
		"from soil texture data using a simple pedotransfer function. "
		"If one grain size fraction input is not provided its content is "
		"estimated from the contents of the other two fractions. "
	));

	Add_Reference("Braeunig, A.", "2000",
		"Entwicklung forstlich rekultivierter sowie renaturierter Boeden und Simulation ihres Wasserhaushaltes in der Mitteldeutschen Braunkohlen-Bergbaufolgelandschaft",
		"Freiberger Forschungshefte, C 489, Geooekologie.",
		SG_T("https://tu-freiberg.de/sites/default/files/media/professur-boden--und-gewaesserschutz-15982/PDF/Dissertationen/diss_braeunig.pdf"), SG_T("Dissertation.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "SAND", _TL("Sand"), _TL("Percent"), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid("", "SILT", _TL("Silt"), _TL("Percent"), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid("", "CLAY", _TL("Clay"), _TL("Percent"), PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid("",
		"AIR"	, _TL("Air Capacity"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"GLUGLA", _TL("Glugla Coefficient"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoilWater_Glugla_Coefficient::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoilWater_Glugla_Coefficient::On_Execute(void)
{
	CSG_Grid *pSand   = Parameters("SAND")->asGrid();
	CSG_Grid *pSilt   = Parameters("SILT")->asGrid();
	CSG_Grid *pClay   = Parameters("CLAY")->asGrid();

	if( (pSand ? 1 : 0) + (pSilt ? 1 : 0) + (pClay ? 1 : 0) < 2 )
	{
		Error_Set("needs at least two of the grain size fractions (sand, silt, clay)");

		return( false );
	}

	CSG_Grid *pAir    = Parameters("AIR"   )->asGrid();
	CSG_Grid *pGlugla = Parameters("GLUGLA")->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( (!pSand || !pSand->is_NoData(x, y))
			&&  (!pSilt || !pSilt->is_NoData(x, y))
			&&  (!pClay || !pClay->is_NoData(x, y)) )
			{
				double Sand = pSand ? pSand->asDouble(x, y) : 100. - (pSilt->asDouble(x, y) + pClay->asDouble(x, y));
				double Silt = pSilt ? pSilt->asDouble(x, y) : 100. - (pSand->asDouble(x, y) + pClay->asDouble(x, y));
				double Clay = pClay ? pClay->asDouble(x, y) : 100. - (pSand->asDouble(x, y) + pSilt->asDouble(x, y));

				if( Sand < 0. ) { Sand = 0.; } else if( Sand > 100. ) { Sand = 100.; }
				if( Silt < 0. ) { Silt = 0.; } else if( Silt > 100. ) { Silt = 100.; }
				if( Clay < 0. ) { Clay = 0.; } else if( Clay > 100. ) { Clay = 100.; }

				double Sum = Sand + Silt + Clay;

				if( Sum > 0. )
				{
					if( Sum != 100. )
					{
						Sand *= 100. / Sum;
						Silt *= 100. / Sum;
						Clay *= 100. / Sum;
					}

					double Air = 15.5775071730 // air capacity
						+ (0.1119881119 * Sand)
						- (0.0421496202 * Silt)
						- (0.0688017612 * Clay)
						- (0.0038780702 * Sand * Silt)
						- (0.0053858488 * Sand * Clay)
						- (0.0013682062 * Silt * Sand)
						+ (0.0000974043 * Sand * Silt * Clay);

					if( pAir    )
					{
						pAir   ->Set_Value(x, y, Air);
					}

					if( pGlugla )
					{
						pGlugla->Set_Value(x, y, 1692.6 * exp(0.0793 * Air));
					}

					continue;
				}
			}

			//---------------------------------------------
			if( pAir    )
			{
				pAir   ->Set_NoData(x, y);
			}

			if( pGlugla )
			{
				pGlugla->Set_NoData(x, y);
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
