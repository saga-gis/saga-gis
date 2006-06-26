
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
//                  tin_elements.cpp                     //
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
#include "tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Point::CTIN_Point(void)
{
	m_ID			= -1;
	m_Point.x		= m_Point.y	= 0.0;
	m_pRecord		= NULL;
	m_Neighbors		= NULL;
	m_nNeighbors	= 0;
	m_Triangles		= NULL;
	m_nTriangles	= 0;
}

//---------------------------------------------------------
CTIN_Point::CTIN_Point(int ID, TGEO_Point Point, CTable_Record *pRecord)
{
	m_ID			= ID;
	m_Point			= Point;
	m_pRecord		= pRecord;
	m_Neighbors		= NULL;
	m_nNeighbors	= 0;
	m_Triangles		= NULL;
	m_nTriangles	= 0;
}

//---------------------------------------------------------
CTIN_Point::~CTIN_Point(void)
{
	_Del_Relations();
}

//---------------------------------------------------------
bool CTIN_Point::_Add_Triangle(CTIN_Triangle *pTriangle)
{
	for(int i=0; i<m_nTriangles; i++)
	{
		if( m_Triangles[i] == pTriangle )
		{
			return( false );
		}
	}

	m_Triangles	= (CTIN_Triangle **)API_Realloc(m_Triangles, (m_nTriangles + 1) * sizeof(CTIN_Triangle *));
	m_Triangles[m_nTriangles++]	= pTriangle;

//	_Add_Neighbor(pTriangle->Get_Point(0));
//	_Add_Neighbor(pTriangle->Get_Point(1));
//	_Add_Neighbor(pTriangle->Get_Point(2));

	return( true );
}

//---------------------------------------------------------
bool CTIN_Point::_Add_Neighbor(CTIN_Point *pNeighbor)
{
	if( pNeighbor == this )
	{
		return( false );
	}

	for(int i=0; i<m_nNeighbors; i++)
	{
		if( m_Neighbors[i] == pNeighbor )
		{
			return( false );
		}
	}

	m_Neighbors	= (CTIN_Point **)API_Realloc(m_Neighbors, (m_nNeighbors + 1) * sizeof(CTIN_Point *));
	m_Neighbors[m_nNeighbors++]	= pNeighbor;

	return( true );
}

//---------------------------------------------------------
bool CTIN_Point::_Del_Relations(void)
{
	if( m_nTriangles > 0 )
	{
		API_Free(m_Triangles);
		m_Triangles		= NULL;
		m_nTriangles	= 0;
	}

	if( m_nNeighbors > 0 )
	{
		API_Free(m_Neighbors);
		m_Neighbors		= NULL;
		m_nNeighbors	= 0;
	}

	return( true );
}

//---------------------------------------------------------
double CTIN_Point::Get_Gradient(int iNeighbor, int iField)
{
	double		dx, dy, dz;
	CTIN_Point	*pNeighbor;

	if( (pNeighbor = Get_Neighbor(iNeighbor)) != NULL )
	{
		dx	= Get_X() - pNeighbor->Get_X();
		dy	= Get_Y() - pNeighbor->Get_Y();
		dz	= Get_Record()->asDouble(iField) - pNeighbor->Get_Record()->asDouble(iField);

		if( (dx = sqrt(dx*dx + dy*dy)) > 0.0 )
		{
			return( dz / dx );
		}
	}

	return( 0.0 );
}

//---------------------------------------------------------
int TIN_Compare_Triangle_Center(const void *pp1, const void *pp2)
{
	double	*p1	= *((double **)pp1),
			*p2	= *((double **)pp2);

	if( p1[2] < p2[2] )
	{
		return( -1 );
	}

	if( p1[2] > p2[2] )
	{
		return(  1 );
	}

	return( 0 );
}

//---------------------------------------------------------
bool CTIN_Point::Get_Polygon(TGEO_Point **ppPoints, int &nPoints)
{
	int		i;
	double	**p;

	if( m_nTriangles >= 3 )
	{
		//-------------------------------------------------
		p	= (double **)API_Malloc(m_nTriangles * sizeof(double *));

		for(i=0; i<m_nTriangles; i++)
		{
			p[i]	= (double *)API_Malloc(3 * sizeof(double));
			p[i][0]	= m_Triangles[i]->Get_CircumCircle_Point().x;
			p[i][1]	= m_Triangles[i]->Get_CircumCircle_Point().y;
			p[i][2]	= GEO_Get_Angle_Of_Direction(p[i][0] - m_Point.x, p[i][1] - m_Point.y);
		}

		qsort(p, m_nTriangles, sizeof(double *), TIN_Compare_Triangle_Center);

		//-------------------------------------------------
		nPoints		= m_nTriangles;
		*ppPoints	= (TGEO_Point *)API_Malloc(nPoints * sizeof(TGEO_Point));

		for(i=0; i<m_nTriangles; i++)
		{
			(*ppPoints)[i].x	= p[i][0];
			(*ppPoints)[i].y	= p[i][1];
		}

		//-------------------------------------------------
		for(i=0; i<m_nTriangles; i++)
		{
			API_Free(p[i]);
		}

		API_Free(p);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CTIN_Point::Get_Polygon_Area(void)
{
	int			nPoints;
	double		Area;
	TGEO_Point	*pPoints;

	if( Get_Polygon(&pPoints, nPoints) )
	{
		Area	= GEO_Get_Polygon_Area(pPoints, nPoints);

		API_Free(pPoints);

		return( Area );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Edge::CTIN_Edge(CTIN_Point *a, CTIN_Point *b)
{
	m_Points[0]		= a;
	m_Points[1]		= b;
}

//---------------------------------------------------------
CTIN_Edge::~CTIN_Edge(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Triangle::CTIN_Triangle(CTIN_Point *a, CTIN_Point *b, CTIN_Point *c)
{
	m_Points[0]		= a;
	m_Points[1]		= b;
	m_Points[2]		= c;

	//-----------------------------------------------------
	m_Extent.xMin	= m_Extent.xMax	= a->Get_X();
	m_Extent.yMin	= m_Extent.yMax	= a->Get_Y();

	if(			m_Extent.xMin	> b->Get_X() )
				m_Extent.xMin	= b->Get_X();
	else if(	m_Extent.xMax	< b->Get_X() )
				m_Extent.xMax	= b->Get_X();

	if(			m_Extent.yMin	> b->Get_Y() )
				m_Extent.yMin	= b->Get_Y();
	else if(	m_Extent.yMax	< b->Get_Y() )
				m_Extent.yMax	= b->Get_Y();

	if(			m_Extent.xMin	> c->Get_X() )
				m_Extent.xMin	= c->Get_X();
	else if(	m_Extent.xMax	< c->Get_X() )
				m_Extent.xMax	= c->Get_X();

	if(			m_Extent.yMin	> c->Get_Y() )
				m_Extent.yMin	= c->Get_Y();
	else if(	m_Extent.yMax	< c->Get_Y() )
				m_Extent.yMax	= c->Get_Y();

	//-----------------------------------------------------
	m_Area	= fabs(	a->Get_X() * (b->Get_Y() - c->Get_Y())
				+	b->Get_X() * (c->Get_Y() - a->Get_Y())
				+	c->Get_X() * (a->Get_Y() - b->Get_Y())	) / 2.0;

	//-----------------------------------------------------
	TGEO_Point	Points[3];

	Points[0]	= m_Points[0]->Get_Point();
	Points[1]	= m_Points[1]->Get_Point();
	Points[2]	= m_Points[2]->Get_Point();

	GEO_Get_Triangle_CircumCircle(Points, m_Center, m_Radius);
}

//---------------------------------------------------------
CTIN_Triangle::~CTIN_Triangle(void)
{
}

//---------------------------------------------------------
bool CTIN_Triangle::Get_Gradient(int zField, double &Decline, double &Azimuth)
{
	int		i;
	double	x[3], y[3], z[3], A, B, C;

	for(i=0; i<3; i++)
	{
		x[i]	= m_Points[i]->Get_X();
		y[i]	= m_Points[i]->Get_Y();
		z[i]	= m_Points[i]->Get_Record()->asDouble(zField);
	}

	A		= z[0] * (x[1] - x[2]) + z[1] * (x[2] - x[0]) + z[2] * (x[0] - x[1]);
	B		= y[0] * (z[1] - z[2]) + y[1] * (z[2] - z[0]) + y[2] * (z[0] - z[1]);
	C		= x[0] * (y[1] - y[2]) + x[1] * (y[2] - y[0]) + x[2] * (y[0] - y[1]);

	if( C != 0.0 )
	{
		A		= - A / C;
		B		= - B / C;

		Decline	= atan(sqrt(A*A + B*B));

		if( A != 0.0 )
			Azimuth	= M_PI_180 + atan2(B, A);
		else
			Azimuth	= B > 0.0 ? M_PI_270 : (B < 0.0 ? M_PI_090 : -1.0);

		return( true );
	}

	Decline	= -1.0;
	Azimuth	= -1.0;

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
