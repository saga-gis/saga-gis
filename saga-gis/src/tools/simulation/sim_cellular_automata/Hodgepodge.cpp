
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                sim_cellular_automata                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Hodgepodge.cpp                     //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include "Hodgepodge.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHodgepodge::CHodgepodge(void)
{
	Set_Name		(_TL("Hodgepodge Machine"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"The hodgepodge machine."
	));

	Add_Reference("Dewdney, A.", "1988",
		"Computer Recreations: The Hodgepodge Machine Makes Waves",
		"Scientific American, V.259, No.8, pp.104-107.",
		SG_T("https://www.jstor.org/stable/24989205"), SG_T("jstor.org")
	);

	Add_Reference("Gerhardt, M., Schuster, H.", "1995",
		"Das Digitale Universum, Zelluläre Automaten als Modelle der Natur",
		"Vieweg Verlag, Braunschweig."
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");

	m_Grid_Target.Add_Grid("GRID", _TL("Hodgepodge"), false);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"REFRESH"	, _TL("Refresh"),
		_TL(""),
		true
	);

	Parameters.Add_Int("",
		"RADIUS"	, _TL("Radius"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Int("",
		"NSTATES"	, _TL("Number of Infection States"),
		_TL(""),
		150, 1, true
	);

	Parameters.Add_Int("",
		"TSICK"		, _TL("Threshold Sickness"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Int("",
		"TINFECTED"	, _TL("Threshold Infected"),
		_TL(""),
		3, 1, true
	);

	Parameters.Add_Int("",
		"SPEED"		, _TL("Infection Speed"),
		_TL(""),
		23, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHodgepodge::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CHodgepodge::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHodgepodge::On_Execute(void)
{
	m_Radius	= Parameters("RADIUS"   )->asInt();
	m_nStates	= Parameters("NSTATES"  )->asInt();
	m_tSick		= Parameters("TSICK"    )->asInt();
	m_tInfected	= Parameters("TINFECTED")->asInt();
	m_Speed		= Parameters("SPEED"    )->asInt();

	//-----------------------------------------------------
	m_pStates	= m_Grid_Target.Get_Grid("GRID", SG_DATATYPE_Byte);

	if( !m_pStates || !m_States.Create(m_pStates) )
	{
		Error_Set(_TL("could not create target grid"));

		return( false );
	}

	m_pStates->Set_Name("Hodgepodge");
	m_pStates->Set_NoData_Value(-1);
	m_States  .Set_NoData_Value(-1);

	DataObject_Add(m_pStates);

	//-----------------------------------------------------
	if( Parameters("REFRESH")->asBool() )
	{
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				m_pStates->Set_Value(x, y, (int)CSG_Random::Get_Uniform(0, m_nStates));
			}
		}

		DataObject_Set_Colors(m_pStates, 11, SG_COLORS_COUNT + 4, true);
	}

	//-----------------------------------------------------
	for(int Cycle=1; Process_Get_Okay(true); Cycle++)
	{
		Process_Set_Text("%s: %d", _TL("Life Cycle"), Cycle);

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				Set_State(x, y);
			}
		}

		SG_UI_ProgressAndMsg_Lock(true);
		m_pStates->Assign(&m_States);
		DataObject_Update(m_pStates, 0, m_nStates, SG_UI_DATAOBJECT_SHOW);
		SG_UI_ProgressAndMsg_Lock(false);
	}

	//-----------------------------------------------------
	m_States.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHodgepodge::Set_State(int x, int y)
{
	int	State	= m_pStates->asInt(x, y);

	if( State >= m_nStates )
	{
		m_States.Set_Value(x, y, 0);

		return( true );
	}

	//-----------------------------------------------------
	int	nSick = 0, nInfected = 0, SumStates = 0;

	for(int iy=y-m_Radius; iy<=y+m_Radius; iy++)
	{
		int	jy	= 0 <= iy ? (iy < Get_NY() ? iy : iy - Get_NY()) : iy + Get_NY();

		for(int ix=x-m_Radius; ix<=x+m_Radius; ix++)
		{
			int	jx	= 0 <= ix ? (ix < Get_NX() ? ix : ix - Get_NX()) : ix + Get_NX();

			int	iState	= m_pStates->asInt(jx, jy);

			if( iState > 0 )
			{
				if( iState >= m_nStates )
				{
					nSick	++;
				}

				nInfected	++;
				SumStates	+= iState;
			}
		}
	}

	//-----------------------------------------------------
	if( State == 0 )	
	{
		State	= (nSick / m_tSick) + ((nInfected - nSick) / m_tInfected);
	}
	else
	{
		State	= m_Speed + (SumStates / nInfected);

		if( State > m_nStates )
		{
			State	= m_nStates;
		}
	}

	//-----------------------------------------------------
	m_States.Set_Value(x, y, State);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
