
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
//                  Grid_Calculator.cpp                  //
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

#include "Grid_Calculator.h"

#include <float.h>

CGrid_Calculator::CGrid_Calculator(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Grid Calculator"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description(_TL(
		"The Grid Calculator calculates a new grid based on existing grids and a mathematical formula.\n\n"
		"The following operators are available for the formula definition:\n"
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
		"ln(a)\n"
		"mod(a,b)\n"
		"gt(a, b) returns 1 if a greater b\n"
		"lt(a, b) returns 1 if a lower b\n"
		"eq(a, b) returns 1 if a equal b\n"
		"ifelse(switch, x,  y) returns x if switch equals 1 else y\n\n"
		"The grid variables are single characters a which correspond in "
		"alphabetical order with the grid list order ('a' = first grid, 'b' = second grid, ...)\n"
		"Example with three grids: sin(a)*b+c\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "INPUT"	, _TL("Grids"),
		"", PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Result"),
		"",
		PARAMETER_OUTPUT
	);
	
	Parameters.Add_String(
		NULL, "FORMUL"	, _TL("Formula"),
		"",
		"(a-b)/(a+b)"
	);
}

//---------------------------------------------------------
CGrid_Calculator::~CGrid_Calculator(void)
{}

//---------------------------------------------------------
#if defined(_SAGA_LINUX)
bool _finite(double val)
{
	return( true );
}
#endif

//---------------------------------------------------------
bool CGrid_Calculator::On_Execute(void)
{
	char			vars[27];
	bool			bContinue;
	
	int				i, x, y;

	if (Parameters("INPUT")->asInt() <= 0)	// Gr� von O.C...
	{
		Error_Set(_TL("No grid in list - Cannot execute calculator"));
		return (false);
	}
	
	pResult		= Parameters("RESULT")->asGrid();
	pResult->Set_Name(Parameters("FORMUL")->asString());
	
	nGrids		= Parameters("INPUT")->asInt();
	Grids		=(CSG_Grid **)Parameters("INPUT")->asPointer();
	
	for (i = 0, bContinue = true; i < nGrids - 1 && bContinue; i++)
	{
		if( (Grids[i]->Get_System() == Grids[i + 1]->Get_System()) == false )
		{
			bContinue	= false;
		}
	}
	
	CSG_Grid MissingMap(Grids[0], GRID_TYPE_Byte);
	
	for (y = 0; y < Get_NY() && Set_Progress(y); y++)
		for (x = 0; x < Get_NX(); x++)
		{
			int missing = 1;
			for (i = 0;  i < nGrids; i++)
			{
				//if (fabs(Grids[i]->asFloat(x, y) - Grids[0]->Get_NoData_Value()) < 0.0001)
				// this was not a very robust way of cheking nodata values, was it?
				if (Grids[i]->is_NoData(x,y))
				{
				//I think this works better, specially if you use a range of nodata values, not a single value
					missing = 0;
				}
			}
			MissingMap.Set_Value(x, y, missing);
		}
		
		pResult->Set_NoData_Value(Grids[0]->Get_NoData_Value());
		for (char c = 'a'; c<'a'+(char) nGrids; c++)
			
			vars[c - 'a'] = c;
		
		const char *formel  = Parameters("FORMUL")->asString();
		
		CSG_Formula Formel;
		
		Formel.Set_Formula(formel);
		
		int Pos;
		const char * Msg;
		if (Formel.Get_Error(&Pos, &Msg))
		{
			CSG_String	msg;

			msg.Printf(_TL("Error at character #%d of the function: \n%s\n"), Pos, formel);
			
			Message_Add(msg);
			
			msg.Printf("\n%s\n", Msg);
			
			Message_Add(msg);
			
			return false;
		}
		
		double *Grid_Vals= new double[nGrids];
		double val;
		
		for (y = 0; y < Get_NY() && Set_Progress(y); y++)
			for (x = 0; x < Get_NX(); x++)
			{
				for (i = 0; i < nGrids; i++)
				{
					Grid_Vals[i]=Grids[i]->asDouble(x,y);
				}
				
				val = Formel.Val(Grid_Vals, nGrids);	

				if (_finite(val) && MissingMap.asByte(x, y))
					pResult->Set_Value(x, y, val);
				else
					pResult->Set_Value(x, y, Grids[0]->Get_NoData_Value());
			}

		delete[] Grid_Vals;	
		return (true);
}

// MinGW ERROR:
// Grid_Calculator.cpp: In member function `virtual bool CGrid_Calculator::On_Execute()':
// Grid_Calculator.cpp:187: error: name lookup of `y' changed for new ISO `for' scoping
// Grid_Calculator.cpp:145: error:   using obsolete binding at `y'
