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
	case wxID_OK               :
	case ID_BTN_OK             :	return( _TL("Okay") );
	case wxID_CANCEL           :
	case ID_BTN_CANCEL         :	return( _TL("Cancel") );

	case ID_BTN_APPLY          :	return( _TL("Apply") );
	case ID_BTN_RESTORE        :	return( _TL("Restore") );

	case ID_BTN_EXECUTE        :	return( _TL("Execute") );

	case ID_BTN_LOAD           :	return( _TL("Load") );
	case ID_BTN_SAVE           :	return( _TL("Save") );

	case ID_BTN_DESCRIPTION     :	return( _TL("Info") );

	case ID_BTN_DEFAULTS        :	return( _TL("Defaults") );

	case ID_BTN_ADD             :	return( _TL("Add") );
	case ID_BTN_ADD_ALL         :	return( _TL("Add All") );
	case ID_BTN_INSERT          :	return( _TL("Insert") );
	case ID_BTN_DELETE          :	return( _TL("Delete") );
	case ID_BTN_DELETE_ALL      :	return( _TL("Clear") );

	case ID_BTN_UP              :	return( _TL("Up") );
	case ID_BTN_DOWN            :	return( _TL("Down") );

	case ID_BTN_TABLE_TO_WKSP   :	return( _TL("Workspace") );
	case ID_BTN_TABLE_FROM_WKSP :	return( _TL("Workspace") );

	case ID_BTN_COLORS          :	return( _TL("Colors") );
	case ID_BTN_COLORS_MIRROR   :	return( _TL("Mirror") );
	case ID_BTN_COLORS_INVERT   :	return( _TL("Invert") );
	case ID_BTN_COLORS_GREYSCALE:	return( _TL("Greyscale") );
	case ID_BTN_COLORS_RANDOM   :	return( _TL("Random") );
	case ID_BTN_COLORS_PRESET   :	return( _TL("Presets") );
	case ID_BTN_COLORS_COUNT    :	return( _TL("Count") );
	}

	return( _TL("Button") );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
