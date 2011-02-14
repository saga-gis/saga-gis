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
//                      shape.cpp                        //
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
CSG_Shape::CSG_Shape(CSG_Shapes *pOwner, int Index)
	: CSG_Table_Record(pOwner, Index)
{
}

//---------------------------------------------------------
CSG_Shape::~CSG_Shape(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape::Destroy(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Shape_Type CSG_Shape::Get_Type(void)
{
	return( ((CSG_Shapes *)m_pTable)->Get_Type() );
}

//---------------------------------------------------------
int CSG_Shape::Get_Point_Count(void)
{
	int		i, n;

	for(i=0, n=0; i<Get_Part_Count(); i++)
	{
		n	+= Get_Point_Count(i);
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape::Add_Point(TSG_Point Point, int iPart)
{
	return( Add_Point(Point.x, Point.y, iPart) );
}

//---------------------------------------------------------
int CSG_Shape::Ins_Point(TSG_Point Point, int iPoint, int iPart)
{
	return( Ins_Point(Point.x, Point.y, iPoint, iPart) );
}

//---------------------------------------------------------
int CSG_Shape::Set_Point(TSG_Point Point, int iPoint, int iPart)
{
	return( Set_Point(Point.x, Point.y, iPoint, iPart) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CSG_Shape::_Invalidate(void)
{
	((CSG_Shapes *)m_pTable)->Set_Update_Flag();
	
	Set_Modified();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape::Intersects(CSG_Shape *pShape)
{
	//-----------------------------------------------------
	if( !pShape || !Intersects(pShape->Get_Extent()) )
	{
		return( INTERSECTION_None );
	}

	//-----------------------------------------------------
	if( Get_Part_Count() == pShape->Get_Part_Count() && Get_Point_Count() == pShape->Get_Point_Count() )
	{
		bool	bIdentical	= true;

		for(int iPart=0; iPart<Get_Part_Count() && bIdentical; iPart++)
		{
			if( Get_Point_Count(iPart) != pShape->Get_Point_Count(iPart) )
			{
				bIdentical	= false;
			}
			else
			{
				for(int iPoint=0; iPoint<Get_Point_Count(iPart) && bIdentical; iPoint++)
				{
					CSG_Point	Point(Get_Point(iPoint, iPart));

					if( !Point.is_Equal(pShape->Get_Point(iPoint, iPart)) )
					{
						bIdentical	= false;
					}
				}
			}
		}

		if( bIdentical )
		{
			return( INTERSECTION_Identical );
		}
	}

	//-----------------------------------------------------
	if( Get_Type() >= pShape->Get_Type() )
	{
		return( On_Intersects(pShape) );
	}

	TSG_Intersection	Intersection	= pShape->On_Intersects(this);

	switch( Intersection )
	{
	default:						return( Intersection );
	case INTERSECTION_Contained:	return( INTERSECTION_Contains );
	case INTERSECTION_Contains:		return( INTERSECTION_Contained );
	}
}

//---------------------------------------------------------
TSG_Intersection CSG_Shape::Intersects(TSG_Rect Region)
{
	TSG_Intersection	Intersection	= Get_Extent().Intersects(Region);

	switch( Intersection )
	{
	default:						return( Intersection );
	case INTERSECTION_Contains:
	case INTERSECTION_Overlaps:		return( On_Intersects(Region) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape::Assign(CSG_Shape *pShape)
{
	return( Assign(pShape, true) );
}

bool CSG_Shape::Assign(CSG_Shape *pShape, bool bAssign_Attributes)
{
	if( pShape && Get_Type() == pShape->Get_Type() && On_Assign(pShape) )
	{
		if( bAssign_Attributes )
		{
			CSG_Table_Record::Assign(pShape);
		}
 
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
