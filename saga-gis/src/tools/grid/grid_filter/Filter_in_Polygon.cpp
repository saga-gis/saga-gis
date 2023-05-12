
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Filter.cpp                       //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Filter_in_Polygon.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_in_Polygon::CFilter_in_Polygon(void)
{
	Set_Name		(_TL("Simple Filter (Restricted to Polygons)"));

	Set_Author		("Johan Van de Wauw (c) 2015");

	Set_Description	(_TW(
		"Simple standard filters for grids, evaluation within polygons."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("",
		"SHAPES"	,_TL("Polygons"),
		_TL("The filter will only operate on cells that belong to the same polygon or to none if skip outside option is off."),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Filter"),
		_TL("Choose the filter method."),
		CSG_String::Format("%s|%s|%s",
			_TL("Smooth"),
			_TL("Sharpen"),
			_TL("Edge")
		), 0
	);

	Parameters.Add_Bool("",
		"SKIP_OUTSIDE",_TL("Skip Outside Cells"),
		_TL("Process only cells that are covered by a polygon."),
		false
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_in_Polygon::On_Execute(void)
{
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("Kernel initialization failed!"));

		return( false );
	}

	int Method = Parameters("METHOD")->asInt();

	bool bSkip = Parameters("SKIP_OUTSIDE")->asBool();

	//-----------------------------------------------------
	CSG_Grid Input; m_pInput = Parameters("INPUT")->asGrid();

	CSG_Grid *pResult = Parameters("RESULT")->asGrid();

	if( !pResult || pResult == m_pInput )
	{
		Input.Create(*m_pInput); pResult = m_pInput; m_pInput = &Input;
	}
	else
	{
		if( Method != 2 )	// not edge...
		{
			DataObject_Set_Parameters(pResult, m_pInput);
		}

		pResult->Fmt_Name("%s [%s]", m_pInput->Get_Name(), Method == 0 ? _TL("Smoothed") : Method == 1 ? _TL("Sharpened") : _TL("Edge"));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing Fields"));

	CSG_Shapes *pPolygons = Parameters("SHAPES")->asShapes();

	m_Polygons.Create(Get_System(), pPolygons->Get_Count() < pow(2., 16.) - 1. ? SG_DATATYPE_Word : SG_DATATYPE_DWord);
	m_Polygons.Set_NoData_Value((double)pPolygons->Get_Count());
	m_Polygons.Assign_NoData();

	for(sLong i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape_Polygon &Polygon = *pPolygons->Get_Shape(i)->asPolygon();

		int xMin = Get_System().Get_xWorld_to_Grid(Polygon.Get_Extent().Get_XMin()) - 1; if( xMin <  0        ) xMin = 0;
		int xMax = Get_System().Get_xWorld_to_Grid(Polygon.Get_Extent().Get_XMax()) + 1; if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
		int yMin = Get_System().Get_yWorld_to_Grid(Polygon.Get_Extent().Get_YMin()) - 1; if( yMin <  0        ) yMin = 0;
		int yMax = Get_System().Get_yWorld_to_Grid(Polygon.Get_Extent().Get_YMax()) + 1; if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

		for(int y=yMin; y<=yMax; y++)
		{
			for(int x=xMin; x<=xMax; x++)
			{
				if( m_pInput->is_InGrid(x, y) && Polygon.Contains(Get_System().Get_Grid_to_World(x, y)) )
				{
					m_Polygons.Set_Value(x, y, (double)i);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double Mean;

			if( bSkip && m_Polygons.is_NoData(x, y) )
			{
				pResult->Set_Value(x, y, Method != 2 ? m_pInput->asDouble(x, y) : 0.);
			}
			else if( Get_Mean(x, y, Mean) == false )
			{
				pResult->Set_NoData(x, y);
			}
			else switch( Method )
			{
			default:	// Smooth...
				pResult->Set_Value(x, y, Mean);
				break;

			case  1:	// Sharpen...
				pResult->Set_Value(x, y, m_pInput->asDouble(x, y) + (m_pInput->asDouble(x, y) - Mean));
				break;

			case  2:	// Edge...
				pResult->Set_Value(x, y, m_pInput->asDouble(x, y) - Mean);
				break;
			}
		}
	}

	m_Polygons.Destroy();

	m_Kernel.Destroy();

	//-------------------------------------------------
	if( pResult == Parameters("INPUT")->asGrid() )
	{
		DataObject_Update(pResult);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_in_Polygon::Get_Mean(int x, int y, double &Value)
{
	CSG_Simple_Statistics s;

	if( m_pInput->is_InGrid(x, y) )
	{
		int	Polygon = m_Polygons.asInt(x, y);

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int ix = m_Kernel.Get_X(i, x);
			int iy = m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) && Polygon == m_Polygons.asInt(ix, iy) )
			{
				s += m_pInput->asDouble(ix, iy);
			}
		}
	}

	if( s.Get_Count() > 0 )
	{
		Value = s.Get_Mean();

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
