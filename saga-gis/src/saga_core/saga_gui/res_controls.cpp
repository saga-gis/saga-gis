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
const wxString CTRL_Get_Name(int ID_CTRL)
{
	switch( ID_CTRL )
	{
	case wxID_OK:
	case ID_BTN_OK:						return( _TL("[BTN] Okay") );
	case wxID_CANCEL:
	case ID_BTN_CANCEL:					return( _TL("[BTN] Cancel") );

	case ID_BTN_APPLY:					return( _TL("[BTN] Apply") );
	case ID_BTN_RESTORE:				return( _TL("[BTN] Restore") );

	case ID_BTN_EXECUTE:				return( _TL("[BTN] Execute") );

	case ID_BTN_LOAD:					return( _TL("[BTN] Load") );
	case ID_BTN_SAVE:					return( _TL("[BTN] Save") );

	case ID_BTN_DEFAULTS:				return( _TL("[BTN] Defaults") );

	case ID_BTN_ADD:					return( _TL("[BTN] Add") );
	case ID_BTN_ADD_ALL:				return( _TL("[BTN] Add All") );
	case ID_BTN_INSERT:					return( _TL("[BTN] Insert") );
	case ID_BTN_DELETE:					return( _TL("[BTN] Delete") );
	case ID_BTN_DELETE_ALL:				return( _TL("[BTN] Clear") );

	case ID_BTN_UP:						return( _TL("[BTN] Up") );
	case ID_BTN_DOWN:					return( _TL("[BTN] Down") );

	case ID_BTN_COLORS_MIRROR:			return( _TL("[BTN] Mirror") );
	case ID_BTN_COLORS_INVERT:			return( _TL("[BTN] Invert") );
	case ID_BTN_COLORS_RANDOM:			return( _TL("[BTN] Random") );
	case ID_BTN_COLORS_PRESET:			return( _TL("[BTN] Presets") );
	case ID_BTN_COLORS_COUNT:			return( _TL("[BTN] Count") );
	}

	return( _TL("[BTN] Button") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
