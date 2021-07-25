/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Filter_Terrain_SlopeBased.cpp            //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Filter_Terrain_SlopeBased.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Terrain_SlopeBased::CFilter_Terrain_SlopeBased(void)
{

	//-----------------------------------------------------
	Set_Name(_TL("DTM Filter (slope-based)"));

	Set_Author(_TL("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The tool can be used to filter a digital elevation model in order to classify "
		"its cells into bare earth and object cells (ground and non-ground cells).\n"
		"The tool uses concepts as described by Vosselman (2000) and is based on the "
		"assumption that a large height difference between two nearby cells is unlikely "
		"to be caused by a steep slope in the terrain. The probability that the higher cell "
		"might be non-ground increases when the distance between the two cells decreases. "
		"Therefore the filter defines a maximum height difference (dz_max) between two cells as "
		"a function of the distance (d) between the cells (dz_max(d) = d). A cell is classified "
		"as terrain if there is no cell within the kernel radius to which the height difference "
		"is larger than the allowed maximum height difference at the distance between these two cells.\n"
		"The approximate terrain slope (s) parameter is used to modify the filter function to match "
		"the overall slope in the study area (dz_max(d) = d * s).\n"
		"A 5% confidence interval (ci = 1.65 * sqrt(2 * stddev)) may be used to modify the "
		"filter function even further by either relaxing (dz_max(d) = d * s + ci) or amplifying "
		"(dz_max(d) = d * s - ci) the filter criterium.\n\n")
	);

	Add_Reference("Vosselman, G.", "2000",
		"Slope based filtering of laser altimetry data",
		"IAPRS, Vol. XXXIII, Part B3, Amsterdam, The Netherlands, 935-942"
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT", _TL("DEM"),
		_TL("The grid to filter."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"GROUND", _TL("Bare Earth"),
		_TL("The filtered DEM containing only cells classified as ground."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"NONGROUND", _TL("Removed Objects"),
		_TL("The non-ground objects removed by the filter."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Int("",
		"RADIUS", _TL("Kernel Radius"),
		_TL("The radius of the filter kernel [grid cells]. Must be large enough to reach ground cells next to non-ground objects."),
		5, 1, true
	);

	Parameters.Add_Double("",
		"TERRAINSLOPE", _TL("Terrain Slope [%]"),
		_TL("The approximate terrain slope [%]. Used to relax the filter criterium in steeper terrain."),
		30.0, 0.0, true
	);

	Parameters.Add_Choice("",
		"FILTERMOD", _TL("Filter Modification"),
		_TL("Choose whether to apply the filter kernel without modification or to use a confidence interval to relax or amplify the height criterium."),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
			_TL("relax filter"),
			_TL("amplify filter")
		), 0
	);

	Parameters.Add_Double("",
		"STDDEV", _TL("Standard Deviation"),
		_TL("The standard deviation used to calculate a 5% confidence interval applied to the height threshold [map units]."),
		0.1, 0.0, true
	);
}

//---------------------------------------------------------
CFilter_Terrain_SlopeBased::~CFilter_Terrain_SlopeBased(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Terrain_SlopeBased::On_Execute(void)
{
	CSG_Grid	*pInput			= Parameters("INPUT")->asGrid();
	CSG_Grid	*pGround		= Parameters("GROUND")->asGrid();
	CSG_Grid	*pNonGround		= Parameters("NONGROUND")->asGrid();
	int			iRadius			= Parameters("RADIUS")->asInt() + 1;
	double		dTerrainSlope	= Parameters("TERRAINSLOPE")->asDouble() / 100.0;
    int			iFilterMod		= Parameters("FILTERMOD")->asInt();
	double		dStdDev			= Parameters("STDDEV")->asDouble();

	
	//---------------------------------------------------------
	pGround->Assign_NoData();
	
	if( pNonGround != NULL )
	{
		pNonGround->Assign_NoData();
	}


	//---------------------------------------------------------
	// create kernel with maximum height difference
		
	CSG_Grid_Radius		Kernel;		Kernel.Create(iRadius);
	std::vector<double>	dzKernel(Kernel.Get_nPoints());

	{
		int x, y;

		for(int iPoint=0; iPoint<Kernel.Get_nPoints(); iPoint++)
		{
			double dDist = Kernel.Get_Point(iPoint, x, y);

			switch (iFilterMod)
			{
			default:
			case 0:		dzKernel[iPoint] = dDist * dTerrainSlope;								break;
			case 1:		dzKernel[iPoint] = dDist * dTerrainSlope + 1.65 * sqrt(2 * dStdDev);	break;
			case 2:		double dz = dDist * dTerrainSlope - 1.65 * sqrt(2 * dStdDev);
						dz > 0.0 ? dzKernel[iPoint] = dz : dzKernel[iPoint] = 0.0;
						break;
			}
		}
	}


	//---------------------------------------------------------
	// filter

	for(int y=0; y<pInput->Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pInput->Get_NX(); x++ )
		{
			if( pInput->is_NoData(x, y) )
			{
				continue;
			}

			int		ix, iy;
			bool	bNonGround = false;

			for(int iPoint=1; iPoint<Kernel.Get_nPoints(); iPoint++)	// iPoint=0 is the central cell
			{
				Kernel.Get_Point(iPoint, ix, iy);						// ix, iy are the offset to the neighbor cell

				ix += x;
				iy += y;

				if( pInput->is_InGrid(ix, iy, true) )
				{
					double dz = pInput->asDouble(x, y) - pInput->asDouble(ix, iy);

					if( dz > 0.0 && dz > dzKernel[iPoint] )
					{
						bNonGround = true;
						break;
					}
				}
			}

			if( !bNonGround )
			{
				pGround->Set_Value(x, y, pInput->asDouble(x, y));
			}
			else if( pNonGround != NULL )
			{
				pNonGround->Set_Value(x, y, pInput->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


//---------------------------------------------------------
int CFilter_Terrain_SlopeBased::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if (pParameter->Cmp_Identifier("FILTERMOD"))
	{
		pParameters->Get_Parameter("STDDEV")->Set_Enabled(pParameter->asInt() > 0);
	}

	//-----------------------------------------------------
	return (1);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
