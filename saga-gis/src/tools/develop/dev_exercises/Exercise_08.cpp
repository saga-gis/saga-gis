
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_08.cpp                     //
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
#include "Exercise_08.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_08::CExercise_08(void)
{
	Set_Name		(_TL("08: Extended neighbourhoods - catchment areas (parallel)"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Extended Neighbourhoods - Catchment areas."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems für Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ELEVATION"	, _TL("Elevation grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "AREA"		, _TL("Catchment area"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "METHOD"		, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s",
			_TL("D8"),
			_TL("MFD")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_08::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	m_pDTM	= Parameters("ELEVATION")->asGrid();

	m_pArea	= Parameters("AREA")->asGrid();

	m_pArea->Assign(0.);
	m_pArea->Set_Unit("m^2");

	DataObject_Set_Colors(m_pArea, 100, SG_COLORS_WHITE_BLUE);


	//-----------------------------------------------------
	// Execute calculation...

	switch( Parameters("METHOD")->asInt() )
	{
	default: return( Method_01() );
	case  1: return( Method_02() );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_08::Method_01(void)
{
	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int	x, y; m_pDTM->Get_Sorted(n, x, y, true, false);

		if( m_pDTM->is_NoData(x, y) )
		{
			m_pArea->Set_NoData(x, y);
		}
		else
		{
			m_pArea->Add_Value(x, y, Get_Cellarea());

			double z = m_pDTM->asDouble(x, y), dzMax = 0.; int iMax = -1;

			for(int i=0; i<8; i++)
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
				{
					double	dz	= (z - m_pDTM->asDouble(ix, iy)) / Get_Length(i);

					if( dz > 0. && (iMax < 0 || (iMax >= 0 && dzMax < dz)) )
					{
						iMax	= i;
						dzMax	= dz;
					}
				}
			}

			if( iMax >= 0 )
			{
				int ix	= Get_xTo(iMax, x);
				int	iy	= Get_yTo(iMax, y);

				m_pArea->Add_Value(ix, iy, m_pArea->asDouble(x, y));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_08::Method_02(void)
{
	const double	MFD_Converge	= 1.1;

	for(sLong n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		int x, y; m_pDTM->Get_Sorted(n, x, y, true, false);

		if( m_pDTM->is_NoData(x, y) )
		{
			m_pArea->Set_NoData(x, y);
		}
		else
		{
			m_pArea->Add_Value(x, y, Get_Cellarea());

			double z = m_pDTM->asDouble(x, y), d, dz[8], dzSum = 0.;

			for(int i=0; i<8; i++)
			{
				int	ix	= Get_xTo(i, x);
				int	iy	= Get_yTo(i, y);

				if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) && (d = z - m_pDTM->asDouble(ix, iy)) > 0. )
				{
					dz[i]	= pow(d / Get_Length(i), MFD_Converge);
					dzSum	+= dz[i];
				}
				else
				{
					dz[i]	= 0.;
				}
			}

			if( dzSum > 0. )
			{
				d	= m_pArea->asDouble(x, y) / dzSum;

				for(int i=0; i<8; i++)
				{
					if( dz[i] > 0. )
					{
						int	ix	= Get_xTo(i, x);
						int	iy	= Get_yTo(i, y);

						m_pArea->Add_Value(ix, iy, dz[i] * d);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
