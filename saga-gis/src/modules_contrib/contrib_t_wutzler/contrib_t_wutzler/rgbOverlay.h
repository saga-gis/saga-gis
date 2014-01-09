/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                      $$modul$$                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      RGBOverlay.h                       //
//                                                       //
//            Copyright (C) 2003 Your Name               //
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
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if !defined(SAGA_RGBOverlay_H__DD97C872_D413_11D4_B653_525400DF71C8__INCLUDED_)
#define SAGA_RGBOverlay_H__DD97C872_D413_11D4_B653_525400DF71C8__INCLUDED_

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CRGBOverlay : public CSG_Module_Grid
{
public:
	CRGBOverlay(void);
	virtual ~CRGBOverlay(void);


protected:

	virtual bool		On_Execute(void);


private:

};

#endif // !defined(SAGA_RGBOverlay_H__DD97C872_D413_11D4_B653_525400DF71C8__INCLUDED_)
