/**********************************************************
 * Version $Id: Wator.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Cellular_Automata                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Wator.cpp                       //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Wator.h"

//---------------------------------------------------------
#define FISH	1
#define SHARK	2


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWator::CWator(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Wa-Tor"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Wa-Tor - an ecological simulation of predator-prey populations - "
		"is based upon A. K. Dewdney's 'Computer Recreations' article "
		"in the December 1984 issue of Scientific American."
	));

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("GRID", _TL("Wa-Tor"), false);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Value(
		NULL	, "REFRESH"			, _TL("Refresh"),
		_TL(""),
		PARAMETER_TYPE_Bool			, true
	);

	Parameters.Add_Value(
		pNode	, "INIT_FISH"		, _TL("Initial Number of Fishes [%]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 30.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		pNode	, "INIT_SHARK"		, _TL("Initial Number of Sharks [%]"),
		_TL(""),
		PARAMETER_TYPE_Double		, 7.5, 0.0, true, 100.0, true
	);

	Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Cycles"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "FISH_BIRTH"		, _TL("Birth Rate of Fishes"),
		_TL(""),
		PARAMETER_TYPE_Int			,  3.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "SHARK_BIRTH"		, _TL("Birth Rate of Sharks"),
		_TL(""),
		PARAMETER_TYPE_Int			, 12.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "SHARK_STARVE"	, _TL("Max. Starvation Time for Sharks"),
		_TL(""),
		PARAMETER_TYPE_Int			,  4.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWator::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWator::On_Execute(void)
{
	//-----------------------------------------------------
	m_pWator	= m_Grid_Target.Get_Grid("GRID", SG_DATATYPE_Byte);

	if( !m_pWator )
	{
		Error_Set(_TL("could not create target grid"));

		return( false );
	}

	//-----------------------------------------------------
	m_pWator->Set_Name(_TL("Wa-Tor"));
	m_pWator->Set_NoData_Value(-1);

	CSG_Colors	Colors(3);

	Colors.Set_Color(0, SG_COLOR_BLACK);
	Colors.Set_Color(1, SG_COLOR_GREEN);
	Colors.Set_Color(2, SG_COLOR_RED  );

	DataObject_Add       (m_pWator);
	DataObject_Set_Colors(m_pWator, Colors);
	DataObject_Update    (m_pWator, 0, 2, SG_UI_DATAOBJECT_SHOW);

	//-----------------------------------------------------
	if( Parameters("REFRESH")->asBool() )
	{
		double	Fish_perc	= Parameters("INIT_FISH" )->asDouble();
		double	Shark_perc	= Parameters("INIT_SHARK")->asDouble() + Fish_perc;

		#pragma omp parallel for
		for(int y=0; y<m_pWator->Get_NY(); y++)
		{
			for(int x=0; x<m_pWator->Get_NX(); x++)
			{
				double	perc	= CSG_Random::Get_Uniform(0, 100);

				if( perc <= Fish_perc )
				{
					m_pWator->Set_Value(x, y, FISH);
				}
				else if( perc <= Shark_perc )
				{
					m_pWator->Set_Value(x, y, SHARK);
				}
				else
				{
					m_pWator->Set_Value(x, y, 0);
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	pTable->Destroy();
	pTable->Set_Name(_TL("Wa-Tor"));

	pTable->Add_Field("Cycle" , SG_DATATYPE_Int);
	pTable->Add_Field("Fishes", SG_DATATYPE_Int);
	pTable->Add_Field("Sharks", SG_DATATYPE_Int);

	//-----------------------------------------------------
	m_Fish_Birth	= Parameters("FISH_BIRTH"  )->asInt();
	m_Shark_Birth	= Parameters("SHARK_BIRTH" )->asInt();
	m_Shark_Starve	= Parameters("SHARK_STARVE")->asInt();

	m_Next  .Create(m_pWator, SG_DATATYPE_Byte);
	m_Age   .Create(m_pWator, SG_DATATYPE_Byte);
	m_Starve.Create(m_pWator, SG_DATATYPE_Byte);

	#pragma omp parallel for
	for(int y=0; y<m_pWator->Get_NY(); y++)
	{
		for(int x=0; x<m_pWator->Get_NX(); x++)
		{
			switch( m_pWator->asByte(x, y) )
			{
			case FISH:
				m_Age   .Set_Value(x, y, CSG_Random::Get_Uniform(0.0, m_Fish_Birth  ));
				break;

			case SHARK:
				m_Age   .Set_Value(x, y, CSG_Random::Get_Uniform(0.0, m_Shark_Birth ));
				m_Starve.Set_Value(x, y, CSG_Random::Get_Uniform(0.0, m_Shark_Starve));
				break;
			}
		}
	}

	//-----------------------------------------------------
	int		i;

	SG_UI_Progress_Lock(true);

	for(i=1; Process_Get_Okay(true) && Next_Cycle(); i++)
	{
		Process_Set_Text(CSG_String::Format("%s: %d", _TL("Life Cycle"), i));

		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(0, i);
		pRecord->Set_Value(1, m_nFishes);
		pRecord->Set_Value(2, m_nSharks);

		DataObject_Update(m_pWator, 0, 2);
		DataObject_Update(pTable);
	}

	SG_UI_Progress_Lock(false);

	//-----------------------------------------------------
	m_Next  .Destroy();
	m_Age   .Destroy();
	m_Starve.Destroy();

	if( is_Progress() )
	{
		Message_Add(CSG_String::Format("%s %d %s", _TL("Dead after"), i, _TL("Life Cycles")));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_NEIGHBOR			{	ix = m_pWator->Get_System().Get_xTo(i, x); if( ix < 0 ) ix = m_pWator->Get_NX() - 1; else if( ix >= m_pWator->Get_NX() ) ix = 0;\
									iy = m_pWator->Get_System().Get_yTo(i, y); if( iy < 0 ) iy = m_pWator->Get_NY() - 1; else if( iy >= m_pWator->Get_NY() ) iy = 0;	}

#define GET_NEIGHBOR_RANDOMLY	{	i = iNeighbor[(int)((double)rand() * nNeighbors / (double)RAND_MAX)];\
									ix = m_pWator->Get_System().Get_xTo(i, x); if( ix < 0 ) ix = m_pWator->Get_NX() - 1; else if( ix >= m_pWator->Get_NX() ) ix = 0;\
									iy = m_pWator->Get_System().Get_yTo(i, y); if( iy < 0 ) iy = m_pWator->Get_NY() - 1; else if( iy >= m_pWator->Get_NY() ) iy = 0;	}

//---------------------------------------------------------
bool CWator::Next_Cycle(void)
{
	static int	iDir	= 0;

	int		x, y, i, ix, iy, xx, yy, ax, ay, dx, dy,
			iNeighbor[8], nNeighbors,
			Age, Starve;

	//-----------------------------------------------------
	m_nFishes	= 0;
	m_nSharks	= 0;

	m_Next.Assign(0.0);

	switch( iDir )
	{
	default: ay	= m_pWator->Get_NY() - 1; dy = -1; ax = m_pWator->Get_NX() - 1; dx = -1; iDir=0; break;
	case  2: ay	= 0;                      dy =  1; ax = m_pWator->Get_NX() - 1; dx = -1; iDir++; break;
	case  1: ay	= m_pWator->Get_NY() - 1; dy = -1; ax = 0;                      dx =  1; iDir++; break;
	case  0: ay	= 0;                      dy =  1; ax = 0;                      dx =  1; iDir++; break;
	}

	//-----------------------------------------------------
	for(yy=0, y=ay; yy<m_pWator->Get_NY(); yy++, y+=dy)
	{
		for(xx=0, x=ax; xx<m_pWator->Get_NX(); xx++, x+=dx)
		{
			if( m_pWator->asByte(x, y) == FISH )
			{
				m_nFishes++;

				Age		= m_Age.asInt(x, y) + 1;

				m_Age.Set_Value(x, y, 0);

				for(i=0, nNeighbors=0; i<8; i++)
				{
					GET_NEIGHBOR;

					if( m_pWator->asByte(ix, iy) == 0 && m_Next.asByte(ix, iy) == 0 )
					{
						iNeighbor[nNeighbors++]	= i;
					}
				}

				if( nNeighbors > 0 )
				{
					GET_NEIGHBOR_RANDOMLY;

					m_Next.Set_Value(ix, iy, FISH);
					m_Age .Set_Value(ix, iy, Age >= m_Fish_Birth ? 0 : Age);

					if( Age >= m_Fish_Birth )
					{
						m_Next.Set_Value(x, y, FISH);
						m_Age .Set_Value(x, y, CSG_Random::Get_Uniform(0, m_Fish_Birth));
					}
					else
					{
						m_pWator->Set_Value(x, y, 0);
					}
				}
				else
				{
					m_Next.Set_Value(x, y, FISH);
					m_Age .Set_Value(x, y, Age >= m_Fish_Birth ? 0 : m_Fish_Birth);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(yy=0, y=ay; yy<m_pWator->Get_NY(); yy++, y+=dy)
	{
		for(xx=0, x=ax; xx<m_pWator->Get_NX(); xx++, x+=dx)
		{
			if( m_pWator->asByte(x, y) == SHARK )
			{
				m_nSharks++;

				Age		= m_Age.asInt(x, y) + 1;

				m_Age.Set_Value(x, y, 0);

				Starve	= m_Starve.asInt(x, y) + 1;
				m_Starve.Set_Value(x, y, 0);

				for(i=0, nNeighbors=0; i<8; i++)
				{
					GET_NEIGHBOR;

					if( m_Next.asByte(ix, iy) == FISH )
					{
						iNeighbor[nNeighbors++]	= i;
					}
				}

				if( nNeighbors > 0 )
				{
					GET_NEIGHBOR_RANDOMLY;

					m_nFishes--;
					m_pWator->Set_Value(ix, iy, 0);
					m_Next   .Set_Value(ix, iy, SHARK);
					m_Age    .Set_Value(ix, iy, Age >= m_Shark_Birth ? 0 : Age);
					m_Starve .Set_Value(ix, iy, 0);

					if( Age >= m_Shark_Birth )
					{
						m_Next   .Set_Value(x, y, SHARK);
						m_Age    .Set_Value(x, y, CSG_Random::Get_Uniform(0, m_Shark_Birth));
						m_Starve .Set_Value(x, y, 0);
					}
					else
					{
						m_pWator->Set_Value(x, y, 0);
					}
				}
				else if( Starve <= m_Shark_Starve )
				{
					for(i=0, nNeighbors=0; i<8; i++)
					{
						GET_NEIGHBOR;

						if( m_pWator->asByte(ix, iy) == 0 && m_Next.asByte(ix, iy) == 0 )
						{
							iNeighbor[nNeighbors++]	= i;
						}
					}

					if( nNeighbors > 0 )
					{
						GET_NEIGHBOR_RANDOMLY;

						m_Next  .Set_Value(ix, iy, SHARK);
						m_Age   .Set_Value(ix, iy, Age >= m_Shark_Birth ? 0 : Age);
						m_Starve.Set_Value(ix, iy, Starve);

						if( Age >= m_Shark_Birth )
						{
							m_Next   .Set_Value(x, y, SHARK);
							m_Age    .Set_Value(x, y, CSG_Random::Get_Uniform(0, m_Shark_Birth));
							m_Starve .Set_Value(x, y, Starve);
						}
						else
						{
							m_pWator	->Set_Value( x,  y, 0);
						}
					}
					else
					{
						m_Next  .Set_Value(x, y, SHARK);
						m_Age   .Set_Value(x, y, Age >= m_Shark_Birth ? 0 : m_Shark_Birth);
						m_Starve.Set_Value(x, y, Starve);
					}
				}
				else
				{
					m_nSharks--;
				}
			}
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for private(x, y)
	for(y=0; y<m_pWator->Get_NY(); y++)
	{
		for(x=0; x<m_pWator->Get_NX(); x++)
		{
			m_pWator->Set_Value(x, y, m_Next.asByte(x, y));
		}
	}

	return( (m_nFishes > 0 && m_nFishes < m_pWator->Get_NCells()) || m_nSharks > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
