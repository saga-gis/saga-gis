
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
	Set_Name		(_TL("Export to Image File"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"With this tool you can save a RGB to an image file. "
		"Optionally, a shade RGB can be overlaid using the "
		"specified transparency and brightness adjustment. "
		"\nIf input rasters come with geographic coordinates an additional"
		"option for KML file creation (Google Earth) is selectable. "
	));

	Parameters.Add_Grid("", "GRID" , _TL("GRID" ), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid("", "SHADE", _TL("Shade"), _TL(""), PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_FilePath("",
		"FILE"           , _TL("Image File"),
		_TL(""),
		CSG_String::Format(
			"%s (*.png)"                "|*.png|"
			"%s (*.jpg, *.jif, *.jpeg)" "|*.jpg;*.jif;*.jpeg|"
			"%s (*.tif, *.tiff)"        "|*.tif;*.tiff|"
			"%s (*.gif)"                "|*.gif|"
			"%s (*.bmp)"                "|*.bmp|"
			"%s (*.pcx)"                "|*.pcx",
			_TL("Portable Network Graphics"  ),
			_TL("JPEG - JFIF Compliant"      ),
			_TL("Tagged Image File Format"   ),
			_TL("Graphics Interchange Format"),
			_TL("Windows or OS/2 Bitmap"     ),
			_TL("Zsoft Paintbrush"           )
		), NULL, true
	);

	Parameters.Add_Bool("",
		"FILE_WORLD"     , _TL("Create World File"),
		_TL("Store georeference along image to an additional file."),
		true
	);

	Parameters.Add_Bool("",
		"FILE_KML"       , _TL("Create KML File"),
		_TL("Expects that the input RGB uses geographic coordinates."),
		false
	);

	Parameters.Add_Bool("",
		"NO_DATA"        , _TL("Set Transparency for No-Data"),
		_TL(""),
		true
	);

	Parameters.Add_Color("NO_DATA",
		"NO_DATA_COL"    , _TL("No-Data Color"),
		_TL(""),
		SG_COLOR_WHITE
	);

	//-----------------------------------------------------
	if( has_GUI() )
	{
		Parameters.Add_Choice("",
			"COLOURING"  , _TL("Coloring"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s|%s",
				_TL("histogram stretch to standard deviation"),
				_TL("histogram stretch to percentage range"),
				_TL("histogram stretch to value range"),
				_TL("lookup table"),
				_TL("rgb coded values"),
				_TL("same as in graphical user interface")
			), 5
		);
	}
	else
	{
		Parameters.Add_Choice("",
			"COLOURING"  , _TL("Coloring"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s|%s",
				_TL("histogram stretch to standard deviation"),
				_TL("histogram stretch to percentage range"),
				_TL("histogram stretch to value range"),
				_TL("lookup table"),
				_TL("rgb coded values")
			), 0
		);

		Parameters.Add_Int("",
			"COL_COUNT"  , _TL("Number of Colors"),
			_TL(""),
			100
		);

		Parameters.Add_Bool("",
			"COL_REVERT" , _TL("Revert Palette"),
			_TL(""),
			false
		);
	}

	Parameters.Add_Colors("",
		"COL_PALETTE"    , _TL("Colors Palette"),
		_TL(""),
		SG_COLORS_RAINBOW
	);

	Parameters.Add_Int("",
		"COL_DEPTH"      , _TL("Color Depth"),
		_TL("Number of color entries used when storing GIF."),
		236, 2, true, 256, true
	);

	Parameters.Add_Bool("COL_PALETTE",
		"GRADUATED"      , _TL("Graduated Colors"),
		_TL(""),
		true
	);

	Parameters.Add_Double("COL_PALETTE",
		"STDDEV"         , _TL("Standard Deviation"),
		_TL(""),
		2., 0., true
	);

	Parameters.Add_Range("COL_PALETTE",
		"LINEAR"         , _TL("Percentage Range"),
		_TL(""),
		0., 100.
	);

	Parameters.Add_Range("COL_PALETTE",
		"STRETCH"        , _TL("Value Range"),
		_TL(""),
		0., 100.
	);

	Parameters.Add_Choice("COL_PALETTE",
		"SCALE_MODE"     , _TL("Scaling"),
		_TL("Scaling applied to coloring choices (i) RGB's standard deviation, (ii) RGB's value range, (iii) specified value range"),
		CSG_String::Format("%s|%s|%s",
			_TL("linear intervals"),
			_TL("increasing geometrical intervals"),
			_TL("decreasing geometrical intervals")
		), 0
	);

	Parameters.Add_Double("SCALE_MODE",
		"SCALE_LOG"      , _TL("Geometrical Interval Factor"),
		_TL(""),
		10., 0.001, true
	);

	//-----------------------------------------------------
	CSG_Table *pLUT = Parameters.Add_FixedTable("", "LUT", _TL("Lookup Table"), _TL(""))->asTable();

	pLUT->Set_Name(_TL("Table"));
	pLUT->Add_Field("Color"      , SG_DATATYPE_Color );
	pLUT->Add_Field("Name"       , SG_DATATYPE_String);
	pLUT->Add_Field("Description", SG_DATATYPE_String);
	pLUT->Add_Field("Minimum"    , SG_DATATYPE_Double);
	pLUT->Add_Field("Maximum"    , SG_DATATYPE_Double);
	pLUT->Add_Record();

	//-----------------------------------------------------
	Parameters.Add_Node("", "SHADE_NODE", _TL("Shading"), _TL(""));

	Parameters.Add_Double("SHADE_NODE",
		"SHADE_TRANS", _TL("Transparency"),
		_TL("The transparency of the shade [%]"),
		75., 0., true, 100., true
	);

	Parameters.Add_Choice("SHADE_NODE",
		"SHADE_COLOURING", _TL("Histogram Stretch"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Linear"),
			_TL("Standard Deviation")
		), 0
	);

	Parameters.Add_Range("SHADE_NODE",
		"SHADE_BRIGHT"   , _TL("Linear"),
		_TL("Minimum and maximum [%], the range for histogram stretch."),
		0., 100.
	);

	Parameters.Add_Double("SHADE_NODE",
		"SHADE_STDDEV"   , _TL("Standard Deviation"),
		_TL(""),
		2., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Export::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID") && pParameter->asGrid() )
	{
		pParameters->Set_Parameter("STRETCH.MIN", pParameter->asGrid()->Get_Min());
		pParameters->Set_Parameter("STRETCH.MAX", pParameter->asGrid()->Get_Max());
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Export::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID") )
	{
		pParameters->Set_Enabled("FILE_KML"     , pParameter->asPointer() && pParameter->asGrid()->Get_Projection().Get_Type() == ESG_CRS_Type::Geographic);
	}

	if( pParameter->Cmp_Identifier("FILE") )
	{
		pParameters->Set_Enabled("COL_DEPTH"    , SG_File_Cmp_Extension(pParameter->asString(), "gif"));
	}

	if( pParameter->Cmp_Identifier("NO_DATA") )
	{
		pParameters->Set_Enabled("NO_DATA_COL"  , pParameter->asBool() == false);
	}

	if( pParameter->Cmp_Identifier("COLOURING") )
	{
		pParameters->Set_Enabled("COL_PALETTE"  , pParameter->asInt() <= 2);
		pParameters->Set_Enabled("STDDEV"       , pParameter->asInt() == 0);
		pParameters->Set_Enabled("LINEAR"       , pParameter->asInt() == 1);
		pParameters->Set_Enabled("STRETCH"      , pParameter->asInt() == 2);
		pParameters->Set_Enabled("SCALE_MODE"   , pParameter->asInt() <= 2);
		pParameters->Set_Enabled("LUT"          , pParameter->asInt() == 3);
		pParameters->Set_Enabled("NO_DATA"      , pParameter->asInt() != 5);
	}

	if( pParameter->Cmp_Identifier("SCALE_MODE") )
	{
		pParameters->Set_Enabled("SCALE_LOG"    , pParameter->asInt() > 0);
	}

	if( pParameter->Cmp_Identifier("SHADE") )
	{
		pParameters->Set_Enabled("SHADE_NODE"   , pParameter->asPointer() != NULL);
	}

	if( pParameter->Cmp_Identifier("SHADE_COLOURING") )
	{
		pParameters->Set_Enabled("SHADE_BRIGHT" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("SHADE_STDDEV" , pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::On_Execute(void)
{
	CSG_Grid RGB(Get_System(), SG_DATATYPE_Int);

	switch( Parameters("COLOURING")->asInt() )
	{
	default: if( !Set_Metric(*Parameters("GRID")->asGrid(), RGB) ) { return( false ); } break; // stretch to RGB's standard deviation
	case  3: if( !Set_LUT   (*Parameters("GRID")->asGrid(), RGB) ) { return( false ); } break; // lookup table
	case  4: if( !Set_RGB   (*Parameters("GRID")->asGrid(), RGB) ) { return( false ); } break; // rgb coded values
	case  5: if( !Set_GUI   (*Parameters("GRID")->asGrid(), RGB) ) { return( false ); } break; // same as in graphical user interface
	}

	Add_Shading(RGB);

	//-----------------------------------------------------
	wxImage Image(Get_NX(), Get_NY()); long BgColor = Parameters("NO_DATA_COL")->asColor();

	if( Parameters("NO_DATA")->asBool() && RGB.Get_NoData_Count() > 0 )
	{
		Image.SetAlpha(); BgColor = SG_COLOR_WHITE;
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( RGB.is_NoData(x, y) == false )
		{
			int rgb = RGB.asInt(x, y);

			Image.SetRGB(x, y, SG_GET_R(rgb), SG_GET_G(rgb), SG_GET_B(rgb));

			if( Image.HasAlpha() )
			{
				Image.SetAlpha(x, y, wxIMAGE_ALPHA_OPAQUE);
			}
		}
		else
		{
			Image.SetRGB(x, y, SG_GET_R(BgColor), SG_GET_G(BgColor), SG_GET_B(BgColor));

			if( Image.HasAlpha() )
			{
				Image.SetAlpha(x, y, wxIMAGE_ALPHA_TRANSPARENT);
			}
		}
	}

	//-------------------------------------------------
	CSG_String File(Parameters("FILE")->asString());

	if( SG_File_Cmp_Extension(File, "gif") )
	{
		if( Image.HasAlpha() )
		{
			Image.ConvertAlphaToMask();
		}

		wxImage _Image(Image);

		wxQuantize::Quantize(_Image, Image, Parameters["COL_DEPTH"].asInt());
	}

	if( !SG_File_Cmp_Extension(File, "bmp")
	&&  !SG_File_Cmp_Extension(File, "jpg")
	&&  !SG_File_Cmp_Extension(File, "pcx")
	&&  !SG_File_Cmp_Extension(File, "png")
	&&  !SG_File_Cmp_Extension(File, "gif")
	&&  !SG_File_Cmp_Extension(File, "tif") )
	{
		File = SG_File_Make_Path("", File, "png");

		Parameters("FILE")->Set_Value(File);
	}

	//-----------------------------------------------------
	wxImageHandler *pHandler = NULL;

	if( !SG_UI_Get_Window_Main() )
	{
		#ifdef _SAGA_MSW
		if( SG_File_Cmp_Extension(File, "bmp") ) { wxImage::AddHandler(pHandler = new wxBMPHandler ); } else
		#endif
		if( SG_File_Cmp_Extension(File, "jpg") ) { wxImage::AddHandler(pHandler = new wxJPEGHandler); } else
		if( SG_File_Cmp_Extension(File, "pcx") ) { wxImage::AddHandler(pHandler = new wxPCXHandler ); } else
		if( SG_File_Cmp_Extension(File, "tif") ) { wxImage::AddHandler(pHandler = new wxTIFFHandler); } else
		if( SG_File_Cmp_Extension(File, "gif") ) { wxImage::AddHandler(pHandler = new wxGIFHandler ); } else
												{ wxImage::AddHandler(pHandler = new wxPNGHandler ); }
	}

	bool bOkay = Image.SaveFile(File.c_str());

	if( !SG_UI_Get_Window_Main() && pHandler )
	{
		wxImage::RemoveHandler(pHandler->GetName());
	}

	if( !bOkay )
	{
		Error_Fmt("%s [%s]", _TL("failed to save image file"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("FILE_WORLD")->asBool() )
	{
		CSG_File Stream;

		if( SG_File_Cmp_Extension(File, "bmp") ) { Stream.Open(SG_File_Make_Path("", File, "bpw"), SG_FILE_W, false); } else
		if( SG_File_Cmp_Extension(File, "jpg") ) { Stream.Open(SG_File_Make_Path("", File, "jgw"), SG_FILE_W, false); } else
		if( SG_File_Cmp_Extension(File, "pcx") ) { Stream.Open(SG_File_Make_Path("", File, "pxw"), SG_FILE_W, false); } else
		if( SG_File_Cmp_Extension(File, "png") ) { Stream.Open(SG_File_Make_Path("", File, "pgw"), SG_FILE_W, false); } else
		if( SG_File_Cmp_Extension(File, "gif") ) { Stream.Open(SG_File_Make_Path("", File, "gfw"), SG_FILE_W, false); } else
		if( SG_File_Cmp_Extension(File, "tif") ) { Stream.Open(SG_File_Make_Path("", File, "tfw"), SG_FILE_W, false); }

		if( Stream.is_Open() )
		{
			Stream.Printf("%.10f\n%f\n%f\n%.10f\n%.10f\n%.10f\n",
				Get_Cellsize(), 0., 0., -Get_Cellsize(), Get_XMin(), Get_YMax()
			);

			CSG_Projection Projection = Parameters("GRID")->asGrid()->Get_Projection();

			if( Projection.is_Okay() && Stream.Open(File + ".aux.xml", SG_FILE_W, false) )
			{
				Stream.Write("<PAMDataset><SRS>" + Projection.Get_WKT() + "</SRS></PAMDataset>");

				Projection.Save(SG_File_Make_Path("", File, "prj"), ESG_CRS_Format::WKT);
			}
		}
	}

	//-----------------------------------------------------
	if( Parameters("FILE_KML")->asBool() && Parameters("GRID")->asGrid()->Get_Projection().is_Geographic() )
	{
		CSG_MetaData KML; KML.Set_Name("kml"); KML.Add_Property("xmlns", "http://www.opengis.net/kml/2.2");

		CSG_MetaData &Overlay = *KML.Add_Child("GroundOverlay");

		Overlay.Add_Child("name"       , Parameters("GRID")->asGrid()->Get_Name());
		Overlay.Add_Child("description", Parameters("GRID")->asGrid()->Get_Description());
		Overlay.Add_Child("Icon"       )->Add_Child("href" , SG_File_Get_Name(File, true));
		Overlay.Add_Child("LatLonBox"  );
		Overlay.Get_Child("LatLonBox"  )->Add_Child("north", Get_YMax());
		Overlay.Get_Child("LatLonBox"  )->Add_Child("south", Get_YMin());
		Overlay.Get_Child("LatLonBox"  )->Add_Child("east" , Get_XMax());
		Overlay.Get_Child("LatLonBox"  )->Add_Child("west" , Get_XMin());

		KML.Save(File, SG_T("kml"));
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::Set_Metric(CSG_Grid &Grid, CSG_Grid &RGB)
{
	CSG_Colors Colors(*Parameters("COL_PALETTE")->asColors());

	if( !has_GUI() )
	{
		Colors.Set_Count(Parameters("COL_COUNT")->asInt());

		if( Parameters("COL_REVERT")->asBool() )
		{
			Colors.Revert();
		}
	}

	bool bGraduated = Parameters("GRADUATED")->asBool();

	//-----------------------------------------------------
	double Minimum, Maximum, Scale = Parameters("SCALE_LOG")->asDouble();

	int Mode = Parameters("SCALE_MODE")->asInt();

	switch( Parameters("COLOURING")->asInt() )
	{
	default: // histogram stretch to standard deviation
		Minimum = Grid.Get_Mean() - Parameters("STDDEV")->asDouble() * Grid.Get_StdDev(); if( Minimum < Grid.Get_Min() ) Minimum = Grid.Get_Min();
		Maximum = Grid.Get_Mean() + Parameters("STDDEV")->asDouble() * Grid.Get_StdDev(); if( Maximum > Grid.Get_Max() ) Maximum = Grid.Get_Max();
		break;

	case  1: // histogram stretch to percentage range
		Minimum = Grid.Get_Min() + Grid.Get_Range() * Parameters("LINEAR.MIN")->asDouble() / 100.;
		Maximum = Grid.Get_Max() + Grid.Get_Range() * Parameters("LINEAR.MAX")->asDouble() / 100.;
		break;

	case  2: // histogram stretch to value range
		Minimum = Parameters("STRETCH.MIN")->asDouble();
		Maximum = Parameters("STRETCH.MAX")->asDouble();
		break;
	}

	if( Minimum >= Maximum || Grid.Get_Range() <= 0. )
	{
		Error_Set(_TL("invalid user specified value range."));

		return( false );
	}

	//-------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0, yy=Get_NY()-y-1; x<Get_NX(); x++)
	{
		if( Grid.is_NoData(x, yy) )
		{
			RGB.Set_NoData(x, y);
		}
		else
		{
			double z = (Grid.asDouble(x, yy) - Minimum) / (Maximum - Minimum);

			switch( Mode )
			{
			case 1: // logarithmic up
				z = z <= 0. ? 0. : log(1. + Scale * z) / log(1. + Scale);
				break;

			case 2: // logarithmic down
				z = 1. - z;
				z = z <= 0. ? 0. : log(1. + Scale * z) / log(1. + Scale);
				z = 1. - z;
				break;
			}

			z *= Colors.Get_Count();

			RGB.Set_Value(x, y, bGraduated ? Colors.Get_Interpolated(z) : Colors.Get_Color((int)z));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Export::Set_LUT(CSG_Grid &Grid, CSG_Grid &RGB)
{
	CSG_Table LUT;

	if( !Parameters("LUT")->asTable() || Parameters("LUT")->asTable()->Get_Field_Count() < 5 )
	{
		Error_Set(_TL("invalid lookup table."));

		return( false );
	}

	LUT.Create(*Parameters("LUT")->asTable());

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0, yy=Get_NY()-y-1; x<Get_NX(); x++)
	{
		double z = Grid.asDouble(x, yy); int Class = -1;

		for(int i=0; Class<0 && i<LUT.Get_Count(); i++)
		{
			if( z == LUT[i][3] )
			{
				RGB.Set_Value(x, y, LUT[Class = i].asInt(0));
			}
		}

		for(int i=0; Class<0 && i<LUT.Get_Count(); i++)
		{
			if( z >= LUT[i][3] && z <= LUT[i][4] )
			{
				RGB.Set_Value(x, y, LUT[Class = i].asInt(0));
			}
		}

		if( Class < 0 )
		{
			RGB.Set_NoData(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Export::Set_RGB(CSG_Grid &Grid, CSG_Grid &RGB)
{
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0, yy=Get_NY()-y-1; x<Get_NX(); x++)
	{
		if( Grid.is_NoData(x, yy) )
		{
			RGB.Set_NoData(x, y);
		}
		else
		{
			RGB.Set_Value(x, y, Grid.asDouble(x, yy));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Export::Set_GUI(CSG_Grid &Grid, CSG_Grid &RGB)
{
	if( !SG_UI_DataObject_asImage(&Grid, &RGB) )
	{
		Error_Set("could not retrieve color coding from graphical user interface.");

		return( false );
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0, yy=Get_NY()-y-1; x<Get_NX(); x++)
	{
		if( Grid.is_NoData(x, y) )
		{
			RGB.Set_NoData(x, yy);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::Add_Shading(CSG_Grid &RGB)
{
	CSG_Grid *pGrid = Parameters("SHADE")->asGrid();

	if( !pGrid || pGrid->Get_Range() <= 0. || Parameters("SHADE_BRIGHT.MIN")->asDouble() >= Parameters("SHADE_BRIGHT.MAX")->asDouble() )
	{
		return( false );
	}

	double Minimum, Maximum, Transparency = Parameters("SHADE_TRANS")->asDouble() / 100.;

	switch( Parameters("SHADE_COLOURING")->asInt() )
	{
	default:
		Minimum = pGrid->Get_Min() + pGrid->Get_Range() * Parameters("SHADE_BRIGHT.MIN")->asDouble() / 100.;
		Maximum = pGrid->Get_Min() + pGrid->Get_Range() * Parameters("SHADE_BRIGHT.MAX")->asDouble() / 100.;
		break;

	case  1:
		Minimum	= pGrid->Get_Mean() - pGrid->Get_StdDev() * Parameters("SHADE_STDDEV")->asDouble(); if( Minimum < pGrid->Get_Min() ) Minimum = pGrid->Get_Min();
		Maximum	= pGrid->Get_Mean() + pGrid->Get_StdDev() * Parameters("SHADE_STDDEV")->asDouble(); if( Maximum > pGrid->Get_Max() ) Maximum = pGrid->Get_Max();
		break;
	}

	if( Minimum >= Maximum )
	{
		return( false );
	}

	double Scale = 255. * (1. - Transparency) / (Maximum - Minimum);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY(); y++) for(int x=0, yy=Get_NY()-y-1; x<Get_NX(); x++)
	{
		if( !RGB.is_NoData(x, y) && !pGrid->is_NoData(x, yy) )
		{
			double d = Scale * (Maximum - pGrid->asDouble(x, yy)); int rgb = RGB.asInt(x, y);

			int r = (int)(Transparency * SG_GET_R(rgb) + d); if( r < 0 ) r = 0; else if( r > 255 ) r = 255;
			int g = (int)(Transparency * SG_GET_G(rgb) + d); if( g < 0 ) g = 0; else if( g > 255 ) g = 255;
			int b = (int)(Transparency * SG_GET_B(rgb) + d); if( b < 0 ) b = 0; else if( b > 255 ) b = 255;

			RGB.Set_Value(x, y, SG_GET_RGB(r, g, b));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
