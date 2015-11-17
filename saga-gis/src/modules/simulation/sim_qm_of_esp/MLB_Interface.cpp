/**********************************************************
 * Version $Id: MLB_Interface.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
		return( _TL("QM of ESP") );

	case MLB_INFO_Category:
		return( _TL("Simulation") );

	case MLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2013") );

	case MLB_INFO_Description:
		return( _TW(
			"Quantitative Modeling of Earth Surface Processes.\n"
			"SAGA implementations following the examples from the text book:\n"
			"Pelletier, J.D. (2008): Quantitative Modeling of Earth Surface Processes. Cambridge, 295p.\n"
		));

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Simulation|Quantitative Modeling of Earth Surface Processes") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "hillslope_evolution_ftcs.h"
#include "fill_sinks.h"
#include "flow_routing.h"
#include "successive_flow_routing.h"
#include "hillslope_evolution_adi.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case  0:	return( new CHillslope_Evolution_FTCS );
	case  1:	return( new CFill_Sinks );
	case  2:	return( new CFlow_Routing );
	case  3:	return( new CSuccessive_Flow_Routing );
	case  4:	return( new CHillslope_Evolution_ADI );

	case 11:	return( NULL );
	default:	return( MLB_INTERFACE_SKIP_MODULE );
	}
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
