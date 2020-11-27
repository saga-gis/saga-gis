
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
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

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	CSG_String	Description, Formats, Filter;

	Description	= _TW(
		"The \"OGR Vector Data Export\" tool exports vector data to various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
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
	Parameters.Add_Shapes("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL(""),
		Filter, NULL, true
	);

	Parameters.Add_Choice("",
		"FORMAT"	, _TL("Format"),
		_TL(""),
		Formats
	);

    Parameters.Add_String("",
        "OPTIONS"	, _TL("Creation Options"),
        _TL("The dataset creation options. A space separated list of key-value pairs (K=V)."), _TL("")		
    );

    Parameters.Add_String("",
        "LAYER_OPTIONS"	, _TL("Layer Creation Options"),
        _TL("The layer creation options. A space separated list of key-value pairs (K=V)."), _TL("")		
    );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COGR_Export::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( has_GUI() && pParameter->Cmp_Identifier("SHAPES") && pParameter->asShapes() )
	{
		CSG_String	Path(SG_File_Get_Path((*pParameters)["FILE"].asString()));

		CSG_String	Extension(SG_Get_OGR_Drivers().Get_Extension((*pParameters)("FORMAT")->asChoice()->Get_Data()));

		pParameters->Set_Parameter("FILE", SG_File_Make_Path(Path, pParameter->asShapes()->Get_Name(), Extension));
	}

	if( has_GUI() && pParameter->Cmp_Identifier("FORMAT") )
	{
		CSG_String	File((*pParameters)["FILE"].asString());

		if( !File.is_Empty() )
		{
			SG_File_Set_Extension(File, SG_Get_OGR_Drivers().Get_Extension((*pParameters)("FORMAT")->asChoice()->Get_Data()));

			pParameters->Set_Parameter("FILE", File);
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COGR_Export::On_Execute(void)
{
	CSG_OGR_DataSet	DataSource;

	CSG_String Driver;

#ifndef GDAL_V2_0_OR_NEWER
	Driver	= Parameters("FORMAT")->asString();
#else
	if( !Parameters("FORMAT")->asChoice()->Get_Data(Driver) )
	{
		return( false );
	}
#endif

	if( !DataSource.Create(Parameters("FILE")->asString(), Driver, Parameters("OPTIONS")->asString()) )
	{
		Error_Set(_TL("data set creation failed"));

		return( false );
	}

	if( !DataSource.Write(Parameters("SHAPES")->asShapes(), Parameters("LAYER_OPTIONS")->asString()) )
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
