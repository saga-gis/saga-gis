
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
	Set_Name	(_TL("Export Image (bmp, jpg, png)"));

	Set_Author	(_TL("Copyrights (c) 2005 by Olaf Conrad"));

	Set_Description(_TL(
		"Saves a grid as image using display properties as used by the graphical user interface.\n")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SHADE"		, _TL("Shade"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Image File"),
		"",
		_TL(
		"Windows or OS/2 Bitmap (*.bmp)"				"|*.bmp|"
		"JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)"	"|*.jpg;*.jif;*.jpeg|"
		"Zsoft Paintbrush (*.pcx)"						"|*.pcx|"
		"Portable Network Graphics (*.png)"				"|*.png|"
		"Tagged Image File Format (*.tif, *.tiff)"		"|*.tif;*.tiff"),

		NULL, true
	);
}

//---------------------------------------------------------
CGrid_Export::~CGrid_Export(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Export::On_Execute(void)
{
	int			x, y, c, r, g, b;
	double		d;
	CGrid		*pGrid, *pShade, Grid, Shade;
	FILE		*Stream;
	wxFileName	fName;
	wxImage		img;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pShade	= Parameters("SHADE")	->asGrid();
	fName	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	if( SG_Callback_DataObject_asImage(pGrid, &Grid) )
	{
		if( !SG_Callback_DataObject_asImage(pShade, &Shade) )
		{
			pShade	= NULL;
		}

		img.Create(Get_NX(), Get_NY());

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				c	= Grid.asInt(x, y);

				r	= COLOR_GET_R(c);
				g	= COLOR_GET_G(c);
				b	= COLOR_GET_B(c);

				if( pShade )
				{
					c	= Shade.asInt(x, y);

					d	= (COLOR_GET_R(c) + COLOR_GET_G(c) + COLOR_GET_B(c)) / (3.0 * 255.0);

					r	= (int)(d * r);
					g	= (int)(d * g);
					b	= (int)(d * b);
				}

				img.SetRGB(x, y, r, g, b);
			}
		}

		//-------------------------------------------------
		if( img.SaveFile(fName.GetFullPath()) )
		{
			if(      !fName.GetExt().CmpNoCase("bmp") )
			{
				fName.SetExt("bpw");
			}
			else if( !fName.GetExt().CmpNoCase("jpg") )
			{
				fName.SetExt("jgw");
			}
			else if( !fName.GetExt().CmpNoCase("png") )
			{
				fName.SetExt("pgw");
			}
			else if( !fName.GetExt().CmpNoCase("tif") )
			{
				fName.SetExt("tfw");
			}
			else
			{
				fName.SetExt("world");
			}

			if( (Stream = fopen(fName.GetFullPath(), "w")) != NULL )
			{
				fprintf(Stream, "%f\n%f\n%f\n%f\n%f\n%f\n",
					 pGrid->Get_Cellsize(),
					 0.0, 0.0,
					-pGrid->Get_Cellsize(),
					 pGrid->Get_XMin(),
					 pGrid->Get_YMax()
				);

				fclose(Stream);
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
