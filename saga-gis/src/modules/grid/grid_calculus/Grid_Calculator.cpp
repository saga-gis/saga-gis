
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


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <float.h>

#include "Grid_Calculator.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if defined(_SAGA_LINUX)
bool _finite(double val)
{
	return( true );
}
#endif


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Calculator::CGrid_Calculator(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Grid Calculator"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	CSG_String	s(_TW(
		"The Grid Calculator calculates a new grid based on existing grids and a mathematical formula. "
		"The grid variables are single characters a which correspond in alphabetical order to the grid list order "
		"('a' = first grid, 'b' = second grid, ...)\n"
		"Example with three grids: sin(a) * b + c\n\n"
		"The following operators are available for the formula definition:\n"
	));

	s	+= CSG_Formula::Get_Help_Operators();

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""), PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);
	
	Parameters.Add_String(
		NULL	, "FORMULA"	, _TL("Formula"),
		_TL(""),
		SG_T("(a - b) / (a + b)")
	);

	CSG_Parameter	*pNode	= Parameters.Add_String(
		NULL	, "NAME"	, _TL("Name"),
		_TL(""),
		_TL("Calculation")
	);

	Parameters.Add_Value(
		pNode	, "FNAME"	, _TL("Take Formula"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Calculator::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FORMULA"))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FNAME")) )
	{
		if( pParameters->Get_Parameter("FNAME")->asBool() )
		{
			pParameters->Get_Parameter("NAME")->Set_Value(CSG_String::Format(SG_T("%s [%s]"), _TL("Calculation"), pParameters->Get_Parameter("FORMULA")->asString()));
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator::On_Execute(void)
{
	double					Result, *Values;
	CSG_Formula				Formula;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Grid				*pResult;

	//-----------------------------------------------------
	pResult		= Parameters("RESULT")	->asGrid();
	pGrids		= Parameters("GRIDS")	->asGridList();

	//-----------------------------------------------------
	if( pGrids->Get_Count() <= 0 )
	{
		Error_Set(_TL("no grid in list"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Formula.Set_Formula(Parameters("FORMULA")->asString()) )
	{
		int			Position;
		CSG_String	Message, s;

		s	+= _TL("Error in formula");
		s	+= SG_T("\n") + Formula.Get_Formula();

		if( Formula.Get_Error(&Position, &Message) )
		{
			s	+= SG_T("\n") + Message;
			s	+= CSG_String::Format(SG_T("\n%s: %d"), _TL("Position") , Position);

			if( Position >= 0 && Position < (int)Formula.Get_Formula().Length() )
			{
				s	+= SG_T("\n")
					+  Formula.Get_Formula().Left(Position - 1) + SG_T("[")
					+  Formula.Get_Formula()[Position] + SG_T("]")
					+  Formula.Get_Formula().Right(Formula.Get_Formula().Length() - (Position + 1));
			}
		}

		Error_Set(s);

		return( false );
	}

	//-----------------------------------------------------
	pResult->Set_Name(Parameters("NAME")->asString());

	Values	= new double[pGrids->Get_Count()];

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(int i=0; i<pGrids->Get_Count() && !bNoData; i++)
			{
				if( pGrids->asGrid(i)->is_NoData(x, y) )
				{
					bNoData		= true;
				}
				else
				{
					Values[i]	= pGrids->asGrid(i)->asDouble(x, y);
				}
			}

			if( bNoData || _finite(Result = Formula.Get_Value(Values, pGrids->Get_Count())) == false )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				pResult->Set_Value(x, y, Result);
			}
		}
	}

	delete[](Values);

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
