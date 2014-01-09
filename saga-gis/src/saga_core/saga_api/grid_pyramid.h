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
//                    grid_pyramids.h                    //
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
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#ifndef HEADER_INCLUDED__SAGA_API__grid_pyramid_H
#define HEADER_INCLUDED__SAGA_API__grid_pyramid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grid_Pyramid_Generalisation
{
	GRID_PYRAMID_Mean	= 0,
	GRID_PYRAMID_Max,
	GRID_PYRAMID_Min,
	GRID_PYRAMID_MaxCount
}
TSG_Grid_Pyramid_Generalisation;

//---------------------------------------------------------
typedef enum ESG_Grid_Pyramid_Grow_Type
{
	GRID_PYRAMID_Arithmetic	= 0,
	GRID_PYRAMID_Geometric
}
TSG_Grid_Pyramid_Grow_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grid_Pyramid
{
public:
	CSG_Grid_Pyramid(void);

										CSG_Grid_Pyramid	(class CSG_Grid *pGrid, double Grow = 2.0, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);
	bool								Create				(class CSG_Grid *pGrid, double Grow = 2.0, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);

										CSG_Grid_Pyramid	(class CSG_Grid *pGrid, double Grow, double Start, int nMaxLevels, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);
	bool								Create				(class CSG_Grid *pGrid, double Grow, double Start, int nMaxLevels, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);

	virtual ~CSG_Grid_Pyramid(void);

	bool								Destroy				(void);


	int									Get_Count			(void)			{	return( m_nLevels );	}
	class CSG_Grid *					Get_Grid			(int iLevel)	{	return( iLevel >= 0 && iLevel < m_nLevels ? m_pLevels[iLevel] : m_pGrid );	}


private:

	int									m_nLevels, m_nMaxLevels;

	double								m_Grow;

	TSG_Grid_Pyramid_Generalisation		m_Generalisation;

	TSG_Grid_Pyramid_Grow_Type			m_Grow_Type;

	class CSG_Grid						**m_pLevels, *m_pGrid;


	bool								_Get_Next_Level		(class CSG_Grid *pGrid);
	bool								_Get_Next_Level		(class CSG_Grid *pGrid, double Cellsize);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__grid_pyramid_H
