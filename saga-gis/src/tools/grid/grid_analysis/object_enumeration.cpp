
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
//                 object_enumeration.cpp                //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
#include "object_enumeration.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CObject_Enumeration::CObject_Enumeration(void)
{
	Set_Name		(_TL("Object Enumeration"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"The \'Object Enumeration\' tool to identifies objects as connected cells "
		"taking all no-data cells (or those cells having a specified value) "
		"as potential boundary cells surrounding each object. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID"          , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OBJECTS"       , _TL("Objects"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);

	Parameters.Add_Table("",
		"SUMMARY"       , _TL("Summary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"EXTENTS"       , _TL("Extents"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice("",
		"NEIGHBOURHOOD" , _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Neumann"),
			_TL("Moore")
		), 0
	);

	Parameters.Add_Choice("",
		"BOUNDARY_CELLS", _TL("Boundary Cells"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("no data"),
			_TL("value")
		), 0
	);

	Parameters.Add_Double("BOUNDARY_CELLS",
		"BOUNDARY_VALUE", _TL("Value"),
		_TL(""),
		0.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CObject_Enumeration::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("BOUNDARY_CELLS") )
	{
		pParameters->Set_Enabled("BOUNDARY_VALUE", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CObject_Enumeration::On_Execute(void)
{
	CSG_Grid Grid; m_pGrid = Parameters("GRID")->asGrid();

	if( Parameters("BOUNDARY_CELLS")->asInt() == 1 )
	{
		Grid.Create(Get_System(), SG_DATATYPE_Char);
		Grid.Set_Name(m_pGrid->Get_Name());
		Grid.Set_NoData_Value(0);

		double Value = Parameters("BOUNDARY_VALUE")->asDouble();

		#pragma omp parallel for
		for(int i=0; i<Get_NCells(); i++)
		{
			Grid.Set_Value(i, m_pGrid->asDouble(i) == Value ? 0 : 1);
		}

		m_pGrid = &Grid;
	}

	//-----------------------------------------------------
	m_pObjects = Parameters("OBJECTS")->asGrid();

	m_pObjects->Fmt_Name("%s [%s]", m_pGrid->Get_Name(), _TL("Objects"));
	m_pObjects->Assign(0.);
	m_pObjects->Set_NoData_Value(0.);

	CSG_Table &Summary = *Parameters("SUMMARY")->asTable(); Summary.Destroy();

	Summary.Fmt_Name("%s [%s]", m_pGrid->Get_Name(), _TL("Objects"));
	Summary.Add_Field("ID"     , SG_DATATYPE_Int   );
	Summary.Add_Field("CELLS"  , SG_DATATYPE_Int   );
	Summary.Add_Field("COL_MIN", SG_DATATYPE_Int   );
	Summary.Add_Field("COL_MAX", SG_DATATYPE_Int   );
	Summary.Add_Field("ROW_MIN", SG_DATATYPE_Int   );
	Summary.Add_Field("ROW_MAX", SG_DATATYPE_Int   );
	Summary.Add_Field("WEST"   , SG_DATATYPE_Double);
	Summary.Add_Field("EAST"   , SG_DATATYPE_Double);
	Summary.Add_Field("SOUTH"  , SG_DATATYPE_Double);
	Summary.Add_Field("NORTH"  , SG_DATATYPE_Double);

	CSG_Shapes *pExtents = Parameters("EXTENTS")->asShapes();

	if( pExtents )
	{
		pExtents->Create(SHAPE_TYPE_Polygon, NULL, &Summary);
		pExtents->Fmt_Name("%s [%s]", m_pGrid->Get_Name(), _TL("Object Extents"));
	}

	//-----------------------------------------------------
	int Neighbourhood = Parameters("NEIGHBOURHOOD")->asInt() == 0 ? 2 : 1;

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( !m_pObjects->asInt(x, y) && !m_pGrid->is_NoData(x, y) )
		{
			int Cells = 0; CSG_Rect_Int Extent;

			if( Get_Object(x, y, (int)(Summary.Get_Count() + 1), Neighbourhood, Cells, Extent) )
			{
				CSG_Table_Record &Info = *Summary.Add_Record();

				Info.Set_Value(0, Summary.Get_Count());
				Info.Set_Value(1, Cells);
				Info.Set_Value(2, Extent.xMin);
				Info.Set_Value(3, Extent.xMax);
				Info.Set_Value(4, Extent.yMin);
				Info.Set_Value(5, Extent.yMax);
				Info.Set_Value(6, Get_XMin() + Get_Cellsize() * (Extent.xMin - 0.5));
				Info.Set_Value(7, Get_XMin() + Get_Cellsize() * (Extent.xMax + 0.5));
				Info.Set_Value(8, Get_YMin() + Get_Cellsize() * (Extent.yMin - 0.5));
				Info.Set_Value(9, Get_YMin() + Get_Cellsize() * (Extent.yMax + 0.5));

				if( pExtents )
				{
					CSG_Shape *pExtent = pExtents->Add_Shape(&Info);

					pExtent->Add_Point(Info.asDouble(6), Info.asDouble(8));
					pExtent->Add_Point(Info.asDouble(6), Info.asDouble(9));
					pExtent->Add_Point(Info.asDouble(7), Info.asDouble(9));
					pExtent->Add_Point(Info.asDouble(7), Info.asDouble(8));
				}
			}
		}
	}

	//-----------------------------------------------------
	if( Summary.Get_Count() < 1 )
	{
		Error_Fmt(_TL("no area surrounded by boundary cells has been detected!"));

		return( false );
	}

	Message_Fmt("\n%s: %lld", _TL("Number of detected objects"), Summary.Get_Count());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CObject_Enumeration::Get_Object(int x, int y, int id, int Neighbourhood, int &Cells, CSG_Rect_Int &Extent)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		m_pObjects->Set_Value(x, y, id); Cells = 1; Extent.Assign(x, y, x, y);

		CSG_Grid_Stack Stack; Stack.Push(x, y);

		while( Stack.Get_Size() > 0 && Process_Get_Okay() )
		{
			Stack.Pop(x, y);

			for(int i=0; i<8; i+=Neighbourhood)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if(	m_pGrid->is_InGrid(ix, iy) && !m_pObjects->asInt(ix, iy) )
				{
					m_pObjects->Set_Value(ix, iy, id); Cells++; Extent.Union(ix, iy);

					Stack.Push(ix, iy);
				}
			}
		}

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
