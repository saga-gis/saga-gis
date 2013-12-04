/**********************************************************
 * Version $Id$
 *********************************************************/
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Buffer_Proximity.h                //
//                                                       //
//                 Copyright (C) 2006 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     volker.wichmann@ku-eichstaett.de       //
//                                                       //
//    contact:    Volker Wichmann                        //
//                Research Associate                     //
//                Chair of Physical Geography		     //
//				  KU Eichstätt-Ingolstadt				 //
//                Ostenstr. 18                           //
//                85072 Eichstätt                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Buffer_Proximity_H
#define HEADER_INCLUDED__Grid_Buffer_Proximity_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class grid_tools_EXPORT CGrid_Proximity_Buffer : public CSG_Module_Grid
{
public:
	CGrid_Proximity_Buffer(void);
	virtual ~CGrid_Proximity_Buffer(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Grid|Distances") );	}


protected:

	virtual bool				On_Execute				(void);

};

#endif // #ifndef HEADER_INCLUDED__Grid_Buffer_Proximity_H
