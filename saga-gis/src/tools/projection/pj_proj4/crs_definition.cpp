
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
		"epsg:4326"
	);

	Parameters.Add_Info_String("", "PROJ", _TL("PROJ" ), _TL(""), "", false);
	Parameters.Add_Info_String("", "WKT1", _TL("WKT-1"), _TL(""), "",  true);
	Parameters.Add_Info_String("", "WKT2", _TL("WKT-2"), _TL(""), "",  true);
	Parameters.Add_Info_String("", "JSON", _TL("JSON" ), _TL(""), "",  true);
	Parameters.Add_Info_String("", "ESRI", _TL("ESRI" ), _TL(""), "", false);

	Parameters.Add_Bool("", "MULTILINE" , _TL("Multiline" ), _TL("applies to JSON and WKT"), true);
	Parameters.Add_Bool("", "SIMPLIFIED", _TL("Simplified"), _TL("applies to WKT-2"       ), true);

	On_Parameter_Changed(&Parameters, Parameters("DEFINITION"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCRS_Definition::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DEFINITION") || pParameter->Cmp_Identifier("MULTILINE") || pParameter->Cmp_Identifier("SIMPLIFIED") )
	{
		CSG_String Definition((*pParameters)["DEFINITION"].asString());

		bool bMultiLine = (*pParameters)["MULTILINE"].asBool(), bSimplified = (*pParameters)["SIMPLIFIED"].asBool();

		pParameters->Set_Parameter("PROJ", CSG_CRSProjector::Convert_CRS_To_PROJ(Definition));
		pParameters->Set_Parameter("ESRI", CSG_CRSProjector::Convert_CRS_To_ESRI(Definition));
		pParameters->Set_Parameter("JSON", CSG_CRSProjector::Convert_CRS_To_JSON(Definition, bMultiLine));
		pParameters->Set_Parameter("WKT1", CSG_CRSProjector::Convert_CRS_To_WKT1(Definition, bMultiLine));
		pParameters->Set_Parameter("WKT2", CSG_CRSProjector::Convert_CRS_To_WKT2(Definition, bMultiLine, bSimplified));
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRS_Definition::On_Execute(void)
{
	CSG_String Definition(Parameters["DEFINITION"].asString());

	bool bMultiLine = Parameters["MULTILINE"].asBool(), bSimplified = Parameters["SIMPLIFIED"].asBool();

	Set_Parameter("PROJ", CSG_CRSProjector::Convert_CRS_To_PROJ(Definition));
	Set_Parameter("ESRI", CSG_CRSProjector::Convert_CRS_To_ESRI(Definition));
	Set_Parameter("JSON", CSG_CRSProjector::Convert_CRS_To_JSON(Definition, bMultiLine));
	Set_Parameter("WKT1", CSG_CRSProjector::Convert_CRS_To_WKT1(Definition, bMultiLine));
	Set_Parameter("WKT2", CSG_CRSProjector::Convert_CRS_To_WKT2(Definition, bMultiLine, bSimplified));

	return( SG_STR_LEN(Parameters["WKT2"].asString()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #if PROJ_VERSION_MAJOR >= 6
