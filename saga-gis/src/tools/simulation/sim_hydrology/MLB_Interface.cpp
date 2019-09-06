
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
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

///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>


//---------------------------------------------------------
// 2. Place general tool library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Hydrology") );

	case TLB_INFO_Category:
		return( _TL("Simulation") );

	case TLB_INFO_Author:
		return( "O.Conrad (c) 2019" );

	case TLB_INFO_Description:
		return( _TL("Simulation of hydrological processes.") );

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Simulation|Hydrology") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "DVWK_SoilMoisture.h"
#include "KinWav_D8.h"
#include "topmodel.h"
#include "WaterRetentionCapacity.h"
#include "diffuse_pollution_risk.h"
#include "diffusion_gradient_concentration.h"
#include "timed_flow_accumulation.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CDVWK_SoilMoisture );
	case  1:	return( new CKinWav_D8 );
	case  2:	return( new CTOPMODEL );
	case  3:	return( new CWaterRetentionCapacity );
	case  4:	return( new CDiffuse_Pollution_Risk );
	case  5:	return( new CSim_Diffusion_Gradient );
	case  6:	return( new CSim_Diffusion_Concentration );
	case  7:	return( new CSim_Diffusion_Gradient_And_Concentration );
	case  8:	return( new CTimed_Flow_Accumulation );

	case  9:	return( NULL );
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
