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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Rank Filter"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Rank filter for grids. Set rank to fifty percent to apply a median filter."
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
		"RANK"		, _TL("Rank"),
		_TL("The rank [percent]."),
		50.0, 0.0, true, 100.0, true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Rank::On_After_Execution(void)
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
bool CFilter_Rank::On_Execute(void)
{
	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	double	Rank	= Parameters("RANK")->asDouble() / 100.0;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")->asGrid();

	CSG_Grid	Result, *pResult	= Parameters("RESULT")->asGrid();

	if( !pResult || pResult == m_pInput )
	{
		pResult	= &Result;
		
		pResult->Create(m_pInput);
	}
	else
	{
		pResult->Fmt_Name("%s [%s: %.1f]", m_pInput->Get_Name(), _TL("Rank"), 100.0 * Rank);

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());

		DataObject_Set_Parameters(pResult, m_pInput);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Value;

			if( Get_Value(x, y, Rank, Value) )
			{
				pResult->Set_Value(x, y, Value);
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
bool CFilter_Rank::Get_Value(int x, int y, double Rank, double &Value)
{
	if( m_pInput->is_InGrid(x, y) )
	{
		CSG_Table	Values;

		Values.Add_Field("Z", SG_DATATYPE_Double);

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) )
			{
				Values.Add_Record()->Set_Value(0, m_pInput->asDouble(ix, iy));
			}
		}

		switch( Values.Get_Count() )
		{
		case 0:
			return( false );

		case 1:
			Value	= Values[0].asDouble(0);
			return( true );

		case 2:
			Value	= (Values[0].asDouble(0) + Values[1].asDouble(0)) / 2.0;
			return( true );

		default:
			{
				Values.Set_Index(0, TABLE_INDEX_Ascending);

				Rank	= Rank * (Values.Get_Count() - 1.0);

				int	i	= (int)Rank;

				Value	= Values.Get_Record_byIndex(i)->asDouble(0);

				if( Rank - i > 0.0 && i < Values.Get_Count() - 1 )
				{
					Value	= (Value + Values.Get_Record_byIndex(i + 1)->asDouble(0)) / 2.0;
				}
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
