
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shape::Intersects(TSG_Rect _Region)
{
	switch( Get_Extent().Intersects(_Region) )
	{
	case INTERSECTION_None:
		return( INTERSECTION_None );

	case INTERSECTION_Identical:
	case INTERSECTION_Contained:
		return( INTERSECTION_Contained );

	default:
	case INTERSECTION_Contains:
	case INTERSECTION_Overlaps:
		return( On_Intersects(_Region) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape::Assign(CSG_Shape *pShape, bool bAssign_Attributes)
{
	if( pShape && On_Assign(pShape) )
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
