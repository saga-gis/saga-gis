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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Plotter.cpp                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

#include "Grid_Plotter.h"

CGrid_Plotter::CGrid_Plotter(void)
{
	Set_Name(_TL("Function"));
	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));
	Set_Description(_TW(
		"Generate a grid based on a functional expression.\n"
		"The function interpreter uses an expression parser "
		"that offers the following operators:\n\n"
		"+ Addition\n"
		"- Subtraction\n"
		"* Multiplication\n"
		"/ Division\n"
		"^ power\n"
		"sin(a)\n"
		"cos(a)\n"
		"tan(a)\n"
		"asin(a)\n"
		"acos(a)\n"
		"atan(a)\n"
		"atan2(a,b)\n"
		"abs(a)\n"
		"int(a)\n"
		"sqrt(a)\n"
		"int(a)\n"
		"mod(a,b)\n"
		"gt(a,b) returns 1 if a greater b\n"
		"lt(a,b) returns 1 if a lower b\n"
		"eq(a,b) returns 1 if a equal b\n"
		"The Variablen are x and y\n"
		"Example: sin(x*x+y*y)/(x*x+y*y)\n")
	);

	Parameters.Add_Grid(	NULL, "RESULT"	, _TL("Function"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "XMIN"	, _TL("xmin")	, _TL(""), PARAMETER_TYPE_Double,-5);
	Parameters.Add_Value(	NULL, "XMAX"	, _TL("xmax")	, _TL(""), PARAMETER_TYPE_Double,5);
	Parameters.Add_Value(	NULL, "YMIN"	, _TL("ymin")	, _TL(""), PARAMETER_TYPE_Double,-5);
	Parameters.Add_Value(	NULL, "YMAX"	, _TL("ymax")	, _TL(""), PARAMETER_TYPE_Double,5);
	Parameters.Add_String(	NULL, "FORMUL"	, _TL("Formula")	, _TL(""), _TL("sin(x*x + y*y)"));
}

//---------------------------------------------------------
CGrid_Plotter::~CGrid_Plotter(void)
{}

//---------------------------------------------------------
bool CGrid_Plotter::On_Execute(void)
{
	pResult		= Parameters("RESULT")->asGrid();

	double xmin	= Parameters("XMIN")->asDouble();
	double ymin	= Parameters("YMIN")->asDouble();
	double xmax	= Parameters("XMAX")->asDouble();
	double ymax	= Parameters("YMAX")->asDouble();

	const SG_Char *formel  = Parameters("FORMUL")->asString();

	CSG_Formula Formel;

	Formel.Set_Formula(formel);

	CSG_String Msg;
	if (Formel.Get_Error(Msg))
	{
		Message_Add(Msg);
		
		return false;
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	for(int x=0; x<Get_NX(); x++)
	{
		pResult->Set_Value(x,y,Formel.Get_Value(SG_T("xy"),(xmax-xmin)*((double)x/Get_NX())+xmin,(ymax-ymin)*((double)y/Get_NY())+ymin)); 
	}
	return( true );
}


