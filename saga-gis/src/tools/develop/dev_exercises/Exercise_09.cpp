
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
//                   Exercise_09.cpp                     //
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
#include "Exercise_09.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_09::CExercise_09(void)
{
	Set_Name		(_TL("09: Extended neighbourhoods - catchment areas (recursive)"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Extended Neighbourhoods - Use recursive function calls for catchment area calculations."
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
		"", "ELEVATION"	, _TL("Elevation grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "AREA"		, _TL("Catchment area"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_09::On_Execute(void)
{
	//-----------------------------------------------------
	// Get parameter settings...

	CSG_Grid *pDTM = Parameters("ELEVATION")->asGrid();


	//-----------------------------------------------------
	// Initialisations...

	m_pArea = Parameters("AREA")->asGrid();

	m_pArea->Assign(0.);
	m_pArea->Set_Unit("m²");

	DataObject_Set_Colors(m_pArea, 11, SG_COLORS_WHITE_BLUE);


	//-----------------------------------------------------
	// Save flow directions to temporary grid...

	m_Dir.Create(Get_System(), SG_DATATYPE_Char);

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			m_Dir.Set_Value(x, y, pDTM->Get_Gradient_NeighborDir(x, y) % 8);
		}
	}


	//-------------------------------------------------
	// Execute calculation...

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Area(x, y);
		}
	}


	//-----------------------------------------------------
	// Finalisations...

	m_Dir.Destroy();


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CExercise_09::Get_Area(int x, int y)
{
	double	area	= m_pArea->asDouble(x, y);

	if( area <= 0.0 )												// cell has not been processed yet...
	{
		m_pArea->Set_Value(x, y, 1.0);								// Very important: mark this cell as processed to prevent endless loops...

		area	= Get_Cellsize() * Get_Cellsize();								// initialize the cell's area with its own cell size...

		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xFrom(i, x);
			int	iy	= Get_yFrom(i, y);

			if( is_InGrid(ix, iy) && i == m_Dir.asInt(ix, iy) )	// drains ith neighbour into this cell ???...
			{
				area	+= Get_Area(ix, iy);						// ...then add its area (recursive call of this function!)...
			}
		}

		m_pArea->Set_Value(x, y, area);
	}

	//-----------------------------------------------------
	return( area );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
