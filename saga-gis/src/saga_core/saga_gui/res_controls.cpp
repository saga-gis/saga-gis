/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   RES_Controls.cpp                    //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "res_controls.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxChar * CTRL_Get_Name(int ID_CTRL)
{
	switch( ID_CTRL )
	{
	case wxID_OK:
	case ID_BTN_OK:						return( LNG("[BTN] Okay") );
	case wxID_CANCEL:
	case ID_BTN_CANCEL:					return( LNG("[BTN] Cancel") );

	case ID_BTN_APPLY:					return( LNG("[BTN] Apply") );
	case ID_BTN_RESTORE:				return( LNG("[BTN] Restore") );

	case ID_BTN_EXECUTE:				return( LNG("[BTN] Execute") );

	case ID_BTN_LOAD:					return( LNG("[BTN] Load") );
	case ID_BTN_SAVE:					return( LNG("[BTN] Save") );

	case ID_BTN_DEFAULTS:				return( LNG("[BTN] Defaults") );

	case ID_BTN_ADD:					return( LNG("[BTN] Add") );
	case ID_BTN_ADD_ALL:				return( LNG("[BTN] Add All") );
	case ID_BTN_INSERT:					return( LNG("[BTN] Insert") );
	case ID_BTN_DELETE:					return( LNG("[BTN] Delete") );
	case ID_BTN_DELETE_ALL:				return( LNG("[BTN] Clear") );

	case ID_BTN_UP:						return( LNG("[BTN] Up") );
	case ID_BTN_DOWN:					return( LNG("[BTN] Down") );

	case ID_BTN_COLORS_MIRROR:			return( LNG("[BTN] Mirror") );
	case ID_BTN_COLORS_INVERT:			return( LNG("[BTN] Invert") );
	case ID_BTN_COLORS_RANDOM:			return( LNG("[BTN] Random") );
	case ID_BTN_COLORS_PRESET:			return( LNG("[BTN] Presets") );
	case ID_BTN_COLORS_COUNT:			return( LNG("[BTN] Count") );
	}

	return( LNG("[BTN] Button") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
