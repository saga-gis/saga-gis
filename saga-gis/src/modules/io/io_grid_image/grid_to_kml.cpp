/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     io_grid_image                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    grid_to_kml.cpp                    //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/dir.h>

//---------------------------------------------------------
#include "grid_to_kml.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_to_KML::CGrid_to_KML(void)
{
	Set_Name		(_TL("Export Grid to KML"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Uses 'Export Image' tool to create the image file. Automatically projects raster to geographic "
		"coordinate system, if its projection is known and not geographic. "
	));

	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SHADE"		, _TL("Shade"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Image File"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("Portable Network Graphics (*.png)")			, SG_T("*.png"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)")	, SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Tagged Image File Format (*.tif, *.tiff)")		, SG_T("*.tif;*.tiff"),
			_TL("Windows or OS/2 Bitmap (*.bmp)")				, SG_T("*.bmp"),
			_TL("Zsoft Paintbrush (*.pcx)")						, SG_T("*.pcx")
		), NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("kml and image files"),
			_TL("kmz, kml and image files"),
			_TL("kmz file")
		), 2
	);

	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Choice(
			NULL	, "COLOURING"	, _TL("Colouring"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s|%s|",
				_TL("stretch to grid's standard deviation"),
				_TL("stretch to grid's value range"),
				_TL("stretch to specified value range"),
				_TL("lookup table"),
				_TL("rgb coded values"),
				_TL("same as in graphical user interface")
			), 5
		);

		Parameters.Add_Colors(
			NULL	, "COL_PALETTE"	, _TL("Colours Palette"),
			_TL("")
		);
	}
	else
	{
		Parameters.Add_Choice(
			NULL	, "COLOURING"	, _TL("Colouring"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s|",
				_TL("stretch to grid's standard deviation"),
				_TL("stretch to grid's value range"),
				_TL("stretch to specified value range"),
				_TL("lookup table"),
				_TL("rgb coded values")
			), 0
		);

		Parameters.Add_Choice(
			NULL	, "COL_PALETTE"	, _TL("Color Palette"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
				_TL("DEFAULT"),			_TL("DEFAULT_BRIGHT"),	_TL("BLACK_WHITE"),		_TL("BLACK_RED"),
				_TL("BLACK_GREEN"),		_TL("BLACK_BLUE"),		_TL("WHITE_RED"),		_TL("WHITE_GREEN"),
				_TL("WHITE_BLUE"),		_TL("YELLOW_RED"),		_TL("YELLOW_GREEN"),	_TL("YELLOW_BLUE"),
				_TL("RED_GREEN"),		_TL("RED_BLUE"),		_TL("GREEN_BLUE"),		_TL("RED_GREY_BLUE"),
				_TL("RED_GREY_GREEN"),	_TL("GREEN_GREY_BLUE"),	_TL("RED_GREEN_BLUE"),	_TL("RED_BLUE_GREEN"),
				_TL("GREEN_RED_BLUE"),	_TL("RAINBOW"),			_TL("NEON"),			_TL("TOPOGRAPHY"),
				_TL("ASPECT_1"),		_TL("ASPECT_2"),		_TL("ASPECT_3")
			), 0
		);

		Parameters.Add_Value(
			NULL	, "COL_COUNT"	, _TL("Number of Colors"),
			_TL(""),
			PARAMETER_TYPE_Int, 100
		);

		Parameters.Add_Value(
			NULL	, "COL_REVERT"	, _TL("Revert Palette"),
			_TL(""),
			PARAMETER_TYPE_Bool, false
		);
	}

	Parameters.Add_Value(
		NULL	, "STDDEV"		, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double, 2.0, 0.0, true
	);

	Parameters.Add_Range(
        NULL	, "STRETCH"		, _TL("Stretch to Value Range"),
        _TL(""),
        0.0, 100.0
    );

	Parameters.Add_Table(
		NULL	, "LUT"			, _TL("Lookup Table"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Value(
        NULL	, "INTERPOL"	, _TL("Interpolation"),
        _TL("interpolate values if projection is needed"),
        PARAMETER_TYPE_Bool, true
    );

	if( !SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Range(
			NULL	, "SHADE_BRIGHT", _TL("Shade Brightness"),
			_TL("Allows to scale shade brightness, [percent]"),
			0.0, 100.0, 0.0, true, 100.0, true
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_to_KML::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "COLOURING") )
	{
		pParameters->Get_Parameter("COL_PALETTE")->Set_Enabled(pParameter->asInt() <= 2);
		pParameters->Get_Parameter("STDDEV"     )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("STRETCH"    )->Set_Enabled(pParameter->asInt() == 2);
		pParameters->Get_Parameter("LUT"        )->Set_Enabled(pParameter->asInt() == 3);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SHADE") && pParameters->Get_Parameter("SHADE_BRIGHT") )
	{
		pParameters->Get_Parameter("SHADE_BRIGHT")->Set_Enabled(pParameter->asGrid() != NULL);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "GRID") || !SG_STR_CMP(pParameter->Get_Identifier(), "COLOURING") )
	{
		CSG_Grid	*pGrid	= pParameters->Get_Parameter("GRID")->asGrid();

		pParameters->Get_Parameter("INTERPOL")->Set_Enabled(
			pGrid && pGrid->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Projected && pParameters->Get_Parameter("COLOURING")->asInt() < 4
		);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_to_KML::On_Execute(void)
{
	//-----------------------------------------------------
	bool	bDelete	= false;

	CSG_Module		*pModule;

	CSG_Grid	*pGrid	= Parameters("GRID" )->asGrid(), Image;
	CSG_Grid	*pShade	= Parameters("SHADE")->asGrid();

	//-----------------------------------------------------
	int	Method	= Parameters("COLOURING")->asInt();

	if( Method == 5 )	// same as in graphical user interface
	{
		if( !SG_UI_DataObject_asImage(pGrid, &Image) )
		{
			Error_Set("could not retrieve colour coding from graphical user interface.");

			return( false );
		}

		Image.Set_Name       (pGrid->Get_Name       ());
		Image.Set_Description(pGrid->Get_Description());
		Image.Flip();
		pGrid	= &Image;
		Method	= 4;	// rgb coded values
	}

	//-----------------------------------------------------
	if( pGrid->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Undefined )
	{
		Message_Add(_TL("layer uses undefined coordinate system, assuming geographic coordinates"));
	}
	else if( pGrid->Get_Projection().Get_Type() != SG_PROJ_TYPE_CS_Geographic )
	{
		Message_Add(CSG_String::Format("\n%s (%s: %s)\n", _TL("re-projection to geographic coordinates"), _TL("original"), pGrid->Get_Projection().Get_Name().c_str()), false);

		if(	(pModule = SG_Get_Module_Library_Manager().Get_Module("pj_proj4", 4)) == NULL )	// Coordinate Transformation (Grid)
		{
			return( false );
		}

		pModule->Settings_Push();

		if( pModule->Set_Parameter("CRS_PROJ4"    , SG_T("+proj=longlat +ellps=WGS84 +datum=WGS84"))
		&&  pModule->Set_Parameter("INTERPOLATION", Method < 4 && Parameters("INTERPOL")->asBool() ? 4 : 0)
		&&  pModule->Set_Parameter("SOURCE"       , pGrid)
		&&  pModule->Execute() )
		{
			bDelete	= true;

			pGrid	= pModule->Get_Parameters("TARGET")->Get_Parameter("GRID")->asGrid();

			if( pShade && pModule->Set_Parameter("SOURCE", pShade) && pModule->Execute() )
			{
				pShade	= pModule->Get_Parameters("TARGET")->Get_Parameter("GRID")->asGrid();
			}
			else
			{
				pShade	= NULL;
			}
		}

		pModule->Settings_Pop();

		if( !bDelete )
		{
			Message_Add(CSG_String::Format("\n%s: %s\n", _TL("re-projection"), _TL("failed")), false);

			return( false );
		}
	}

	//-----------------------------------------------------
	if(	(pModule = SG_Get_Module_Library_Manager().Get_Module("io_grid_image", 0)) == NULL )	// Export Image
	{
		return( false );
	}

	bool	bResult	= false;

	pModule->Settings_Push();

	if( pModule->Set_Parameter("GRID"        , pGrid)
	&&  pModule->Set_Parameter("SHADE"       , pShade)
	&&  pModule->Set_Parameter("FILE_KML"    , true)
	&&  pModule->Set_Parameter("FILE"        , Parameters("FILE"))
	&&  pModule->Set_Parameter("COLOURING"   , Method)
	&&  pModule->Set_Parameter("COL_PALETTE" , Parameters("COL_PALETTE"))
	&&  pModule->Set_Parameter("STDDEV"      , Parameters("STDDEV"))
	&&  pModule->Set_Parameter("STRETCH"     , Parameters("STRETCH"))
	&&  pModule->Set_Parameter("LUT"         , Parameters("LUT"))
	&&  (SG_UI_Get_Window_Main() || pModule->Set_Parameter("SHADE_BRIGHT", Parameters("SHADE_BRIGHT")))
	&&  pModule->Execute() )
	{
		bResult	= true;
	}

	pModule->Settings_Pop();

	//-----------------------------------------------------
	if( bDelete )
	{
		delete(pGrid);

		if( pShade )
		{
			delete(pShade);
		}
	}

//---------------------------------------------------------
#define ZIP_ADD_FILE(zip, fn)	{\
	wxFileInputStream	*pInput;\
	\
	if( SG_File_Exists(fn) && (pInput = new wxFileInputStream(fn)) != NULL )\
	{\
		zip.PutNextEntry(SG_File_Get_Name(fn, true).c_str());\
		zip.Write(*pInput);\
		delete(pInput);\
	}\
}
//---------------------------------------------------------

	if( Parameters("OUTPUT")->asInt() != 0 )	// create kmz
	{
		CSG_String	Filename	= Parameters("FILE")->asString();	SG_File_Set_Extension(Filename, "kmz");

		wxDir		dir;
		wxString	file;

		//-------------------------------------------------
		if( dir.Open(SG_File_Get_Path(Filename).c_str()) && dir.GetFirst(&file, wxString::Format("%s.*", SG_File_Get_Name(Filename, false).c_str()), wxDIR_FILES) )
		{
			CSG_Strings	Files;

			do
			{
				if( !SG_File_Cmp_Extension(file, SG_T("kmz")) )
				{
					Files	+= SG_File_Make_Path(SG_File_Get_Path(Filename), file);
				}
			}
			while( dir.GetNext(&file) );

			dir.Close();

			//---------------------------------------------
			wxZipOutputStream	Zip(new wxFileOutputStream(Filename.c_str()));

			for(int i=0; i<Files.Get_Count(); i++)
			{
				ZIP_ADD_FILE(Zip, Files[i].c_str());

				if( Parameters("OUTPUT")->asInt() != 1 )	// delete kml, image and associated files
				{
					SG_File_Delete(Files[i]);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_from_KML::CGrid_from_KML(void)
{
	Set_Name		(_TL("Import Grids from KML"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Uses 'Import Image' tool to load the ground overlay image files associated with the kml. "
	));

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("KML/KMZ File"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("KML/KMZ Files"), SG_T("*.kml;*.kmz"),
			_TL("All Files"    ), SG_T("*.*")
		), NULL, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_from_KML::On_Execute(void)
{
	//-----------------------------------------------------
	wxString	Dir, File	= Parameters("FILE")->asString();

	//-----------------------------------------------------
	bool	bKMZ	= SG_File_Cmp_Extension(File, SG_T("kmz"));

	if( !bKMZ )
	{
		Dir	= SG_File_Get_Path(File).c_str();
	}
	else	// unzip to temporary directory
	{
		Dir	= wxFileName::CreateTempFileName("kml_");

		wxRemoveFile(Dir);
		wxFileName::Mkdir(Dir);

		wxZipEntry			*pEntry;
		wxZipInputStream	Zip(new wxFileInputStream(File));

		while( (pEntry = Zip.GetNextEntry()) != NULL )
		{
			wxFileName	fn(Dir, pEntry->GetName());

			wxFileOutputStream	*pOutput	= new wxFileOutputStream(fn.GetFullPath());

			pOutput->Write(Zip);

			delete(pOutput);
			delete(pEntry);

			if( !fn.GetExt().CmpNoCase("kml") )
			{
				File	= fn.GetFullPath();
			}
		}
	}

	//-----------------------------------------------------
	CSG_MetaData	KML;

	if( !KML.Load(&File) )
	{
		Error_Fmt("%s [%s]", _TL("failed to load file"), File.wc_str());

		return( false );
	}

	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")->asGridList();
	m_pGrids->Del_Items();

	Load_KML(Dir, KML);

	//-----------------------------------------------------
	if( bKMZ && wxDirExists(Dir) )
	{
		wxFileName::Rmdir(Dir, wxPATH_RMDIR_FULL|wxPATH_RMDIR_RECURSIVE);
	}

	//-----------------------------------------------------
	return( m_pGrids->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_from_KML::Load_KML(const SG_Char *Dir, const CSG_MetaData &KML)
{
	for(int i=0; i<KML.Get_Children_Count(); i++)
	{
		if( KML.Get_Child(i)->Cmp_Name("GroundOverlay") )
		{
			Load_Overlay(Dir, *KML.Get_Child(i));
		}
		else
		{
			Load_KML    (Dir, *KML.Get_Child(i));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_from_KML::Load_Overlay(const SG_Char *Dir, const CSG_MetaData &KML)
{
	//-----------------------------------------------------
	if( !KML("Icon") || !KML["Icon"]("HRef") )
	{
		Error_Set(_TL("missing icon tags"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Rect	r;

	if(    !KML("LatLonBox")
		|| !KML["LatLonBox"]("North") || !KML["LatLonBox"]["North"].Get_Content().asDouble(r.m_rect.yMax)
		|| !KML["LatLonBox"]("South") || !KML["LatLonBox"]["South"].Get_Content().asDouble(r.m_rect.yMin)
		|| !KML["LatLonBox"]("East" ) || !KML["LatLonBox"]["East" ].Get_Content().asDouble(r.m_rect.xMax)
		|| !KML["LatLonBox"]("West" ) || !KML["LatLonBox"]["West" ].Get_Content().asDouble(r.m_rect.xMin) )
	{
		Error_Set(_TL("failed to load georeference for KML ground overlay"));

		return( false );
	}

	//-----------------------------------------------------
	wxFileName	fn(KML["Icon"]["HRef"].Get_Content().c_str());

	if( !fn.FileExists() )
	{
		fn.SetPath(Dir);
	}

	CSG_Data_Manager	Data;
	CSG_String			FullPath = fn.GetFullPath().wc_str();

	if( !Data.Add(FullPath) || !Data.Get_Grid_System(0) || !Data.Get_Grid_System(0)->Get(0) )
	{
		Error_Fmt("%s: %s", _TL("failed to load KML ground overlay icon"), fn.GetFullPath().wc_str());
	}

	//-----------------------------------------------------
	CSG_Grid	*pIcon	= (CSG_Grid *)Data.Get_Grid_System(0)->Get(0);

	CSG_Grid	*pGrid	= SG_Create_Grid(pIcon->Get_Type(), pIcon->Get_NX(), pIcon->Get_NY(), r.Get_YRange() / (pIcon->Get_NY() - 1), r.Get_XMin(), r.Get_YMin());

	if( KML("Name") && !KML["Name"].Get_Content().is_Empty() )
		pGrid->Set_Name(KML["Name"].Get_Content());

	if( KML("Description") && !KML["Description"].Get_Content().is_Empty() )
		pGrid->Set_Name(KML["Description"].Get_Content());

	pGrid->Get_Projection().Assign("+proj=longlat +ellps=WGS84 +datum=WGS84", SG_PROJ_FMT_Proj4);

	#pragma omp parallel for
	for(int y=0; y<pGrid->Get_NY(); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			pGrid->Set_Value(x, y, pIcon->asDouble(x, y));
		}
	}

	m_pGrids->Add_Item(pGrid);

	DataObject_Add(pGrid);
	DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 6);	// Color Classification Type: RGB

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
