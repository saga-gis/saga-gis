/**********************************************************
 * Version $Id: Flat_Detection.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "NOFLATS"		, _TL("No Flats"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "FLATS"		, _TL("Flat Areas"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL, "FLAT_OUTPUT"	, _TL("Flat Area Values"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("elevation"),
			_TL("enumeration")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlat_Detection::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM			= Parameters("DEM")			->asGrid();
	m_pNoFlats		= Parameters("NOFLATS")		->asGrid();
	m_pFlats		= Parameters("FLATS")		->asGrid();
	m_Flat_Output	= Parameters("FLAT_OUTPUT")	->asInt();

	//-----------------------------------------------------
	if( m_pNoFlats )
	{
		m_pNoFlats	->Assign(m_pDEM);
		m_pNoFlats	->Set_Name("%s [%s]", m_pDEM->Get_Name(), _TL("No Flats"));
	}

	if( m_pFlats )
	{
		m_pFlats	->Assign_NoData();
		m_pFlats	->Set_Name("%s [%s]", m_pDEM->Get_Name(), _TL("Flats"));
	}

	m_Flats.Create(Get_System(), SG_DATATYPE_Int);

	m_Flats.Assign(0.0);
	m_nFlats	= 0;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_Flats.asInt(x, y) && is_Flat(x, y) )
			{
				Set_Flat(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Stack.Destroy();
	m_Flats.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlat_Detection::is_Flat(int x, int y)
{
	if( m_pDEM->is_NoData(x, y) )
	{
		return( false );
	}

	int		i, ix, iy, n;
	double	z;

	for(i=0, n=0, z=m_pDEM->asDouble(x, y); i<8; i++)
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_pDEM->is_InGrid(ix, iy) )
		{
			if( z != m_pDEM->asDouble(ix, iy) )
			{
				return( false );
			}

			n++;
		}
	}

	return( n > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlat_Detection::Set_Flat_Cell(int x, int y)
{
	if( m_pDEM->is_InGrid(x, y) && m_Flats.asInt(x, y) != m_nFlats && m_zFlat == m_pDEM->asDouble(x, y) )
	{
		m_Stack.Push(x, y);

		m_Flats.Set_Value(x, y, m_nFlats);

		if( m_pNoFlats )
		{
			m_pNoFlats->Set_NoData(x, y);
		}

		if( m_pFlats )
		{
			switch( m_Flat_Output )
			{
			default:
			case 0:	m_pFlats->Set_Value(x, y, m_zFlat);		break;
			case 1: m_pFlats->Set_Value(x, y, m_nFlats);	break;
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlat_Detection::Set_Flat(int x, int y)
{
	m_zFlat		= m_pDEM->asDouble(x, y);
	m_nFlats	++;

	m_Stack.Clear();

	Set_Flat_Cell(x, y);

	//-----------------------------------------------------
	while( m_Stack.Get_Size() > 0 && Process_Get_Okay() )
	{
		m_Stack.Pop(x, y);

		for(int i=0; i<8; i++)
		{
			Set_Flat_Cell(Get_xTo(i, x), Get_yTo(i, y));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
