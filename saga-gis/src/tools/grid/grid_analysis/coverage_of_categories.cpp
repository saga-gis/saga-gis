
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

	Parameters.Add_Table("",
		"LUT"		, _TL("Classification"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("LUT",
		"LUT_VAL"	, _TL("Value"),
		_TL("The class value or - in combination with value 2 - the minimum/maximum value specifying a value range."),
		false
	);

	Parameters.Add_Table_Field("LUT",
		"LUT_MAX"	, _TL("Maximum Value"),
		_TL("Use this option to specify a value range equal or greater than previous value and less than this (maximum) value."),
		true
	);

	Parameters.Add_Table_Field("LUT",
		"LUT_NAME"	, _TL("Class name"),
		_TL("Optional, a class name used for the naming of the target coverage rasters."),
		true
	);

	Parameters.Add_Bool("",
		"NO_DATA"	, _TL("Mark No Coverage as No-Data"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"DATADEPTH"	, _TL("Data Depth"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("1-byte"),
			_TL("2-byte"),
			_TL("4-byte"),
			_TL("8-byte")
		), 1
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
	if( pParameter->Cmp_Identifier("LUT") )
	{
		pParameter->Set_Children_Enabled(pParameter->asTable() != NULL);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCoverage_of_Categories::On_Execute(void)
{
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
		m_Classes.Destroy();

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
	m_Classes.Destroy();

	m_Classes.Add_Field("NAM", SG_DATATYPE_String);
	m_Classes.Add_Field("VAL", m_pClasses->Get_Type());

	//-----------------------------------------------------
	if( Parameters("LUT")->asTable() )
	{
		CSG_Table	&Classes	= *Parameters("LUT")->asTable();

		int	fVal	= Parameters("LUT_VAL" )->asInt();
		int	fMax	= Parameters("LUT_MAX" )->asInt();
		int	fNam	= Parameters("LUT_NAME")->asInt();

		if( fMax >= 0 )
		{
			m_Classes.Add_Field("MAX", m_pClasses->Get_Type());
		}

		m_Classes.Set_Count(Classes.Get_Count());

		for(int iClass=0; iClass<Classes.Get_Count(); iClass++)
		{
			m_Classes[iClass].Set_Value(1, Classes[iClass].asDouble(fVal));

			if( fMax >= 0 )
			{
				m_Classes[iClass].Set_Value(2, Classes[iClass].asDouble(fMax));
			}

			if( fNam >= 0 )
			{
				m_Classes[iClass].Set_Value(0, Classes[iClass].asString(fNam));
			}
			else
			{
				m_Classes[iClass].Set_Value(0, SG_Get_String(Classes[iClass].asDouble(fVal), -6));
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Unique_Number_Statistics	Classes;

		for(sLong iCell=0; iCell<Get_NCells() && Set_Progress_NCells(iCell); iCell++)
		{
			if( !m_pClasses->is_NoData(iCell) )
			{
				Classes	+= m_pClasses->asDouble(iCell);
			}
		}

		//-------------------------------------------------
		m_Classes.Set_Count(Classes.Get_Count());

		for(int iClass=0; iClass<Classes.Get_Count(); iClass++)
		{
			m_Classes[iClass].Set_Value(0, SG_Get_String(Classes.Get_Value(iClass), -6));
			m_Classes[iClass].Set_Value(1,               Classes.Get_Value(iClass)     );
		}

		m_Classes.Set_Index(1, TABLE_INDEX_Ascending);
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("Number of Classes"), m_Classes.Get_Count());

	if( m_Classes.Get_Count() < 1 )
	{
		Error_Set(_TL("No valid cells found"));

		return( false );
	}

	if( m_Classes.Get_Count() > 32 && !Message_Dlg_Confirm(CSG_String::Format("%s: %s [%d]!", _TL("Warning"), _TL("There are many unique values"), m_Classes.Get_Count()), _TL("Do you really want to proceed?")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Data_Type	Type;	double	Scaling;

	switch( Parameters("DATADEPTH")->asInt() )
	{
	case  0: Type = SG_DATATYPE_Byte  ; Scaling = 1. /   255.; break;
	default: Type = SG_DATATYPE_Word  ; Scaling = 1. / 65535.; break;
	case  2: Type = SG_DATATYPE_Float ; Scaling = 1.         ; break;
	case  3: Type = SG_DATATYPE_Double; Scaling = 1.         ; break;
	}

	CSG_Parameter_Grid_List	*pCoverages	= Parameters("COVERAGES")->asGridList();

	pCoverages->Del_Items();

	for(int iClass=0; iClass<m_Classes.Get_Count(); iClass++)
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(System, Type);

		if( !pGrid )
		{
			Error_Set(_TL("Failed to allocate memory for coverage grid"));

			return( false );
		}

		pGrid->Fmt_Name("%s [%s]", m_pClasses->Get_Name(), m_Classes[iClass].asString(0));
		pGrid->Set_NoData_Value(Parameters("NO_DATA")->asBool() ? 0. : -1.);
		pGrid->Set_Scaling(Scaling);

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
	if( m_pClasses->is_InGrid(x, y) )
	{
		double	Value	= m_pClasses->asDouble(x, y);
		
		if( m_Classes.Get_Field_Count() > 2 )
		{
			return( Value >= m_Classes[iClass].asDouble(1)
				&&  Value <  m_Classes[iClass].asDouble(2)
			);
		}

		return( Value == m_Classes[iClass].asDouble(1) );
	}

	return( false );
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
