
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   grid_pyramids.cpp                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "grid_pyramid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_Pyramid::CSG_Grid_Pyramid(void)
{
	m_nLevels	= 0;
	m_pLevels	= NULL;
	m_pGrid		= NULL;
}

//---------------------------------------------------------
CSG_Grid_Pyramid::CSG_Grid_Pyramid(CSG_Grid *pGrid, double Grow, TSG_Grid_Pyramid_Generalisation Generalisation)
{
	m_nLevels	= 0;
	m_pLevels	= NULL;
	m_pGrid		= NULL;

	Create(pGrid, Grow, Generalisation);
}

//---------------------------------------------------------
CSG_Grid_Pyramid::~CSG_Grid_Pyramid(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Pyramid::Create(CSG_Grid *pGrid, double Grow, TSG_Grid_Pyramid_Generalisation Generalisation)
{
	if( pGrid && pGrid->is_Valid() && Grow > 1.0 && (pGrid->Get_NX() > Grow || pGrid->Get_NY() > Grow) )
	{
		Destroy();

		m_pGrid				= pGrid;
		m_Grow				= Grow;
		m_Generalisation	= Generalisation;

		_Get_Next_Level(pGrid);

		return( true );
	}

	return( false );	
}

//---------------------------------------------------------
bool CSG_Grid_Pyramid::Destroy(void)
{
	if( m_pLevels )
	{
		for(int i=0; i<m_nLevels; i++)
		{
			delete(m_pLevels[i]);
		}

		SG_Free(m_pLevels);

		m_nLevels	= 0;
		m_pLevels	= NULL;
		m_pGrid		= NULL;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Pyramid::_Get_Next_Level(CSG_Grid *pGrid)
{
	if( pGrid->Get_NX() > m_Grow || pGrid->Get_NY() > m_Grow )
	{
		int		nx, ny;
		double	d;

		d	= pGrid->Get_Cellsize() * m_Grow;
		nx	= (int)(1.5 + m_pGrid->Get_XRange() / d);	if( nx < 1 )	nx	= 1;
		ny	= (int)(1.5 + m_pGrid->Get_YRange() / d);	if( ny < 1 )	ny	= 1;

		if( nx > 1 || ny > 1 )
		{
			CSG_Grid	*pNext	= SG_Create_Grid(GRID_TYPE_Float, nx, ny, d, pGrid->Get_XMin(), pGrid->Get_YMin());

			pNext->Set_NoData_Value(pGrid->Get_NoData_Value());
			pNext->Assign(pGrid);

			m_pLevels	= (CSG_Grid **)SG_Realloc(m_pLevels, (m_nLevels + 1) * sizeof(CSG_Grid *));
			m_pLevels[m_nLevels++]	= pNext;

			_Get_Next_Level(pNext);

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
