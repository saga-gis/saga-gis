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
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 opencv_morphology.cpp                 //
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
#include "opencv_morphology.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_Morphology::COpenCV_Morphology(void)
{
	Set_Name		(_TL("OpenCV - Basic Morphological Operations"));

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
		PARAMETER_OUTPUT, SG_DATATYPE_Byte
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Operation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|"),
			_TL("dilation"),
			_TL("erosion"),
			_TL("opening"),
			_TL("closing"),
 			_TL("morpological gradient"),
			_TL("top hat"),
			_TL("black hat")
		)
	);

	Parameters.Add_Choice(
		NULL	, "SHAPE"		, _TL("Element Shape"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("ellipse"),
			_TL("rectangle"),
			_TL("cross")
		)
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Radius (cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "ITERATIONS"	, _TL("Iterations"),
		_TL(""),
		PARAMETER_TYPE_Int, 1.0, 1.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_Morphology::On_Execute(void)
{
	int			Type, Shape, Radius, Iterations;
	CSG_Grid	*pInput, *pOutput;

	pInput		= Parameters("INPUT")		->asGrid();
	pOutput		= Parameters("OUTPUT")		->asGrid();
	Type		= Parameters("TYPE")		->asInt();
	Shape		= Parameters("SHAPE")		->asInt();
	Radius		= Parameters("RADIUS")		->asInt();
	Iterations	= Parameters("ITERATIONS")	->asInt();

	//-----------------------------------------------------
	switch( Shape )
	{
	default:
	case 0:	Shape	= CV_SHAPE_ELLIPSE;	break;
	case 1:	Shape	= CV_SHAPE_RECT;	break;
	case 2:	Shape	= CV_SHAPE_CROSS;	break;
	}

	//-----------------------------------------------------
	IplImage	*cv_pInput	= Get_CVImage(pInput);
	IplImage	*cv_pOutput	= Get_CVImage(Get_NX(), Get_NY(), pInput->Get_Type());
	IplImage	*cv_pTmp	= NULL;

	//-----------------------------------------------------
	IplConvKernel	*cv_pElement	= cvCreateStructuringElementEx(Radius * 2 + 1, Radius * 2 + 1, Radius, Radius, Shape, 0);

	switch( Type )
	{
	case 0:	// dilation
		cvDilate		(cv_pInput, cv_pOutput, cv_pElement, Iterations);
		break;

	case 1:	// erosion
		cvErode			(cv_pInput, cv_pOutput, cv_pElement, Iterations);
		break;

	case 2:	// opening
		cvMorphologyEx	(cv_pInput, cv_pOutput, cv_pTmp,
			cv_pElement, CV_MOP_OPEN    , Iterations
		);
		break;

	case 3:	// closing
		cvMorphologyEx	(cv_pInput, cv_pOutput, cv_pTmp,
			cv_pElement, CV_MOP_CLOSE   , Iterations
		);
		break;

	case 4:	// morpological gradient
		cvMorphologyEx	(cv_pInput, cv_pOutput, cv_pTmp	= Get_CVImage(Get_NX(), Get_NY(), pInput->Get_Type()),
			cv_pElement, CV_MOP_GRADIENT, Iterations
		);
		break;

	case 5:	// top hat
		cvMorphologyEx	(cv_pInput, cv_pOutput, cv_pTmp	= Get_CVImage(Get_NX(), Get_NY(), pInput->Get_Type()),
			cv_pElement, CV_MOP_TOPHAT  , Iterations
		);
		break;

	case 6:	// black hat
		cvMorphologyEx	(cv_pInput, cv_pOutput, cv_pTmp	= Get_CVImage(Get_NX(), Get_NY(), pInput->Get_Type()),
			cv_pElement, CV_MOP_BLACKHAT, Iterations
		);
		break;
	}

	cvReleaseStructuringElement(&cv_pElement);

	//-----------------------------------------------------
	Copy_CVImage_To_Grid(pOutput, cv_pOutput);

    cvReleaseImage(&cv_pInput);
    cvReleaseImage(&cv_pOutput);

	if( cv_pTmp )
	{
		cvReleaseImage(&cv_pTmp);
	}

	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), Get_Name().c_str()));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
