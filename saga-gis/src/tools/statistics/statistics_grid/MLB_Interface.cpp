
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Grids") );

	case TLB_INFO_Category:
		return( _TL("Spatial and Geostatistics") );

	case TLB_INFO_Author:
		return( SG_T("O.Conrad, V.Wichmann (c) 2002-16" ));

	case TLB_INFO_Description:
		return( _TL("Tools for spatial and geostatistical analyses.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Spatial and Geostatistics|Grids") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "fast_representativeness.h"
#include "GSGrid_Residuals.h"
#include "GSGrid_Variance.h"
#include "GSGrid_Variance_Radius.h"
#include "GSGrid_Statistics.h"
#include "GSGrid_Zonal_Statistics.h"
#include "GSGrid_Directional_Statistics.h"
#include "grid_autocorrelation.h"
#include "grid_pca.h"
#include "multiband_variation.h"
#include "grid_latlon_statistics.h"
#include "categorical_variation.h"
#include "grid_pca_focal.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CFast_Representativeness );
	case  1:	return( new CGSGrid_Residuals );
	case  2:	return( new CGSGrid_Variance );
	case  3:	return( new CGSGrid_Variance_Radius );
	case  4:	return( new CGSGrid_Statistics );
	case  5:	return( new CGSGrid_Zonal_Statistics );
	case  6:	return( new CGSGrid_Directional_Statistics );
	case  7:	return( new CGrid_Autocorrelation );
	case  8:	return( new CGrid_PCA );
	case  9:	return( new CMultiBand_Variation );
	case 10:	return( new CGrid_PCA_Inverse );
	case 11:	return( new CGrid_Statistics_Latitudinal );
	case 12:	return( new CGrid_Statistics_Meridional );
	case 13:	return( new CGSGrid_Statistics_To_Table );
	case 14:	return( new CCategorical_Variation );
	case 15:	return( new CGrid_PCA_Focal );

	case 16:	return( NULL );
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
