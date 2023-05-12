
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Filter_Majority.cpp                  //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                      Olaf Conrad                      //
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
#include "Filter_Majority.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Majority::CFilter_Majority(void)
{
	Set_Name		(_TL("Majority/Minority Filter"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Majority filter for grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"TYPE"		, _TL("Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Majority"),
			_TL("Minority")
		), 0
	);

	Parameters.Add_Double("",
		"THRESHOLD"	, _TL("Threshold"),
		_TL("The majority/minority threshold [percent]."),
		0., 0., true, 100., true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Majority::On_Execute(void)
{
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("Kernel initialization failed!"));

		return( false );
	}

	//-----------------------------------------------------
	bool bMajority = Parameters("TYPE")->asInt() == 0;

	double d = Parameters("THRESHOLD")->asDouble() / 100.;

	if( bMajority == false )
	{
		d = 1. - d;
	}

	int Threshold = (int)(0.5 + d * m_Kernel.Get_Count());

	//-----------------------------------------------------
	CSG_Grid Input; m_pInput = Parameters("INPUT")->asGrid();

	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	if( !pResult || pResult == m_pInput )
	{
		Input.Create(*m_pInput); pResult = m_pInput; m_pInput = &Input;
	}
	else
	{
		DataObject_Set_Parameters(pResult, m_pInput);

		pResult->Fmt_Name("%s [%s %s]", m_pInput->Get_Name(), bMajority ? _TL("Majority") : _TL("Minority"), _TL("Filter"));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pInput->is_NoData(x, y) )
			{
				pResult->Set_Value(x, y, Get_Value(x, y, bMajority, Threshold));
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	m_Kernel.Destroy();

	//-------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_Majority::Get_Value(int x, int y, bool bMajority, int Threshold)
{
	CSG_Unique_Number_Statistics s;

	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int ix = m_Kernel.Get_X(i, x);
		int iy = m_Kernel.Get_Y(i, y);

		if( m_pInput->is_InGrid(ix, iy) )
		{
			s += m_pInput->asDouble(ix, iy);
		}
	}

	int Count; double Value;

	if( bMajority )
	{
		s.Get_Majority(Value, Count);

		return( Count > Threshold ? Value : m_pInput->asDouble(x, y) );
	}
	else
	{
		s.Get_Minority(Value, Count);

		return( Count < Threshold ? Value : m_pInput->asDouble(x, y) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
