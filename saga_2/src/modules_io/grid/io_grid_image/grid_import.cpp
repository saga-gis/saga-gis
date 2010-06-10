
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
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Import::On_Execute(void)
{
	bool				bTransform;
	int					x, y, yy, Method;
	double				ax, ay, dx, dy, rx, ry, xMin, yMin, Cellsize;
	CSG_Colors			Colors;
	CSG_String			fImage, fWorld, Name;
	CSG_Grid			*pImage;
	CSG_File			Stream;
	wxImage				Image;
	wxImageHistogram	Histogram;

	//-----------------------------------------------------
	fImage	= Parameters("FILE")	->asString();
	Method	= Parameters("METHOD")	->asInt();

	Name	= SG_File_Get_Name(fImage, false);

	if( !Image.LoadFile(fImage.c_str()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	     if( SG_File_Cmp_Extension(fImage, SG_T("bmp")) )
	{
		fWorld	= SG_File_Make_Path(NULL, fImage, SG_T("bpw"));
	}
	else if( SG_File_Cmp_Extension(fImage, SG_T("jpg")) )
	{
		fWorld	= SG_File_Make_Path(NULL, fImage, SG_T("jgw"));
	}
	else if( SG_File_Cmp_Extension(fImage, SG_T("png")) )
	{
		fWorld	= SG_File_Make_Path(NULL, fImage, SG_T("pgw"));
	}
	else if( SG_File_Cmp_Extension(fImage, SG_T("tif")) )
	{
		fWorld	= SG_File_Make_Path(NULL, fImage, SG_T("tfw"));
	}
	else
	{
		fWorld	= SG_File_Make_Path(NULL, fImage, SG_T("world"));
	}

	bTransform	= false;
	xMin		= 0.0;
	yMin		= 0.0;
	Cellsize	= 1.0;

	if(	Stream.Open(fWorld, SG_FILE_R, false) && fscanf(Stream.Get_Stream(), "%lf %lf %lf %lf %lf %lf ", &dx, &ry, &rx, &dy, &ax, &ay) == 6 )
	{
		if( dx != -dy || rx != 0.0 || ry != 0.0 )
		{
			bTransform	= true;
		}
		else
		{
			xMin		= ax;
			yMin		= ay + dy * (Image.GetHeight() - 1);
			Cellsize	= dx;
		}
	}


	//-----------------------------------------------------
	// color look-up table...

	if( Method == 0 && (yy = Image.ComputeHistogram(Histogram)) <= 256 )
	{
		Colors.Set_Count(yy);

		for(wxImageHistogram::iterator i=Histogram.begin(); i!=Histogram.end(); ++i)
		{
			Colors.Set_Color(i->second.index, SG_GET_R(i->first), SG_GET_G(i->first), SG_GET_B(i->first));
		}

		pImage	= SG_Create_Grid(yy <= 2 ? SG_DATATYPE_Bit : SG_DATATYPE_Byte, Image.GetWidth(), Image.GetHeight(), Cellsize, xMin, yMin);

		for(y=0; y<pImage->Get_NY() && Set_Progress(y, pImage->Get_NY()); y++)
		{
			yy	= bTransform ? y : pImage->Get_NY() - 1 - y;

			for(x=0; x<pImage->Get_NX(); x++)
			{
				pImage->Set_Value(x, y, Histogram[SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy))].index);
			}
		}

		if( bTransform )
		{
			Set_Transformation(&pImage, ax, ay, dx, dy, rx, ry);
		}

		pImage->Set_Name(Name);
		Parameters("OUT_GRID")->Set_Value(pImage);
		DataObject_Set_Colors(pImage, Colors);
		DataObject_Update(pImage, 0, Colors.Get_Count() - 1);
	}

	//-----------------------------------------------------
	else	// true color...
	{
		pImage	= SG_Create_Grid(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), Cellsize, xMin, yMin);
		pImage	->Set_Name(Name);

		for(y=0; y<pImage->Get_NY() && Set_Progress(y, pImage->Get_NY()); y++)
		{
			yy	= bTransform ? y : pImage->Get_NY() - 1 - y;

			for(x=0; x<pImage->Get_NX(); x++)
			{
				pImage->Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
			}
		}

		if( bTransform )
		{
			Set_Transformation(&pImage, ax, ay, dx, dy, rx, ry);
		}

		//-------------------------------------------------
		if( Method != 1 )	// true color...
		{
			pImage->Get_Projection().Load(fImage, SG_PROJ_FMT_WKT);
			pImage->Set_Name(Name);
			Parameters("OUT_GRID")->Set_Value(pImage);
			DataObject_Set_Colors(pImage, 100, SG_COLORS_BLACK_WHITE);
			DataObject_Set_Parameter(pImage, "COLORS_TYPE", 3);	// Color Classification Type: RGB
		}

		//-------------------------------------------------
		else				// split channels...
		{
			CSG_Grid	*pR, *pG, *pB;

			pR	= SG_Create_Grid(pImage->Get_System(), SG_DATATYPE_Byte);
			pG	= SG_Create_Grid(pImage->Get_System(), SG_DATATYPE_Byte);
			pB	= SG_Create_Grid(pImage->Get_System(), SG_DATATYPE_Byte);

			for(y=0; y<pImage->Get_NY() && Set_Progress(y, pImage->Get_NY()); y++)
			{
				for(x=0; x<pImage->Get_NX(); x++)
				{
					pR->Set_Value(x, y, SG_GET_R(pImage->asInt(x, y)));
					pG->Set_Value(x, y, SG_GET_G(pImage->asInt(x, y)));
					pB->Set_Value(x, y, SG_GET_B(pImage->asInt(x, y)));
				}
			}

			pR->Get_Projection().Load(fImage, SG_PROJ_FMT_WKT);
			pG->Get_Projection().Load(fImage, SG_PROJ_FMT_WKT);
			pB->Get_Projection().Load(fImage, SG_PROJ_FMT_WKT);

			pR->Set_Name(CSG_String::Format(SG_T("%s [R]"), Name.c_str()));
			pG->Set_Name(CSG_String::Format(SG_T("%s [G]"), Name.c_str()));
			pB->Set_Name(CSG_String::Format(SG_T("%s [B]"), Name.c_str()));

			Parameters("OUT_RED")	->Set_Value(pR);
			Parameters("OUT_GREEN")	->Set_Value(pG);
			Parameters("OUT_BLUE")	->Set_Value(pB);

			DataObject_Set_Colors(pR, 100, SG_COLORS_BLACK_RED);
			DataObject_Set_Colors(pG, 100, SG_COLORS_BLACK_GREEN);
			DataObject_Set_Colors(pB, 100, SG_COLORS_BLACK_BLUE);
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
void CGrid_Import::Set_Transformation(CSG_Grid **ppImage, double ax, double ay, double dx, double dy, double rx, double ry)
{
	int			x, y;
	double		z;
	TSG_Rect	r;
	CSG_Vector	A(2), XSrc(2), XTgt(2);
	CSG_Matrix	D(2, 2), DInv;
	CSG_Grid	*pSource, *pTarget;

	//-----------------------------------------------------
	pSource		= *ppImage;

	A[0]	= ax;	A[1]	= ay;
	D[0][0]	= dx;	D[0][1]	= rx;
	D[1][0]	= ry;	D[1][1]	= dy;
	DInv	= D.Get_Inverse();

	//-----------------------------------------------------
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

	z	= fabs(dx) < fabs(dy) ? fabs(dx) : fabs(dy);	// guess a suitable cellsize; could be improved...
	x	= 1 + (int)((r.xMax - r.xMin) / z);
	y	= 1 + (int)((r.yMax - r.yMin) / z);

	//-----------------------------------------------------
	pTarget		= *ppImage	= SG_Create_Grid(pSource->Get_Type(), x, y, z, r.xMin, r.yMin);

	for(y=0, XTgt[1]=pTarget->Get_YMin(); y<pTarget->Get_NY() && Set_Progress(y, pTarget->Get_NY()); y++, XTgt[1]+=pTarget->Get_Cellsize())
	{
		for(x=0, XTgt[0]=pTarget->Get_XMin(); x<pTarget->Get_NX(); x++, XTgt[0]+=pTarget->Get_Cellsize())
		{
			XSrc	= DInv * (XTgt - A);

			if( pSource->Get_Value(XSrc[0], XSrc[1], z, GRID_INTERPOLATION_NearestNeighbour, false, true) )
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
