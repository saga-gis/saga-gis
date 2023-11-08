
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_To_Contour.cpp                  //
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
#include "Grid_To_Contour.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_To_Contour::CGrid_To_Contour(void)
{
	Set_Name		(_TL("Contour Lines from Grid"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Derive contour lines (isolines) from a grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  (""        , "GRID"      , _TL("Grid"               ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Shapes(""        , "CONTOUR"   , _TL("Contour"            ), _TL(""), PARAMETER_OUTPUT         , SHAPE_TYPE_Line   );
	Parameters.Add_Choice("CONTOUR" , "VERTEX"    , _TL("Vertex Type"        ), _TL(""), "x, y|x, y, z", 0);
	Parameters.Add_Bool  ("CONTOUR" , "LINE_PARTS", _TL("Split Line Parts"   ), _TL(""),  true);

	Parameters.Add_Shapes(""        , "POLYGONS"  , _TL("Polygons"           ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Bool  ("POLYGONS", "POLY_PARTS", _TL("Split Polygon Parts"), _TL(""), false);

	Parameters.Add_Double("",
		"SCALE"    , _TL("Interpolation Scale"),
		_TL("set greater one for line smoothing"),
		1., 0., true
	);

	Parameters.Add_Bool("",
		"BOUNDARY" , _TL("Boundary"),
		_TL("Extend contours beyond boundary. Extrapolates input grid internally by one cell size."),
		false
	);

	Parameters.Add_Double("",
		"MINLENGTH", _TL("Minimum Length"),
		_TL("Contour line segments with minimum length [map units] or less will be removed from resulting data set."),
		0., 0., true
	);

	Parameters.Add_Choice("",
		"INTERVALS", _TL("Interval Definition"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("single value"),
			_TL("equal intervals"),
			_TL("from list")
		), 1
	);

	Parameters.Add_Double("INTERVALS", "ZMIN" , _TL("Base Contour Value"   ), _TL(""),    0.);
	Parameters.Add_Double("INTERVALS", "ZMAX" , _TL("Maximum Contour Value"), _TL(""), 1000.);
	Parameters.Add_Double("INTERVALS", "ZSTEP", _TL("Contour Interval"     ), _TL(""),  100., 0., true);
	Parameters.Add_String("INTERVALS", "ZLIST", _TL("Contour Values"       ), _TL("List of comma separated values."), "0, 10, 20, 50, 100, 200, 500, 1000");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_To_Contour::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID") && pParameter->asGrid() != NULL )
	{
		double zStep = SG_Get_Rounded_To_SignificantFigures(pParameter->asGrid()->Get_Range() / 10., 1);

		pParameters->Set_Parameter("ZSTEP", zStep);

		if( zStep > 0. )
		{
			pParameters->Set_Parameter("ZMIN", zStep * (floor(pParameter->asGrid()->Get_Min() / zStep) + 1.));
			pParameters->Set_Parameter("ZMAX", zStep * (ceil (pParameter->asGrid()->Get_Max() / zStep) - 1.));
		}
		else
		{
			pParameters->Set_Parameter("ZMIN", pParameter->asGrid()->Get_Min());
			pParameters->Set_Parameter("ZMAX", pParameter->asGrid()->Get_Max());
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_To_Contour::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("INTERVALS") )
	{
		pParameters->Set_Enabled("ZMIN" , pParameter->asInt() != 2);
		pParameters->Set_Enabled("ZSTEP", pParameter->asInt() == 1);
		pParameters->Set_Enabled("ZLIST", pParameter->asInt() == 2);
	}

	if( pParameter->Cmp_Identifier("INTERVALS") || pParameter->Cmp_Identifier("ZSTEP") )
	{
		pParameters->Set_Enabled("ZMAX", (*pParameters)("INTERVALS")->asInt() == 1 && (*pParameters)("ZSTEP")->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("POLYGONS") )
	{
		pParameters->Set_Enabled("POLY_PARTS", pParameter->asPointer() != NULL);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::On_Execute(void)
{
	m_pGrid = Parameters("GRID")->asGrid();

	m_zMin = m_pGrid->Get_Min(); m_zMax = m_pGrid->Get_Max();

	if( m_pGrid->Get_Max_Samples() < m_pGrid->Get_NCells() )
	{
		for(sLong i=0; i<m_pGrid->Get_NCells(); i++)
		{
			if( !m_pGrid->is_NoData(i) )
			{
				double z = m_pGrid->asDouble(i);

				if( m_zMin > z ) { m_zMin = z; } else if( m_zMax < z ) { m_zMax = z; }
			}
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pContours = Parameters("CONTOUR")->asShapes();

	pContours->Create(SHAPE_TYPE_Line, NULL, NULL, Parameters("VERTEX")->asInt() == 0 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZ);

	pContours->Add_Field("ID"   , SG_DATATYPE_Int   );
	pContours->Add_Field("VALUE", SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Vector Intervals;

	if( Parameters("INTERVALS")->asInt() == 2 ) // from list
	{
		CSG_Strings s = SG_String_Tokenize(Parameters("ZLIST")->asString(), ",;");

		for(int i=0; i<s.Get_Count(); i++)
		{
			double Value;

			if( s[i].asDouble(Value) && Value >= m_zMin && Value <= m_zMax )
			{
				Intervals.Add_Row(Value);
			}
		}

		pContours->Fmt_Name("%s [%s]", m_pGrid->Get_Name(), _TL("Contours"));
	}
	else if( Parameters("INTERVALS")->asInt() == 1 && Parameters("ZSTEP")->asDouble() > 0. ) // equal intervals
	{
		double Value = Parameters("ZMIN" )->asDouble();
		double   Max = Parameters("ZMAX" )->asDouble();
		double  Step = Parameters("ZSTEP")->asDouble();

		for( ; Value<=Max; Value+=Step)
		{
			if( Value >= m_zMin && Value <= m_zMax )
			{
				Intervals.Add_Row(Value);
			}
		}

		pContours->Fmt_Name("%s [%s %s]", m_pGrid->Get_Name(), _TL("Interval"), SG_Get_String(Step, -10).c_str());
	}
	else // single value
	{
		double Value = Parameters("ZMIN")->asDouble();

		if( Value >= m_zMin && Value <= m_zMax )
		{
			Intervals.Add_Row(Value);
		}

		pContours->Fmt_Name("%s [%s %s]", m_pGrid->Get_Name(), _TL("Contour" ), SG_Get_String(Value, -10).c_str());
	}

	if( Intervals.Get_Size() == 0 )
	{
		Error_Fmt(_TL("requested contour values are out of range"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid Patched;

	if( Parameters("BOUNDARY")->asBool() )
	{
		CSG_Rect Extent(m_pGrid->Get_Extent()); Extent.Inflate(m_pGrid->Get_Cellsize(), false);

		if( !Patched.Create(CSG_Grid_System(m_pGrid->Get_Cellsize(), Extent)) )
		{
			Error_Set(_TL("could allocate memory for patched grid"));

			return( false );
		}

		Patched.Assign(m_pGrid, GRID_RESAMPLING_NearestNeighbour);

		#pragma omp parallel for
		for(int y=0; y<Patched.Get_NY(); y++) for(int x=0; x<Patched.Get_NX(); x++)
		{
			if( Patched.is_NoData(x, y) )
			{
				CSG_Simple_Statistics s;

				for(int i=0; i<8; i++)
				{
					int ix = CSG_Grid_System::Get_xTo(i, x - 1);
					int iy = CSG_Grid_System::Get_yTo(i, y - 1);

					if( m_pGrid->is_InGrid(ix, iy) )
					{
						s += m_pGrid->asDouble(ix, iy);
					}
				}

				if( s.Get_Count() )
				{
					Patched.Set_Value(x, y, s.Get_Mean());
				}
			}
		}

		m_pGrid	= &Patched;
	}

	//-----------------------------------------------------
	CSG_Grid Scaled; double Scale = Parameters("SCALE")->asDouble();

	if( Scale > 0. && Scale != 1. )
	{
		if( !Scaled.Create(CSG_Grid_System(m_pGrid->Get_Cellsize() / Scale, m_pGrid->Get_Extent())) )
		{
			Error_Set(_TL("could allocate memory for scaled grid"));

			return( false );
		}

		Scaled.Assign(m_pGrid, GRID_RESAMPLING_BSpline);

		m_pGrid	= &Scaled; Patched.Destroy();
	}

	//-----------------------------------------------------
	double minLength = Parameters("MINLENGTH")->asDouble();

	m_Edge.Create(SG_DATATYPE_Char, m_pGrid->Get_NX() + 1, m_pGrid->Get_NY() + 1, m_pGrid->Get_Cellsize(), m_pGrid->Get_XMin(), m_pGrid->Get_YMin());

	Intervals.Sort();

	for(int i=0; i<Intervals.Get_N() && Set_Progress(i, Intervals.Get_N()); i++)
	{
		if( i == 0 || Intervals[i] != Intervals[i - 1] )
		{
			Process_Set_Text("%s: %s", _TL("Contour"), SG_Get_String(Intervals[i], -2).c_str());

			Get_Contour(pContours, Intervals[i], minLength);
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	if( pPolygons && pContours->Get_Count() > 0 && Set_Progress(0.) )
	{
		pPolygons->Create(SHAPE_TYPE_Polygon, pContours->Get_Name(), NULL,
			Parameters("VERTEX")->asInt() == 0 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZ
		);

		pPolygons->Add_Field("ID"   , SG_DATATYPE_Int   );
		pPolygons->Add_Field("MIN"  , SG_DATATYPE_Double);
		pPolygons->Add_Field("MAX"  , SG_DATATYPE_Double);
		pPolygons->Add_Field("LABEL", SG_DATATYPE_String);

		Get_Polygons(pPolygons, NULL, pContours->Get_Shape(0)->asLine());

		for(sLong i=1; i<pContours->Get_Count() && Set_Progress(i, pContours->Get_Count()); i++)
		{
			Get_Polygons(pPolygons, pContours->Get_Shape(i - 1)->asLine(), pContours->Get_Shape(i)->asLine());
		}

		if( Set_Progress(1.) )
		{
			Get_Polygons(pPolygons, pContours->Get_Shape(pContours->Get_Count() - 1)->asLine(), NULL);
		}

		//-------------------------------------------------
		if( Parameters("POLY_PARTS")->asBool() )
		{
			Split_Polygon_Parts(pPolygons);
		}
	}

	m_Edge.Destroy();

	//-----------------------------------------------------
	if( Parameters("LINE_PARTS")->asBool() )
	{
		Split_Line_Parts(pContours);
	}

	//-----------------------------------------------------
	return( pContours->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define EDGE_ROW 0x01
#define EDGE_COL 0x02

//---------------------------------------------------------
inline bool CGrid_To_Contour::is_Edge(int x, int y)
{
	return(  m_pGrid->is_InGrid(x    , y    )
		&& (!m_pGrid->is_InGrid(x - 1, y - 1)
		||  !m_pGrid->is_InGrid(x    , y - 1)
		||  !m_pGrid->is_InGrid(x + 1, y - 1)
		||  !m_pGrid->is_InGrid(x + 1, y    )
		||  !m_pGrid->is_InGrid(x + 1, y + 1)
		||  !m_pGrid->is_InGrid(x    , y + 1)
		||  !m_pGrid->is_InGrid(x - 1, y + 1)
		||  !m_pGrid->is_InGrid(x - 1, y    ) )
	);
}

//---------------------------------------------------------
inline void CGrid_To_Contour::Set_Row(int x, int y, bool bOn)
{
	int	Edge = m_Edge.asInt(x, y);

	m_Edge.Set_Value(x, y, bOn ? (Edge | EDGE_ROW) : (Edge & EDGE_COL));
}

inline bool CGrid_To_Contour::Get_Row(int x, int y)
{
	return( (m_Edge.asInt(x, y) & EDGE_ROW) != 0 );
}

//---------------------------------------------------------
inline void CGrid_To_Contour::Set_Col(int x, int y, bool bOn)
{
	int	Edge = m_Edge.asInt(x, y);

	m_Edge.Set_Value(x, y, bOn ? (Edge | EDGE_COL) : (Edge & EDGE_ROW));
}

inline bool CGrid_To_Contour::Get_Col(int x, int y)
{
	return( (m_Edge.asInt(x, y) & EDGE_COL) != 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Contour(CSG_Shapes *pContours, double z, double minLength)
{
	#pragma omp parallel for
	for(int y=0; y<m_pGrid->Get_NY(); y++)	// Find Border Cells
	{
		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			if( !m_pGrid->is_NoData(x, y) )
			{
				if( m_pGrid->asDouble(x, y) >= z )
				{
					if( m_pGrid->is_InGrid(x + 1, y    ) && m_pGrid->asDouble(x + 1, y    ) <  z ) Set_Row(x, y, true);
					if( m_pGrid->is_InGrid(x    , y + 1) && m_pGrid->asDouble(x    , y + 1) <  z ) Set_Col(x, y, true);
				}
				else // if( m_pGrid->asDouble(x, y) < z )
				{
					if( m_pGrid->is_InGrid(x + 1, y    ) && m_pGrid->asDouble(x + 1, y    ) >= z ) Set_Row(x, y, true);
					if( m_pGrid->is_InGrid(x    , y + 1) && m_pGrid->asDouble(x    , y + 1) >= z ) Set_Col(x, y, true);
				}
			}
		}
 	}

	//-----------------------------------------------------
	CSG_Shape_Line *pContour = pContours->Add_Shape()->asLine();

	pContour->Set_Value(0, pContours->Get_Count());
	pContour->Set_Value(1, z);

	//-----------------------------------------------------
	for(int y=0; y<m_pGrid->Get_NY(); y++)	// find unclosed contours first so that these start/end at edges and not somewhere else
	{
		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			if( m_Edge.asInt(x, y) && is_Edge(x, y) )
			{
				Get_Contour(pContour, z, x, y);
			}
		}
	}

	for(int y=0; y<m_pGrid->Get_NY(); y++)
	{
		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			while( Get_Contour(pContour, z, x, y) );
		}
	}

	//-----------------------------------------------------
	for(int i=pContour->Get_Part_Count()-1; i>=0; i--)
	{
		if( (pContour->Get_Length(i) <= minLength && CSG_Point(pContour->Get_Point(0, i, true)) == pContour->Get_Point(0, i, false)) )
		{
			pContour->Del_Part(i);
		}
	}

	if( pContour->Get_Part_Count() < 1 )
	{
		pContours->Del_Shape(pContour);

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Contour(CSG_Shape_Line *pContour, double z, int x, int y)
{
	bool bRow;

	if( Get_Row(x, y) )
	{
		bRow = true;
	}
	else if( Get_Col(x, y) )
	{
		bRow = false;
	}
	else
	{
		return( false );
	}

	//-----------------------------------------------------
	int iPart = pContour->Get_Part_Count();

	for(int Dir=0, x0=x, y0=y, bRow0=bRow?1:0; Dir>=0; )
	{
		int zx = bRow ? x + 1 : x;
		int zy = bRow ? y : y + 1;

		double d = m_pGrid->asDouble(x, y); d = (d - z) / (d - m_pGrid->asDouble(zx, zy));

		pContour->Add_Point(
			m_pGrid->Get_XMin() + m_pGrid->Get_Cellsize() * (x + d * (zx - (double)x)),
			m_pGrid->Get_YMin() + m_pGrid->Get_Cellsize() * (y + d * (zy - (double)y)), iPart
		);

		if( pContour->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
		{
			pContour->Set_Z(z, pContour->Get_Point_Count(iPart) - 1, iPart);
		}

		if( Get_Contour_Cell(Dir, x, y, bRow) || Get_Contour_Cell(Dir, x, y, bRow) )
		{
			if( bRow  )	{ Set_Row(x , y , false); } else { Set_Col(x , y , false); }
		}
		else
		{
			if( bRow0 )	{ Set_Row(x0, y0, false); } else { Set_Col(x0, y0, false); }

			Dir	= -1;
		}
	}

	//-----------------------------------------------------
	return( pContour->Get_Point_Count(iPart) > 1 ); // found at least one contour pixel
}

//---------------------------------------------------------
inline bool CGrid_To_Contour::Get_Contour_Cell(int &Dir, int &x, int &y, bool &bRow)
{
	if( bRow )
	{
		switch( Dir )
		{
		case  0: if( Get_Row(x    , y + 1) ) { Dir = 5;                    y++; return( true ); }	// Norden
		case  1: if( Get_Col(x + 1, y    ) ) { Dir = 6; bRow = false; x++;      return( true ); }	// Nord-Ost
		case  2:	// Osten ist nicht...
		case  3: if( y - 1 >= 0
				 &&  Get_Col(x + 1, y - 1) ) { Dir = 0; bRow = false; x++; y--; return( true ); }	// Sued-Ost
		case  4: if( y - 1 >= 0
				 &&  Get_Row(x    , y - 1) ) { Dir = 1;                    y--; return( true ); }	// Sueden
		case  5: if( y - 1 >= 0
				 &&  Get_Col(x    , y - 1) ) { Dir = 2; bRow = false;      y--; return( true ); }	// Sued-West
		case  6:	// Westen ist nicht...
		case  7: if( Get_Col(x    , y    ) ) { Dir = 4; bRow = false;           return( true ); }	// Nord-West
		default:
			Dir = 0;
		}
	}
	else
	{
		switch( Dir )
		{
		case  0:	// Norden ist nicht...
		case  1: if( Get_Row(x    , y + 1) ) { Dir = 6; bRow =  true;      y++; return( true ); }	// Nord-Ost
		case  2: if( Get_Col(x + 1, y    ) ) { Dir = 7;               x++;      return( true ); }	// Osten
		case  3: if( Get_Row(x    , y    ) ) { Dir = 0; bRow =  true;           return( true ); }	// Sued-Ost
		case  4:	// Sueden ist nicht...
		case  5: if( x - 1 >= 0
				 &&  Get_Row(x - 1, y    ) ) { Dir = 2; bRow =  true; x--;      return( true ); }	// Sued-West
		case  6: if( x - 1 >= 0
				 &&  Get_Col(x - 1, y    ) ) { Dir = 3;               x--;      return( true ); }	// Westen
		case  7: if( x - 1 >= 0
				 &&  Get_Row(x - 1, y + 1) ) { Dir = 5; bRow =  true; x--; y++; return( true ); }	// Nord-West
		default:
			Dir = 0;
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define EDGE_LINE     1
#define EDGE_NODE_ONE 2
#define EDGE_NODE_TWO 3

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Polygons(CSG_Shapes *pPolygons, CSG_Shape_Line *pContour_Lo, CSG_Shape_Line *pContour_Hi)
{
	double zMin = pContour_Lo ? pContour_Lo->asDouble(1) : m_zMin;
	double zMax = pContour_Hi ? pContour_Hi->asDouble(1) : m_zMax;

	#pragma omp parallel for
	for(int y=0; y<m_pGrid->Get_NY(); y++)
	{
		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			m_Edge.Set_Value(x, y, is_Edge(x, y) && zMin <= m_pGrid->asDouble(x, y) && m_pGrid->asDouble(x, y) <= zMax ? EDGE_LINE : 0);
		}
	}

	//-----------------------------------------------------
	CSG_Shape_Polygon *pPolygon = pPolygons->Add_Shape()->asPolygon();

	pPolygon->Set_Value(0, pPolygons->Get_Count());
	pPolygon->Set_Value(1, zMin);
	pPolygon->Set_Value(2, zMax);
	pPolygon->Set_Value(3, SG_Get_String(zMin) + " - " + SG_Get_String(zMax));

	//-----------------------------------------------------
	CSG_Shapes	Segments(SHAPE_TYPE_Line);

	Segments.Add_Field("x0", SG_DATATYPE_Int);
	Segments.Add_Field("y0", SG_DATATYPE_Int);
	Segments.Add_Field("x1", SG_DATATYPE_Int);
	Segments.Add_Field("y1", SG_DATATYPE_Int);
	
	if( pContour_Lo ) { Add_Contour(Segments, pPolygon, pContour_Lo); }
	if( pContour_Hi ) { Add_Contour(Segments, pPolygon, pContour_Hi); }

	//-----------------------------------------------------
	if( Segments.Get_Count() == 0 )	// no edge crossing line
	{
		for(int y=0; y<m_pGrid->Get_NY(); y++)
		{
			for(int x=0; x<m_pGrid->Get_NX(); x++)
			{
				if( m_Edge.asInt(x, y) == EDGE_LINE )	// add outer ring
				{
					Add_Edge(Segments, x, y);

					if( Segments.Get_Count() )
					{
						pPolygon->Add_Part(Segments.Get_Shape(0)->Get_Part(0));

						Segments.Del_Shapes();
					}
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
//	{	CSG_Grid *pGrid	= SG_Create_Grid(m_Edge); pGrid->Set_NoData_Value(0); DataObject_Add(pGrid);	}
	for(int y=0; y<m_pGrid->Get_NY(); y++)
	{
		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			if( m_Edge.asInt(x, y) == EDGE_NODE_ONE )
			{
				Add_Edge(Segments, x, y);
			}
		}
	}
//	DataObject_Add(SG_Create_Shapes(Segments));

	//-----------------------------------------------------
	while( Segments.Get_Count() > 0 )
	{
		int iPart = pPolygon->Get_Part_Count();

		CSG_Shape *pSegment = Segments.Get_Shape(0);

		int x = pSegment->asInt(2);
		int y = pSegment->asInt(3);

		bool bAscending = true;

		do
		{
			Add_Segment(pPolygon, iPart, pSegment, bAscending);
		}
		while( (pSegment = Get_Segment(Segments, x, y, bAscending)) != NULL );

		if( pPolygon->Get_Point_Count(iPart) < 3 )
		{
			pPolygon->Del_Part(iPart);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Add_Contour(CSG_Shapes &Segments, CSG_Shape_Polygon *pPolygon, CSG_Shape_Line *pContour)
{
	for(int iPart=0, x, y; iPart<pContour->Get_Part_Count(); iPart++)
	{
		CSG_Shape_Part *pPart = pContour->Get_Part(iPart);

		if( !CSG_Point(pPart->Get_Point(0, true)).is_Equal(pPart->Get_Point(0, false)) )	// island
		{
			CSG_Shape *pSegment = Segments.Add_Shape();	pSegment->Add_Part(pPart);

			m_Edge.Get_System().Get_World_to_Grid(x, y, pPart->Get_Point(0,  true));
			m_Edge.Set_Value(x, y, m_Edge.asInt(x, y) == EDGE_NODE_ONE ? EDGE_NODE_TWO : EDGE_NODE_ONE);
			pSegment->Set_Value(0, x);
			pSegment->Set_Value(1, y);

			m_Edge.Get_System().Get_World_to_Grid(x, y, pPart->Get_Point(0, false));
			m_Edge.Set_Value(x, y, m_Edge.asInt(x, y) == EDGE_NODE_ONE ? EDGE_NODE_TWO : EDGE_NODE_ONE);
			pSegment->Set_Value(2, x);
			pSegment->Set_Value(3, y);
		}
		else if( pPart->Get_Count() >= 3 )
		{
			pPolygon->Add_Part(pPart);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Add_Edge(CSG_Shapes &Segments, int x, int y)
{
	CSG_Shape *pSegment = NULL;

	for(int i=0; i<8 && !pSegment; i+=2)
	{
		int ix = CSG_Grid_System::Get_xTo(i, x);
		int iy = CSG_Grid_System::Get_yTo(i, y);

		if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) == EDGE_LINE )
		{
			pSegment = Segments.Add_Shape();

			pSegment->Set_Value(0, x);
			pSegment->Set_Value(1, y);

			x = ix; y = iy;
		}
	}

	if( !pSegment )
	{
		for(int i=0; i<8 && !pSegment; i+=2)
		{
			int ix = CSG_Grid_System::Get_xTo(i, x);
			int iy = CSG_Grid_System::Get_yTo(i, y);

			if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) == EDGE_NODE_ONE )
			{
				pSegment = Segments.Add_Shape();

				pSegment->Set_Value(0,  x);
				pSegment->Set_Value(1,  y);
				pSegment->Set_Value(2, ix);
				pSegment->Set_Value(3, iy);
			}
		}

		return( pSegment != NULL );
	}

	//-----------------------------------------------------
	int iLast = -1;

	for(bool bNext=true; bNext; )
	{
		bNext = false;

		m_Edge.Set_Value(x, y, 0);

		for(int i=0; i<8 && !bNext; i+=2)
		{
			int ix = CSG_Grid_System::Get_xTo(i, x);
			int iy = CSG_Grid_System::Get_yTo(i, y);

			if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) == EDGE_LINE )
			{
				bNext = true;

				if( i != iLast )
				{
					iLast = i;

					pSegment->Add_Point(m_Edge.Get_System().Get_Grid_to_World(x, y));
				}

				x = ix; y = iy;
			}
		}
	}

	//-----------------------------------------------------
	pSegment->Add_Point(m_Edge.Get_System().Get_Grid_to_World(x, y));

	for(int i=0; i<8; i+=2)
	{
		int ix = CSG_Grid_System::Get_xTo(i, x);
		int iy = CSG_Grid_System::Get_yTo(i, y);

		if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) != 0 )
		{
			pSegment->Set_Value(2, ix);
			pSegment->Set_Value(3, iy);

			return( true );
		}
	}

	pSegment->Set_Value(2, x);
	pSegment->Set_Value(3, y);

	return( false );
}

//---------------------------------------------------------
CSG_Shape * CGrid_To_Contour::Get_Segment(const CSG_Shapes &Segments, int &x, int &y, bool &bAscending)
{
	for(sLong i=0; i<Segments.Get_Count(); i++)
	{
		if( Segments[i].asInt(0) == x && Segments[i].asInt(1) == y )
		{
			bAscending	= true;

			x	= Segments[i].asInt(2);
			y	= Segments[i].asInt(3);

			return( Segments.Get_Shape(i) );
		}

		if( Segments[i].asInt(2) == x && Segments[i].asInt(3) == y )
		{
			bAscending	= false;

			x	= Segments[i].asInt(0);
			y	= Segments[i].asInt(1);

			return( Segments.Get_Shape(i) );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Add_Segment(CSG_Shape *pPolygon, int iPart, CSG_Shape *pSegment, bool bAscending)
{
	for(int i=0; i<pSegment->Get_Point_Count(0); i++)
	{
		pPolygon->Add_Point(pSegment->Get_Point(i, 0, bAscending), iPart);
	}

	((CSG_Shapes *)pSegment->Get_Table())->Del_Shape(pSegment);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Split_Line_Parts(CSG_Shapes *pLines)
{
	sLong nLines = pLines->Get_Count();

	for(sLong iLine=nLines-1; iLine>=0 && Set_Progress(nLines - 1 - iLine, nLines); iLine--)
	{
		CSG_Shape_Line *pLine = pLines->Get_Shape(iLine)->asLine();

		for(int iPart=0; iPart<pLine->Get_Part_Count() && Process_Get_Okay(); iPart++)
		{
			CSG_Shape_Line *pPart = (CSG_Shape_Line *)pLines->Add_Shape(pLine, TSG_ADD_Shape_Copy_Mode::SHAPE_COPY_ATTR);

			pPart->Add_Part(pLine->Get_Part(iPart));
		}

		pLines->Del_Shape(iLine);
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Split_Polygon_Parts(CSG_Shapes *pPolygons)
{
	CSG_Shapes Polygons(*pPolygons);

	pPolygons->Del_Shapes();

	//-----------------------------------------------------
	for(sLong iPolygon=0; iPolygon<Polygons.Get_Count() && Set_Progress(iPolygon, Polygons.Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon *pPolygon = Polygons.Get_Shape(iPolygon)->asPolygon();

		for(int iPart=0; iPart<pPolygon->Get_Part_Count() && Process_Get_Okay(); iPart++)
		{
			if( !pPolygon->is_Lake(iPart) )
			{
				CSG_Shape_Polygon *pPart = pPolygons->Add_Shape(pPolygon, SHAPE_COPY_ATTR)->asPolygon();

				pPart->Add_Part(pPolygon->Get_Part(iPart));

				for(int jPart=0; jPart<pPolygon->Get_Part_Count(); jPart++)
				{
					if(	pPolygon->is_Lake(jPart) && pPart->Contains(pPolygon->Get_Point(0, jPart)) )
					{
						pPart->Add_Part(pPolygon->Get_Part(jPart));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
