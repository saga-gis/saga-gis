
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
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShape_Points::CShape_Points(CShapes *pOwner, CTable_Record *pRecord)
	: CShape(pOwner, pRecord)
{
	m_Points	= NULL;
	m_nPoints	= NULL;
	m_nParts	= 0;

	m_bUpdate	= true;
}

//---------------------------------------------------------
CShape_Points::~CShape_Points(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShape_Points::Destroy(void)
{
	CShape::Destroy();

	Del_AllParts();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShape_Points::On_Assign(CShape *pShape)
{
	int		iPart, iPoint;

	Del_AllParts();

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			CShape::Add_Point(pShape->Get_Point(iPoint, iPart), iPart);
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
int CShape_Points::_Add_Part(void)
{
	m_Points			= (TGEO_Point **)	API_Realloc( m_Points, (m_nParts + 1) * sizeof(TGEO_Point *));
	m_Points[m_nParts]	= NULL;

	m_nPoints			= (int *)			API_Realloc(m_nPoints, (m_nParts + 1) * sizeof(int));
	m_nPoints[m_nParts]	= 0;

	m_nParts++;

	return( m_nParts );
}

//---------------------------------------------------------
int CShape_Points::Del_Part(int del_Part)
{
	int		iPart;

	if( del_Part >= 0 && del_Part < m_nParts )
	{
		if( m_Points[del_Part] != NULL )
		{
			API_Free(m_Points[del_Part]);
		}

		m_nParts--;

		for(iPart=del_Part; iPart<m_nParts; iPart++)
		{
			m_Points[iPart]	= m_Points[iPart + 1];
			m_nPoints[iPart]	= m_nPoints[iPart + 1];
		}

		m_Points	= (TGEO_Point **)API_Realloc(m_Points, m_nParts * sizeof(TGEO_Point *));
		m_nPoints	= (int          *)API_Realloc(m_nPoints, m_nParts * sizeof(int));

		_Extent_Invalidate();
	}

	return( m_nParts );
}

//---------------------------------------------------------
int CShape_Points::Del_AllParts(void)
{
	int		iPart;

	for(iPart=m_nParts-1; iPart>=0; iPart--)
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
int CShape_Points::Add_Point(double x, double y, int iPart)
{
	return( Ins_Point(x, y, iPart >= 0 && iPart < m_nParts ? m_nPoints[iPart] : 0, iPart) );
}

//---------------------------------------------------------
int CShape_Points::Ins_Point(double x, double y, int iPoint, int iPart)
{
	int		i;

	if( iPart >= m_nParts )
	{
		for(i=m_nParts; i<=iPart; i++)
		{
			_Add_Part();
		}
	}

	if( iPart >= 0 && iPart < m_nParts && iPoint >= 0 && iPoint <= m_nPoints[iPart] )
	{
		m_Points[iPart]	= (TGEO_Point *)API_Realloc(m_Points[iPart], (m_nPoints[iPart] + 1) * sizeof(TGEO_Point));

		for(i=m_nPoints[iPart]; i>iPoint; i--)
		{
			m_Points[iPart][i]	= m_Points[iPart][i - 1];
		}

		m_nPoints[iPart]++;

		m_Points[iPart][iPoint].x	= x;
		m_Points[iPart][iPoint].y	= y;

		_Extent_Invalidate();

		return( m_nPoints[iPart] );
	}

	return( -1 );
}

//---------------------------------------------------------
int CShape_Points::Set_Point(double x, double y, int iPoint, int iPart)
{
	TGEO_Point	*pPoint;

	if( iPart >= 0 && iPart < m_nParts && iPoint >= 0 && iPoint < m_nPoints[iPart] )
	{
		pPoint			= m_Points[iPart] + iPoint;

		pPoint->x		= x;
		pPoint->y		= y;

		_Extent_Invalidate();

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CShape_Points::Del_Point(int del_Point, int iPart)
{
	int		iPoint;

	if( iPart >= 0 && iPart < m_nParts && del_Point >= 0 && del_Point < m_nPoints[iPart] )
	{
		if( m_nPoints[iPart] <= 1 )
		{
			Del_Part(iPart);
		}
		else
		{
			m_nPoints[iPart]--;

			for(iPoint=del_Point; iPoint<m_nPoints[iPart]; iPoint++)
			{
				m_Points[iPart][iPoint]	= m_Points[iPart][iPoint + 1];
			}

			m_Points[iPart]	= (TGEO_Point *)API_Realloc(m_Points[iPart], m_nPoints[iPart] * sizeof(TGEO_Point));
		}

		_Extent_Invalidate();

		return( 1 );
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShape_Points::_Extent_Update(void)
{
	int			iPart, iPoint;
	TGEO_Point	*pPoint;
	TGEO_Rect	r;

	if( m_bUpdate )
	{
		r.xMin	=  1.0;
		r.xMax	= -1.0;

		for(iPart=0; iPart<m_nParts; iPart++)
		{
			for(iPoint=0, pPoint=m_Points[iPart]; iPoint<m_nPoints[iPart]; iPoint++, pPoint++)
			{
				if( r.xMin > r.xMax )
				{
					r.xMin	= r.xMax	= pPoint->x;
					r.yMin	= r.yMax	= pPoint->y;
				}
				else
				{
					if( r.xMin > pPoint->x )
					{
						r.xMin	= pPoint->x;
					}
					else if( r.xMax < pPoint->x )
					{
						r.xMax	= pPoint->x;
					}

					if( r.yMin > pPoint->y )
					{
						r.yMin	= pPoint->y;
					}
					else if( r.yMax < pPoint->y )
					{
						r.yMax	= pPoint->y;
					}
				}
			}
		}

		m_Extent.Assign(r);

		m_bUpdate	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CShape_Points::Get_Distance(TGEO_Point Point)
{
	TGEO_Point	Next;

	return( Get_Distance(Point, Next) );
}

//---------------------------------------------------------
double CShape_Points::Get_Distance(TGEO_Point Point, TGEO_Point &Next)
{
	int			iPart;
	double		d, Distance;
	TGEO_Point	pt;

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
double CShape_Points::Get_Distance(TGEO_Point Point, int iPart)
{
	TGEO_Point	Next;

	return( Get_Distance(Point, Next, iPart) );
}

//---------------------------------------------------------
double CShape_Points::Get_Distance(TGEO_Point Point, TGEO_Point &Next, int iPart)
{
	int			i;
	double		d, Distance;
	TGEO_Point	*pA;

	Distance	= -1.0;

	if( iPart >= 0 && iPart < m_nParts )
	{
		for(i=0, pA=m_Points[iPart]; i<m_nPoints[iPart] && Distance!=0.0; i++, pA++)
		{
			if(	(d = GEO_Get_Distance(Point, *pA)) < Distance || Distance < 0.0 )
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
int CShape_Points::On_Intersects(TGEO_Rect Extent)
{
	int			iPart, iPoint;
	TGEO_Point	*p;

	for(iPart=0; iPart<m_nParts; iPart++)
	{
		for(iPoint=0, p=m_Points[iPart]; iPoint<m_nPoints[iPart]; iPoint++, p++)
		{
			if(	Extent.xMin <= p->x && p->x <= Extent.xMax
			&&	Extent.yMin <= p->y && p->y <= Extent.yMax	)
			{
				return( 1 );
			}
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
