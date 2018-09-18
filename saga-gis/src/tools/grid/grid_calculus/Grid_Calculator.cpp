/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "Grid_Calculator.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_LINUX
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
double CGrid_Calculator_Base::m_NoData_Value	= -99999;

//---------------------------------------------------------
CGrid_Calculator_Base::CGrid_Calculator_Base(void)
{
	//-----------------------------------------------------
	CSG_String	s(_TW(
		"The Grid Calculator calculates a new grid based on existing grids and a mathematical formula. "
		"The grid variables in the formula begin with the letter 'g' followed by a position index, "
		"which corresponds to the order of the grids in the input grid list "
		"(i.e.: g1, g2, g3, ... correspond to the first, second, third, ... grid in list). "
		"Grids from other systems than the default one can be addressed likewise using the letter 'h' "
		"(h1, h2, h3, ...), which correspond to the \'Grids from different Systems\' list.\n"
		"\n"
		"Example:\t sin(g1) * g2 + 2 * h1\n"
		"\n"
		"The following operators are available for the formula definition:\n"
	));

	const CSG_String	Operators[5][2]	=
	{
		{	"xpos(), ypos()"  , _TL("Get the x/y coordinates for the current cell")	},
		{	"col(), row()"    , _TL("Get the current cell's column/row index"     )	},
		{	"ncols(), nrows()", _TL("Get the number of columns/rows"              )	},
		{	"nodata()"        , _TL("Returns resulting grid's no-data value"      )	},
		{	"", ""	}
	};

	s	+= CSG_Formula::Get_Help_Operators(true, Operators);

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);
	
	Parameters.Add_String("",
		"FORMULA"	, _TL("Formula"),
		_TL(""),
		"(g1 - g2) / (g1 + g2)"
	);

	Parameters.Add_String("",
		"NAME"		, _TL("Name"),
		_TL(""),
		_TL("Calculation")
	);

	Parameters.Add_Bool("NAME",
		"FNAME"		, _TL("Take Formula"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"USE_NODATA", _TL("Use No-Data"),
		_TL("Check this in order to include no-data cells in the calculation."),
		false
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Data Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s",
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

	//-----------------------------------------------------
	m_Formula.Add_Function(SG_T("nodata"), (TSG_PFNC_Formula_1)CGrid_Calculator_Base::Get_NoData_Value, 0, 0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Calculator_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "FORMULA")
	||	!SG_STR_CMP(pParameter->Get_Identifier(), "FNAME"  ) )
	{
		if( (*pParameters)("FNAME")->asBool() )
		{
			pParameters->Set_Parameter("NAME", CSG_String::Format("%s [%s]", _TL("Calculation"), (*pParameters)("FORMULA")->asString()));
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Calculator_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "XGRIDS") )
	{
		pParameters->Set_Enabled("RESAMPLING", pParameter->asList()->Get_Data_Count() > 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator_Base::Initialize(int nGrids, int nXGrids)
{
	//-----------------------------------------------------
	const int		nVars		= 27;

	const SG_Char	Vars[nVars]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	//-----------------------------------------------------
	CSG_String	Formula(Parameters("FORMULA")->asString());

	m_nValues	= nGrids + nXGrids;

	if( Formula.Find("col()"  ) ) { m_bPosition[0] = true; m_nValues++; } else { m_bPosition[0] = false; }
	if( Formula.Find("row()"  ) ) { m_bPosition[1] = true; m_nValues++; } else { m_bPosition[1] = false; }
	if( Formula.Find("xpos()" ) ) { m_bPosition[2] = true; m_nValues++; } else { m_bPosition[2] = false; }
	if( Formula.Find("ypos()" ) ) { m_bPosition[3] = true; m_nValues++; } else { m_bPosition[3] = false; }
	if( Formula.Find("ncols()") ) { m_bPosition[4] = true; m_nValues++; } else { m_bPosition[4] = false; }
	if( Formula.Find("nrows()") ) { m_bPosition[5] = true; m_nValues++; } else { m_bPosition[5] = false; }

	//-----------------------------------------------------
	if( m_nValues > nVars )
	{
		Error_Set(_TL("too many input variables"));

		return( false );
	}

	//-----------------------------------------------------
	int		i, n	= m_nValues;

	if( m_bPosition[5] ) Formula.Replace("nrows()", Vars[--n]);
	if( m_bPosition[4] ) Formula.Replace("ncols()", Vars[--n]);
	if( m_bPosition[3] ) Formula.Replace("ypos()" , Vars[--n]);
	if( m_bPosition[2] ) Formula.Replace("xpos()" , Vars[--n]);
	if( m_bPosition[1] ) Formula.Replace("row()"  , Vars[--n]);
	if( m_bPosition[0] ) Formula.Replace("col()"  , Vars[--n]);

	for(i=nXGrids; i>0 && n>0; i--)
	{
		Formula.Replace(CSG_String::Format("h%d", i), Vars[--n]);
	}

	for(i=nGrids; i>0 && n>0; i--)
	{
		Formula.Replace(CSG_String::Format("g%d", i), Vars[--n]);
	}

	//-----------------------------------------------------
	if( !m_Formula.Set_Formula(Formula) )
	{
		CSG_String	Message;

		if( !m_Formula.Get_Error(Message) )
		{
			Message.Printf("%s: %s", _TL("error in formula"), Formula.c_str());
		}

		Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Used(m_Formula.Get_Used_Variables());

	if( m_nValues < (int)Used.Length() )
	{
		Error_Fmt("%s (%d < %d)", _TL("The number of supplied grids is less than the number of variables in formula."), m_nValues, Used.Length());

		return( false );
	}

	if( m_nValues > (int)Used.Length() )
	{
		Message_Add(CSG_String::Format("%s: %s (%d > %d)", _TL("Warning"), _TL("The number of supplied grids exceeds the number of variables in formula."), m_nValues, Used.Length()));
	}

	//-----------------------------------------------------
	m_bUseNoData	= Parameters("USE_NODATA")->asBool();

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: m_Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: m_Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: m_Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: m_Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
TSG_Data_Type CGrid_Calculator_Base::Get_Result_Type(void)
{
	switch( Parameters("TYPE")->asInt() )
	{
	default: return( SG_DATATYPE_Float  );
	case  0: return( SG_DATATYPE_Bit    );
	case  1: return( SG_DATATYPE_Byte   );
	case  2: return( SG_DATATYPE_Char   );
	case  3: return( SG_DATATYPE_Word   );
	case  4: return( SG_DATATYPE_Short  );
	case  5: return( SG_DATATYPE_DWord  );
	case  6: return( SG_DATATYPE_Int    );
	case  7: return( SG_DATATYPE_Float  );
	case  8: return( SG_DATATYPE_Double );
	}
}

//---------------------------------------------------------
inline bool CGrid_Calculator_Base::Get_Result(const CSG_Vector &Values, double &Result)
{
	return( _finite(Result = m_Formula.Get_Value(Values)) != 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Calculator::CGrid_Calculator(void)
{
	Set_Name	(_TL("Grid Calculator"));

	Set_Author	("O.Conrad (c) 2017, A.Ringeler (c) 2003");

	CSG_String	s(_TW(
		"The Grid Calculator calculates a new grid based on existing grids and a mathematical formula. "
		"The grid variables in the formula begin with the letter 'g' followed by a position index, "
		"which corresponds to the order of the grids in the input grid list "
		"(i.e.: g1, g2, g3, ... correspond to the first, second, third, ... grid in list). "
		"Grids from other systems than the default one can be addressed likewise using the letter 'h' "
		"(h1, h2, h3, ...), which correspond to the \'Grids from different Systems\' list.\n"
		"\n"
		"Example:\t sin(g1) * g2 + 2 * h1\n"
		"\n"
		"The following operators are available for the formula definition:\n"
	));

	const CSG_String	Operators[5][2]	=
	{
		{	"xpos(), ypos()"  , _TL("Get the x/y coordinates for the current cell")	},
		{	"col(), row()"    , _TL("Get the current cell's column/row index"     )	},
		{	"ncols(), nrows()", _TL("Get the number of columns/rows"              )	},
		{	"nodata()"        , _TL("Returns resulting grid's no-data value"      )	},
		{	"", ""	}
	};

	s	+= CSG_Formula::Get_Help_Operators(true, Operators);

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL("in the formula these grids are addressed in order of the list as 'g1, g2, g3, ...'"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_List("",
		"XGRIDS"	, _TL("Grids from different Systems"),
		_TL("in the formula these grids are addressed in order of the list as 'h1, h2, h3, ...'"),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS" )->asGridList();
	m_pXGrids	= Parameters("XGRIDS")->asGridList();

	if( !Initialize(m_pGrids->Get_Grid_Count(), m_pXGrids->Get_Grid_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pResult	= Parameters("RESULT")->asGrid();

	if( pResult->Get_Type() != Get_Result_Type() )
	{
		pResult->Create(*Get_System(), Get_Result_Type());
	}

	pResult->Set_Name(Parameters("NAME")->asString());

	m_NoData_Value	= pResult->Get_NoData_Value();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Result;	CSG_Vector	Values(m_nValues);

			if( Get_Values(x, y, Values) && Get_Result(Values, Result) )
			{
				pResult->Set_Value(x, y, Result);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator::Get_Values(int x, int y, CSG_Vector &Values)
{
	TSG_Point	p	= Get_System()->Get_Grid_to_World(x, y);

	if( m_pXGrids->Get_Grid_Count() > 0 )
	{
		for(int i=0, j=m_pGrids->Get_Grid_Count(); i<m_pXGrids->Get_Grid_Count(); i++, j++)
		{
			if( !m_pXGrids->Get_Grid(i)->Get_Value(p, Values[j], m_Resampling, m_bUseNoData) )
			{
				return( false );
			}
		}
	}

	for(int i=0; i<m_pGrids->Get_Grid_Count(); i++)
	{
		if( !m_bUseNoData && m_pGrids->Get_Grid(i)->is_NoData(x, y) )
		{
			return( false );
		}

		Values[i]	= m_pGrids->Get_Grid(i)->asDouble(x, y);
	}

	int	n	= m_pGrids->Get_Grid_Count() + m_pXGrids->Get_Grid_Count();

	if( m_bPosition[0] ) Values[n++] =      x  ;	// col()
	if( m_bPosition[1] ) Values[n++] =      y  ;	// row()
	if( m_bPosition[2] ) Values[n++] =    p.x  ;	// xpos()
	if( m_bPosition[3] ) Values[n++] =    p.y  ;	// ypos()
	if( m_bPosition[4] ) Values[n++] = Get_NX();	// ncols()
	if( m_bPosition[5] ) Values[n++] = Get_NY();	// nrows()

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Calculator::CGrids_Calculator(void)
{
	Set_Name	(_TL("Grid Collection Calculator"));

	Set_Author	("O.Conrad (c) 2018");

	CSG_String	s(_TW(
		"The Grid Collection Calculator creates a new grid collection combining existing ones using the given formula. "
		"It is assumed that all input grid collections have the same number of grid layers. "
		"The variables in the formula begin with the letter 'g' followed by a position index, "
		"which corresponds to the order of the grid collections in the input grid collection list "
		"(i.e.: g1, g2, g3, ... correspond to the first, second, third, ... grid collection in list). "
		"Grid collections from other systems than the default one can be addressed likewise using the letter 'h' "
		"(h1, h2, h3, ...), which correspond to the \'Grid collections from different Systems\' list.\n"
		"\n"
		"Example:\t sin(g1) * g2 + 2 * h1\n"
		"\n"
		"The following operators are available for the formula definition:\n"
	));

	const CSG_String	Operators[5][2]	=
	{
		{	"xpos(), ypos()"  , _TL("Get the x/y coordinates for the current cell")	},
		{	"col(), row()"    , _TL("Get the current cell's column/row index"     )	},
		{	"ncols(), nrows()", _TL("Get the number of columns/rows"              )	},
		{	"nodata()"        , _TL("Returns resulting grid's no-data value"      )	},
		{	"", ""	}
	};

	s	+= CSG_Formula::Get_Help_Operators(true, Operators);

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Grids_List("",
		"GRIDS"		, _TL("Grid Collections"),
		_TL("in the formula these grid collections are addressed in order of the list as 'g1, g2, g3, ...'"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grids_List("",
		"XGRIDS"	, _TL("Grid Collections from different Systems"),
		_TL("in the formula these grid collections are addressed in order of the list as 'h1, h2, h3, ...'"),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Grids("",
		"RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Calculator::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS" )->asGridsList();
	m_pXGrids	= Parameters("XGRIDS")->asGridsList();

	int	i, nz	= m_pGrids->Get_Grids(0)->Get_NZ();

	for(i=1; i<m_pGrids->Get_Item_Count(); i++)
	{
		CSG_Grids	*pGrids	= m_pGrids->Get_Grids(i);

		if( pGrids->Get_NZ() != nz )
		{
			Error_Fmt("%s [%d, %s]", _TL("incompatible number of grid layers"), pGrids->Get_NZ(), pGrids->Get_Name());

			return( false );
		}
	}

	if( !Initialize(m_pGrids->Get_Item_Count(), m_pXGrids->Get_Item_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grids	*pResult	= Parameters("RESULT")->asGrids();

	if( pResult->Get_Type() != Get_Result_Type() || pResult->Get_NZ() != nz )
	{
		CSG_Grids	*pGrids	= m_pGrids->Get_Grids(0);

		pResult->Create(*Get_System(), pGrids->Get_Attributes(), pGrids->Get_Z_Attribute(), Get_Result_Type(), true);
	}

	pResult->Set_Name(Parameters("NAME")->asString());

	m_NoData_Value	= pResult->Get_NoData_Value();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Result;	CSG_Vector	Values(m_nValues);

			for(int z=0; z<pResult->Get_NZ(); z++)
			{
				if( Get_Values(x, y, z, Values) && Get_Result(Values, Result) )
				{
					pResult->Set_Value(x, y, z, Result);
				}
				else
				{
					pResult->Set_NoData(x, y, z);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Calculator::Get_Values(int x, int y, int z, CSG_Vector &Values)
{
	TSG_Point	p	= Get_System()->Get_Grid_to_World(x, y);

	if( m_pXGrids->Get_Item_Count() > 0 )
	{
		CSG_Grids	*pGrids	= m_pGrids->Get_Grids(0);

		double	pz	= pGrids->Get_Z(z);

		for(int i=0, j=m_pGrids->Get_Item_Count(); i<m_pXGrids->Get_Item_Count(); i++, j++)
		{
			if( !m_pXGrids->Get_Grids(i)->Get_Value(p.x, p.y, pz, Values[j], m_Resampling) )
			{
				return( false );
			}
		}
	}

	for(int i=0; i<m_pGrids->Get_Item_Count(); i++)
	{
		if( !m_bUseNoData && m_pGrids->Get_Grids(i)->is_NoData(x, y, z) )
		{
			return( false );
		}

		Values[i]	= m_pGrids->Get_Grids(i)->asDouble(x, y, z);
	}

	int	n	= m_pGrids->Get_Item_Count() + m_pXGrids->Get_Item_Count();

	if( m_bPosition[0] ) Values[n++] =      x  ;	// col()
	if( m_bPosition[1] ) Values[n++] =      y  ;	// row()
	if( m_bPosition[2] ) Values[n++] =    p.x  ;	// xpos()
	if( m_bPosition[3] ) Values[n++] =    p.y  ;	// ypos()
	if( m_bPosition[4] ) Values[n++] = Get_NX();	// ncols()
	if( m_bPosition[5] ) Values[n++] = Get_NY();	// nrows()

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
