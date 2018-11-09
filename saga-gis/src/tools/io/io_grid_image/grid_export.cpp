/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Export.cpp                    //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/quantize.h>

#include "grid_export.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Export::CGrid_Export(void)
{
	Set_Name		(_TL("Export Image (bmp, jpg, pcx, png, tif)"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"The tool allows one to save a grid as image.\n"
		"Optionally, a shade grid can be overlayed and it's "
		"transparency and brightness can be adjusted.\n\n")
	);

	Parameters.Add_Grid("",
		"GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SHADE"		, _TL("Shade"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("Image File"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Portable Network Graphics (*.png)"           ), SG_T("*.png"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)"), SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Tagged Image File Format (*.tif, *.tiff)"    ), SG_T("*.tif;*.tiff"),
			_TL("Windows or OS/2 Bitmap (*.bmp)"              ), SG_T("*.bmp"),
			_TL("Zsoft Paintbrush (*.pcx)"                    ), SG_T("*.pcx")
		), NULL, true
	);

	Parameters.Add_Bool("",
		"FILE_KML"	, _TL("Create KML File"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"NO_DATA"	, _TL("Set Transparency for No-Data"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Choice("",
			"COLOURING"		, _TL("Colouring"),
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

		Parameters.Add_Colors("",
			"COL_PALETTE"	, _TL("Colours Palette"),
			_TL("")
		);
	}
	else
	{
		Parameters.Add_Choice("",
			"COLOURING"		, _TL("Colouring"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s",
				_TL("stretch to grid's standard deviation"),
				_TL("stretch to grid's value range"),
				_TL("stretch to specified value range"),
				_TL("lookup table"),
				_TL("rgb coded values")
			), 0
		);

		Parameters.Add_Choice("",
			"COL_PALETTE"	, _TL("Color Palette"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
				_TL("DEFAULT"       ),	_TL("DEFAULT_BRIGHT" ),	_TL("BLACK_WHITE"   ),	_TL("BLACK_RED"     ),
				_TL("BLACK_GREEN"   ),	_TL("BLACK_BLUE"     ),	_TL("WHITE_RED"     ),	_TL("WHITE_GREEN"   ),
				_TL("WHITE_BLUE"    ),	_TL("YELLOW_RED"     ),	_TL("YELLOW_GREEN"  ),	_TL("YELLOW_BLUE"   ),
				_TL("RED_GREEN"     ),	_TL("RED_BLUE"       ),	_TL("GREEN_BLUE"    ),	_TL("RED_GREY_BLUE" ),
				_TL("RED_GREY_GREEN"),	_TL("GREEN_GREY_BLUE"),	_TL("RED_GREEN_BLUE"),	_TL("RED_BLUE_GREEN"),
				_TL("GREEN_RED_BLUE"),	_TL("RAINBOW"        ),	_TL("NEON"          ),	_TL("TOPOGRAPHY"    ),
				_TL("ASPECT_1"      ),	_TL("ASPECT_2"       ),	_TL("ASPECT_3"      )
			), 0
		);

		Parameters.Add_Int("",
			"COL_COUNT"		, _TL("Number of Colors"),
			_TL(""),
			100
		);

		Parameters.Add_Bool("",
			"COL_REVERT"	, _TL("Revert Palette"),
			_TL(""),
			false
		);
	}

	Parameters.Add_Double("",
		"STDDEV"		, _TL("Standard Deviation"),
		_TL(""),
		2.0, 0.0, true
	);

	Parameters.Add_Range("",
		"STRETCH"		, _TL("Stretch to Value Range"),
        _TL(""),
        0.0, 100.0
    );

	Parameters.Add_Choice("",
		"SCALE_MODE"	, _TL("Scaling Mode"),
		_TL("Scaling mode applied to colouring choices (i) grid's standard deviation, (ii) grid's value range, (iii) specified value range"),
		CSG_String::Format("%s|%s|%s",
			_TL("Linear"),
			_TL("Logarithmic (up)"),
			_TL("Logarithmic (down)")
		), 0
	);

	Parameters.Add_Double("",
		"SCALE_LOG"		, _TL("Logarithmic Scale Factor"),
		_TL(""),
		1.0, 0.001, true
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"LUT"			, _TL("Lookup Table"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"SHADE_TRANS"	, _TL("Shade Transparency [%]"),
		_TL("The transparency of the shade [%]"),
		40.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Choice("",
		"SHADE_COLOURING", _TL("Shade Colouring"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("scale to brightness range"),
			_TL("stretch to grid's standard deviation")
		), 0
	);

	Parameters.Add_Range("SHADE_COLOURING",
		"SHADE_BRIGHT"	, _TL("Shade Brightness [%]"),
		_TL("Allows one to scale shade brightness [%]"),
		0.0, 100.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Double("SHADE_COLOURING",
		"SHADE_STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		2.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Export::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("COLOURING") )
	{
		pParameters->Set_Enabled("COL_PALETTE"    , pParameter->asInt() <= 2);
		pParameters->Set_Enabled("STDDEV"         , pParameter->asInt() == 0);
		pParameters->Set_Enabled("STRETCH"        , pParameter->asInt() == 2);
		pParameters->Set_Enabled("SCALE_MODE"     , pParameter->asInt() <= 2);
		pParameters->Set_Enabled("SCALE_LOG"      , pParameter->asInt() <= 2 && pParameters->Get_Parameter("SCALE_MODE")->asInt() > 0);
		pParameters->Set_Enabled("LUT"            , pParameter->asInt() == 3);
	}

	if(	pParameter->Cmp_Identifier("SCALE_MODE") )
	{
		pParameters->Set_Enabled("SCALE_LOG"      , pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier("SHADE") )
	{
		pParameters->Set_Enabled("SHADE_TRANS"    , pParameter->asPointer() != NULL);
		pParameters->Set_Enabled("SHADE_COLOURING", pParameter->asPointer() != NULL);
		pParameters->Set_Enabled("SHADE_BRIGHT"   , pParameter->asPointer() != NULL);
		pParameters->Set_Enabled("SHADE_STDDEV"   , pParameter->asPointer() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::On_Execute(void)
{
	//-----------------------------------------------------
	int	y, Colouring	= Parameters("COLOURING")->asInt();

	CSG_Grid	*pGrid	= Parameters("GRID" )->asGrid(),  Grid;
	CSG_Grid	*pShade	= Parameters("SHADE")->asGrid(), Shade;

	//-----------------------------------------------------
	if( Colouring == 5 )	// same as in graphical user interface
	{
		if( !SG_UI_DataObject_asImage(pGrid, &Grid) )
		{
			Error_Set("could not retrieve colour coding from graphical user interface.");

			return( false );
		}
	}
	else
	{
		double		zMin, zMax, zScale;
		CSG_Colors	Colors;
		CSG_Table	LUT;

		int		ScaleMode	= Parameters("SCALE_MODE")->asInt();
		double	ScaleLog	= Parameters("SCALE_LOG" )->asDouble();

		if( SG_UI_Get_Window_Main() )
		{
			Colors.Assign(Parameters("COL_PALETTE")->asColors());
		}
		else
		{
			Colors.Set_Palette(
				Parameters("COL_PALETTE")->asInt (),
				Parameters("COL_REVERT" )->asBool(),
				Parameters("COL_COUNT"  )->asInt ()
			);
		}

		switch( Colouring )
		{
		case 0:	// stretch to grid's standard deviation
			zMin	= pGrid->Get_Mean() - Parameters("STDDEV")->asDouble() * pGrid->Get_StdDev(); if( zMin < pGrid->Get_Min() ) zMin = pGrid->Get_Min();
			zMax	= pGrid->Get_Mean() + Parameters("STDDEV")->asDouble() * pGrid->Get_StdDev(); if( zMax > pGrid->Get_Max() ) zMax = pGrid->Get_Max();
			zScale	= Colors.Get_Count() / (zMax - zMin);
			break;

		case 1:	// stretch to grid's value range
			zMin	= pGrid->Get_Min();
			zMax	= pGrid->Get_Max();
			zScale	= Colors.Get_Count() / pGrid->Get_Range();
			break;

		case 2:	// stretch to specified value range
			zMin	= Parameters("STRETCH")->asRange()->Get_Min();
			zMax	= Parameters("STRETCH")->asRange()->Get_Max();
			if( zMin >= zMax )
			{
				Error_Set(_TL("invalid user specified value range."));

				return( false );
			}
			zScale	= Colors.Get_Count() / (zMax - zMin);
			break;

		case 3:	// lookup table
			if( !Parameters("LUT")->asTable() || Parameters("LUT")->asTable()->Get_Field_Count() < 5 )
			{
				Error_Set(_TL("invalid lookup table."));

				return( false );
			}

			LUT.Create(*Parameters("LUT")->asTable());
			break;

		case 4:	// rgb coded values
			break;
		}

		//-------------------------------------------------
		Grid.Create(Get_System(), SG_DATATYPE_Int);

		#pragma omp parallel for private(y)
		for(y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				double	z	= pGrid->asDouble(x, y);

				if( Colouring == 3 )	// lookup table
				{
					int	i, iColor	= -1;

					for(i=0; iColor<0 && i<LUT.Get_Count(); i++)
					{
						if( z == LUT[i][3] )
						{
							Grid.Set_Value(x, y, LUT[iColor = i].asInt(0));
						}
					}

					for(i=0; iColor<0 && i<LUT.Get_Count(); i++)
					{
						if( z >= LUT[i][3] && z <= LUT[i][4] )
						{
							Grid.Set_Value(x, y, LUT[iColor = i].asInt(0));
						}
					}

					if( iColor < 0 )
					{
						Grid.Set_NoData(x, y);
					}
				}
				else if( pGrid->is_NoData(x, y) )
				{
					Grid.Set_NoData(x, y);
				}
				else if( Colouring == 4 )	// rgb coded values
				{
					Grid.Set_Value(x, y, z);
				}
				else
				{
					int i = 0;

					if( ScaleMode != 0 )
					{
						if( pGrid->Get_Range() != 0 )
						{
							double Value = (z - zMin) / (zMax - zMin);

							if( ScaleMode == 1 )	// logarithmic up
							{
								if( Value > 0.0 )
								{
									Value	= log(1.0 + ScaleLog * Value) / log(1.0 + ScaleLog);
								}
								else
								{
									Value	= 0.0;
								}
							}
							else					// logarithmic down
							{
								if( Value < 1.0 )
								{
									Value	= 1.0 - Value;
									Value	= log(1.0 + ScaleLog * Value) / log(1.0 + ScaleLog);
									Value	= 1.0 - Value;
								}
								else
								{
									Value	= 1.0;
								}
							}

							i	= (int)(Value * Colors.Get_Count());
						}
					}
					else	// linear
					{
						i	= (int)(zScale * (z - zMin));
					}

					Grid.Set_Value(x, y, Colors[i < 0 ? 0 : i >= Colors.Get_Count() ? Colors.Get_Count() - 1 : i]);
				}
			}
		}
	}

	//-----------------------------------------------------
	double	dMinBright	= Parameters("SHADE_BRIGHT")->asRange()->Get_Min() / 100.0;
	double	dMaxBright	= Parameters("SHADE_BRIGHT")->asRange()->Get_Max() / 100.0;
	double	dShadeTrans	= Parameters("SHADE_TRANS")->asDouble() / 100.0;

	if( !pShade || pShade->Get_Range() <= 0.0 ||  dMinBright >= dMaxBright )
	{
		pShade	= NULL;
	}
	else
	{
		CSG_Colors	Colors(100, SG_COLORS_BLACK_WHITE, true);

		Shade.Create(Get_System(), SG_DATATYPE_Int);

		double	minShade = 0.0, maxShade = 0.0, scaleShade = 0.0;

		if( Parameters("SHADE_COLOURING")->asInt() == 1 )
		{
			minShade	= pShade->Get_Mean() - Parameters("SHADE_STDDEV")->asDouble() * pShade->Get_StdDev();
			maxShade	= pShade->Get_Mean() + Parameters("SHADE_STDDEV")->asDouble() * pShade->Get_StdDev();
			if( minShade < pShade->Get_Min() )
				minShade = pShade->Get_Min();
			if( maxShade > pShade->Get_Max() )
				maxShade = pShade->Get_Max();
			scaleShade	= Colors.Get_Count() / (maxShade - minShade);
		}

		#pragma omp parallel for private(y)
		for(y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( pShade->is_NoData(x, y) )
				{
					Shade.Set_NoData(x, y);
				}
				else if( Parameters("SHADE_COLOURING")->asInt() == 0 )
				{
					Shade.Set_Value (x, y, Colors[(int)(Colors.Get_Count() * (dMaxBright - dMinBright) * (pShade->asDouble(x, y) - pShade->Get_Min()) / pShade->Get_Range() + dMinBright)]);
				}
				else
				{
					int i	= (int)(scaleShade * (pShade->asDouble(x, y) - minShade));
					Shade.Set_Value(x, y, Colors[i < 0 ? 0 : i >= Colors.Get_Count() ? Colors.Get_Count() - 1 : i]);
				}
			}
		}
	}

	//-----------------------------------------------------
	wxImage	Image(Get_NX(), Get_NY());

	if( Parameters("NO_DATA")->asBool() && pGrid->Get_NoData_Count() > 0 )
	{
		Image.SetAlpha();
	}

	#ifndef _DEBUG
	#pragma omp parallel for private(y)
	#endif
	for(y=0; y<Get_NY(); y++)
	{
		int	iy	= Get_NY() - y - 1;

		for(int x=0; x<Get_NX(); x++)
		{
			int	c	= Grid.asInt(x, y);

			int	r	= SG_GET_R(c);
			int	g	= SG_GET_G(c);
			int	b	= SG_GET_B(c);

			if( pGrid->is_NoData(x, y) || (pShade != NULL && Shade.is_NoData(x, y)) )
			{
				if( Image.HasAlpha() )
				{
					Image.SetAlpha(x, iy, wxIMAGE_ALPHA_TRANSPARENT);
				}
			}
			else
			{
				if( Image.HasAlpha() )
				{
					Image.SetAlpha(x, iy, wxIMAGE_ALPHA_OPAQUE);
				}

				if( pShade )
				{
					c	= Shade.asInt(x, y);

					r	= dShadeTrans * r + SG_GET_R(c) * (1.0 - dShadeTrans);
					g	= dShadeTrans * g + SG_GET_G(c) * (1.0 - dShadeTrans);
					b	= dShadeTrans * b + SG_GET_B(c) * (1.0 - dShadeTrans);
				}
			}

			Image.SetRGB(x, iy, r, g, b);
		}
	}

	if( Parameters("NO_DATA")->asBool() && pGrid->Get_NoData_Count() > 0 )
	{
		wxQuantize::Quantize(Image, Image);

		Image.ConvertAlphaToMask();
	}

	//-------------------------------------------------
	CSG_String	fName(Parameters("FILE")->asString());

	if( !SG_File_Cmp_Extension(fName, "bmp")
	&&  !SG_File_Cmp_Extension(fName, "jpg")
	&&  !SG_File_Cmp_Extension(fName, "pcx")
	&&  !SG_File_Cmp_Extension(fName, "png")
	&&  !SG_File_Cmp_Extension(fName, "tif") )
	{
		fName	= SG_File_Make_Path("", fName, "png");

		Parameters("FILE")->Set_Value(fName);
	}

	//-----------------------------------------------------
	wxImageHandler	*pImgHandler = NULL;

	if( !SG_UI_Get_Window_Main() )
	{
		if     ( SG_File_Cmp_Extension(fName, "jpg") )	pImgHandler = new wxJPEGHandler;
		else if( SG_File_Cmp_Extension(fName, "pcx") )	pImgHandler = new wxPCXHandler ;
		else if( SG_File_Cmp_Extension(fName, "tif") )	pImgHandler = new wxTIFFHandler;
#ifdef _SAGA_MSW
		else if( SG_File_Cmp_Extension(fName, "bmp") )	pImgHandler = new wxBMPHandler ;
#endif
		else/*if(SG_File_Cmp_Extension(fName, "png") )*/pImgHandler = new wxPNGHandler ;

		wxImage::AddHandler(pImgHandler);
	}

	if( !Image.SaveFile(fName.c_str()) )
	{
		Error_Fmt("%s [%s]", _TL("failed to save image file"), fName.c_str());

		return( false );
	}

	pGrid->Get_Projection().Save(SG_File_Make_Path("", fName, "prj"), SG_PROJ_FMT_WKT);

	//-----------------------------------------------------
	CSG_File	Stream;

	if(      SG_File_Cmp_Extension(fName, "bmp") ) Stream.Open(SG_File_Make_Path("", fName, "bpw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "jpg") ) Stream.Open(SG_File_Make_Path("", fName, "jgw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "pcx") ) Stream.Open(SG_File_Make_Path("", fName, "pxw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "png") ) Stream.Open(SG_File_Make_Path("", fName, "pgw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "tif") ) Stream.Open(SG_File_Make_Path("", fName, "tfw"), SG_FILE_W, false);

	if( Stream.is_Open() )
	{
		Stream.Printf("%.10f\n%f\n%f\n%.10f\n%.10f\n%.10f\n",
			 pGrid->Get_Cellsize(),
			 0.0, 0.0,
			-pGrid->Get_Cellsize(),
			 pGrid->Get_XMin(),
			 pGrid->Get_YMax()
		);
	}

	//-----------------------------------------------------
	if( Parameters("FILE_KML")->asBool() )
	{
		CSG_MetaData	KML;	KML.Set_Name("kml");	KML.Add_Property("xmlns", "http://www.opengis.net/kml/2.2");

	//	CSG_MetaData	*pFolder	= KML.Add_Child("Folder");
	//	pFolder->Add_Child("name"       , "Raster exported from SAGA");
	//	pFolder->Add_Child("description", "System for Automated Geoscientific Analyses - www.saga-gis.org");
	//	CSG_MetaData	*pOverlay	= pFolder->Add_Child("GroundOverlay");

		CSG_MetaData	*pOverlay	= KML.Add_Child("GroundOverlay");
		pOverlay->Add_Child("name"       , pGrid->Get_Name());
		pOverlay->Add_Child("description", pGrid->Get_Description());
		pOverlay->Add_Child("Icon"       )->Add_Child("href", SG_File_Get_Name(fName, true));
		pOverlay->Add_Child("LatLonBox"  );
		pOverlay->Get_Child("LatLonBox"  )->Add_Child("north", pGrid->Get_YMax());
		pOverlay->Get_Child("LatLonBox"  )->Add_Child("south", pGrid->Get_YMin());
		pOverlay->Get_Child("LatLonBox"  )->Add_Child("east" , pGrid->Get_XMax());
		pOverlay->Get_Child("LatLonBox"  )->Add_Child("west" , pGrid->Get_XMin());

		KML.Save(fName, SG_T("kml"));
	}

	//-----------------------------------------------------
	if( !SG_UI_Get_Window_Main() && pImgHandler != NULL)
	{
		wxImage::RemoveHandler(pImgHandler->GetName());
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
