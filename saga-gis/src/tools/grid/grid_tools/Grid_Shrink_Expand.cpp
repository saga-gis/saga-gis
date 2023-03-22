
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Shrink_Expand.cpp                 //
//                                                       //
//                 Copyright (C) 2011 by                 //
//            Volker Wichmann and Olaf Conrad            //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Shrink_Expand.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	EXPAND_MIN = 0,
	EXPAND_MAX,
	EXPAND_MEAN,
	EXPAND_MAJORITY
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Shrink_Expand::CGrid_Shrink_Expand(void)
{
	Set_Name		(_TL("Shrink and Expand"));

	Set_Author		("V.Wichmann & O.Conrad (c) 2011");

	Set_Description	(_TW(
		"With this tool you can shrink and/or expand regions with valid data by a certain distance "
		"defined by the (kernel) radius. Shrinking just invalidates all (valid) data cells found within "
		"the given distance to no-data cells, while expansion replaces no-data cells with new values "
		"based on the evaluation of all (valid) data cells found within the neighbourhood as defined "
		"by the kernel. Both operations can be combined.\n\n"
		"The method for the value expansion can be chosen as minimum, maximum, mean or majority value. "
		"The neighbourhood can be evaluated either at once or in a stepwise iterative way. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"OPERATION"	, _TL("Operation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("shrink"           ),
			_TL("expand"           ),
			_TL("shrink and expand"),
			_TL("expand and shrink")
		), 3
	);

	Parameters.Add_Choice("",
		"CIRCLE"	, _TL("Search Mode"),
		_TL("Choose the shape of the kernel."),
		CSG_String::Format("%s|%s",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Int("",
		"RADIUS"	, _TL("Radius"),
		_TL("The kernel radius [cells]."),
		1, 1, true
	);

	Parameters.Add_Choice("",
		"EXPAND"	, _TL("Method"),
		_TL("Choose how to fill no-data cells."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("minimum" ),
			_TL("maximum" ),
			_TL("mean"    ),
			_TL("majority")
		), 3
	);

	Parameters.Add_Bool("EXPAND",
		"KEEP_TYPE"	, _TL("Preserve Data Type"),
		_TL("If false, mean value expansion results will be stored with floating point precision."),
		true
	);

	Parameters.Add_Bool("",
		"ITERATIVE"	, _TL("Iterative Expansion"),
		_TL("If false, the neighbourhood for expansion is evaluated in one step, else expansion is done stepwise with a one cell radius for each iteration until desired kernel radius is reached."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Shrink_Expand::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("OPERATION") )
	{
		pParameters->Set_Enabled("EXPAND"   , pParameter->asInt() > 0);
		pParameters->Set_Enabled("ITERATIVE", pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier("EXPAND") )
	{
		pParameters->Set_Enabled("KEEP_TYPE", pParameter->asInt() == 2); // mean?!
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::On_Execute(void)
{
	if( !Set_Kernel() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pInput  = Parameters("INPUT" )->asGrid(), Input;
	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	TSG_Data_Type Type = pInput->Get_Type();

	if( Parameters("OPERATION")->asInt ()  > 0     // expands
	&&  Parameters("EXPAND"   )->asInt () == 2     // mean
	&&  Parameters("KEEP_TYPE")->asBool() == false // keep data type?
	&&  Type != SG_DATATYPE_Float && Type != SG_DATATYPE_Double )
	{
		Type = SG_DATATYPE_Float;
	}

	if( !pResult || pResult == pInput )
	{
		Input.Create(*pInput); pResult = pInput; pInput = &Input;
	}

	//-----------------------------------------------------
	if( pResult->Get_Type() != Type )
	{
		pResult->Create(Get_System(), Type);
		pResult->Set_Name(pInput->Get_Name());
	}

	//-----------------------------------------------------
	bool bResult = Do_Operation(pInput, pResult);

	//-----------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);
	}
	else switch( Parameters("OPERATION")->asInt() )
	{
	case  0: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Shrink"           )); break;
	case  1: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Expand"           )); break;
	case  2: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Shrink and Expand")); break;
	default: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Expand and Shrink")); break;
	}

	Set_Kernel(false);

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Set_Kernel(bool bInitialize)
{
	if( !bInitialize )
	{
		return( m_Kernel.Destroy() );
	}

	if( !m_Kernel.Set_Radius(Parameters("RADIUS")->asInt(), Parameters("CIRCLE")->asInt() == 0) )
	{
		Error_Set(_TL("could not initialize search kernel"));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Do_Operation(CSG_Grid *pInput, CSG_Grid *pResult)
{
	switch( Parameters("OPERATION")->asInt() )
	{
	case  0: // shrink
		{ return( Do_Shrink(pInput, pResult) ); }

	case  1: // expand
		{ return( Do_Expand(pInput, pResult) ); }

	case  2: // shrink and expand
		{ CSG_Grid TMP(pResult); return( Do_Shrink(pInput, &TMP) && Do_Expand(&TMP, pResult) ); }

	default: // expand and shrink
		{ CSG_Grid TMP(pResult); return( Do_Expand(pInput, &TMP) && Do_Shrink(&TMP, pResult) ); }
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Do_Shrink(CSG_Grid *pInput, CSG_Grid *pResult)
{
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			bool bShrink = pInput->is_NoData(x, y);

			for(int i=0; !bShrink && i<m_Kernel.Get_Count(); i++)
			{
				int ix = m_Kernel.Get_X(i, x);
				int iy = m_Kernel.Get_Y(i, y);

				if( is_InGrid(ix, iy) && pInput->is_NoData(ix, iy) )
				{
					bShrink = true;
				}
			}

			if( bShrink )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				pResult->Set_Value(x, y, pInput->asDouble(x, y));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Do_Expand(CSG_Grid *pInput, CSG_Grid *pResult)
{
	if( m_Kernel.Get_Radius() == 1 || !Parameters("ITERATIVE")->asBool() )
	{
		return( Do_Expand(pInput, pResult, m_Kernel) );
	}

	CSG_Grid_Cell_Addressor Kernel; Kernel.Set_Radius(1, Parameters("CIRCLE")->asInt() == 0);

	Do_Expand(pInput, pResult, Kernel);

	int Method = Parameters("EXPAND")->asInt();

	for(int i=0; i<m_Kernel.Get_Radius(); i++)
	{
		CSG_Grid Input(*pResult); bool bChanged = false;

		for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( Input.is_NoData(x, y) )
				{
					double Value;

					if( Get_Expand_Value(&Input, x, y, Method, Value, Kernel) )
					{
						pResult->Set_Value(x, y, Value);

						bChanged = true;
					}
				}
			}
		}

		if( !bChanged )
		{
			return( true );
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Do_Expand(CSG_Grid *pInput, CSG_Grid *pResult, const CSG_Grid_Cell_Addressor &Kernel)
{
	int Method = Parameters("EXPAND")->asInt();

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Value;

			if( Get_Expand_Value(pInput, x, y, Method, Value, Kernel) )
			{
				pResult->Set_Value(x, y, Value);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Shrink_Expand::Get_Expand_Value(CSG_Grid *pInput, int x, int y, int Method, double &Value, const CSG_Grid_Cell_Addressor &Kernel)
{
	if( !pInput->is_NoData(x, y) )
	{
		Value = pInput->asDouble(x, y);

		return( true );
	}

	//-----------------------------------------------------
	if( Method == EXPAND_MAJORITY )
	{
		CSG_Unique_Number_Statistics s;

		for(int i=0; i<Kernel.Get_Count(); i++)
		{
			int ix = Kernel.Get_X(i, x);
			int iy = Kernel.Get_Y(i, y);

			if( pInput->is_InGrid(ix, iy) )
			{
				s += pInput->asDouble(ix, iy);
			}
		}

		return( s.Get_Majority(Value) );
	}

	//-----------------------------------------------------
	{
		CSG_Simple_Statistics s;

		for(int i=0; i<Kernel.Get_Count(); i++)
		{
			int ix = Kernel.Get_X(i, x);
			int iy = Kernel.Get_Y(i, y);

			if( pInput->is_InGrid(ix, iy) )
			{
				s += pInput->asDouble(ix, iy);
			}
		}

		if( s.Get_Count() > 0 )
		{
			switch( Method )
			{
			default        : Value = s.Get_Mean   (); break;
			case EXPAND_MIN: Value = s.Get_Minimum(); break;
			case EXPAND_MAX: Value = s.Get_Maximum(); break;
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Shrink_Expand::CGrids_Shrink_Expand(void)
{
	Set_Name		(_TL("Shrink and Expand (Grid Collection)"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"This is the multi-raster version of the \'Shrink and Expand\' tool for single grids "
		"and applies the tool operation to each grid provided by the input grid collection.\n\n"
	) + Get_Description());

	//-----------------------------------------------------
	Parameters.Del_Parameter("INPUT" ); Parameters.Add_Grids("", "INPUT" , _TL("Input" ), _TL(""), PARAMETER_INPUT);
	Parameters.Del_Parameter("RESULT"); Parameters.Add_Grids("", "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Shrink_Expand::On_Execute(void)
{
	if( !Set_Kernel() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grids *pInput  = Parameters("INPUT" )->asGrids(), Input;
	CSG_Grids *pResult = Parameters("RESULT")->asGrids();

	TSG_Data_Type Type = pInput->Get_Type();

	if( Parameters("OPERATION")->asInt ()  > 0     // expands
	&&  Parameters("EXPAND"   )->asInt () == 2     // mean
	&&  Parameters("KEEP_TYPE")->asBool() == false // keep data type?
	&&  Type != SG_DATATYPE_Float && Type != SG_DATATYPE_Double )
	{
		Type = SG_DATATYPE_Float;
	}

	if( !pResult || pResult == pInput )
	{
		Input.Create(*pInput); pResult = pInput; pInput = &Input;
	}
	else
	{
		pResult->Create(Get_System(), pInput->Get_Attributes(), pInput->Get_Z_Attribute(), Type, true);
		pResult->Set_Z_Name_Field(pInput->Get_Z_Name_Field());
		pResult->Set_Name(pInput->Get_Name());
	}

	//-----------------------------------------------------
	for(int i=0; i<pInput->Get_NZ(); i++)
	{
		Process_Set_Text(CSG_String::Format("%s %d/%d", _TL("processing"), i, pInput->Get_NZ()));

		Do_Operation(pInput->Get_Grid_Ptr(i), pResult->Get_Grid_Ptr(i));
	}

	//-----------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrids() )
	{
		DataObject_Update(pResult);
	}
	else switch( Parameters("OPERATION")->asInt() )
	{
	case  0: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Shrink"           )); break;
	case  1: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Expand"           )); break;
	case  2: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Shrink and Expand")); break;
	default: pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Expand and Shrink")); break;
	}

	Set_Kernel(false);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
