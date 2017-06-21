
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   gdal_formats.cpp                    //
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#include "gdal_formats.h"

#include "gdal_driver.h"
#include "ogr_driver.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	GDAL_LIST_FMT_ID	= 0,
	GDAL_LIST_FMT_NAME,
	GDAL_LIST_FMT_FILTER,
	GDAL_LIST_FMT_TYPE,
	GDAL_LIST_FMT_ACCESS
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Formats::CGDAL_Formats(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("GDAL Formats"));

	Set_Author	("O.Conrad (c) 2016");

	CSG_String	Description;

	Description	= _TW(
		"This tool lists all (file) formats supported by the currently loaded GDAL library. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"FORMATS"	, _TL("GDAL Formats"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("raster"),
			_TL("vector"),
			_TL("all")
		), 2
	);

	Parameters.Add_Choice("",
		"ACCESS"	, _TL("Access"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("read"),
			_TL("write"),
			_TL("read or write")
		), 2
	);

	Parameters.Add_Bool("",
		"RECOGNIZED", _TL("All Recognized Files"),
		_TL("Add an entry for all recognized files."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Formats::On_Execute(void)
{
	CSG_Table	*pFormats	= Parameters("FORMATS")->asTable();

	pFormats->Destroy();
	pFormats->Set_Name(_TL("GDAL Formats"));

	pFormats->Add_Field("ID"    , SG_DATATYPE_String);
	pFormats->Add_Field("NAME"  , SG_DATATYPE_String);
	pFormats->Add_Field("FILTER", SG_DATATYPE_String);
	pFormats->Add_Field("TYPE"  , SG_DATATYPE_String);
	pFormats->Add_Field("ACCESS", SG_DATATYPE_String);

	//-----------------------------------------------------
	int		Type	= Parameters("TYPE"  )->asInt();
	int		Access	= Parameters("ACCESS")->asInt();

	//-----------------------------------------------------
	if( Type != 1 )	// not vectors only
	{
		for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
		{
			if( SG_Get_GDAL_Drivers().is_Raster(i) )
			{
				CSG_String	R(SG_Get_GDAL_Drivers().Can_Read (i) ? "R" : "");
				CSG_String	W(SG_Get_GDAL_Drivers().Can_Write(i) ? "W" : "");

				if( (Access != 0 || !R.is_Empty()) && (Access != 1 || !W.is_Empty()) )
				{
					CSG_Table_Record	*pFormat	= pFormats->Add_Record();

					pFormat->Set_Value(GDAL_LIST_FMT_ID    , SG_Get_GDAL_Drivers().Get_Description(i));
					pFormat->Set_Value(GDAL_LIST_FMT_NAME  , SG_Get_GDAL_Drivers().Get_Name       (i));
					pFormat->Set_Value(GDAL_LIST_FMT_FILTER, SG_Get_GDAL_Drivers().Get_Extension  (i));
					pFormat->Set_Value(GDAL_LIST_FMT_TYPE  , "RASTER");
					pFormat->Set_Value(GDAL_LIST_FMT_ACCESS, R + W);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Type != 0 )	// not rasters only
	{
		for(int i=0; i<SG_Get_OGR_Drivers().Get_Count(); i++)
		{
			if( SG_Get_OGR_Drivers().is_Vector(i) )
			{
				CSG_String	R(SG_Get_OGR_Drivers().Can_Read (i) ? "R" : "");
				CSG_String	W(SG_Get_OGR_Drivers().Can_Write(i) ? "W" : "");

				if( (Access != 0 || !R.is_Empty()) && (Access != 1 || !W.is_Empty()) )
				{
					CSG_Table_Record	*pFormat	= pFormats->Add_Record();

					pFormat->Set_Value(GDAL_LIST_FMT_ID    , SG_Get_OGR_Drivers().Get_Description(i));
					pFormat->Set_Value(GDAL_LIST_FMT_NAME  , SG_Get_OGR_Drivers().Get_Name       (i));
					pFormat->Set_Value(GDAL_LIST_FMT_FILTER, SG_Get_OGR_Drivers().Get_Extension  (i));
					pFormat->Set_Value(GDAL_LIST_FMT_TYPE  , "VECTOR");
					pFormat->Set_Value(GDAL_LIST_FMT_ACCESS, R + W);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("RECOGNIZED")->asBool() )
	{
		CSG_String	Filter_All, Filter_Last;

		pFormats->Set_Index(GDAL_LIST_FMT_FILTER, TABLE_INDEX_Ascending);

		for(int i=0; i<pFormats->Get_Count(); i++)
		{
			CSG_String	Filter	= pFormats->Get_Record_byIndex(i)->asString(GDAL_LIST_FMT_FILTER);

			if( !Filter.is_Empty() && Filter.Cmp(Filter_Last) )
			{
				Filter.Replace("/", ";");

				Filter_All	+= (Filter_All.is_Empty() ? "*." : ";*.") + Filter;

				Filter_Last	 = Filter;
			}
		}

		pFormats->Del_Index();

		if( !Filter_All.is_Empty() )
		{
			CSG_Table_Record	*pFormat	= pFormats->Add_Record();

			pFormat->Set_Value(GDAL_LIST_FMT_NAME  , _TL("All Recognized Files"));
			pFormat->Set_Value(GDAL_LIST_FMT_FILTER, Filter_All);
			pFormat->Set_Value(GDAL_LIST_FMT_TYPE  , Type   == 0 ? "RASTER" : Type   == 1 ? "VECTOR" : "RASTER/VECTOR");
			pFormat->Set_Value(GDAL_LIST_FMT_ACCESS, Access == 0 ? "R"      : Access == 1 ? "W"      : "RW"           );
		}
	}

	//-----------------------------------------------------
	return( pFormats->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
