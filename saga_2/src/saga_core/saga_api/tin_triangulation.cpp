
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
//                tin_triangulation.cpp                  //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// The Delaunay Triangulation algorithm used here is based
// on Paul Bourke's C source codes, which can be found at:
//
//     http://astronomy.swin.edu.au/~pbourke/
//
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int TIN_Compare(const void *pp1, const void *pp2)
{
	CTIN_Point	*p1	= *((CTIN_Point **)pp1),
				*p2	= *((CTIN_Point **)pp2);

	if( p1->Get_X() < p2->Get_X() )
	{
		return( -1 );
	}

	if( p1->Get_X() > p2->Get_X() )
	{
		return(  1 );
	}

	if( p1->Get_Y() < p2->Get_Y() )
	{
		return( -1 );
	}

	if( p1->Get_Y() > p2->Get_Y() )
	{
		return(  1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::_Triangulate(void)
{
	bool			bResult;
	int				i, j, n, nTriangles;
	CTIN_Point		**Points;
	TTIN_Triangle	*Triangles;

	//-----------------------------------------------------
	_Destroy_Edges();
	_Destroy_Triangles();

	//-----------------------------------------------------
	Points		= (CTIN_Point **)SG_Malloc(m_nPoints * sizeof(CTIN_Point *));

	for(i=0; i<m_nPoints; i++)
	{
		Points[i]	= m_Points[i];
		Points[i]->_Del_Relations();
	}

	qsort(Points, m_nPoints, sizeof(CTIN_Point *), TIN_Compare);

	//-----------------------------------------------------
	for(i=0, j=0, n=m_nPoints; j<n; i++)
	{
		Points[i]	= Points[j++];

		while(	j < n
			&&	Points[i]->Get_X() == Points[j]->Get_X()
			&&	Points[i]->Get_Y() == Points[j]->Get_Y() )
		{
			Del_Point(Points[j++]->Get_Record()->Get_Index(), false);
		}
	}

	//-----------------------------------------------------
	Points		= (CTIN_Point **)SG_Realloc(Points, (m_nPoints + 3) * sizeof(CTIN_Point *));

	for(i=m_nPoints; i<m_nPoints+3; i++)
	{
		Points[i]	= new CTIN_Point;
	}

	//-----------------------------------------------------
	Triangles	= (TTIN_Triangle *)SG_Malloc(3 * m_nPoints * sizeof(TTIN_Triangle));

	if( (bResult = _Triangulate(Points, m_nPoints, Triangles, nTriangles)) == true )
	{
		for(i=0; i<nTriangles && SG_UI_Process_Set_Progress(i, nTriangles); i++)
		{
			_Add_Triangle(Points[Triangles[i].p1], Points[Triangles[i].p2], Points[Triangles[i].p3]);
		}
	}

	SG_Free(Triangles);

	//-----------------------------------------------------
	for(i=m_nPoints; i<m_nPoints+3; i++)
	{
		delete(Points[i]);
	}

	SG_Free(Points);

	SG_UI_Process_Set_Ready();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN::_Triangulate(CTIN_Point **Points, int nPoints, TTIN_Triangle *Triangles, int &nTriangles)
{
	int			i, j, k, inside, trimax,
				nedge		= 0,
				emax		= 200,
				status		= 0,
				*complete	= NULL;

	double		xmid, ymid, dmax,
				xp, yp, x1, y1, x2, y2, x3, y3, xc, yc, r;

	TTIN_Edge	*edges		= NULL;

	//-----------------------------------------------------
	// Allocate memory for the completeness list, flag for each triangle
	trimax	= 4 * nPoints;
	if( (complete	= (int       *)SG_Malloc(trimax * sizeof(int))) == NULL )
	{
		status	= 1;
		goto skip;
	}

	//-----------------------------------------------------
	// Allocate memory for the edge list
	if( (edges		= (TTIN_Edge *)SG_Malloc(emax   * sizeof(TTIN_Edge))) == NULL )
	{
		status	= 2;
		goto skip;
	}

	//-----------------------------------------------------
	//	Find the maximum and minimum vertex bounds.
	//	This is to allow calculation of the bounding triangle
	//	Set up the supertriangle
	//	This is a triangle which encompasses all the sample points.
	//	The supertriangle coordinates are added to the end of the
	//	vertex list. The supertriangle is the first triangle in
	//	the triangle list.

	_Extent_Update();

	dmax	= m_Extent.Get_XRange() > m_Extent.Get_YRange() ? m_Extent.Get_XRange() : m_Extent.Get_YRange();
	xmid	= m_Extent.Get_XCenter();
	ymid	= m_Extent.Get_YCenter();

	Points[nPoints + 0]->m_Point.x	= xmid - 20 * dmax;
	Points[nPoints + 0]->m_Point.y	= ymid - dmax;
	Points[nPoints + 1]->m_Point.x	= xmid;
	Points[nPoints + 1]->m_Point.y	= ymid + 20 * dmax;
	Points[nPoints + 2]->m_Point.x	= xmid + 20 * dmax;
	Points[nPoints + 2]->m_Point.y	= ymid - dmax;

	Triangles[0].p1	= nPoints;
	Triangles[0].p2	= nPoints + 1;
	Triangles[0].p3	= nPoints + 2;

	complete [0]	= false;

	nTriangles		= 1;

	//-----------------------------------------------------
	//	Include each point one at a time into the existing mesh
	for(i=0; i<nPoints && SG_UI_Process_Set_Progress(i, nPoints); i++)
	{
		xp		= Points[i]->m_Point.x;
		yp		= Points[i]->m_Point.y;
		nedge	= 0;

		//-------------------------------------------------
		//	Set up the edge buffer.
		//	If the point (xp,yp) lies inside the circumcircle then the
		//	three edges of that triangle are added to the edge buffer
		//	and that triangle is removed.
		for(j=0; j<nTriangles; j++)
		{
			if( complete[j] )
			{
				continue;
			}

			x1		= Points[Triangles[j].p1]->m_Point.x;
			y1		= Points[Triangles[j].p1]->m_Point.y;
			x2		= Points[Triangles[j].p2]->m_Point.x;
			y2		= Points[Triangles[j].p2]->m_Point.y;
			x3		= Points[Triangles[j].p3]->m_Point.x;
			y3		= Points[Triangles[j].p3]->m_Point.y;

			inside	= _CircumCircle(xp, yp, x1, y1, x2, y2, x3, y3, &xc, &yc, &r);

			if( xc + r < xp )
			{
				complete[j]	= true;
			}

			if( inside )
			{
				// Check that we haven't exceeded the edge list size
				if( nedge + 3 >= emax )
				{
					emax	+= 100;

					if( (edges = (TTIN_Edge *)SG_Realloc(edges, emax * sizeof(TTIN_Edge))) == NULL )
					{
						status	= 3;
						goto skip;
					}
				}

				edges[nedge + 0].p1	= Triangles[j].p1;
				edges[nedge + 0].p2	= Triangles[j].p2;
				edges[nedge + 1].p1	= Triangles[j].p2;
				edges[nedge + 1].p2	= Triangles[j].p3;
				edges[nedge + 2].p1	= Triangles[j].p3;
				edges[nedge + 2].p2	= Triangles[j].p1;

				nedge	+= 3;

				Triangles[j]	= Triangles[nTriangles - 1];
				complete [j]	= complete [nTriangles - 1];

				nTriangles--;
				j--;
			}
		}

		//-------------------------------------------------
		//	Tag multiple edges
		//	Note: if all triangles are specified anticlockwise then all
		//	      interior edges are opposite pointing in direction.
		for(j=0; j<nedge-1; j++)
		{
			for(k=j+1; k<nedge; k++)
			{
				if( (edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1) )
				{
					edges[j].p1 = -1;
					edges[j].p2 = -1;
					edges[k].p1 = -1;
					edges[k].p2 = -1;
				}

				// Shouldn't need the following, see note above
				if( (edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2) )
				{
					edges[j].p1 = -1;
					edges[j].p2 = -1;
					edges[k].p1 = -1;
					edges[k].p2 = -1;
				}
			}
		}

		//-------------------------------------------------
		//	Form new triangles for the current point
		//	Skipping over any tagged edges.
		//	All edges are arranged in clockwise order.
		for(j=0; j<nedge; j++)
		{
			if( edges[j].p1 < 0 || edges[j].p2 < 0 )
			{
				continue;
			}

			if( nTriangles >= trimax )
			{
				status	= 4;
				goto skip;
			}

			Triangles[nTriangles].p1	= edges[j].p1;
			Triangles[nTriangles].p2	= edges[j].p2;
			Triangles[nTriangles].p3	= i;
			complete [nTriangles]		= false;
			nTriangles++;
		}
	}

	//-----------------------------------------------------
	//	Remove triangles with supertriangle vertices
	//	These are triangles which have a vertex number greater than nPoints
	for(i=0; i<nTriangles; i++)
	{
		if(	Triangles[i].p1 >= nPoints
		||	Triangles[i].p2 >= nPoints
		||	Triangles[i].p3 >= nPoints )
		{
			Triangles[i] = Triangles[nTriangles - 1];
			nTriangles--;
			i--;
		}
	}

	//-----------------------------------------------------
	skip:

	if( edges )
	{
		SG_Free(edges);
	}

	if( complete )
	{
		SG_Free(complete);
	}

	return( status == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//	Return true if a point (xp,yp) is inside the circumcircle made up
//	of the points (x1,y1), (x2,y2), (x3,y3)
//	The circumcircle centre is returned in (xc,yc) and the radius r
//	NOTE: A point on the edge is inside the circumcircle
//

//---------------------------------------------------------
#define IS_IDENTICAL(a, b)	(a == b)
//#define IS_IDENTICAL(a, b)	(fabs(a - b) < 0.0001)

//---------------------------------------------------------
int CTIN::_CircumCircle(double xp, double yp, double x1, double y1, double x2, double y2, double x3, double y3, double *xc, double *yc, double *r)
{
	double	m1, m2, mx1, mx2, my1, my2,
			dx, dy, rsqr, drsqr;

	// Check for coincident points
	if( IS_IDENTICAL(y1, y2) && IS_IDENTICAL(y2, y3) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( IS_IDENTICAL(y2, y1) )
	{
		m2	= -(x3 - x2) / (y3 - y2);
		mx2	=  (x2 + x3) / 2.0;
		my2	=  (y2 + y3) / 2.0;
		*xc	=  (x2 + x1) / 2.0;

		*yc	= m2 * (*xc - mx2) + my2;
	}
	else if( IS_IDENTICAL(y3, y2) )
	{
		m1	= -(x2 - x1) / (y2 - y1);
		mx1	=  (x1 + x2) / 2.0;
		my1	=  (y1 + y2) / 2.0;
		*xc	=  (x3 + x2) / 2.0;

		*yc	= m1 * (*xc - mx1) + my1;
	}
	else
	{
		m1	= -(x2 - x1) / (y2 - y1);
		m2	= -(x3 - x2) / (y3 - y2);
		mx1	=  (x1 + x2) / 2.0;
		mx2	=  (x2 + x3) / 2.0;
		my1	=  (y1 + y2) / 2.0;
		my2	=  (y2 + y3) / 2.0;

		*xc	= (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
		*yc	= m1 * (*xc - mx1) + my1;
	}

	//-----------------------------------------------------
	dx		= x2 - *xc;
	dy		= y2 - *yc;
	rsqr	= dx*dx + dy*dy;
	*r		= sqrt(rsqr);

	dx		= xp - *xc;
	dy		= yp - *yc;
	drsqr	= dx*dx + dy*dy;

	return( drsqr <= rsqr ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
