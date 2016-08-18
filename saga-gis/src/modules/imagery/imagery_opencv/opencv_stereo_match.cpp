/**********************************************************
 * Version $Id: opencv_stereo_match.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "opencv_stereo_match.h"

#if CV_MAJOR_VERSION == 3


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
		"References:\n"
		"OpenCV - Open Source Computer Vision\n"
		"<a target=\"_blank\" href=\"http://opencv.org\">http://opencv.org</a>"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	Parameters.Add_Grid(
		NULL	, "LEFT"		, _TL("Left Image"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RIGHT"		, _TL("Right Image"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "DISPARITY"	, _TL("Disparity Image"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "ALGORITHM"		, _TL("Algorithm"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Block Matching"),
			_TL("Modified Hirschmuller")
		)
	);

	Parameters.Add_Int(
		NULL	, "DISP_MIN"		, _TL("Minimum Disparity"),
		_TL("Minimum possible disparity value. Normally, it is zero but sometimes rectification algorithms can shift images, so this parameter needs to be adjusted accordingly."),
		0, 0, true
	);

	Parameters.Add_Int(
		NULL	, "DISP_NUM"		, _TL("Number of Disparities"),
		_TL("Maximum disparity minus minimum disparity. The value is always greater than zero."),
		1, 1, true
	);

	Parameters.Add_Int(
		NULL	, "BLOCKSIZE"		, _TL("Block Size"),
		_TL("The linear size of the blocks compared by the algorithm. Larger block size implies smoother, though less accurate disparity map. Smaller block size gives more detailed disparity map, but there is higher chance for algorithm to find a wrong correspondence."),
		4, 0, true
	);

	Parameters.Add_Int(
		NULL	, "DIFF_MAX"		, _TL("Maximum Disparity Difference"),
		_TL("Maximum allowed difference (in integer pixel units) in the left-right disparity check. Set it to a non-positive value to disable the check."),
		1, -1, true
	);

	Parameters.Add_Int(
		NULL	, "UNIQUENESS"		, _TL("Uniqueness Ratio"),
		_TL("Margin in percentage by which the best (minimum) computed cost function value should \"win\" the second best value to consider the found match correct. Normally, a value within the 5-15 range is good enough."),
		15, 0, true
	);

	Parameters.Add_Int(
		NULL	, "SPECKLE_SIZE"	, _TL("Speckle Window Size"),
		_TL("Maximum size of smooth disparity regions to consider their noise speckles and invalidate. Set it to 0 to disable speckle filtering. Otherwise, set it somewhere in the 50-200 range."),
		100, 0, true
	);

	Parameters.Add_Int(
		NULL	, "SPECKLE_RANGE"	, _TL("Speckle Range"),
		_TL("Maximum disparity variation within each connected component. If you do speckle filtering, set the parameter to a positive value, it will be implicitly multiplied by 16. Normally, 1 or 2 is good enough."),
		2, 0, true
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "BM_NODE"			, _TL("Block Matching"),
		_TL("")
	);

	Parameters.Add_Int(
		pNode	, "BM_TEXTURE"		, _TL("Texture Threshold"),
		_TL(""),
		31, 0, true
	);

	Parameters.Add_Int(
		pNode	, "BM_FILTER_CAP"	, _TL("Prefilter Truncation Value"),
		_TL("Truncation value for the prefiltered image pixels."),
		31, 0, true
	);

	//Parameters.Add_Int(
	//	pNode	, "BM_FILTER_SIZE"	, _TL("Prefilter Size"),
	//	_TL(""),
	//	31, 0, true
	//);

	//Parameters.Add_Choice(
	//	pNode	, "BM_FILTER_TYPE"	, _TL("Prefilter Type"),
	//	_TL(""),
	//	CSG_String::Format("%s|%s|",
	//		_TL("normalized response"),
	//		_TL("Sobel")
	//	)
	//);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "SGBM_NODE"		, _TL("Modified Hirschmuller"),
		_TL("")
	);

	Parameters.Add_Choice(
		pNode	, "SGBM_MODE"		, _TL("Mode"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("Semi-Global Block Matching"),
			_TL("HH"),
			_TL("SGBM 3 Way")
		)
	);

	Parameters.Add_Int(
		pNode	, "SGBM_P1"			, _TL("Disparity Smoothness Parameter 1"),
		_TL("The larger the value, the smoother the disparity. Parameter 1 is the penalty on the disparity change by plus or minus 1 between neighbor pixels. The algorithm requires Parameter 2 > Parameter 1."),
		8, 0, true
	);

	Parameters.Add_Int(
		pNode	, "SGBM_P2"			, _TL("Disparity Smoothness Parameter 2"),
		_TL("The larger the value, the smoother the disparity. Parameter 2 is the penalty on the disparity change by more than 1 between neighbor pixels. The algorithm requires Parameter 2 > Parameter 1."),
		32, 0, true
	);

	Parameters.Add_Int(
		pNode	, "SGBM_FILTER_CAP"	, _TL("Prefilter Truncation Value"),
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
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "ALGORITHM") )
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
	//-----------------------------------------------------
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
	Copy_CVMatrix_To_Grid(Parameters("DISPARITY")->asGrid(), &Disparity);

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
#endif // CV_MAJOR_VERSION == 2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

/*/---------------------------------------------------------
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include <stdio.h>

using namespace cv;

static void print_help()
{
    printf("\nDemo stereo matching converting L and R images into disparity and point clouds\n");
    printf("\nUsage: stereo_match <left_image> <right_image> [--algorithm=bm|sgbm|hh|var] [--blocksize=<block_size>]\n"
           "[--max-disparity=<max_disparity>] [--scale=scale_factor>] [-i <intrinsic_filename>] [-e <extrinsic_filename>]\n"
           "[--no-display] [-o <disparity_image>] [-p <point_cloud_file>]\n");
}

static void saveXYZ(const char* filename, const Mat& mat)
{
    const double max_z = 1.0e4;
    FILE* fp = fopen(filename, "wt");
    for(int y = 0; y < mat.rows; y++)
    {
        for(int x = 0; x < mat.cols; x++)
        {
            Vec3f point = mat.at<Vec3f>(y, x);
            if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
            fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
        }
    }
    fclose(fp);
}

int main(int argc, char** argv)
{
    const char* algorithm_opt = "--algorithm=";
    const char* maxdisp_opt = "--max-disparity=";
    const char* blocksize_opt = "--blocksize=";
    const char* nodisplay_opt = "--no-display";
    const char* scale_opt = "--scale=";

    if(argc < 3)
    {
        print_help();
        return 0;
    }
    const char* img1_filename = 0;
    const char* img2_filename = 0;
    const char* intrinsic_filename = 0;
    const char* extrinsic_filename = 0;
    const char* disparity_filename = 0;
    const char* point_cloud_filename = 0;

    enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3 };
    int alg = STEREO_SGBM;
    int SADWindowSize = 0, numberOfDisparities = 0;
    bool no_display = false;
    float scale = 1.f;

    StereoBM bm;
    StereoSGBM sgbm;
    StereoVar var;

    for( int i = 1; i < argc; i++ )
    {
        if( argv[i][0] != '-' )
        {
            if( !img1_filename )
                img1_filename = argv[i];
            else
                img2_filename = argv[i];
        }
        else if( strncmp(argv[i], algorithm_opt, strlen(algorithm_opt)) == 0 )
        {
            char* _alg = argv[i] + strlen(algorithm_opt);
            alg = strcmp(_alg, "bm") == 0 ? STEREO_BM :
                  strcmp(_alg, "sgbm") == 0 ? STEREO_SGBM :
                  strcmp(_alg, "hh") == 0 ? STEREO_HH :
                  strcmp(_alg, "var") == 0 ? STEREO_VAR : -1;
            if( alg < 0 )
            {
                printf("Command-line parameter error: Unknown stereo algorithm\n\n");
                print_help();
                return -1;
            }
        }
        else if( strncmp(argv[i], maxdisp_opt, strlen(maxdisp_opt)) == 0 )
        {
            if( sscanf( argv[i] + strlen(maxdisp_opt), "%d", &numberOfDisparities ) != 1 ||
                numberOfDisparities < 1 || numberOfDisparities % 16 != 0 )
            {
                printf("Command-line parameter error: The max disparity (--maxdisparity=<...>) must be a positive integer divisible by 16\n");
                print_help();
                return -1;
            }
        }
        else if( strncmp(argv[i], blocksize_opt, strlen(blocksize_opt)) == 0 )
        {
            if( sscanf( argv[i] + strlen(blocksize_opt), "%d", &SADWindowSize ) != 1 ||
                SADWindowSize < 1 || SADWindowSize % 2 != 1 )
            {
                printf("Command-line parameter error: The block size (--blocksize=<...>) must be a positive odd number\n");
                return -1;
            }
        }
        else if( strncmp(argv[i], scale_opt, strlen(scale_opt)) == 0 )
        {
            if( sscanf( argv[i] + strlen(scale_opt), "%f", &scale ) != 1 || scale < 0 )
            {
                printf("Command-line parameter error: The scale factor (--scale=<...>) must be a positive floating-point number\n");
                return -1;
            }
        }
        else if( strcmp(argv[i], nodisplay_opt) == 0 )
            no_display = true;
        else if( strcmp(argv[i], "-i" ) == 0 )
            intrinsic_filename = argv[++i];
        else if( strcmp(argv[i], "-e" ) == 0 )
            extrinsic_filename = argv[++i];
        else if( strcmp(argv[i], "-o" ) == 0 )
            disparity_filename = argv[++i];
        else if( strcmp(argv[i], "-p" ) == 0 )
            point_cloud_filename = argv[++i];
        else
        {
            printf("Command-line parameter error: unknown option %s\n", argv[i]);
            return -1;
        }
    }

    if( !img1_filename || !img2_filename )
    {
        printf("Command-line parameter error: both left and right images must be specified\n");
        return -1;
    }

    if( (intrinsic_filename != 0) ^ (extrinsic_filename != 0) )
    {
        printf("Command-line parameter error: either both intrinsic and extrinsic parameters must be specified, or none of them (when the stereo pair is already rectified)\n");
        return -1;
    }

    if( extrinsic_filename == 0 && point_cloud_filename )
    {
        printf("Command-line parameter error: extrinsic and intrinsic parameters must be specified to compute the point cloud\n");
        return -1;
    }

    int color_mode = alg == STEREO_BM ? 0 : -1;
    Mat img1 = imread(img1_filename, color_mode);
    Mat img2 = imread(img2_filename, color_mode);

    if( scale != 1.f )
    {
        Mat temp1, temp2;
        int method = scale < 1 ? INTER_AREA : INTER_CUBIC;
        resize(img1, temp1, Size(), scale, scale, method);
        img1 = temp1;
        resize(img2, temp2, Size(), scale, scale, method);
        img2 = temp2;
    }

    Size img_size = img1.size();

    Rect roi1, roi2;
    Mat Q;

    if( intrinsic_filename )
    {
        // reading intrinsic parameters
        FileStorage fs(intrinsic_filename, CV_STORAGE_READ);
        if(!fs.isOpened())
        {
            printf("Failed to open file %s\n", intrinsic_filename);
            return -1;
        }

        Mat M1, D1, M2, D2;
        fs["M1"] >> M1;
        fs["D1"] >> D1;
        fs["M2"] >> M2;
        fs["D2"] >> D2;

        M1 *= scale;
        M2 *= scale;

        fs.open(extrinsic_filename, CV_STORAGE_READ);
        if(!fs.isOpened())
        {
            printf("Failed to open file %s\n", extrinsic_filename);
            return -1;
        }

        Mat R, T, R1, P1, R2, P2;
        fs["R"] >> R;
        fs["T"] >> T;

        stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );

        Mat map11, map12, map21, map22;
        initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
        initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);

        Mat img1r, img2r;
        remap(img1, img1r, map11, map12, INTER_LINEAR);
        remap(img2, img2r, map21, map22, INTER_LINEAR);

        img1 = img1r;
        img2 = img2r;
    }

    numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((img_size.width/8) + 15) & -16;

    bm.state->roi1 = roi1;
    bm.state->roi2 = roi2;
    bm.state->preFilterCap = 31;
    bm.state->SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 9;
    bm.state->minDisparity = 0;
    bm.state->numberOfDisparities = numberOfDisparities;
    bm.state->textureThreshold = 10;
    bm.state->uniquenessRatio = 15;
    bm.state->speckleWindowSize = 100;
    bm.state->speckleRange = 32;
    bm.state->disp12MaxDiff = 1;

    sgbm.preFilterCap = 63;
    sgbm.SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 3;

    int cn = img1.channels();

    sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
    sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
    sgbm.minDisparity = 0;
    sgbm.numberOfDisparities = numberOfDisparities;
    sgbm.uniquenessRatio = 10;
    sgbm.speckleWindowSize = bm.state->speckleWindowSize;
    sgbm.speckleRange = bm.state->speckleRange;
    sgbm.disp12MaxDiff = 1;
    sgbm.fullDP = alg == STEREO_HH;

    var.levels = 3;                                 // ignored with USE_AUTO_PARAMS
    var.pyrScale = 0.5;                             // ignored with USE_AUTO_PARAMS
    var.nIt = 25;
    var.minDisp = -numberOfDisparities;
    var.maxDisp = 0;
    var.poly_n = 3;
    var.poly_sigma = 0.0;
    var.fi = 15.0f;
    var.lambda = 0.03f;
    var.penalization = var.PENALIZATION_TICHONOV;   // ignored with USE_AUTO_PARAMS
    var.cycle = var.CYCLE_V;                        // ignored with USE_AUTO_PARAMS
    var.flags = var.USE_SMART_ID | var.USE_AUTO_PARAMS | var.USE_INITIAL_DISPARITY | var.USE_MEDIAN_FILTERING ;

    Mat disp, disp8;
    //Mat img1p, img2p, dispp;
    //copyMakeBorder(img1, img1p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
    //copyMakeBorder(img2, img2p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);

    int64 t = getTickCount();
    if( alg == STEREO_BM )
        bm(img1, img2, disp);
    else if( alg == STEREO_VAR ) {
        var(img1, img2, disp);
    }
    else if( alg == STEREO_SGBM || alg == STEREO_HH )
        sgbm(img1, img2, disp);
    t = getTickCount() - t;
    printf("Time elapsed: %fms\n", t*1000/getTickFrequency());

    //disp = dispp.colRange(numberOfDisparities, img1p.cols);
    if( alg != STEREO_VAR )
        disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
    else
        disp.convertTo(disp8, CV_8U);
    if( !no_display )
    {
        namedWindow("left", 1);
        imshow("left", img1);
        namedWindow("right", 1);
        imshow("right", img2);
        namedWindow("disparity", 0);
        imshow("disparity", disp8);
        printf("press any key to continue...");
        fflush(stdout);
        waitKey();
        printf("\n");
    }

    if(disparity_filename)
        imwrite(disparity_filename, disp8);

    if(point_cloud_filename)
    {
        printf("storing the point cloud...");
        fflush(stdout);
        Mat xyz;
        reprojectImageTo3D(disp, xyz, Q, true);
        saveXYZ(point_cloud_filename, xyz);
        printf("\n");
    }

    return 0;
}
/**/
