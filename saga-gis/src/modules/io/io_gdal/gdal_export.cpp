/**********************************************************
 * Version $Id: gdal_export.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   gdal_export.cpp                     //
//                                                       //
//            Copyright (C) 2007 O. Conrad               //
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
#include "gdal_export.h"

#include <cpl_string.h>

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Export::CGDAL_Export(void)
{
	Set_Name	(_TL("Export Raster"));

	Set_Author	("O.Conrad (c) 2007");

	CSG_String	Description, Formats, Filter;

	Description	= _TW(
		"The \"GDAL Raster Export\" module exports one or more grids to various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Description	+= _TL("Following raster formats are currently supported:");

	Description	+= CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",
		_TL("ID"), _TL("Name"), _TL("Extension")
	);

	Filter.Printf("%s|*.*", _TL("All Files"));

	for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
    {
		if( SG_Get_GDAL_Drivers().is_Raster(i) && SG_Get_GDAL_Drivers().Can_Write(i) )
		{
			CSG_String	ID		= SG_Get_GDAL_Drivers().Get_Description(i).c_str();
			CSG_String	Name	= SG_Get_GDAL_Drivers().Get_Name       (i).c_str();
			CSG_String	Ext		= SG_Get_GDAL_Drivers().Get_Extension  (i).c_str();

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
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grid(s)"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL("The GDAL dataset to be created."),
		Filter, NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "FORMAT"		, _TL("Format"),
		_TL("The GDAL raster format (driver) to be used."),
		Formats
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Data Type"),
		_TL("The GDAL datatype of the created dataset."),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("match input data"),
			_TL("8 bit unsigned integer"),
			_TL("16 bit unsigned integer"),
			_TL("16 bit signed integer"),
			_TL("32 bit unsigned integer"),
			_TL("32 bit signed integer"),
			_TL("32 bit floating point"),
			_TL("64 bit floating point")
		), 0
	);
	
	Parameters.Add_Bool(
		NULL	, "SET_NODATA"	, _TL("Set Custom NoData"),
		_TL(""),
		false
	);

	Parameters.Add_Double(
		NULL	, "NODATA"		, _TL("NoData Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_String(
		NULL	, "OPTIONS"		, _TL("Creation Options"),
		_TL("A space separated list of key-value pairs (K=V)."), _TL("")		
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export::On_Execute(void)
{
	TSG_Data_Type			Type;
	CSG_String				File_Name, Driver, Options;
	CSG_Projection			Projection;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_GDAL_DataSet		DataSet;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	File_Name	= Parameters("FILE")	->asString();
	Options		= Parameters("OPTIONS")	->asString();

	Get_Projection(Projection);

	//-----------------------------------------------------
	switch( Parameters("TYPE")->asInt() )
	{
	default:
	case 0:	Type	= SG_Get_Grid_Type(pGrids);	break;	// match input data
	case 1:	Type	= SG_DATATYPE_Byte;			break;	// Eight bit unsigned integer
	case 2:	Type	= SG_DATATYPE_Word;			break;	// Sixteen bit unsigned integer
	case 3:	Type	= SG_DATATYPE_Short;		break;	// Sixteen bit signed integer
	case 4:	Type	= SG_DATATYPE_DWord;		break;	// Thirty two bit unsigned integer
	case 5:	Type	= SG_DATATYPE_Int;			break;	// Thirty two bit signed integer
	case 6:	Type	= SG_DATATYPE_Float;		break;	// Thirty two bit floating point
	case 7:	Type	= SG_DATATYPE_Double;		break;	// Sixty four bit floating point
	}

	//-----------------------------------------------------
	if( !Parameters("FORMAT")->asChoice()->Get_Data(Driver) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !DataSet.Open_Write(File_Name, Driver, Options, Type, pGrids->Get_Count(), *Get_System(), Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s %d"), _TL("Band"), i + 1));

		if ( Parameters("SET_NODATA")->asBool() )
		{
			DataSet.Write(i, pGrids->asGrid(i), Parameters("NODATA")->asDouble());
		}
		else
		{
			DataSet.Write(i, pGrids->asGrid(i));
		}
	}
	
	if( !DataSet.Close() )
	{
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
