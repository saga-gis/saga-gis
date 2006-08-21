
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
//                  mat_grid_radius.cpp                  //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//						Grid Radius						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mat_tools.h"
#include "grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_Radius::CSG_Grid_Radius(int maxRadius)
{
	m_maxRadius		= 0;

	Create(maxRadius);
}

//---------------------------------------------------------
CSG_Grid_Radius::~CSG_Grid_Radius(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid_Radius::Create(int maxRadius)
{
	int		x, y, nMask, ix, iy, Radius;
	double	dRadius;
	CGrid	gMask;

	Destroy();

	//-----------------------------------------------------
	if( maxRadius > 0 )
	{
		m_maxRadius	= maxRadius;

		m_nPoints	= (int *)				SG_Calloc(m_maxRadius, sizeof(int));
		m_Points	= (TSG_Grid_Radius **)	SG_Calloc(m_maxRadius, sizeof(TSG_Grid_Radius *));

		nMask		= 1 + 2 * m_maxRadius;
		gMask.Create(GRID_TYPE_Double, nMask, nMask);

		for(iy=-m_maxRadius, y=0; y<nMask; iy++, y++)
		{
			for(ix=-m_maxRadius, x=0; x<nMask; ix++, x++)
			{
				gMask.Set_Value(x, y, dRadius = M_GET_LENGTH(ix, iy));

				if( (Radius = (int)dRadius) < m_maxRadius )
				{
					m_nPoints[Radius]++;
				}
			}
		}

		//-------------------------------------------------
		for(Radius=0; Radius<m_maxRadius; Radius++)
		{
			m_Points[Radius]	= (TSG_Grid_Radius *)SG_Calloc(m_nPoints[Radius], sizeof(TSG_Grid_Radius));
			m_nPoints[Radius]	= 0;
		}

		//-------------------------------------------------
		for(y=0; y<nMask; y++)
		{
			for(x=0; x<nMask; x++)
			{
				if( (Radius = gMask.asInt(x, y)) < m_maxRadius )
				{
					m_Points[Radius][m_nPoints[Radius]].x	= x - m_maxRadius;
					m_Points[Radius][m_nPoints[Radius]].y	= y - m_maxRadius;
					m_Points[Radius][m_nPoints[Radius]].d	= gMask.asDouble(x, y);

					m_nPoints[Radius]++;
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Grid_Radius::Destroy(void)
{
	if( m_maxRadius > 0 )
	{
		for(int i=0; i<m_maxRadius; i++)
		{
			if( m_Points[i] )
			{
				SG_Free(m_Points[i]);
			}
		}

		SG_Free(m_Points);
		SG_Free(m_nPoints);

		m_maxRadius	= 0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
