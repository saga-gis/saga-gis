
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
//                   gdal_buildvrt.cpp                   //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_buildvrt.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef GDAL_V2_1_OR_NEWER

//---------------------------------------------------------
#include <cpl_string.h>
#include <gdal_utils.h>
#include "gdal_driver.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_BuildVRT::CGDAL_BuildVRT(void)
{
	Set_Name	(_TL("Create Virtual Raster (VRT)"));

	Set_Author	("V. Wichmann (c) 2019");

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	Add_Reference("GDAL/OGR contributors", "2019",
		"The gdalbuildvrt utility",
		"GDAL documentation.",
		SG_T("https://gdal.org/programs/gdalbuildvrt.html"), SG_T("Link")
	);

	CSG_String	Description, Filter, Filter_All;

	Description	= _TW(
		"The tool allows one to create a virtual dataset (VRT) which is a mosaic of the input raster datasets. "
		"Such a VRT can be used for seamless data access to a large number of raster tiles, a typical application "
		"is the clipping of raster tiles from such a VRT.\n\n"
	);

	Description	+= CSG_String::Format("\nGDAL %s:%s\n\n", _TL("Version"), SG_Get_GDAL_Drivers().Get_Version().c_str());

	Description	+= _TL("Following raster formats are currently supported:");

	Description	+= CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>",
		_TL("ID"), _TL("Name"), _TL("Extension")
	);

	for(int i=0; i<SG_Get_GDAL_Drivers().Get_Count(); i++)
    {
		if( SG_Get_GDAL_Drivers().is_Raster(i) && SG_Get_GDAL_Drivers().Can_Read(i) )
		{
			CSG_String	ID		= SG_Get_GDAL_Drivers().Get_Description(i).c_str();
			CSG_String	Name	= SG_Get_GDAL_Drivers().Get_Name       (i).c_str();
			CSG_String	Ext		= SG_Get_GDAL_Drivers().Get_Extension  (i).c_str();

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

	Filter.Prepend(CSG_String::Format("%s|%s|" , _TL("All Recognized Files"), Filter_All.c_str()));
	Filter.Append (CSG_String::Format("%s|*.*" , _TL("All Files")));
	
	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILES"		, _TL("Files"),
		_TL("The input files."),
		Filter, NULL, false, false, true
	);

	Parameters.Add_FilePath("",
		"FILE_LIST"	, _TL("Input File List"),
		_TL("A text file with the full path to an input grid on each line."),
		CSG_String::Format("%s|*.txt|%s|*.*",
			_TL("Text Files"),
            _TL("All Files")
        ), NULL, false, false, false
	)->Set_UseInGUI(false);

	Parameters.Add_FilePath("",
		"VRT_NAME"	, _TL("VRT Filename"),
		_TL("The full path and name of the .vrt output file."),
		CSG_String::Format("%s (*.vrt)|*.vrt|%s|*.*",
			_TL("Virtual Dataset"),
			_TL("All Files")
		), NULL, true, false, false
 	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
        _TL("The resampling algorithm used when datasets are queried from the VRT."),
		CSG_String::Format("{%s}%s|{%s}%s|{%s}%s|{%s}%s|{%s}%s|{%s}%s|{%s}%s",
			// we use the choices' data item for the option names that have to be passed (untranslated) to the gdal utility call
            SG_T("nearest"    ), _TL("nearest"     ),	
			SG_T("bilinear"   ), _TL("bilinear"    ),
			SG_T("cubic"      ), _TL("cubic"       ),
			SG_T("cubicspline"), _TL("cubic spline"),
			SG_T("lanczos"    ), _TL("lanczos"     ),
			SG_T("average"    ), _TL("average"     ),
			SG_T("mode"       ), _TL("mode"        )
        ), 0
    );

	Parameters.Add_Choice("",
		"RESOLUTION", _TL("Resolution"),
        _TL("The method how to compute the output resolution if the resolution of all input files is not the same."),
        CSG_String::Format("{%s}%s|{%s}%s|{%s}%s|{%s}%s",
            SG_T("highest"), _TL("highest"),
            SG_T("lowest" ), _TL("lowest" ),
			SG_T("average"), _TL("average"),
			SG_T("user"   ), _TL("user"   )
        ), 0
    );

	Parameters.Add_Double("",
		"CELLSIZE"	, _TL("Cellsize"),
		_TL(""),
		1., 0., true
	);

	Parameters.Add_Bool("",
		"ALIGN"		, _TL("Align"),
		_TL("Align the coordinates of the extent to the cellsize."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_BuildVRT::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("RESOLUTION") )
	{
		pParameters->Set_Enabled("CELLSIZE", pParameter->asInt() == 3);
		pParameters->Set_Enabled("ALIGN"   , pParameter->asInt() == 3);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_BuildVRT::On_Execute(void)
{
	char **pFiles = NULL; int nFiles = 0; CSG_Strings Files;

	if( Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		for(int i=0; i<Files.Get_Count(); i++)
		{
			CSG_String File(Files.Get_String(i));

			if( SG_File_Exists(File) )
			{
				pFiles = CSLAddString(pFiles, File.b_str()); nFiles++;
			}
		}
	}
	else if( has_GUI() == false )
	{
		SG_UI_Msg_Add(_TL("No files specified with the \"Files\" parameter, trying to use input file list."), true);

		CSG_Table Table(Parameters("FILE_LIST")->asString(), TABLE_FILETYPE_Text_NoHeadLine);

		if( Table.Get_Field_Count() < 1 || Table.Get_Count() < 1 )
		{
			Error_Set(_TL("Input file list could not be opened or is empty!"));

			return( false );
		}
		
		for(int i=0; i<Table.Get_Count(); i++)	// build our own list, as the -input_file_list option is not available in the library call, only in the gdal binary
		{
			CSG_String File(Table.Get_Record(i)->asString(0));

			if( SG_File_Exists(File) )
			{
				pFiles = CSLAddString(pFiles, File.b_str()); nFiles++;
			}
		}
	}

	if( nFiles < 1 )
	{
		Error_Set(_TL("No existing files have been selected for input."));

		return( false );
	}

	//-----------------------------------------------------
	char **pOpts = NULL;

	pOpts = CSLAddString(pOpts, "-r");
	pOpts = CSLAddString(pOpts, Parameters("RESAMPLING")->asChoice()->Get_Data().b_str());
	pOpts = CSLAddString(pOpts, "-resolution");
	pOpts = CSLAddString(pOpts, Parameters("RESOLUTION")->asChoice()->Get_Data().b_str());

	if( Parameters("RESOLUTION")->asInt() == 3 ) // user
	{
		CSG_String Cellsize(Parameters("CELLSIZE")->asString());
	
		pOpts = CSLAddString(pOpts, "-tr");
		pOpts = CSLAddString(pOpts, Cellsize.b_str()); // xres
		pOpts = CSLAddString(pOpts, Cellsize.b_str()); // yres

		if( Parameters("ALIGN")->asBool() )
		{
			pOpts = CSLAddString(pOpts, "-tap");
		}
	}

	GDALBuildVRTOptions *pOptions = GDALBuildVRTOptionsNew(pOpts, nullptr);

	//-----------------------------------------------------
	int bUsageError = 0;

	CSG_String File(Parameters("VRT_NAME")->asString());
	
	GDALDatasetH hVRT = GDALBuildVRT(File.b_str(), nFiles, nullptr, pFiles, pOptions, &bUsageError);

    if( bUsageError != 0 )
	{
		Error_Set(_TL("Unable to build virtual dataset."));
	}

	CSLDestroy(pFiles);
	GDALBuildVRTOptionsFree(pOptions);
	GDALClose(hVRT);

	return( bUsageError == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // GDAL_V2_1_OR_NEWER
