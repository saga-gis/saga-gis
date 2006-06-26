
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
//                  shapes_search.cpp                    //
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
CShapes_Search::CShapes_Search(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CShapes_Search::CShapes_Search(CShapes *pPoints)
{
	_On_Construction();

	Create(pPoints);
}

//---------------------------------------------------------
void CShapes_Search::_On_Construction(void)
{
	m_pPoints		= NULL;
	m_nPoints		= 0;

	m_nSelected		= 0;
	m_Selected		= NULL;
	m_Selected_Dst	= NULL;
	m_Selected_Idx	= NULL;
	m_Selected_Buf	= 0;

	m_Idx			= NULL;
	m_Pos			= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Search::~CShapes_Search(void)
{
	Destroy();
}

//---------------------------------------------------------
void CShapes_Search::Destroy(void)
{
	if( m_nPoints > 0 )
	{
		API_Free(m_Idx);
		API_Free(m_Pos);
	}

	m_Idx			= NULL;
	m_Pos			= NULL;

	m_pPoints		= NULL;
	m_nPoints		= 0;

	if( m_Selected )
	{
		API_Free(m_Selected);
		API_Free(m_Selected_Dst);
		API_Free(m_Selected_Idx);
	}

	m_Selected		= NULL;
	m_Selected_Dst	= NULL;
	m_Selected_Idx	= NULL;
	m_nSelected		= 0;
	m_Selected_Buf	= 0;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Search::Create(CShapes *apPoints)
{
	int		iPoint;
	double	*Value;

	Destroy();

	if( apPoints && apPoints->Get_Type() == SHAPE_TYPE_Point && apPoints->Get_Count() > 1 )
	{
		m_pPoints	= apPoints;
		m_nPoints	= m_pPoints->Get_Count();

		//-------------------------------------------------
		Value		= (double     *)API_Malloc(m_nPoints * sizeof(double));
		m_Pos		= (TGEO_Point *)API_Malloc(m_nPoints * sizeof(TGEO_Point));
		m_Idx		= (int        *)API_Malloc(m_nPoints * sizeof(int));

		for(iPoint=0; iPoint<m_nPoints; iPoint++)
		{
			Value[iPoint]	= m_pPoints->Get_Shape(iPoint)->Get_Point(0).x;
		}

		MAT_Create_Index(m_nPoints, Value, true, m_Idx);

		for(iPoint=0; iPoint<m_nPoints; iPoint++)
		{
			m_Pos[iPoint]	= m_pPoints->Get_Shape(m_Idx[iPoint])->Get_Point(0);
		}

		//-------------------------------------------------
		API_Free(Value);

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
int CShapes_Search::_Get_Index_Next(double Position)
{
	int		i, iLo, iHi;

	if( m_Pos[0].x > Position )
	{
		return( 0 );
	}
	else if( m_Pos[m_nPoints - 1].x < Position )
	{
		return( m_nPoints - 1 );
	}

	for(iLo=0, iHi=m_nPoints-1; iHi-iLo>1; )
	{
		i	= iLo + (iHi - iLo) / 2;

		if( m_Pos[i].x <= Position )
		{
			iLo	= i;
		}
		else
		{
			iHi	= i;
		}
	}

	return( Position - m_Pos[iLo].x < m_Pos[iHi].x - Position ? iLo : iHi );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShape * CShapes_Search::Get_Point_Nearest(double x, double y)
{
	int			ax, ix, iPoint_Min;
	double		dx, dy, Dist, Dist_Min;

	//-----------------------------------------------------
	iPoint_Min	= -1;
	ax			= _Get_Index_Next(x);

	//-----------------------------------------------------
	for(ix=ax, Dist_Min=-1.0; ix<m_nPoints; ix++)
	{
		dy		= m_Pos[ix].y - y;
		dx		= m_Pos[ix].x - x;

		if( iPoint_Min >= 0 && Dist_Min < dx )
		{
			break;
		}
		else
		{
			Dist	= sqrt(dx*dx + dy*dy);

			if( iPoint_Min < 0 || Dist < Dist_Min )
			{
				iPoint_Min	= m_Idx[ix];
				Dist_Min	= Dist;
			}
		}
	}

	//-----------------------------------------------------
	for(ix=ax-1; ix>=0; ix--)
	{
		dy		= m_Pos[ix].y - y;
		dx		= m_Pos[ix].x - x;

		if( iPoint_Min >= 0 && Dist_Min < dx )
		{
			break;
		}
		else
		{
			Dist	= sqrt(dx*dx + dy*dy);

			if( iPoint_Min < 0 || Dist < Dist_Min )
			{
				iPoint_Min	= m_Idx[ix];
				Dist_Min	= Dist;
			}
		}
	}

	return( iPoint_Min < 0 ? NULL : m_pPoints->Get_Shape(iPoint_Min) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShape * CShapes_Search::Get_Point_Nearest(double x, double y, int Quadrant)
{
	int		iPoint;

	iPoint	= _Get_Point_Nearest(x, y, Quadrant);

	return( iPoint >= 0 && iPoint < m_nPoints ? m_pPoints->Get_Shape(iPoint) : NULL );
}

//---------------------------------------------------------
int CShapes_Search::_Get_Point_Nearest(double x, double y, int Quadrant)
{
	int		ax, ix, iPoint_Min;
	double	dx, dy, Dist, Dist_Min;

	//-----------------------------------------------------
	Dist_Min	= -1.0;
	iPoint_Min	= -1;
	ax			= _Get_Index_Next(x);

	switch( Quadrant )
	{
	//-----------------------------------------------------
	case 0:	// +x +y
		if( m_Pos[ax].x < x )
		{
			ax++;
		}

		for(ix=ax; ix<m_nPoints; ix++)
		{
			if( (dy = m_Pos[ix].y - y) >= 0.0 )
			{
				dx		= m_Pos[ix].x - x;

				if( iPoint_Min >= 0 && Dist_Min < dx )
				{
					return( iPoint_Min );
				}

				Dist	= sqrt(dx*dx + dy*dy);

				if( iPoint_Min < 0 || Dist < Dist_Min )
				{
					iPoint_Min	= m_Idx[ix];
					Dist_Min	= Dist;
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// +x -y
		if( m_Pos[ax].x < x )
		{
			ax++;
		}

		for(ix=ax; ix<m_nPoints; ix++)
		{
			if( (dy = m_Pos[ix].y - y) <= 0.0 )
			{
				dx		= m_Pos[ix].x - x;

				if( iPoint_Min >= 0 && Dist_Min < dx )
				{
					return( iPoint_Min );
				}

				Dist	= sqrt(dx*dx + dy*dy);

				if( iPoint_Min < 0 || Dist < Dist_Min )
				{
					iPoint_Min	= m_Idx[ix];
					Dist_Min	= Dist;
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// -x -y
		if( m_Pos[ax].x > x )
		{
			ax--;
		}

		for(ix=ax; ix>=0; ix--)
		{
			if( (dy = m_Pos[ix].y - y) <= 0.0 )
			{
				dx		= m_Pos[ix].x - x;

				if( iPoint_Min >= 0 && Dist_Min < dx )
				{
					return( iPoint_Min );
				}

				Dist	= sqrt(dx*dx + dy*dy);

				if( iPoint_Min < 0 || Dist < Dist_Min )
				{
					iPoint_Min	= m_Idx[ix];
					Dist_Min	= Dist;
				}
			}
		}
		break;

	//-----------------------------------------------------
	case 3:	// -x +y
		if( m_Pos[ax].x > x )
		{
			ax--;
		}

		for(ix=ax; ix>=0; ix--)
		{
			if( (dy = m_Pos[ix].y - y) >= 0.0 )
			{
				dx		= m_Pos[ix].x - x;

				if( iPoint_Min >= 0 && Dist_Min < dx )
				{
					return( iPoint_Min );
				}

				Dist	= sqrt(dx*dx + dy*dy);

				if( iPoint_Min < 0 || Dist < Dist_Min )
				{
					iPoint_Min	= m_Idx[ix];
					Dist_Min	= Dist;
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	return( iPoint_Min );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes_Search::_Select_Add(CShape *pPoint, double Distance)
{
	if( m_nSelected >= m_Selected_Buf )
	{
		m_Selected_Buf	+= 8;

		m_Selected		= (CShape **)API_Realloc(m_Selected    , m_Selected_Buf * sizeof(CShape *));
		m_Selected_Dst	= (double  *)API_Realloc(m_Selected_Dst, m_Selected_Buf * sizeof(double  ));
		m_Selected_Idx	= (int     *)API_Realloc(m_Selected_Idx, m_Selected_Buf * sizeof(int     ));
	}

	m_Selected    [m_nSelected]	= pPoint;
	m_Selected_Dst[m_nSelected]	= Distance;
	m_Selected_Idx[m_nSelected]	= m_nSelected++;
}

//---------------------------------------------------------
int CShapes_Search::Select_Radius(double x, double y, double Radius, bool bSort)
{
	int			ix, xLeft, xRight;
	double		d, dx, Radius_2;

	m_nSelected	= 0;

	Radius_2	= Radius * Radius;

	xLeft		= _Get_Index_Next(x - Radius);
	xRight		= _Get_Index_Next(x + Radius);

	for(ix=xLeft; ix<=xRight; ix++)
	{
		d		= m_Pos[ix].y - y;

		if( d >= -Radius && d <= Radius )
		{
			dx	= m_Pos[ix].x - x;
			d	= dx*dx + d*d;

			if( d <= Radius_2 )
			{
				_Select_Add(m_pPoints->Get_Shape(m_Idx[ix]), d);
			}
		}
	}

	if( bSort )
	{
		MAT_Create_Index(m_nSelected, m_Selected_Dst, true, m_Selected_Idx);
	}

	return( m_nSelected );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
