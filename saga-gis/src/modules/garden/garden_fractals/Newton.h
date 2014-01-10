/**********************************************************
 * Version $Id: Newton.h 1921 2014-01-09 10:24:11Z oconrad $
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
//                       Newton.h                        //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Newton_H
#define HEADER_INCLUDED__Newton_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CNewton : public CSG_Module_Grid_Interactive
{
public:
	CNewton(void);
	virtual ~CNewton(void);

	bool					doNewton();

protected:

	virtual bool			On_Execute(void);
	virtual bool			On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);


private:
	double xMin, xMax, yMin, yMax;
	int    iMax, method;
	double dx, dy;


	CSG_Grid	 *pResult, *pShade;

};

#endif // #ifndef HEADER_INCLUDED__Newton_H
