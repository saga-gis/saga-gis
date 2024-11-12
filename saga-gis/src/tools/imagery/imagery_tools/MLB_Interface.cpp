
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
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
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>


//---------------------------------------------------------
// 2. Place general tool library information here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Tools") );

	case TLB_INFO_Category:
		return( _TL("Imagery") );

	case TLB_INFO_Author:
		return( "SAGA User Group Association" );

	case TLB_INFO_Description:
		return( _TL("Image processing and analysis tools.") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Imagery") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "Image_VI_Distance.h"
#include "Image_VI_Slope.h"
#include "evi.h"
#include "tasseled_cap.h"

#include "pansharpening.h"

#include "landsat_toar.h"
#include "landsat_acca.h"
#include "landsat_import.h"
#include "landsat_scene_import.h"

#include "textural_features.h"
#include "local_statistical_measures.h"
#include "image_quality_index.h"
#include "sentinel_2_scene_import.h"
#include "sentinel_3_scene_import.h"

#include "spot_scene_import.h"

#include "Spectral_Profile.h"

#include "topographic_correction.h"
#include "clouds_and_shadows.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CImage_VI_Distance );
	case  1: return( new CImage_VI_Slope );
	case  2: return( new CEnhanced_VI );
	case  3: return( new CTasseled_Cap );

	case  4: return( new CPanSharp_IHS );
	case  5: return( new CPanSharp_Brovey );
	case  6: return( new CPanSharp_CN );
	case  7: return( new CPanSharp_PCA );

	case  8: return( new CLandsat_TOAR );
	case  9: return( new CLandsat_ACCA );
	case 10: return( new CLandsat_Import );
	case 14: return( new CLandsat_Scene_Import );
	case 23: return( new CLandsat_QA_Import );

	case 11: return( new CTextural_Features );
	case 12: return( new CLocal_Statistical_Measures );
	case 13: return( new CImage_Quality_Index );

	case 15: return( new CSentinel_2_Scene_Import );
	case 16: return( new CSentinel_3_Scene_Import );

	case 17: return( new CSpectral_Profile );
	case 18: return( new CSpectral_Profile_Interactive );

	case 19: return( new CTopographic_Correction );

	case 20: return( new CDetect_Clouds );
	case 21: return( new CDetect_CloudShadows );

	case 22: return( new CSPOT_Scene_Import );

	//-----------------------------------------------------
	case 24: return( NULL );
	default: return( TLB_INTERFACE_SKIP_TOOL );
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
