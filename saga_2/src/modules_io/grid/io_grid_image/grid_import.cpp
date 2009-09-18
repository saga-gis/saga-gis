
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
//                    Grid_Import.cpp                    //
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

#include "grid_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Import::CGrid_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Image (bmp, jpg, png, tif, gif, pnm, xpm)"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description	(_TW(
		"Loads an image."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "OUT_GRID"	, _TL("Image"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUT_RED"		, _TL("Image (Red Channel)"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUT_GREEN"	, _TL("Image (Green Channel)"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "OUT_BLUE"	, _TL("Image (Blue Channel)"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Image File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s"),
			_TL("All Recognized File Types")					, SG_T("*.bmp;*.ico;*.gif;*.jpg;*.jif;*.jpeg;*.pcx;*.png;*.pnm;*.tif;*.tiff;*.xpm"),
			_TL("CompuServe Graphics Interchange (*.gif)")		, SG_T("*.gif"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)")	, SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Portable Network Graphics (*.png)")			, SG_T("*.png"),
			_TL("Tagged Image File Format (*.tif, *.tiff)")		, SG_T("*.tif;*.tiff"),
			_TL("Windows or OS/2 Bitmap (*.bmp)")				, SG_T("*.bmp"),
			_TL("Zsoft Paintbrush (*.pcx)")						, SG_T("*.pcx"),
			_TL("All Files")									, SG_T("*.*")
		)
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Options"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Standard"),
			_TL("Split Channels"),
			_TL("Enforce True Color")
		)
	);
}

//---------------------------------------------------------
CGrid_Import::~CGrid_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_GRID(g, s, t)	g	= SG_Create_Grid(t, img.GetWidth(), img.GetHeight(), Cellsize, xMin, yMin); g->Set_Name(s);

//---------------------------------------------------------
bool CGrid_Import::On_Execute(void)
{
	int					x, y, yy, Method;
	double				d, Cellsize, xMin, yMin;
	CSG_Colors			Colors;
	CSG_Grid			*pR, *pG, *pB;
	FILE				*Stream;
	wxFileName			fName;
	wxImage				img;
	wxImageHistogram	hst;

	//-----------------------------------------------------
	fName	= Parameters("FILE")	->asString();
	Method	= Parameters("METHOD")	->asInt();

	if( img.LoadFile(fName.GetFullPath()) )
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

		//-------------------------------------------------
		if( (Stream = fopen(fName.GetFullPath().mb_str(), "r")) != NULL )
		{
			fscanf(Stream, "%lf %lf %lf %lf %lf %lf ", &Cellsize, &d, &d, &d, &xMin, &yMin);
			yMin		= yMin - (img.GetHeight() - 1) * Cellsize;
			fclose(Stream);
		}
		else
		{
			Cellsize	= 1.0;
			xMin		= 0.0;
			yMin		= 0.0;
		}

		//-------------------------------------------------
		if( Method == 1 )
		{
			ADD_GRID(pR, CSG_String::Format(SG_T("%s [R]"), fName.GetName().c_str()), SG_DATATYPE_Byte);
			ADD_GRID(pG, CSG_String::Format(SG_T("%s [G]"), fName.GetName().c_str()), SG_DATATYPE_Byte);
			ADD_GRID(pB, CSG_String::Format(SG_T("%s [B]"), fName.GetName().c_str()), SG_DATATYPE_Byte);

			for(y=0, yy=pR->Get_NY()-1; y<pR->Get_NY() && Set_Progress(y, pR->Get_NY()); y++, yy--)
			{
				for(x=0; x<pR->Get_NX(); x++)
				{
					pR->Set_Value(x, y, img.GetRed  (x, yy));
					pG->Set_Value(x, y, img.GetGreen(x, yy));
					pB->Set_Value(x, y, img.GetBlue (x, yy));
				}
			}

			Parameters("OUT_RED")	->Set_Value(pR);
			DataObject_Set_Colors(pR, 100, SG_COLORS_BLACK_RED);

			Parameters("OUT_GREEN")	->Set_Value(pG);
			DataObject_Set_Colors(pG, 100, SG_COLORS_BLACK_GREEN);

			Parameters("OUT_BLUE")	->Set_Value(pB);
			DataObject_Set_Colors(pB, 100, SG_COLORS_BLACK_BLUE);
		}

		//-------------------------------------------------
		else if( Method == 0 && (yy = img.ComputeHistogram(hst)) <= 256 )
		{
			Colors.Set_Count(yy);

			for(wxImageHistogram::iterator i=hst.begin(); i!=hst.end(); ++i)
			{
				Colors.Set_Color(i->second.index, SG_GET_R(i->first), SG_GET_G(i->first), SG_GET_B(i->first));
			}

			ADD_GRID(pR, CSG_String( fName.GetName() ), yy <= 2 ? SG_DATATYPE_Bit : SG_DATATYPE_Byte);

			for(y=0, yy=pR->Get_NY()-1; y<pR->Get_NY() && Set_Progress(y, pR->Get_NY()); y++, yy--)
			{
				for(x=0; x<pR->Get_NX(); x++)
				{
					pR->Set_Value(x, y, hst[SG_GET_RGB(img.GetRed(x, yy), img.GetGreen(x, yy), img.GetBlue(x, yy))].index);
				}
			}

			Parameters("OUT_GRID")	->Set_Value(pR);
			DataObject_Set_Colors(pR, Colors);
			DataObject_Update(pR, 0, Colors.Get_Count() - 1);
		}

		//-------------------------------------------------
		else
		{
			hst.clear();

			ADD_GRID(pR, CSG_String( fName.GetName() ).c_str(), SG_DATATYPE_Int);

			for(y=0, yy=pR->Get_NY()-1; y<pR->Get_NY() && Set_Progress(y, pR->Get_NY()); y++, yy--)
			{
				for(x=0; x<pR->Get_NX(); x++)
				{
					pR->Set_Value(x, y, SG_GET_RGB(img.GetRed(x, yy), img.GetGreen(x, yy), img.GetBlue(x, yy)));
				}
			}

			Parameters("OUT_GRID")	->Set_Value(pR);
			DataObject_Set_Colors(pR, 100, SG_COLORS_BLACK_WHITE);

			CSG_Parameters	Parms;

			if( DataObject_Get_Parameters(pR, Parms) && Parms("COLORS_TYPE") )
			{
				Parms("COLORS_TYPE")->Set_Value(3);	// Color Classification Type: RGB

				DataObject_Set_Parameters(pR, Parms);
			}
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
