/**********************************************************
 * Version $Id: Life.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                       Life.cpp                        //
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
#include "Life.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLife::CLife(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Conway's Game of Life"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Conway's Game of Life - a classical cellular automat.\n"
		"\n"
		"Reference:\n"
		"- Eigen, M., Winkler, R. (1985): "
		"'Das Spiel - Naturgesetze steuern den Zufall', "
		"Muenchen, 404p.\n"
	));

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("LIFE", _TL("Life"), false);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "FADECOLOR"	, _TL("Fade Color Count"),
		_TL(""),
		PARAMETER_TYPE_Int, 64, 1, true, 255, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLife::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLife::On_Execute(void)
{
	//-----------------------------------------------------
	m_pLife	= m_Grid_Target.Get_Grid("LIFE", SG_DATATYPE_Byte);

	if( !m_pLife )
	{
		Error_Set(_TL("could not create target grid"));

		return( false );
	}

	//-----------------------------------------------------
	m_nColors	= Parameters("FADECOLOR")->asInt();

	for(int y=0; y<m_pLife->Get_NY(); y++)
	{
		for(int x=0; x<m_pLife->Get_NX(); x++)
		{
			m_pLife->Set_Value(x, y, CSG_Random::Get_Uniform(0, 100) < 50 ? 0 : m_nColors);
		}
	}

	//-----------------------------------------------------
	m_pLife->Set_Name(_TL("Conway's Game of Life"));
	m_pLife->Set_NoData_Value(-1);

	DataObject_Add       (m_pLife);
	DataObject_Set_Colors(m_pLife, 11, SG_COLORS_WHITE_BLUE);
	DataObject_Update    (m_pLife, 0, m_nColors, SG_UI_DATAOBJECT_SHOW);

	//-----------------------------------------------------
	int		i;

	m_Count.Create(m_pLife->Get_System(), SG_DATATYPE_Byte);

	for(i=1; Process_Get_Okay(true) && Next_Cycle(i > m_nColors); i++)
	{
		Process_Set_Text("%s: %d", _TL("Life Cycle"), i);

		DataObject_Update(m_pLife, 0, m_nColors);
	}

	m_Count.Destroy();

	//-----------------------------------------------------
	if( is_Progress() )
	{
		Message_Fmt("\n%s %d %s\n", _TL("Dead after"), i, _TL("Life Cycles"));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLife::Next_Cycle(bool bCheck4Change)
{
	//-----------------------------------------------------
	bool	bContinue	= bCheck4Change ? false : true;

	int		y;

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<m_pLife->Get_NY(); y++)
	{
		for(int x=0; x<m_pLife->Get_NX(); x++)
		{
			int		n	= 0;

			for(int i=0; i<8; i++)
			{
				int	ix	= CSG_Grid_System::Get_xTo(i, x);;
				int iy	= CSG_Grid_System::Get_yTo(i, y);;

				if( ix < 0 ) ix = m_pLife->Get_NX() - 1; else if( ix >= m_pLife->Get_NX() ) ix = 0;
				if( iy < 0 ) iy = m_pLife->Get_NY() - 1; else if( iy >= m_pLife->Get_NY() ) iy = 0;

				if( m_pLife->asByte(ix, iy) == m_nColors )
				{
					n++;
				}
			}

			m_Count.Set_Value(x, y, n);

			if( bCheck4Change && m_pLife->asByte(x, y) > 0 && m_pLife->asByte(x, y) < m_nColors - 1 )
			{
				bContinue	= true;
			}
		}
	}

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<m_pLife->Get_NY(); y++)
	{
		for(int x=0; x<m_pLife->Get_NX(); x++)
		{
			int		n	= m_Count.asByte(x, y);

			switch( n )
			{
			case  2:	// keep status
				if( m_pLife->asByte(x, y) > 0 && m_pLife->asByte(x, y) < m_nColors )
				{
					m_pLife->Add_Value(x, y, -1);
				}
				break;

			case  3:	// birth
				{
					m_pLife->Set_Value(x, y, m_nColors);
				}
				break;

			default:	// death
				if( m_pLife->asByte(x, y) > 0 )
				{
					m_pLife->Add_Value(x, y, -1);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( bContinue );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
