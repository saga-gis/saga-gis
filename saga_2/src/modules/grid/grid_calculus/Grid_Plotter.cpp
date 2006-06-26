
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
	Set_Description(_TL(
		"Generate a grid based on a functional expression.\n"
		"The function interpreter uses an expression parser "
		"that offers the folowing operators:\n\n"
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

	Parameters.Add_Grid(	NULL, "RESULT"	, _TL("Function"), "", PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "XMIN"	, "xmin"	, "", PARAMETER_TYPE_Double,-5);
	Parameters.Add_Value(	NULL, "XMAX"	, "xmax"	, "", PARAMETER_TYPE_Double,5);
	Parameters.Add_Value(	NULL, "YMIN"	, "ymin"	, "", PARAMETER_TYPE_Double,-5);
	Parameters.Add_Value(	NULL, "YMAX"	, "ymax"	, "", PARAMETER_TYPE_Double,5);
	Parameters.Add_String(	NULL, "FORMUL"	, _TL("Formula")	, "", "sin(x*x + y*y)");
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

	const char *formel  = Parameters("FORMUL")->asString();

	CMAT_Formula Formel;

	Formel.Set_Formula(formel);

	int Pos;
	const char * Msg;
	if (Formel.Get_Error(&Pos,&Msg))
	{
		CAPI_String	msg;
		msg.Printf(_TL("Error at character #%d of the function: \n%s\n"), Pos, formel);
		
		Message_Add(msg);
		
		msg.Printf("\n%s\n", Msg);
		
		Message_Add(msg);

		return false;
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	for(int x=0; x<Get_NX(); x++)
	{
		pResult->Set_Value(x,y,Formel.Val("xy",(xmax-xmin)*((double)x/Get_NX())+xmin,(ymax-ymin)*((double)y/Get_NY())+ymin)); 
	}
	return( true );
}


