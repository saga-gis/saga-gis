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
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Terrain_Map.cpp                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
//    e-mail:     wichmann@alps-gmbh.com                 //
//                                                       //
//    contact:    alpS GmbH                              //
//                Center for Climate Change Adaptation   //
//                Grabenweg 68                           //
//                6020 Innsbruck                         //
//                Austria                                //
//                www.alps-gmbh.com                      //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Terrain_Map.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RUN_TOOL(LIBRARY, TOOL, CONDITION)	{\
	bool	bResult;\
	SG_RUN_TOOL(bResult, LIBRARY, TOOL, CONDITION)\
	if( !bResult ) return( false );\
}

#define SET_PARAMETER(IDENTIFIER, VALUE)	pTool->Get_Parameters()->Set_Parameter(SG_T(IDENTIFIER), VALUE)


//---------------------------------------------------------
CGrid_Terrain_Map::CGrid_Terrain_Map(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Terrain Map View"));

	Set_Author(_TL("Copyrights (c) 2014 by Volker Wichmann"));

	Set_Description	(_TW(
		"This tool allows one to create different terrain visualisations from an elevation dataset:\n\n"
		"* Topography: a simple map with an analytical hillshading of the terrain\n\n"
		"* Morphology: a map which visualizes the terrain by combining positive and "
		"negative openness (Yokoyama et al. 2002) with terrain slope in a single map. "
		"In contrast to conventional shading methods this has the advantage of being "
		"independent from the direction of the light source.\n\n"
		"References:\n"
		"Yokoyama, R. / Shirasawa, M. / Pike, R.J. (2002): "
		"Visualizing topography by openness: A new application of image processing to digital elevation models. "
		"Photogrammetric Engineering and Remote Sensing, Vol.68, pp.251-266. "
		"<a target=\"_blank\" href=\"http://info.asprs.org/publications/pers/2002journal/march/2002_mar_257-265.pdf\">online at ASPRS</a>.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"		,_TL("DEM"),
		_TL("Digital elevation model."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SHADE"	, _TL("Shade"),
		_TL("The shaded DTM."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "OPENNESS"	, _TL("Openness"),
		_TL("The difference of positive and negative openness."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "SLOPE"	, _TL("Slope"),
		_TL("The calculated terrain slope [radians]."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "CONTOURS"	, _TL("Contours"),
		_TL("The generated contour lines."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	,	_TL("Method"),
		_TL("Choose the map type to generate."),
		CSG_String::Format(SG_T("%s|%s"),
			_TL("Topography"),
			_TL("Morphology")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radial Limit"),
		_TL("Radial search limit for openness calculation."),
		PARAMETER_TYPE_Double	, 1000.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "CONTOUR_LINES"	, _TL("Contour Lines"),
		_TL("Derive contour lines."),
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Value(
		Parameters("CONTOUR_LINES")	, "EQUIDISTANCE"	, _TL("Equidistance"),
		_TL("Contour lines equidistance [map units]."),
		PARAMETER_TYPE_Double	, 50.0, 0.0, true
	);
}


//---------------------------------------------------------
CGrid_Terrain_Map::~CGrid_Terrain_Map(void)
{}


//---------------------------------------------------------
int CGrid_Terrain_Map::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier(SG_T("METHOD")) )
	{
		pParameters->Get_Parameter("SHADE")			->Set_Enabled(pParameter->asInt() == 0);

		pParameters->Get_Parameter("OPENNESS")		->Set_Enabled(pParameter->asInt() == 1);
		pParameters->Get_Parameter("SLOPE")			->Set_Enabled(pParameter->asInt() == 1);
		pParameters->Get_Parameter("RADIUS")		->Set_Enabled(pParameter->asInt() == 1);
	}

	//-----------------------------------------------------
	if(	pParameter->Cmp_Identifier(SG_T("CONTOUR_LINES")) )
	{
		pParameters->Get_Parameter("CONTOURS")		->Set_Enabled(pParameter->asBool());
		pParameters->Get_Parameter("EQUIDISTANCE")	->Set_Enabled(pParameter->asBool());
	}

	//-----------------------------------------------------
	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Terrain_Map::On_Execute(void)
{
	bool	bOkay = false;

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	default:
	case 0:		bOkay = Generate_Topography();		break;
	case 1:		bOkay = Generate_Morphology();		break;
	}

	if( !bOkay )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("CONTOUR_LINES")->asBool() )
	{
		return( Generate_Contours() );
	}

	//-----------------------------------------------------
	return( true );

}


//---------------------------------------------------------
bool CGrid_Terrain_Map::Generate_Topography()
{
	CSG_Grid	*pShade;

	pShade		= Parameters("SHADE")->asGrid();


	//-----------------------------------------------------
	if( pShade == NULL )
	{
		pShade = SG_Create_Grid(*Get_System(), SG_DATATYPE_Float);
		Parameters("SHADE")->Set_Value(pShade);
		DataObject_Add(pShade);
	}


	//-----------------------------------------------------
	RUN_TOOL("ta_lighting"			, 0,
			SET_PARAMETER("ELEVATION"	, Parameters("DEM"))
		&&	SET_PARAMETER("SHADE"		, pShade)
		&&	SET_PARAMETER("METHOD"		, 0)
	)


	//-----------------------------------------------------
	DataObject_Set_Colors(Parameters("DEM")->asGrid()	, 11, SG_COLORS_TOPOGRAPHY , false);
	DataObject_Set_Colors(pShade						, 11, SG_COLORS_BLACK_WHITE, true);


	CSG_Parameters	Parms;

	if( DataObject_Get_Parameters(pShade, Parms) && Parms("DISPLAY_TRANSPARENCY") )
	{
		Parms("DISPLAY_TRANSPARENCY")->Set_Value(40);

		DataObject_Set_Parameters(pShade, Parms);
	}


	DataObject_Update(Parameters("DEM")->asGrid()	, SG_UI_DATAOBJECT_SHOW_NEW_MAP);
	DataObject_Update(pShade						, SG_UI_DATAOBJECT_SHOW_LAST_MAP);


	//-----------------------------------------------------
	return( true );
}


//---------------------------------------------------------
bool CGrid_Terrain_Map::Generate_Morphology()
{

	CSG_Grid	*pOpenness, *pSlope;
	double		dRadius;

	pOpenness	= Parameters("OPENNESS")->asGrid();
	pSlope		= Parameters("SLOPE")->asGrid();
	dRadius		= Parameters("RADIUS")->asDouble();


	//-----------------------------------------------------
	if( pOpenness == NULL )
	{
		pOpenness = new CSG_Grid(*Get_System(), SG_DATATYPE_Float);
		Parameters("OPENNESS")->Set_Value(pOpenness);
		DataObject_Add(pOpenness);
	}

	if( pSlope == NULL )
	{
		pSlope = SG_Create_Grid(*Get_System(), SG_DATATYPE_Float);
		Parameters("SLOPE")->Set_Value(pSlope);
		DataObject_Add(pSlope);
	}

	CSG_Grid	TMP1(*Get_System(), SG_DATATYPE_Float);


	//-----------------------------------------------------
	RUN_TOOL("ta_lighting"			, 5,
			SET_PARAMETER("DEM"			, Parameters("DEM"))
		&&	SET_PARAMETER("POS"			, pOpenness)
		&&	SET_PARAMETER("NEG"			, &TMP1)
		&&	SET_PARAMETER("RADIUS"		, dRadius)
		&&	SET_PARAMETER("METHOD"		, 1)
		&&	SET_PARAMETER("NDIRS"		, 8)
	)

	pOpenness->Subtract(TMP1);
	pOpenness->Set_Name(_TL("Openness"));


	//-----------------------------------------------------
	RUN_TOOL("ta_morphometry"			, 0,
			SET_PARAMETER("ELEVATION"	, Parameters("DEM"))
		&&	SET_PARAMETER("SLOPE"		, pSlope)
		&&	SET_PARAMETER("ASPECT"		, &TMP1)
	)


	//-----------------------------------------------------
	DataObject_Set_Colors(pOpenness, 11, SG_COLORS_BLACK_WHITE, false);
	DataObject_Set_Colors(pSlope   , 11, SG_COLORS_WHITE_RED  , false);

	CSG_Parameters	Parms;

	if( DataObject_Get_Parameters(pSlope, Parms) && Parms("DISPLAY_TRANSPARENCY") )
	{
		Parms("DISPLAY_TRANSPARENCY")->Set_Value(60);

		DataObject_Set_Parameters(pSlope, Parms);
	}


	DataObject_Update(pOpenness	, SG_UI_DATAOBJECT_SHOW_NEW_MAP);
	DataObject_Update(pSlope	, SG_UI_DATAOBJECT_SHOW_LAST_MAP);


	//-----------------------------------------------------
	return( true );

}


//---------------------------------------------------------
bool CGrid_Terrain_Map::Generate_Contours()
{
	CSG_Shapes	*pContours;

	pContours	= Parameters("CONTOURS")->asShapes();


	//-----------------------------------------------------
	if( pContours == NULL )
	{
		pContours = SG_Create_Shapes(SHAPE_TYPE_Line);
		Parameters("CONTOURS")->Set_Value(pContours);
		DataObject_Add(pContours);
	}


	//-----------------------------------------------------
	RUN_TOOL("shapes_grid"			, 5,
			SET_PARAMETER("GRID"		, Parameters("DEM"))
		&&	SET_PARAMETER("CONTOUR"		, pContours)
		&&	SET_PARAMETER("ZSTEP"		, Parameters("EQUIDISTANCE"))
	)


	//-----------------------------------------------------
	CSG_Parameters	Parms;

	if( DataObject_Get_Parameters(pContours, Parms) && Parms("UNISYMBOL_COLOR") && Parms("DISPLAY_TRANSPARENCY") )
	{
		Parms("UNISYMBOL_COLOR")->Set_Value(0);
		Parms("DISPLAY_TRANSPARENCY")->Set_Value(70);

		DataObject_Set_Parameters(pContours, Parms);
	}

	pContours->Set_Name("%s (%s)", _TL("Contours"), Parameters("DEM")->asGrid()->Get_Name());
	DataObject_Update(pContours, SG_UI_DATAOBJECT_SHOW_LAST_MAP);


	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
