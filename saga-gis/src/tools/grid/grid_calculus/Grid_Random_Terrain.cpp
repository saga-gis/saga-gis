
/*******************************************************************************
    Grid_Random_Terrain.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Random_Terrain.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Random_Terrain::CGrid_Random_Terrain(void)
{	 
	Set_Name		(_TL("Random Terrain"));

	Set_Author		("V.Olaya (c) 2004");

	Set_Description	(_TW(
		"(c) 2004 by Victor Olaya. Random Terrain Generation"
	));

	Parameters.Add_Int(
		"", "RADIUS"    , _TL("Radius (cells)"), 
		_TL(""), 
		25, 1, true
	);

	Parameters.Add_Int(
		"", "ITERATIONS", _TL("Iterations"), 
		_TL(""),
		100, 1, true
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, true, "", "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Random_Terrain::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Random_Terrain::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Random_Terrain::On_Execute(void)
{
	if( (m_pGrid = m_Grid_Target.Get_Grid()) == NULL )
	{
		Error_Set(_TL("invalid target grid"));

		return( false );
	}

	m_pGrid->Set_Name(_TL("Random Terrain"));

	m_pGrid->Assign(0.);

	//-----------------------------------------------------
	m_Kernel.Set_Radius(m_Radius = Parameters("RADIUS")->asInt());

	int Iterations	= Parameters("ITERATIONS")->asInt();

	for(int i=0; i<Iterations && Set_Progress(i, Iterations); i++)
	{
		Add_Bump();
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Random_Terrain::Add_Bump(void)
{
	int	x	= CSG_Random::Get_Uniform(-m_Radius, m_Radius + m_pGrid->Get_NX());
	int	y	= CSG_Random::Get_Uniform(-m_Radius, m_Radius + m_pGrid->Get_NY());

	for(int i=0; i<m_Kernel.Get_Count(); i++)
	{
		int	ix	= m_Kernel.Get_X(i, x);
		int	iy	= m_Kernel.Get_Y(i, y);

		if( m_pGrid->is_InGrid(ix, iy) )
		{
			m_pGrid->Add_Value(ix, iy, (m_Radius*m_Radius) - SG_Get_Square(m_Kernel.Get_Distance(i)));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
