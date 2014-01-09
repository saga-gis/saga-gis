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
CSG_TIN_Node::CSG_TIN_Node(CSG_TIN *pOwner, int Index)
	: CSG_Table_Record(pOwner, Index)
{
	m_Point.x		= m_Point.y	= 0.0;

	m_Neighbors		= NULL;
	m_nNeighbors	= 0;

	m_Triangles		= NULL;
	m_nTriangles	= 0;
}

//---------------------------------------------------------
CSG_TIN_Node::~CSG_TIN_Node(void)
{
	_Del_Relations();
}

//---------------------------------------------------------
bool CSG_TIN_Node::_Add_Triangle(CSG_TIN_Triangle *pTriangle)
{
	for(int i=0; i<m_nTriangles; i++)
	{
		if( m_Triangles[i] == pTriangle )
		{
			return( false );
		}
	}

	m_Triangles	= (CSG_TIN_Triangle **)SG_Realloc(m_Triangles, (m_nTriangles + 1) * sizeof(CSG_TIN_Triangle *));
	m_Triangles[m_nTriangles++]	= pTriangle;

//	_Add_Neighbor(pTriangle->Get_Point(0));
//	_Add_Neighbor(pTriangle->Get_Point(1));
//	_Add_Neighbor(pTriangle->Get_Point(2));

	return( true );
}

//---------------------------------------------------------
bool CSG_TIN_Node::_Add_Neighbor(CSG_TIN_Node *pNeighbor)
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

	m_Neighbors	= (CSG_TIN_Node **)SG_Realloc(m_Neighbors, (m_nNeighbors + 1) * sizeof(CSG_TIN_Node *));
	m_Neighbors[m_nNeighbors++]	= pNeighbor;

	return( true );
}

//---------------------------------------------------------
bool CSG_TIN_Node::_Del_Relations(void)
{
	if( m_nTriangles > 0 )
	{
		SG_Free(m_Triangles);
		m_Triangles		= NULL;
		m_nTriangles	= 0;
	}

	if( m_nNeighbors > 0 )
	{
		SG_Free(m_Neighbors);
		m_Neighbors		= NULL;
		m_nNeighbors	= 0;
	}

	return( true );
}

//---------------------------------------------------------
double CSG_TIN_Node::Get_Gradient(int iNeighbor, int iField)
{
	double		dx, dy, dz;
	CSG_TIN_Node	*pNeighbor;

	if( (pNeighbor = Get_Neighbor(iNeighbor)) != NULL )
	{
		dx	= Get_X() - pNeighbor->Get_X();
		dy	= Get_Y() - pNeighbor->Get_Y();
		dz	= asDouble(iField) - pNeighbor->asDouble(iField);

		if( (dx = sqrt(dx*dx + dy*dy)) > 0.0 )
		{
			return( dz / dx );
		}
	}

	return( 0.0 );
}

//---------------------------------------------------------
int SG_TIN_Compare_Triangle_Center(const void *pz1, const void *pz2)
{
	double	z1	= ((TSG_Point_Z *)pz1)->z,
			z2	= ((TSG_Point_Z *)pz2)->z;

	if( z1 < z2 )
	{
		return( -1 );
	}

	if( z1 > z2 )
	{
		return(  1 );
	}

	return( 0 );
}

//---------------------------------------------------------
#define M_GET_DIRECTION(a, b)	(b.x != a.x ? M_PI_180 - atan2(b.y - a.y, b.x - a.x) : (b.y > a.y ? M_PI_270 : (b.y < a.y ? M_PI_090 : 0.0)))

//---------------------------------------------------------
bool CSG_TIN_Node::Get_Polygon(CSG_Points &Points)
{
	if( m_nTriangles >= 3 )
	{
		int				i;
		TSG_Point		c;
		CSG_Points_Z	p;

		for(i=0; i<m_nTriangles; i++)
		{
			c	= m_Triangles[i]->Get_CircumCircle_Point();

			p.Add(c.x, c.y, M_GET_DIRECTION(m_Point, c));
		}

		qsort(&(p[0]), p.Get_Count(), sizeof(TSG_Point_Z), SG_TIN_Compare_Triangle_Center);

		Points.Clear();

		for(i=0; i<m_nTriangles; i++)
		{
			Points.Add(p[i].x, p[i].y);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CSG_TIN_Node::Get_Polygon_Area(void)
{
	CSG_Points	Points;

	if( Get_Polygon(Points) )
	{
		return( SG_Get_Polygon_Area(Points) );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_TIN_Edge::CSG_TIN_Edge(CSG_TIN_Node *a, CSG_TIN_Node *b)
{
	m_Nodes[0]		= a;
	m_Nodes[1]		= b;
}

//---------------------------------------------------------
CSG_TIN_Edge::~CSG_TIN_Edge(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_TIN_Triangle::CSG_TIN_Triangle(CSG_TIN_Node *a, CSG_TIN_Node *b, CSG_TIN_Node *c)
{
	m_Nodes[0]		= a;
	m_Nodes[1]		= b;
	m_Nodes[2]		= c;

	//-----------------------------------------------------
	double	xMin, yMin, xMax, yMax;

	xMin	= xMax	= a->Get_X();
	yMin	= yMax	= a->Get_Y();

	if(			xMin	> b->Get_X() )
				xMin	= b->Get_X();
	else if(	xMax	< b->Get_X() )
				xMax	= b->Get_X();

	if(			yMin	> b->Get_Y() )
				yMin	= b->Get_Y();
	else if(	yMax	< b->Get_Y() )
				yMax	= b->Get_Y();

	if(			xMin	> c->Get_X() )
				xMin	= c->Get_X();
	else if(	xMax	< c->Get_X() )
				xMax	= c->Get_X();

	if(			yMin	> c->Get_Y() )
				yMin	= c->Get_Y();
	else if(	yMax	< c->Get_Y() )
				yMax	= c->Get_Y();

	m_Extent.Assign(xMin, yMin, xMax, yMax);

	//-----------------------------------------------------
	m_Area	= fabs(	a->Get_X() * (b->Get_Y() - c->Get_Y())
				+	b->Get_X() * (c->Get_Y() - a->Get_Y())
				+	c->Get_X() * (a->Get_Y() - b->Get_Y())	) / 2.0;

	//-----------------------------------------------------
	TSG_Point	Points[3];

	Points[0]	= m_Nodes[0]->Get_Point();
	Points[1]	= m_Nodes[1]->Get_Point();
	Points[2]	= m_Nodes[2]->Get_Point();

	SG_Get_Triangle_CircumCircle(Points, m_Center, m_Radius);
}

//---------------------------------------------------------
CSG_TIN_Triangle::~CSG_TIN_Triangle(void)
{}


//---------------------------------------------------------
bool CSG_TIN_Triangle::is_Containing(const TSG_Point &Point)
{
	return( is_Containing(Point.x, Point.y) );
}

//---------------------------------------------------------
#define IS_ONLINE(A, B)	(A.y == B.y && ((A.x <= x && x <= B.x) || (B.x <= x && x <= A.x)))

//---------------------------------------------------------
bool CSG_TIN_Triangle::is_Containing(double x, double y)
{
	if( m_Extent.Contains(x, y) )
	{
		int			nCrossings;
		TSG_Point	A, B, C;

		if(	(x == m_Nodes[0]->Get_Point().x && y == m_Nodes[0]->Get_Point().y)
		||	(x == m_Nodes[1]->Get_Point().x && y == m_Nodes[1]->Get_Point().y)
		||	(x == m_Nodes[2]->Get_Point().x && y == m_Nodes[2]->Get_Point().y) )
			return( true );

		if( y == m_Extent.Get_YMin() || y == m_Extent.Get_YMax() )
		{
			if(	IS_ONLINE(m_Nodes[0]->Get_Point(), m_Nodes[1]->Get_Point())
			||	IS_ONLINE(m_Nodes[1]->Get_Point(), m_Nodes[2]->Get_Point())
			||	IS_ONLINE(m_Nodes[2]->Get_Point(), m_Nodes[0]->Get_Point()) )
				return( true );
		}

		nCrossings	= 0;

		if(	(y == m_Nodes[0]->Get_Point().y && x > m_Nodes[0]->Get_Point().x)
		||	(y == m_Nodes[1]->Get_Point().y && x > m_Nodes[1]->Get_Point().x)
		||	(y == m_Nodes[2]->Get_Point().y && x > m_Nodes[2]->Get_Point().x) )
			nCrossings	= -1;

		A.x			= m_Extent.m_rect.xMin - 1.0;
		B.x			= x;
		A.y = B.y	= y;

		if( SG_Get_Crossing(C, m_Nodes[0]->Get_Point(), m_Nodes[1]->Get_Point(), A, B) )
			nCrossings++;

		if( SG_Get_Crossing(C, m_Nodes[1]->Get_Point(), m_Nodes[2]->Get_Point(), A, B) )
			nCrossings++;

		if( SG_Get_Crossing(C, m_Nodes[2]->Get_Point(), m_Nodes[0]->Get_Point(), A, B) )
			nCrossings++;

		return( nCrossings == 1 );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_TIN_Triangle::Get_Value(int zField, const TSG_Point &p, double &z)
{
	return( Get_Value(zField, p.x, p.y, z) );
}

bool CSG_TIN_Triangle::Get_Value(int zField, double x, double y, double &z)
{
	CSG_Vector	B, Z(3);
	CSG_Matrix	M(3, 3), Mt;

	for(int i=0; i<3; i++)
	{
		M[i][0]	= 1.0;
		M[i][1]	= m_Nodes[i]->Get_X();
		M[i][2]	= m_Nodes[i]->Get_Y();
		Z[i]	= m_Nodes[i]->asDouble(zField);
	}

	Mt	= M.Get_Transpose();
	B	= (Mt * M).Get_Inverse() * (Mt * Z);

	z	= B[0] + B[1] * x + B[2] * y;

	return( true );
}

//---------------------------------------------------------
bool CSG_TIN_Triangle::Get_Gradient(int zField, double &Decline, double &Azimuth)
{
	int		i;
	double	x[3], y[3], z[3], A, B, C;

	for(i=0; i<3; i++)
	{
		x[i]	= m_Nodes[i]->Get_X();
		y[i]	= m_Nodes[i]->Get_Y();
		z[i]	= m_Nodes[i]->asDouble(zField);
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
