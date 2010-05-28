
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

	pNode_0	= Parameters.Add_Choice(
		NULL	, "LINE_TYPE"	, _TL("Method for Lines"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("thin"),
			_TL("thick")
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
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("grid")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes2Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
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
	int		iField, iShape, iType;

	//-----------------------------------------------------
	m_pShapes		= Parameters("INPUT")		->asShapes();
	m_Method_Lines	= Parameters("LINE_TYPE")	->asInt();
	iField			= Parameters("FIELD")		->asInt();
	iType			= Parameters("GRID_TYPE")	->asInt();

	if( iField < 0 || iField >= m_pShapes->Get_Field_Count() || m_pShapes->Get_Field_Type(iField) == SG_DATATYPE_String )
	{
		iField		= -1;

		Message_Add(_TL("WARNING: selected attribute is not numeric; generating unique identifiers instead."));
	}

	//-----------------------------------------------------
	m_pGrid		= NULL;

	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( m_Grid_Target.Init_User(m_pShapes->Get_Extent()) && Dlg_Parameters("USER") )
		{
			m_pGrid	= m_Grid_Target.Get_User(Get_Grid_Type(iType));
		}
		break;

	case 1:	// grid...
		if( Dlg_Parameters("GRID") )
		{
			m_pGrid	= m_Grid_Target.Get_Grid(Get_Grid_Type(iType));
		}
		break;
	}

	if( m_pGrid == NULL )
	{
		return( false );
	}

	//-------------------------------------------------
	m_pGrid->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pShapes->Get_Name(), iField < 0 ? _TL("ID") : m_pShapes->Get_Field_Name(iField)));
	m_pGrid->Assign_NoData();

	m_pLock	= m_pShapes->Get_Type() == SHAPE_TYPE_Point ? NULL : SG_Create_Grid(m_pGrid, SG_DATATYPE_Byte);

	//-----------------------------------------------------
	for(iShape=0, m_Lock_ID=1; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++, m_Lock_ID++)
	{
		CSG_Shape	*pShape	= m_pShapes->Get_Shape(iShape);

		if( m_pShapes->Get_Selection_Count() <= 0 || pShape->is_Selected() )
		{
			if( iField < 0 || !pShape->is_NoData(iField) )
			{
				m_Value	= iField < 0 ? iShape + 1 : pShape->asDouble(iField);

				if( pShape->Intersects(m_pGrid->Get_Extent().m_rect) )
				{
					switch( m_pShapes->Get_Type() )
					{
					case SHAPE_TYPE_Point:
					case SHAPE_TYPE_Points:		Set_Points	(pShape);	break;
					case SHAPE_TYPE_Line:		Set_Line	(pShape);	break;
					case SHAPE_TYPE_Polygon:	Set_Polygon	(pShape);	break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_pLock )
	{
		delete(m_pLock);
	}

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
		if( m_pLock )
		{
			if( m_Lock_ID >= 255 )
			{
				m_Lock_ID	= 1;

				m_pLock->Assign(0.0);
			}

			if( m_pLock->asInt(x, y) != m_Lock_ID )
			{
				m_pLock	->Set_Value(x, y, m_Lock_ID);

				m_pGrid	->Set_Value(x, y, m_pGrid->is_NoData(x, y) ? m_Value : (m_Value + m_pGrid->asDouble(x, y)) / 2.0);
			}
		}
		else
		{
			m_pGrid	->Set_Value(x, y, m_pGrid->is_NoData(x, y) ? m_Value : (m_Value + m_pGrid->asDouble(x, y)) / 2.0);
		}
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
	bool		bFill, *bCrossing;
	int			x, y, xStart, xStop;
	TSG_Point	A, B, a, b, c;
	CSG_Rect	Extent;

	//-----------------------------------------------------
	bCrossing	= (bool *)SG_Malloc(m_pGrid->Get_NX() * sizeof(bool));

	Extent		= pShape->Get_Extent();

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

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				b	= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);

				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					a	= b;
					b	= pShape->Get_Point(iPoint, iPart);

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
							x	= m_pGrid->Get_NX() - 1;
						}

						bCrossing[x]	= !bCrossing[x];
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
