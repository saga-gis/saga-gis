
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
//                 Olaf Conrad (C) 2023                  //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
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
CGrid_Calculator_Base::CGrid_Calculator_Base(void)
{
	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
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
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Calculator_Base::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("FORMULA")
	||	pParameter->Cmp_Identifier("FNAME"  ) )
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
	if( pParameter->Cmp_Identifier("XGRIDS") )
	{
		pParameters->Set_Enabled("RESAMPLING", pParameter->asList()->Get_Data_Count() > 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator_Base::Preprocess_Find(const CSG_String &Formula, const CSG_String &Function, CSG_String &Head, CSG_String &Argument, CSG_String &Tail)
{
	int pos  = Formula.Find(Function); if( pos < 0 ) { return( false ); }

	Head     = Formula.Left((size_t)pos); Tail = Formula.c_str() + pos + Function.Length();
	Argument = Tail.AfterFirst('(').BeforeFirst(')'); Argument.Trim_Both();
	Tail     = Tail.AfterFirst(')');

	return( true );
}

//---------------------------------------------------------
CSG_Data_Object * CGrid_Calculator_Base::Preprocess_Get_Object(const CSG_String &Argument)
{
	if( Argument.is_Empty() )
	{
		return( Parameters("RESULT")->asDataObject() );
	}

	CSG_Parameter_List *pList =
		Argument[0] == 'g' ? Parameters("GRIDS" )->asList() :
		Argument[0] == 'h' ? Parameters("XGRIDS")->asList() : NULL;

	if( pList )
	{
		CSG_String Index(Argument.c_str() + 1); int i;

		if( Index.asInt(i) && --i >= 0 && i < pList->Get_Data_Count() )
		{
			return( pList->Get_Data(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CGrid_Calculator_Base::Preprocess_Formula(CSG_String &Formula)
{
	Formula.Replace("\\n", "");
	Formula.Replace( "\n", "");
	Formula.Replace( "\t", "");

	//-----------------------------------------------------
	#define EXIT_ON_ERROR(func) { Error_Fmt("%s\n\n...%s(%s)", _TL("Invalid argument for function!"), SG_T(func), Argument.c_str()); return( false ); }

	#define PROCESS_SYSTEM_FUNCTION(name, func) { CSG_String Head, Argument, Tail; while( Preprocess_Find(Formula, name, Head, Argument, Tail) ) {\
		if( !Argument.is_Empty() ) { EXIT_ON_ERROR(name); }\
		Formula.Printf("%s(%f)%s", Head.c_str(), (double)func, Tail.c_str()); }\
	}

	#define PROCESS_OBJECT_FUNCTION(name, func) { CSG_String Head, Argument, Tail; while( Preprocess_Find(Formula, name, Head, Argument, Tail) ) {\
		CSG_Data_Object *pObject = Preprocess_Get_Object(Argument); if( !pObject ) { EXIT_ON_ERROR(name); }\
		double d = pObject->asGrid() ? pObject->asGrid()->func : pObject->asGrids()->func;\
		Formula.Printf("%s(%f)%s", Head.c_str(), d, Tail.c_str()); }\
	}

	PROCESS_SYSTEM_FUNCTION("ncols"   , Get_NX          ());
	PROCESS_SYSTEM_FUNCTION("nrows"   , Get_NY          ());
	PROCESS_OBJECT_FUNCTION("nodata"  , Get_NoData_Value());
	PROCESS_OBJECT_FUNCTION("cellsize", Get_Cellsize    ());
	PROCESS_OBJECT_FUNCTION("cellarea", Get_Cellarea    ());
	PROCESS_OBJECT_FUNCTION("xmin"    , Get_XMin        ());
	PROCESS_OBJECT_FUNCTION("xmax"    , Get_XMax        ());
	PROCESS_OBJECT_FUNCTION("xrange"  , Get_XRange      ());
	PROCESS_OBJECT_FUNCTION("ymin"    , Get_YMin        ());
	PROCESS_OBJECT_FUNCTION("ymax"    , Get_YMax        ());
	PROCESS_OBJECT_FUNCTION("yrange"  , Get_YRange      ());
	PROCESS_OBJECT_FUNCTION("zmin"    , Get_Min         ());
	PROCESS_OBJECT_FUNCTION("zmax"    , Get_Max         ());
	PROCESS_OBJECT_FUNCTION("zrange"  , Get_Range       ());
	PROCESS_OBJECT_FUNCTION("zmean"   , Get_Mean        ());
	PROCESS_OBJECT_FUNCTION("zstddev" , Get_StdDev      ());

	//-----------------------------------------------------
	if( Formula.Find(';') > 0 )
	{
		CSG_Strings Tokens = SG_String_Tokenize(Formula, ";"); Formula = Tokens[0];

		for(int i=1; i<Tokens.Get_Count(); i++)
		{
			CSG_String Key = Tokens[i].BeforeFirst('='); Key.Trim_Both();

			if( !Key.is_Empty() )
			{
				CSG_String Expression = Tokens[i].AfterFirst('='); Expression.Trim_Both();

				Formula.Replace(Key, "(" + Expression + ")");
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Calculator_Base::Initialize(int nGrids, int nGrids_X)
{
	const int nVars = 27;

	const SG_Char Vars[nVars] = SG_T("abcdefghijklmnopqrstuvwxyz");

	//-----------------------------------------------------
	CSG_String Formula(Parameters("FORMULA")->asString());

	if( !Preprocess_Formula(Formula) )
	{
		return( false );
	}

	int nFuncs = 0;

	if( (m_bPosition[0] = Formula.Find("col()"  ) >= 0) ) { nFuncs++; }
	if( (m_bPosition[1] = Formula.Find("row()"  ) >= 0) ) { nFuncs++; }
	if( (m_bPosition[2] = Formula.Find("xpos()" ) >= 0) ) { nFuncs++; }
	if( (m_bPosition[3] = Formula.Find("ypos()" ) >= 0) ) { nFuncs++; }

	//-----------------------------------------------------
	m_nValues = nGrids + nGrids_X + nFuncs;

	if( m_nValues > nVars )
	{
		Error_Set(_TL("too many input variables"));

		return( false );
	}

	//-----------------------------------------------------
	int i, n = m_nValues;

	if( m_bPosition[3] ) Formula.Replace("ypos()" , Vars[--n]);
	if( m_bPosition[2] ) Formula.Replace("xpos()" , Vars[--n]);
	if( m_bPosition[1] ) Formula.Replace("row()"  , Vars[--n]);
	if( m_bPosition[0] ) Formula.Replace("col()"  , Vars[--n]);

	for(i=nGrids_X; i>0 && n>0; i--)
	{
		Formula.Replace(CSG_String::Format("h%d", i), Vars[--n]);
	}

	for(i= nGrids; i>0 && n>0; i--)
	{
		Formula.Replace(CSG_String::Format("g%d", i), Vars[--n]);
	}

	//-----------------------------------------------------
	if( !m_Formula.Set_Formula(Formula) )
	{
		CSG_String Message;

		if( !m_Formula.Get_Error(Message) )
		{
			Message.Printf("%s: %s", _TL("error in formula"), Formula.c_str());
		}

		Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Used(m_Formula.Get_Used_Variables());

	int nUsed = (int)Used.Length() - nFuncs;

	if( nGrids + nGrids_X < nUsed )
	{
		Error_Fmt("%s (%d < %d)", _TL("The number of supplied grids is less than the number of variables in formula."),
			nGrids + nGrids_X, nUsed
		);

		return( false );
	}

	if( nGrids + nGrids_X > nUsed )
	{
		Message_Fmt("\n%s: %s (%d > %d)", _TL("Warning"), _TL("The number of supplied grids exceeds the number of variables in formula."),
			nGrids + nGrids_X, nUsed
		);
	}

	//-----------------------------------------------------
	m_bUseNoData = Parameters("USE_NODATA")->asBool();

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
	Set_Name		(_TL("Grid Calculator"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"The Grid Calculator calculates a new grid based on existing grids and a mathematical formula. "
		"The grid variables in the formula begin with the letter 'g' followed by a position index, "
		"which corresponds to the order of the grids in the input grid list "
		"(i.e.: g1, g2, g3, ... correspond to the first, second, third, ... grid in list). "
		"Grids from other systems than the default one can be addressed likewise using the letter 'h' "
		"(h1, h2, h3, ...), which correspond to the \'Grids from different Systems\' list.\n"
		"\n"
		"Example:\t sin(g1) * g2 + 2 * h1\n"
		"\n"
		"To make complex formulas look more intuitive you have the option to use shortcuts. Shortcuts are "
		"defined following the formula separated by semicolons as 'shortcut = expression'.\n"
		"\n"
		"Example:\t ifelse(lt(NDVI, 0.4), nodata(), NDVI); NDVI = (g1 - g2) / (g1 + g2)\n"
		"\n"
		"The following operators are available for the formula definition:\n"
	));

	static const CSG_String Operators[][2] =
	{
		{	"xpos(), ypos()"         , _TL("The coordinate (x/y) for the center of the currently processed cell"            ) },
		{	"col(), row()"           , _TL("The currently processed cell's column/row index"                                ) },
		{	"ncols(), nrows()"       , _TL("Number of the grid system's columns/rows"                                       ) },
		{	"nodata(), nodata(g)"    , _TL("No-data value of the resulting (empty) or requested grid (g = g1...gn, h1...hn)") },
		{	"cellsize(), cellsize(g)", _TL("Cell size of the resulting (empty) or requested grid (g = h1...hn)"             ) },
		{	"cellarea(), cellarea(g)", _TL("Cell area of the resulting (empty) or requested grid (g = h1...hn)"             ) },
		{	"xmin(), xmin(g)"        , _TL("Left bound of the resulting (empty) or requested grid (g = h1...hn)"            ) },
		{	"xmax(), xmax(g)"        , _TL("Right bound of the resulting (empty) or requested grid (g = h1...hn)"           ) },
		{	"xrange(), xrange(g)"    , _TL("Left to right range of the resulting (empty) or requested grid (g = h1...hn)"   ) },
		{	"ymin(), ymin(g)"        , _TL("Lower bound of the resulting (empty) or requested grid (g = h1...hn)"           ) },
		{	"ymax(), ymax(g)"        , _TL("Upper bound of the resulting (empty) or requested grid (g = h1...hn)"           ) },
		{	"yrange(), yrange(g)"    , _TL("Lower to upper range of the resulting (empty) or requested grid (g = h1...hn)"  ) },
		{	"zmin(g)"                , _TL("Minimum value of the requested grid (g = g1...gn, h1...hn)"                     ) },
		{	"zmax(g)"                , _TL("Maximum value of the requested grid (g = g1...gn, h1...hn)"                     ) },
		{	"zrange(g)"              , _TL("Value range of the requested grid (g = g1...gn, h1...hn)"                       ) },
		{	"zmean(g)"               , _TL("Mean value of the requested grid (g = g1...gn, h1...hn)"                        ) },
		{	"zstddev(g)"             , _TL("Standard deviation of the requested grid (g = g1...gn, h1...hn)"                ) },
		{	"", ""	}
	};

	Set_Description(Get_Description() + CSG_Formula::Get_Help_Operators(true, Operators));

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
	m_pGrids   = Parameters("GRIDS" )->asGridList();
	m_pGrids_X = Parameters("XGRIDS")->asGridList();

	//-----------------------------------------------------
	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	if( pResult->Get_Type() != Get_Result_Type() )
	{
		pResult->Create(Get_System(), Get_Result_Type());
	}

	pResult->Set_Name(Parameters("NAME")->asString());

	//-----------------------------------------------------
	if( !Initialize(m_pGrids->Get_Grid_Count(), m_pGrids_X->Get_Grid_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Result; CSG_Vector Values(m_nValues);

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
	TSG_Point p = Get_System().Get_Grid_to_World(x, y);

	if( m_pGrids_X->Get_Grid_Count() > 0 )
	{
		for(int i=0, j=m_pGrids->Get_Grid_Count(); i<m_pGrids_X->Get_Grid_Count(); i++, j++)
		{
			if( !m_pGrids_X->Get_Grid(i)->Get_Value(p, Values[j], m_Resampling, m_bUseNoData) )
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

		Values[i] = m_pGrids->Get_Grid(i)->asDouble(x, y);
	}

	int n = m_pGrids->Get_Grid_Count() + m_pGrids_X->Get_Grid_Count();

	if( m_bPosition[0] ) Values[n++] =   x; // col()
	if( m_bPosition[1] ) Values[n++] =   y; // row()
	if( m_bPosition[2] ) Values[n++] = p.x; // xpos()
	if( m_bPosition[3] ) Values[n++] = p.y; // ypos()

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
	Set_Name		(_TL("Grid Collection Calculator"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
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
		"To make complex formulas look more intuitive you have the option to use shortcuts. Shortcuts are "
		"defined following the formula separated by semicolons as 'shortcut = expression'.\n"
		"\n"
		"Example:\t ifelse(lt(NDVI, 0.4), nodata(), NDVI); NDVI = (g1 - g2) / (g1 + g2)\n"
		"\n"
		"The following operators are available for the formula definition:\n"
	));

	static const CSG_String Operators[][2] =
	{
		{	"xpos(), ypos()"         , _TL("The coordinate (x/y) for the center of the currently processed cell"                       ) },
		{	"col(), row()"           , _TL("The currently processed cell's column/row index"                                           ) },
		{	"ncols(), nrows()"       , _TL("Number of the grid system's columns/rows"                                                  ) },
		{	"nodata(), nodata(g)"    , _TL("No-data value of the resulting (empty) or requested grid collection (g = g1...gn, h1...hn)") },
		{	"cellsize(), cellsize(g)", _TL("Cell size of the resulting (empty) or requested grid collection (g = h1...hn)"             ) },
		{	"cellarea(), cellarea(g)", _TL("Cell area of the resulting (empty) or requested grid collection (g = h1...hn)"             ) },
		{	"xmin(), xmin(g)"        , _TL("Left bound of the resulting (empty) or requested grid collection (g = h1...hn)"            ) },
		{	"xmax(), xmax(g)"        , _TL("Right bound of the resulting (empty) or requested grid collection (g = h1...hn)"           ) },
		{	"xrange(), xrange(g)"    , _TL("Left to right range of the resulting (empty) or requested grid collection (g = h1...hn)"   ) },
		{	"ymin(), ymin(g)"        , _TL("Lower bound of the resulting (empty) or requested grid collection (g = h1...hn)"           ) },
		{	"ymax(), ymax(g)"        , _TL("Upper bound of the resulting (empty) or requested grid collection (g = h1...hn)"           ) },
		{	"yrange(), yrange(g)"    , _TL("Lower to upper range of the resulting (empty) or requested grid collection (g = h1...hn)"  ) },
		{	"zmin(g)"                , _TL("Minimum value of the requested grid collection (g = g1...gn, h1...hn)"                     ) },
		{	"zmax(g)"                , _TL("Maximum value of the requested grid collection (g = g1...gn, h1...hn)"                     ) },
		{	"zrange(g)"              , _TL("Value range of the requested grid collection (g = g1...gn, h1...hn)"                       ) },
		{	"zmean(g)"               , _TL("Mean value of the requested grid collection (g = g1...gn, h1...hn)"                        ) },
		{	"zstddev(g)"             , _TL("Standard deviation of the requested grid collection (g = g1...gn, h1...hn)"                ) },
		{	"", ""	}
	};

	Set_Description(Get_Description() + CSG_Formula::Get_Help_Operators(true, Operators));

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
bool CGrids_Calculator::Preprocess_Formula(CSG_String &Formula)
{
	return( true );
}

//---------------------------------------------------------
bool CGrids_Calculator::On_Execute(void)
{
	m_pGrids   = Parameters("GRIDS" )->asGridsList();
	m_pGrids_X = Parameters("XGRIDS")->asGridsList();

	int i, nz = m_pGrids->Get_Grids(0)->Get_NZ();

	for(i=1; i<m_pGrids->Get_Item_Count(); i++)
	{
		CSG_Grids *pGrids = m_pGrids->Get_Grids(i);

		if( pGrids->Get_NZ() != nz )
		{
			Error_Fmt("%s [%d, %s]", _TL("incompatible number of grid layers"), pGrids->Get_NZ(), pGrids->Get_Name());

			return( false );
		}
	}

	//-----------------------------------------------------
	CSG_Grids *pResult = Parameters("RESULT")->asGrids();

	if( pResult->Get_Type() != Get_Result_Type() || pResult->Get_NZ() != nz )
	{
		CSG_Grids *pGrids = m_pGrids->Get_Grids(0);

		pResult->Create(Get_System(), pGrids->Get_Attributes(), pGrids->Get_Z_Attribute(), Get_Result_Type(), true);
	}

	pResult->Set_Name(Parameters("NAME")->asString());

	//-----------------------------------------------------
	if( !Initialize(m_pGrids->Get_Item_Count(), m_pGrids_X->Get_Item_Count()) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Result; CSG_Vector Values(m_nValues);

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
	TSG_Point p = Get_System().Get_Grid_to_World(x, y);

	if( m_pGrids_X->Get_Item_Count() > 0 )
	{
		CSG_Grids *pGrids = m_pGrids->Get_Grids(0);

		double pz = pGrids->Get_Z(z);

		for(int i=0, j=m_pGrids->Get_Item_Count(); i<m_pGrids_X->Get_Item_Count(); i++, j++)
		{
			if( !m_pGrids_X->Get_Grids(i)->Get_Value(p.x, p.y, pz, Values[j], m_Resampling) )
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

		Values[i] = m_pGrids->Get_Grids(i)->asDouble(x, y, z);
	}

	int n = m_pGrids->Get_Item_Count() + m_pGrids_X->Get_Item_Count();

	if( m_bPosition[0] ) Values[n++] =   x; // col()
	if( m_bPosition[1] ) Values[n++] =   y; // row()
	if( m_bPosition[2] ) Values[n++] = p.x; // xpos()
	if( m_bPosition[3] ) Values[n++] = p.y; // ypos()

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
