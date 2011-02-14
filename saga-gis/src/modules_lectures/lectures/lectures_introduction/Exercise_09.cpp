/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
#include "Exercise_09.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_09::CExercise_09(void)
{
	//-----------------------------------------------------
	// Give some information about your module...

	Set_Name	(_TL("09: Extended neighbourhoods - catchment areas (recursive)"));

	Set_Author	(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description	(_TW(
		"Extended Neighbourhoods - Use recursive function calls for catchment area calculations.\n"
		"(c) 2003 by Olaf Conrad, Goettingen\n"
		"email: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Define your parameters list...

	Parameters.Add_Grid(
		NULL, "ELEVATION"	, _TL("Elevation grid"),
		_TL("This must be your input data of type grid."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "AREA"		, _TL("Catchment area"),
		_TL("This will contain your output data of type grid."),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CExercise_09::~CExercise_09(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_09::On_Execute(void)
{
	int			x, y;
	CSG_Grid	*pDTM;

	//-----------------------------------------------------
	// Get parameter settings...

	pDTM		= Parameters("ELEVATION")->asGrid();
	m_pArea		= Parameters("AREA"     )->asGrid();


	//-----------------------------------------------------
	// Initialisations...

	m_pArea		->Assign(0.0);
	m_pArea		->Set_Unit(SG_T("m\xc2\xb2"));
	DataObject_Set_Colors(m_pArea, 100, SG_COLORS_WHITE_BLUE);


	//-----------------------------------------------------
	// Save flow directions to temporary grid...

	m_pDir		= new CSG_Grid(pDTM, SG_DATATYPE_Char);	// this object has to be deleted later...

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			m_pDir->Set_Value(x, y, pDTM->Get_Gradient_NeighborDir(x, y) % 8);
		}
	}


	//-------------------------------------------------
	// Execute calculation...

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Get_Area(x, y);
		}
	}


	//-----------------------------------------------------
	// Finalisations...

	delete(m_pDir);


	//-----------------------------------------------------
	// Return 'true' if everything went okay...

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CExercise_09::Get_Area(int x, int y)
{
	int		i, ix, iy;
	double	area;

	//-----------------------------------------------------
	area	= m_pArea->asDouble(x, y);

	if( area <= 0.0 )												// cell has not been processed yet...
	{
		m_pArea->Set_Value(x, y, 1.0);								// Very important: mark this cell as processed to prevent endless loops...

		area	= Get_Cellsize() * Get_Cellsize();								// initialize the cell's area with its own cell size...

		for(i=0; i<8; i++)
		{
			ix	= Get_xFrom(i, x);
			iy	= Get_yFrom(i, y);

			if( is_InGrid(ix, iy) && i == m_pDir->asInt(ix, iy) )	// drains ith neigbour into this cell ???...
			{
				area	+= Get_Area(ix, iy);						// ...then add its area (recursive call of this function!)...
			}
		}

		m_pArea->Set_Value(x, y, area);
	}

	//-----------------------------------------------------
	return( area );
}
