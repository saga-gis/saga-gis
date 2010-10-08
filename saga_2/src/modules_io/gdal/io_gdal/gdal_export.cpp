
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
	Set_Name	(_TL("GDAL: Export Raster"));

	Set_Author	(SG_T("(c) 2007 by O.Conrad"));

	CSG_String	Description, Formats;

	Description	= _TW(
		"The \"GDAL Raster Export\" module exports one or more grids to various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
		"\n"
		"Following raster formats are currently supported:\n"
		"<table border=\"1\"><tr><th>ID</th><th>Name</th></tr>\n"
	);

	for(int i=0; i<g_GDAL_Driver.Get_Count(); i++)
    {
		if( !g_GDAL_Driver.is_ReadOnly(i) )
		{
			Description	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td></tr>\n"),
				SG_STR_MBTOSG(g_GDAL_Driver.Get_Description(i)),
				SG_STR_MBTOSG(g_GDAL_Driver.Get_Name(i))
			);

			Formats		+= CSG_String::Format(SG_T("%s|"), SG_STR_MBTOSG(g_GDAL_Driver.Get_Name(i)));

			m_DriverNames.Add(SG_STR_MBTOSG(g_GDAL_Driver.Get_Description(i)));
		}
    }

	Description	+= SG_T("</table>");

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grid(s)"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL, "FILE"	, _TL("File"),
		_TL(""),
		NULL, NULL, true
	);

	Parameters.Add_Choice(
		NULL, "FORMAT"	, _TL("Format"),
		_TL(""),
		Formats
	);

	Parameters.Add_Choice(
		NULL, "TYPE"	, _TL("Data Type"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|"),
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
}

//---------------------------------------------------------
CGDAL_Export::~CGDAL_Export(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export::On_Execute(void)
{
	TSG_Data_Type			Type;
	CSG_String				File_Name;
	CSG_Projection			Projection;
	CSG_Parameter_Grid_List	*pGrids;
	CGDAL_System			System;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	File_Name	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	switch( Parameters("TYPE")->asInt() )
	{
	default:
	case 0:	Type	= g_GDAL_Driver.Get_Grid_Type(pGrids);	break;	// match input data
	case 1:	Type	= SG_DATATYPE_Byte;		break;	// Eight bit unsigned integer
	case 2:	Type	= SG_DATATYPE_Word;		break;	// Sixteen bit unsigned integer
	case 3:	Type	= SG_DATATYPE_Short;	break;	// Sixteen bit signed integer
	case 4:	Type	= SG_DATATYPE_DWord;	break;	// Thirty two bit unsigned integer
	case 5:	Type	= SG_DATATYPE_Int;		break;	// Thirty two bit signed integer
	case 6:	Type	= SG_DATATYPE_Float;	break;	// Thirty two bit floating point
	case 7:	Type	= SG_DATATYPE_Double;	break;	// Sixty four bit floating point
	}

	//-----------------------------------------------------
	if( !System.Open_Write(File_Name, m_DriverNames[Parameters("FORMAT")->asInt()], Type, pGrids->Get_Count(), *Get_System(), Projection) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count(); i++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s %d"), _TL("Band"), i + 1));

		System.Write_Band(i, pGrids->asGrid(i));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
