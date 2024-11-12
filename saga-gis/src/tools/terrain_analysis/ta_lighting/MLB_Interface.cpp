
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
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
		return( _TL("Lighting, Visibility" ));

	case TLB_INFO_Category:
		return( _TL("Terrain Analysis") );

	case TLB_INFO_Author:
		return( "O.Conrad, V.Wichmann (c) 2003-23" );

	case TLB_INFO_Description:
		return( _TL("Lighting and visibility calculations for digital terrain models." ));

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Terrain Analysis|Lighting" ));
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "HillShade.h"
#include "Visibility_Point.h"
#include "SolarRadiation.h"
#include "SolarRadiationYear.h"
#include "view_shed.h"
#include "topographic_openness.h"
#include "Visibility_Point.h"
#include "geomorphons.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool * Create_Tool(int i)
{
	switch( i )
	{
	case  0: return( new CHillShade );
	case  1: return( new CVisibility_Point );
	case  2: return( new CSolarRadiation );
	case  7: return( new CSolarRadiationYear );
	case  3: return( new CView_Shed );
	case  5: return( new CTopographic_Openness );
	case  6: return( new CVisibility_Points );
	case  8: return( new CGeomorphons );

	//-----------------------------------------------------
	case  9: return( NULL );
	default: return( TLB_INTERFACE_SKIP_TOOL );
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

	TLB_INTERFACE

//}}AFX_SAGA
