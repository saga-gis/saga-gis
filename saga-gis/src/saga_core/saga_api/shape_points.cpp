
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
//                   shape_points.cpp                    //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape_Points::CSG_Shape_Points(CSG_Shapes *pOwner, int Index)
	: CSG_Shape(pOwner, Index)
{
	m_pParts  = NULL;
	m_nParts  = 0;

	m_nPoints = 0; // total number of points

	m_bUpdate = true;
}

//---------------------------------------------------------
CSG_Shape_Points::~CSG_Shape_Points(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Points::Destroy(void)
{
	CSG_Shape::Destroy();

	Del_Parts();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Points::On_Assign(CSG_Shape *pShape)
{
	Del_Parts();

	TSG_Vertex_Type	Vertex_Type	= Get_Vertex_Type();

	if( pShape->Get_Type() == SHAPE_TYPE_Point )	// just in case...
	{
		Add_Point(pShape->Get_Point(0), 0);

		switch( Vertex_Type )
		{
		case SG_VERTEX_TYPE_XYZM: Get_Part(0)->Set_M(pShape->Get_M(0), 0);
		case SG_VERTEX_TYPE_XYZ : Get_Part(0)->Set_Z(pShape->Get_Z(0), 0);
		default: break;
		}

		return( true );
	}

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		Add_Part(((CSG_Shape_Points *)pShape)->Get_Part(iPart));
	}
 
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Points::_Add_Part(void)
{
	m_pParts = (CSG_Shape_Part **)SG_Realloc(m_pParts, (m_nParts + 1) * sizeof(CSG_Shape_Part *));

	m_pParts[m_nParts++] = _Get_Part();

	return( m_nParts );
}

//---------------------------------------------------------
int CSG_Shape_Points::Add_Part(CSG_Shape_Part *pPart)
{
	int iPart = m_nParts;

	if( pPart && _Add_Part() > iPart )
	{
		m_pParts[iPart]->Assign(pPart);
	}

	return( m_nParts );
}

//---------------------------------------------------------
int CSG_Shape_Points::Del_Part(int del_Part)
{
	if( del_Part >= 0 && del_Part < m_nParts )
	{
		m_nParts--;

		delete(m_pParts[del_Part]);

		for(int iPart=del_Part; iPart<m_nParts; iPart++)
		{
			m_pParts[iPart] = m_pParts[iPart + 1];
		}

		m_pParts = (CSG_Shape_Part **)SG_Realloc(m_pParts, m_nParts * sizeof(CSG_Shape_Part *));

		_Invalidate();
	}

	return( m_nParts );
}

//---------------------------------------------------------
int CSG_Shape_Points::Del_Parts(void)
{
	for(int iPart=m_nParts-1; iPart>=0; iPart--)
	{
		Del_Part(iPart);
	}

	return( m_nParts );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Points::Ins_Point(double x, double y, int iPoint, int iPart)
{
	if( iPart >= m_nParts )
	{
		for(int i=m_nParts; i<=iPart; i++)
		{
			_Add_Part();
		}
	}

	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Ins_Point(x, y, iPoint) : 0 );
}

//---------------------------------------------------------
int CSG_Shape_Points::Ins_Point(const TSG_Point_Z &p, int iPoint, int iPart)
{
	if( Ins_Point(p.x, p.y, iPoint, iPart) )
	{
		Set_Z(p.z, iPoint, iPart);

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Shape_Points::Ins_Point(const TSG_Point_ZM &p, int iPoint, int iPart)
{
	if( Ins_Point(p.x, p.y, iPoint, iPart) )
	{
		Set_Z(p.z, iPoint, iPart);
		Set_M(p.m, iPoint, iPart);

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Points::Set_Point(double x, double y, int iPoint, int iPart)
{
	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Set_Point(x, y, iPoint) : 0 );
}

//---------------------------------------------------------
int CSG_Shape_Points::Set_Point(const TSG_Point_Z &p, int iPoint, int iPart)
{
	if( Set_Point(p.x, p.y, iPoint, iPart) )
	{
		Set_Z(p.z, iPoint, iPart);

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_Shape_Points::Set_Point(const TSG_Point_ZM &p, int iPoint, int iPart)
{
	if( Set_Point(p.x, p.y, iPoint, iPart) )
	{
		Set_Z(p.z, iPoint, iPart);
		Set_M(p.m, iPoint, iPart);

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Points::Del_Point(int iPoint, int iPart)
{
	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Del_Point(iPoint) : 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CSG_Shape_Points::Get_Point(int iPoint) const
{
	for(int iPart=0; iPart<m_nParts; iPoint-=m_pParts[iPart++]->Get_Count())
	{
		if( iPoint < m_pParts[iPart]->Get_Count() )
		{
			return( m_pParts[iPart]->Get_Point(iPoint) );
		}
	}

	return( CSG_Point(0., 0.) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Points::_Update_Extent(void)
{
	if( m_bUpdate )
	{
		int nPoints = 0;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			CSG_Shape_Part *pPart = m_pParts[iPart];

			if( pPart->Get_Count() > 0 )
			{
				if( nPoints == 0 )
				{
					nPoints  = pPart->Get_Count();

					m_Extent = pPart->Get_Extent();

					m_ZMin   = pPart->Get_ZMin();
					m_ZMax   = pPart->Get_ZMax();

					m_MMin   = pPart->Get_MMin();
					m_MMax   = pPart->Get_MMax();
				}
				else
				{
					nPoints += pPart->Get_Count();

					m_Extent.Union(pPart->Get_Extent());

					if     ( m_ZMin > pPart->Get_ZMin() ) { m_ZMin = pPart->Get_ZMin(); }
					else if( m_ZMax < pPart->Get_ZMax() ) { m_ZMax = pPart->Get_ZMax(); }

					if     ( m_MMin > pPart->Get_MMin() ) { m_MMin = pPart->Get_MMin(); }
					else if( m_MMax < pPart->Get_MMax() ) { m_MMax = pPart->Get_MMax(); }
				}
			}
		}

		m_bUpdate = false;

		//_ASSERT(nPoints == m_nPoints);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CSG_Shape_Points::Get_Centroid(void)
{
	int n = 0; CSG_Point c(0., 0.);

	for(int iPart=0; iPart<Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<Get_Point_Count(iPart); iPoint++)
		{
			c	+= Get_Point(iPoint, iPart);
			n	++;
		}
	}

	if( n > 0 )
	{
		c.Assign(c.Get_X() / n, c.Get_Y() / n);
	}

	return( c );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point) const 
{
	TSG_Point Next; return( Get_Distance(Point, Next) );
}

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point, int iPart) const 
{
	TSG_Point Next; return( Get_Distance(Point, Next, iPart) );
}

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point, TSG_Point &Next) const 
{
	double Distance = Get_Distance(Point, Next, 0);

	for(int iPart=1; iPart<m_nParts && Distance!=0.; iPart++)
	{
		TSG_Point iNext; double iDistance = Get_Distance(Point, iNext, iPart);

		if(	iDistance >= 0. && (iDistance < Distance || Distance < 0.) )
		{
			Distance = iDistance; Next = iNext;
		}
	}

	return( Distance );
}

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart) const 
{
	double Distance = -1.;

	if( iPart >= 0 && iPart < m_nParts )
	{
		TSG_Point *pPoint = m_pParts[iPart]->m_Points;

		for(int iPoint=0; iPoint<m_pParts[iPart]->Get_Count() && Distance!=0.; iPoint++, pPoint++)
		{
			double iDistance = SG_Get_Distance(Point, *pPoint);

			if(	iDistance < Distance || Distance < 0. )
			{
				Distance = iDistance; Next = *pPoint;
			}
		}
	}

	return( Distance );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Points::On_Intersects(CSG_Shape *pShape)
{
	CSG_Shape	*piPoints, *pjPoints;

	if( Get_Point_Count() < pShape->Get_Point_Count() )
	{
		piPoints	= this;
		pjPoints	= pShape;
	}
	else
	{
		piPoints	= pShape;
		pjPoints	= this;
	}

	bool	bIn		= false;
	bool	bOut	= false;

	for(int iPart=0; iPart<piPoints->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<piPoints->Get_Point_Count(iPart); iPoint++)
		{
			CSG_Point	Point	= piPoints->Get_Point(iPoint, iPart);

			for(int jPart=0; jPart<pjPoints->Get_Part_Count(); jPart++)
			{
				for(int jPoint=0; jPoint<pjPoints->Get_Point_Count(jPart); jPoint++)
				{
					if( Point.is_Equal(pjPoints->Get_Point(jPoint, jPart)) )
					{
						bIn		= true;
					}
					else
					{
						bOut	= true;
					}

					if( bIn && bOut )
					{
						return( INTERSECTION_Overlaps );
					}
				}
			}
		}
	}

	if( bIn )
	{
		return( piPoints == this ? INTERSECTION_Contained : INTERSECTION_Contains );
	}

	return( INTERSECTION_None );
}

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Points::On_Intersects(TSG_Rect Extent)
{
	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		TSG_Point	*p	= m_pParts[iPart]->m_Points;

		for(int iPoint=0; iPoint<m_pParts[iPart]->Get_Count(); iPoint++, p++)
		{
			if(	Extent.xMin <= p->x && p->x <= Extent.xMax
			&&	Extent.yMin <= p->y && p->y <= Extent.yMax	)
			{
				return( INTERSECTION_Overlaps );
			}
		}
	}

	return( INTERSECTION_None );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
