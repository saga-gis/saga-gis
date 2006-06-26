
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
CShape::CShape(CShapes *pOwner, CTable_Record *pRecord)
{
	m_pOwner	= pOwner;
	m_pRecord	= pRecord;
}

//---------------------------------------------------------
CShape::~CShape(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShape::Destroy(void)
{
	m_pRecord	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TShape_Type CShape::Get_Type(void)
{
	return( m_pOwner->Get_Type() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShape::Add_Point(TGEO_Point Point, int iPart)
{
	return( Add_Point(Point.x, Point.y, iPart) );
}

//---------------------------------------------------------
int CShape::Ins_Point(TGEO_Point Point, int iPoint, int iPart)
{
	return( Ins_Point(Point.x, Point.y, iPoint, iPart) );
}

//---------------------------------------------------------
int CShape::Set_Point(TGEO_Point Point, int iPoint, int iPart)
{
	return( Set_Point(Point.x, Point.y, iPoint, iPart) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CShape::_Extent_Invalidate(void)
{
	m_pOwner->_Extent_Invalidate();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShape::Intersects(TGEO_Rect _Region)
{
	int			Result;
	CGEO_Rect	r(_Region);

	switch( r.Intersects(Get_Extent()) )
	{
	case INTERSECTION_None:
		Result	= 0;
		break;

	case INTERSECTION_Identical:
	case INTERSECTION_Contains:
		Result	= 1;
		break;

	default:
		Result	= On_Intersects(_Region);
		break;
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShape::Assign(CShape *pShape, bool bAssign_Attributes)
{
	if( pShape && On_Assign(pShape) )
	{
		if( bAssign_Attributes )
		{
			m_pRecord->Assign(pShape->Get_Record());
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
