/**********************************************************
 * Version $Id: Filter_Sieve.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
	Set_Name		(_TL("Sieve Classes"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"The 'Sieve Classes' tool counts the number of adjacent cells "
		"sharing the same value (the class identifier). Areas that are formed "
		"by less cells than specified by the treshold will be removed "
		"(sieved), i.e. they are set to no-data. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Classes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Sieved Classes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"MODE"		, _TL("Neighbourhood"),
		_TL("Neumann: the four horizontally and vertically neighboured cells; Moore: all eight adjacent cells"),
		CSG_String::Format("%s|%s|",
			_TL("Neumann"),
			_TL("Moore")
		), 0
	);

	Parameters.Add_Int("",
		"THRESHOLD"	, _TL("Minimum Threshold"),
		_TL("Minimum number of cells in a group of adjacent cells."),
		4, 2, true
	);

	Parameters.Add_Choice("",
		"ALL"		, _TL("Class Selection"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("single class"),
			_TL("all classes")
		), 1
	);

	Parameters.Add_Double("ALL",
		"CLASS"		, _TL("Class Identifier"),
		_TL(""),
		1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFilter_Sieve::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("ALL") )
	{
		pParameters->Set_Enabled("CLASS", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	UNPROCESSED	= 0,
	IN_PROCESS,
	SIEVE,
	KEEP
};


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
		CSG_Grid	*pGrid	= Parameters("INPUT")->asGrid();

		m_pGrid->Create(*pGrid);

		m_pGrid->Set_Name(CSG_String::Format("%s [%s]", pGrid->Get_Name(), Get_Name().c_str()));
		m_pGrid->Set_NoData_Value(pGrid->Get_NoData_Value());

		DataObject_Set_Parameters(m_pGrid, pGrid);
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

	Lock_Create();

	int	y;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pGrid->is_NoData(x, y) )
			{
				Lock_Set(x, y, KEEP);
			}
			else if( Lock_Get(x, y) == UNPROCESSED )
			{
				if( !bAll && Class != m_pGrid->asDouble(x, y) )
				{
					Lock_Set(x, y, KEEP);
				}
				else
				{
					m_Class	= m_pGrid->asDouble(x, y);

					Do_Sieve(x, y, Get_Size(x, y) < m_Threshold);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( Lock_Get(x, y) == SIEVE )
			{
				m_pGrid->Set_NoData(x, y);
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
		case KEEP:	// already marked as not to be sieved, so don't sieve any adjacent cell either
			{
				return( m_Threshold );
			}

		case UNPROCESSED:	// not yet processed at all
			{
				Lock_Set(x, y, IN_PROCESS);

				n++;

				for(int i=0; i<8 && n<m_Threshold; i+=m_Mode)
				{
					n	= Get_Size(Get_xTo(i, x), Get_yTo(i, y), n);
				}
			}
		}
	}

	return( n );
}

//---------------------------------------------------------
void CFilter_Sieve::Do_Sieve(int x, int y, bool bSieve)
{
	if( m_pGrid->is_InGrid(x, y) && Lock_Get(x, y) == IN_PROCESS )
	{
		Lock_Set(x, y, bSieve ? SIEVE : KEEP);

		for(int i=0; i<8; i+=m_Mode)
		{
			Do_Sieve(Get_xTo(i, x), Get_yTo(i, y), bSieve);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
