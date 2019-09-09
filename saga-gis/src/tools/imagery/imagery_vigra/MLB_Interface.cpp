
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        VIGRA                          //
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
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>

#include <vigra/stdimage.hxx>


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("ViGrA") );

	case TLB_INFO_Category:
		return( _TL("Imagery") );

	case TLB_INFO_Author:
		return( _TL("O. Conrad (c) 2009") );

	case TLB_INFO_Description:
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
				"<a target=\"_blank\" href=\"http://ukoethe.github.io/vigra/\">"
				"http://hci.iwr.uni-heidelberg.de</a>\n"
			);

			return( s );
		}

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Imagery|ViGrA") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "vigra_smoothing.h"
#include "vigra_edges.h"
#include "vigra_morphology.h"
#include "vigra_distance.h"
#include "vigra_fft.h"
#include "vigra_watershed.h"
#include "vigra_random_forest.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
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
	case  9:	return( new_CViGrA_Random_Forest );
	case 10:	return( new_CViGrA_RF_Presence );
	case 11:	return( new_CViGrA_RF_Table );

	//-----------------------------------------------------
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
