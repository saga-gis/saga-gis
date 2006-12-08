
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

	Set_Description	(_TW(
		"Saves a grid as image using display properties as used by the graphical user interface.\n")
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
	CSG_Grid	*pGrid, *pShade, Grid, Shade;
	FILE		*Stream;
	wxFileName	fName;
	wxImage		img;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	pShade	= Parameters("SHADE")	->asGrid();
	fName	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	if( SG_UI_DataObject_asImage(pGrid, &Grid) )
	{
		if( !SG_UI_DataObject_asImage(pShade, &Shade) )
		{
			pShade	= NULL;
		}

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
		if( img.SaveFile(fName.GetFullPath()) )
		{
			if(      !fName.GetExt().CmpNoCase(SG_T("bmp")) )
			{
				fName.SetExt(SG_T("bpw"));
			}
			else if( !fName.GetExt().CmpNoCase(SG_T("jpg")) )
			{
				fName.SetExt(SG_T("jgw"));
			}
			else if( !fName.GetExt().CmpNoCase(SG_T("png")) )
			{
				fName.SetExt(SG_T("pgw"));
			}
			else if( !fName.GetExt().CmpNoCase(SG_T("tif")) )
			{
				fName.SetExt(SG_T("tfw"));
			}
			else
			{
				fName.SetExt(SG_T("world"));
			}

			if( (Stream = fopen(fName.GetFullPath().mb_str(), "w")) != NULL )
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
