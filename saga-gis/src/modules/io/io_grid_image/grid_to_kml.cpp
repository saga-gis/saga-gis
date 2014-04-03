/**********************************************************
 * Version $Id: grid_to_kml.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Windows or OS/2 Bitmap (*.bmp)")				, SG_T("*.bmp"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)")	, SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Zsoft Paintbrush (*.pcx)")						, SG_T("*.pcx"),
			_TL("Portable Network Graphics (*.png)")			, SG_T("*.png"),
			_TL("Tagged Image File Format (*.tif, *.tiff)")		, SG_T("*.tif;*.tiff")
		), NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "COLOURING"	, _TL("Colouring"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("stretch to grid's standard deviation"),
			_TL("stretch to grid's value range"),
			_TL("stretch to specified value range"),
			_TL("lookup table"),
			_TL("rgb coded values")
		), 0
	);

	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Colors(
			NULL	, "COL_PALETTE"	, _TL("Colours Palette"),
			_TL("")
		);
	}
	else
	{
		Parameters.Add_Choice(
			NULL	, "COL_PALETTE"	, _TL("Color Palette"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
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

	Parameters.Add_Range(
        NULL	, "SHADE_BRIGHT", _TL("Shade Brightness"),
        _TL("Allows to scale shade brightness, [percent]"),
        0.0, 100.0, 0.0, true, 100.0, true
    );

	Parameters.Add_Value(
        NULL	, "INTERPOL"	, _TL("Interpolation"),
        _TL("interpolate values if projection is needed"),
        PARAMETER_TYPE_Bool, true
    );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "SHADE") )
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_to_KML::On_Execute(void)
{
	//-----------------------------------------------------
	bool	bDelete	= false;

	CSG_Parameters	P;
	CSG_Module		*pModule;

	CSG_Grid	Grid , *pGrid	= Parameters("GRID" )->asGrid();
	CSG_Grid	Shade, *pShade	= Parameters("SHADE")->asGrid();

	//-----------------------------------------------------
	if( pGrid->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Undefined )
	{
		Message_Add(_TL("layer uses undefined coordinate system, assuming geographic coordinates"));
	}
	else if( pGrid->Get_Projection().Get_Type() != SG_PROJ_TYPE_CS_Geographic )
	{
		Message_Add(CSG_String::Format(SG_T("\n%s (%s: %s)\n"), _TL("re-projection to geographic coordinates"), _TL("original"), pGrid->Get_Projection().Get_Name().c_str()), false);

		if(	(pModule = SG_Get_Module_Library_Manager().Get_Module(SG_T("pj_proj4"), 4)) == NULL )	// Coordinate Transformation (Grid)
		{
			return( false );
		}

		P.Assign(pModule->Get_Parameters());

		pModule->Set_Manager(NULL);

		if( pModule->Get_Parameters()->Set_Parameter("CRS_PROJ4"    , "+proj=longlat +ellps=WGS84 +datum=WGS84")
		&&  pModule->Get_Parameters()->Set_Parameter("INTERPOLATION", Parameters("INTERPOL")->asBool() ? 4 : 0)
		&&  pModule->Get_Parameters()->Set_Parameter("SOURCE"       , pGrid)
		&&  pModule->Execute() )
		{
			bDelete	= true;

			pGrid	= pModule->Get_Parameters("GET_USER")->Get_Parameter("GRID")->asGrid();

			if( pShade
			&&  pModule->Get_Parameters()->Set_Parameter("SOURCE", pShade)
			&&  pModule->Execute() )
			{
				pShade	= pModule->Get_Parameters("GET_USER")->Get_Parameter("GRID")->asGrid();
			}
			else
			{
				pShade	= NULL;
			}
		}

		pModule->Get_Parameters()->Assign_Values(&P);
		pModule->Set_Manager(P.Get_Manager());

		if( !bDelete )
		{
			Message_Add(CSG_String::Format(SG_T("\n%s: %s\n"), _TL("re-projection"), _TL("failed")), false);

			return( false );
		}
	}

	//-----------------------------------------------------
	if(	(pModule = SG_Get_Module_Library_Manager().Get_Module(SG_T("io_grid_image"), 0)) == NULL )	// Export Image
	{
		return( false );
	}

	P.Assign(pModule->Get_Parameters());

	pModule->Set_Manager(NULL);

	bool	bResult	= false;

	if( pModule->Get_Parameters()->Set_Parameter("GRID"        , pGrid)
	&&  pModule->Get_Parameters()->Set_Parameter("SHADE"       , pShade)
	&&  pModule->Get_Parameters()->Set_Parameter("FILE_KML"    , true)
	&&  pModule->Get_Parameters()->Set_Parameter("FILE"        , Parameters("FILE"))
	&&  pModule->Get_Parameters()->Set_Parameter("COLOURING"   , Parameters("COLOURING"))
	&&  pModule->Get_Parameters()->Set_Parameter("COL_PALETTE" , Parameters("COL_PALETTE"))
	&&  pModule->Get_Parameters()->Set_Parameter("STDDEV"      , Parameters("STDDEV"))
	&&  pModule->Get_Parameters()->Set_Parameter("STRETCH"     , Parameters("STRETCH"))
	&&  pModule->Get_Parameters()->Set_Parameter("LUT"         , Parameters("LUT"))
	&&  pModule->Get_Parameters()->Set_Parameter("SHADE_BRIGHT", Parameters("SHADE_BRIGHT"))
	&&  pModule->Execute() )
	{
		bResult	= true;
	}

	//-----------------------------------------------------
	pModule->Get_Parameters()->Assign_Values(&P);
	pModule->Set_Manager(P.Get_Manager());

	if( bDelete )
	{
		delete(pGrid);

		if( pShade )
		{
			delete(pShade);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
