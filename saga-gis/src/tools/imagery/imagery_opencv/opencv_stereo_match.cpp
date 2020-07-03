
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
//                opencv_stereo_match.cpp                //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#include "opencv_stereo_match.h"

#if CV_MAJOR_VERSION >= 3

#include <opencv2/calib3d.hpp>

using namespace cv;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_Stereo_Match::COpenCV_Stereo_Match(void)
{
	Set_Name		(_TL("Stereo Match (OpenCV)"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Stereo Match."
	));

	Add_Reference("https://opencv.org/", SG_T("OpenCV - Open Source Computer Vision"));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"LEFT"		, _TL("Left Image"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RIGHT"		, _TL("Right Image"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"DISPARITY"	, _TL("Disparity Image"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_PointCloud("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"ALGORITHM"		, _TL("Algorithm"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Block Matching"),
			_TL("Modified Hirschmuller")
		)
	);

	Parameters.Add_Int("",
		"DISP_MIN"		, _TL("Minimum Disparity"),
		_TL("Minimum possible disparity value. Normally, it is zero but sometimes rectification algorithms can shift images, so this parameter needs to be adjusted accordingly."),
		0, 0, true
	);

	Parameters.Add_Int("",
		"DISP_NUM"		, _TL("Number of Disparities"),
		_TL("Maximum disparity minus minimum disparity. The value is always greater than zero."),
		1, 1, true
	);

	Parameters.Add_Int("",
		"BLOCKSIZE"		, _TL("Block Size"),
		_TL("The linear size of the blocks compared by the algorithm. Larger block size implies smoother, though less accurate disparity map. Smaller block size gives more detailed disparity map, but there is higher chance for algorithm to find a wrong correspondence."),
		4, 0, true
	);

	Parameters.Add_Int("",
		"DIFF_MAX"		, _TL("Maximum Disparity Difference"),
		_TL("Maximum allowed difference (in integer pixel units) in the left-right disparity check. Set it to a non-positive value to disable the check."),
		1, -1, true
	);

	Parameters.Add_Int("",
		"UNIQUENESS"	, _TL("Uniqueness Ratio"),
		_TL("Margin in percentage by which the best (minimum) computed cost function value should \"win\" the second best value to consider the found match correct. Normally, a value within the 5-15 range is good enough."),
		15, 0, true
	);

	Parameters.Add_Int("",
		"SPECKLE_SIZE"	, _TL("Speckle Window Size"),
		_TL("Maximum size of smooth disparity regions to consider their noise speckles and invalidate. Set it to 0 to disable speckle filtering. Otherwise, set it somewhere in the 50-200 range."),
		100, 0, true
	);

	Parameters.Add_Int("",
		"SPECKLE_RANGE"	, _TL("Speckle Range"),
		_TL("Maximum disparity variation within each connected component. If you do speckle filtering, set the parameter to a positive value, it will be implicitly multiplied by 16. Normally, 1 or 2 is good enough."),
		2, 0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"BM_NODE"		, _TL("Block Matching"),
		_TL("")
	);

	Parameters.Add_Int("BM_NODE",
		"BM_TEXTURE"	, _TL("Texture Threshold"),
		_TL(""),
		31, 0, true
	);

	Parameters.Add_Int("BM_NODE",
		"BM_FILTER_CAP"	, _TL("Prefilter Truncation Value"),
		_TL("Truncation value for the prefiltered image pixels."),
		31, 0, true
	);

//	Parameters.Add_Int("BM_NODE",
//		"BM_FILTER_SIZE", _TL("Prefilter Size"),
//		_TL(""),
//		31, 0, true
//	);

//	Parameters.Add_Choice("BM_NODE",
//		"BM_FILTER_TYPE", _TL("Prefilter Type"),
//		_TL(""),
//		CSG_String::Format("%s|%s",
//			_TL("normalized response"),
//			_TL("Sobel")
//		)
//	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"SGBM_NODE"		, _TL("Modified Hirschmuller"),
		_TL("")
	);

	Parameters.Add_Choice("SGBM_NODE",
		"SGBM_MODE"		, _TL("Mode"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Semi-Global Block Matching"),
			_TL("HH"),
			_TL("SGBM 3 Way")
		)
	);

	Parameters.Add_Int("SGBM_NODE",
		"SGBM_P1"		, _TL("Disparity Smoothness Parameter 1"),
		_TL("The larger the value, the smoother the disparity. Parameter 1 is the penalty on the disparity change by plus or minus 1 between neighbor pixels. The algorithm requires Parameter 2 > Parameter 1."),
		8, 0, true
	);

	Parameters.Add_Int("SGBM_NODE",
		"SGBM_P2"		, _TL("Disparity Smoothness Parameter 2"),
		_TL("The larger the value, the smoother the disparity. Parameter 2 is the penalty on the disparity change by more than 1 between neighbor pixels. The algorithm requires Parameter 2 > Parameter 1."),
		32, 0, true
	);

	Parameters.Add_Int("SGBM_NODE",
		"SGBM_FILTER_CAP", _TL("Prefilter Truncation Value"),
		_TL("Truncation value for the prefiltered image pixels. The algorithm first computes x-derivative at each pixel and clips its value by [-preFilterCap, preFilterCap] interval. The result values are passed to the Birchfield-Tomasi pixel cost function."),
		31, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_Stereo_Match::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("ALGORITHM") )
	{
		pParameters->Set_Enabled(  "BM_NODE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("SGBM_NODE", pParameter->asInt() == 1);
	}

	//-----------------------------------------------------
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_Stereo_Match::On_Execute(void)
{
	Mat	Left, Right, Disparity;

	Get_CVMatrix(Left , Parameters("LEFT" )->asGrid(), SG_DATATYPE_Byte);
	Get_CVMatrix(Right, Parameters("RIGHT")->asGrid(), SG_DATATYPE_Byte);

	switch( Parameters("ALGORITHM")->asInt() )
	{
	//-----------------------------------------------------
	default:
		{
			cv::Ptr<cv::StereoBM>	Algorithm	= cv::StereoBM::create();

			//---------------------------------------------
			Algorithm->setMinDisparity      (Parameters("DISP_MIN"     )->asInt());
			Algorithm->setNumDisparities    (Parameters("DISP_NUM"     )->asInt() * 16);
			Algorithm->setBlockSize         (Parameters("BLOCKSIZE"    )->asInt() * 2 + 1);
			Algorithm->setDisp12MaxDiff     (Parameters("DIFF_MAX"     )->asInt());
			Algorithm->setUniquenessRatio   (Parameters("UNIQUENESS"   )->asInt());
			Algorithm->setSpeckleWindowSize (Parameters("SPECKLE_SIZE" )->asInt());
			Algorithm->setSpeckleRange      (Parameters("SPECKLE_RANGE")->asInt() * 16);

			//---------------------------------------------
			cv::Rect	BM_ROIs[2];

			Algorithm->setROI1(BM_ROIs[0]);
			Algorithm->setROI2(BM_ROIs[1]);

			Algorithm->setTextureThreshold  (Parameters("BM_TEXTURE"    )->asInt());

			//Algorithm->setSmallerBlockSize  (Parameters("BM_SMALLER"    )->asInt());

			Algorithm->setPreFilterCap      (Parameters("BM_FILTER_CAP" )->asInt());
			//Algorithm->setPreFilterSize     (Parameters("BM_FILTER_SIZE")->asInt());

			//switch( Parameters("BM_FILTER_TYPE")->asInt() )
			//{
			//default: Algorithm->setPreFilterType(cv::StereoBM::PREFILTER_NORMALIZED_RESPONSE); break;
			//case  1: Algorithm->setPreFilterType(cv::StereoBM::PREFILTER_XSOBEL             ); break;
			//}

			//---------------------------------------------
			Algorithm->compute(Left, Right, Disparity);
		}
		break;

	//-----------------------------------------------------
	case  1:
		{
			cv::Ptr<cv::StereoSGBM>	Algorithm	= cv::StereoSGBM::create(0, 16, 3);

			//---------------------------------------------
			Algorithm->setMinDisparity      (Parameters("DISP_MIN"     )->asInt());
			Algorithm->setNumDisparities    (Parameters("DISP_NUM"     )->asInt() * 16);
			Algorithm->setBlockSize         (Parameters("BLOCKSIZE"    )->asInt() * 2 + 1);
			Algorithm->setDisp12MaxDiff     (Parameters("DIFF_MAX"     )->asInt());
			Algorithm->setUniquenessRatio   (Parameters("UNIQUENESS"   )->asInt());
			Algorithm->setSpeckleWindowSize (Parameters("SPECKLE_SIZE" )->asInt());
			Algorithm->setSpeckleRange      (Parameters("SPECKLE_RANGE")->asInt() * 16);

			//---------------------------------------------
			switch( Parameters("SGBM_MODE")->asInt() )
			{
			default: Algorithm->setMode(cv::StereoSGBM::MODE_SGBM     ); break;
			case  1: Algorithm->setMode(cv::StereoSGBM::MODE_HH       ); break;
			case  2: Algorithm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY); break;
			}

			Algorithm->setP1                (Parameters("SGBM_P1")->asInt() * Algorithm->getBlockSize()*Algorithm->getBlockSize());
			Algorithm->setP2                (Parameters("SGBM_P2")->asInt() * Algorithm->getBlockSize()*Algorithm->getBlockSize());

			Algorithm->setPreFilterCap      (Parameters("SGBM_FILTER_CAP")->asInt());

			//---------------------------------------------
			Algorithm->compute(Left, Right, Disparity);
		}
		break;
	}

	//-----------------------------------------------------
	Copy_CVMatrix_To_Grid(Parameters("DISPARITY")->asGrid(), Disparity);

	//-----------------------------------------------------
	CSG_PointCloud	*pPoints	= Parameters("POINTS")->asPointCloud();

	if( pPoints )
	{
		pPoints->Create();	pPoints->Set_Name(_TL("Points"));

		const double max_z = 1.0e4;

		cv::Mat	Points, Q(4, 4, CV_32F);

		cv::reprojectImageTo3D(Disparity, Points, Q, true);

		for(int y=0; y<Points.rows; y++)
		{
			for(int x=0; x<Points.cols; x++)
			{
				cv::Vec3f	Point	= Points.at<cv::Vec3f>(y, x);

			//	if( fabs(Point[2] - max_z) <  FLT_EPSILON || fabs(Point[2]) >  max_z ) continue;
				if( fabs(Point[2] - max_z) >= FLT_EPSILON && fabs(Point[2]) <= max_z )
				{
					pPoints->Add_Point(Point[0], Point[1], Point[2]);
				}
			}
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
#endif // CV_MAJOR_VERSION >= 3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
