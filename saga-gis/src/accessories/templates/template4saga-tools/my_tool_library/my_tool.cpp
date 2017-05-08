
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                       my_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     my_tool.cpp                       //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                      Hein Bloed                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2.1 of the   //
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
//    e-mail:     hbloed@saga-gis.org                    //
//                                                       //
//    contact:    Hein Bloed                             //
//                Sesamestreet 42                        //
//                Metropolis                             //
//                Middle Earth                           //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "my_tool.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMy_Tool::CMy_Tool(void)
{
	Set_Name		(_TL("My Tool"));

	Set_Author		("H.Bloed (c) 2017");

	Set_Description	(_TW(
		"My tool description. "
	));

	Parameters.Add_Double("",
		"VALUE"	, _TL("Value"),
		_TL(""),
		M_PI
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMy_Tool::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMy_Tool::On_Execute(void)
{
	double	Value;

	Value	= Parameters("VALUE")->asDouble();

	Message_Dlg(CSG_String::Format("%s: %f", _TL("Value"), Value), _TL("Hello World"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
