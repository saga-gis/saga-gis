
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                     CliffMetrics                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   CliffMetrics.cpp                    //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "CliffMetrics_tool.h"

#include "CliffMetrics/delineation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCliffMetrics_Tool::CCliffMetrics_Tool(void)
{
	Set_Name		(_TL("CliffMetrics"));

	Set_Author		("SAGA Interface by O.Conrad (c) 2019, CliffMetrics by Payo et al. (c) 2018");

	Set_Description	(_TW(
		"CliffMetrics (Automatic Cliff Metrics delineation) delineates the location of the "
		"coastline, coastline normals, and cliff top and toe location along these normals. "
	));

	Add_Reference("Payo, A., Jigena Antelo, B., Hurst, M., Palaseanu-Lovejoy, M., Williams, C., Jenkins, G., Lee, K., Favis-Mortlock, D., Barkwith, A., Ellis, M.A.", "2018",
		"Development of an automatic delineation of cliff top and toe on very irregular planform coastlines (CliffMetrics v1.0)",
		"Geoscientific Model Development 11, 4317–4337.",
		SG_T("https://doi.org/10.5194/gmd-11-4317-2018"), SG_T("doi: 10.5194/gmd-11-4317-2018.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"				, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SEDIMENT_TOP"		, _TL("Sediment Top Elevation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"RASTER_COAST"		, _TL("Coastline"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid("",
		"RASTER_NORMAL"		, _TL("Normals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Int
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"COAST_INITIAL"		, _TL("Initial Coastline Points"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice("COAST_INITIAL",
		"CoastSeaHandiness"	, _TL("Sea handiness"),
		_TL("which side of shoreline the sea is?"),
		CSG_String::Format("%s|%s",
			_TL("right"),
			_TL("left")
		)
	);

	Parameters.Add_Shapes("",
		"COAST"				, _TL("Coastline"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"NORMALS"			, _TL("Coastline-Normal Profiles"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"CLIFF_TOP"			, _TL("Cliff Top Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"CLIFF_TOE"			, _TL("Cliff Toe Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"COAST_POINT"		, _TL("Coast Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"INVALID_NORMALS"	, _TL("Invalid Coastline-Normal Profiles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("",
		"COAST_CURVATURE"	, _TL("Coastline Curvature"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table("",
		"PROFILES"			, _TL("Profile Data"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"StillWaterLevel"	, _TL("Still Water Level"),
		_TL("Still water level (m) used to extract the shoreline."),
		1., 0., true
	);

	Parameters.Add_Choice("",
		"CoastSmooth"		, _TL("Coastline Smoothing Algorithm"),
		_TL("Vector coastline smoothing algorithm."),
		CSG_String::Format("%s|%s|%s",
			_TL("none"),
			_TL("running mean"),
			_TL("Savitsky-Golay")
		), 1
	);

	Parameters.Add_Int("",
		"CoastSmoothWindow"	, _TL("Coastline Smoothing Window Size"),
		_TL("Size of coastline smoothing window, resulting kernel size will be: 1 + 2 * size."),
		30, 1, true
	);

	Parameters.Add_Int("",
		"SavGolCoastPoly"	, _TL("Polynomial Order for Savitsky-Golay"),
		_TL("Order of coastline profile smoothing polynomial for Savitsky-Golay smoothing: usually 2 or 4, max is 6."),
		4, 1, true, 6, true
	);

	Parameters.Add_Bool("",
		"ScaleRasterOutput"	, _TL("Scale Raster Output Values"),
		_TL("If needed, scale GIS raster output values."),
		true
	);

	Parameters.Add_Bool("",
		"RandomCoastEdgeSearch", _TL("Random Edge for Coastline Search"),
		_TL("Random edge for coastline search."),
		true
	);

	Parameters.Add_Double("",
		"CoastNormalLength"	, _TL("Length of Coastline Normals"),
		_TL("Length of coastline normals (m)."),
		500., 0.01, true
	);

	Parameters.Add_Double("",
		"EleTolerance"		, _TL("Vertical Tolerance"),
		_TL("Vertical tolerance to avoid false cliff tops and toes."),
		0.5, 0.01, true
	);

	Parameters.Add_FilePath("",
		"OutPath"			, _TL("Main Output File Directory"),
		_TL(""),
		NULL, NULL, true, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCliffMetrics_Tool::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("COAST_INITIAL") )
	{
		pParameters->Set_Enabled("CoastSeaHandiness", pParameter->asShapes() != NULL);
	}

	if( pParameter->Cmp_Identifier("CoastSmooth") )
	{
		pParameters->Set_Enabled("CoastSmoothWindow", pParameter->asInt() != 0);
		pParameters->Set_Enabled("SavGolCoastPoly"  , pParameter->asInt() == 2);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCliffMetrics_Tool::On_Execute(void)
{
	CDelineation	Delineation;

	int	nRtn	= Delineation.nDoDelineation(&Parameters);

	Delineation.DoDelineationEnd(nRtn);

	return( nRtn == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
