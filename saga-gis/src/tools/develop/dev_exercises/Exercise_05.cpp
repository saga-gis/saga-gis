
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
//                   Exercise_05.cpp                     //
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
#include "Exercise_05.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_05::CExercise_05(void)
{
	Set_Name		(_TL("05: Direct neighbours - slope and aspect"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Simple neighbourhood analysis for grid cells."
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

	Add_Reference("Zevenbergen, L.W. & Thorne, C.R.", "1987",
		"Quantitative analysis of land surface topography",
		"Earth Surface Processes and Landforms, 12: 47-56."
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ELEVATION", _TL("Input grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "SLOPE"    , _TL("Slope"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "ASPECT"   , _TL("Aspect"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		"", "METHOD"   , _TL("Method"),
		_TL("Choose a method"),
		CSG_String::Format("%s|%s|%s",
			_TL("Steepest gradient (first version)"),
			_TL("Steepest gradient (second version)"),
			_TL("Zevenbergen & Thorne")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_05::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	m_pDTM    = Parameters("ELEVATION")->asGrid();
	m_pSlope  = Parameters("SLOPE"    )->asGrid();
	m_pAspect = Parameters("ASPECT"   )->asGrid();

	m_pSlope ->Set_Unit(_TL("radians"));
	m_pAspect->Set_Unit(_TL("radians"));


	//-----------------------------------------------------
	// Execute calculation...

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pDTM->is_NoData(x, y) )
			{
				m_pSlope ->Set_NoData(x, y);
				m_pAspect->Set_NoData(x, y);

				continue;
			}

			switch( Parameters("METHOD")->asInt() )
			{
			case  0: Method_01(x, y); break;
			case  1: Method_02(x, y); break;
			default: Method_03(x, y); break;
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_05::Method_01(int x, int y)
{
	const double dx[2] = { Get_Cellsize(), sqrt(2.) * Get_Cellsize() };

	int iMax = -1; double dzMax = 0., z = m_pDTM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

		if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
		{
			double dz = (z - m_pDTM->asDouble(ix, iy)) / dx[i % 2];

			if( dzMax < dz )
			{
				dzMax = dz; iMax = i;						
			}
		}
	}

	if( iMax < 0 )
	{
		m_pSlope ->Set_Value(x, y, 0.);
		m_pAspect->Set_NoData(x, y);
	}
	else
	{
		m_pSlope ->Set_Value(x, y, atan(dzMax));
		m_pAspect->Set_Value(x, y, M_PI_045 * iMax);
	}

	return( true );
}

//---------------------------------------------------------
bool CExercise_05::Method_02(int x, int y)
{
	int iMax = -1; double dzMax = 0., z = m_pDTM->asDouble(x, y);

	for(int i=0; i<8; i++)
	{
		int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

		if( is_InGrid(ix, iy) && !m_pDTM->is_NoData(ix, iy) )
		{
			double dz = (z - m_pDTM->asDouble(ix, iy)) / Get_Length(i);

			if( dzMax < dz )
			{
				dzMax = dz; iMax = i;
			}
		}
	}

	if( iMax < 0 )
	{
		m_pSlope ->Set_Value(x, y, 0.);
		m_pAspect->Set_NoData(x, y);
	}
	else
	{
		m_pSlope ->Set_Value(x, y, atan(dzMax));
		m_pAspect->Set_Value(x, y, M_PI_045 * iMax);
	}

	return( true );
}

//---------------------------------------------------------
bool CExercise_05::Method_03(int x, int y)
{
	static int x_To[] = { 0, 1, 0, -1 };
	static int y_To[] = { 1, 0, -1, 0 };

	double dz[4], z = m_pDTM->asDouble(x, y);

	for(int i=0; i<4; i++)
	{
		int ix = x + x_To[i], iy = y + y_To[i];

		dz[i] = m_pDTM->is_InGrid(ix, iy) ? m_pDTM->asDouble(ix, iy) - z : 0.;
	}

	//-----------------------------------------
	double G = (dz[0] - dz[2]) / (2. * Get_Cellsize());
	double H = (dz[1] - dz[3]) / (2. * Get_Cellsize());

	m_pSlope->Set_Value(x, y, atan(sqrt(G*G + H*H)));

	if( G != 0. )
	{
		m_pAspect->Set_Value(x, y, M_PI_180 + atan2(H, G));
	}
	else if( H > 0. )
	{
		m_pAspect->Set_Value(x, y, M_PI_270);
	}
	else if( H < 0. )
	{
		m_pAspect->Set_Value(x, y, M_PI_090);
	}
	else
	{
		m_pAspect->Set_NoData(x, y);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
