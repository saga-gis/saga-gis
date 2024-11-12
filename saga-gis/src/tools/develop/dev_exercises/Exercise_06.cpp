
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
//                   Exercise_06.cpp                     //
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
#include "Exercise_06.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_06::CExercise_06(void)
{
	Set_Name		(_TL("06: Extended neighbourhoods"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Extended neigbourhoods for grids."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems für Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.",
		SG_T("https://ediss.uni-goettingen.de/handle/11858/00-1735-0000-0006-B26C-6"), SG_T("online")
	);

	Add_Reference("O. Conrad, B. Bechtel, M. Bock, H. Dietrich, E. Fischer, L. Gerlitz, J. Wehberg, V. Wichmann, and J. Böhner", "2015",
		"System for Automated Geoscientific Analyses (SAGA) v. 2.1.4",
		"Geoscientific Model Development, 8, 1991-2007.",
		SG_T("https://doi.org/10.5194/gmd-8-1991-2015"), SG_T("doi:10.5194/gmd-8-1991-2015")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"		, _TL("Output grid"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		"", "RADIUS"		, _TL("Radius"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Choice(
		"", "METHOD"		, _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s|%s",
			_TL("Quadratic"),
			_TL("Circle"),
			_TL("Distance Weighted (inverse distance)")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_06::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	m_pInput  = Parameters("INPUT" )->asGrid();
	m_pOutput = Parameters("OUTPUT")->asGrid();


	//-----------------------------------------------------
	// Execute calculation...

	switch( Parameters("METHOD")->asInt() )
	{
	case 0: return( Method_01(Parameters("RADIUS")->asInt()) );
	case 1: return( Method_02(Parameters("RADIUS")->asInt()) );
	case 2: return( Method_03(Parameters("RADIUS")->asInt()) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_06::Method_01(int Radius)
{
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int n = 0; double s = 0.;

			for(int iy=y-Radius; iy<=y+Radius; iy++)
			{
				for(int ix=x-Radius; ix<=x+Radius; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						n++; s += m_pInput->asDouble(ix, iy);
					}
				}
			}

			if( n > 0 )
			{
				m_pOutput->Set_Value(x, y, s / n);
			}
			else
			{
				m_pOutput->Set_NoData(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CExercise_06::Method_02(int Radius)
{
	CSG_Grid Mask(SG_DATATYPE_Byte, 1 + 2 * Radius, 1 + 2 * Radius);

	for(int iy=-Radius, yMask=0; yMask<Mask.Get_NY(); iy++, yMask++)
	{
		for(int ix=-Radius, xMask=0; xMask<Mask.Get_NX(); ix++, xMask++)
		{
			double Distance = sqrt(((double)ix*ix + (double)iy*iy));

			Mask.Set_Value(xMask, yMask, Distance <= Radius ? 1. : 0.);
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int n = 0; double s = 0.;

			for(int iy=y-Radius, yMask=0; yMask<Mask.Get_NY(); iy++, yMask++)
			{
				for(int ix=x-Radius, xMask=0; xMask<Mask.Get_NX(); ix++, xMask++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) && Mask.asByte(xMask, yMask) )
					{
						n++; s += m_pInput->asDouble(ix, iy);
					}
				}
			}

			if( n > 0 )
			{
				m_pOutput->Set_Value(x, y, s / n);
			}
			else
			{
				m_pOutput->Set_NoData(x, y);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CExercise_06::Method_03(int Radius)
{
	CSG_Grid Weight(SG_DATATYPE_Double, 1 + 2 * Radius, 1 + 2 * Radius);

	for(int iy=-Radius, wy=0; wy<Weight.Get_NY(); iy++, wy++)
	{
		for(int ix=-Radius, wx=0; wx<Weight.Get_NX(); ix++, wx++)
		{
			double Distance = sqrt(((double)ix*ix + (double)iy*iy));

			Weight.Set_Value(wx, wy, Distance > Radius ? 0. : 1. / Distance);
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics Statistics;

			for(int iy=y-Radius, wy=0; wy<Weight.Get_NY(); iy++, wy++)
			{
				for(int ix=x-Radius, wx=0; wx<Weight.Get_NX(); ix++, wx++)
				{
					if( m_pInput->is_InGrid(ix, iy) )
					{
						Statistics.Add_Value(m_pInput->asDouble(ix, iy), Weight.asDouble(wx, wy));
					}
				}
			}

			if( Statistics.Get_Count() > 0 )
			{
				m_pOutput->Set_Value(x, y, Statistics.Get_Mean());
			}
			else
			{
				m_pOutput->Set_NoData(x, y);
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
