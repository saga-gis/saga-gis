/**********************************************************
 * Version $Id: Filter_Sieve.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                   Filter_Sieve.cpp                    //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "Filter_Sieve.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Sieve::CFilter_Sieve(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Sieving Classes"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Majority filter for grids."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Classes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"		, _TL("Sieved Classes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "MODE"		, _TL("Neighbourhood"),
		_TL("Neumann: the four horizontally and vertically neighboured cells; Moore: all eight adjacent cells"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Neumann"),
			_TL("Moore")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Minimum Threshold"),
		_TL("Minimum number of cells in a group of adjacent cells."),
		PARAMETER_TYPE_Int, 4, 2, true
	);

	CSG_Parameter	*pNode	= Parameters.Add_Choice(
		NULL	, "ALL"			, _TL("Class Selection"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("single class"),
			_TL("all classes")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "CLASS"		, _TL("Class Identifier"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFilter_Sieve::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "ALL") )
	{
		pParameters->Get_Parameter("CLASS")->Set_Enabled(pParameter->asInt() == 0);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Sieve::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrid		= Parameters("OUTPUT")->asGrid();

	if( m_pGrid && m_pGrid != Parameters("INPUT")->asGrid() )
	{
		m_pGrid->Create(*Parameters("INPUT")->asGrid());
	}
	else
	{
		m_pGrid	= Parameters("INPUT")->asGrid();
	}

	//-----------------------------------------------------
	m_Mode		= Parameters("MODE"     )->asInt() == 0 ? 2 : 1;
	m_Threshold	= Parameters("THRESHOLD")->asInt();

	bool	bAll	= Parameters("ALL"  )->asInt() == 1;
	double	Class	= Parameters("CLASS")->asDouble();

	//-----------------------------------------------------
	Lock_Create();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !Lock_Get(x, y) && !m_pGrid->is_NoData(x, y) && (bAll || Class == m_pGrid->asDouble(x, y)) )
			{
				m_Class	= m_pGrid->asDouble(x, y);

				if( Get_Size(x, y) < m_Threshold )
				{
					Do_Sieve(x, y);
				}
				else
				{
					Do_Keep (x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	Lock_Destroy();

	if( m_pGrid == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(m_pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFilter_Sieve::Get_Size(int x, int y, int n)
{
	if( m_pGrid->is_InGrid(x, y) && m_Class == m_pGrid->asDouble(x, y) )
	{
		switch( Lock_Get(x, y) )
		{
		case 0:	// not yet processed at all
			{
				Lock_Set(x, y, 1);

				n++;

				for(int i=0; i<8 && n<m_Threshold; i+=m_Mode)
				{
					n	= Get_Size(Get_xTo(i, x), Get_yTo(i, y), n);
				}

				return( n );
			}

		case 2:	// marked as not to be sieved, so don't sieve any adjacent cell either
			{
				return( m_Threshold );
			}
		}
	}

	return( 0 );
}

//---------------------------------------------------------
void CFilter_Sieve::Do_Keep(int x, int y)
{
	if( m_pGrid->is_InGrid(x, y) && Lock_Get(x, y) == 1 )
	{
		Lock_Set(x, y, 2);

		for(int i=0; i<8; i+=m_Mode)
		{
			Do_Keep(Get_xTo(i, x), Get_yTo(i, y));
		}
	}
}

//---------------------------------------------------------
void CFilter_Sieve::Do_Sieve(int x, int y)
{
	if( m_pGrid->is_InGrid(x, y) && Lock_Get(x, y) == 1 )
	{
		Lock_Set(x, y, 3);

		m_pGrid->Set_NoData(x, y);

		for(int i=0; i<8; i+=m_Mode)
		{
			Do_Sieve(Get_xTo(i, x), Get_yTo(i, y));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
