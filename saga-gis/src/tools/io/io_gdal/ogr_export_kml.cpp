/**********************************************************
 * Version $Id: ogr_export_kml.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  ogr_export_kml.cpp                   //
//                                                       //
//            Copyright (C) 2012 O. Conrad               //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "ogr_export_kml.h"

#include <cpl_string.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_Export_KML::COGR_Export_KML(void)
{
	Set_Name		(_TL("Export Shapes to KML"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		"This tool exports a vector layer to a Google Earth KML Format using "
		"Frank Warmerdam's \"Geospatial Data Abstraction Library\" (GDAL/OGR). "
		"Output file will automatically re-projected to geographic coordinates "
		"if necessary and possible. "
		"For more information on GDAL/OGR have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format(
			"%s|*.kml;*.kml|%s|*.*",
			_TL("KML files (*.kml)"),
			_TL("All Files")
		), NULL, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COGR_Export_KML::On_Execute(void)
{
	CSG_Shapes	Shapes, *pShapes	= Parameters("SHAPES")->asShapes();

	//-----------------------------------------------------
	if( pShapes->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Undefined )
	{
		Message_Add(_TL("layer uses undefined coordinate system, assuming geographic coordinates"));
	}
	else if( pShapes->Get_Projection().Get_Type() != SG_PROJ_TYPE_CS_Geographic )
	{
		Message_Add(CSG_String::Format("\n%s (%s: %s)\n", _TL("re-projection to geographic coordinates"), _TL("original"), pShapes->Get_Projection().Get_Name().c_str()), false);

		bool	bResult;

		SG_RUN_TOOL(bResult, "pj_proj4", 2,
				SG_TOOL_PARAMETER_SET("SOURCE"   , pShapes)
			&&	SG_TOOL_PARAMETER_SET("TARGET"   , &Shapes)
			&&	SG_TOOL_PARAMETER_SET("CRS_PROJ4", SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
		);

		if( bResult )
		{
			pShapes	= &Shapes;

			Message_Add(CSG_String::Format("\n%s: %s\n", _TL("re-projection"), _TL("success")), false);
		}
		else
		{
			Message_Add(CSG_String::Format("\n%s: %s\n", _TL("re-projection"), _TL("failed" )), false);
		}
	}

	//-----------------------------------------------------
	CSG_OGR_DataSet	DataSource;

	if( !DataSource.Create(Parameters("FILE")->asString(), "KML") )
	{
		Error_Set(_TL("KML file creation failed"));

		return( false );
	}

	if( !DataSource.Write(pShapes) )
	{
		Error_Set(_TL("failed to store data"));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
