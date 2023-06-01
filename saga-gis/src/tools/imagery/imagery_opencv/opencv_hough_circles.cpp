
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
//                opencv_hough_circles.cpp               //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
#include "opencv_hough_circles.h"

#include "opencv2/imgproc.hpp"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_Hough_Circles::COpenCV_Hough_Circles(void)
{
	Set_Name		(_TL("Hough Circle Transformation"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"Hough Circle Transformation."
	));

	Add_Reference("https://docs.opencv.org/4.7.0/d4/d70/tutorial_hough_circle.html",
		SG_T("OpenCV Tutorial | Hough Circle Transformation")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "GRID"   , _TL("Grid"   ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Shapes("", "CIRCLES", _TL("Circles"), _TL(""), PARAMETER_OUTPUT, SHAPE_TYPE_Polygon);

	Parameters.Add_Choice("",
		"UNIT"     , _TL("Unit"),
		_TL(""),
		CSG_String::Format("%s|%s", _TL("cells"), _TL("map units")), 0
	);

	Parameters.Add_Range ("UNIT",
		"RADIUS"   , _TL("Radius"),
		_TL(""),
		1., 30., 0., true
	);

	Parameters.Add_Double("UNIT",
		"MIN_DIST"  , _TL("Minimum Distance"),
		_TL("Minimum distance between the centers of the detected circles. If the parameter is too small, multiple neighbor circles may be falsely detected in addition to a true one. If it is too large, some circles may be missed."),
		3., 0., true
	);

	Parameters.Add_Choice("",
		"METHOD"    , _TL("Method"),
		_TL(""),
	#if CV_MAJOR_VERSION > 4 || (CV_MAJOR_VERSION == 4 && CV_MINOR_VERSION >= 3)
		CSG_String::Format("%s|%s", _TL("Hough gradient"), _TL("Hough gradient (alternative)")), 0
	#else
		_TL("Hough gradient"), 0
	#endif
	);

	Parameters.Add_Double("",
		"RESOLUTION", _TL("Accumulator Resolution"),
		_TL("Inverse ratio of the accumulator resolution to the image resolution. if set to 1, the accumulator has the same resolution as the input image. If set to 2, the accumulator has half as big width and height. For \'Hough gradient (alternative)\' the recommended value is 1.5, unless some small very circles need to be detected."),
		3., 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_Hough_Circles::On_Execute(void)
{
	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	cv::Mat Grid(Get_NY(), Get_NX(), CV_8U); // HoughCircles tool works on 8-bit 'gray-scale' images only!

	unsigned char *data = (unsigned char *)Grid.data;

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		data[i] = (unsigned char)((pGrid->asDouble(i) - pGrid->Get_Min()) * 255 / pGrid->Get_Range());
	}

	//-----------------------------------------------------
	double      Scale = Parameters("UNIT")->asInt() == 0 ? 1. : 1. / Get_Cellsize();
	int     minRadius = (int)(Parameters("RADIUS.MIN")->asDouble() * Scale);
	int     maxRadius = (int)(Parameters("RADIUS.MAX")->asDouble() * Scale);
	double    minDist = Parameters("MIN_DIST")->asDouble() * Scale;
	double Resolution = Parameters("RESOLUTION")->asDouble();
#if CV_MAJOR_VERSION > 4 || (CV_MAJOR_VERSION == 4 && CV_MINOR_VERSION >= 3)
	int        Method = Parameters("METHOD")->asInt() == 0 ? cv::HOUGH_GRADIENT : cv::HOUGH_GRADIENT_ALT;
#else
	int        Method = cv::HOUGH_GRADIENT;
#endif

	std::vector<cv::Vec3f> Circles;

	HoughCircles(Grid, Circles, Method, Resolution, minDist, 100, 30, minRadius, maxRadius);

	//-----------------------------------------------------
	CSG_Shapes *pCircles = Parameters("CIRCLES")->asShapes();

	pCircles->Create(SHAPE_TYPE_Polygon);
	pCircles->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Circles"));
	pCircles->Add_Field("ID"      , SG_DATATYPE_Int   );
	pCircles->Add_Field("X_CENTER", SG_DATATYPE_Double);
	pCircles->Add_Field("Y_CENTER", SG_DATATYPE_Double);
	pCircles->Add_Field("RADIUS"  , SG_DATATYPE_Double);

	for(size_t i=0; i<Circles.size(); i++)
	{
		CSG_Shape *pCircle = pCircles->Add_Shape();

		CSG_Point Center(
			Get_XMin() + Get_Cellsize() * Circles[i][0],
			Get_YMin() + Get_Cellsize() * Circles[i][1]
		);

		double Radius = Get_Cellsize() * Circles[i][2];

		pCircle->Set_Value(0, pCircles->Get_Count());
		pCircle->Set_Value(1, Center.x);
		pCircle->Set_Value(2, Center.y);
		pCircle->Set_Value(3, Radius);

		for(double a=0.; a<M_PI_360; a+=5.*M_DEG_TO_RAD)
		{
			double x = Center.x + Radius * sin(a);
			double y = Center.y + Radius * cos(a);

			pCircle->Add_Point(x, y);
		}
	}

	//-----------------------------------------------------
	if( pCircles->Get_Count() > 0 )
	{
		Message_Fmt("\n%s: %lld\n", _TL("Number of detected circles"), pCircles->Get_Count());

		return( true );
	}

	Message_Fmt("\n%s\n", _TL("No circles have been detected"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
