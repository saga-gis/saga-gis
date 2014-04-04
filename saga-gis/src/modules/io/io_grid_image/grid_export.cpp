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
//    contact:    SAGA User Group Association            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW(
		"The module allows to save a grid as image.\n\n"
		"On the command line, in case a shade grid is specified, "
		"it's minimum and maximum brightness values can be specified in "
		"percent.\n")
	);

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
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s"),
			_TL("Windows or OS/2 Bitmap (*.bmp)")				, SG_T("*.bmp"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)")	, SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Zsoft Paintbrush (*.pcx)")						, SG_T("*.pcx"),
			_TL("Portable Network Graphics (*.png)")			, SG_T("*.png"),
			_TL("Tagged Image File Format (*.tif, *.tiff)")		, SG_T("*.tif;*.tiff")
		), NULL, true
	);

	Parameters.Add_Value(
		NULL	, "FILE_KML"	, _TL("Create KML File"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Choice(
			NULL	, "COLOURING"	, _TL("Colouring"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
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
			CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
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

	if( !SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Range(
			NULL	, "SHADE_BRIGHT", _TL("Shade Brightness"),
			_TL("Allows to scale shade brightness [percent]"),
			0.0, 100.0, 0.0, true, 100.0, true
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Export::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("COLOURING")) )
	{
		pParameters->Get_Parameter("COL_PALETTE")->Set_Enabled(pParameter->asInt() <= 2);
		pParameters->Get_Parameter("STDDEV"     )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("STRETCH"    )->Set_Enabled(pParameter->asInt() == 2);
		pParameters->Get_Parameter("LUT"        )->Set_Enabled(pParameter->asInt() == 3);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::On_Execute(void)
{
	//-----------------------------------------------------
	int			y, iy, Method;
	CSG_Grid	*pGrid, *pShade, Grid, Shade;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID"     )->asGrid();
	pShade	= Parameters("SHADE"    )->asGrid();
	Method	= Parameters("COLOURING")->asInt ();

	if( !pGrid )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Method == 5 )	// same as in graphical user interface
	{
		if( !SG_UI_DataObject_asImage(pGrid, &Grid) )
		{
			Error_Set("could not retrieve colour coding from graphical user interface.");

			return( false );
		}
	}
	else
	{
		double		zMin, zScale;
		CSG_Colors	Colors;
		CSG_Table	*pLUT	= Parameters("LUT")->asTable();

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

		switch( Method )
		{
		case 0:	// stretch to grid's standard deviation
			zMin	= pGrid->Get_ArithMean() -  Parameters("STDDEV")->asDouble() * pGrid->Get_StdDev();
			zScale	= Colors.Get_Count() / (2 * Parameters("STDDEV")->asDouble() * pGrid->Get_StdDev());
			break;

		case 1:	// stretch to grid's value range
			zMin	= pGrid->Get_ZMin();
			zScale	= Colors.Get_Count() / pGrid->Get_ZRange();
			break;

		case 2:	// stretch to specified value range
			zMin	= Parameters("STRETCH")->asRange()->Get_LoVal();
			if( zMin >= (zScale = Parameters("STRETCH")->asRange()->Get_HiVal()) )
			{
				Error_Set(_TL("invalid user specified value range."));

				return( false );
			}
			zScale	= Colors.Get_Count() / (zScale - zMin);
			break;

		case 3:	// lookup table
			if( !pLUT || pLUT->Get_Field_Count() < 5 )
			{
				Error_Set(_TL("invalid lookup table."));

				return( false );
			}
			break;

		case 4:	// rgb coded values
			break;
		}

		//-------------------------------------------------
		Grid.Create(*Get_System(), SG_DATATYPE_Int);

		for(y=0, iy=Get_NY()-1; y<Get_NY() && Set_Progress(y); y++, iy--)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				double	z	= pGrid->asDouble(x, y);

				if( Method == 3 )	// lookup table
				{
					int		i, iColor;

					for(i=0, iColor=-1; i<pLUT->Get_Record_Count() && iColor<0; i++)
					{
						if( z >= pLUT->Get_Record(i)->asDouble(3) && z < pLUT->Get_Record(i)->asDouble(4) )
						{
							Grid.Set_Value(x, iy, pLUT->Get_Record(iColor = i)->asInt(0));
						}
					}

					if( iColor < 0 )
					{
						Grid.Set_NoData(x, iy);
					}
				}
				else if( pGrid->is_NoData(x, y) )
				{
					Grid.Set_NoData(x, iy);
				}
				else if( Method == 4 )	// rgb coded values
				{
					Grid.Set_Value (x, iy, z);
				}
				else
				{
					int	i	= (int)(zScale * (z - zMin));

					Grid.Set_Value (x, iy, Colors[i < 0 ? 0 : i >= Colors.Get_Count() ? Colors.Get_Count() - 1 : i]);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !pShade || pShade->Get_ZRange() <= 0.0 )
	{
		pShade	= NULL;
	}
	else if( !SG_UI_DataObject_asImage(pShade, &Shade) )
	{
		double	dMinBright, dMaxBright;

		dMinBright	= Parameters("SHADE_BRIGHT")->asRange()->Get_LoVal() / 100.0;
		dMaxBright	= Parameters("SHADE_BRIGHT")->asRange()->Get_HiVal() / 100.0;

		if( dMinBright >= dMaxBright )
		{
			SG_UI_Msg_Add_Error(_TL("Minimum shade brightness must be lower than maximum shade brightness!"));

			return( false );
		}

		int			nColors	= 100;
		CSG_Colors	Colors(nColors, SG_COLORS_BLACK_WHITE, true);

	    //-------------------------------------------------
		Shade.Create(*Get_System(), SG_DATATYPE_Int);

		for(y=0, iy=Get_NY()-1; y<Get_NY() && Set_Progress(y); y++, iy--)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( pShade->is_NoData(x, y) )
				{
					Shade.Set_NoData(x, iy);
				}
				else
				{
					Shade.Set_Value (x, iy, Colors[(int)(nColors * (dMaxBright - dMinBright) * (pShade->asDouble(x, y) - pShade->Get_ZMin()) / pShade->Get_ZRange() + dMinBright)]);
				}
			}
		}
	}

	//-----------------------------------------------------
	wxImage	Image(Get_NX(), Get_NY());

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( Grid.is_NoData(x, y) )
			{
				Image.SetRGB(x, y, 255, 255, 255);
			}
			else
			{
				int	r, g, b, c	= Grid.asInt(x, y);

				r	= SG_GET_R(c);
				g	= SG_GET_G(c);
				b	= SG_GET_B(c);

				if( pShade )
				{
					c	= Shade.asInt(x, y);

					double d	= (SG_GET_R(c) + SG_GET_G(c) + SG_GET_B(c)) / (3.0 * 255.0);

					r	= (int)(d * r);
					g	= (int)(d * g);
					b	= (int)(d * b);
				}

				Image.SetRGB(x, y, r, g, b);
			}
		}
	}

	//-------------------------------------------------
	CSG_String	fName(Parameters("FILE")->asString());

	if( !SG_File_Cmp_Extension(fName, SG_T("bmp"))
	&&  !SG_File_Cmp_Extension(fName, SG_T("jpg"))
	&&  !SG_File_Cmp_Extension(fName, SG_T("pcx"))
	&&  !SG_File_Cmp_Extension(fName, SG_T("png"))
	&&  !SG_File_Cmp_Extension(fName, SG_T("tif")) )
	{
		fName	= SG_File_Make_Path(NULL, fName, SG_T("png"));
	}

	//-----------------------------------------------------
	wxImageHandler	*pImgHandler = NULL;

	if( !SG_UI_Get_Window_Main() )
	{
		if( SG_File_Cmp_Extension(fName, SG_T("jpg")) )
			pImgHandler = new wxJPEGHandler;
		else if( SG_File_Cmp_Extension(fName, SG_T("pcx")) )
			pImgHandler = new wxPCXHandler;
		else if( SG_File_Cmp_Extension(fName, SG_T("tif")) )
			pImgHandler = new wxTIFFHandler;
#ifdef _SAGA_MSW
		else if( SG_File_Cmp_Extension(fName, SG_T("bmp")) )
			pImgHandler = new wxBMPHandler;
#endif
		else // if( SG_File_Cmp_Extension(fName, SG_T("png")) )
			pImgHandler = new wxPNGHandler;

		wxImage::AddHandler(pImgHandler);
	}

	if( !Image.SaveFile(fName.c_str()) )
	{
		Error_Set(CSG_String::Format(SG_T("%s [%s]"), _TL("could not save image file"), fName.c_str()));

		return( false );
	}

	pGrid->Get_Projection().Save(SG_File_Make_Path(NULL, fName, SG_T("prj")), SG_PROJ_FMT_WKT);

	//-----------------------------------------------------
	CSG_File	Stream;

	if(      SG_File_Cmp_Extension(fName, SG_T("bmp")) ) Stream.Open(SG_File_Make_Path(NULL, fName, SG_T("bpw")), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, SG_T("jpg")) ) Stream.Open(SG_File_Make_Path(NULL, fName, SG_T("jgw")), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, SG_T("pcx")) ) Stream.Open(SG_File_Make_Path(NULL, fName, SG_T("pxw")), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, SG_T("png")) ) Stream.Open(SG_File_Make_Path(NULL, fName, SG_T("pgw")), SG_FILE_W, false);
	else if( SG_File_Cmp_Extension(fName, SG_T("tif")) ) Stream.Open(SG_File_Make_Path(NULL, fName, SG_T("tfw")), SG_FILE_W, false);

	if( Stream.is_Open() )
	{
		Stream.Printf(SG_T("%.10f\n%f\n%f\n%.10f\n%.10f\n%.10f\n"),
			 pGrid->Get_Cellsize(),
			 0.0, 0.0,
			-pGrid->Get_Cellsize(),
			 pGrid->Get_XMin(),
			 pGrid->Get_YMax()
		);
	}

	//-----------------------------------------------------
	if( Parameters("FILE_KML")->asBool() && Stream.Open(SG_File_Make_Path(NULL, fName, SG_T("kml")), SG_FILE_W, false) )
	{
		Stream.Printf(SG_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
		Stream.Printf(SG_T("<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"));
		Stream.Printf(SG_T("  <Folder>\n"));
		Stream.Printf(SG_T("    <name>Raster exported from SAGA</name>\n"));
		Stream.Printf(SG_T("    <description>System for Automated Geoscientific Analyses - www.saga-gis.org</description>\n"));
		Stream.Printf(SG_T("    <GroundOverlay>\n"));
		Stream.Printf(SG_T("      <name>%s</name>\n")				, pGrid->Get_Name());
		Stream.Printf(SG_T("      <description>%s</description>\n")	, pGrid->Get_Description());
		Stream.Printf(SG_T("      <Icon>\n"));
		Stream.Printf(SG_T("        <href>%s</href>\n")				, SG_File_Get_Name(fName, true).c_str());
		Stream.Printf(SG_T("      </Icon>\n"));
		Stream.Printf(SG_T("      <LatLonBox>\n"));
		Stream.Printf(SG_T("        <north>%.10f</north>\n")			, pGrid->Get_YMax());
		Stream.Printf(SG_T("        <south>%.10f</south>\n")			, pGrid->Get_YMin());
		Stream.Printf(SG_T("        <east>%.10f</east>\n")				, pGrid->Get_XMax());
		Stream.Printf(SG_T("        <west>%.10f</west>\n")				, pGrid->Get_XMin());
		Stream.Printf(SG_T("        <rotation>0.0</rotation>\n"));
		Stream.Printf(SG_T("      </LatLonBox>\n"));
		Stream.Printf(SG_T("    </GroundOverlay>\n"));
		Stream.Printf(SG_T("  </Folder>\n"));
		Stream.Printf(SG_T("</kml>\n"));
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
