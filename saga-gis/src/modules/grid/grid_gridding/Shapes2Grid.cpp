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
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Shapes2Grid.cpp                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Shapes2Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define X_WORLD_TO_GRID(X)	(((X) - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize())
#define Y_WORLD_TO_GRID(Y)	(((Y) - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize())


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes2Grid::CShapes2Grid(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	Set_Name		(_TL("Shapes to Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Gridding of a shapes layer. If some shapes are selected, only these will be gridded."
	));


	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Shapes(
		NULL	, "INPUT"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode_1	= Parameters.Add_Table_Field(
		pNode_0	, "FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Output Values"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("data / no-data"),
			_TL("index number"),
			_TL("attribute")
		), 2
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "MULTIPLE"	, _TL("Method for Multiple Values"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("first"),
			_TL("last"),
			_TL("minimum"),
			_TL("maximum"),
			_TL("mean")
		), 1
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "LINE_TYPE"	, _TL("Lines"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("thin"),
			_TL("thick")
		), 1
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "POLY_TYPE"	, _TL("Polygon"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("node"),
			_TL("cell")
		), 1
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "GRID_TYPE"	, _TL("Preferred Target Grid Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Integer (1 byte)"),
			_TL("Integer (2 byte)"),
			_TL("Integer (4 byte)"),
			_TL("Floating Point (4 byte)"),
			_TL("Floating Point (8 byte)")
		), 3
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("GRID" , _TL("Grid")            , false);
	m_Grid_Target.Add_Grid("COUNT", _TL("Number of Values"), true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes2Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "INPUT") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Module::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CShapes2Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "INPUT") )
	{
		pParameters->Set_Enabled("LINE_TYPE", pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Line);
		pParameters->Set_Enabled("POLY_TYPE", pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Polygon);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "OUTPUT") )
	{
		pParameters->Set_Enabled("FIELD"    , pParameter->asInt() == 2);
		pParameters->Set_Enabled("MULTIPLE" , pParameter->asInt() == 2);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Data_Type CShapes2Grid::Get_Grid_Type(int iType)
{
	switch( iType )
	{
	case 0:	return( SG_DATATYPE_Byte   );
	case 1:	return( SG_DATATYPE_Short  );
	case 2:	return( SG_DATATYPE_Int    );
	case 3:	return( SG_DATATYPE_Float  );
	case 4:	return( SG_DATATYPE_Double );
	}

	return( SG_DATATYPE_Float );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes2Grid::On_Execute(void)
{
	//-----------------------------------------------------
	m_pShapes			= Parameters("INPUT"    )->asShapes();

	m_Method_Lines		= Parameters("LINE_TYPE")->asInt();
	m_Method_Polygon	= Parameters("POLY_TYPE")->asInt();
	m_Method_Multi		= Parameters("MULTIPLE" )->asInt();

	if( m_pShapes->Get_Type() == SHAPE_TYPE_Polygon && m_Method_Polygon == 1 )	// all cells intersected have to be marked
	{
		m_Method_Lines	= 1;	// thick, each cell crossed by polygon boundary will be marked additionally
	}

	//-----------------------------------------------------
	int		iField;

	switch( Parameters("OUTPUT")->asInt() )
	{
	case 0:	iField	= -1;	break;
	case 1:	iField	= -2;	break;
	case 2:
		if( (iField = Parameters("FIELD")->asInt()) < 0 || !SG_Data_Type_is_Numeric(m_pShapes->Get_Field_Type(iField)) )
		{
			iField		= -2;

			Message_Add(_TL("WARNING: selected attribute is not numeric; generating unique identifiers instead."));
		}
		break;
	}

	//-----------------------------------------------------
	if( (m_pGrid = m_Grid_Target.Get_Grid("GRID", Get_Grid_Type(Parameters("GRID_TYPE")->asInt()))) == NULL )
	{
		return( false );
	}

	if( iField < 0 )
	{
		m_pGrid->Set_NoData_Value(0.0);
	}

	m_pGrid->Set_Name(CSG_String::Format("%s [%s]", m_pShapes->Get_Name(), iField < 0 ? _TL("ID") : m_pShapes->Get_Field_Name(iField)));
	m_pGrid->Assign_NoData();

	//-------------------------------------------------
	m_pCount	= m_Grid_Target.Get_Grid("COUNT", m_pShapes->Get_Count() < 256 ? SG_DATATYPE_Byte : SG_DATATYPE_Word);

	if( m_pCount == NULL )
	{
		m_Count.Create(m_pGrid->Get_System(), SG_DATATYPE_Word);

		m_pCount	= &m_Count;
	}

	m_pCount->Set_Name(CSG_String::Format("%s [%s]", m_pShapes->Get_Name(), _TL("Count")));
	m_pCount->Set_NoData_Value(0.0);
	m_pCount->Assign(0.0);

	//-----------------------------------------------------
	for(int iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= m_pShapes->Get_Shape(iShape);

		if( m_pShapes->Get_Selection_Count() <= 0 || pShape->is_Selected() )
		{
			if( iField < 0 || !pShape->is_NoData(iField) )
			{
				m_Value	= iField >= 0 ? pShape->asDouble(iField) : iField == -2 ? iShape + 1 : 1;

				if( pShape->Intersects(m_pGrid->Get_Extent()) )
				{
					switch( m_pShapes->Get_Type() )
					{
					case SHAPE_TYPE_Point:	case SHAPE_TYPE_Points:
						Set_Points	(pShape);
						break;

					case SHAPE_TYPE_Line:
						Set_Line	(pShape);
						break;

					case SHAPE_TYPE_Polygon:
						Set_Polygon	(pShape);

						if( m_Method_Polygon == 1 )	// all cells intersected have to be marked
						{
							Set_Line(pShape);	// thick, each cell crossed by polygon boundary will be marked additionally
						}
						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Method_Multi == 4 )	// mean
	{
		for(int y=0; y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++)
		{
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				if( m_pCount->asInt(x, y) > 1 )
				{
					m_pGrid->Mul_Value(x, y, 1.0 / m_pCount->asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Count.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CShapes2Grid::Set_Value(int x, int y)
{
	if( m_pGrid->is_InGrid(x, y, false) )
	{
		if( m_pCount->asInt(x, y) == 0 )
		{
			m_pGrid->Set_Value(x, y, m_Value);
		}
		else switch( m_Method_Multi )
		{
		case 0:	// first
			break;

		case 1:	// last
			m_pGrid->Set_Value(x, y, m_Value);
			break;

		case 2:	// minimum
			if( m_pGrid->asDouble(x, y) > m_Value )
			{
				m_pGrid->Set_Value(x, y, m_Value);
			}
			break;

		case 3:	// maximum
			if( m_pGrid->asDouble(x, y) < m_Value )
			{
				m_pGrid->Set_Value(x, y, m_Value);
			}
			break;

		case 4:	// mean
			m_pGrid->Add_Value(x, y, m_Value);
			break;
		}

		m_pCount->Add_Value(x, y, 1);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes2Grid::Set_Points(CSG_Shape *pShape)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

			Set_Value(
				(int)(0.5 + X_WORLD_TO_GRID(p.x)),
				(int)(0.5 + Y_WORLD_TO_GRID(p.y))
			);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes2Grid::Set_Line(CSG_Shape *pShape)
{
	TSG_Point	a, b;

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		b	= pShape->Get_Point(0, iPart);
		b.x	= X_WORLD_TO_GRID(b.x);
		b.y	= Y_WORLD_TO_GRID(b.y);

		for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			a	= b;
			b	= pShape->Get_Point(iPoint, iPart);
			b.x	= X_WORLD_TO_GRID(b.x);
			b.y	= Y_WORLD_TO_GRID(b.y);

			switch( m_Method_Lines )
			{
			case 0:	Set_Line_A(a, b);	break;
			case 1:	Set_Line_B(a, b);	break;
			}
		}
	}
}

//---------------------------------------------------------
void CShapes2Grid::Set_Line_A(TSG_Point a, TSG_Point b)
{
	double			ix, iy, sig;
	double			dx, dy;
	TSG_Point_Int	A, B;

	A.x	= (int)(a.x	+= 0.5);
	A.y	= (int)(a.y	+= 0.5);
	B.x	= (int)(b.x	+= 0.5);
	B.y	= (int)(b.y	+= 0.5);

	//-----------------------------------------------------
	if( A.x != B.x || A.y != B.y )
	{
		dx	= b.x - a.x;
		dy	= b.y - a.y;

		if( fabs(dx) > fabs(dy) )
		{
			sig	= dx < 0 ? -1 : 1;
			dx	= fabs(dx);
			dy	/= dx;

			for(ix=0; ix<=dx; ix++, a.x+=sig, a.y+=dy)
			{
				Set_Value((int)a.x, (int)a.y);
			}
		}
		else if( fabs(dy) >= fabs(dx) && dy != 0 )
		{
			sig	= dy < 0 ? -1 : 1;
			dy	= fabs(dy);
			dx	/= dy;

			for(iy=0; iy<=dy; iy++, a.x+=dx, a.y+=sig)
			{
				Set_Value((int)a.x, (int)a.y);
			}
		}
	}
	else
	{
		Set_Value(A.x, A.y);
	}
}

/*/---------------------------------------------------------
void CShapes2Grid::Set_Line_A(TSG_Point a, TSG_Point b)
{
	TSG_Point_Int	A, B;

	A.x	= (int)(a.x	+ 0.5);
	A.y	= (int)(a.y	+ 0.5);
	B.x	= (int)(b.x	+ 0.5);
	B.y	= (int)(b.y	+ 0.5);

	//-----------------------------------------------------
	if( A.x != B.x || A.y != B.y )
	{
		int		d, dx, dy;
		double	m, t;

		dx	= B.x - A.x;
		dy	= B.y - A.y;

		if( fabs(dx) > fabs(dy) )
		{
			d	= dx < 0 ? -1 : 1;
			m	= d * (double)dy / (double)dx;

			for(t=A.y; A.x!=B.x; A.x+=d, t+=m)
			{
				Set_Value(A.x, (int)t);
			}
		}
		else // if( fabs(dy) >= fabs(dx) )
		{
			d	= dy < 0 ? -1 : 1;
			m	= d * (double)dx / (double)dy;

			for(t=A.x; A.y!=B.y; A.y+=d, t+=m)
			{
				Set_Value((int)t, A.y);
			}
		}
	}
	else
	{
		Set_Value(A.x, A.y);
	}
}/**/

//---------------------------------------------------------
void CShapes2Grid::Set_Line_B(TSG_Point a, TSG_Point b)
{
	int				ix, iy;
	double			e, d, dx, dy;
	TSG_Point_Int	A, B;

	A.x	= (int)(a.x	+= 0.5);
	A.y	= (int)(a.y	+= 0.5);
	B.x	= (int)(b.x	+= 0.5);
	B.y	= (int)(b.y	+= 0.5);

	Set_Value(A.x, A.y);

	//-----------------------------------------------------
	if( A.x != B.x || A.y != B.y )
	{
		dx	= b.x - a.x;
		dy	= b.y - a.y;

		a.x	= a.x > 0.0 ? a.x - (int)a.x : 1.0 + (a.x - (int)a.x);
		a.y	= a.y > 0.0 ? a.y - (int)a.y : 1.0 + (a.y - (int)a.y);

		//-------------------------------------------------
		if( fabs(dx) > fabs(dy) )
		{
			ix	= dx > 0.0 ? 1 : -1;
			iy	= dy > 0.0 ? 1 : -1;
			d	= fabs(dy / dx);
			dx	= ix < 0 ? a.x : 1.0 - a.x;
			e	= iy > 0 ? a.y : 1.0 - a.y;
			e	+= d * dx;

			while( e > 1.0 )
			{
				e	-= 1.0;
				A.y	+= iy;
				Set_Value(A.x, A.y);
			}

			while( A.x != B.x )
			{
				A.x	+= ix;
				e	+= d;
				Set_Value(A.x, A.y);

				if( A.x != B.x )
				{
					while( e > 1.0 )
					{
						e	-= 1.0;
						A.y	+= iy;
						Set_Value(A.x, A.y);
					}
				}
			}

			if( A.y != B.y )
			{
				iy	= A.y < B.y ? 1 : -1;

				while( A.y != B.y )
				{
					A.y	+= iy;
					Set_Value(A.x, A.y);
				}
			}
		}

		//-------------------------------------------------
		else // if( fabs(dy) > fabs(dx) )
		{
			ix	= dx > 0.0 ? 1 : -1;
			iy	= dy > 0.0 ? 1 : -1;
			d	= fabs(dx / dy);
			dy	= iy < 0 ? a.y : 1.0 - a.y;
			e	= ix > 0 ? a.x : 1.0 - a.x;
			e	+= d * dy;

			while( e > 1.0 )
			{
				e	-= 1.0;
				A.x	+= ix;
				Set_Value(A.x, A.y);
			}

			while( A.y != B.y )
			{
				A.y	+= iy;
				e	+= d;
				Set_Value(A.x, A.y);

				if( A.y != B.y )
				{
					while( e > 1.0 )
					{
						e	-= 1.0;
						A.x	+= ix;
						Set_Value(A.x, A.y);
					}
				}
			}

			if( A.x != B.x )
			{
				ix	= A.x < B.x ? 1 : -1;

				while( A.x != B.x )
				{
					A.x	+= ix;
					Set_Value(A.x, A.y);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes2Grid::Set_Polygon(CSG_Shape *pShape)
{
	Set_Polygon_Node((CSG_Shape_Polygon *)pShape);
}

//---------------------------------------------------------
void CShapes2Grid::Set_Polygon_Node(CSG_Shape_Polygon *pPolygon)
{
	bool		bFill, *bCrossing;
	int			x, y, xStart, xStop;
	TSG_Point	A, B, a, b, c;
	CSG_Rect	Extent;

	//-----------------------------------------------------
	bCrossing	= (bool *)SG_Malloc(m_pGrid->Get_NX() * sizeof(bool));

	Extent		= pPolygon->Get_Extent();

	xStart		= (int)((Extent.m_rect.xMin - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize()) - 1;
	if( xStart < 0 )
		xStart	= 0;

	xStop		= (int)((Extent.m_rect.xMax - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize()) + 1;
	if( xStop >= m_pGrid->Get_NX() )
		xStop	= m_pGrid->Get_NX() - 1;

	A.x			= m_pGrid->Get_XMin() - 1.0;
	B.x			= m_pGrid->Get_XMax() + 1.0;

	//-----------------------------------------------------
	for(y=0, A.y=m_pGrid->Get_YMin(); y<m_pGrid->Get_NY(); y++, A.y+=m_pGrid->Get_Cellsize())
	{
		if( A.y >= Extent.m_rect.yMin && A.y <= Extent.m_rect.yMax )
		{
			B.y	= A.y;

			memset(bCrossing, 0, m_pGrid->Get_NX() * sizeof(bool));

			for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
			{
				if( pPolygon->Get_Part(iPart)->Get_Extent().Intersects(m_pGrid->Get_Extent(true)) )
				{
					b	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

					for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
					{
						a	= b;
						b	= pPolygon->Get_Point(iPoint, iPart);

						if(	((a.y <= A.y && A.y  < b.y)
						||	 (a.y  > A.y && A.y >= b.y)) )
						{
							SG_Get_Crossing(c, a, b, A, B, false);

							x	= (int)(1.0 + X_WORLD_TO_GRID(c.x));

							if( x < 0 )
							{
								x	= 0;
							}
							else if( x >= m_pGrid->Get_NX() )
							{
								continue;
							}

							bCrossing[x]	= !bCrossing[x];
						}
					}
				}
			}

			//---------------------------------------------
			for(x=xStart, bFill=false; x<=xStop; x++)
			{
				if( bCrossing[x] )
				{
					bFill	= !bFill;
				}

				if( bFill )
				{
					Set_Value(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(bCrossing);
}

//---------------------------------------------------------
void CShapes2Grid::Set_Polygon_Cell(CSG_Shape_Polygon *pPolygon)
{
	//-----------------------------------------------------
	CSG_Grid_System	s(m_pGrid->Get_System());

	int	xA	= s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin());	if( xA <  0          )	xA	= 0;
	int	xB	= s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax());	if( xB >= s.Get_NX() )	xB	= s.Get_NX() - 1;
	int	yA	= s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMin());	if( yA <  0          )	yA	= 0;
	int	yB	= s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMax());	if( yB >= s.Get_NY() )	yB	= s.Get_NY() - 1;

	//-----------------------------------------------------
	TSG_Rect	r;

	r.yMax	= s.Get_yGrid_to_World(yA) - 0.5 * s.Get_Cellsize();

	for(int y=yA; y<=yB; y++)
	{
		r.yMin	= r.yMax;	r.yMax	+= s.Get_Cellsize();

		r.xMax	= s.Get_xGrid_to_World(xA) - 0.5 * s.Get_Cellsize();

		for(int x=xA; x<=xB; x++)
		{
			r.xMin	= r.xMax;	r.xMax	+= s.Get_Cellsize();

			if( pPolygon->Intersects(r) )
			{
				Set_Value(x, y);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
