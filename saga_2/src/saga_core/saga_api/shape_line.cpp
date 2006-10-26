
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
CSG_Shape_Line::CSG_Shape_Line(CSG_Shapes *pOwner, CSG_Table_Record *pRecord)
	: CSG_Shape_Points(pOwner, pRecord)
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
int CSG_Shape_Line::On_Intersects(TSG_Rect Extent)
{
	int			iPart, iPoint;
	TSG_Point	*pA, *pB, Crossing;

	for(iPart=0; iPart<m_nParts; iPart++)
	{
		if( m_nPoints[iPart] > 1 )
		{
			pB	= m_Points[iPart];
			pA	= pB + 1;

			for(iPoint=1; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
			{
				if( SG_Get_Crossing_InRegion(Crossing, *pA, *pB, Extent) )
				{
					return( 1 );
				}
			}
		}
	}

	return( 0 );
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

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 1 )
	{
		pB	= m_Points[iPart];
		pA	= pB + 1;

		for(iPoint=1, Length=0.0; iPoint<m_nPoints[iPart]; iPoint++, pB=pA++)
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

	if( iPart >= 0 && iPart < m_nParts && m_nPoints[iPart] > 1 )
	{
		pB	= m_Points[iPart];
		pA	= pB + 1;

		Distance	= SG_Get_Nearest_Point_On_Line(Point, *pA, *pB, Next);

		for(i=1; i<m_nPoints[iPart] && Distance!=0.0; i++, pB=pA++)
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
