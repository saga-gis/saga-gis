/**********************************************************
 * Version $Id: tool_counter.h 2332 2014-11-07 14:12:16Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    tool_counter.h                     //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute for Geography                //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__tool_counter_H
#define HEADER_INCLUDED__tool_counter_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTool_Counter : public CSG_Module
{
public:
	CTool_Counter(void);

//	virtual CSG_String			Get_MenuPath	(void)	{	return( SG_T("Translation") );	}


protected:

	virtual bool				On_Execute		(void);


private:

	int							Read_Directory	(const SG_Char *Directory, CSG_Table &Elements);
	int							Read_File		(const SG_Char *File     , CSG_Table &Elements);
	int							Read_Text		(const SG_Char *String, CSG_String &Text);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTool_Menus : public CSG_Module
{
public:
	CTool_Menus(void);

//	virtual CSG_String			Get_MenuPath	(void)	{	return( SG_T("Translation") );	}


protected:

	virtual bool				On_Execute		(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__tool_counter_H
