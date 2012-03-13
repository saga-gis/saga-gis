/**********************************************************
 * Version $Id: ogr_export_kml.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	Set_Name		(_TL("OGR: Export Shapes to KML"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"This module exports a vector layer to a Google Earth KML Format using "
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
		NULL, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.kml;*.kml|%s|*.*"),
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
	CSG_String	File_Name;
	CSG_Shapes	*pShapes, Shapes;

	pShapes		= Parameters("SHAPES")->asShapes();
	File_Name	= Parameters("FILE"  )->asString();

	//-----------------------------------------------------
	if( pShapes->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Undefined )
	{
		Message_Add(_TL("layer uses undefined coordinate system, assuming geographic coordinates"));
	}
	else if( pShapes->Get_Projection().Get_Type() != SG_PROJ_TYPE_CS_Geographic )
	{
		Message_Add(CSG_String::Format(SG_T("\n%s (%s: %s)\n"), _TL("re-projection to geographic coordinates"), _TL("original"), pShapes->Get_Projection().Get_Name().c_str()), false);

		bool	bResult;

		SG_RUN_MODULE(bResult, "pj_proj4", 2,
				pModule->Get_Parameters()->Set_Parameter(SG_T("SOURCE"), pShapes)
			&&	pModule->Get_Parameters()->Set_Parameter(SG_T("TARGET"), &Shapes)
			&&	pModule->Get_Parameters()->Set_Parameter(SG_T("CRS_PROJ4"), SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
		);

		if( bResult )
		{
			pShapes	= &Shapes;

			Message_Add(CSG_String::Format(SG_T("\n%s: %s\n"), _TL("re-projection"), _TL("success")), false);
		}
		else
		{
			Message_Add(CSG_String::Format(SG_T("\n%s: %s\n"), _TL("re-projection"), _TL("failed")), false);
		}
	}

	//-----------------------------------------------------
	CSG_OGR_DataSource	DataSource;

	if( DataSource.Create(File_Name, "KML") == false )
	{
		Message_Add(_TL("could not create KML file"));
	}
	else if( DataSource.Write(pShapes) )
	{
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
