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
//                     quadtree.cpp                      //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PRQuadTree_Node::CSG_PRQuadTree_Node(double xCenter, double yCenter, double Size)
	: CSG_PRQuadTree_Item(xCenter, yCenter, Size)
{
	m_pChildren[0]	=
	m_pChildren[1]	=
	m_pChildren[2]	=
	m_pChildren[3]	= NULL;
}

//---------------------------------------------------------
CSG_PRQuadTree_Node::CSG_PRQuadTree_Node(CSG_PRQuadTree_Leaf *pLeaf)
	: CSG_PRQuadTree_Item(pLeaf->m_xCenter, pLeaf->m_yCenter, pLeaf->m_Size)
{
	m_pChildren[0]	=
	m_pChildren[1]	=
	m_pChildren[2]	=
	m_pChildren[3]	= NULL;

	int		i		=  pLeaf->Get_Y() < m_yCenter
					? (pLeaf->Get_X() < m_xCenter ? 0 : 3)
					: (pLeaf->Get_X() < m_xCenter ? 1 : 2);

	pLeaf->m_Size	= 0.5 * m_Size;

	switch( i )
	{
	case 0:	m_pChildren[i]	= pLeaf; pLeaf->m_xCenter -= pLeaf->m_Size; pLeaf->m_yCenter -= pLeaf->m_Size;	break;
	case 1:	m_pChildren[i]	= pLeaf; pLeaf->m_xCenter -= pLeaf->m_Size; pLeaf->m_yCenter += pLeaf->m_Size;	break;
	case 2:	m_pChildren[i]	= pLeaf; pLeaf->m_xCenter += pLeaf->m_Size; pLeaf->m_yCenter += pLeaf->m_Size;	break;
	case 3:	m_pChildren[i]	= pLeaf; pLeaf->m_xCenter += pLeaf->m_Size; pLeaf->m_yCenter -= pLeaf->m_Size;	break;
	}
}

//---------------------------------------------------------
CSG_PRQuadTree_Node::~CSG_PRQuadTree_Node(void)
{
	for(int i=0; i<4; i++)
	{
		if( m_pChildren[i] )
		{
			if( m_pChildren[i]->is_Leaf() )
			{
				delete((CSG_PRQuadTree_Leaf *)m_pChildren[i]);
			}
			else
			{
				delete((CSG_PRQuadTree_Node *)m_pChildren[i]);
			}
		}
	}
}

//---------------------------------------------------------
CSG_PRQuadTree_Item * CSG_PRQuadTree_Node::Get_Child(double x, double y)
{
	for(int i=0; i<4; i++)
	{
		if( m_pChildren[i] && m_pChildren[i]->Contains(x, y) )
		{
			return( m_pChildren[i]->is_Node() ? m_pChildren[i]->asNode()->Get_Child(x, y) : m_pChildren[i] );
		}
	}

	return( this );
}

//---------------------------------------------------------
bool CSG_PRQuadTree_Node::Add_Point(double x, double y, double z)
{
	if( Contains(x, y) )
	{
		if( has_Statistics() )
		{
			Get_X()->Add_Value(x);
			Get_Y()->Add_Value(y);
			Get_Z()->Add_Value(z);
		}

		int		i	=  y < m_yCenter ? (x < m_xCenter ? 0 : 3) : (x < m_xCenter ? 1 : 2);

		//-------------------------------------------------
		if( m_pChildren[i] == NULL )
		{
			double	Size	= 0.5 * m_Size;

			switch( i )
			{
			case 0:	m_pChildren[i]	= new CSG_PRQuadTree_Leaf(m_xCenter - Size, m_yCenter - Size, Size, x, y, z);	break;
			case 1:	m_pChildren[i]	= new CSG_PRQuadTree_Leaf(m_xCenter - Size, m_yCenter + Size, Size, x, y, z);	break;
			case 2:	m_pChildren[i]	= new CSG_PRQuadTree_Leaf(m_xCenter + Size, m_yCenter + Size, Size, x, y, z);	break;
			case 3:	m_pChildren[i]	= new CSG_PRQuadTree_Leaf(m_xCenter + Size, m_yCenter - Size, Size, x, y, z);	break;
			}

			return( true );
		}

		//-----------------------------------------------------
		else if( m_pChildren[i]->is_Leaf() )
		{
			CSG_PRQuadTree_Leaf	*pLeaf	= m_pChildren[i]->asLeaf();

			if( x != pLeaf->Get_X() || y != pLeaf->Get_Y() )
			{
				if( has_Statistics() )
				{
					m_pChildren[i]	= new CSG_PRQuadTree_Node_Statistics	(pLeaf);
				}
				else
				{
					m_pChildren[i]	= new CSG_PRQuadTree_Node				(pLeaf);
				}

				((CSG_PRQuadTree_Node *)m_pChildren[i])->Add_Point(x, y, z);
			}
			else
			{
				if( !pLeaf->has_Statistics() )
				{
					m_pChildren[i]	= new CSG_PRQuadTree_Leaf_List(pLeaf->m_xCenter, pLeaf->m_yCenter, pLeaf->m_Size, x, y, pLeaf->Get_Z());

					delete(pLeaf);
				}

				((CSG_PRQuadTree_Leaf_List *)m_pChildren[i])->Add_Value(z);
			}

			return( true );
		}

		//-------------------------------------------------
		else // if( m_pChildren[i]->is_Node() )
		{
			return( ((CSG_PRQuadTree_Node *)m_pChildren[i])->Add_Point(x, y, z) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PRQuadTree::CSG_PRQuadTree(void)
{
	m_pRoot			= NULL;
	m_nPoints		= 0;

	m_Selected.Create(sizeof(TLeaf), 0, SG_ARRAY_GROWTH_3);
}

//---------------------------------------------------------
CSG_PRQuadTree::CSG_PRQuadTree(const TSG_Rect &Extent, bool bStatistics)
{
	m_pRoot			= NULL;
	m_nPoints		= 0;

	m_Selected.Create(sizeof(TLeaf), 0, SG_ARRAY_GROWTH_3);

	Create(Extent, bStatistics);
}

//---------------------------------------------------------
CSG_PRQuadTree::CSG_PRQuadTree(CSG_Shapes *pShapes, int Attribute, bool bStatistics)
{
	m_pRoot			= NULL;
	m_nPoints		= 0;

	m_Selected.Create(sizeof(TLeaf), 0, SG_ARRAY_GROWTH_3);

	Create(pShapes, Attribute, bStatistics);
}

//---------------------------------------------------------
CSG_PRQuadTree::~CSG_PRQuadTree(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_PRQuadTree::Create(const CSG_Rect &Extent, bool bStatistics)
{
	Destroy();

	if( Extent.Get_XRange() > 0.0 && Extent.Get_YRange() > 0.0 )
	{
		if( bStatistics )
		{
			m_pRoot	= new CSG_PRQuadTree_Node_Statistics(
				Extent.Get_XCenter(),
				Extent.Get_YCenter(),
				(0.5 + 0.01) * (Extent.Get_XRange() > Extent.Get_YRange() ? Extent.Get_XRange() : Extent.Get_YRange())
			);
		}
		else
		{
			m_pRoot	= new CSG_PRQuadTree_Node(
				Extent.Get_XCenter(),
				Extent.Get_YCenter(),
				(0.5 + 0.01) * (Extent.Get_XRange() > Extent.Get_YRange() ? Extent.Get_XRange() : Extent.Get_YRange())
			);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PRQuadTree::Create(CSG_Shapes *pShapes, int Attribute, bool bStatistics)
{
	Destroy();

	if( pShapes && pShapes->is_Valid() && Create(pShapes->Get_Extent(), bStatistics) )
	{
		for(int iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

			if( Attribute < 0 || !pShape->is_NoData(Attribute) )
			{
				double	z	= Attribute < 0 ? iShape : pShape->asDouble(Attribute);

				for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

						Add_Point(p.x, p.y, z);
					}
				}
			}
		}

		return( Get_Point_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_PRQuadTree::Destroy(void)
{
	if( m_pRoot )
	{
		delete(m_pRoot);

		m_pRoot	= NULL;
	}

	m_nPoints		= 0;

	m_Selected.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PRQuadTree::Add_Point(double x, double y, double z)
{
	if( _Check_Root(x, y) && m_pRoot->Add_Point(x, y, z) )
	{
		m_nPoints++;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PRQuadTree::Add_Point(const TSG_Point &p, double z)
{
	return( Add_Point(p.x, p.y, z) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PRQuadTree::_Check_Root(double x, double y)
{
	if( !m_pRoot )
	{
		return( false );
	}
	
	if( m_pRoot->Get_Extent().Contains(x, y) )
	{
		return( true );
	}

	//-----------------------------------------------------
	CSG_PRQuadTree_Node	*pRoot;

	if( m_pRoot->has_Statistics() )
	{
		CSG_PRQuadTree_Node_Statistics	*pNode	= new CSG_PRQuadTree_Node_Statistics(
			x < m_pRoot->Get_xMin() ? m_pRoot->Get_xMin() : m_pRoot->Get_xMax(),
			y < m_pRoot->Get_yMin() ? m_pRoot->Get_yMin() : m_pRoot->Get_yMax(),
			2.0 * m_pRoot->Get_Size()
		);

		pNode->m_x	= ((CSG_PRQuadTree_Node_Statistics *)m_pRoot)->m_x;
		pNode->m_y	= ((CSG_PRQuadTree_Node_Statistics *)m_pRoot)->m_y;
		pNode->m_z	= ((CSG_PRQuadTree_Node_Statistics *)m_pRoot)->m_z;

		pRoot	= pNode;
	}
	else
	{
		pRoot	= new CSG_PRQuadTree_Node(
			x < m_pRoot->Get_xMin() ? m_pRoot->Get_xMin() : m_pRoot->Get_xMax(),
			y < m_pRoot->Get_yMin() ? m_pRoot->Get_yMin() : m_pRoot->Get_yMax(),
			2.0 * m_pRoot->Get_Size()
		);
	}

	int	i	=  m_pRoot->Get_yCenter() < pRoot->Get_yCenter()
			? (m_pRoot->Get_xCenter() < pRoot->Get_xCenter() ? 0 : 3)
			: (m_pRoot->Get_xCenter() < pRoot->Get_xCenter() ? 1 : 2);

	pRoot->m_pChildren[i]	= m_pRoot;

	m_pRoot	= pRoot;

	return( _Check_Root(x, y) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_PRQuadTree::_Quadrant_Contains(double x, double y, int iQuadrant, const TSG_Point &p)
{
	switch( iQuadrant )
	{
	case 0:	return( x <  p.x && y <  p.y );	// lower left
	case 1:	return( x <  p.x && y >= p.y );	// upper left
	case 2:	return( x >= p.x && y >= p.y );	// upper right
	case 3:	return( x >= p.x && y <  p.y );	// lower right
	}

	return( true );
}

//---------------------------------------------------------
inline bool CSG_PRQuadTree::_Radius_Contains(double x, double y, double r, const TSG_Point &p)
{
	double	dx, dy;

	if( fabs(dx = x - p.x) <= r && fabs(dy = y - p.y) <= r )
	{
		return( dx*dx + dy*dy < r*r );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSG_PRQuadTree::_Radius_Contains(double x, double y, double r, int iQuadrant, const TSG_Point &p)
{
	return( _Quadrant_Contains(x, y, iQuadrant, p) && _Radius_Contains(x, y, r, p) );
}

//---------------------------------------------------------
inline bool CSG_PRQuadTree::_Quadrant_Intersects(double x, double y, int iQuadrant, CSG_PRQuadTree_Item *pItem)
{
	switch( iQuadrant )
	{
	case 0:	return( x <  pItem->Get_xMax() && y <  pItem->Get_yMax() );	// lower left
	case 1:	return( x <  pItem->Get_xMax() && y >= pItem->Get_yMin() );	// upper left
	case 2:	return( x >= pItem->Get_xMin() && y >= pItem->Get_yMin() );	// upper right
	case 3:	return( x >= pItem->Get_xMin() && y <  pItem->Get_yMax() );	// lower right
	}

	return( true );
}

//---------------------------------------------------------
inline bool CSG_PRQuadTree::_Radius_Intersects(double x, double y, double r, CSG_PRQuadTree_Item *pItem)
{
	if( r <= 0.0 )
	{
		return( true );
	}

	if(	pItem->Get_xMax() < x - r || pItem->Get_xMin() > x + r
	||	pItem->Get_yMax() < y - r || pItem->Get_yMin() > y + r )
	{
		return( false );
	}

	if( (pItem->Get_xMin() <= x && x <= pItem->Get_xMax())
	||	(pItem->Get_yMin() <= y && y <= pItem->Get_yMax()) )
	{
		return( true );
	}

	TSG_Point	p;

	if( pItem->Get_xMax() < x )
	{
		p.x	= pItem->Get_xMax();
		p.y	= pItem->Get_yMax() < y ? pItem->Get_yMax() : pItem->Get_yMin();
	}
	else // if( pItem->Get_xMin() >= x )
	{
		p.x	= pItem->Get_xMin();
		p.y	= pItem->Get_yMax() < y ? pItem->Get_yMax() : pItem->Get_yMin();
	}

	return( _Radius_Contains(x, y, r, p) );
}

//---------------------------------------------------------
inline bool CSG_PRQuadTree::_Radius_Intersects(double x, double y, double r, int iQuadrant, CSG_PRQuadTree_Item *pItem)
{
	return( _Quadrant_Intersects(x, y, iQuadrant, pItem) && _Radius_Intersects(x, y, r, pItem) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PRQuadTree::_Add_Selected(CSG_PRQuadTree_Leaf *pLeaf, double Distance)
{
	if( m_Selected.Inc_Array() )
	{
		TLeaf	*pL		= _Get_Selected(Get_Selected_Count() - 1);

		pL->pLeaf		= pLeaf;
		pL->Distance	= Distance;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PRQuadTree::_Set_Selected(int i, CSG_PRQuadTree_Leaf *pLeaf, double Distance)
{
	TLeaf	*pL		= _Get_Selected(i);

	if( pL )
	{
		pL->pLeaf		= pLeaf;
		pL->Distance	= Distance;

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PRQuadTree_Leaf * CSG_PRQuadTree::Get_Nearest_Leaf(const TSG_Point &p, double &Distance)
{
	return( Get_Nearest_Leaf(p.x, p.y, Distance) );
}

CSG_PRQuadTree_Leaf * CSG_PRQuadTree::Get_Nearest_Leaf(double x, double y, double &Distance)
{
	return( _Get_Nearest_Point(m_pRoot, x, y, Distance = -1) );
}

//---------------------------------------------------------
bool CSG_PRQuadTree::Get_Nearest_Point(const TSG_Point &p, TSG_Point &Point, double &Value, double &Distance)
{
	return( Get_Nearest_Point(p.x, p.y, Point, Value, Distance) );
}

bool CSG_PRQuadTree::Get_Nearest_Point(double x, double y, TSG_Point &Point, double &Value, double &Distance)
{
	CSG_PRQuadTree_Leaf	*pLeaf	= _Get_Nearest_Point(m_pRoot, x, y, Distance = -1);

	if( pLeaf )
	{
		Point.x		= pLeaf->Get_X();
		Point.y		= pLeaf->Get_Y();
		Value		= pLeaf->Get_Z();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_PRQuadTree_Leaf	* CSG_PRQuadTree::_Get_Nearest_Point(CSG_PRQuadTree_Item *pItem, double x, double y, double &Distance)
{
	if( pItem )
	{
		if( pItem->is_Leaf() )
		{
			CSG_PRQuadTree_Leaf	*pLeaf	= (CSG_PRQuadTree_Leaf *)pItem;

			double	d	= SG_Get_Distance(x, y, pLeaf->Get_X(), pLeaf->Get_Y());

			if( Distance < 0.0 || Distance > d )
			{
				Distance	= d;

				return( pLeaf );
			}
		}
		else // if( pItem->is_Node() )
		{
			CSG_PRQuadTree_Leaf	*pLeaf, *pNearest	= NULL;

			if( pItem->Contains(x, y) )
			{
				for(int i=0; i<4; i++)
				{
					if( (pLeaf = _Get_Nearest_Point(pItem->asNode()->Get_Child(i), x, y, Distance)) != NULL )
					{
						pNearest	= pLeaf;
					}
				}
			}
			else if(	Distance < 0.0
				||	(	Distance > (x < pItem->Get_xCenter() ? pItem->Get_xMin() - x : x - pItem->Get_xMax())
					&&	Distance > (y < pItem->Get_yCenter() ? pItem->Get_yMin() - y : y - pItem->Get_yMax())	) )
			{
				for(int i=0; i<4; i++)
				{
					if( (pLeaf = _Get_Nearest_Point(pItem->asNode()->Get_Child(i), x, y, Distance)) != NULL )
					{
						pNearest	= pLeaf;
					}
				}
			}

			return( pNearest );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_PRQuadTree::Select_Nearest_Points(const TSG_Point &p, int maxPoints, double Radius, int iQuadrant)
{
	return( Select_Nearest_Points(p.x, p.y, maxPoints, Radius, iQuadrant) );
}

//---------------------------------------------------------
int CSG_PRQuadTree::Select_Nearest_Points(double x, double y, int maxPoints, double Radius, int iQuadrant)
{
	m_Selected.Destroy();

	if( m_pRoot )
	{
		double	Distance;

		if( maxPoints < 1 )
		{
			maxPoints	= m_nPoints;
		}

		if( iQuadrant != 4 )
		{
			_Get_Nearest_Points(m_pRoot, x, y, Distance = 0.0, Radius, maxPoints, iQuadrant);
		}
		else // if( iQuadrant == 4 )	// quadrant-wise search
		{
			for(iQuadrant=0; iQuadrant<4; iQuadrant++)
			{
				_Get_Nearest_Points(m_pRoot, x, y, Distance = 0.0, Radius, maxPoints, iQuadrant);
			}
		}
	}

	return( Get_Selected_Count() );
}

//---------------------------------------------------------
void CSG_PRQuadTree::_Get_Nearest_Points(CSG_PRQuadTree_Item *pItem, double x, double y, double &Distance, double Radius, int maxPoints, int iQuadrant)
{
	int		i;

	//-----------------------------------------------------
	if( pItem->is_Leaf() )
	{
		CSG_PRQuadTree_Leaf	*pLeaf	= (CSG_PRQuadTree_Leaf *)pItem;

		if( _Quadrant_Contains(x, y, iQuadrant, pLeaf->Get_Point()) == false )
		{
			return;
		}
		
		double	d	= SG_Get_Distance(x, y, pLeaf->Get_X(), pLeaf->Get_Y());

		if( Radius > 0.0 && Radius < d )
		{
			return;
		}

		//-------------------------------------------------
		if( Get_Selected_Count() < maxPoints )
		{
			if( Distance < d )
			{
				Distance	= d;
			}

			_Add_Selected(pLeaf, d);
		}
		else if( d < Distance )
		{
			for(i=0; i<maxPoints; i++)
			{
				if( Distance <= Get_Selected_Distance(i) )
				{
					_Set_Selected(i, pLeaf, d);

					break;
				}
			}

			for(i=0, Distance=d; i<maxPoints; i++)
			{
				if( Distance < Get_Selected_Distance(i) )
				{
					Distance	= Get_Selected_Distance(i);
				}
			}
		}
	}

	//-----------------------------------------------------
	else // if( pItem->is_Node() )
	{
		CSG_PRQuadTree_Item	*pChild;

		for(i=0; i<4; i++)
		{
			if( (pChild = ((CSG_PRQuadTree_Node *)pItem)->Get_Child(i)) != NULL && pChild->Contains(x, y) == true )
			{
				_Get_Nearest_Points(pChild, x, y, Distance, Radius, maxPoints, iQuadrant);
			}
		}

		for(i=0; i<4; i++)
		{
			if( (pChild = ((CSG_PRQuadTree_Node *)pItem)->Get_Child(i)) != NULL && pChild->Contains(x, y) == false )
			{
				if( _Radius_Intersects(x, y, Radius, iQuadrant, pChild) )
				{
					if( Get_Selected_Count() < maxPoints
					||	(	Distance > (x < pChild->Get_xCenter() ? pChild->Get_xMin() - x : x - pChild->Get_xMax())
						&&	Distance > (y < pChild->Get_yCenter() ? pChild->Get_yMin() - y : y - pChild->Get_yMax())	) )
					{
						_Get_Nearest_Points(pChild, x, y, Distance, Radius, maxPoints, iQuadrant);
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
