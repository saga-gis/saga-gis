/**********************************************************
 * Version $Id$
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
#include "ogr_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COGR_Import::COGR_Import(void)
{
	Set_Name	(_TL("OGR: Import Vector Data"));

	Set_Author	(SG_T("O.Conrad (c) 2008"));

	CSG_String	Description;

	Description	= _TW(
		"The \"GDAL Vector Data Import\" module imports vector data from various file/database formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
		"\n"
		"Following vector data formats are currently supported:\n"
		"<table border=\"1\"><tr><th>Name</th><th>Description</th></tr>\n"
	);

	for(int i=0; i<SG_Get_OGR_Drivers().Get_Count(); i++)
    {
		if( SG_Get_OGR_Drivers().Can_Read(i) )
		{
			Description	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td></tr>\n"),
				SG_Get_OGR_Drivers().Get_Name(i).c_str(),
				SG_Get_OGR_Drivers().Get_Description(i).c_str()
			);
		}
    }

	Description	+= SG_T("</table>");

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL, "FILES"	, _TL("Files"),
		_TL(""),
		NULL, NULL, false, false, true
	);

	CSG_String	sChoices;
	for(int i=0; i<GEOM_TYPE_KEY_Count; i++)
	{
		sChoices += gSG_Geom_Type_Choice_Key_Name[i];
		sChoices += SG_T("|");
	}

	Parameters.Add_Choice(
		NULL, "GEOM_TYPE"	, _TL("Geometry Type"),
		_TL("Some OGR drivers are unable to determine the geometry type automatically, please choose the appropriate one in this case"),
		sChoices,
		0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COGR_Import::On_Execute(void)
{
	CSG_Strings			Files;
	CSG_OGR_DataSource	DataSource;

	//-----------------------------------------------------
	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Parameters("SHAPES")->asShapesList()->Del_Items();

	for(int iFile=0; iFile<Files.Get_Count(); iFile++)
	{
		Message_Add(CSG_String::Format(SG_T("%s: %s"), _TL("loading"), Files[iFile].c_str()));

		if( !DataSource.Create(Files[iFile]) )
		{
			Message_Add(_TL("could not open data source"));
		}
		else if( DataSource.Get_Count() <= 0 )
		{
			Message_Add(_TL("no layers in data source"));
		}
		else
		{
			for(int iLayer=0; iLayer<DataSource.Get_Count(); iLayer++)
			{
				CSG_Shapes	*pShapes	= DataSource.Read(iLayer, Parameters("GEOM_TYPE")->asInt());

				if( pShapes )
				{
					Parameters("SHAPES")->asShapesList()->Add_Item(pShapes);
				}
			}
		}
	}

	return( Parameters("SHAPES")->asShapesList()->Get_Count() > 0 );
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

	if(	!Import.Get_Parameters()->Set_Parameter(SG_T("FILES"), File_Name, PARAMETER_TYPE_FilePath) )
	{
		return( false );
	}

	if(	!Import.Execute() )
	{
		return( false );
	}

	CSG_Parameter_Shapes_List	*pShapes	= Import.Get_Parameters()->Get_Parameter(SG_T("SHAPES"))->asShapesList();

	for(int i=0; i<pShapes->Get_Count(); i++)
	{
		SG_UI_DataObject_Add(pShapes->asShapes(i), SG_UI_DATAOBJECT_UPDATE_ONLY);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
