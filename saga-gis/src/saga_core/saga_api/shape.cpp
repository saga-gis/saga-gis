
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape::CSG_Shape(CSG_Shapes *pOwner, sLong Index)
	: CSG_Table_Record(pOwner, Index)
{}

//---------------------------------------------------------
CSG_Shape::~CSG_Shape(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Shape::Destroy(void)
{}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Shape_Type CSG_Shape::Get_Type(void) const
{
	return( ((CSG_Shapes *)m_pTable)->Get_Type() );
}

//---------------------------------------------------------
TSG_Vertex_Type CSG_Shape::Get_Vertex_Type(void) const
{
	return( ((CSG_Shapes *)m_pTable)->Get_Vertex_Type() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CSG_Shape::_Invalidate(void)
{
	((CSG_Shapes *)m_pTable)->Set_Update_Flag();
	
	Set_Modified();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Intersection CSG_Shape::Intersects(CSG_Shape *pShape)
{
	if( !pShape || !Intersects(pShape->Get_Extent()) )
	{
		return( INTERSECTION_None );
	}

	//-----------------------------------------------------
	if( Get_Extent().is_Equal(pShape->Get_Extent()) && Get_Part_Count() == pShape->Get_Part_Count() && Get_Point_Count() == pShape->Get_Point_Count() )
	{
		bool bIdentical = true;

		for(int iPart=0; iPart<Get_Part_Count() && bIdentical; iPart++)
		{
			if( Get_Point_Count(iPart) != pShape->Get_Point_Count(iPart) )
			{
				bIdentical = false;
			}
			else for(int iPoint=0; iPoint<Get_Point_Count(iPart) && bIdentical; iPoint++)
			{
				CSG_Point Point(Get_Point(iPoint, iPart));

				if( !Point.is_Equal(pShape->Get_Point(iPoint, iPart)) )
				{
					bIdentical = false;
				}
			}
		}

		if( bIdentical )
		{
			return( INTERSECTION_Identical );
		}
	}

	//-----------------------------------------------------
	TSG_Intersection Intersection;

	if( Get_Type() >= pShape->Get_Type() && (Intersection = On_Intersects(pShape)) != INTERSECTION_None )
	{
		return( Intersection );
	}

	Intersection = pShape->On_Intersects(this);

	if( Intersection == INTERSECTION_Contained )
	{
		return( INTERSECTION_Contains );
	}

	if( Intersection == INTERSECTION_Contains )
	{
		return( INTERSECTION_Contained );
	}

	return( Intersection );
}

//---------------------------------------------------------
TSG_Intersection CSG_Shape::Intersects(const TSG_Rect &Region)
{
	TSG_Intersection Intersection = Get_Extent().Intersects(Region);

	switch( Intersection )
	{
	default                   : return( Intersection );
	case INTERSECTION_Contains:
	case INTERSECTION_Overlaps: return( On_Intersects(Region) );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shape::Assign(CSG_Table_Record *pRecord)
{
	return( Assign(pRecord, SHAPE_COPY) );
}

bool CSG_Shape::Assign(CSG_Table_Record *pRecord, bool bAssign_Attributes)
{
	return( Assign(pRecord, bAssign_Attributes ? SHAPE_COPY : SHAPE_COPY_GEOM) );
}

bool CSG_Shape::Assign(CSG_Table_Record *pRecord, TSG_ADD_Shape_Copy_Mode mCopy)
{
	if( pRecord )
	{
		if( (mCopy == SHAPE_COPY || mCopy == SHAPE_COPY_GEOM) && pRecord->Get_Table()->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{
			CSG_Shape *pShape = (CSG_Shape *)pRecord;

			if( Get_Type() == pShape->Get_Type() )
			{
				On_Assign(pShape);
			}
		}

		if( (mCopy == SHAPE_COPY || mCopy == SHAPE_COPY_ATTR) )
		{
			return( CSG_Table_Record::Assign(pRecord) );
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
