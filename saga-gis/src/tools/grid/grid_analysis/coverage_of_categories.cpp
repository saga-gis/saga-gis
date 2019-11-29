
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               coverage_of_categories.cpp              //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "coverage_of_categories.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCoverage_of_Categories::CCoverage_of_Categories(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Coverage of Categories"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"The Coverage of Categories tool calculates for each category of "
		"the categories input grid the percentage it covers in each cell "
		"of the target grid system. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"CLASSES"	, _TL("Categories"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"COVERAGES"	, _TL("Coverages"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CCoverage_of_Categories::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCoverage_of_Categories::On_Execute(void)
{
	//-----------------------------------------------------
	m_pClasses	= Parameters("CLASSES")->asGrid();

	CSG_Grid_System	System	= m_Grid_Target.Get_System();

	if( !System.Get_Extent().Intersects(m_pClasses->Get_Extent()) )
	{
		Error_Set(_TL("no overlap of grid extents"));

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("initializing"));

	if( !Initialize(System) )
	{
		return( false );
	}

	//---------------------------------------------------------
	CSG_Parameter_Grid_List	*pCoverages	= Parameters("COVERAGES")->asGridList();

	double	dSize	= 0.5 * System.Get_Cellsize() / Get_Cellsize();

	//-----------------------------------------------------
	Process_Set_Text(_TL("processing"));

	for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
	{
		double	yy	= (System.Get_yGrid_to_World(y) - Get_YMin()) / Get_Cellsize();

		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<System.Get_NX(); x++)
		{
			double	xx	= (System.Get_xGrid_to_World(x) - Get_XMin()) / Get_Cellsize();

			CSG_Rect	Cell(xx - dSize, yy - dSize, xx + dSize, yy + dSize);

			for(int iClass=0; iClass<pCoverages->Get_Grid_Count(); iClass++)
			{
				pCoverages->Get_Grid(iClass)->Set_Value(x, y, Get_Coverage(Cell, iClass));
			}
		}
	}

	//-------------------------------------------------
	m_Classes.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCoverage_of_Categories::Initialize(const CSG_Grid_System &System)
{
	CSG_Unique_Number_Statistics	Classes;

	for(sLong iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
	{
		if( !m_pClasses->is_NoData(iCell) )
		{
			Classes	+= m_pClasses->asDouble(iCell);
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("Number of Classes"), Classes.Get_Count());

	if( Classes.Get_Count() < 1 )
	{
		Error_Set(_TL("No valid cells found"));

		return( false );
	}

	if( Classes.Get_Count() > 32 && !Message_Dlg_Confirm(CSG_String::Format("%s: %s [%d]!", _TL("Warning"), _TL("There are many unique values"), Classes.Get_Count()), _TL("Do you really want to proceed?")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_Classes.Destroy();
	m_Classes.Add_Field("VALUE", m_pClasses->Get_Type());
	m_Classes.Set_Count(Classes.Get_Count());

	for(int iClass=0; iClass<Classes.Get_Count(); iClass++)
	{
		m_Classes[iClass].Set_Value(0, Classes.Get_Value(iClass));
	}

	m_Classes.Set_Index(0, TABLE_INDEX_Ascending);

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pCoverages	= Parameters("COVERAGES")->asGridList();

	pCoverages->Del_Items();

	for(int iClass=0; iClass<m_Classes.Get_Count(); iClass++)
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(System);

		if( !pGrid )
		{
			Error_Set(_TL("Failed to allocate memory for coverage grid"));

			return( false );
		}

		pGrid->Fmt_Name("%s [%s]", m_pClasses->Get_Name(), SG_Get_String(m_Classes[iClass].asDouble(0), -6).c_str());
		pGrid->Set_NoData_Value(0.);

		pCoverages->Add_Item(pGrid);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CCoverage_of_Categories::Cmp_Class(int x, int y, int iClass)
{
	return( m_pClasses->is_InGrid(x, y) && m_pClasses->asDouble(x, y) == m_Classes[iClass].asDouble(0) );
}

//---------------------------------------------------------
inline double CCoverage_of_Categories::Get_Coverage(const CSG_Rect &Cell, int x, int y)
{
	CSG_Rect	c(x - 0.5, y - 0.5, x + 0.5, y + 0.5);

	return( c.Intersect(Cell) ? c.Get_Area() : 0. );
}

//---------------------------------------------------------
double CCoverage_of_Categories::Get_Coverage(const CSG_Rect &Cell, int iClass)
{
	double	Coverage	= 0.;

	for(int y=(int)Cell.Get_YMin()-1; y<=(int)Cell.Get_YMax()+1; y++)
	{
		for(int x=(int)Cell.Get_XMin()-1; x<=(int)Cell.Get_XMax()+1; x++)
		{
			if( Cmp_Class(x, y, iClass) )
			{
				Coverage	+= Get_Coverage(Cell, x, y);
			}
		}
	}

	return( Coverage / Cell.Get_Area() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
