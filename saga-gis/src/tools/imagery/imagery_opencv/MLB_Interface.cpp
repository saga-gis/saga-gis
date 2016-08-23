/**********************************************************
 * Version $Id: TLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                   TLB_Interface.cpp                   //
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
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"

#include <opencv/cv.h>

//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("OpenCV") );

	case TLB_INFO_Category:
		return( _TL("Imagery") );

	case TLB_INFO_Author:
		return( _TL("O. Conrad (c) 2009") );

	case TLB_INFO_Description:
		{
			CSG_String	s;

			s	+= _TW("OpenCV - \"Open Source Computer Vision Library\"\nVersion: ");
			s	+= CV_VERSION;
			s	+= "\n<a target=\"_blank\" href=\"http://opencv.org\">OpenCV homepage</a>";

			return( s );
		}

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Imagery") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "opencv_morphology.h"
#include "opencv_fourier.h"
#include "opencv_svd.h"
#include "opencv_nnet.h"
#include "opencv_stereo_match.h"
#include "opencv_ml.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new COpenCV_Morphology );
	case  1:	return( new COpenCV_FFT );
	case  2:	return( new COpenCV_SVD );
	case  3:	return( new_COpenCV_NNet );
	case  4:	return( new_COpenCV_Stereo_Match );

	case  5:	return( new_COpenCV_ML_NBayes );
	case  6:	return( new_COpenCV_ML_KNN );
	case  7:	return( new_COpenCV_ML_SVM );
	case  8:	return( new_COpenCV_ML_DTrees );
	case  9:	return( new_COpenCV_ML_Boost );
	case 10:	return( new_COpenCV_ML_RTrees );
	case 11:	return( new_COpenCV_ML_ANN );

	case 12:	return( NULL );
	default:	return( TLB_INTERFACE_SKIP_TOOL );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	TLB_INTERFACE

//}}AFX_SAGA
