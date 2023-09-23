
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
#include "gdal_export.h"


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

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	CSG_String	Description, Formats, Filter;

	Description	= _TW(
		"The \"GDAL Raster Export\" tool exports one or more grids to various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
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
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grid(s)"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice("",
		"MULTIPLE"	, _TL("Multiple"),
		_TL("If multiple grids are supplied: export each grid to a separate file or all grids to one multi-band file. Notice: Storing multiple bands in one file might not be supported by the selected format. For single files the grid name is used as file name."),
		CSG_String::Format("%s|%s",
			_TL("single files"),
			_TL("one file")
		), 1
	);

	Parameters.Add_FilePath("",
		"FOLDER"	, _TL("Folder"),
		_TL("The folder location to which single files will be stored."),
		NULL, NULL, true, true
	);

	Parameters.Add_String("",
		"EXTENSION"	, _TL("Extension"),
		_TL(""),
		""
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL("The GDAL dataset to be created."),
		Filter, NULL, true
	);

	Parameters.Add_Choice("",
		"FORMAT"	, _TL("Format"),
		_TL("The GDAL raster format (driver) to be used."),
		Formats
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Data Type"),
		_TL("The GDAL datatype of the created dataset."),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
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
	
	Parameters.Add_Bool("",
		"SET_NODATA", _TL("Set Custom NoData"),
		_TL(""),
		false
	);

	Parameters.Add_Double("SET_NODATA",
		"NODATA"	, _TL("NoData Value"),
		_TL("")
	);

	Parameters.Add_String("",
		"OPTIONS"	, _TL("Creation Options"),
		_TL("A space separated list of key-value pairs (K=V)."), _TL("")		
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Export::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	bool bSingles = (*pParameters)("GRIDS")->asGridList()->Get_Grid_Count() > 1 && (*pParameters)("MULTIPLE")->asInt() == 0; // single files

	pParameters->Set_Enabled("FOLDER"   ,  bSingles);
	pParameters->Set_Enabled("EXTENSION",  bSingles);
	pParameters->Set_Enabled("FILE"     , !bSingles);

	if( pParameter->Cmp_Identifier("GRIDS") )
	{
		pParameters->Set_Enabled("MULTIPLE", pParameter->asGridList()->Get_Grid_Count() > 1);
	}

	if( pParameter->Cmp_Identifier("FORMAT") )
	{
		pParameters->Set_Parameter("EXTENSION", SG_Get_GDAL_Drivers().Get_Extension(pParameter->asChoice()->Get_Data()));
	}

	if( pParameter->Cmp_Identifier("SET_NODATA") )
	{
		pParameters->Set_Enabled("NODATA", pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Export::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	CSG_String Driver(Parameters("FORMAT")->asChoice()->Get_Data());

	CSG_String Options(Parameters("OPTIONS")->asString());

	TSG_Data_Type Type;

	switch( Parameters("TYPE")->asInt() )
	{
	default: Type = SG_Get_Grid_Type(pGrids); break; // match input data
	case  1: Type = SG_DATATYPE_Byte        ; break; // Eight bit unsigned integer
	case  2: Type = SG_DATATYPE_Word        ; break; // Sixteen bit unsigned integer
	case  3: Type = SG_DATATYPE_Short       ; break; // Sixteen bit signed integer
	case  4: Type = SG_DATATYPE_DWord       ; break; // Thirty two bit unsigned integer
	case  5: Type = SG_DATATYPE_Int         ; break; // Thirty two bit signed integer
	case  6: Type = SG_DATATYPE_Float       ; break; // Thirty two bit floating point
	case  7: Type = SG_DATATYPE_Double      ; break; // Sixty four bit floating point
	}

	CSG_Projection Projection; Get_Projection(Projection);

	//-----------------------------------------------------
	if( pGrids->Get_Grid_Count() > 1 && Parameters("MULTIPLE")->asInt() == 0 ) // single files
	{
		CSG_String Folder(Parameters("FOLDER")->asString()), Extension(Parameters("EXTENSION")->asString());

		for(int i=0; i<pGrids->Get_Grid_Count(); i++)
		{
			Process_Set_Text("%s %d", _TL("Band"), i + 1);

			CSG_GDAL_DataSet DataSet; CSG_String File(Get_File_Name(Folder, pGrids->Get_Grid(i)->Get_Name(), Extension, i));

			if( DataSet.Open_Write(File, Driver, Options, Type, 1, Get_System(), Projection) )
			{
				if( Parameters("SET_NODATA")->asBool() )
				{
					DataSet.Write(0, pGrids->Get_Grid(i), Parameters("NODATA")->asDouble());
				}
				else
				{
					DataSet.Write(0, pGrids->Get_Grid(i));
				}

				DataSet.Close();
			}
		}
	}

	//-----------------------------------------------------
	else // one file
	{
		CSG_GDAL_DataSet DataSet; CSG_String File(Parameters("FILE")->asString());

		if( !DataSet.Open_Write(File, Driver, Options, Type, pGrids->Get_Grid_Count(), Get_System(), Projection) )
		{
			return( false );
		}

		for(int i=0; i<pGrids->Get_Grid_Count(); i++)
		{
			Process_Set_Text("%s %d", _TL("Band"), i + 1);

			if( Parameters("SET_NODATA")->asBool() )
			{
				DataSet.Write(i, pGrids->Get_Grid(i), Parameters("NODATA")->asDouble());
			}
			else
			{
				DataSet.Write(i, pGrids->Get_Grid(i));
			}
		}

		if( !DataSet.Close() )
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CGDAL_Export::Get_File_Name(const CSG_String &Folder, const CSG_String &Name, const CSG_String &Extension, int Index)
{
	CSG_String File(SG_File_Make_Path(Folder, Name));

	if( !Extension.is_Empty() )
	{
		File += "." + Extension;
	}

	if( SG_File_Exists(File) && Index > 0 )
	{
		CSG_String Filename = CSG_String::Format("%s_%d", Name.c_str(), Index + 1);

		File = SG_File_Make_Path(Folder, Filename);

		if( !Extension.is_Empty() )
		{
			File += "." + Extension;
		}
	}

	return( File );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
