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
#include "Filter_in_Polygon.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_in_Polygon::CFilter_in_Polygon(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Simple Filter (Restricted to Polygons)"));

	Set_Author		("Johan Van de Wauw, 2015");

	Set_Description	(_TW(
		"Simple standard filters for grids, evaluation within polygons."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(NULL,
		"SHAPES"	,_TL("pPolygons"),
		_TL("pPolygons: the simple filter will only operate on grid cells which fall in the same shape"),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Filter"),
		_TL("Choose the shape of the filter kernel."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Smooth"),
			_TL("Sharpen"),
			_TL("Edge")
		), 0
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_in_Polygon::On_After_Execution(void)
{
	if( Parameters("RESULT")->asGrid() == Parameters("INPUT")->asGrid() )
	{
		Parameters("RESULT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_in_Polygon::On_Execute(void)
{
	//-----------------------------------------------------
	int	Method	= Parameters("METHOD")->asInt();

	m_pInput	= Parameters("INPUT")->asGrid();

	CSG_Grid	Result, *pResult	= Parameters("RESULT")->asGrid();

	//-----------------------------------------------------
	if( !pResult || pResult == m_pInput )
	{
		pResult	= &Result;
		
		pResult->Create(m_pInput);
	}
	else
	{
		pResult->Set_Name(CSG_String::Format("%s [%s]", m_pInput->Get_Name(), _TL("Filter")));

		pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());

		if( Method != 2 )	// Edge...
		{
			DataObject_Set_Parameters(pResult, m_pInput);
		}
	}

	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Initializing Fields"));

	CSG_Shapes	*pPolygons	= Parameters("SHAPES")->asShapes();

	int m_nFields	= pPolygons->Get_Count();

	m_Fields.Create(*Get_System(), m_nFields < pow(2.0, 16.0) - 1.0 ? SG_DATATYPE_Word : SG_DATATYPE_DWord);
	m_Fields.Set_NoData_Value(m_nFields);
	m_Fields.Assign_NoData();

	for(int iField=0; iField<pPolygons->Get_Count() && Set_Progress(iField, pPolygons->Get_Count()); iField++)
	{
		CSG_Shape_Polygon	*pField	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iField);

		int	xMin	= Get_System()->Get_xWorld_to_Grid(pField->Get_Extent().Get_XMin()) - 1; if( xMin <  0        ) xMin = 0;
		int	xMax	= Get_System()->Get_xWorld_to_Grid(pField->Get_Extent().Get_XMax()) + 1; if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
		int	yMin	= Get_System()->Get_yWorld_to_Grid(pField->Get_Extent().Get_YMin()) - 1; if( yMin <  0        ) yMin = 0;
		int	yMax	= Get_System()->Get_yWorld_to_Grid(pField->Get_Extent().Get_YMax()) + 1; if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

		for(int y=yMin; y<=yMax; y++)
		{
			for(int x=xMin; x<=xMax; x++)
			{
				if( m_pInput->is_InGrid(x, y) && pField->Contains(Get_System()->Get_Grid_to_World(x, y)) )
				{
					m_Fields.Set_Value(x, y, iField);
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Mean;

			if( Get_Mean(x, y, Mean) )
			{
				switch( Method )
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
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-------------------------------------------------
	if( pResult == &Result )
	{
		CSG_MetaData	History	= m_pInput->Get_History();

		m_pInput->Assign(pResult);
		m_pInput->Get_History() = History;

		DataObject_Update(m_pInput);

		Parameters("RESULT")->Set_Value(m_pInput);
	}

	m_Fields.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_in_Polygon::Get_Mean(int x, int y, double &Value)
{
	CSG_Simple_Statistics	s;

	if( m_pInput->is_InGrid(x, y) )
	{
		int	Field = m_Fields.asInt(x, y);

		for(int i=0; i<m_Kernel.Get_Count(); i++)
		{
			int	ix	= m_Kernel.Get_X(i, x);
			int	iy	= m_Kernel.Get_Y(i, y);

			if( m_pInput->is_InGrid(ix, iy) && Field == m_Fields.asInt(ix, iy) )
			{
				s	+= m_pInput->asDouble(ix, iy);
			}
		}
	}

	if( s.Get_Count() > 0 )
	{
		Value	= s.Get_Mean();

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
