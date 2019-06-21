
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
//                    ogr_import.cpp                     //
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
#include "ogr_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_Import::COGR_Import(void)
{
	Set_Name	(_TL("Import Shapes"));

	Set_Author	("O.Conrad (c) 2008");

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	CSG_String	Description, Filter, Filter_All;

	Description	= _TW(
		"The \"OGR Vector Data Import\" tool imports vector data from various file/database formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_OGR_Drivers().Get_Version().c_str());

	Description	+= _TL("Following raster formats are currently supported:");

	Description	+= CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>",
		_TL("ID"), _TL("Name"), _TL("Extension")
	);

	for(int i=0; i<SG_Get_OGR_Drivers().Get_Count(); i++)
    {
		if( SG_Get_OGR_Drivers().is_Vector(i) && SG_Get_OGR_Drivers().Can_Read(i) )
		{
			CSG_String	ID		= SG_Get_OGR_Drivers().Get_Description(i).c_str();
			CSG_String	Name	= SG_Get_OGR_Drivers().Get_Name       (i).c_str();
			CSG_String	Ext		= SG_Get_OGR_Drivers().Get_Extension  (i).c_str();

			Description	+= "<tr><td>" + ID + "</td><td>" + Name + "</td><td>" + Ext + "</td></tr>";

			if( !Ext.is_Empty() )
			{
				Ext.Replace("/", ";");

				Filter		+= Name + "|*." + Ext + "|";
				Filter_All	+= (Filter_All.is_Empty() ? "*." : ";*.") + Ext;
			}
		}
    }

	Description	+= "</table>";

	Set_Description(Description);

	Filter.Prepend(CSG_String::Format("%s|%s|", _TL("All Recognized Files"), Filter_All.c_str()));
	Filter.Append (CSG_String::Format("%s|*.*", _TL("All Files")));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath("",
		"FILES"		, _TL("Files"),
		_TL(""),
		Filter, NULL, false, false, true
	);

	Parameters.Add_Choice("",
		"GEOM_TYPE"	, _TL("Geometry Type"),
		_TL("Some OGR drivers are unable to determine the geometry type automatically, please choose the appropriate one in this case"),
		SG_Get_OGR_WKB_Type_Choices(), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COGR_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Strings		Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Parameters("SHAPES")->asShapesList()->Del_Items();

	for(int iFile=0; iFile<Files.Get_Count(); iFile++)
	{
		Message_Fmt("\n%s: %s", _TL("loading"), Files[iFile].c_str());

		CSG_OGR_DataSet	DataSource;

		if( !DataSource.Create(Files[iFile]) )
		{
			Message_Add(_TL("could not open data source"));
		}
		else if( DataSource.Get_Count() <= 0 )
		{
			Message_Add(_TL("no layers in data source"));
		}
		else for(int iLayer=0; iLayer<DataSource.Get_Count(); iLayer++)
		{
			CSG_Shapes	*pShapes	= DataSource.Read(iLayer, Parameters("GEOM_TYPE")->asInt());

			if( pShapes )
			{
				Parameters("SHAPES")->asShapesList()->Add_Item(pShapes);

				CSG_String	Name	= pShapes->Get_Name();

				pShapes->Get_MetaData().Add_Child("GDAL_DRIVER", DataSource.Get_DriverID());
				pShapes->Set_File_Name(Files[iFile]);
				pShapes->Set_Description(DataSource.Get_Description(iLayer));

				if( Name.is_Empty() )
				{
					pShapes->Set_Name(SG_File_Get_Name(Files[iFile], false) + (DataSource.Get_Count() == 1 ? CSG_String("") : CSG_String::Format(" [%d]", 1 + iLayer)));
				}
				else
				{
					pShapes->Set_Name(Name);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Parameters("SHAPES")->asShapesList()->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_OGR_Import	(const CSG_String &File_Name)
{
	COGR_Import	Import;

	if(	!Import.Get_Parameters()->Set_Parameter("FILES", File_Name, PARAMETER_TYPE_FilePath) )
	{
		return( false );
	}

	if(	!Import.Execute() )
	{
		return( false );
	}

	CSG_Parameter_Shapes_List	*pShapes	= Import.Get_Parameters()->Get_Parameter("SHAPES")->asShapesList();

	for(int i=0; i<pShapes->Get_Item_Count(); i++)
	{
		SG_UI_DataObject_Add(pShapes->Get_Shapes(i), SG_UI_DATAOBJECT_UPDATE_ONLY);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
