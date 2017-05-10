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
//                shapes_selection.cpp                   //
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
bool CSG_Shapes::Select(int Index, bool bInvert)
{
	return( CSG_Table::Select(Index, bInvert) );
}

//---------------------------------------------------------
bool CSG_Shapes::Select(CSG_Shape *pShape, bool bInvert)
{
	return( CSG_Table::Select(pShape, bInvert) );
}

//---------------------------------------------------------
bool CSG_Shapes::Select(TSG_Rect Extent, bool bInvert)
{
	if( !bInvert )
	{
		CSG_Table::Select();
	}

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Shape(i)->Intersects(Extent) )
		{
			CSG_Table::Select(i, true);
		}
	}

	return( Get_Selection_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Shapes::Select(TSG_Point Point, bool bInvert)
{
	if( Get_Type() != SHAPE_TYPE_Polygon )
	{
		return( Select(CSG_Rect(Point, Point), bInvert) );
	}

	if( !bInvert )
	{
		CSG_Table::Select();
	}

	for(int i=0; i<Get_Count(); i++)
	{
		if( ((CSG_Shape_Polygon *)Get_Shape(i))->Contains(Point) )
		{
			CSG_Table::Select(i, true);
		}
	}

	return( Get_Selection_Count() > 0 );
}

//---------------------------------------------------------
const CSG_Rect & CSG_Shapes::Get_Selection_Extent(void)
{
	if( Get_Selection_Count() > 0 )
	{
		m_Extent_Selected	= Get_Selection(0)->Get_Extent();

		for(int i=1; i<(int)Get_Selection_Count(); i++)
		{
			m_Extent_Selected.Union(Get_Selection(i)->Get_Extent());
		}
	}
	else
	{
		m_Extent_Selected.Assign(0.0, 0.0, 0.0, 0.0);
	}

	return( m_Extent_Selected );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
