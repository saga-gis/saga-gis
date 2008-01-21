
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
CSG_Shape * CSG_Shapes::Get_Selection(int Index)
{
	int				i;
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_Table.Get_Selection(Index)) != NULL )
	{
		for(i=0; i<m_nShapes; i++)
		{
			if( m_Shapes[i]->Get_Record() == pRecord )
			{
				return( m_Shapes[i] );
			}
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
bool CSG_Shapes::Select(int iShape, bool bInvert)
{
	return( m_Table.Select(iShape, bInvert) );
}

//---------------------------------------------------------
bool CSG_Shapes::Select(CSG_Shape *pShape, bool bInvert)
{
	return( m_Table.Select(pShape ? pShape->Get_Record() : NULL, bInvert) );
}

//---------------------------------------------------------
bool CSG_Shapes::Select(TSG_Rect Extent, bool bInvert)
{
	if( !bInvert )
	{
		m_Table.Select();
	}

	for(int i=0; i<m_nShapes; i++)
	{
		if( m_Shapes[i]->Intersects(Extent) )
		{
			Select(i, true);
		}
	}

	return( Get_Selection_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_Rect & CSG_Shapes::Get_Selection_Extent(void)
{
	if( Get_Selection_Count() > 0 )
	{
		m_Extent_Selected	= Get_Selection(0)->Get_Extent();

		for(int i=1; i<Get_Selection_Count(); i++)
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
int CSG_Shapes::Del_Selection(void)
{
	int		i, n	= 0;

	for(i=Get_Selection_Count()-1; i>=0; i--)
	{
		if( Del_Shape(Get_Selection(i)) )
		{
			n++;
		}
	}

	return( n );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shapes::Inv_Selection(void)
{
	for(int i=0; i<m_nShapes; i++)
	{
		Select(i, true);
	}

	return( Get_Selection_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
