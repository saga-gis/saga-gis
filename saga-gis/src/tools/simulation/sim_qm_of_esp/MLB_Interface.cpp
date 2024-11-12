
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
		return( _TL("QM of ESP") );

	case TLB_INFO_Category:
		return( _TL("Simulation") );

	case TLB_INFO_Author:
		return( "O.Conrad (c) 2013" );

	case TLB_INFO_Description:
		return( _TW(
			"Quantitative Modeling of Earth Surface Processes.\n"
			"SAGA implementations following the examples from the text book:\n"
			"Pelletier, J.D. (2008): Quantitative Modeling of Earth Surface Processes. Cambridge, 295p. "
			"<a href=\"https://doi.org/10.1017/CBO9780511813849\">doi:10.1017/CBO9780511813849</a>"
		));

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Simulation|Quantitative Modeling of Earth Surface Processes") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "hillslope_evolution_ftcs.h"
#include "fill_sinks.h"
#include "flow_routing.h"
#include "successive_flow_routing.h"
#include "hillslope_evolution_adi.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CHillslope_Evolution_FTCS );
	case  1:	return( new CFill_Sinks );
	case  2:	return( new CFlow_Routing );
	case  3:	return( new CSuccessive_Flow_Routing );
	case  4:	return( new CHillslope_Evolution_ADI );

	case 11:	return( NULL );
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
