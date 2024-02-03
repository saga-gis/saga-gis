
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 pretty_printer_tool.h                 //
//                                                       //
//                 Copyright (C) 2024 by                 //
//                  Justus Spitzmueller                  //
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
//    e-mail:     jspitzmueller@scilands.de              //
//                                                       //
//    contact:    Justus Spitzmueller                    //
//                scilands GmbH                          //
//                Goethe-Allee 11                        //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#pragma once

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPrettyPrinter : public CSG_Tool
{
public: ////// public members and functions: //////////////

	CPrettyPrinter(void);							// constructor


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////


