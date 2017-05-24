/**********************************************************
 * Version $Id$
 *********************************************************/

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
//          Copyright (C) 2009 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
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
CSG_Grid_Pyramid::CSG_Grid_Pyramid(CSG_Grid *pGrid, double Grow, TSG_Grid_Pyramid_Generalisation Generalisation, TSG_Grid_Pyramid_Grow_Type Grow_Type)
{
	m_nLevels	= 0;
	m_pLevels	= NULL;
	m_pGrid		= NULL;

	Create(pGrid, Grow, Generalisation, Grow_Type);
}

//---------------------------------------------------------
CSG_Grid_Pyramid::CSG_Grid_Pyramid(CSG_Grid *pGrid, double Grow, double Start, int nMaxLevels, TSG_Grid_Pyramid_Generalisation Generalisation, TSG_Grid_Pyramid_Grow_Type Grow_Type)
{
	m_nLevels	= 0;
	m_pLevels	= NULL;
	m_pGrid		= NULL;

	Create(pGrid, Grow, Start, nMaxLevels, Generalisation, Grow_Type);
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
bool CSG_Grid_Pyramid::Create(CSG_Grid *pGrid, double Grow, TSG_Grid_Pyramid_Generalisation Generalisation, TSG_Grid_Pyramid_Grow_Type Grow_Type)
{
	if( pGrid && pGrid->is_Valid() && Grow > 1.0 && (pGrid->Get_NX() > Grow || pGrid->Get_NY() > Grow) )
	{
		Destroy();

		m_Grow_Type			= Grow_Type;
		m_nMaxLevels		= 0;
		m_pGrid				= pGrid;
		m_Grow				= Grow;
		m_Generalisation	= Generalisation;

		_Get_Next_Level(pGrid);

		return( true );
	}

	return( false );	
}

//---------------------------------------------------------
bool CSG_Grid_Pyramid::Create(CSG_Grid *pGrid, double Grow, double Start, int nMaxLevels, TSG_Grid_Pyramid_Generalisation Generalisation, TSG_Grid_Pyramid_Grow_Type Grow_Type)
{
	if( pGrid && pGrid->is_Valid() && Grow > 0.0 && (pGrid->Get_NX() > Grow || pGrid->Get_NY() > Grow) )
	{
		Destroy();

		m_Grow_Type			= Grow_Type;
		m_nMaxLevels		= nMaxLevels;
		m_pGrid				= pGrid;
		m_Grow				= Grow;
		m_Generalisation	= Generalisation;

		if( Start > 0.0 )
		{
			_Get_Next_Level(pGrid, Start);
		}
		else
		{
			_Get_Next_Level(pGrid);
		}

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
	if( (m_nMaxLevels <= 0 || m_nLevels < m_nMaxLevels) )
	{
		int		nx, ny;
		double	Cellsize;

		switch( m_Grow_Type )
		{
		case GRID_PYRAMID_Arithmetic:	Cellsize	= pGrid->Get_Cellsize() + m_Grow;	break;
		case GRID_PYRAMID_Geometric:	Cellsize	= pGrid->Get_Cellsize() * m_Grow;	break;
		default: Cellsize	= pGrid->Get_Cellsize() * m_Grow;	break;
		}

		nx	= (int)(1.5 + m_pGrid->Get_XRange() / Cellsize);	if( nx < 1 )	nx	= 1;
		ny	= (int)(1.5 + m_pGrid->Get_YRange() / Cellsize);	if( ny < 1 )	ny	= 1;

		if( nx > 1 || ny > 1 )
		{
			CSG_Grid	*pNext	= SG_Create_Grid(SG_DATATYPE_Float, nx, ny, Cellsize, pGrid->Get_XMin(), pGrid->Get_YMin());

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

//---------------------------------------------------------
bool CSG_Grid_Pyramid::_Get_Next_Level(CSG_Grid *pGrid, double Cellsize)
{
	if( (m_nMaxLevels <= 0 || m_nLevels < m_nMaxLevels) )
	{
		int		nx, ny;

		nx	= (int)(1.5 + m_pGrid->Get_XRange() / Cellsize);	if( nx < 1 )	nx	= 1;
		ny	= (int)(1.5 + m_pGrid->Get_YRange() / Cellsize);	if( ny < 1 )	ny	= 1;

		if( nx > 1 || ny > 1 )
		{
			CSG_Grid	*pNext	= SG_Create_Grid(SG_DATATYPE_Float, nx, ny, Cellsize, pGrid->Get_XMin(), pGrid->Get_YMin());

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
