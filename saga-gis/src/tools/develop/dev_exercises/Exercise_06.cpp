
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
		"ediss.uni-goettingen.de.", SG_T("http://hdl.handle.net/11858/00-1735-0000-0006-B26C-6"), SG_T("Online")
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
	int		Radius;

	//-----------------------------------------------------
	// Get parameter settings...

	m_pInput	= Parameters("INPUT" )->asGrid();
	m_pOutput	= Parameters("OUTPUT")->asGrid();

	Radius		= Parameters("RADIUS")->asInt();


	//-----------------------------------------------------
	// Execute calculation...

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:
		return( Method_01(Radius) );

	case 1:
		return( Method_02(Radius) );

	case 2:
		return( Method_03(Radius) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_06::Method_01(int Radius)
{
	int		x, y, ix, iy, n;
	double	s;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			n	= 0;
			s	= 0.0;

			for(iy=y-Radius; iy<=y+Radius; iy++)
			{
				for(ix=x-Radius; ix<=x+Radius; ix++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) )
					{
						n++;
						s	+= m_pInput->asDouble(ix, iy);
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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_06::Method_02(int Radius)
{
	int		x, y, ix, iy, xMask, yMask, nMask, n;
	double	s, Distance;
	CSG_Grid	gMask;

	//-----------------------------------------------------
	nMask	= 1 + 2 * Radius;

	gMask.Create(SG_DATATYPE_Byte, nMask, nMask);

	for(iy=-Radius, yMask=0; yMask<nMask; iy++, yMask++)
	{
		for(ix=-Radius, xMask=0; xMask<nMask; ix++, xMask++)
		{
			Distance	= sqrt((double)(ix*ix + iy*iy));

			gMask.Set_Value(xMask, yMask, Distance <= Radius ? 1.0 : 0.0);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			n	= 0;
			s	= 0.0;

			for(iy=y-Radius, yMask=0; yMask<nMask; iy++, yMask++)
			{
				for(ix=x-Radius, xMask=0; xMask<nMask; ix++, xMask++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) && gMask.asByte(xMask, yMask) )
					{
						n++;
						s	+= m_pInput->asDouble(ix, iy);
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

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_06::Method_03(int Radius)
{
	int		x, y, ix, iy, xMask, yMask, nMask;
	double	s, n, Distance, Weight;
	CSG_Grid	gMask;

	//-----------------------------------------------------
	nMask	= 1 + 2 * Radius;

	gMask.Create(SG_DATATYPE_Double, nMask, nMask);

	for(iy=-Radius, yMask=0; yMask<nMask; iy++, yMask++)
	{
		for(ix=-Radius, xMask=0; xMask<nMask; ix++, xMask++)
		{
			Distance	= sqrt((double)(ix*ix + iy*iy));

			gMask.Set_Value(xMask, yMask, Distance > 0.0 && Distance <= Radius ? 1.0 / Distance : 0.0);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			n	= 0;
			s	= 0.0;

			for(iy=y-Radius, yMask=0; yMask<nMask; iy++, yMask++)
			{
				for(ix=x-Radius, xMask=0; xMask<nMask; ix++, xMask++)
				{
					if( is_InGrid(ix, iy) && !m_pInput->is_NoData(ix, iy) && (Weight = gMask.asDouble(xMask, yMask)) > 0.0 )
					{
						n	+= Weight;
						s	+= Weight * m_pInput->asDouble(ix, iy);
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

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
