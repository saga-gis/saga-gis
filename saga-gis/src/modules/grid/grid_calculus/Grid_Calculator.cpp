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
		"The grid variables are single characters which correspond in alphabetical order to the grid list order "
		"('a' = first grid, 'b' = second grid, ...)\n"
		"Alternatively you can address with letter 'g' followed by position index (g1, g2, g3, ...). "
		"Grids from other systems are addressed similarly, just using letter 'h' (h1, h2, h3, ...)\n"
		"Examples:\n"
		"using letters: sin(a) * b + c\n"
		"the same using indices: sin(g1) * g2 + g3\n\n"
		"The following operators are available for the formula definition:\n"
	));

	s	+= CSG_Formula::Get_Help_Operators();

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL("in formula these grids are addressed in order of the list as 'g1, g2, g3, ...'"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "XGRIDS"	, _TL("Grids from different Systems"),
		_TL("in formula these grids are addressed in order of the list as 'h1, h2, h3, ...'"),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);
	
	Parameters.Add_String(
		NULL	, "FORMULA"	, _TL("Formula"),
		_TL(""),
		SG_T("(g1 - g2) / (g1 + g2)")
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

	Parameters.Add_Value(
		NULL	, "USE_NODATA"	, _TL("Use NoData"),
		_TL("Check this in order to include NoData cells in the calculation."),
		PARAMETER_TYPE_Bool, false
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
	int						x, y, i, j;
	double					Result, *Values;
	TSG_Point				p;
	CSG_Formula				Formula;
	CSG_Parameter_Grid_List	*pGrids, *pXGrids;
	CSG_Grid				*pResult;
	bool					bUseNoData;

	//-----------------------------------------------------
	pResult		= Parameters("RESULT")		->asGrid();
	pGrids		= Parameters("GRIDS")		->asGridList();
	pXGrids		= Parameters("XGRIDS")		->asGridList();
	bUseNoData	= Parameters("USE_NODATA")	->asBool();

	//-----------------------------------------------------
	if( pGrids->Get_Count() <= 0 )
	{
		Error_Set(_TL("no grid in list"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Formula.Set_Formula(Get_Formula(Parameters("FORMULA")->asString(), pGrids->Get_Count(), pXGrids->Get_Count())) )
	{
		CSG_String	Message;

		Formula.Get_Error(Message);

		Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	pResult->Set_Name(Parameters("NAME")->asString());

	Values	= new double[pGrids->Get_Count() + pXGrids->Get_Count()];

	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			bool	bNoData	= false;

			for(i=0; i<pGrids->Get_Count() && !bNoData; i++)
			{
				if( !bUseNoData && pGrids->asGrid(i)->is_NoData(x, y) )
				{
					bNoData		= true;
				}
				else
				{
					Values[i]	= pGrids->asGrid(i)->asDouble(x, y);
				}
			}

			for(i=0, j=pGrids->Get_Count(); i<pXGrids->Get_Count() && !bNoData; i++, j++)
			{
				if( !pXGrids->asGrid(i)->Get_Value(p, Values[j]) )
				{
					bNoData		= true;
				}
			}

			if( bNoData || _finite(Result = Formula.Get_Value(Values, pGrids->Get_Count() + pXGrids->Get_Count())) == false )
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
CSG_String CGrid_Calculator::Get_Formula(CSG_String sFormula, int nGrids, int nXGrids)
{
	const SG_Char	vars[27]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	int		i, n = 0;

	for(i=0; n<27 && i<nGrids; i++, n++)
	{
		sFormula.Replace(CSG_String::Format(SG_T("g%d"), i + 1), CSG_String(vars[n]));
	}

	for(i=0; n<27 && i<nXGrids; i++, n++)
	{
		sFormula.Replace(CSG_String::Format(SG_T("h%d"), i + 1), CSG_String(vars[n]));
	}

	return( sFormula );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
