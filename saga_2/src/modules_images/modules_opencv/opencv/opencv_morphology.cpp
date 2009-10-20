
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
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Dilation"),
			_TL("Erosion"),
			_TL("Median"),
			_TL("User defined rank")
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
		NULL	, "RANK"		, _TL("User defined rank"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.5, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "RESCALE"		, _TL("Rescale Values (0-255)"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
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
	bool		bRescale;
	int			Type, Shape, Radius;
	double		Rank;
	CSG_Grid	*pInput, *pOutput;

	pInput		= Parameters("INPUT")	->asGrid();
	pOutput		= Parameters("OUTPUT")	->asGrid();
	Type		= Parameters("TYPE")	->asInt();
	Shape		= Parameters("SHAPE")	->asInt();
	Radius		= Parameters("RADIUS")	->asInt();
	Rank		= Parameters("RANK")	->asDouble();
	bRescale	= Parameters("RESCALE")	->asBool();

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
	IplImage	*cv_pOutput	= Get_CVImage(Get_NX(), Get_NY(), SG_DATATYPE_Float);

	//-----------------------------------------------------
	IplConvKernel	*cv_pElement	= cvCreateStructuringElementEx(Radius * 2 + 1, Radius * 2 + 1, Radius, Radius, Shape, 0);

	switch( Type )
	{
	case 0:	// Dilation
		cvDilate(cv_pInput, cv_pOutput, cv_pElement, 1);
		break;

	case 1:	// Erosion
		cvErode (cv_pInput, cv_pOutput, cv_pElement, 1);
		break;
	}

	cvReleaseStructuringElement(&cv_pElement);

	//-----------------------------------------------------
	Copy_CVImage_To_Grid(pOutput, cv_pOutput);

    cvReleaseImage(&cv_pInput);
    cvReleaseImage(&cv_pOutput);

	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), Get_Name()));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
