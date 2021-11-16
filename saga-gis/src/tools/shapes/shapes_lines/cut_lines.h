///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      cut_lines.h                      //
//                                                       //
//                 Copyright (C) 2021 by                 //
//                   Justus Spitzmüller                  //
//                     scilands GmbH                     //
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
//    contact:    Justus Spitzmüller                     //
//                scilands GmbH                          //
//                Goethe-Allee 11                        //
//                37073 Göttingen                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////
#pragma once

#include <saga_api/saga_api.h>

class CCut_Lines : public CSG_Tool
{
public:

	CCut_Lines (void);
	virtual ~CCut_Lines (void);

protected:

	virtual bool 	On_Execute(void);
	int 			On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
private:
};
