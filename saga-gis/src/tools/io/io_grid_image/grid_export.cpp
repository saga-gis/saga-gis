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
		"With this tool you can save a grid to an image file. "
		"Optionally, a shade grid can be overlayed using the "
		"specified transparency and brightness adjustment. "
	));

	Parameters.Add_Grid("", "GRID" , _TL("Grid" ), _TL(""), PARAMETER_INPUT         );
	Parameters.Add_Grid("", "SHADE", _TL("Shade"), _TL(""), PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("Image File"),
		_TL(""),
		CSG_String::Format("%s (*.png)|*.png|%s (*.jpg, *.jif, *.jpeg)|*.jpg;*.jif;*.jpeg|%s (*.tif, *.tiff)|*.tif;*.tiff|%s (*.bmp)|*.bmp|%s (*.pcx)|*.pcx",
			_TL("Portable Network Graphics"),
			_TL("JPEG - JFIF Compliant"),
			_TL("Tagged Image File Format"),
			_TL("Windows or OS/2 Bitmap"),
			_TL("Zsoft Paintbrush")
		), NULL, true
	);

	Parameters.Add_Bool("",
		"FILE_KML"	, _TL("Create KML File"),
		_TL("Expects that the input grid uses geographic coordinates."),
		false
	);

	Parameters.Add_Bool("",
		"NO_DATA"	, _TL("Set Transparency for No-Data"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	if( has_GUI() )
	{
		Parameters.Add_Choice("",
			"COLOURING"		, _TL("Colouring"),
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
				_TL("histogram stretch to standard deviation"),
				_TL("histogram stretch to percentage range"),
				_TL("histogram stretch to value range"),
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

	Parameters.Add_Bool("COL_PALETTE",
		"GRADUATED"		, _TL("Graduated Colors"),
		_TL(""),
		true
	);

	Parameters.Add_Double("COL_PALETTE",
		"STDDEV"		, _TL("Standard Deviation"),
		_TL(""),
		2., 0., true
	);

	Parameters.Add_Range("COL_PALETTE",
		"LINEAR"		, _TL("Percentage Range"),
		_TL(""),
		0., 100.
	);

	Parameters.Add_Range("COL_PALETTE",
		"STRETCH"		, _TL("Value Range"),
		_TL(""),
		0., 100.
	);

	Parameters.Add_Choice("COL_PALETTE",
		"SCALE_MODE"	, _TL("Scaling"),
		_TL("Scaling applied to colouring choices (i) grid's standard deviation, (ii) grid's value range, (iii) specified value range"),
		CSG_String::Format("%s|%s|%s",
			_TL("linear intervals"),
			_TL("increasing geometrical intervals"),
			_TL("decreasing geometrical intervals")
		), 0
	);

	Parameters.Add_Double("SCALE_MODE",
		"SCALE_LOG"		, _TL("Geometrical Interval Factor"),
		_TL(""),
		10.0, 0.001, true
	);

	//-----------------------------------------------------
	CSG_Table	*pLUT	= Parameters.Add_FixedTable("", "LUT", _TL("Lookup Table"), _TL(""))->asTable();

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
		"SHADE_TRANS"	, _TL("Transparency"),
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
		"SHADE_BRIGHT"	, _TL("Linear"),
		_TL("Minimum and maximum [%], the range for histogram stretch."),
		0., 100.
	);

	Parameters.Add_Double("SHADE_NODE",
		"SHADE_STDDEV"	, _TL("Standard Deviation"),
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
		pParameters->Set_Enabled("FILE_KML"       , pParameter->asPointer() && pParameter->asGrid()->Get_Projection().Get_Type() == SG_PROJ_TYPE_CS_Geographic);
	}

	if( pParameter->Cmp_Identifier("COLOURING") )
	{
		pParameters->Set_Enabled("COL_PALETTE"    , pParameter->asInt() <= 2);
		pParameters->Set_Enabled("STDDEV"         , pParameter->asInt() == 0);
		pParameters->Set_Enabled("LINEAR"         , pParameter->asInt() == 1);
		pParameters->Set_Enabled("STRETCH"        , pParameter->asInt() == 2);
		pParameters->Set_Enabled("SCALE_MODE"     , pParameter->asInt() <= 2);
		pParameters->Set_Enabled("LUT"            , pParameter->asInt() == 3);
		pParameters->Set_Enabled("NO_DATA"        , pParameter->asInt() != 5);
	}

	if( pParameter->Cmp_Identifier("SCALE_MODE") )
	{
		pParameters->Set_Enabled("SCALE_LOG"      , pParameter->asInt() > 0);
	}

	if( pParameter->Cmp_Identifier("SHADE") )
	{
		pParameters->Set_Enabled("SHADE_NODE"     , pParameter->asPointer() != NULL);
	}

	if( pParameter->Cmp_Identifier("SHADE_COLOURING") )
	{
		pParameters->Set_Enabled("SHADE_BRIGHT"   , pParameter->asInt() == 0);
		pParameters->Set_Enabled("SHADE_STDDEV"   , pParameter->asInt() == 1);
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
	CSG_Grid	Grid(Get_System(), SG_DATATYPE_Int);

	switch( Parameters("COLOURING")->asInt() )
	{
	default:	// stretch to grid's standard deviation
		if( !Set_Metric(Grid) )
		{
			return( false );
		}
		break;

	case  3:	// lookup table
		if( !Set_LUT(Grid) )
		{
			return( false );
		}
		break;

	case  4:	// rgb coded values
		if( !Set_RGB(Grid) )
		{
			return( false );
		}
		break;

	case  5:	// same as in graphical user interface
		if( !SG_UI_DataObject_asImage(Parameters("GRID")->asGrid(), &Grid) )
		{
			Error_Set("could not retrieve colour coding from graphical user interface.");

			return( false );
		}
		break;
	}

	Add_Shading(Grid);

	//-----------------------------------------------------
	wxImage	Image(Get_NX(), Get_NY());

	if( Parameters("NO_DATA")->asBool() && Grid.Get_NoData_Count() > 0 )
	{
		Image.SetAlpha();
	}

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !Grid.is_NoData(x, y) )
			{
				int	c	= Grid.asInt(x, y);

				Image.SetRGB(x, y, SG_GET_R(c), SG_GET_G(c), SG_GET_B(c));

				if( Image.HasAlpha() )
				{
					Image.SetAlpha(x, y, wxIMAGE_ALPHA_OPAQUE);
				}
			}
			else
			{
				Image.SetRGB(x, y, 255, 255, 255);

				if( Image.HasAlpha() )
				{
					Image.SetAlpha(x, y, wxIMAGE_ALPHA_TRANSPARENT);
				}
			}
		}
	}

	if( Parameters("NO_DATA")->asBool() && Grid.Get_NoData_Count() > 0 )
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

	if( !has_GUI() )
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

	bool	bOkay	= Image.SaveFile(fName.c_str());

	if( !has_GUI() && pImgHandler != NULL )
	{
		wxImage::RemoveHandler(pImgHandler->GetName());
	}

	if( !bOkay )
	{
		Error_Fmt("%s [%s]", _TL("failed to save image file"), fName.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if     ( SG_File_Cmp_Extension(fName, "bmp") ) Stream.Open(SG_File_Make_Path("", fName, "bpw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "jpg") ) Stream.Open(SG_File_Make_Path("", fName, "jgw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "pcx") ) Stream.Open(SG_File_Make_Path("", fName, "pxw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "png") ) Stream.Open(SG_File_Make_Path("", fName, "pgw"), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, "tif") ) Stream.Open(SG_File_Make_Path("", fName, "tfw"), SG_FILE_W, false);

	if( Stream.is_Open() )
	{
		Stream.Printf("%.10f\n%f\n%f\n%.10f\n%.10f\n%.10f\n",
			 Get_Cellsize(), 0.0, 0.0, -Get_Cellsize(), Get_XMin(), Get_YMax()
		);
	}

	//-----------------------------------------------------
	CSG_Projection	Projection	= Parameters("GRID")->asGrid()->Get_Projection();

	if( Projection.is_Okay() )
	{
		Projection.Save(SG_File_Make_Path("", fName, "prj"), SG_PROJ_FMT_WKT);
	}

	//-----------------------------------------------------
	if( Projection.Get_Type() == SG_PROJ_TYPE_CS_Geographic && Parameters("FILE_KML")->asBool() )
	{
		CSG_MetaData	KML; KML.Set_Name("kml"); KML.Add_Property("xmlns", "http://www.opengis.net/kml/2.2");

		CSG_MetaData	&Overlay	= *KML.Add_Child("GroundOverlay");

		Overlay.Add_Child("name"       , Parameters("GRID")->asGrid()->Get_Name());
		Overlay.Add_Child("description", Parameters("GRID")->asGrid()->Get_Description());
		Overlay.Add_Child("Icon"       )->Add_Child("href" , SG_File_Get_Name(fName, true));
		Overlay.Add_Child("LatLonBox"  );
		Overlay.Get_Child("LatLonBox"  )->Add_Child("north", Get_YMax());
		Overlay.Get_Child("LatLonBox"  )->Add_Child("south", Get_YMin());
		Overlay.Get_Child("LatLonBox"  )->Add_Child("east" , Get_XMax());
		Overlay.Get_Child("LatLonBox"  )->Add_Child("west" , Get_XMin());

		KML.Save(fName, SG_T("kml"));
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::Set_Metric(CSG_Grid &Grid)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	//-----------------------------------------------------
	CSG_Colors	Colors;

	if( has_GUI() )
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

	bool	bGraduated	= Parameters("GRADUATED")->asBool();

	//-----------------------------------------------------
	double	Minimum, Maximum, Scale	= Parameters("SCALE_LOG")->asDouble();

	int		Mode	= Parameters("SCALE_MODE")->asInt();

	switch( Parameters("COLOURING")->asInt() )
	{
	default:	// histogram stretch to standard deviation
		Minimum	= pGrid->Get_Mean() - Parameters("STDDEV")->asDouble() * pGrid->Get_StdDev(); if( Minimum < pGrid->Get_Min() ) Minimum = pGrid->Get_Min();
		Maximum	= pGrid->Get_Mean() + Parameters("STDDEV")->asDouble() * pGrid->Get_StdDev(); if( Maximum > pGrid->Get_Max() ) Maximum = pGrid->Get_Max();
		break;

	case  1:	// histogram stretch to percentage range
		Minimum	= pGrid->Get_Min() + pGrid->Get_Range() * Parameters("LINEAR.MIN")->asDouble() / 100.;
		Maximum	= pGrid->Get_Max() + pGrid->Get_Range() * Parameters("LINEAR.MAX")->asDouble() / 100.;
		break;

	case  2:	// histogram stretch to value range
		Minimum	= Parameters("STRETCH.MIN")->asDouble();
		Maximum	= Parameters("STRETCH.MAX")->asDouble();
		break;
	}

	if( Minimum >= Maximum || pGrid->Get_Range() <= 0.0 )
	{
		Error_Set(_TL("invalid user specified value range."));

		return( false );
	}

	//-------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		int	yy	= Get_NY() - y - 1;

		for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, yy) )
			{
				Grid.Set_NoData(x, y);
			}
			else
			{
				double	z	= (pGrid->asDouble(x, yy) - Minimum) / (Maximum - Minimum);

				switch( Mode )
				{
				case  1:	// logarithmic up
					z	= z <= 0. ? 0. : log(1. + Scale * z) / log(1. + Scale);
					break;

				case  2:	// logarithmic down
					z	= 1. - z;
					z	= z <= 0. ? 0. : log(1. + Scale * z) / log(1. + Scale);
					z	= 1. - z;
					break;
				}

				z	*= Colors.Get_Count();

				Grid.Set_Value(x, y, bGraduated ? Colors.Get_Interpolated(z) : Colors.Get_Color((int)z));
			}
		}
	}
	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_Export::Set_LUT(CSG_Grid &Grid)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	//-----------------------------------------------------
	CSG_Table	LUT;

	if( !Parameters("LUT")->asTable() || Parameters("LUT")->asTable()->Get_Field_Count() < 5 )
	{
		Error_Set(_TL("invalid lookup table."));

		return( false );
	}

	LUT.Create(*Parameters("LUT")->asTable());

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		int	yy	= Get_NY() - y - 1;

		for(int x=0; x<Get_NX(); x++)
		{
			double	z	= pGrid->asDouble(x, yy);

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
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_Export::Set_RGB(CSG_Grid &Grid)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int	yy	= Get_NY() - y - 1;

			if( pGrid->is_NoData(x, yy) )
			{
				Grid.Set_NoData(x, y);
			}
			else
			{
				Grid.Set_Value(x, y, pGrid->asDouble(x, yy));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::Add_Shading(CSG_Grid &Grid)
{
	CSG_Grid	*pShade	= Parameters("SHADE")->asGrid();

	if( !pShade || pShade->Get_Range() <= 0.0 || Parameters("SHADE_BRIGHT.MIN")->asDouble() >= Parameters("SHADE_BRIGHT.MAX")->asDouble() )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	Minimum, Maximum, Scale, Transparency	= Parameters("SHADE_TRANS")->asDouble() / 100.0;

	switch( Parameters("SHADE_COLOURING")->asInt() )
	{
	default:
		Minimum	= pShade->Get_Min() + pShade->Get_Range() * Parameters("SHADE_BRIGHT.MIN")->asDouble() / 100.;
		Maximum	= pShade->Get_Min() + pShade->Get_Range() * Parameters("SHADE_BRIGHT.MAX")->asDouble() / 100.;
		break;

	case  1:
		Minimum	= pShade->Get_Mean() - pShade->Get_StdDev() * Parameters("SHADE_STDDEV")->asDouble(); if( Minimum < pShade->Get_Min() ) Minimum = pShade->Get_Min();
		Maximum	= pShade->Get_Mean() + pShade->Get_StdDev() * Parameters("SHADE_STDDEV")->asDouble(); if( Maximum > pShade->Get_Max() ) Maximum = pShade->Get_Max();
		break;
	}

	if( Minimum >= Maximum )
	{
		return( false );
	}

	Scale	= 255 * (1 - Transparency) / (Maximum - Minimum);

	//-----------------------------------------------------
	#pragma omp parallel
	for(int y=0; y<Get_NY(); y++)
	{
		int	yy	= Get_NY() - y - 1;

		for(int x=0; x<Get_NX(); x++)
		{
			if( !Grid.is_NoData(x, y) && !pShade->is_NoData(x, yy) )
			{
				double	d	= Scale * (Maximum - pShade->asDouble(x, yy));

				int	c	= Grid.asInt(x, y);

				int	r	= (int)(Transparency * SG_GET_R(c) + d); if( r < 0 ) r = 0; else if( r > 255 ) r = 255;
				int	g	= (int)(Transparency * SG_GET_G(c) + d); if( g < 0 ) g = 0; else if( g > 255 ) g = 255;
				int	b	= (int)(Transparency * SG_GET_B(c) + d); if( b < 0 ) b = 0; else if( b > 255 ) b = 255;

				Grid.Set_Value(x, y, SG_GET_RGB(r, g, b));
			}
		}
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
