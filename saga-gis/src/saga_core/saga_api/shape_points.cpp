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
	m_pParts	= NULL;
	m_nParts	= 0;

	m_bUpdate	= true;
}

//---------------------------------------------------------
CSG_Shape_Points::~CSG_Shape_Points(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape_Points::On_Assign(CSG_Shape *pShape)
{
	Del_Parts();

	TSG_Vertex_Type	Vertex_Type	= Get_Vertex_Type();

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			CSG_Shape::Add_Point(pShape->Get_Point(iPoint, iPart), iPart);

			switch( Vertex_Type )
			{
			case SG_VERTEX_TYPE_XYZM:	CSG_Shape::Set_M(pShape->Get_M(iPoint, iPart), iPoint, iPart);
			case SG_VERTEX_TYPE_XYZ :	CSG_Shape::Set_Z(pShape->Get_Z(iPoint, iPart), iPoint, iPart);
			default:	break;
			}
		}
	}
 
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Points::_Add_Part(void)
{
	m_pParts			= (CSG_Shape_Part **)SG_Realloc(m_pParts , (m_nParts + 1) * sizeof(CSG_Shape_Part *));
	m_pParts[m_nParts]	= _Get_Part();

	m_nParts++;

	return( m_nParts );
}

//---------------------------------------------------------
int CSG_Shape_Points::Del_Part(int del_Part)
{
	if( del_Part >= 0 && del_Part < m_nParts )
	{
		delete(m_pParts[del_Part]);

		m_nParts--;

		for(int iPart=del_Part; iPart<m_nParts; iPart++)
		{
			m_pParts[iPart]	= m_pParts[iPart + 1];
		}

		m_pParts	= (CSG_Shape_Part **)SG_Realloc(m_pParts , m_nParts * sizeof(CSG_Shape_Part *));

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape_Points::Add_Point(double x, double y, int iPart)
{
	if( iPart >= m_nParts )
	{
		for(int i=m_nParts; i<=iPart; i++)
		{
			_Add_Part();
		}
	}

	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Add_Point(x, y) : 0 );
}

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
int CSG_Shape_Points::Set_Point(double x, double y, int iPoint, int iPart)
{
	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Set_Point(x, y, iPoint) : 0 );
}

//---------------------------------------------------------
int CSG_Shape_Points::Del_Point(int del_Point, int iPart)
{
	return( iPart >= 0 && iPart < m_nParts ? m_pParts[iPart]->Del_Point(del_Point) : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape_Points::_Update_Extent(void)
{
	if( m_bUpdate )
	{
		bool	bFirst;
		int		iPart;

		for(iPart=0, bFirst=true; iPart<m_nParts; iPart++)
		{
			CSG_Shape_Part	*pPart	= m_pParts[iPart];

			if( pPart->Get_Count() > 0 )
			{
				if( bFirst )
				{
					bFirst		= false;

					m_Extent	= pPart->Get_Extent();

					m_ZMin		= pPart->Get_ZMin();
					m_ZMax		= pPart->Get_ZMax();

					m_MMin		= pPart->Get_MMin();
					m_MMax		= pPart->Get_MMax();
				}
				else
				{
					m_Extent.Union(pPart->Get_Extent());

					if( m_ZMin > pPart->Get_ZMin() )	m_ZMin	= pPart->Get_ZMin();
					if( m_ZMax < pPart->Get_ZMax() )	m_ZMax	= pPart->Get_ZMax();

					if( m_MMin > pPart->Get_MMin() )	m_MMin	= pPart->Get_MMin();
					if( m_MMax < pPart->Get_MMax() )	m_MMax	= pPart->Get_MMax();
				}
			}
		}

		m_bUpdate	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CSG_Shape_Points::Get_Centroid(void)
{
	int			n	= 0;
	CSG_Point	c(0.0, 0.0);

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point)
{
	TSG_Point	Next;

	return( Get_Distance(Point, Next) );
}

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point, TSG_Point &Next)
{
	int			iPart;
	double		d, Distance;
	TSG_Point	pt;

	Distance	= Get_Distance(Point, Next, 0);

	for(iPart=1; iPart<m_nParts && Distance!=0.0; iPart++)
	{
		if(	(d = Get_Distance(Point, pt, iPart)) >= 0.0
		&&	(d < Distance || Distance < 0.0) )
		{
			Distance	= d;
			Next		= pt;
		}
	}

	return( Distance );
}

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point, int iPart)
{
	TSG_Point	Next;

	return( Get_Distance(Point, Next, iPart) );
}

//---------------------------------------------------------
double CSG_Shape_Points::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart)
{
	int			i;
	double		d, Distance;
	TSG_Point	*pA;

	Distance	= -1.0;

	if( iPart >= 0 && iPart < m_nParts )
	{
		for(i=0, pA=m_pParts[iPart]->m_Points; i<m_pParts[iPart]->Get_Count() && Distance!=0.0; i++, pA++)
		{
			if(	(d = SG_Get_Distance(Point, *pA)) < Distance || Distance < 0.0 )
			{
				Distance	= d;
				Next		= *pA;
			}
		}
	}

	return( Distance );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Points::On_Intersects(CSG_Shape *pShape)
{
	CSG_Shape	*piPoints, *pjPoints;

	if( CSG_Shape::Get_Point_Count() < pShape->Get_Point_Count() )
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
