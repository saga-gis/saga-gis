
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_FastSegments.cpp                 //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Grid_FastSegments.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_FastSegments::CGrid_FastSegments(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Grid Segmentation (b)"));

	Set_Author(_TL("Copyrights (c) 2006 by Olaf Conrad"));

	Set_Description(_TL(
		"Segmentation with the local minimum/maximum method."
	));


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SEGMENTS"	, _TL("Segments"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "OBJECTS"		, _TL("Features"),
		"",
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// 3. General Parameters...

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Choose if you want to segmentate either on minima or on maxima."),

		CSG_String::Format("%s|%s|",
			_TL("Minima"),
			_TL("Maxima")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "VALUES"		, _TL("Output"),
		_TL("The values of the resultant grid can be either the seed value (e.g. the local maximum) or the enumerated segment id."),

		CSG_String::Format("%s|%s|",
			_TL("Segment ID"),
			_TL("Seed Value")
		), 1
	);
}

//---------------------------------------------------------
CGrid_FastSegments::~CGrid_FastSegments(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_FastSegments::On_Execute(void)
{
	int		n, x, y;

	//-----------------------------------------------------
	m_pGrid		= Parameters("INPUT")		->asGrid();
	m_pSegments	= Parameters("SEGMENTS")	->asGrid();
	m_pObjects	= Parameters("OBJECTS")		->asGrid();

	m_bDown		= Parameters("METHOD")		->asInt() == 1;

	//-----------------------------------------------------
	m_nSegments	= 0;
	m_pSegments	->Assign(0.0);
	m_Values	= NULL;

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		m_pGrid->Get_Sorted(n, x, y, m_bDown);

		Set_Cell(x, y);
	}

	//-----------------------------------------------------
	if( m_nSegments > 0 )
	{
		if( Parameters("VALUES")->asInt() == 1 )
		{
			for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
			{
				if( (x = m_pSegments->asInt(n) - 1) >= 0 && x < m_nSegments )
				{
					m_pSegments->Set_Value(n, m_Values[x]);
				}
				else
				{
					m_pSegments->Set_NoData(n);
				}
			}
		}

		SG_Free(m_Values);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_FastSegments::Set_Cell(int x, int y)
{
	int		i, ix, iy, n, s, id;
	double	zMin;

	//-----------------------------------------------------
	if( !m_pGrid->is_InGrid(x, y) )
	{
		m_pSegments	->Set_NoData(x, y);
		m_pObjects	->Set_NoData(x, y);
	}

	//-----------------------------------------------------
	else
	{
		for(i=0, n=0, id=-1; i<8; i++)
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( m_pGrid->is_InGrid(ix, iy) )
			{
				if( (s = m_pSegments->asInt(ix, iy)) > 0 && s != id )
				{
					n++;

					if( n == 1 || ( m_bDown && zMin > m_pGrid->asDouble(ix, iy)) || (!m_bDown && zMin < m_pGrid->asDouble(ix, iy)) )
				//	if( n == 1 || ( m_bDown && zMin > m_Values[id - 1]) || (!m_bDown && zMin < m_Values[id - 1]) )
					{
						id		= s;
						zMin	= m_pGrid->asDouble(ix, iy);
					}
				}
			}
		}

		//-----------------------------------------------------
		switch( n )
		{
		case 0:
			id					= ++m_nSegments;
			m_Values			= (double *)SG_Realloc(m_Values, m_nSegments * sizeof(double));
			m_Values[id - 1]	= m_pGrid->asDouble(x, y);

			m_pSegments	->Set_Value	(x, y, id);
			m_pObjects	->Set_Value	(x, y, 2);
			break;

		case 1:
			m_pSegments	->Set_Value	(x, y, id);
			m_pObjects	->Set_NoData(x, y);
			break;

		default:
			m_pSegments	->Set_Value	(x, y, id);
			m_pObjects	->Set_Value	(x, y, 1);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
