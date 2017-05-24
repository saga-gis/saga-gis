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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Majority Filter"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Majority filter for grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double(NULL,
		"THRESHOLD"	, _TL("Threshold"),
		_TL("The majority threshold [percent]."),
		0.0, 0.0, true, 100.0, true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Majority::On_After_Execution(void)
{
	if( Parameters("RESULT")->asGrid() == Parameters("INPUT")->asGrid() )
	{
		Parameters("RESULT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Majority::On_Execute(void)
{
	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	m_Threshold	= 1 + (int)((1 + m_Kernel.Get_Count()) * Parameters("THRESHOLD")->asDouble() / 100.0);

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")->asGrid();

	CSG_Grid	Result, *pResult = Parameters("RESULT")->asGrid();

	if( !pResult || pResult == m_pInput )
	{
		pResult	= &Result;
		
		pResult->Create(m_pInput);
	}
	else
	{
		pResult->Set_Name(CSG_String::Format("%s [%s]", m_pInput->Get_Name(), _TL("Majority Filter")));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());

		DataObject_Set_Parameters(pResult, m_pInput);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_InGrid(x, y) )
			{
				pResult->Set_Value(x, y, Get_Majority(x, y));
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-------------------------------------------------
	if( pResult == &Result )
	{
		CSG_MetaData	History	= m_pInput->Get_History();

		m_pInput->Assign(pResult);
		m_pInput->Get_History() = History;

		DataObject_Update(m_pInput);

		Parameters("RESULT")->Set_Value(m_pInput);
	}

	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_Majority::Get_Majority(int x, int y)
{
	CSG_Class_Statistics	Majority;

	Majority.Add_Value(m_pInput->asDouble(x, y));

	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i, x);
		int	iy	= m_Kernel.Get_Y(i, y);

		if( m_pInput->is_InGrid(ix, iy) )
		{
			Majority.Add_Value(m_pInput->asDouble(ix, iy));
		}
	}

	int		Count;
	double	Value;

	Majority.Get_Majority(Value, Count);

	return( Count > m_Threshold ? Value : m_pInput->asDouble(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
