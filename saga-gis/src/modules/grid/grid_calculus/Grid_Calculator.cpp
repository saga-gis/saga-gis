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
	Set_Name	(_TL("Grid Calculator"));

	Set_Author	(_TL("A.Ringeler (c) 2003"));

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

	s	+= _TW(
		"xpos(), ypos() - get the x/y coordinates of the current cell\n"
		"row(), col() - get the current cell's column/row index\n"
	);

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL("in the formula these grids are addressed in order of the list as 'g1, g2, g3, ...'"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_List(
		NULL	, "XGRIDS"	, _TL("Grids from different Systems"),
		_TL("in the formula these grids are addressed in order of the list as 'h1, h2, h3, ...'"),
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

	Parameters.Add_Choice(
		NULL	, "TYPE"	, _TL("Data Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			SG_Data_Type_Get_Name(SG_DATATYPE_Bit   ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Char  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Word  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Short ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_DWord ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Int   ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Float ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Double).c_str()
		), 7
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
	bool					bUseNoData;
	CSG_Formula				Formula;
	CSG_Parameter_Grid_List	*pGrids, *pXGrids;
	CSG_Grid				*pResult;

	//-----------------------------------------------------
	pResult		= Parameters("RESULT"    )->asGrid();
	pGrids		= Parameters("GRIDS"     )->asGridList();
	pXGrids		= Parameters("XGRIDS"    )->asGridList();
	bUseNoData	= Parameters("USE_NODATA")->asBool();

	//-----------------------------------------------------
	if( !Get_Formula(Formula, Parameters("FORMULA")->asString(), pGrids->Get_Count(), pXGrids->Get_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Data_Type	Type;

	switch( Parameters("TYPE")->asInt() )
	{
	default:	Type	= SG_DATATYPE_Float ;	break;
	case  0:	Type	= SG_DATATYPE_Bit   ;	break;
	case  1:	Type	= SG_DATATYPE_Byte  ;	break;
	case  2:	Type	= SG_DATATYPE_Char  ;	break;
	case  3:	Type	= SG_DATATYPE_Word  ;	break;
	case  4:	Type	= SG_DATATYPE_Short ;	break;
	case  5:	Type	= SG_DATATYPE_DWord ;	break;
	case  6:	Type	= SG_DATATYPE_Int   ;	break;
	case  7:	Type	= SG_DATATYPE_Float ;	break;
	case  8:	Type	= SG_DATATYPE_Double;	break;
	}

	if( Type != pResult->Get_Type() )
	{
		pResult->Create(*Get_System(), Type);
	}

	pResult->Set_Name(Parameters("NAME")->asString());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	py	= Get_YMin() + y * Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool		bNoData	= false;
			int			i, n;
			double		Result, px	= Get_XMin() + x * Get_Cellsize();
			CSG_Vector	Values(pGrids->Get_Count() + pXGrids->Get_Count() + 2);

			for(i=0, n=0; !bNoData && i<pGrids->Get_Count(); i++)
			{
				if( !bUseNoData && pGrids->asGrid(i)->is_NoData(x, y) )
				{
					bNoData		= true;
				}
				else
				{
					Values[n++]	= pGrids->asGrid(i)->asDouble(x, y);
				}
			}

			for(i=0, n=pGrids->Get_Count(); !bNoData && i<pXGrids->Get_Count(); i++)
			{
				if( !pXGrids->asGrid(i)->Get_Value(px, py, Values[n++]) )
				{
					bNoData		= true;
				}
			}

			if( !bNoData )
			{
				n	= pGrids->Get_Count() + pXGrids->Get_Count();

				Values[n++]	= y;	// col()
				Values[n++]	= x;	// row()
				Values[n++]	= py;	// ypos()
				Values[n++]	= px;	// xpos()

				bNoData	= !_finite(Result = Formula.Get_Value(Values));
			}

			if( bNoData )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				pResult->Set_Value(x, y, Result);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator::Get_Formula(CSG_Formula &Formula, CSG_String sFormula, int nGrids, int nXGrids)
{
	//-----------------------------------------------------
	const int		nVars		= 27;

	const SG_Char	Vars[nVars]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	//-----------------------------------------------------
	if( nGrids + nXGrids + 4 > nVars )
	{
		Error_Set(_TL("too many input grids"));

		return( false );
	}

	//-----------------------------------------------------
	int		i, n = nGrids + nXGrids + 4 - 1;

	sFormula.Replace("xpos()", Vars[n--]);
	sFormula.Replace("ypos()", Vars[n--]);
	sFormula.Replace("row()" , Vars[n--]);
	sFormula.Replace("col()" , Vars[n--]);

	for(i=nXGrids; i>0; i--, n--)
	{
		sFormula.Replace(CSG_String::Format(SG_T("h%d"), i), Vars[n]);
	}

	for(i=nGrids; i>0; i--, n--)
	{
		sFormula.Replace(CSG_String::Format(SG_T("g%d"), i), Vars[n]);
	}

	//-----------------------------------------------------
	if( !Formula.Set_Formula(sFormula) )
	{
		CSG_String	Message;

		if( !Formula.Get_Error(Message) )
		{
			Message.Printf(SG_T("%s: %s"), _TL("error in formula"), sFormula.c_str());
		}

		Error_Set(Message);

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
