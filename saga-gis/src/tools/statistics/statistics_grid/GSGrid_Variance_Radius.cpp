/**********************************************************
 * Version $Id: GSGrid_Variance_Radius.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               GSGrid_Variance_Radius.cpp              //
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
#include "GSGrid_Variance_Radius.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Variance_Radius::CGSGrid_Variance_Radius(void)
{
	Set_Name		(_TL("Radius of Variance (Grid)"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Find the radius within which the cell values exceed the given variance criterium. "
		"This tool is closely related to the representativeness calculation "
		"(variance within given search radius). "
		"For easier usage, the variance criterium is entered as standard deviation value. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		""	, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		""	, "RESULT"	, _TL("Radius"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		""	, "STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Int(
		""	, "RADIUS"	, _TL("Maximum Search Radius (cells)"),
		_TL(""),
		20, 1, true
	);

	Parameters.Add_Choice(
		""	, "OUTPUT"	, _TL("Type of Output"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Cells"),
			_TL("Map Units")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Variance_Radius::On_Execute(void)
{
	if( !m_Kernel.Set_Radius(Parameters("RADIUS")->asInt()) )
	{
		return( false );
	}

	m_pGrid		= Parameters("INPUT")->asGrid();

	m_StdDev	= Parameters("STDDEV")->asDouble();

	double	Scale	= Parameters("OUTPUT")->asInt() == 0 ? 1.0 : Get_Cellsize();

	CSG_Grid	*pResult	= Parameters("RESULT")->asGrid();

	pResult->Fmt_Name("%s >= %f", _TL("Radius with Standard Deviation"), m_StdDev);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Radius;

			if( Get_Radius(x, y, Radius) )
			{
				pResult->Set_Value(x, y, Scale * Radius);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Variance_Radius::Get_Radius(int x, int y, double &Radius)
{
	if( !m_pGrid->is_NoData(x, y) )
	{
		CSG_Simple_Statistics	s;

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pGrid->is_InGrid(ix, iy) )
			{
				s	+= m_pGrid->asDouble(ix, iy);

				if( s.Get_StdDev() >= m_StdDev )
				{
					Radius	= m_Kernel.Get_Distance(i);

					return( true );
				}
			}
		}

		if( s.Get_Count() > 0 )
		{
			Radius	= m_Kernel.Get_Radius();

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
