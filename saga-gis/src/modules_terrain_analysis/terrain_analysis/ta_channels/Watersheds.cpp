
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Watersheds.cpp                     //
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
#include "Watersheds.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWatersheds::CWatersheds(void)
{
	Set_Name(_TL("Watershed Basins"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description(_TL(""));

	Parameters.Add_Grid(
		NULL, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "CHANNELS"	, _TL("Channel Network"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "SINKROUTE"	, _TL("Sink Route"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "BASINS"		, _TL("Watershed Basins"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "MINSIZE"		, _TL("Min. Size"),
		_TL("Minimum size of basin (cells)"),
		PARAMETER_TYPE_Int
	);
}

//---------------------------------------------------------
CWatersheds::~CWatersheds(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	NO_BASIN	-1

//---------------------------------------------------------
bool CWatersheds::On_Execute(void)
{
	int		x, y, n, nCells, nCells_Min, nBasins;
	CSG_Grid	*pDTM, *pSeed, *pRoute;

	//-----------------------------------------------------
	pDTM		= Parameters("ELEVATION")	->asGrid();
	pSeed		= Parameters("CHANNELS")	->asGrid();
	pRoute		= Parameters("SINKROUTE")	->asGrid();
	nCells_Min	= Parameters("MINSIZE")		->asInt();
	m_pBasins	= Parameters("BASINS")		->asGrid();

	m_pBasins->Set_NoData_Value(NO_BASIN);
	m_pBasins->Assign_NoData();

	m_Direction.Create(m_pBasins, SG_DATATYPE_Char);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pDTM->is_NoData(x, y) )
			{
				m_Direction.Set_NoData(x, y);
			}
			else
			{
				if( !pRoute || (n = pRoute->asChar(x, y)) <= 0 )
				{
					n	= pDTM->Get_Gradient_NeighborDir(x, y);
				}

				m_Direction.Set_Value(x, y, n < 0 ? -1 : (n + 4) % 8);
			}
		}
	}

	//-----------------------------------------------------
	for(n=0, m_nBasins=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		pDTM->Get_Sorted(n, x, y, true, false);

		if( !pSeed->is_NoData(x, y) && pSeed->asInt(x, y) < 0 )
		{
			m_nBasins++;

			if( (nCells = Get_Basin(x, y)) < nCells_Min )
			{
				nBasins		= m_nBasins - 1;
				m_nBasins	= NO_BASIN;
				Get_Basin(x, y);
				m_nBasins	= nBasins;
			}
		}
	}

	//-----------------------------------------------------
	m_Direction.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWatersheds::Get_Basin(int x, int y)
{
	int		i, ix, iy, nCells;

	if( m_pBasins->is_NoData(x, y) && !m_Direction.is_NoData(x, y) )
	{
		m_pBasins->Set_Value(x, y, m_nBasins);

		for(i=0, nCells=1; i<8; i++)
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( is_InGrid(ix,iy) && m_Direction.asInt(ix, iy) == i )
			{
				nCells	+= Get_Basin(ix, iy);
			}
		}

		return( nCells );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
