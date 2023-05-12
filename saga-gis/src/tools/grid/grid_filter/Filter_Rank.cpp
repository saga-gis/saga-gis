
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
//                    Filter_Rank.cpp                    //
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
#include "Filter_Rank.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Rank::CFilter_Rank(void)
{
	Set_Name		(_TL("Rank Filter"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Rank filter for grids. Set rank to fifty percent to apply a median filter."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT" , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT", _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"RANK"  , _TL("Rank"),
		_TL("The rank [percent]."),
		50., 0., true, 100., true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Rank::On_Execute(void)
{
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("Kernel initialization failed!"));

		return( false );
	}

	double Quantile = Parameters("RANK")->asDouble() / 100.;

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

		pResult->Fmt_Name("%s [%s: %.1f%%]", m_pInput->Get_Name(), _TL("Rank"), 100. * Quantile);

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Value;

			if( Get_Value(x, y, Quantile, Value) )
			{
				pResult->Set_Value(x, y, Value);
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
bool CFilter_Rank::Get_Value(int x, int y, double Quantile, double &Value)
{
	if( m_pInput->is_InGrid(x, y) )
	{
		CSG_Simple_Statistics s(true);

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int ix = m_Kernel.Get_X(i, x);
			int iy = m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) )
			{
				s += m_pInput->asDouble(ix, iy);
			}
		}

		if( s.Get_Count() > 0 )
		{
			Value = s.Get_Quantile(Quantile);

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
