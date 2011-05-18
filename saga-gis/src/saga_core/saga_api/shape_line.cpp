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
//                    shape_line.cpp                     //
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
CSG_Shape_Line::CSG_Shape_Line(CSG_Shapes *pOwner, int Index)
	: CSG_Shape_Points(pOwner, Index)
{}

//---------------------------------------------------------
CSG_Shape_Line::~CSG_Shape_Line(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Line::On_Intersects(CSG_Shape *pShape)
{
	//-----------------------------------------------------
	if( pShape->Get_Type() == SHAPE_TYPE_Point || pShape->Get_Type() == SHAPE_TYPE_Points )
	{
		bool	bIn		= false;
		bool	bOut	= false;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
			{
				for(int jPoint=1; jPoint<pShape->Get_Point_Count(jPart); jPoint++)
				{
					TSG_Point	Point;

					if( Get_Distance(pShape->Get_Point(jPoint, jPart), Point, iPart) == 0.0 )
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

		if( bIn )
		{
			return( INTERSECTION_Contained );
		}
	}

	//-----------------------------------------------------
	else if( pShape->Get_Type() == SHAPE_TYPE_Line )
	{
		TSG_Point	iA, iB, jA, jB, Crossing;

		for(int iPart=0; iPart<m_nParts; iPart++)
		{
			if( Get_Point_Count(iPart) > 1 )
			{
				iA	= Get_Point(0, iPart);

				for(int iPoint=1; iPoint<Get_Point_Count(iPart); iPoint++)
				{
					iB	= iA;
					iA	= Get_Point(iPoint, iPart);

					for(int jPart=0; jPart<pShape->Get_Part_Count(); jPart++)
					{
						if( pShape->Get_Point_Count(jPart) > 1 )
						{
							jA	= pShape->Get_Point(0, jPart);

							for(int jPoint=1; jPoint<pShape->Get_Point_Count(jPart); jPoint++)
							{
								jB	= jA;
								jA	= pShape->Get_Point(jPoint, jPart);

								if( SG_Get_Crossing(Crossing, iA, iB, jA, jB) )
								{
									return( INTERSECTION_Overlaps );
								}
							}
						}
					}
				}
			}
		}
	}

	return( INTERSECTION_None );
}

//---------------------------------------------------------
TSG_Intersection CSG_Shape_Line::On_Intersects(TSG_Rect Region)
{
	// called if polygon's bounding box contains or overlaps with region.
	// now let's figure out how region intersects with polygon itself

	//-----------------------------------------------------
	for(int iPart=0; iPart<m_nParts; iPart++)
	{
		CSG_Shape_Part	*pPart	= m_pParts[iPart];

		switch( pPart->Get_Extent().Intersects(Region) )
		{
		case INTERSECTION_None:			// region and line part are distinct
			break;

		case INTERSECTION_Identical:	// region contains line part
		case INTERSECTION_Contained:
			return( Get_Extent().Intersects(Region) );

		case INTERSECTION_Contains:
		case INTERSECTION_Overlaps:		// region overlaps with line part's extent, now let's look at the line part itself!
			if( pPart->Get_Count() > 1 )
			{
				TSG_Point	*pa, *pb, c;

				pb	= pPart->m_Points;
				pa	= pb + 1;

				for(int iPoint=1; iPoint<pPart->Get_Count(); iPoint++, pb=pa++)
				{
					if( SG_Get_Crossing_InRegion(c, *pa, *pb, Region) )
					{
						return( INTERSECTION_Overlaps );
					}
				}
			}
			break;
		}
	}

	//-----------------------------------------------------
	TSG_Point	p	= Get_Point(0, 0);

	if(	Region.xMin <= p.x && p.x <= Region.xMax
	&&	Region.yMin <= p.y && p.y <= Region.yMax )
	{
		return( INTERSECTION_Contained );
	}

	return( INTERSECTION_None );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Point CSG_Shape_Line::Get_Centroid(void)
{
	return( Get_Extent().Get_Center() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Line::Get_Length(void)
{
	int		iPart;
	double	Length;

	for(iPart=0, Length=0.0; iPart<m_nParts; iPart++)
	{
		Length	+= Get_Length(iPart);
	}

	return( Length );
}

//---------------------------------------------------------
double CSG_Shape_Line::Get_Length(int iPart)
{
	int			iPoint;
	double		Length;
	TSG_Point	*pA, *pB;

	if( iPart >= 0 && iPart < m_nParts && m_pParts[iPart]->Get_Count() > 1 )
	{
		pB	= m_pParts[iPart]->m_Points;
		pA	= pB + 1;

		for(iPoint=1, Length=0.0; iPoint<m_pParts[iPart]->Get_Count(); iPoint++, pB=pA++)
		{
			Length	+= SG_Get_Distance(*pA, *pB);
		}

		return( Length );
	}

	return( 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Shape_Line::Get_Distance(TSG_Point Point, TSG_Point &Next, int iPart)
{
	int			i;
	double		d, Distance;
	TSG_Point	*pA, *pB, pt;

	Distance	= -1.0;

	if( iPart >= 0 && iPart < m_nParts && m_pParts[iPart]->Get_Count() > 1 )
	{
		pB	= m_pParts[iPart]->m_Points;
		pA	= pB + 1;

		Distance	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, Next);

		for(i=1; i<m_pParts[iPart]->Get_Count() && Distance!=0.0; i++, pB=pA++)
		{
			if(	(d = SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, pt)) >= 0.0
			&&	(d < Distance || Distance < 0.0) )
			{
				Distance	= d;
				Next		= pt;
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
