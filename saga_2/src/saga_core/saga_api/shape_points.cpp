
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
CSG_Shape_Points::CSG_Shape_Points(CSG_Shapes *pOwner, CSG_Table_Record *pRecord)
	: CSG_Shape(pOwner, pRecord)
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

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			CSG_Shape::Add_Point(pShape->Get_Point(iPoint, iPart), iPart);
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
		bool	bOkay	= false;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			if( m_pParts[iPart]->Get_Count() > 0 )
			{
				if( !bOkay )
				{
					bOkay		= true;
					m_Extent	= m_pParts[iPart]->Get_Extent();
				}
				else
				{
					m_Extent.Union(m_pParts[iPart]->Get_Extent());
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
int CSG_Shape_Points::On_Intersects(TSG_Rect Extent)
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
