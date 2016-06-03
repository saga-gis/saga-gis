/**********************************************************
 * Version $Id: ogr_export.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                    ogr_export.cpp                     //
//                                                       //
//            Copyright (C) 2008 O. Conrad               //
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
#include "ogr_export.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_Export::COGR_Export(void)
{
	Set_Name	(_TL("Export Shapes"));

	Set_Author	("O.Conrad (c) 2008");

	CSG_String	Description, Formats, Filter;

	Description	= _TW(
		"The \"OGR Vector Data Export\" module exports vector data to various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_OGR_Drivers().Get_Version().c_str());

	Description	+= _TL("Following vector formats are currently supported:");

	Description	+= CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",
		_TL("ID"), _TL("Name"), _TL("Extension")
	);

	Filter.Printf("%s|*.*", _TL("All Files"));

	for(int i=0; i<SG_Get_OGR_Drivers().Get_Count(); i++)
    {
		if( SG_Get_OGR_Drivers().is_Vector(i) && SG_Get_OGR_Drivers().Can_Write(i) )
		{
			CSG_String	ID		= SG_Get_OGR_Drivers().Get_Description(i).c_str();
			CSG_String	Name	= SG_Get_OGR_Drivers().Get_Name       (i).c_str();
			CSG_String	Ext		= SG_Get_OGR_Drivers().Get_Extension  (i).c_str();

			Description	+= "<tr><td>" + ID + "</td><td>" + Name + "</td><td>" + Ext + "</td></tr>";
			Formats		+= "{" + ID + "}" + Name + "|";

			if( !Ext.is_Empty() )
			{
				Ext.Replace("/", ";");

				Filter	+= "|" + Name + "|*." + Ext;
			}
		}
    }

	Description	+= "</table>";

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		Filter, NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "FORMAT"	, _TL("Format"),
		_TL(""),
		Formats
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COGR_Export::On_Execute(void)
{
	CSG_OGR_DataSource	DataSource;

	if( !DataSource.Create(Parameters("FILE")->asString(), Parameters("FORMAT")->asString()) )
	{
		Error_Set(_TL("data set creation failed"));

		return( false );
	}

	if( !DataSource.Write(Parameters("SHAPES")->asShapes()) )
	{
		Error_Set(_TL("failed to write data"));

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
