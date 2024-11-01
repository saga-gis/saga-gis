
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
#include "Grid_Terrain_Map.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Terrain_Map::CGrid_Terrain_Map(void)
{
	Set_Name		(_TL("Terrain Map View"));

	Set_Author		("V.Wichmann (c) 2014");

	Set_Description	(_TW(
		"This tool allows one to create different terrain visualisations from an elevation dataset:\n\n"
		"* Topography: a simple map with an analytical hillshading of the terrain\n\n"
		"* Morphology: a map which visualizes the terrain by combining positive and "
		"negative openness (Yokoyama et al. 2002) with terrain slope in a single map. "
		"In contrast to conventional shading methods this has the advantage of being "
		"independent from the direction of the light source.\n"
	));

	Add_Reference(
		"Chiba, T., Kaneta, S., Suzuki, Y.", "2008",
		"Red Relief Image Map: New Visualization Method for Three Dimensional Data",
		"The International Archives of the Photogrammetry, Remote Sensing and Spatial Information Sciences. Vol. XXXVII. Part B2. Beijing, pp.1071-1076.",
		SG_T("https://www.isprs.org/proceedings/XXXVII/congress/2_pdf/11_ThS-6/08.pdf"), SG_T("online")
	);

	Add_Reference(
		"Yokoyama, R., Shirasawa, M., Pike, R.J.", "2002",
		"Visualizing topography by openness: A new application of image processing to digital elevation models",
		"Photogrammetric Engineering and Remote Sensing, Vol.68, pp.251-266.",
		SG_T("https://www.asprs.org/wp-content/uploads/pers/2002journal/march/2002_mar_257-265.pdf"), SG_T("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"          ,_TL("DEM"),
		_TL("Digital elevation model."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SHADE"        , _TL("Shade"),
		_TL("The shaded DTM."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"OPENNESS"     , _TL("Openness"),
		_TL("The difference of positive and negative openness."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"SLOPE"        , _TL("Slope"),
		_TL("The calculated terrain slope [radians]."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("",
		"CONTOURS"     , _TL("Contours"),
		_TL("The generated contour lines."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"       , _TL("Method"),
		_TL("Choose the map type to generate."),
		CSG_String::Format("%s|%s",
			_TL("Topography"),
			_TL("Morphology")
		), 0
	);

	Parameters.Add_Double("",
		"RADIUS"       , _TL("Radial Limit"),
		_TL("Radial search limit for openness calculation."),
		1000., 0., true
	);

	Parameters.Add_Bool("",
		"CONTOUR_LINES", _TL("Contour Lines"),
		_TL("Derive contour lines."),
		true
	);

	Parameters.Add_Double("CONTOUR_LINES",
		"EQUIDISTANCE" , _TL("Equidistance"),
		_TL("Contour lines equidistance [map units]."),
		50., 0., true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Terrain_Map::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("SHADE"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("OPENNESS"    , pParameter->asInt() == 1);
		pParameters->Set_Enabled("SLOPE"       , pParameter->asInt() == 1);
		pParameters->Set_Enabled("RADIUS"      , pParameter->asInt() == 1);
	}

	if(	pParameter->Cmp_Identifier("CONTOUR_LINES") )
	{
		pParameters->Set_Enabled("CONTOURS"    , pParameter->asBool());
		pParameters->Set_Enabled("EQUIDISTANCE", pParameter->asBool());
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Terrain_Map::On_Execute(void)
{
	bool bOkay = false;

	switch( Parameters("METHOD")->asInt() )
	{
	default: bOkay = Generate_Topography(); break;
	case  1: bOkay = Generate_Morphology(); break;
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


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Terrain_Map::Generate_Topography()
{
	CSG_Grid *pShade = Parameters("SHADE")->asGrid();

	if( pShade == NULL )
	{
		pShade = SG_Create_Grid(Get_System(), SG_DATATYPE_Float);
		Parameters("SHADE")->Set_Value(pShade);
		DataObject_Add(pShade);
	}

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_lighting", 0,
		   SG_TOOL_PARAMETER_SET("ELEVATION", Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("SHADE"    , pShade)
		&& SG_TOOL_PARAMETER_SET("METHOD"   , 0)
	);

	//-----------------------------------------------------
	DataObject_Set_Colors(Parameters("DEM")->asGrid(), 11, SG_COLORS_TOPOGRAPHY , false);
	DataObject_Set_Colors(pShade                     , 11, SG_COLORS_BLACK_WHITE,  true);

	DataObject_Set_Parameter(pShade, "DISPLAY_TRANSPARENCY", 40);

	pShade->Fmt_Name("%s (%s)", _TL("Shading"), Parameters("DEM")->asGrid()->Get_Name());

	DataObject_Update(Parameters("DEM")->asGrid(), SG_UI_DATAOBJECT_SHOW_MAP_NEW );
	DataObject_Update(pShade                     , SG_UI_DATAOBJECT_SHOW_MAP_LAST);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

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
		pOpenness = SG_Create_Grid(Get_System(), SG_DATATYPE_Float);
		Parameters("OPENNESS")->Set_Value(pOpenness);
		DataObject_Add(pOpenness);
	}

	if( pSlope == NULL )
	{
		pSlope = SG_Create_Grid(Get_System(), SG_DATATYPE_Float);
		Parameters("SLOPE")->Set_Value(pSlope);
		DataObject_Add(pSlope);
	}

	CSG_Grid	TMP1(Get_System(), SG_DATATYPE_Float);


	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_lighting", 5,
		   SG_TOOL_PARAMETER_SET("DEM"   , Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("POS"   , pOpenness)
		&& SG_TOOL_PARAMETER_SET("NEG"   , &TMP1)
		&& SG_TOOL_PARAMETER_SET("RADIUS", dRadius)
		&& SG_TOOL_PARAMETER_SET("METHOD", 1)
		&& SG_TOOL_PARAMETER_SET("NDIRS" , 8)
	);

	pOpenness->Subtract(TMP1);


	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("ta_morphometry", 0,
		   SG_TOOL_PARAMETER_SET("ELEVATION", Parameters("DEM"))
		&& SG_TOOL_PARAMETER_SET("SLOPE"    , pSlope)
		&& SG_TOOL_PARAMETER_SET("ASPECT"   , &TMP1)
	)


	//-----------------------------------------------------
	DataObject_Set_Colors(pOpenness, 11, SG_COLORS_BLACK_WHITE, false);
	DataObject_Set_Colors(pSlope   , 11, SG_COLORS_WHITE_RED  , false);

	DataObject_Set_Parameter(pSlope, "DISPLAY_TRANSPARENCY", 60);

	pOpenness->Fmt_Name("%s (%s)", _TL("Openness"), Parameters("DEM")->asGrid()->Get_Name());
	pSlope   ->Fmt_Name("%s (%s)", _TL("Slope"   ), Parameters("DEM")->asGrid()->Get_Name());

	DataObject_Update(pOpenness	, SG_UI_DATAOBJECT_SHOW_MAP_NEW);
	DataObject_Update(pSlope	, SG_UI_DATAOBJECT_SHOW_MAP_LAST);


	//-----------------------------------------------------
	return( true );

}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Terrain_Map::Generate_Contours()
{
	CSG_Shapes *pContours = Parameters("CONTOURS")->asShapes();

	//-----------------------------------------------------
	if( pContours == NULL )
	{
		pContours = SG_Create_Shapes(SHAPE_TYPE_Line);
		Parameters("CONTOURS")->Set_Value(pContours);
		DataObject_Add(pContours);
	}

	CSG_Grid *pGrid = Parameters("DEM")->asGrid();
	double	 zStep	= Parameters("EQUIDISTANCE")->asDouble();

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("shapes_grid", 5,
		   SG_TOOL_PARAMETER_SET("GRID"     , pGrid)
		&& SG_TOOL_PARAMETER_SET("CONTOUR"  , pContours)
		&& SG_TOOL_PARAMETER_SET("INTERVALS", 1)
    	&& SG_TOOL_PARAMETER_SET("ZMIN"     , zStep * (ceil (pGrid->Get_Min() / zStep)))
    	&& SG_TOOL_PARAMETER_SET("ZMAX"     , zStep * (floor(pGrid->Get_Max() / zStep)))
		&& SG_TOOL_PARAMETER_SET("ZSTEP"    , zStep)
	)

	//-----------------------------------------------------
	DataObject_Set_Parameter(pContours, "SINGLE_COLOR", (int)SG_COLOR_BLACK);
	DataObject_Set_Parameter(pContours, "DISPLAY_TRANSPARENCY", 70);

	pContours->Fmt_Name("%s (%s)", _TL("Contours"), Parameters("DEM")->asGrid()->Get_Name());

	DataObject_Update(pContours, SG_UI_DATAOBJECT_SHOW_MAP_LAST);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
