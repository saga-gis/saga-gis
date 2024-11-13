
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Gaps_Spline_Fill.cpp               //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Gaps_Spline_Fill.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Gaps_Spline_Fill::CGrid_Gaps_Spline_Fill(void)
{
	Set_Name		(_TL("Close Gaps with Spline"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(
		"", "GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "MASK"			, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Int(
		"", "MAXGAPCELLS"	, _TL("Only Process Gaps with Less Cells"),
		_TL("ignored if set to zero"),
		0, 0, true
	);

	Parameters.Add_Grid(
		"", "CLOSED"		, _TL("Closed Gaps Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Int(
		"", "MAXPOINTS"		, _TL("Maximum Points"),
		_TL(""),
		1000, 2, true
	);

	Parameters.Add_Int(
		"", "LOCALPOINTS"	, _TL("Number of Points for Local Interpolation"),
		_TL(""),
		20, 2, true
	);

	Parameters.Add_Bool(
		"", "EXTENDED"		, _TL("Extended Neighbourhood"),
		_TL(""),
		false
	);

	Parameters.Add_Choice(
		"", "NEIGHBOURS"	, _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Neumann"),
			_TL("Moore")
		), 0
	);

	Parameters.Add_Int(
		"", "RADIUS"		, _TL("Radius (Cells)"),
		_TL(""),
		0, 0, true
	);

	Parameters.Add_Double(
		"", "RELAXATION"	, _TL("Relaxation"),
		_TL(""),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Gaps_Spline_Fill::On_Execute(void)
{
	m_pGrid         = Parameters("CLOSED"     )->asGrid();
	m_pMask         = Parameters("MASK"       )->asGrid();
	m_nGapCells_Max = Parameters("MAXGAPCELLS")->asInt();
	m_nPoints_Max   = Parameters("MAXPOINTS"  )->asInt();
	m_nPoints_Local = Parameters("LOCALPOINTS")->asInt();
	m_bExtended     = Parameters("EXTENDED"   )->asBool();
	m_Neighbours    = Parameters("NEIGHBOURS" )->asInt() == 0 ? 2 : 1;
	m_Radius        = Parameters("RADIUS"     )->asDouble();
	m_Relaxation    = Parameters("RELAXATION" )->asDouble();

	if( m_pGrid == NULL )
	{
		m_pGrid	= Parameters("GRID")->asGrid();

		Parameters("CLOSED")->Set_Value(m_pGrid);
	}
	else if( m_pGrid != Parameters("GRID")->asGrid() )
	{
		m_pGrid->Assign(Parameters("GRID")->asGrid());

		m_pGrid->Fmt_Name("%s [%s]", Parameters("GRID")->asGrid()->Get_Name(), _TL("no gaps"));
	}

	if( m_nGapCells_Max == 0 )
	{
		m_nGapCells_Max	= Get_NCells();
	}

	if( m_nPoints_Local > m_nPoints_Max )
	{
		m_nPoints_Local	= m_nPoints_Max;
	}

	//-----------------------------------------------------
	m_Gaps.Create(Get_System(), SG_DATATYPE_Int);
	m_Gaps.Assign(0.);
	m_nGaps	= 0;

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( is_Gap(x, y) && m_Gaps.asInt(x, y) == 0 )
			{
				Close_Gap(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Stack   .Clear();
	m_GapCells.Clear();
	m_Gaps    .Destroy();
	m_Spline  .Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CGrid_Gaps_Spline_Fill::is_Gap(int x, int y)
{
	return( (!m_pMask || !m_pMask->is_NoData(x, y)) && m_pGrid->is_NoData(x, y) );
}

//---------------------------------------------------------
inline void CGrid_Gaps_Spline_Fill::Push(int x, int y)
{
	if( m_iStack >= m_Stack.Get_Count() )
	{
		m_Stack.Set_Count(m_Stack.Get_Count() + 1024);
	}

	m_Stack[m_iStack].x = x;
	m_Stack[m_iStack].y = y;

	m_iStack++;
}

//---------------------------------------------------------
inline void CGrid_Gaps_Spline_Fill::Pop(int &x, int &y)
{
	if( m_iStack > 0 )
	{
		m_iStack--;

		x = m_Stack[m_iStack].x;
		y = m_Stack[m_iStack].y;
	}
}

//---------------------------------------------------------
void CGrid_Gaps_Spline_Fill::Set_Gap_Cell(int x, int y)
{
	if( is_InGrid(x, y) && (!m_pMask || !m_pMask->is_NoData(x, y)) && m_Gaps.asInt(x, y) != m_nGaps )
	{
		m_Gaps.Set_Value(x, y, m_nGaps);

		if( !is_Gap(x, y) )
		{
			m_Spline.Add_Point(x, y, m_pGrid->asDouble(x, y));

			for(int i=0; m_bExtended && i<8; i+=m_Neighbours)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if( m_pGrid->is_InGrid(ix, iy) && m_Gaps.asInt(ix, iy) != m_nGaps )
				{
					m_Gaps.Set_Value(ix, iy, m_nGaps);

					m_Spline.Add_Point(ix, iy, m_pGrid->asDouble(ix, iy));
				}
			}
		}
		else
		{
			if( m_nGapCells >= m_GapCells.Get_Count() )
			{
				m_GapCells.Set_Count(m_GapCells.Get_Count() + 1024);
			}

			m_GapCells[m_nGapCells].x = x;
			m_GapCells[m_nGapCells].y = y;

			m_nGapCells++;

			Push(x, y);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Gaps_Spline_Fill::Close_Gap(int x, int y)
{
	m_nGaps++; m_nGapCells = m_iStack = 0;

	m_Spline.Destroy();

	Set_Gap_Cell(x, y);

	while( m_iStack > 0 && m_nGapCells <= m_nGapCells_Max && Process_Get_Okay() )
	{
		Pop(x, y);

		for(int i=0; i<8; i+=m_Neighbours)
		{
			Set_Gap_Cell(Get_xTo(i, x), Get_yTo(i, y));
		}
	}

	//-----------------------------------------------------
	if( m_nGapCells <= m_nGapCells_Max )
	{
		if( m_nPoints_Max == 0 || m_Spline.Get_Point_Count() <= m_nPoints_Max )
		{
			if( m_Spline.Create(m_Relaxation, true) )
			{
				for(sLong i=0; i<m_nGapCells; i++)
				{
					TSG_Point_Int p = m_GapCells[i];

					m_pGrid->Set_Value(p.x, p.y, m_Spline.Get_Value(p.x, p.y));
				}
			}
		}
		else
		{
			Close_Gap();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Gaps_Spline_Fill::Close_Gap(void)
{
	CSG_Shapes Points(SHAPE_TYPE_Point, NULL, NULL, SG_VERTEX_TYPE_XYZ);

	for(int i=0; i<m_Spline.Get_Point_Count(); i++)
	{
		Points.Add_Shape()->Add_Point(m_Spline.Get_Points()[i]);
	}

	CSG_KDTree_2D Search(&Points);

	for(sLong i=0; i<m_nGapCells && Process_Get_Okay(); i++)
	{
		TSG_Point_Int p = m_GapCells[i];

		m_Spline.Destroy();

		Search.Get_Nearest_Points(p.x, p.y, m_nPoints_Local, m_Radius);

		for(size_t j=0; j<Search.Get_Match_Count(); j++)
		{
			CSG_Shape &Point = *Points.Get_Shape(Search.Get_Match_Index(j));

			m_Spline.Add_Point(Point.Get_Point(), Point.Get_Z());
		}

		if( m_Spline.Create(m_Relaxation, true) )
		{
			m_pGrid->Set_Value(p.x, p.y, m_Spline.Get_Value(p.x, p.y));
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
