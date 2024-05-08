
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  crs_definition.cpp                   //
//                                                       //
//                 Copyright (C) 2024 by                 //
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
#if PROJ_VERSION_MAJOR >= 6

//---------------------------------------------------------
#include "crs_definition.h"
#include "crs_transform.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCRS_Definition::CCRS_Definition(void)
{
	Set_Name		(_TL("Coordinate Reference System Format Conversion"));

	Set_Author		("O.Conrad (c) 2024");

	Set_Description	(_TW(
		"This tool allows you to define the Coordinate Reference System (CRS) "
		"for the supplied data sets. The tool applies no transformation to "
		"the data sets, it just updates their CRS metadata.\n"
		"A complete and correct description of the CRS of a dataset is necessary "
		"in order to be able to actually apply a projection with one of the "
		"'Coordinate Transformation' tools."
	));

	//-----------------------------------------------------
	Parameters.Add_String("",
		"DEFINITION", _TL("Definition"),
		_TL(""),
		"+proj=longlat +datum=WGS84 +no_defs +type=crs"
	);

	Parameters.Add_Info_String("", "PROJ", _TL("PROJ" ), _TL(""), "", false);
	Parameters.Add_Info_String("", "WKT1", _TL("WKT-1"), _TL(""), "",  true);
	Parameters.Add_Info_String("", "WKT2", _TL("WKT-2"), _TL(""), "",  true);
	Parameters.Add_Info_String("", "ESRI", _TL("ESRI" ), _TL(""), "", false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Definition::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DEFINITION") )
	{
		CSG_String PROJ, WKT1, WKT2, ESRI;

		CSG_CRSProjector::Convert_CRS_Format(pParameter->asString(), &PROJ, &WKT1, &WKT2, &ESRI);

		pParameters->Set_Parameter("PROJ", PROJ);
		pParameters->Set_Parameter("WKT1", WKT1);
		pParameters->Set_Parameter("WKT2", WKT2);
		pParameters->Set_Parameter("ESRI", ESRI);
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Definition::On_Execute(void)
{
	CSG_String PROJ, WKT1, WKT2, ESRI;

	if( CSG_CRSProjector::Convert_CRS_Format(Parameters("DEFINITION")->asString(), &PROJ, &WKT1, &WKT2, &ESRI) )
	{
		Set_Parameter("PROJ", PROJ);
		Set_Parameter("WKT1", WKT1);
		Set_Parameter("WKT2", WKT2);
		Set_Parameter("ESRI", ESRI);

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
#endif // #if PROJ_VERSION_MAJOR >= 6
