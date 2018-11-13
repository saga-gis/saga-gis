/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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

//---------------------------------------------------------
#define OUTPUT_NODATA	-2
#define OUTPUT_INDEX	-1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes2Grid::CShapes2Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Shapes to Grid"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Gridding of a shapes layer. If some shapes are selected, only these will be gridded."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"INPUT"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("INPUT",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output Values"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("data / no-data"),
			_TL("index number"),
			_TL("attribute")
		), 2
	);

	Parameters.Add_Choice("",
		"MULTIPLE"	, _TL("Method for Multiple Values"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("first"),
			_TL("last"),
			_TL("minimum"),
			_TL("maximum"),
			_TL("mean")
		), 1
	);

	Parameters.Add_Choice("",
		"LINE_TYPE"	, _TL("Lines"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("thin"),
			_TL("thick")
		), 1
	);

	Parameters.Add_Choice("",
		"POLY_TYPE"	, _TL("Polygon"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("node"),
			_TL("cell")
		), 1
	);

	Parameters.Add_Choice("",
		"GRID_TYPE"	, _TL("Data Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("1 bit"),
			_TL("1 byte unsigned integer"),
			_TL("1 byte signed integer"),
			_TL("2 byte unsigned integer"),
			_TL("2 byte signed integer"),
			_TL("4 byte unsigned integer"),
			_TL("4 byte signed integer"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point"),
			_TL("same as attribute")
		), 9
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("GRID" , _TL("Grid")            , false);
	m_Grid_Target.Add_Grid("COUNT", _TL("Number of Values"),  true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes2Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("INPUT") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CShapes2Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("INPUT") )
	{
		pParameters->Set_Enabled("LINE_TYPE", pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Line);
		pParameters->Set_Enabled("POLY_TYPE", pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Polygon);
	}

	if(	pParameter->Cmp_Identifier("OUTPUT") )
	{
		pParameters->Set_Enabled("FIELD"    , pParameter->asInt() == 2);
		pParameters->Set_Enabled("MULTIPLE" , pParameter->asInt() != 0);
		pParameters->Set_Enabled("GRID_TYPE", pParameter->asInt() == 2);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Data_Type CShapes2Grid::Get_Data_Type(int Field)
{
	CSG_Shapes	*pShapes	= Parameters("INPUT")->asShapes();

	if( Field >= 0 && (Field >= pShapes->Get_Field_Count() || !SG_Data_Type_is_Numeric(pShapes->Get_Field_Type(Field))) )
	{
		Field	= OUTPUT_INDEX;	// index number
	}

	if( Field == OUTPUT_NODATA )	// data / no-data
	{
		return( SG_DATATYPE_Byte );
	}

	if( Field <= OUTPUT_INDEX )	// index number
	{
		return( pShapes->Get_Count() < 65535 ? SG_DATATYPE_Word : SG_DATATYPE_DWord );
	}

//	if( Field >= 0 )	// attribute
	{
		switch( Parameters("GRID_TYPE")->asInt() )
		{
		case  0: return( SG_DATATYPE_Bit    );
		case  1: return( SG_DATATYPE_Byte   );
		case  2: return( SG_DATATYPE_Char   );
		case  3: return( SG_DATATYPE_Word   );
		case  4: return( SG_DATATYPE_Short  );
		case  5: return( SG_DATATYPE_DWord  );
		case  6: return( SG_DATATYPE_Int    );
		case  7: return( SG_DATATYPE_Float  );
		case  8: return( SG_DATATYPE_Double );
		}

		return( pShapes->Get_Field_Type(Field) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes2Grid::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pShapes	= Parameters("INPUT")->asShapes();

	//-----------------------------------------------------
	bool	bFat;

	switch( pShapes->Get_Type() )
	{
	default                :	bFat	= false;	break;
	case SHAPE_TYPE_Line   :	bFat	= Parameters("LINE_TYPE")->asInt() == 1;	break;
	case SHAPE_TYPE_Polygon:	bFat	= Parameters("POLY_TYPE")->asInt() == 1;	break;
	}

	//-----------------------------------------------------
	int		Field;

	switch( Parameters("OUTPUT")->asInt() )
	{
	case  0:	Field	= OUTPUT_NODATA;	break;		// data / no-data
	case  1:	Field	= OUTPUT_INDEX ;	break;		// index number
	default:	Field	= Parameters("FIELD")->asInt();	// attribute
		if( Field < 0 || !SG_Data_Type_is_Numeric(pShapes->Get_Field_Type(Field)) )
		{
			Message_Add(_TL("WARNING: selected attribute is not numeric."));
		}
		break;
	}

	//-----------------------------------------------------
	if( (m_pGrid = m_Grid_Target.Get_Grid("GRID", Get_Data_Type(Field))) == NULL )
	{
		return( false );
	}

	if( !pShapes->Get_Extent().Intersects(m_pGrid->Get_Extent()) )
	{
		Error_Set(_TL("Polygons' and target grid's extent do not intersect."));

		return( false );
	}

	if( Field < 0 )
	{
		m_pGrid->Set_NoData_Value(0.);
	}

	m_pGrid->Fmt_Name("%s [%s]", pShapes->Get_Name(), Field < 0 ? _TL("ID") : pShapes->Get_Field_Name(Field));
	m_pGrid->Assign_NoData();

	//-------------------------------------------------
	CSG_Grid	Count;

	m_pCount	= m_Grid_Target.Get_Grid("COUNT", pShapes->Get_Count() < 256 ? SG_DATATYPE_Byte : SG_DATATYPE_Word);

	if( m_pCount == NULL )
	{
		Count.Create(m_pGrid->Get_System(), SG_DATATYPE_Word);

		m_pCount	= &Count;
	}

	m_pCount->Fmt_Name("%s [%s]", pShapes->Get_Name(), _TL("Count"));
	m_pCount->Set_NoData_Value(0.);
	m_pCount->Assign(0.);

	//-----------------------------------------------------
	for(int i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(i);

		if( pShapes->Get_Selection_Count() <= 0 || pShape->is_Selected() )
		{
			if( Field < 0 || !pShape->is_NoData(Field) )
			{
				if( pShape->Intersects(m_pGrid->Get_Extent()) )
				{
					double	Value	= Field >= 0 ? pShape->asDouble(Field) : Field == OUTPUT_INDEX ? i + 1 : 1;

					switch( pShapes->Get_Type() )
					{
					case SHAPE_TYPE_Point:	case SHAPE_TYPE_Points:
						Set_Points	(pShape, Value);
						break;

					case SHAPE_TYPE_Line:
						Set_Line	(pShape, bFat, Value);
						break;

					case SHAPE_TYPE_Polygon:
						Set_Polygon	(pShape, bFat, Value);
						break;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Multiple == 4 )	// mean
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
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CShapes2Grid::Set_Value(int x, int y, double Value)
{
	if( m_pGrid->is_InGrid(x, y, false) )
	{
		if( m_pCount->asInt(x, y) == 0 )
		{
			m_pGrid->Set_Value(x, y, Value);
		}
		else switch( m_Multiple )
		{
		case 0:	// first
			break;

		case 1:	// last
			m_pGrid->Set_Value(x, y, Value);
			break;

		case 2:	// minimum
			if( m_pGrid->asDouble(x, y) > Value )
			{
				m_pGrid->Set_Value(x, y, Value);
			}
			break;

		case 3:	// maximum
			if( m_pGrid->asDouble(x, y) < Value )
			{
				m_pGrid->Set_Value(x, y, Value);
			}
			break;

		case 4:	// mean
			m_pGrid->Add_Value(x, y, Value);
			break;
		}

		m_pCount->Add_Value(x, y, 1);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes2Grid::Set_Points(CSG_Shape *pShape, double Value)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

			Set_Value(
				(int)(0.5 + X_WORLD_TO_GRID(p.x)),
				(int)(0.5 + Y_WORLD_TO_GRID(p.y)), Value
			);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes2Grid::Set_Line(CSG_Shape *pShape, bool bFat, double Value)
{
	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		int	iPoint	= pShape->Get_Type() == SHAPE_TYPE_Polygon ? 0 : 1;

		TSG_Point	a, b	= pShape->Get_Point(0, iPart, iPoint != 0);

		b.x	= X_WORLD_TO_GRID(b.x);
		b.y	= Y_WORLD_TO_GRID(b.y);

		for( ; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			a	= b;	b	= pShape->Get_Point(iPoint, iPart);

			b.x	= X_WORLD_TO_GRID(b.x);
			b.y	= Y_WORLD_TO_GRID(b.y);

			if( bFat )
			{
				Set_Line_Fat(a, b, Value);
			}
			else
			{
				Set_Line_Thin(a, b, Value);
			}
		}
	}
}

//---------------------------------------------------------
void CShapes2Grid::Set_Line_Thin(TSG_Point a, TSG_Point b, double Value)
{
	TSG_Point_Int	A;	A.x	= (int)(a.x	+= 0.5);	A.y	= (int)(a.y	+= 0.5);
	TSG_Point_Int	B;	B.x	= (int)(b.x	+= 0.5);	B.y	= (int)(b.y	+= 0.5);

	//-----------------------------------------------------
	if( A.x != B.x || A.y != B.y )
	{
		double	dx	= b.x - a.x;
		double	dy	= b.y - a.y;

		if( fabs(dx) > fabs(dy) )
		{
			int	sig	= dx < 0 ? -1 : 1;
			dx	= fabs(dx);
			dy	/= dx;

			for(int ix=0; ix<=dx; ix++, a.x+=sig, a.y+=dy)
			{
				Set_Value((int)a.x, (int)a.y, Value);
			}
		}
		else if( fabs(dy) >= fabs(dx) && dy != 0 )
		{
			int	sig	= dy < 0 ? -1 : 1;
			dy	= fabs(dy);
			dx	/= dy;

			for(int iy=0; iy<=dy; iy++, a.x+=dx, a.y+=sig)
			{
				Set_Value((int)a.x, (int)a.y, Value);
			}
		}
	}
	else
	{
		Set_Value(A.x, A.y, Value);
	}
}

//---------------------------------------------------------
void CShapes2Grid::Set_Line_Fat(TSG_Point a, TSG_Point b, double Value)
{
	TSG_Point_Int	A;	A.x	= (int)(a.x	+= 0.5);	A.y	= (int)(a.y	+= 0.5);
	TSG_Point_Int	B;	B.x	= (int)(b.x	+= 0.5);	B.y	= (int)(b.y	+= 0.5);

	Set_Value(A.x, A.y, Value);

	//-----------------------------------------------------
	if( A.x != B.x || A.y != B.y )
	{
		double	dx	= b.x - a.x;
		double	dy	= b.y - a.y;

		a.x	= a.x > 0. ? a.x - (int)a.x : 1. + (a.x - (int)a.x);
		a.y	= a.y > 0. ? a.y - (int)a.y : 1. + (a.y - (int)a.y);

		//-------------------------------------------------
		if( fabs(dx) > fabs(dy) )
		{
			int	ix	= dx > 0. ? 1 : -1;
			int	iy	= dy > 0. ? 1 : -1;

			double	d, e;

			d	= fabs(dy / dx);
			dx	= ix < 0 ? a.x : 1. - a.x;
			e	= iy > 0 ? a.y : 1. - a.y;
			e	+= d * dx;

			while( e > 1. )
			{
				Set_Value(A.x, A.y += iy, Value);	e--;
			}

			while( A.x != B.x )
			{
				Set_Value(A.x += ix, A.y, Value);	e += d;

				if( A.x != B.x )
				{
					while( e > 1. )
					{
						Set_Value(A.x, A.y += iy, Value);	e--;
					}
				}
			}

			if( A.y != B.y )
			{
				iy	= A.y < B.y ? 1 : -1;

				while( A.y != B.y )
				{
					Set_Value(A.x, A.y += iy, Value);
				}
			}
		}

		//-------------------------------------------------
		else // if( fabs(dy) > fabs(dx) )
		{
			int	ix	= dx > 0.0 ? 1 : -1;
			int	iy	= dy > 0.0 ? 1 : -1;

			double	d, e;

			d	= fabs(dx / dy);
			dy	= iy < 0 ? a.y : 1.0 - a.y;
			e	= ix > 0 ? a.x : 1.0 - a.x;
			e	+= d * dy;

			while( e > 1. )
			{
				Set_Value(A.x += ix, A.y, Value);	e--;
			}

			while( A.y != B.y )
			{
				Set_Value(A.x, A.y += iy, Value);	e += d;

				if( A.y != B.y )
				{
					while( e > 1. )
					{
						Set_Value(A.x += ix, A.y, Value);	e--;
					}
				}
			}

			if( A.x != B.x )
			{
				ix	= A.x < B.x ? 1 : -1;

				while( A.x != B.x )
				{
					Set_Value(A.x += ix, A.y, Value);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CShapes2Grid::Set_Polygon(CSG_Shape *pShape, bool bFat, double Value)
{
	Set_Polygon((CSG_Shape_Polygon *)pShape, Value);

	if( bFat )	// all cells intersected have to be marked
	{
		Set_Line(pShape, true, Value);	// thick, each cell crossed by polygon boundary will be marked additionally
	}
}

//---------------------------------------------------------
void CShapes2Grid::Set_Polygon(CSG_Shape_Polygon *pPolygon, double Value)
{
	//-----------------------------------------------------
	bool	*bCrossing	= (bool *)SG_Malloc(m_pGrid->Get_NX() * sizeof(bool));

	CSG_Rect	Extent(pPolygon->Get_Extent());

	int	xStart	= (int)((Extent.Get_XMin() - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize()) - 1; if( xStart < 0 ) xStart = 0;
	int	xStop	= (int)((Extent.Get_XMax() - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize()) + 1; if( xStop >= m_pGrid->Get_NX() ) xStop = m_pGrid->Get_NX() - 1;

	TSG_Point	A, B;

	A.y	= m_pGrid->Get_YMin();
	A.x	= m_pGrid->Get_XMin() - 1.;
	B.x	= m_pGrid->Get_XMax() + 1.;

	//-----------------------------------------------------
	for(int y=0; y<m_pGrid->Get_NY(); y++, A.y+=m_pGrid->Get_Cellsize())
	{
		if( A.y >= Extent.m_rect.yMin && A.y <= Extent.m_rect.yMax )
		{
			B.y	= A.y;

			memset(bCrossing, 0, m_pGrid->Get_NX() * sizeof(bool));

			for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
			{
				if( pPolygon->Get_Part(iPart)->Get_Extent().Intersects(m_pGrid->Get_Extent(true)) )
				{
					TSG_Point	a, b, c;

					b	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

					for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
					{
						a	= b;
						b	= pPolygon->Get_Point(iPoint, iPart);

						if(	((a.y <= A.y && A.y  < b.y)
						||	 (a.y  > A.y && A.y >= b.y)) )
						{
							SG_Get_Crossing(c, a, b, A, B, false);

							int	x	= (int)(1.0 + X_WORLD_TO_GRID(c.x));

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
			bool	bFill	= false;

			for(int x=xStart; x<=xStop; x++)
			{
				if( bCrossing[x] )
				{
					bFill	= !bFill;
				}

				if( bFill )
				{
					Set_Value(x, y, Value);
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
CPolygons2Grid::CPolygons2Grid(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polygons to Grid"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Gridding of polygons. If any polygons are selected, only these will be gridded."
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Table_Field("POLYGONS",
		"FIELD"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Choice("",
		"OUTPUT"	, _TL("Output Values"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("index number"),
			_TL("attribute")
		), 2
	);

	Parameters.Add_Choice("",
		"MULTIPLE"	, _TL("Multiple Polygons"),
		_TL("Output value for cells that intersect wiht more than one polygon."),
		CSG_String::Format("%s|%s|%s|",
			_TL("minimum coverage"),
			_TL("maximum coverage"),
			_TL("average proportional to area coverage")
		), 1
	);

	Parameters.Add_Choice("",
		"GRID_TYPE"	, _TL("Data Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("1 bit"),
			_TL("1 byte unsigned integer"),
			_TL("1 byte signed integer"),
			_TL("2 byte unsigned integer"),
			_TL("2 byte signed integer"),
			_TL("4 byte unsigned integer"),
			_TL("4 byte signed integer"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point"),
			_TL("same as attribute")
		), 9
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

	m_Grid_Target.Add_Grid("GRID"    , _TL("Grid"    ), false);
	m_Grid_Target.Add_Grid("COVERAGE", _TL("Coverage"),  true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPolygons2Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("POLYGONS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CPolygons2Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("OUTPUT") )
	{
		pParameters->Set_Enabled("FIELD"    , pParameter->asInt() == 1);
		pParameters->Set_Enabled("GRID_TYPE", pParameter->asInt() == 1);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Data_Type CPolygons2Grid::Get_Data_Type(int Field)
{
	CSG_Shapes	*pShapes	= Parameters("POLYGONS")->asShapes();

	if( Field >= 0 && (Field >= pShapes->Get_Field_Count() || !SG_Data_Type_is_Numeric(pShapes->Get_Field_Type(Field))) )
	{
		Field	= OUTPUT_INDEX;	// index number
	}

	if( Field <= OUTPUT_INDEX )	// index number
	{
		return( pShapes->Get_Count() < 65535 ? SG_DATATYPE_Word : SG_DATATYPE_DWord );
	}

//	if( Field >= 0 )	// attribute
	{
		switch( Parameters("GRID_TYPE")->asInt() )
		{
		case  0: return( SG_DATATYPE_Bit    );
		case  1: return( SG_DATATYPE_Byte   );
		case  2: return( SG_DATATYPE_Char   );
		case  3: return( SG_DATATYPE_Word   );
		case  4: return( SG_DATATYPE_Short  );
		case  5: return( SG_DATATYPE_DWord  );
		case  6: return( SG_DATATYPE_Int    );
		case  7: return( SG_DATATYPE_Float  );
		case  8: return( SG_DATATYPE_Double );
		}

		return( pShapes->Get_Field_Type(Field) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygons2Grid::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPolygons	= Parameters("POLYGONS")->asShapes();

	m_Multiple	= Parameters("MULTIPLE")->asInt();

	//-----------------------------------------------------
	int		Field;

	switch( Parameters("OUTPUT")->asInt() )
	{
	case  0:	Field	= OUTPUT_INDEX ;	break;		// index number
	default:	Field	= Parameters("FIELD")->asInt();	// attribute
		if( Field < 0 || !SG_Data_Type_is_Numeric(pPolygons->Get_Field_Type(Field)) )
		{
			Message_Add(_TL("WARNING: selected attribute is not numeric."));
		}
		break;
	}

	//-----------------------------------------------------
	if( (m_pGrid = m_Grid_Target.Get_Grid("GRID", Get_Data_Type(Field))) == NULL )
	{
		return( false );
	}

	if( !pPolygons->Get_Extent().Intersects(m_pGrid->Get_Extent()) )
	{
		Error_Set(_TL("Polygons' and target grid's extent do not intersect."));

		return( false );
	}

	if( Field < 0 )
	{
		m_pGrid->Set_NoData_Value(0.);
	}

	m_pGrid->Fmt_Name("%s [%s]", pPolygons->Get_Name(), Field < 0 ? _TL("ID") : pPolygons->Get_Field_Name(Field));
	m_pGrid->Assign_NoData();

	//-------------------------------------------------
	CSG_Grid	Coverage;

	m_pCoverage	= m_Grid_Target.Get_Grid("COVERAGE");

	if( m_pCoverage == NULL )
	{
		Coverage.Create(m_pGrid->Get_System());

		m_pCoverage	= &Coverage;
	}

	m_pCoverage->Fmt_Name("%s [%s]", pPolygons->Get_Name(), _TL("Coverage"));
	m_pCoverage->Set_NoData_Value(0.);
	m_pCoverage->Assign(0.);

	//-----------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(i);

		if( pPolygons->Get_Selection_Count() <= 0 || pPolygon->is_Selected() )
		{
			if( Field < 0 || !pPolygon->is_NoData(Field) )
			{
				if( pPolygon->Intersects(m_pGrid->Get_Extent()) )
				{
					Set_Polygon(pPolygon, Field < 0 ? i + 1 : pPolygon->asDouble(Field));
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Multiple == 2 )	// average
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(sLong i=0; i<m_pGrid->Get_NCells(); i++)
		{
			double	Area	= m_pCoverage->asDouble(i);

			if( Area > 0. )
			{
				m_pGrid->Mul_Value(i, 1. / Area);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CPolygons2Grid::Set_Value(int x, int y, double Value, double Coverage)
{
	if( m_pGrid->is_InGrid(x, y, false) )
	{
		if( m_pCoverage->asDouble(x, y) <= 0. )
		{
			m_pGrid    ->Set_Value(x, y, m_Multiple != 2 ? Value : Coverage * Value);
			m_pCoverage->Set_Value(x, y, Coverage);
		}
		else switch( m_Multiple )
		{
		case  0:	// minimum
			if( m_pCoverage->asDouble(x, y) > Value )
			{
				m_pGrid    ->Set_Value(x, y, Value   );
				m_pCoverage->Set_Value(x, y, Coverage);
			}
			break;

		default:	// maximum
			if( m_pCoverage->asDouble(x, y) < Value )
			{
				m_pGrid    ->Set_Value(x, y, Value   );
				m_pCoverage->Set_Value(x, y, Coverage);
			}
			break;

		case  2:	// average
			m_pGrid    ->Add_Value(x, y, Coverage * Value);
			m_pCoverage->Add_Value(x, y, Coverage);
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPolygons2Grid::Set_Polygon(CSG_Shape_Polygon *pPolygon, double Value)
{
	//-----------------------------------------------------
	CSG_Grid_System	s(m_pGrid->Get_System());

	int	xA	= s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin());	if( xA <  0          )	xA	= 0;
	int	xB	= s.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax());	if( xB >= s.Get_NX() )	xB	= s.Get_NX() - 1;
	int	yA	= s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMin());	if( yA <  0          )	yA	= 0;
	int	yB	= s.Get_yWorld_to_Grid(pPolygon->Get_Extent().Get_YMax());	if( yB >= s.Get_NY() )	yB	= s.Get_NY() - 1;

	//-----------------------------------------------------
	CSG_Shapes	Intersect(SHAPE_TYPE_Polygon);

	CSG_Shape_Polygon	*pCell	= (CSG_Shape_Polygon *)Intersect.Add_Shape();

	TSG_Rect	r;

	r.yMax	= s.Get_yGrid_to_World(yA) - 0.5 * s.Get_Cellsize();

	for(int y=yA; y<=yB; y++)
	{
		r.yMin	= r.yMax;	r.yMax	+= s.Get_Cellsize();

		r.xMax	= s.Get_xGrid_to_World(xA) - 0.5 * s.Get_Cellsize();

		for(int x=xA; x<=xB; x++)
		{
			r.xMin	= r.xMax;	r.xMax	+= s.Get_Cellsize();

			pCell->Add_Point(r.xMin, r.yMin);
			pCell->Add_Point(r.xMin, r.yMax);
			pCell->Add_Point(r.xMax, r.yMax);
			pCell->Add_Point(r.xMax, r.yMin);

			if( SG_Polygon_Intersection(pCell, pPolygon) )
			{
				Set_Value(x, y, Value, pCell->Get_Area());
			}

			pCell->Del_Parts();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
