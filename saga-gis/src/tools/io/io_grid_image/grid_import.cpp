
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
//    contact:    SAGA User Group Association            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
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
	Set_Name		(_TL("Import Image (bmp, jpg, png, tif, gif, pnm, xpm)"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Loads an image."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"OUT_GRID"	, _TL("Image"),
		_TL("")
	);

	Parameters.Add_Grid_Output("",
		"OUT_RED"	, _TL("Image (Red Channel)"),
		_TL("")
	);

	Parameters.Add_Grid_Output("",
		"OUT_GREEN"	, _TL("Image (Green Channel)"),
		_TL("")
	);

	Parameters.Add_Grid_Output("",
		"OUT_BLUE"	, _TL("Image (Blue Channel)"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILE"		, _TL("Image File"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("All Recognized File Types"                   ), SG_T("*.bmp;*.ico;*.gif;*.jpg;*.jif;*.jpeg;*.pcx;*.png;*.pnm;*.tif;*.tiff;*.xpm"),
			_TL("CompuServe Graphics Interchange (*.gif)"     ), SG_T("*.gif"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)"), SG_T("*.jpg;*.jif;*.jpeg"),
			_TL("Portable Network Graphics (*.png)"           ), SG_T("*.png"),
			_TL("Tagged Image File Format (*.tif, *.tiff)"    ), SG_T("*.tif;*.tiff"),
			_TL("Windows or OS/2 Bitmap (*.bmp)"              ), SG_T("*.bmp"),
			_TL("Zsoft Paintbrush (*.pcx)"                    ), SG_T("*.pcx"),
			_TL("All Files"                                   ), SG_T("*.*")
		)
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Options"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Standard"),
			_TL("Split Channels"),
			_TL("Enforce True Color")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Import::On_Execute(void)
{
	CSG_String	File	= Parameters("FILE")->asString();

	if( !SG_File_Exists(File) )
	{
		Error_Set(_TL("File not found!"));

		return( false );
	}

	//-----------------------------------------------------
	if( wxImage::GetHandlers().GetCount() <= 1 ) // wxBMPHandler is always installed by default
	{
		wxInitAllImageHandlers();
	}

	wxImage	Image;

	if( !Image.LoadFile(File.c_str()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if     ( SG_File_Cmp_Extension(File, "bmp") ) { Stream.Open(SG_File_Make_Path("", File,   "bpw"), SG_FILE_R, false); }
	else if( SG_File_Cmp_Extension(File, "jpg") ) { Stream.Open(SG_File_Make_Path("", File,   "jgw"), SG_FILE_R, false); }
	else if( SG_File_Cmp_Extension(File, "png") ) { Stream.Open(SG_File_Make_Path("", File,   "pgw"), SG_FILE_R, false); }
	else if( SG_File_Cmp_Extension(File, "tif") ) { Stream.Open(SG_File_Make_Path("", File,   "tfw"), SG_FILE_R, false); }
	else                                          { Stream.Open(SG_File_Make_Path("", File, "world"), SG_FILE_R, false); }

	bool bTransform = false; double xMin = 0., yMin = 0., Cellsize = 1., m[6];

	if(	Stream.is_Open() && Stream.Scan(m[0]) && Stream.Scan(m[1]) && Stream.Scan(m[2]) && Stream.Scan(m[3]) && Stream.Scan(m[4]) && Stream.Scan(m[5]) )
	{
		if( m[0] != -m[3] || m[2] != 0. || m[1] != 0. )
		{
			bTransform	= true;
		}
		else
		{
			xMin     = m[4];
			yMin     = m[5] + m[3] * (Image.GetHeight() - 1);
			Cellsize = m[0];
		}
	}

	//-----------------------------------------------------
	#define SET_METADATA(pGrid, Suffix, Output) {\
		pGrid->Set_Name(SG_File_Get_Name(File, false) + Suffix);\
		pGrid->Get_Projection().Load(SG_File_Make_Path("", File, "prj"), SG_PROJ_FMT_WKT);\
		Parameters(Output)->Set_Value(pGrid);\
		DataObject_Set_Colors(pGrid, 11, SG_COLORS_BLACK_WHITE);\
	}

	wxImageHistogram	Histogram;

	//-----------------------------------------------------
	// look-up color table...

	if( Parameters("METHOD")->asInt() == 0 && Image.ComputeHistogram(Histogram) <= 256 )
	{
		CSG_Colors	Colors((int)Histogram.size());

		for(wxImageHistogram::iterator i=Histogram.begin(); i!=Histogram.end(); ++i)
		{
			Colors.Set_Color(i->second.index, SG_GET_R(i->first), SG_GET_G(i->first), SG_GET_B(i->first));
		}

		CSG_Grid *pRGB = SG_Create_Grid(Histogram.size() <= 2 ? SG_DATATYPE_Bit : SG_DATATYPE_Byte, Image.GetWidth(), Image.GetHeight(), Cellsize, xMin, yMin);

		for(int y=0; y<pRGB->Get_NY() && Set_Progress(y, pRGB->Get_NY()); y++)
		{
			int	yy	= bTransform ? y : pRGB->Get_NY() - 1 - y;

			for(int x=0; x<pRGB->Get_NX(); x++)
			{
				pRGB->Set_Value(x, y, Histogram[SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy))].index);
			}
		}

		if( bTransform )
		{
			Set_Transformation(&pRGB, m[4], m[5], m[0], m[3], m[2], m[1]);
		}

		SET_METADATA(pRGB, "", "OUT_GRID");

		DataObject_Set_Colors(pRGB, Colors);
		DataObject_Update(pRGB, 0, Colors.Get_Count() - 1);
	}

	//-----------------------------------------------------
	// true color...

	else
	{
		CSG_Grid *pRGB = SG_Create_Grid(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), Cellsize, xMin, yMin);

		for(int y=0; y<pRGB->Get_NY() && Set_Progress(y, pRGB->Get_NY()); y++)
		{
			int	yy	= bTransform ? y : pRGB->Get_NY() - 1 - y;

			for(int x=0; x<pRGB->Get_NX(); x++)
			{
				pRGB->Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
			}
		}

		if( bTransform )
		{
			Set_Transformation(&pRGB, m[4], m[5], m[0], m[3], m[2], m[1]);
		}

		//-------------------------------------------------
		if( Parameters("METHOD")->asInt() != 1 ) // true color...
		{
			SET_METADATA(pRGB, "", "OUT_GRID");

			DataObject_Set_Parameter(pRGB, "COLORS_TYPE", 5);	// Color Classification Type: RGB Coded Values
		}
		else                                     // split channels...
		{
			CSG_Grid *pR = SG_Create_Grid(pRGB->Get_System(), SG_DATATYPE_Byte);
			CSG_Grid *pG = SG_Create_Grid(pRGB->Get_System(), SG_DATATYPE_Byte);
			CSG_Grid *pB = SG_Create_Grid(pRGB->Get_System(), SG_DATATYPE_Byte);

			for(int y=0; y<pRGB->Get_NY() && Set_Progress(y, pRGB->Get_NY()); y++)
			{
				for(int x=0; x<pRGB->Get_NX(); x++)
				{
					pR->Set_Value(x, y, SG_GET_R(pRGB->asInt(x, y)));
					pG->Set_Value(x, y, SG_GET_G(pRGB->asInt(x, y)));
					pB->Set_Value(x, y, SG_GET_B(pRGB->asInt(x, y)));
				}
			}

			SET_METADATA(pR, " [R]", "OUT_RED"  );
			SET_METADATA(pG, " [G]", "OUT_GREEN");
			SET_METADATA(pB, " [B]", "OUT_BLUE" );
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Import::Set_Transformation(CSG_Grid **ppImage, double ax, double ay, double dx, double dy, double rx, double ry)
{
	CSG_Vector	A(2);	CSG_Matrix	D(2, 2), DInv;

	A[0]    = ax; A[1]    = ay;
	D[0][0] = dx; D[0][1] = rx;
	D[1][0] = ry; D[1][1] = dy;

	DInv	= D.Get_Inverse();

	//-----------------------------------------------------
	CSG_Grid	*pSource	= *ppImage;

	TSG_Rect	r;	CSG_Vector	XSrc(2), XTgt(2);

	XSrc[0]	= pSource->Get_XMin();	XSrc[1]	= pSource->Get_YMin();	XTgt	= D * XSrc + A;
	r.xMin	= r.xMax	= XTgt[0];
	r.yMin	= r.yMax	= XTgt[1];

	XSrc[0]	= pSource->Get_XMin();	XSrc[1]	= pSource->Get_YMax();	XTgt	= D * XSrc + A;
	if( r.xMin > XTgt[0] )	r.xMin	= XTgt[0];	else if( r.xMax < XTgt[0] )	r.xMax	= XTgt[0];
	if( r.yMin > XTgt[1] )	r.yMin	= XTgt[1];	else if( r.yMax < XTgt[1] )	r.yMax	= XTgt[1];

	XSrc[0]	= pSource->Get_XMax();	XSrc[1]	= pSource->Get_YMax();	XTgt	= D * XSrc + A;
	if( r.xMin > XTgt[0] )	r.xMin	= XTgt[0];	else if( r.xMax < XTgt[0] )	r.xMax	= XTgt[0];
	if( r.yMin > XTgt[1] )	r.yMin	= XTgt[1];	else if( r.yMax < XTgt[1] )	r.yMax	= XTgt[1];

	XSrc[0]	= pSource->Get_XMax();	XSrc[1]	= pSource->Get_YMin();	XTgt	= D * XSrc + A;
	if( r.xMin > XTgt[0] )	r.xMin	= XTgt[0];	else if( r.xMax < XTgt[0] )	r.xMax	= XTgt[0];
	if( r.yMin > XTgt[1] )	r.yMin	= XTgt[1];	else if( r.yMax < XTgt[1] )	r.yMax	= XTgt[1];

	//-----------------------------------------------------
	double z = fabs(dx) < fabs(dy) ? fabs(dx) : fabs(dy);	// guess a suitable cellsize; could be improved...
	int    x = 1 + (int)((r.xMax - r.xMin) / z);
	int    y = 1 + (int)((r.yMax - r.yMin) / z);

	CSG_Grid	*pTarget	= *ppImage	= SG_Create_Grid(pSource->Get_Type(), x, y, z, r.xMin, r.yMin);

	for(y=0, XTgt[1]=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, XTgt[1]+=pTarget->Get_Cellsize())
	{
		for(x=0, XTgt[0]=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, XTgt[0]+=pTarget->Get_Cellsize())
		{
			XSrc	= DInv * (XTgt - A);

			if( pSource->Get_Value(XSrc[0], XSrc[1], z, GRID_RESAMPLING_NearestNeighbour) )
			{
				pTarget->Set_Value(x, y, z);
			}
			else
			{
				pTarget->Set_NoData(x, y);
			}
		}
	}

	delete(pSource);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
