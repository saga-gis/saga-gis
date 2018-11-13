/**********************************************************
 * Version $Id: opencv_svd.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                    opencv_svd.cpp                     //
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
#include "opencv_svd.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_SVD::COpenCV_SVD(void)
{
	Set_Name		(_TL("Single Value Decomposition (OpenCV)"));

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
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Range(
		NULL	, "RANGE"		, _TL("Range"),
		_TL(""),
		0.1, 0.9, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_SVD::On_Execute(void)
{
	int			sMin, sMax;
	CSG_Grid	*pInput, *pOutput;

	int	n	= MIN(Get_NY(), Get_NX());
	int	m	= MAX(Get_NY(), Get_NX());

	pInput	= Parameters("INPUT")	->asGrid();
	pOutput	= Parameters("OUTPUT")	->asGrid();
	sMin	= (int)(Parameters("RANGE")	->asRange()->Get_Min() * n);
	sMax	= (int)(Parameters("RANGE")	->asRange()->Get_Max() * n);

	//-----------------------------------------------------
	IplImage	*cv_pInput	= Get_CVImage(pInput, SG_DATATYPE_Double);

	//-----------------------------------------------------
	IplImage	*cv_pOutput	= Get_CVImage(m, n, SG_DATATYPE_Double);

	CvMat		*cv_pW		= cvCreateMat(n, cv_pInput->height, CV_64FC1);
	CvMat		*cv_pU		= cvCreateMat(n, cv_pInput->height, CV_64FC1);
	CvMat		*cv_pV		= cvCreateMat(n, cv_pInput->width , CV_64FC1);

	//-----------------------------------------------------
	cvSVD(cv_pInput, cv_pW, cv_pU, cv_pV, CV_SVD_MODIFY_A+CV_SVD_V_T);

	CSG_Matrix	u(cv_pU->rows, cv_pU->cols, cv_pU->data.db);
	CSG_Matrix	v(cv_pV->rows, cv_pV->cols, cv_pV->data.db);
	CSG_Matrix	w(cv_pW->rows, cv_pW->cols, cv_pW->data.db);
	CSG_Matrix	svd(Get_NY(), Get_NX());

	for(int i=0; i<n; i++)
		if( i < sMin || i > sMax )
			w[i][i]	= 0.0;

	svd	= u * w;
	svd	*= v.Get_Transpose();

	for(int y=0; y<Get_NY(); y++)
		for(int x=0; x<Get_NX(); x++)
			pOutput->Set_Value(x, y, svd[y][x]);
//			pOutput->Set_Value(x, y, u[y][x]);
//			pOutput->Set_Value(x, y, v[y][x]);
//			pOutput->Set_Value(x, y, w[y][x]);

	//-----------------------------------------------------

//	cvSVBkSb(cv_pW, cv_pU, cv_pV, NULL, cv_pOutput, CV_SVD_U_T+CV_SVD_V_T);

	cvReleaseMat(&cv_pW);
	cvReleaseMat(&cv_pU);
	cvReleaseMat(&cv_pV);

	//-----------------------------------------------------
//	Copy_CVImage_To_Grid(pOutput, cv_pOutput);

    cvReleaseImage(&cv_pInput);
    cvReleaseImage(&cv_pOutput);

	pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), Get_Name().c_str());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
