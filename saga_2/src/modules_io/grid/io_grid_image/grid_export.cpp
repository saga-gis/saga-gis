
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

	Set_Author		(SG_T("O. Conrad (c) 2005"));

	Set_Description	(_TW(
		"Saves a grid as image using display properties as used by the graphical user interface.\n\n"
		"On the command line there are further parameters available to either use one of the default "
		"palettes or to use a Lookup Table for coloring.\n")
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
		NULL, "FILE_KML"		, _TL("Create KML File"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	if( !SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Value(
			NULL	, "COL_COUNT"	, _TL("Number of Colors"),
			_TL(""),
			PARAMETER_TYPE_Int, 100
		);

		Parameters.Add_Choice(
			NULL	, "COL_PALETTE"	, _TL("Color Palette"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
				_TL("DEFAULT"),
				_TL("DEFAULT_BRIGHT"),
				_TL("BLACK_WHITE"),
				_TL("BLACK_RED"),
				_TL("BLACK_GREEN"),
				_TL("BLACK_BLUE"),
				_TL("WHITE_RED"),
				_TL("WHITE_GREEN"),
				_TL("WHITE_BLUE"),
				_TL("YELLOW_RED"),
				_TL("YELLOW_GREEN"),
				_TL("YELLOW_BLUE"),
				_TL("RED_GREEN"),
				_TL("RED_BLUE"),
				_TL("GREEN_BLUE"),
				_TL("RED_GREY_BLUE"),
				_TL("RED_GREY_GREEN"),
				_TL("GREEN_GREY_BLUE"),
				_TL("RED_GREEN_BLUE"),
				_TL("RED_BLUE_GREEN"),
				_TL("GREEN_RED_BLUE"),
				_TL("RAINBOW"),
				_TL("NEON")
			), 0
		);

		Parameters.Add_Value(
			NULL, "COL_REVERT"	, _TL("Revert Palette"),
			_TL(""),
			PARAMETER_TYPE_Bool, false
		);

		Parameters.Add_Table(
			NULL, "LUT"	, _TL("Lookup Table"),
			_TL(""),
			PARAMETER_INPUT_OPTIONAL
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::On_Execute(void)
{
	wxInitAllImageHandlers();

	int			x, y, c, r, g, b;
	double		d;
	CSG_Grid	*pGrid, *pShade, Grid, Shade;
	CSG_File	Stream;
	CSG_String	fName, fExt;
	wxImage		img;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pShade	= Parameters("SHADE")	->asGrid();
	fName	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	if(      SG_File_Cmp_Extension(fName, SG_T("bmp")) )
	{
		fExt	= SG_T("bpw");
	}
	else if( SG_File_Cmp_Extension(fName, SG_T("jpg")) )
	{
		fExt	= SG_T("jgw");
	}
	else if( SG_File_Cmp_Extension(fName, SG_T("pcx")) )
	{
		fExt	= SG_T("pxw");
	}
	else if( SG_File_Cmp_Extension(fName, SG_T("png")) )
	{
		fExt	= SG_T("pgw");
	}
	else if( SG_File_Cmp_Extension(fName, SG_T("tif")) )
	{
		fExt	= SG_T("tfw");
	}
	else
	{
		fExt	= SG_T("pgw");

		fName	= SG_File_Make_Path(NULL, fName, SG_T("png"));
	}

	//-----------------------------------------------------
	if( !pGrid || pGrid->Get_ZRange() == 0.0 )
	{
		return( false );
	}
	else if( !SG_UI_DataObject_asImage(pGrid, &Grid) )
	{
		int			nColors	= Parameters("COL_COUNT")->asInt();
		CSG_Colors	Colors(nColors, Parameters("COL_PALETTE")->asInt(), Parameters("COL_REVERT")->asBool());
		CSG_Table	*pLUT	= Parameters("LUT")->asTable();

		if( pLUT && pLUT->Get_Field_Count() < 5 )
		{
			SG_UI_Msg_Add_Error(_TL("Improperly formatted Lookup Table."));
			return( false );
		}


		Grid.Create(*Get_System(), SG_DATATYPE_Int);

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( pLUT )
				{
					bool	bFound = false;

					d = pGrid->asDouble(x, y);
					
					for(int i=0; i<pLUT->Get_Record_Count(); i++)
					{
						if( d >= pLUT->Get_Record(i)->asDouble(3) && d < pLUT->Get_Record(i)->asDouble(4) )
						{
							nColors	= i;
							bFound	= true;
							break;
						}
					}
					
					if( !bFound )
						Grid.Set_NoData(x, Get_NY() - 1 - y);
					else
						Grid.Set_Value(x, Get_NY() - 1 - y, pLUT->Get_Record(nColors)->asInt(0));
				}
				else
				{
					if( pGrid->is_NoData(x, y) )
						Grid.Set_NoData(x, Get_NY() - 1 - y);
					else
						Grid.Set_Value(x, Get_NY() - 1 - y, Colors[(int)(
							nColors * (pGrid->asDouble(x, y) - pGrid->Get_ZMin()) / pGrid->Get_ZRange()
						)]);
				}
			}
		}
	}

	if( !pShade || pShade->Get_ZRange() == 0.0 )
	{
		pShade	= NULL;
	}
	else if( !SG_UI_DataObject_asImage(pShade, &Shade) )
	{
		int			nColors	= 100;
		CSG_Colors	Colors(nColors, SG_COLORS_BLACK_WHITE);

		Shade.Create(*Get_System(), SG_DATATYPE_Int);

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( pShade->is_NoData(x, y) )
					Shade.Set_NoData(x, Get_NY() - 1 - y);
				else
					Shade.Set_Value(x, Get_NY() - 1 - y, Colors[(int)(
						nColors * (pShade->asDouble(x, y) - pShade->Get_ZMin()) / pShade->Get_ZRange()
					)]);
			}
		}
	}

	//-------------------------------------------------
	img.Create(Get_NX(), Get_NY());

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			c	= Grid.asInt(x, y);

			r	= SG_GET_R(c);
			g	= SG_GET_G(c);
			b	= SG_GET_B(c);

			if( pShade )
			{
				c	= Shade.asInt(x, y);

				d	= (SG_GET_R(c) + SG_GET_G(c) + SG_GET_B(c)) / (3.0 * 255.0);

				r	= (int)(d * r);
				g	= (int)(d * g);
				b	= (int)(d * b);
			}

			img.SetRGB(x, y, r, g, b);
		}
	}

	//-------------------------------------------------
	if( img.SaveFile(fName.c_str()) )
	{
		pGrid->Get_Projection().Save(SG_File_Make_Path(NULL, fName, SG_T("prj")), SG_PROJ_FMT_WKT);

		if( Stream.Open(SG_File_Make_Path(NULL, fName, fExt), SG_FILE_W, false) )
		{
			Stream.Printf(SG_T("%f\n%f\n%f\n%f\n%f\n%f\n"),
				 pGrid->Get_Cellsize(),
				 0.0, 0.0,
				-pGrid->Get_Cellsize(),
				 pGrid->Get_XMin(),
				 pGrid->Get_YMax()
			);
		}

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
			Stream.Printf(SG_T("        <north>%f</north>\n")			, pGrid->Get_YMax());
			Stream.Printf(SG_T("        <south>%f</south>\n")			, pGrid->Get_YMin());
			Stream.Printf(SG_T("        <east>%f</east>\n")				, pGrid->Get_XMax());
			Stream.Printf(SG_T("        <west>%f</west>\n")				, pGrid->Get_XMin());
			Stream.Printf(SG_T("        <rotation>0.0</rotation>\n"));
			Stream.Printf(SG_T("      </LatLonBox>\n"));
			Stream.Printf(SG_T("    </GroundOverlay>\n"));
			Stream.Printf(SG_T("  </Folder>\n"));
			Stream.Printf(SG_T("</kml>\n"));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
