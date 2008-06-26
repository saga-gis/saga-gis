
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    grid_multilevel                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    grid_pyramids.h                    //
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
#ifndef HEADER_INCLUDED__grid_pyramid_H
#define HEADER_INCLUDED__grid_pyramid_H

//---------------------------------------------------------
#include "MLB_Interface.h"


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
class CSG_Grid_Pyramid
{
public:
	CSG_Grid_Pyramid(void);

	CSG_Grid_Pyramid									(CSG_Grid *pGrid, double Grow = 2.0, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);
	bool								Create			(CSG_Grid *pGrid, double Grow = 2.0, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);

	CSG_Grid_Pyramid									(CSG_Grid *pGrid, double Grow, double Start, int nMaxLevels, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);
	bool								Create			(CSG_Grid *pGrid, double Grow, double Start, int nMaxLevels, TSG_Grid_Pyramid_Generalisation Generalisation = GRID_PYRAMID_Mean, TSG_Grid_Pyramid_Grow_Type Grow_Type = GRID_PYRAMID_Geometric);

	virtual ~CSG_Grid_Pyramid(void);

	bool								Destroy			(void);


	int									Get_Count		(void)			{	return( m_nLevels );	}
	CSG_Grid *							Get_Grid		(int iLevel)	{	return( iLevel >= 0 && iLevel < m_nLevels ? m_pLevels[iLevel] : m_pGrid );	}


private:

	int									m_nLevels, m_nMaxLevels;

	double								m_Grow;

	TSG_Grid_Pyramid_Generalisation		m_Generalisation;

	TSG_Grid_Pyramid_Grow_Type			m_Grow_Type;

	CSG_Grid							**m_pLevels, *m_pGrid;


	bool								_Get_Next_Level	(CSG_Grid *pGrid);
	bool								_Get_Next_Level	(CSG_Grid *pGrid, double Cellsize);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__grid_pyramid_H
