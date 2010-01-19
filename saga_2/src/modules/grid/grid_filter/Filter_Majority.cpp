
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
void CMajority::Create(int nBuffer)
{
	m_nValues	= 0;
	m_nBuffer	= nBuffer;

	if( m_nBuffer > 0 )
	{
		m_Values	= (double *)SG_Malloc(m_nBuffer * sizeof(double));
		m_Count		= (int    *)SG_Malloc(m_nBuffer * sizeof(int));
	}
}

//---------------------------------------------------------
void CMajority::Destroy(void)
{
	if( m_nBuffer > 0 )
	{
		SG_Free(m_Values);
		SG_Free(m_Count);
	}

	m_nBuffer	= 0;
	m_nValues	= 0;
}

//---------------------------------------------------------
void CMajority::Add_Value(double Value)
{
	for(int i=0; i<m_nValues; i++)
	{
		if( m_Values[i] == Value )
		{
			m_Count[i]++;

			return;
		}
	}

	if( m_nValues < m_nBuffer )
	{
		m_Values[m_nValues]	= Value;
		m_Count [m_nValues]	= 1;
		m_nValues++;
	}
}

//---------------------------------------------------------
bool CMajority::Get_Majority(int &Count, double &Value)
{
	if( m_nValues > 0 )
	{
		Value	= m_Values[0];
		Count	= m_Count [0];

		for(int i=1; i<m_nValues; i++)
		{
			if( m_Count[i] > Count )
			{
				Value	= m_Values[i];
				Count	= m_Count [i];
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Majority::CFilter_Majority(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Majority Filter"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Majority filter for grids."
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Value(
		NULL, "THRESHOLD"	, _TL("Threshold [Percent]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Majority::On_Execute(void)
{
	int			x, y, ix, iy;
	CSG_Grid	*pResult;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();
	m_Radius	= Parameters("RADIUS")	->asInt();

	//-----------------------------------------------------
	if( !pResult || pResult == m_pInput )
	{
		pResult	= SG_Create_Grid(m_pInput);

		Parameters("RESULT")->Set_Value(m_pInput);
	}

	pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());

	//-----------------------------------------------------
	m_Kernel.Create(SG_DATATYPE_Byte, 1 + 2 * m_Radius, 1 + 2 * m_Radius);
	m_Kernel.Set_NoData_Value(0.0);
	m_Kernel.Assign(1.0);
	m_Kernel.Set_Value(m_Radius, m_Radius, 0.0);

	if( Parameters("MODE")->asInt() == 1 )
	{
		for(y=-m_Radius, iy=0; y<=m_Radius; y++, iy++)
		{
			for(x=-m_Radius, ix=0; x<=m_Radius; x++, ix++)
			{
				if( x*x + y*y > m_Radius*m_Radius )
				{
					m_Kernel.Set_Value(ix, iy, 0.0);
				}
			}
		}
	}

	m_Majority.Create(m_Kernel.Get_NCells());

	m_Threshold	= m_Kernel.Get_NoData_Count();

	m_Threshold	= 1 + (int)(0.01 * Parameters("THRESHOLD")->asDouble() * (1 + m_Kernel.Get_NCells() - m_Kernel.Get_NoData_Count()));

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				pResult->Set_NoData(x, y);
			}
			else
			{
				pResult->Set_Value(x, y, Get_Majority(x, y));
			}
		}
	}

	//-----------------------------------------------------
	if( m_pInput == Parameters("RESULT")->asGrid() )
	{
		m_pInput->Assign(pResult);

		delete(pResult);
	}
	else
	{
		pResult->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Majority Filtered")));
	}

	m_Kernel	.Destroy();
	m_Majority	.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CFilter_Majority::Get_Majority(int x, int y)
{
	m_Majority.Reset();

	m_Majority.Add_Value(m_pInput->asDouble(x, y));

	for(int iy=0, jy=y-m_Radius; iy<m_Kernel.Get_NY(); iy++, jy++)
	{
		for(int ix=0, jx=x-m_Radius; ix<m_Kernel.Get_NX(); ix++, jx++)
		{
			if( m_Kernel.asByte(ix, iy) && m_pInput->is_InGrid(jx, jy) )
			{
				m_Majority.Add_Value(m_pInput->asDouble(jx, jy));
			}
		}
	}

	int		Count;
	double	Value;

	m_Majority.Get_Majority(Count, Value);

	return( Count > m_Threshold ? Value : m_pInput->asDouble(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
