/**********************************************************
 * Version $Id: MLB_Interface.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        VIGRA                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
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
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("ViGrA") );

	case MLB_INFO_Category:
		return( _TL("Imagery") );

	case MLB_INFO_Author:
		return( _TL("O. Conrad (c) 2009") );

	case MLB_INFO_Description:
		{
			CSG_String	s;
			
			s	+= "ViGrA - \"Vision with Generic Algorithms\"\n";
#ifdef VIGRA_VERSION
			s	+= "Version: ";	s += VIGRA_VERSION; s += "\n";
#endif
			s	+= _TW(
				"ViGrA is a novel computer vision library that puts its main "
				"emphasize on customizable algorithms and data structures. "
				"By using template techniques similar to those in the C++ "
				"Standard Template Library (STL), you can easily adapt any ViGrA "
				"component to the needs of your application, without thereby "
				"giving up execution speed.\n"
				"Find out more at the ViGrA - Vision with Generic Algorithms - homepage:\n"
				"<a target=\"_blank\" href=\"http://hci.iwr.uni-heidelberg.de/vigra\">"
				"http://hci.iwr.uni-heidelberg.de</a>\n"
			);

			return( s );
		}

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Imagery|ViGrA") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "vigra_smoothing.h"
#include "vigra_edges.h"
#include "vigra_morphology.h"
#include "vigra_distance.h"
#include "vigra_fft.h"
#include "vigra_watershed.h"
#include "vigra_random_forest.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CViGrA_Smoothing );
	case  1:	return( new CViGrA_Edges );
	case  2:	return( new CViGrA_Morphology );
	case  3:	return( new CViGrA_Distance );
	case  4:	return( new CViGrA_Watershed );
	case  5:	return( new CViGrA_FFT );
	case  6:	return( new CViGrA_FFT_Inverse );
	case  7:	return( new CViGrA_FFT_Real );
	case  8:	return( new CViGrA_FFT_Filter );
	case  9:	return( new CViGrA_Random_Forest );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA
