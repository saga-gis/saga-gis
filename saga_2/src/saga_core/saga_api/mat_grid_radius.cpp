
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
CMAT_Grid_Radius::CMAT_Grid_Radius(int _max_Radius)
{
	max_Radius		= 0;

	Create(_max_Radius);
}

//---------------------------------------------------------
CMAT_Grid_Radius::~CMAT_Grid_Radius(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMAT_Grid_Radius::Create(int _max_Radius)
{
	int		x, y, nMask, ix, iy, Radius;
	double	dRadius;
	CGrid	gMask;

	Destroy();

	//-----------------------------------------------------
	if( _max_Radius > 0 )
	{
		max_Radius	= _max_Radius;

		nPoints		= (int *)				API_Calloc(max_Radius, sizeof(int));
		Points		= (TMAT_Grid_Radius **)	API_Calloc(max_Radius, sizeof(TMAT_Grid_Radius *));

		nMask		= 1 + 2 * max_Radius;
		gMask.Create(GRID_TYPE_Double, nMask, nMask);

		for(iy=-max_Radius, y=0; y<nMask; iy++, y++)
		{
			for(ix=-max_Radius, x=0; x<nMask; ix++, x++)
			{
				gMask.Set_Value(x, y, dRadius = M_GET_DIST(ix, iy));

				if( (Radius = (int)dRadius) < max_Radius )
				{
					nPoints[Radius]++;
				}
			}
		}

		//-------------------------------------------------
		for(Radius=0; Radius<max_Radius; Radius++)
		{
			Points[Radius]	= (TMAT_Grid_Radius *)API_Calloc(nPoints[Radius], sizeof(TMAT_Grid_Radius));
			nPoints[Radius]	= 0;
		}

		//-------------------------------------------------
		for(y=0; y<nMask; y++)
		{
			for(x=0; x<nMask; x++)
			{
				if( (Radius = gMask.asInt(x, y)) < max_Radius )
				{
					Points[Radius][nPoints[Radius]].x	= x - max_Radius;
					Points[Radius][nPoints[Radius]].y	= y - max_Radius;
					Points[Radius][nPoints[Radius]].d	= gMask.asDouble(x, y);

					nPoints[Radius]++;
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CMAT_Grid_Radius::Destroy(void)
{
	int		i;

	if( max_Radius > 0 )
	{
		for(i=0; i<max_Radius; i++)
		{
			if( Points[i] )
			{
				API_Free(Points[i]);
			}
		}

		API_Free(Points);
		API_Free(nPoints);

		max_Radius	= 0;
	}
}
