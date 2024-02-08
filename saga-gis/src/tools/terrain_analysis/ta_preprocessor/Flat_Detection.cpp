
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Flat_Detection.cpp                   //
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
#include "Flat_Detection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlat_Detection::CFlat_Detection(void)
{
	Set_Name		(_TL("Flat Detection"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Identifies areas of connected cells sharing identical values. "
		"Connected cells can be searched by Neumann or Moore neighbourhood. "
	));

	Parameters.Add_Grid("",
		"DEM"          , _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"NOFLATS"      , _TL("No Flats"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"FLATS"        , _TL("Flat Areas"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"FLAT_OUTPUT"  , _TL("Flat Area Values"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("elevation"),
			_TL("enumeration")
		), 0
	);

	Parameters.Add_Choice("",
		"NEIGHBOURHOOD", _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s",
			SG_T("Neumann"),
			SG_T("Moore")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlat_Detection::On_Execute(void)
{
	m_pDEM        = Parameters("DEM"        )->asGrid();
	m_pNoFlats    = Parameters("NOFLATS"    )->asGrid();
	m_pFlats      = Parameters("FLATS"      )->asGrid();
	m_Flat_Output = Parameters("FLAT_OUTPUT")->asInt();

	//-----------------------------------------------------
	if( m_pNoFlats )
	{
		m_pNoFlats->Assign(m_pDEM);
		m_pNoFlats->Fmt_Name("%s [%s]", m_pDEM->Get_Name(), _TL("No Flats"));
	}

	if( m_pFlats )
	{
		m_pFlats  ->Assign_NoData();
		m_pFlats  ->Fmt_Name("%s [%s]", m_pDEM->Get_Name(), _TL("Flats"));
	}

	m_Flats.Create(Get_System(), SG_DATATYPE_Int); m_Flats.Assign(0.);

	m_Neighbour = Parameters("NEIGHBOURHOOD")->asInt() == 0 ? 2 : 1;

	int nFlats = 0;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( Needs_Processing(x, y) )
			{
				Process_Flat(x, y, ++nFlats);
			}
		}
	}

	//-----------------------------------------------------
	m_Stack.Destroy();
	m_Flats.Destroy();

	Message_Fmt("%s: %d", _TL("detected flats"), nFlats);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlat_Detection::Needs_Processing(int x, int y)
{
	if( m_Flats.asInt(x, y) == 0 && m_pDEM->is_NoData(x, y) == false )
	{
		double z = m_pDEM->asDouble(x, y);

		for(int i=0; i<8; i+=m_Neighbour)
		{
			int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && m_pDEM->asDouble(ix, iy) == z )
			{
				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CFlat_Detection::Process_Cell(int x, int y, int id, double z)
{
	if( m_pDEM->is_InGrid(x, y) && m_Flats.asInt(x, y) != id && m_pDEM->asDouble(x, y) == z )
	{
		m_Stack.Push(x, y);

		m_Flats.Set_Value(x, y, id);

		if( m_pNoFlats )
		{
			m_pNoFlats->Set_NoData(x, y);
		}

		if( m_pFlats )
		{
			switch( m_Flat_Output )
			{
			default: m_pFlats->Set_Value(x, y, z ); break;
			case  1: m_pFlats->Set_Value(x, y, id); break;
			}
		}
	}
}

//---------------------------------------------------------
void CFlat_Detection::Process_Flat(int x, int y, int id)
{
	m_Stack.Clear();

	double z = m_pDEM->asDouble(x, y);

	Process_Cell(x, y, id, z);

	while( m_Stack.Get_Size() > 0 && Process_Get_Okay() )
	{
		m_Stack.Pop(x, y);

		for(int i=0; i<8; i+=m_Neighbour)
		{
			Process_Cell(Get_xTo(i, x), Get_yTo(i, y), id, z);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
