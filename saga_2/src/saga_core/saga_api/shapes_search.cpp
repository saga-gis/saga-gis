
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
CSG_Shapes_Search::CSG_Shapes_Search(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Shapes_Search::CSG_Shapes_Search(CSG_Shapes *pPoints)
{
	_On_Construction();

	Create(pPoints);
}

//---------------------------------------------------------
void CSG_Shapes_Search::_On_Construction(void)
{
	m_pPoints		= NULL;
	m_nPoints		= 0;
	m_bDestroy		= false;

	m_nSelected		= 0;
	m_Selected		= NULL;
	m_Selected_Dst	= NULL;
	m_Selected_Buf	= 0;

	m_Pos			= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shapes_Search::~CSG_Shapes_Search(void)
{
	Destroy();
}

//---------------------------------------------------------
void CSG_Shapes_Search::Destroy(void)
{
	if( m_nPoints > 0 )
	{
		SG_Free(m_Pos);
	}

	m_Pos			= NULL;
	m_Idx			.Destroy();

	//-----------------------------------------------------
	if( m_bDestroy && m_pPoints )
	{
		delete(m_pPoints);
	}

	m_pPoints		= NULL;
	m_nPoints		= 0;
	m_bDestroy		= false;

	//-----------------------------------------------------
	if( m_Selected )
	{
		SG_Free(m_Selected);
		SG_Free(m_Selected_Dst);
	}

	m_Selected		= NULL;
	m_Selected_Dst	= NULL;
	m_nSelected		= 0;
	m_Selected_Buf	= 0;

	m_Selected_Idx	.Destroy();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Shapes_Search::Create(CSG_Shapes *pShapes)
{
	int		iShape, iPart, iPoint;
	CSG_Shape	*pShape, *pPoint;
	double	*Value;

	Destroy();

	//-----------------------------------------------------
	if( pShapes && pShapes->is_Valid() )
	{
		if( pShapes->Get_Type() == SHAPE_TYPE_Point )
		{
			m_bDestroy	= false;
			m_pPoints	= pShapes;
		}
		else
		{
			m_bDestroy	= true;
			m_pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, NULL, &pShapes->Get_Table());

			for(iShape=0; iShape<pShapes->Get_Count() && SG_UI_Process_Set_Progress(iShape, pShapes->Get_Count()); iShape++)
			{
				pShape	= pShapes->Get_Shape(iShape);

				for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
				{
					for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
					{
						pPoint	= m_pPoints->Add_Shape(pShape->Get_Record());
						pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));
					}
				}
			}
		}

		//-------------------------------------------------
		if( m_pPoints->Get_Count() > 1 )
		{
			m_nPoints	= m_pPoints->Get_Count();

			Value		= (double    *)SG_Malloc(m_nPoints * sizeof(double));
			m_Pos		= (TSG_Point *)SG_Malloc(m_nPoints * sizeof(TSG_Point));

			for(iPoint=0; iPoint<m_nPoints; iPoint++)
			{
				Value[iPoint]	= m_pPoints->Get_Shape(iPoint)->Get_Point(0).x;
			}

			m_Idx.Create(m_nPoints, Value, true);

			for(iPoint=0; iPoint<m_nPoints; iPoint++)
			{
				m_Pos[iPoint]	= m_pPoints->Get_Shape(m_Idx[iPoint])->Get_Point(0);
			}

			SG_Free(Value);

			return( true );
		}
	}

	//-----------------------------------------------------
	Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Shapes_Search::_Get_Index_Next(double Position)
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
CSG_Shape * CSG_Shapes_Search::Get_Point_Nearest(double x, double y)
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
CSG_Shape * CSG_Shapes_Search::Get_Point_Nearest(double x, double y, int Quadrant)
{
	int		iPoint;

	iPoint	= _Get_Point_Nearest(x, y, Quadrant);

	return( iPoint >= 0 && iPoint < m_nPoints ? m_pPoints->Get_Shape(iPoint) : NULL );
}

//---------------------------------------------------------
int CSG_Shapes_Search::_Get_Point_Nearest(double x, double y, int Quadrant)
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
void CSG_Shapes_Search::_Select_Add(CSG_Shape *pPoint, double Distance)
{
	if( m_nSelected >= m_Selected_Buf )
	{
		m_Selected_Buf	+= 8;

		m_Selected		= (CSG_Shape **)SG_Realloc(m_Selected    , m_Selected_Buf * sizeof(CSG_Shape *));
		m_Selected_Dst	= (double     *)SG_Realloc(m_Selected_Dst, m_Selected_Buf * sizeof(double     ));
	}

	m_Selected    [m_nSelected]	= pPoint;
	m_Selected_Dst[m_nSelected]	= Distance;
	m_nSelected++;
}

//---------------------------------------------------------
int CSG_Shapes_Search::Select_Radius(double x, double y, double Radius, bool bSort, int MaxPoints, int iQuadrant)
{
	bool		bInclude;
	int			ix, xLeft, xRight;
	double		d, dx, Radius_2;

	m_nSelected	= 0;

	Radius_2	= Radius * Radius;

	switch( iQuadrant )
	{
	default:
		xLeft		= _Get_Index_Next(x - Radius);
		xRight		= _Get_Index_Next(x + Radius);
		break;

	case 0:	// upper right
		xLeft		= _Get_Index_Next(x);
		xRight		= _Get_Index_Next(x + Radius);
		break;

	case 1:	// lower right
		xLeft		= _Get_Index_Next(x);
		xRight		= _Get_Index_Next(x + Radius);
		break;

	case 2:	// upper left
		xLeft		= _Get_Index_Next(x - Radius);
		xRight		= _Get_Index_Next(x);
		break;

	case 3:	// lower left
		xLeft		= _Get_Index_Next(x - Radius);
		xRight		= _Get_Index_Next(x);
		break;
	}

	for(ix=xLeft; ix<=xRight; ix++)
	{
		d		= m_Pos[ix].y - y;

		switch( iQuadrant )
		{
		default:		bInclude	= d >= -Radius && d <= Radius;	break;	// all
		case 0:	case 2:	bInclude	= d >= 0.0     && d <= Radius;	break;	// upper
		case 1:	case 3:	bInclude	= d >= -Radius && d <  0.0;		break;	// lower
		}

		if( bInclude )
		{
			dx	= m_Pos[ix].x - x;
			d	= dx*dx + d*d;

			if( d <= Radius_2 )
			{
				_Select_Add(m_pPoints->Get_Shape(m_Idx[ix]), d);
			}
		}
	}

	if( bSort || (MaxPoints > 0 && MaxPoints < m_nSelected) )
	{
		m_Selected_Idx.Create(m_nSelected, m_Selected_Dst, true);
	}

	return( MaxPoints <= 0 || MaxPoints > m_nSelected ? m_nSelected : MaxPoints );
}

//---------------------------------------------------------
int CSG_Shapes_Search::Select_Quadrants(double x, double y, double Radius, int MaxPoints, int MinPoints)
{
	if( MaxPoints <= 0 )
	{
		return( Select_Radius(x, y, Radius, true, MaxPoints) );
	}

	int			iQuadrant, i, n, nTotal;

	CSG_Shape	**Selected		= (CSG_Shape **)SG_Malloc(4 * MaxPoints * sizeof(CSG_Shape *));


	for(iQuadrant=0, nTotal=0; iQuadrant<4; iQuadrant++)
	{
		n	= Select_Radius(x, y, Radius, false, MaxPoints, iQuadrant);

		if( n < MinPoints )
		{
			return( 0 );
		}

		for(i=0; i<n; i++)
		{
			Selected[nTotal + i]	= Get_Selected_Point(i);
		}

		nTotal	+= n;
	}


	for(i=0, m_nSelected=0; i<nTotal; i++)
	{
		_Select_Add(Selected[i], -1.0);
	}

	SG_Free(Selected);

	return( m_nSelected );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
