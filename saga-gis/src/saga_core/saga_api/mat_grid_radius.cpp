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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	m_maxRadius	= 0;
	m_nPoints	= 0;
	m_Points	= NULL;
	m_nPoints_R	= NULL;
	m_Points_R	= NULL;

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
	Destroy();

	//-----------------------------------------------------
	if( maxRadius > 0 && maxRadius != m_maxRadius )
	{
		int		x, y, i, n;
		double	d;

		m_maxRadius	= maxRadius;

		m_nPoints_R	= (int *)SG_Calloc(m_maxRadius + 1, sizeof(int));

		for(y=-m_maxRadius; y<=m_maxRadius; y++)
		{
			for(x=-m_maxRadius; x<=m_maxRadius; x++)
			{
				if( (d = M_GET_LENGTH(x, y)) <= m_maxRadius )
				{
					m_nPoints++;
					m_nPoints_R[(int)d]++;
				}
			}
		}

		//-------------------------------------------------
		if( m_nPoints > 0 )
		{
			m_Points	= (TSG_Grid_Radius  *)SG_Calloc(m_nPoints      , sizeof(TSG_Grid_Radius  ));
			m_Points_R	= (TSG_Grid_Radius **)SG_Calloc(m_maxRadius + 1, sizeof(TSG_Grid_Radius *));

			for(i=0, n=0; i<=m_maxRadius; i++)
			{
				m_Points_R [i]	 = m_Points + n;
				n				+= m_nPoints_R[i];
				m_nPoints_R[i]	 = 0;
			}

			//---------------------------------------------
			for(y=-m_maxRadius; y<=m_maxRadius; y++)
			{
				for(x=-m_maxRadius; x<=m_maxRadius; x++)
				{
					if( (d = M_GET_LENGTH(x, y)) <= m_maxRadius )
					{
						i	= (int)d;
						n	= m_nPoints_R[i]++;

						m_Points_R[i][n].x	= x;
						m_Points_R[i][n].y	= y;
						m_Points_R[i][n].d	= d;
					}
				}
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	Destroy();

	return( false );
}

//---------------------------------------------------------
void CSG_Grid_Radius::Destroy(void)
{
	if( m_Points )
	{
		SG_Free(m_Points);
	}

	if( m_nPoints_R )
	{
		SG_Free(m_nPoints_R);
	}

	if( m_Points_R )
	{
		SG_Free(m_Points_R);
	}

	m_maxRadius	= 0;
	m_nPoints	= 0;
	m_Points	= NULL;
	m_nPoints_R	= NULL;
	m_Points_R	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
