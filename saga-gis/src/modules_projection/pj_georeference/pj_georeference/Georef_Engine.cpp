/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    pj_Georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Georef_Engine.cpp                   //
//                                                       //
//                 Copyright (C) 2013 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@gwdg.de                        //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Georef_Engine.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Engine::CGeoref_Engine(void)
{
	m_Method	= GEOREF_NotSet;
	m_Order		= 0;
	m_Scaling	= 0.0;

	m_TIN_Fwd.Add_Field("X", SG_DATATYPE_Double);
	m_TIN_Fwd.Add_Field("Y", SG_DATATYPE_Double);

	m_TIN_Inv.Add_Field("X", SG_DATATYPE_Double);
	m_TIN_Inv.Add_Field("Y", SG_DATATYPE_Double);
}

//---------------------------------------------------------
bool CGeoref_Engine::Destroy(void)
{
	m_Method	= GEOREF_NotSet;

	m_Error.Clear();

	m_From.Clear();
	m_To  .Clear();

	m_TIN_Fwd.Del_Records();
	m_TIN_Inv.Del_Records();

	m_Spline_Fwd[0].Destroy();
	m_Spline_Fwd[1].Destroy();
	m_Spline_Inv[0].Destroy();
	m_Spline_Inv[1].Destroy();

	m_Polynom_Fwd[0].Destroy();
	m_Polynom_Fwd[1].Destroy();
	m_Polynom_Inv[0].Destroy();
	m_Polynom_Inv[1].Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::Add_Reference(TSG_Point From, TSG_Point To)
{
	if( m_From.Add(From) && m_To.Add(To) )
	{
		m_Method	= GEOREF_NotSet;

		if( Get_Reference_Count() == 1 )
		{
			m_rFrom.Assign(From, From);
			m_rTo  .Assign(To  , To  );
		}
		else
		{
			m_rFrom.Union(From);
			m_rTo  .Union(To  );
		}

		return( true );
	}

	if( m_From.Get_Count() > m_To.Get_Count() )
	{
		m_From.Del(m_From.Get_Count() - 1);
	}

	return( false );
}

//---------------------------------------------------------
bool CGeoref_Engine::Add_Reference(double xFrom, double yFrom, double xTo, double yTo)
{
	return( Add_Reference(CSG_Point(xFrom, yFrom), CSG_Point(xTo, yTo)) );
}

//---------------------------------------------------------
bool CGeoref_Engine::Set_Reference(CSG_Shapes *pFrom, CSG_Shapes *pTo)
{
	if( !pFrom || pFrom->Get_Count() <= 0
	||  !pTo   || pTo  ->Get_Count() <= 0 )
	{
		return( false );
	}

	Destroy();

	for(int iShape=0; iShape<pFrom->Get_Count() && iShape<pTo->Get_Count(); iShape++)
	{
		CSG_Shape	*_pFrom	= pFrom->Get_Shape(iShape);
		CSG_Shape	*_pTo	= pTo  ->Get_Shape(iShape);

		for(int iPart=0; iPart<_pFrom->Get_Part_Count() && iPart<_pTo->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<_pFrom->Get_Point_Count(iPart) && iPoint<_pTo->Get_Point_Count(iPart); iPoint++)
			{
				Add_Reference(_pFrom->Get_Point(iPoint, iPart), _pTo->Get_Point(iPoint, iPart));
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGeoref_Engine::Set_Reference(CSG_Shapes *pFrom, int xTo_Field, int yTo_Field)
{
	if( !pFrom || pFrom->Get_Count() <= 0 || pFrom->Get_Type() != SHAPE_TYPE_Point
	||  xTo_Field < 0 || xTo_Field >= pFrom->Get_Field_Count()
	||  yTo_Field < 0 || yTo_Field >= pFrom->Get_Field_Count()	)
	{
		return( false );
	}

	Destroy();

	for(int iPoint=0; iPoint<pFrom->Get_Count(); iPoint++)
	{
		CSG_Shape	*pPoint	= pFrom->Get_Shape(iPoint);

		Add_Reference(
			pPoint->Get_Point(0).x,
			pPoint->Get_Point(0).y,
			pPoint->asDouble(xTo_Field),
			pPoint->asDouble(yTo_Field)
		);
	}

	return( true );
}

//---------------------------------------------------------
bool CGeoref_Engine::Get_Reference_Extent(CSG_Rect &Extent, bool bInverse)
{
	CSG_Points	&Points	= bInverse ? m_From : m_To;

	if( Points.Get_Count() >= 3 )
	{
		Extent.Assign(Points[0], Points[1]);

		for(int i=2; i<Points.Get_Count(); i++)
		{
			Extent.Union(Points[i]);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CGeoref_Engine::Get_Reference_Residual(int i)
{
	if( is_Okay() && i >= 0 && i < Get_Reference_Count() )
	{
		TSG_Point	p	= m_From[i];

		if( Get_Converted(p) )
		{
			return( SG_Get_Distance(p, m_To[i]) );
		}
	}

	return( -1.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::Set_Scaling(double Scaling)
{
	m_Scaling	= Scaling > 0.0 ? Scaling : 0.0;

	return( m_Scaling > 0.0 );
}

//---------------------------------------------------------
bool CGeoref_Engine::Get_Converted(TSG_Point &Point, bool bInverse)
{
	return( Get_Converted(Point.x, Point.y, bInverse) );
}

//---------------------------------------------------------
bool CGeoref_Engine::Get_Converted(double &x, double &y, bool bInverse)
{
	bool	bResult;

	if( m_Scaling > 0.0 )
	{
		x	= bInverse
			? (x - m_rTo  .Get_XMin()) * m_Scaling / m_rTo  .Get_XRange()
			: (x - m_rFrom.Get_XMin()) * m_Scaling / m_rFrom.Get_XRange();

		y	= bInverse
			? (y - m_rTo  .Get_YMin()) * m_Scaling / m_rTo  .Get_YRange()
			: (y - m_rFrom.Get_YMin()) * m_Scaling / m_rFrom.Get_YRange();
	}

	switch( m_Method )
	{
	case GEOREF_Triangulation:
		bResult	= _Get_Triangulation(x, y, bInverse ? &m_TIN_Inv    : &m_TIN_Fwd   );
		break;

	case GEOREF_Spline:
		bResult	= _Get_Spline       (x, y, bInverse ? m_Spline_Inv  : m_Spline_Fwd );
		break;

	case GEOREF_Affine:
	case GEOREF_Polynomial_1st_Order:
	case GEOREF_Polynomial_2nd_Order:
	case GEOREF_Polynomial_3rd_Order:
	case GEOREF_Polynomial:
		bResult	= _Get_Polynomial   (x, y, bInverse ? m_Polynom_Inv : m_Polynom_Fwd);
		break;

	default:
		bResult	= false;
		break;
	}

	if( bResult && m_Scaling > 0.0 )
	{
		x	= bInverse
			? m_rFrom.Get_XMin() + x * m_rFrom.Get_XRange() / m_Scaling
			: m_rTo  .Get_XMin() + x * m_rTo  .Get_XRange() / m_Scaling;

		y	= bInverse
			? m_rFrom.Get_YMin() + y * m_rFrom.Get_YRange() / m_Scaling
			: m_rTo  .Get_YMin() + y * m_rTo  .Get_YRange() / m_Scaling;
	}

	return( bResult );
}

//---------------------------------------------------------
bool CGeoref_Engine::Evaluate(int Method, int Order)
{
	//-----------------------------------------------------
	if( Method == GEOREF_NotSet )	// Automatic
	{
		if( Get_Reference_Count() >= 10 )	return( Evaluate(GEOREF_Spline) );
		if( Get_Reference_Count() >=  4 )	return( Evaluate(GEOREF_Polynomial_1st_Order) );
		
		return( Evaluate(GEOREF_Affine) );
	}

	//-----------------------------------------------------
	int	n	= _Get_Reference_Minimum(Method, Order);

	if( n < 0 || Get_Reference_Count() < n )
	{
		m_Error.Printf(SG_T("%s\n%s: %d"), _TL("not enough reference points"), _TL("minimum requirement"), n);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Points	From, To;

	if( m_Scaling > 0.0 )
	{
		From	= m_From;
		To		= m_To;

		for(int i=0; i<Get_Reference_Count(); i++)
		{
			m_From[i].x	= (m_From[i].x - m_rFrom.Get_XMin()) * m_Scaling / m_rFrom.Get_XRange();
			m_From[i].y	= (m_From[i].y - m_rFrom.Get_YMin()) * m_Scaling / m_rFrom.Get_YRange();

			m_To  [i].x	= (m_To  [i].x - m_rTo  .Get_XMin()) * m_Scaling / m_rTo  .Get_XRange();
			m_To  [i].y	= (m_To  [i].y - m_rTo  .Get_YMin()) * m_Scaling / m_rTo  .Get_YRange();
		}
	}

	//-----------------------------------------------------
	bool	bResult;

	m_Error.Clear();

	switch( m_Method = Method )
	{
	case GEOREF_Triangulation:
		bResult	= _Set_Triangulation();
		break;

	case GEOREF_Spline:
		bResult	= _Set_Spline();
		break;

	case GEOREF_Affine:
	case GEOREF_Polynomial_1st_Order:
	case GEOREF_Polynomial_2nd_Order:
	case GEOREF_Polynomial_3rd_Order:
	case GEOREF_Polynomial:
		m_Order	= Order;
		bResult	=  _Set_Polynomial(m_From, m_To, m_Polynom_Fwd)
				&& _Set_Polynomial(m_To, m_From, m_Polynom_Inv);
		break;

	default:	// should not happen ...
		bResult	= false;
		break;
	}

	//-----------------------------------------------------
	if( m_Scaling > 0.0 )
	{
		m_From	= From;
		m_To	= To;
	}

	if( !bResult )
	{
		m_Method	= GEOREF_NotSet;
	}

	return( bResult );
}

//---------------------------------------------------------
int CGeoref_Engine::_Get_Reference_Minimum(int Method, int Order)
{
	switch( Method )
	{
	default:							return(  0 );
	case GEOREF_Triangulation:			return(  3 );
	case GEOREF_Spline:					return(  3 );
	case GEOREF_Affine:					return(  3 );	// 3: a + bx + cy	aka RST=rotation/scaling/translation
	case GEOREF_Polynomial_1st_Order:	return(  4 );	// 4: a + bx + cy + dxy
	case GEOREF_Polynomial_2nd_Order:	return(  6 );	// 6: a + bx + cy + dxy + ex^2 + fy^2
	case GEOREF_Polynomial_3rd_Order:	return(  9 );	// 9: a + bx + cy + dxy + ex^2 + fy^2 + gx^3 + hy^3 + ix^2y^2
	case GEOREF_Polynomial:				return( Order > 0 ? (int)SG_Get_Square(Order + 1) : -1 );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::_Set_Triangulation(void)
{
	m_TIN_Fwd.Del_Records();
	m_TIN_Inv.Del_Records();

	for(int i=0; i<Get_Reference_Count(); i++)
	{
		CSG_TIN_Node	*pNode;

		pNode	= m_TIN_Fwd.Add_Node(m_From[i], NULL, false);
		pNode->Set_Value(0, m_To  [i].x);
		pNode->Set_Value(1, m_To  [i].y);

		pNode	= m_TIN_Inv.Add_Node(m_To  [i], NULL, false);
		pNode->Set_Value(0, m_From[i].x);
		pNode->Set_Value(1, m_From[i].y);
	}

	return( m_TIN_Fwd.Update() && m_TIN_Inv.Update() );
}

//---------------------------------------------------------
bool CGeoref_Engine::_Get_Triangulation(double &x, double &y, CSG_TIN *pTIN)
{
	CSG_Point	p(x, y);

	for(int i=0; i<pTIN->Get_Triangle_Count(); i++)
	{
		CSG_TIN_Triangle	*pTriangle	= pTIN->Get_Triangle(i);

		if( pTriangle->is_Containing(p) )
		{
			return( pTriangle->Get_Value(0, p, x)
				&&  pTriangle->Get_Value(1, p, y) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::_Set_Spline(void)
{
	m_Spline_Fwd[0].Destroy();
	m_Spline_Fwd[1].Destroy();

	m_Spline_Inv[0].Destroy();
	m_Spline_Inv[1].Destroy();

	for(int i=0; i<Get_Reference_Count(); i++)
	{
		m_Spline_Fwd[0].Add_Point(m_From[i], m_To[i].x);
		m_Spline_Fwd[1].Add_Point(m_From[i], m_To[i].y);

		m_Spline_Inv[0].Add_Point(m_To[i], m_From[i].x);
		m_Spline_Inv[1].Add_Point(m_To[i], m_From[i].y);
	}

	return( m_Spline_Fwd[0].Create() && m_Spline_Fwd[1].Create()
		&&  m_Spline_Inv[0].Create() && m_Spline_Inv[1].Create()
	);
}

//---------------------------------------------------------
bool CGeoref_Engine::_Get_Spline(double &x, double &y, CSG_Thin_Plate_Spline Spline[2])
{
	if( Spline[0].is_Okay() && Spline[1].is_Okay() )
	{
		double	_x	= x;
		
		x	= Spline[0].Get_Value(_x, y);
		y	= Spline[1].Get_Value(_x, y);

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
void CGeoref_Engine::_Get_Polynomial(double x, double y, double *z)
{
	z[0]	= 1.0;

	switch( m_Method )
	{
	case GEOREF_Polynomial_3rd_Order:	// 9: a + bx + cy + dxy + ex^2 + fy^2 + gx^2y^2 + hx^3 + iy^3
		z[8]	= y*y*y;
		z[7]	= x*x*x;
		z[6]	= x*x*y*y;
	case GEOREF_Polynomial_2nd_Order:	// 6: a + bx + cy + dxy + ex^2 + fy^2
		z[5]	= y*y;
		z[4]	= x*x;
	case GEOREF_Polynomial_1st_Order:	// 4: a + bx + cy + dxy
		z[3]	= x*y;
	case GEOREF_Affine:					// 3: a + bx + cy	aka RST=rotation/scaling/translation
		z[2]	= y;
		z[1]	= x;
		break;

	case GEOREF_Polynomial:
		{
			int	j, k, n;

			CSG_Vector	xPow(m_Order + 1);	xPow[0]	= 1.0;
			CSG_Vector	yPow(m_Order + 1);	yPow[0]	= 1.0;

			for(j=1, n=1; j<=m_Order; j++)
			{
				z[n++]	= (xPow[j]	= xPow[j - 1] * x);
				z[n++]	= (yPow[j]	= yPow[j - 1] * y);
			}

			for(j=1; j<=m_Order; j++)
			{
				for(k=1; k<=m_Order; k++)
				{
					z[n++]	= yPow[j] * xPow[k];
				}
			}
		}
		break;
	}
}

//---------------------------------------------------------
bool CGeoref_Engine::_Set_Polynomial(CSG_Points &From, CSG_Points &To, CSG_Vector b[2])
{
	CSG_Vector	X(Get_Reference_Count());
	CSG_Vector	Y(Get_Reference_Count());

	CSG_Matrix	M(_Get_Reference_Minimum(m_Method, m_Order), Get_Reference_Count());

	//-----------------------------------------------------
	for(int i=0; i<Get_Reference_Count(); i++)
	{
		_Get_Polynomial(From[i].x, From[i].y, M[i]);

		X[i]	= To[i].x;
		Y[i]	= To[i].y;
	}

	//-----------------------------------------------------
	CSG_Matrix	Mt	= M.Get_Transpose();
	CSG_Matrix	Mi	= (Mt * M).Get_Inverse() * Mt;

	b[0]	= Mi * X;
	b[1]	= Mi * Y;

	return( true );
}

//---------------------------------------------------------
bool CGeoref_Engine::_Get_Polynomial(double &x, double &y, CSG_Vector b[2])
{
	CSG_Vector	z(_Get_Reference_Minimum(m_Method, m_Order));

	_Get_Polynomial(x, y, z.Get_Data());

	x	= b[0] * z;
	y	= b[1] * z;

	return( true );
}

/**//*/---------------------------------------------------------
bool CGeoref_Engine::_Set_Polynomial(CSG_Points &From, CSG_Points &To, CSG_Vector b[2])
{
	//-----------------------------------------------------
	CSG_Vector	X(Get_Reference_Count());
	CSG_Vector	Y(Get_Reference_Count());

	CSG_Matrix	M(_Get_Reference_Minimum(m_Method, m_Order), Get_Reference_Count());

	//-----------------------------------------------------
	for(int i=0; i<Get_Reference_Count(); i++)
	{
		double	x	= From[i].x;
		double	y	= From[i].y;

		X[i]	= To[i].x;
		Y[i]	= To[i].y;

		M[i][0]	= 1.0;

		switch( m_Method )
		{
		case GEOREF_Polynomial_3rd_Order:	// 9: a + bx + cy + dxy + ex^2 + fy^2 + gx^2y^2 + hx^3 + iy^3
			M[i][8]	= y*y*y;
			M[i][7]	= x*x*x;
			M[i][6]	= x*x*y*y;
		case GEOREF_Polynomial_2nd_Order:	// 6: a + bx + cy + dxy + ex^2 + fy^2
			M[i][5]	= y*y;
			M[i][4]	= x*x;
		case GEOREF_Polynomial_1st_Order:	// 4: a + bx + cy + dxy
			M[i][3]	= x*y;
		case GEOREF_Affine:					// 3: a + bx + cy	aka RST=rotation/scaling/translation
			M[i][2]	= y;
			M[i][1]	= x;
			break;

		case GEOREF_Polynomial:
			{
				int	j, k, n;

				CSG_Vector	xPow(m_Order + 1);	xPow[0]	= 1.0;
				CSG_Vector	yPow(m_Order + 1);	yPow[0]	= 1.0;

				for(j=1, n=1; j<=m_Order; j++)
				{
					M[i][n++]	= (xPow[j]	= xPow[j - 1] * x);
					M[i][n++]	= (yPow[j]	= yPow[j - 1] * y);
				}

				for(j=1; j<=m_Order; j++)
				{
					for(k=1; k<=m_Order; k++)
					{
						M[i][n++]	= yPow[j] * xPow[k];
					}
				}
			}
			break;
		}
	}

	//-----------------------------------------------------
	CSG_Matrix	Mt	= M.Get_Transpose();
	CSG_Matrix	Mi	= (Mt * M).Get_Inverse() * Mt;

	b[0]	= Mi * X;
	b[1]	= Mi * Y;

	return( true );
}

//---------------------------------------------------------
bool CGeoref_Engine::_Get_Polynomial(double &x, double &y, CSG_Vector b[2])
{
	TSG_Point	p;

	p.x	= b[0][0];
	p.y	= b[1][0];

	//-----------------------------------------------------
	switch( m_Method )
	{
	case GEOREF_Polynomial_3rd_Order:	// 9: a + bx + cy + dxy + ex^2 + fy^2 + gx^2y^2 + hx^3 + iy^3
		p.x	+= b[0][6]*x*x*y*y + b[0][7]*x*x*x + b[0][8]*y*y*y;
		p.y	+= b[1][6]*x*x*y*y + b[1][7]*x*x*x + b[1][8]*y*y*y;
	case GEOREF_Polynomial_2nd_Order:	// 6: a + bx + cy + dxy + ex^2 + fy^2
		p.x	+= b[0][4]*x*x + b[0][5]*y*y;
		p.y	+= b[1][4]*x*x + b[1][5]*y*y;
	case GEOREF_Polynomial_1st_Order:	// 4: a + bx + cy + dxy
		p.x	+= b[0][3]*x*y;
		p.y	+= b[1][3]*x*y;
	case GEOREF_Affine:					// 3: a + bx + cy	aka RST=rotation/scaling/translation
		p.x	+= b[0][1]*x + b[0][2]*y;
		p.y	+= b[1][1]*x + b[1][2]*y;
		break;

	case GEOREF_Polynomial:
		{
			int	j, k, n;

			CSG_Vector	xPow(m_Order + 1);	xPow[0]	= 1.0;
			CSG_Vector	yPow(m_Order + 1);	yPow[0]	= 1.0;

			for(j=1, n=1; j<=m_Order; j++)
			{
				p.x	+= b[0][n  ] * (xPow[j] = xPow[j - 1] * x);
				p.y	+= b[1][n++] *  xPow[j];

				p.x	+= b[0][n  ] * (yPow[j] = yPow[j - 1] * y);
				p.y	+= b[1][n++] *  yPow[j];
			}

			for(j=1; j<=m_Order; j++)
			{
				for(k=1; k<=m_Order; k++)
				{
					p.x	+= b[0][n  ] * yPow[j] * xPow[k];
					p.y	+= b[1][n++] * yPow[j] * xPow[k];
				}
			}
		}
		break;
	}

	//-----------------------------------------------------
	x	= p.x;
	y	= p.y;

	return( true );
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
