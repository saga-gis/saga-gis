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

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "GRID") )
	{
		pParameters->Get_Parameter("INTERPOL")->Set_Enabled(pParameter->asGrid()
			&& pParameter->asGrid()->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Projected
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
		&&  pModule->Set_Parameter("INTERPOLATION", Parameters("INTERPOL")->asBool() ? 4 : 0)
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
