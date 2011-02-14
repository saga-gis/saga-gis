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
bool CSG_PRQuadTree_Node::Add_Point(double x, double y, double z)
{
	if( Contains(x, y) )
	{
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
			CSG_PRQuadTree_Leaf	*pLeaf	= (CSG_PRQuadTree_Leaf *)m_pChildren[i];

			if( x != pLeaf->Get_X() || y != pLeaf->Get_Y() )
			{
				CSG_PRQuadTree_Node	*pNode	= new CSG_PRQuadTree_Node(pLeaf->m_xCenter, pLeaf->m_yCenter, pLeaf->m_Size);

				pNode->Add_Point(pLeaf->Get_X(), pLeaf->Get_Y(), pLeaf->Get_Z());
				pNode->Add_Point(x, y, z);

				m_pChildren[i]	= pNode;

				delete(pLeaf);

				return( true );
			}
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
	m_pRoot		= NULL;
	m_nSelected	= 0;
	m_nPoints	= 0;
}

//---------------------------------------------------------
CSG_PRQuadTree::CSG_PRQuadTree(const TSG_Rect &Extent)
{
	m_pRoot		= NULL;
	m_nSelected	= 0;
	m_nPoints	= 0;

	Create(Extent);
}

//---------------------------------------------------------
CSG_PRQuadTree::CSG_PRQuadTree(CSG_Shapes *pShapes, int Attribute)
{
	m_pRoot		= NULL;
	m_nSelected	= 0;
	m_nPoints	= 0;

	Create(pShapes, Attribute);
}

//---------------------------------------------------------
CSG_PRQuadTree::~CSG_PRQuadTree(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_PRQuadTree::Create(const CSG_Rect &Extent)
{
	Destroy();

	if( Extent.Get_XRange() > 0.0 && Extent.Get_YRange() > 0.0 )
	{
		m_pRoot	= new CSG_PRQuadTree_Node(
			Extent.Get_XCenter(),
			Extent.Get_YCenter(),
			(0.5 + 0.01) * (Extent.Get_XRange() > Extent.Get_YRange() ? Extent.Get_XRange() : Extent.Get_YRange())
		);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_PRQuadTree::Create(CSG_Shapes *pShapes, int Attribute)
{
	Destroy();

	if( pShapes && pShapes->is_Valid() && Create(pShapes->Get_Extent()) )
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

	m_Selected.Destroy();
	m_nSelected	= 0;
	m_nPoints	= 0;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_PRQuadTree::Add_Point(double x, double y, double z)
{
	if( m_pRoot && m_pRoot->Add_Point(x, y, z) )
	{
		m_nPoints++;

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
bool CSG_PRQuadTree::Get_Nearest_Point(double x, double y, TSG_Point &Point, double &Value, double &Distance)
{
	if( m_pRoot && m_pRoot->Contains(x, y) )
	{
		double	maxDistance	= -1.0, p[4];

		_Get_Nearest_Point(m_pRoot, x, y, maxDistance, p);

		Point.x		= p[0];
		Point.y		= p[1];
		Value		= p[2];
		Distance	= p[3];

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_PRQuadTree::_Get_Nearest_Point(CSG_PRQuadTree_Item *pItem, double x, double y, double &maxDistance, double Point[4])
{
	int		i;

	if( pItem->is_Leaf() )
	{
		CSG_PRQuadTree_Leaf	*pLeaf	= (CSG_PRQuadTree_Leaf *)pItem;

		double	d	= SG_Get_Distance(x, y, pLeaf->Get_X(), pLeaf->Get_Y());

		if( maxDistance < 0.0 || maxDistance > d )
		{
			Point[0]	= pLeaf->Get_X();
			Point[1]	= pLeaf->Get_Y();
			Point[2]	= pLeaf->Get_Z();
			Point[3]	= maxDistance	= d;
		}
	}
	else
	{
		CSG_PRQuadTree_Item	*pChild;

		for(i=0; i<4; i++)
		{
			if( (pChild = ((CSG_PRQuadTree_Node *)pItem)->Get_Child(i)) != NULL && pChild->Contains(x, y) == true )
			{
				_Get_Nearest_Point(pChild, x, y, maxDistance, Point);
			}
		}

		for(i=0; i<4; i++)
		{
			if( (pChild = ((CSG_PRQuadTree_Node *)pItem)->Get_Child(i)) != NULL && pChild->Contains(x, y) == false )
			{
				if( maxDistance < 0.0
				||	(	maxDistance > (x < pChild->Get_xCenter() ? pChild->Get_xMin() - x : x - pChild->Get_xMax())
					&&	maxDistance > (y < pChild->Get_yCenter() ? pChild->Get_yMin() - y : y - pChild->Get_yMax())	) )
				{
					_Get_Nearest_Point(pChild, x, y, maxDistance, Point);
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
int CSG_PRQuadTree::Select_Nearest_Points(double x, double y, int maxPoints, double Radius, int iQuadrant)
{
	m_nSelected	= 0;

	if( m_pRoot )
	{
		double	maxDistance;

		if( maxPoints < 1 )
		{
			maxPoints	= m_nPoints;
		}

		if( iQuadrant != 4 )
		{
			if( m_Selected.Get_NY() != maxPoints )
			{
				m_Selected.Create(4, maxPoints);
			}

			_Get_Nearest_Points(m_pRoot, x, y, maxDistance = 0.0, Radius, iQuadrant);
		}
		else // if( iQuadrant == 4 )	// quadrant-wise search
		{
			int			n[4];
			CSG_Matrix	m[4];

			m_Selected.Create(4, (int)(maxPoints / 4.0));

			for(iQuadrant=0; iQuadrant<4; iQuadrant++)
			{
				m_nSelected	= 0;

				_Get_Nearest_Points(m_pRoot, x, y, maxDistance = 0.0, Radius, iQuadrant);

				m[iQuadrant].Create(m_Selected);
				n[iQuadrant]	= m_nSelected;
			}

			m_Selected.Create(4, maxPoints);

			for(iQuadrant=0, m_nSelected=0; iQuadrant<4; iQuadrant++)
			{
				for(int i=0; i<n[iQuadrant]; i++, m_nSelected++)
				{
					m_Selected[m_nSelected][0]	= m[iQuadrant][i][0];
					m_Selected[m_nSelected][1]	= m[iQuadrant][i][1];
					m_Selected[m_nSelected][2]	= m[iQuadrant][i][2];
					m_Selected[m_nSelected][3]	= m[iQuadrant][i][3];
				}
			}
		}
	}

	return( m_nSelected );
}

//---------------------------------------------------------
void CSG_PRQuadTree::_Get_Nearest_Points(CSG_PRQuadTree_Item *pItem, double x, double y, double &maxDistance, double Radius, int iQuadrant)
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
		if( m_nSelected < m_Selected.Get_NY() )
		{
			if( maxDistance < d )
			{
				maxDistance	= d;
			}

			m_Selected[m_nSelected][0]	= pLeaf->Get_X();
			m_Selected[m_nSelected][1]	= pLeaf->Get_Y();
			m_Selected[m_nSelected][2]	= pLeaf->Get_Z();
			m_Selected[m_nSelected][3]	= d;

			m_nSelected++;
		}
		else if( d < maxDistance )
		{
			for(i=0; i<m_Selected.Get_NY(); i++)
			{
				if( m_Selected[i][3] >= maxDistance )
				{
					m_Selected[i][0]	= pLeaf->Get_X();
					m_Selected[i][1]	= pLeaf->Get_Y();
					m_Selected[i][2]	= pLeaf->Get_Z();
					m_Selected[i][3]	= d;

					break;
				}
			}

			for(i=0, maxDistance=d; i<m_Selected.Get_NY(); i++)
			{
				if( m_Selected[i][3] > maxDistance )
				{
					maxDistance	= m_Selected[i][3];
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
				_Get_Nearest_Points(pChild, x, y, maxDistance, Radius, iQuadrant);
			}
		}

		for(i=0; i<4; i++)
		{
			if( (pChild = ((CSG_PRQuadTree_Node *)pItem)->Get_Child(i)) != NULL && pChild->Contains(x, y) == false )
			{
				if( _Radius_Intersects(x, y, Radius, iQuadrant, pChild) )
				{
					if( m_nSelected < m_Selected.Get_NY()
					||	(	maxDistance > (x < pChild->Get_xCenter() ? pChild->Get_xMin() - x : x - pChild->Get_xMax())
						&&	maxDistance > (y < pChild->Get_yCenter() ? pChild->Get_yMin() - y : y - pChild->Get_yMax())	) )
					{
						_Get_Nearest_Points(pChild, x, y, maxDistance, Radius, iQuadrant);
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
