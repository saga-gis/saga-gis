/**********************************************************
 * Version $Id: opencv_fourier.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   opencv_fourier.cpp                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "opencv_fourier.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	DFT		(IplImage *pInput, IplImage **ppReal, IplImage **ppImag);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_FFT::COpenCV_FFT(void)
{
	Set_Name		(_TL("Fourier Transformation (OpenCV)"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"References:\n"
		"OpenCV - Open Source Computer Vision\n"
		"<a target=\"_blank\" href=\"http://opencv.willowgarage.com\">http://opencv.willowgarage.com</a>"
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "REAL"		, _TL("Fourier Transformation (Real)"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "IMAG"		, _TL("Fourier Transformation (Imaginary)"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_FFT::On_Execute(void)
{
	CSG_Grid	*pInput, *pReal, *pImag;

	pInput		= Parameters("INPUT")	->asGrid();
	pReal		= Parameters("REAL")	->asGrid();
	pImag		= Parameters("IMAG")	->asGrid();

	//-----------------------------------------------------
	IplImage	*cv_pInput	= Get_CVImage(pInput, SG_DATATYPE_Float);
	IplImage	*cv_pReal	= NULL;// Get_CVImage(Get_NX(), Get_NY(), SG_DATATYPE_Float);
	IplImage	*cv_pImag	= NULL;// Get_CVImage(Get_NX(), Get_NY(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	DFT(cv_pInput, &cv_pReal, &cv_pImag);

	//-----------------------------------------------------
	Copy_CVImage_To_Grid(pReal, cv_pReal, false);
	Copy_CVImage_To_Grid(pImag, cv_pImag, false);

    cvReleaseImage(&cv_pInput);
    cvReleaseImage(&cv_pReal);
    cvReleaseImage(&cv_pImag);

	pReal->Set_Name(CSG_String::Format(SG_T("%s [DFT, %s]"), pInput->Get_Name(), _TL("Real")));
	pImag->Set_Name(CSG_String::Format(SG_T("%s [DFT, %s]"), pInput->Get_Name(), _TL("Imaginary")));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Rearrange the quadrants of Fourier image so that the origin is at
// the image center
// src & dst arrays of equal size & type
bool cvShiftDFT(CvArr * src_arr, CvArr * dst_arr )
{
	CvMat * tmp;
	CvMat q1stub, q2stub;
	CvMat q3stub, q4stub;
	CvMat d1stub, d2stub;
	CvMat d3stub, d4stub;
	CvMat * q1, * q2, * q3, * q4;
	CvMat * d1, * d2, * d3, * d4;

	CvSize size = cvGetSize(src_arr);
	CvSize dst_size = cvGetSize(dst_arr);
	int cx, cy;

	if( dst_size.width != size.width || dst_size.height != size.height )
	{
		// cvError( CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__, __LINE__ );   
		return( false );
	}

	if(src_arr==dst_arr){
		tmp = cvCreateMat(size.height/2, size.width/2, cvGetElemType(src_arr));
	}

	cx = size.width/2;
	cy = size.height/2; // image center

	q1 = cvGetSubRect( src_arr, &q1stub, cvRect(0,0,cx, cy) );
	q2 = cvGetSubRect( src_arr, &q2stub, cvRect(cx,0,cx,cy) );
	q3 = cvGetSubRect( src_arr, &q3stub, cvRect(cx,cy,cx,cy) );
	q4 = cvGetSubRect( src_arr, &q4stub, cvRect(0,cy,cx,cy) );
	d1 = cvGetSubRect( src_arr, &d1stub, cvRect(0,0,cx,cy) );
	d2 = cvGetSubRect( src_arr, &d2stub, cvRect(cx,0,cx,cy) );
	d3 = cvGetSubRect( src_arr, &d3stub, cvRect(cx,cy,cx,cy) );
	d4 = cvGetSubRect( src_arr, &d4stub, cvRect(0,cy,cx,cy) );

	if(src_arr!=dst_arr){
		if( !CV_ARE_TYPES_EQ( q1, d1 )){
		//	cvError( CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format", __FILE__, __LINE__ ); 
			return( false );
		}
		cvCopy(q3, d1, 0);
		cvCopy(q4, d2, 0);
		cvCopy(q1, d3, 0);
		cvCopy(q2, d4, 0);
	}
	else{
		cvCopy(q3, tmp, 0);
		cvCopy(q1, q3, 0);
		cvCopy(tmp, q1, 0);
		cvCopy(q4, tmp, 0);
		cvCopy(q2, q4, 0);
		cvCopy(tmp, q2, 0);
	}

	return( true );
}

//---------------------------------------------------------
bool DFT(IplImage *im, IplImage **ppReal, IplImage **ppImag)
{
	IplImage * realInput;
	IplImage * imaginaryInput;
	IplImage * complexInput;
	int dft_M, dft_N;
	CvMat* dft_A, tmp;
	IplImage * image_Re;
	IplImage * image_Im;

	if( !im )
		return false;

	realInput		= cvCreateImage( cvGetSize(im), IPL_DEPTH_64F, 1);
	imaginaryInput	= cvCreateImage( cvGetSize(im), IPL_DEPTH_64F, 1);
	complexInput	= cvCreateImage( cvGetSize(im), IPL_DEPTH_64F, 2);

	cvScale(im, realInput, 1.0, 0.0);
	cvZero(imaginaryInput);
	cvMerge(realInput, imaginaryInput, NULL, NULL, complexInput);

	dft_M = cvGetOptimalDFTSize( im->height - 1 );
	dft_N = cvGetOptimalDFTSize( im->width - 1 );

	dft_A = cvCreateMat( dft_M, dft_N, CV_64FC2 );
	*ppReal	= image_Re = cvCreateImage( cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);
	*ppImag	= image_Im = cvCreateImage( cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);

	// copy A to dft_A and pad dft_A with zeros
	cvGetSubRect( dft_A, &tmp, cvRect(0,0, im->width, im->height));
	cvCopy( complexInput, &tmp, NULL );
	if( dft_A->cols > im->width )
	{
		cvGetSubRect( dft_A, &tmp, cvRect(im->width,0, dft_A->cols - im->width, im->height));
		cvZero( &tmp );
	}

	// no need to pad bottom part of dft_A with zeros because of
	// use nonzero_rows parameter in cvDFT() call below

	cvDFT( dft_A, dft_A, CV_DXT_FORWARD, complexInput->height );

	// Split Fourier in real and imaginary parts
	cvSplit( dft_A, image_Re, image_Im, 0, 0 );

	// Compute the magnitude of the spectrum Mag = sqrt(Re^2 + Im^2)
//	cvPow( image_Re, image_Re, 2.0);
//	cvPow( image_Im, image_Im, 2.0);
//	cvAdd( image_Re, image_Im, image_Re, NULL);
//	cvPow( image_Re, image_Re, 0.5 );

	// Compute log(1 + Mag)
//	cvAddS( image_Re, cvScalarAll(1.0), image_Re, NULL ); // 1 + Mag
//	cvLog( image_Re, image_Re ); // log(1 + Mag)

	// Rearrange the quadrants of Fourier image so that the origin is at
	// the image center
//	cvShiftDFT( image_Re, image_Re );

//	cvMinMaxLoc(image_Re, &m, &M, NULL, NULL, NULL);
//	cvScale(image_Re, image_Re, 1.0/(M-m), 1.0*(-m)/(M-m));

	return true;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
