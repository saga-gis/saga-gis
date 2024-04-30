
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
	Parameters.Add_Bool  ("CONTOUR" , "LINE_OMP"  , _TL("Parallel Processing"), _TL(""), true);
	Parameters.Add_Bool  ("CONTOUR" , "LINE_PARTS", _TL("Split Line Parts"   ), _TL(""), true);

	Parameters.Add_Shapes(""        , "POLYGONS"  , _TL("Polygons"           ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon);
	Parameters.Add_Bool  ("POLYGONS", "POLY_OMP"  , _TL("Parallel Processing"), _TL(""), true);
	Parameters.Add_Bool  ("POLYGONS", "POLY_PARTS", _TL("Split Polygon Parts"), _TL(""), true);

	Parameters.Add_Double("",
		"SCALE"    , _TL("Interpolation Scale"),
		_TL("set greater one for line smoothing"),
		1., 0., true
	);

	Parameters.Add_Bool("",
		"BOUNDARY" , _TL("Boundary Extension"),
		_TL("Extend contours beyond boundary. Internally input grid is extrapolated by one cell size in each direction (top, down, left, right)."),
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
			pParameters->Set_Parameter("ZMIN", zStep * (floor(pParameter->asGrid()->Get_Min() / zStep)));
			pParameters->Set_Parameter("ZMAX", zStep * (ceil (pParameter->asGrid()->Get_Max() / zStep)));
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
		pParameter->Set_Children_Enabled(pParameter->asPointer() != NULL);
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

			if( s[i].asDouble(Value) && Value >= m_pGrid->Get_Min() && Value <= m_pGrid->Get_Max() )
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
			if( Value >= m_pGrid->Get_Min() && Value <= m_pGrid->Get_Max() )
			{
				Intervals.Add_Row(Value);
			}
		}

		pContours->Fmt_Name("%s [%s %s]", m_pGrid->Get_Name(), _TL("Interval"), SG_Get_String(Step, -10).c_str());
	}
	else // single value
	{
		double Value = Parameters("ZMIN")->asDouble();

		if( Value >= m_pGrid->Get_Min() && Value <= m_pGrid->Get_Max() )
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

	Intervals.Sort();

	pContours->Set_Count(Intervals.Get_Size());

	if( Parameters("LINE_OMP")->asBool() )
	{
		m_Flags.Create(m_pGrid->Get_System(), SG_OMP_Get_Max_Num_Threads(), 0., SG_DATATYPE_Char);

		#pragma omp parallel for
		for(int i=0; i<Intervals.Get_N(); i++)
		{
			if( i == 0 || Intervals[i] != Intervals[i - 1] )
			{
				Get_Contour(pContours->Get_Shape(i)->asLine(), Intervals[i], minLength);
			}
		}
	}
	else
	{
		m_Flags.Create(m_pGrid->Get_System(), 1, 0., SG_DATATYPE_Char);

		for(int i=0; i<Intervals.Get_N() && Set_Progress(i, Intervals.Get_N()); i++)
		{
			if( i == 0 || Intervals[i] != Intervals[i - 1] )
			{
				Get_Contour(pContours->Get_Shape(i)->asLine(), Intervals[i], minLength);
			}
		}
	}

	m_Flags.Destroy();

	for(sLong i=pContours->Get_Count()-1; i>=0; i--)
	{
		if( pContours->Get_Shape(i)->Get_Part_Count() < 1 )
		{
			pContours->Del_Shape(i);
		}
	}

	//-----------------------------------------------------
	CSG_Shapes *pPolygons = Parameters("POLYGONS")->asShapes();

	if( pPolygons && pContours->Get_Count() > 0 && Set_Progress(0.) )
	{
		CSG_Shapes Edges; Get_Edge_Segments(Edges, pContours);

		pPolygons->Create(SHAPE_TYPE_Polygon, pContours->Get_Name(), NULL,
			Parameters("VERTEX")->asInt() == 0 ? SG_VERTEX_TYPE_XY : SG_VERTEX_TYPE_XYZ
		);

		pPolygons->Add_Field("ID"   , SG_DATATYPE_Int   );
		pPolygons->Add_Field("MIN"  , SG_DATATYPE_Double);
		pPolygons->Add_Field("MAX"  , SG_DATATYPE_Double);
		pPolygons->Add_Field("LABEL", SG_DATATYPE_String);

		if( Parameters("POLY_OMP")->asBool() )
		{
			pPolygons->Set_Count(pContours->Get_Count() + 1);

			#pragma omp parallel for
			for(int i=0; i<=(int)pContours->Get_Count(); i++) // omp seems not to work with non-standard variables (=>sLong) ?!
			{
				Get_Polygons(*pPolygons->Get_Shape(i)->asPolygon(), Edges,
					i > 0                      ? pContours->Get_Shape(i - 1)->asLine() : NULL,
					i < pContours->Get_Count() ? pContours->Get_Shape(i    )->asLine() : NULL
				);
			}
		}
		else
		{
			for(sLong i=0; i<=pContours->Get_Count() && Set_Progress(i, pContours->Get_Count()); i++)
			{
				Get_Polygons(*pPolygons->Add_Shape()->asPolygon(), Edges,
					i > 0                      ? pContours->Get_Shape(i - 1)->asLine() : NULL,
					i < pContours->Get_Count() ? pContours->Get_Shape(i    )->asLine() : NULL
				);
			}
		}

		//-------------------------------------------------
		for(sLong i=pPolygons->Get_Count()-1; i>=0; i--)
		{
			if( pPolygons->Get_Shape(i)->Get_Part_Count() < 1 )
			{
				pPolygons->Del_Shape(i);
			}
		}

		if( Parameters("POLY_PARTS")->asBool() )
		{
			Split_Polygon_Parts(pPolygons);
		}
	}

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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Contour(CSG_Shape_Line *pContour, double z, double minLength)
{
	if( !SG_OMP_Get_Thread_Num() )
	{
		Process_Set_Text("%s: %s", _TL("Contour"), SG_Get_String(z, -2).c_str());
	}

	pContour->Set_Value(0, 1 + pContour->Get_Index());
	pContour->Set_Value(1, z);

	//-----------------------------------------------------
	CSG_Grid &m_Flag = m_Flags[SG_OMP_Get_Thread_Num()];

//	#pragma omp parallel for
	for(int y=0; y<m_pGrid->Get_NY(); y++) for(int x=0; x<m_pGrid->Get_NX(); x++) // Find Border Cells
	{
		if( m_pGrid->is_NoData(x, y) )
		{
			m_Flag.Set_NoData(x, y);
		}
		else if( m_pGrid->asDouble(x, y) < z )
		{
			m_Flag.Set_Value(x, y, -1);
		}
		else // if( m_pGrid->asDouble(x, y) >= z )
		{
			int Flag = 0;

			for(int i=0; i<8; i+=2)
			{
				int ix = CSG_Grid_System::Get_xTo(i, x);
				int iy = CSG_Grid_System::Get_yTo(i, y);

				if( m_pGrid->is_InGrid(ix, iy) && m_pGrid->asDouble(ix, iy) < z )
				{
					Flag++;
				}
			}

			m_Flag.Set_Value(x, y, Flag);
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<m_pGrid->Get_NY(); y++) for(int x=0; x<m_pGrid->Get_NX(); x++) // find unclosed contours first so that these start/end at edges and not somewhere else
	{
		if( is_Edge(x, y) )
		{
			Get_Contour(pContour, z, x, y, true);
		}
	}

	for(int y=0; y<m_pGrid->Get_NY(); y++) for(int x=0; x<m_pGrid->Get_NX(); x++)
	{
		while( Get_Contour(pContour, z, x, y, false) );
	}

	//-----------------------------------------------------
	for(int i=pContour->Get_Part_Count()-1; i>=0; i--)
	{
		if( pContour->Get_Length(i) <= minLength )
		{
			pContour->Del_Part(i);
		}
	}

	return( pContour->Get_Part_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Contour(CSG_Shape_Line *pContour, double z, int x, int y, bool bEdge)
{
	int Dir = Get_Contour_Vertex_First(x, y, bEdge);

	if( Dir >= 0 )
	{
		int iPart = pContour->Get_Part_Count();

		do
		{
			Add_Contour_Vertex(pContour, iPart, z, x, y, Dir);
		}
		while( Get_Contour_Vertex_Next(x, y, Dir) );

		//-------------------------------------------------
		if( pContour->Get_Point_Count(iPart) < 2 ) // found at least one contour segment
		{
			pContour->Del_Part(iPart);
		}
		else if( !bEdge && SG_Get_Distance(pContour->Get_Point(0, iPart, true), pContour->Get_Point(0, iPart, false)) < sqrt(2.) * m_pGrid->Get_Cellsize() )
		{
			pContour->Add_Point(pContour->Get_Point_Z(0, iPart), iPart); // close contour line
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline int CGrid_To_Contour::Get_Contour_Vertex_First(int x, int y, bool bEdge)
{
	CSG_Grid &m_Flag = m_Flags[SG_OMP_Get_Thread_Num()];

	if( m_Flag.asInt(x, y) > 0 )
	{
		for(int i=8; i>0; i-=2) // we want to work counter-clockwise
		{
			int ix = CSG_Grid_System::Get_xTo(i, x);
			int iy = CSG_Grid_System::Get_yTo(i, y);

			if( m_Flag.is_InGrid(ix, iy) && m_Flag.asInt(ix, iy) < 0 )
			{
				if( bEdge )
				{
					ix = CSG_Grid_System::Get_xTo(i + 2, x);
					iy = CSG_Grid_System::Get_yTo(i + 2, y);

					if( !m_Flag.is_InGrid(ix, iy) )
					{
						return( i );
					}
				}
				else
				{
					return( i );
				}
			}
		}
	}

	return( -1 );
}

//---------------------------------------------------------
inline bool CGrid_To_Contour::Get_Contour_Vertex_Next(int &x, int &y, int &Dir)
{
	CSG_Grid &m_Flag = m_Flags[SG_OMP_Get_Thread_Num()];

	int xo = CSG_Grid_System::Get_xTo(Dir + 6, x);
	int yo = CSG_Grid_System::Get_yTo(Dir + 6, y);

	if( m_Flag.is_InGrid(xo, yo) )
	{
		if( m_Flag.asInt(xo, yo) < 0 )
		{
			if( m_Flag.asInt(x, y) > 0 )
			{
				Dir = (Dir + 6) % 8;

				return( true );
			}
		}
		else if( m_Flag.asInt(xo, yo) > 0 )
		{
			int xd = CSG_Grid_System::Get_xTo(Dir + 7, x);
			int yd = CSG_Grid_System::Get_yTo(Dir + 7, y);

			if( m_Flag.is_InGrid(xd, yd) && m_Flag.asInt(xd, yd) < 0 )
			{
				x = xo; y = yo;

				return( true );
			}
		}
	}

	int xd = CSG_Grid_System::Get_xTo(Dir + 7, x);
	int yd = CSG_Grid_System::Get_yTo(Dir + 7, y);

	if( m_Flag.is_InGrid(xd, yd) && m_Flag.asInt(xd, yd) > 0 )
	{
		x = xd; y = yd; Dir = (Dir + 2) % 8;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CGrid_To_Contour::Add_Contour_Vertex(CSG_Shape_Line *pContour, int iPart, double z, int x, int y, int Dir)
{
	CSG_Grid &m_Flag = m_Flags[SG_OMP_Get_Thread_Num()];

	if( m_Flag.asInt(x, y) > 0 )
	{
		int x1 = CSG_Grid_System::Get_xTo(Dir, x), y1 = CSG_Grid_System::Get_yTo(Dir, y);

		if( m_Flag.is_InGrid(x1, y1) )
		{
			double z0 = m_pGrid->asDouble(x, y), z1 = m_pGrid->asDouble(x1, y1);

			double d = (z0 - z) / (z0 - z1);

			CSG_Point_3D p(
				m_pGrid->Get_XMin() + m_pGrid->Get_Cellsize() * (x + d * (x1 - (double)x)),
				m_pGrid->Get_YMin() + m_pGrid->Get_Cellsize() * (y + d * (y1 - (double)y)), z
			);

			pContour->Add_Point(p, iPart);

			m_Flag.Add_Value(x, y, -1);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Edge_Segments(CSG_Shapes &Edges, CSG_Shapes *pContours)
{
	Process_Set_Text("%s...", _TL("edge segments"));

	m_Flag.Create(m_pGrid->Get_System(), SG_DATATYPE_Char); m_Flag.Set_NoData_Value(0);

	#pragma omp parallel for
	for(int y=0; y<m_pGrid->Get_NY(); y++) for(int x=0; x<m_pGrid->Get_NX(); x++)
	{
		m_Flag.Set_Value(x, y, m_pGrid->is_NoData(x, y) ? 0 : is_Edge(x, y) ? 1 : -1);
	}

	#pragma omp parallel for
	for(int y=0; y<m_pGrid->Get_NY(); y++) for(int x=0; x<m_pGrid->Get_NX(); x++)
	{
		if( m_Flag.asInt(x, y) > 0 )
		{
			int n = 0;

			for(int i=0; i<8; i++)
			{
				if( Get_Edge_Flag(x, y, i) < 0 )
				{
					n++;
				}
			}

			if( n == 0 )
			{
				m_Flag.Set_Value(x, y, 0.);
			}
			else if( (Get_Edge_Flag(x, y, 0) > 0 && Get_Edge_Flag(x, y, 4) > 0)
				&& ( (Get_Edge_Flag(x, y, 1) < 0 || Get_Edge_Flag(x, y, 2) < 0 || Get_Edge_Flag(x, y, 3) < 0)
				  && (Get_Edge_Flag(x, y, 5) < 0 || Get_Edge_Flag(x, y, 6) < 0 || Get_Edge_Flag(x, y, 7) < 0) ) )
			{
				m_Flag.Set_Value(x, y, 2.);
			}
			else if( (Get_Edge_Flag(x, y, 2) > 0 && Get_Edge_Flag(x, y, 6) > 0)
				&& ( (Get_Edge_Flag(x, y, 3) < 0 || Get_Edge_Flag(x, y, 4) < 0 || Get_Edge_Flag(x, y, 5) < 0)
				  && (Get_Edge_Flag(x, y, 7) < 0 || Get_Edge_Flag(x, y, 0) < 0 || Get_Edge_Flag(x, y, 1) < 0) ) )
			{
				m_Flag.Set_Value(x, y, 2.);
			}
		}
	}

	Edges.Create(SHAPE_TYPE_Line);
	Edges.Add_Field("A", SG_DATATYPE_Long);
	Edges.Add_Field("B", SG_DATATYPE_Long);

	for(int y=0; y<m_pGrid->Get_NY(); y++) for(int x=0; x<m_pGrid->Get_NX(); x++)
	{
		Add_Edge_Segment(Edges, x, y);
	}

	m_Flag.Destroy();

	//-----------------------------------------------------
	for(sLong i=0; i<pContours->Get_Count(); i++)
	{
		CSG_Shape *pContour = pContours->Get_Shape(i);

		for(int iPart=0; iPart<pContour->Get_Part_Count(); iPart++)
		{
			CSG_Point Point[2] = { pContour->Get_Point(0, iPart, true), pContour->Get_Point(0, iPart, false) };

			if( ((Add_Edge_Point(Edges, Point[0], pContour->Get_Index(), iPart) ? 1 : 0)
			+    (Add_Edge_Point(Edges, Point[1], pContour->Get_Index(), iPart) ? 1 : 0)) == 1 )
			{
				Message_Fmt("\nWarning: one end on edge, the other not (z=%f, line=%lld, part=%d)", pContour->asDouble(1), i, iPart);
			}
		}
	}

	//-----------------------------------------------------
	for(sLong i=Edges.Get_Count()-1; i>=0; i--)
	{
		CSG_Shape_Line &Edge = *Edges.Get_Shape(i)->asLine();

		if( !Edge.Get_Length() )
		{
			Edges.Del_Shape(i);
		}
		else if( Edge.asLong(0) > Edge.asLong(1) ) // store lower contour index in first attribute for easy collection...
		{
			Edge.Add_Value(0, -1);
		}
		else if( Edge.asLong(0) < 0 && Edge.asLong(1) < 0 ) // still fresh segment! no intersection found, either outer or inner ring!
		{
			double z = m_pGrid->Get_Value(Edge.Get_Point(0));

			for(sLong j=0; j<pContours->Get_Count(); j++)
			{
				if( z > pContours->Get_Shape(j)->asDouble(1) )
				{
					Edge.Set_Value(0, j);
				}
				else // if( z < pContours->Get_Shape(j)->asDouble(1) )
				{
					break;
				}
			}
		}
		else if( Edge.asLong(0) == Edge.asLong(1) ) // needs to determine 'going lower/higher' state in relation to contour elevation
		{
			CSG_Point p(Edge.Get_Point(0)), d(Edge.Get_Point(1)); d.x -= p.x; d.y -= p.y;

			int x = (int)floor((p.x - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize());
			int y = (int)floor((p.y - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize());

			if( (d.x < 0. && m_pGrid->asDouble(x + 1, y) > m_pGrid->asDouble(x, y))
			||  (d.x > 0. && m_pGrid->asDouble(x + 1, y) < m_pGrid->asDouble(x, y))
			||  (d.y < 0. && m_pGrid->asDouble(x, y + 1) > m_pGrid->asDouble(x, y))
			||  (d.y > 0. && m_pGrid->asDouble(x, y + 1) < m_pGrid->asDouble(x, y)) )
			{
				Edge.Add_Value(0, -1);
			}
		}
	}

	return( Edges.Get_Count() > 0 );
}

//---------------------------------------------------------
inline int CGrid_To_Contour::Get_Edge_Flag(int x, int y, int Dir)
{
	int ix = CSG_Grid_System::Get_xTo(Dir, x);
	int iy = CSG_Grid_System::Get_yTo(Dir, y);

	return( m_Flag.is_InGrid(ix, iy) ? m_Flag.asInt(ix, iy) : 0 );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Add_Edge_Segment(CSG_Shapes &Edges, int x, int y)
{
	if( m_Flag.asInt(x, y) < 1 )
	{
		return( false );
	}

	int Dir = -1;

	for(int i=0; Dir<0 && i<8; i+=2)
	{
		if( Get_Edge_Flag(x, y, i) > 0 && (Get_Edge_Flag(x, y, i + 1) < 0 || Get_Edge_Flag(x, y, i + 2) < 0) )
		{
			Dir = i;
		}
	}

	if( Dir < 0 )
	{
		m_Flag.Set_Value(x, y, 0.);

		return( false );
	}

	CSG_Shape &Edge = *Edges.Add_Shape(); Edge.Set_Value(0, -1); Edge.Set_Value(1, -1); // mark as fresh segment

	Edge.Add_Point(m_Flag.Get_System().Get_Grid_to_World(x, y));

	do
	{
		m_Flag.Add_Value(x, y, -1);

		int nextDir = -1;

		for(int i=Dir, j=0; nextDir<0 && j<8; i+=2, j+=2)
		{
			if( Get_Edge_Flag(x, y, i) > 0 && (Get_Edge_Flag(x, y, i + 1) < 0 || Get_Edge_Flag(x, y, i + 2) < 0) )
			{
				if( i != Dir )
				{
					Edge.Add_Point(m_Flag.Get_System().Get_Grid_to_World(x, y));
				}

				nextDir = i % 8;

				x += CSG_Grid_System::Get_xTo(i);
				y += CSG_Grid_System::Get_yTo(i);
			}
		}

		Dir = nextDir;
	}
	while( Dir >= 0 );

	if( Edge.Get_Point_Count() < 2 )
	{
		Edges.Del_Shape(&Edge);

		return( false );
	}

	if( !CSG_Point(Edge.Get_Point(0, 0, true)).is_Equal(Edge.Get_Point(0, 0, false)) )
	{
		Edge.Add_Point(Edge.Get_Point(0, 0, true));
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Add_Edge_Point(CSG_Shapes &Edges, const CSG_Point &Point, sLong Line, int Part)
{
	for(sLong i=0; i<Edges.Get_Count(); i++)
	{
		CSG_Shape_Line &Edge = *Edges.Get_Shape(i)->asLine();

		for(int iPart=0; iPart<Edge.Get_Part_Count(); iPart++)
		{
			CSG_Point A = Edge.Get_Point(0, iPart);

			for(int iPoint=1; iPoint<Edge.Get_Point_Count(iPart); iPoint++)
			{
				CSG_Point B = A; A = Edge.Get_Point(iPoint, iPart);

				if( SG_Is_Point_On_Line(Point, A, B, true) )
				{
					if( Edge.asInt(0) < 0 && Edge.asInt(1) < 0 ) // is fresh segment
					{
						int nPart = Edge.Get_Part_Count();

						Edge.Add_Point(Point, nPart);

						for(int jPoint=iPoint; jPoint<Edge.Get_Point_Count(iPart); jPoint++)
						{
							Edge.Add_Point(Edge.Get_Point(jPoint, iPart), nPart);
						}

						for(int jPoint=0; jPoint<iPoint; jPoint++)
						{
							Edge.Add_Point(Edge.Get_Point(jPoint, iPart), nPart);
						}

						Edge.Add_Point(Point, nPart);
						Edge.Del_Part(iPart);
						Edge.Set_Value(0, Line); Edge.Set_Value(1, Line);
					}
					else
					{
						CSG_Shape_Line &New = *Edges.Add_Shape()->asLine();
						New.Add_Point(Point);
						New.Set_Value(0, Line); New.Set_Value(1, Edge.asLong(1));

						for(; iPoint<Edge.Get_Point_Count(iPart); )
						{
							New.Add_Point(Edge.Get_Point(iPoint, iPart));

							Edge.Del_Point(iPoint, iPart);
						}

						Edge.Add_Point(Point, iPart);
						Edge.Set_Value(1, Line);
					}

					return( true );
				}
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Get_Polygons(CSG_Shape_Polygon &Polygon, CSG_Shapes &Edges, CSG_Shape_Line *pContour_Lo, CSG_Shape_Line *pContour_Hi)
{
	Polygon.Set_Value(0, 1 + Polygon.Get_Index());
	Polygon.Set_Value(1, pContour_Lo ? pContour_Lo->asDouble(1) : m_pGrid->Get_Min());
	Polygon.Set_Value(2, pContour_Hi ? pContour_Hi->asDouble(1) : m_pGrid->Get_Max());

	if( pContour_Hi )
	{
		if( !SG_OMP_Get_Thread_Num() )
		{
			Process_Set_Text("%s: < %s", _TL("Polygon"), SG_Get_String(pContour_Hi->asDouble(1), -10).c_str());
		}

		Polygon.Set_Value(3, "< " + SG_Get_String(pContour_Hi->asDouble(1), -10));
	}	
	else if( pContour_Lo )
	{
		if( !SG_OMP_Get_Thread_Num() )
		{
			Process_Set_Text("%s: > %s", _TL("Polygon"), SG_Get_String(pContour_Lo->asDouble(1), -10).c_str());
		}

		Polygon.Set_Value(3, "> " + SG_Get_String(pContour_Lo->asDouble(1), -10));
	}

	//-----------------------------------------------------
	CSG_Array_Pointer Segments;

	for(int i=0; pContour_Lo && i<pContour_Lo->Get_Part_Count(); i++)
	{
		Segments += pContour_Lo->Get_Part(i);
	}

	for(int i=0; pContour_Hi && i<pContour_Hi->Get_Part_Count(); i++)
	{
		Segments += pContour_Hi->Get_Part(i);
	}

	sLong Index = pContour_Lo ? pContour_Lo->Get_Index() : -1;

	for(sLong i=0; i<Edges.Get_Count(); i++)
	{
		CSG_Shape &Edge = *Edges.Get_Shape(i);

		if( Edge.asLong(0) == Index )
		{
			Segments += Edge.Get_Part(0);
		}
	}

	//-----------------------------------------------------
	while( Segments.Get_Size() )
	{
		CSG_Shape_Part *pSegment = (CSG_Shape_Part *)Segments[0]; Segments.Del(pSegment);

		int iPart = Polygon.Get_Part_Count(); Polygon.Add_Part(pSegment);

		CSG_Shape_Part *pPart = Polygon.Get_Part(iPart);

		while( Add_Polygon_Segment(Segments, pPart) );

		if( !Polygon.Get_Area(iPart) )
		{
			Polygon.Del_Part(iPart);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_To_Contour::Add_Polygon_Segment(CSG_Array_Pointer &Segments, CSG_Shape_Part *pPolygon)
{
	CSG_Point Point(pPolygon->Get_Point(0, false));

	for(sLong i=0; i<Segments.Get_Size(); i++)
	{
		CSG_Shape_Part *pSegment = (CSG_Shape_Part *)Segments[i]; bool bAscending;

		if( (Point == pSegment->Get_Point(0, bAscending =  true))
		||  (Point == pSegment->Get_Point(0, bAscending = false)) )
		{
			pPolygon->Add_Points(pSegment, bAscending); Segments.Del(i);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_To_Contour::Split_Line_Parts(CSG_Shapes *pLines)
{
	Process_Set_Text("%s...", _TL("Split Line Parts"));

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
	Process_Set_Text("%s...", _TL("Split Polygon Parts"));

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
