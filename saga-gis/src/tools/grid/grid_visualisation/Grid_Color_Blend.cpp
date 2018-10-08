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
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Color_Blend.cpp                  //
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
#include "Grid_Color_Blend.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Blend::CGrid_Color_Blend(void)
{
	Set_Name		(_TL("Grid Animation"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description(_TW(
		"Creates an animation based on the values of selected grids. "
		"Previously known as 'Color Blending'."
	));

	Parameters.Add_Grid_List("",
		"GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath("",
		"FILE"	, _TL("Save Images"),
		_TL(""),
		CSG_String::Format("%s|*.png|%s|*.jpg;*.jif;*.jpeg|%s|*.tif;*.tiff|%s|*.bmp|%s|*.pcx",
			_TL("Portable Network Graphics (*.png)"),
			_TL("JPEG - JFIF Compliant (*.jpg, *.jif, *.jpeg)"),
			_TL("Tagged Image File Format (*.tif, *.tiff)"),
			_TL("Windows or OS/2 Bitmap (*.bmp)"),
			_TL("Zsoft Paintbrush (*.pcx)")
		), NULL, true
	);

	Parameters.Add_Colors("",
		"COLORS"	, _TL("Colours"),
		_TL("")
	);

	Parameters.Add_Int("",
		"NSTEPS"	, _TL("Interpolation Steps"),
		_TL(""),
		0, 0, true
	);

	Parameters.Add_Bool("",
		"PROGRESS"	, _TL("Progress Bar"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"LOOP"	, _TL("Loop"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("do not loop"),
			_TL("loop to first grid"),
			_TL("loop")
		), 1
	);

	Parameters.Add_Choice("",
		"RANGE"		, _TL("Histogram Stretch"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("each grid's range"),
			_TL("each grid's standard deviation"),
			_TL("overall range"),
			_TL("overall standard deviation"),
			_TL("user defined")
		), 0
	);

	Parameters.Add_Double("RANGE",
		"RANGE_PERCENT"	, _TL("Percent Stretch"),
		_TL(""),
		2.0, 0.0, true, 50.0, true
	);

	Parameters.Add_Double("RANGE",
		"RANGE_STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		2.0, 0.0, true
	);

	Parameters.Add_Bool("RANGE_STDDEV",
		"RANGE_KEEP"	, _TL("Keep in Range"),
		_TL(""),
		true
	);

	Parameters.Add_Range("RANGE",
		"RANGE_USER"	, _TL("Range"),
		_TL(""),
		2.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Color_Blend::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRIDS") && pParameter->asGridList()->Get_Grid_Count() > 0 )
	{
		(*pParameters)("RANGE_USER")->asRange()->Set_Range(
			pParameter->asGridList()->Get_Grid(0)->Get_Min(),
			pParameter->asGridList()->Get_Grid(0)->Get_Max()
		);
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Color_Blend::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("RANGE") )
	{
		pParameters->Set_Enabled("RANGE_PERCENT", pParameter->asInt() == 0 || pParameter->asInt() == 2);
		pParameters->Set_Enabled("RANGE_STDDEV" , pParameter->asInt() == 1 || pParameter->asInt() == 3);
		pParameters->Set_Enabled("RANGE_USER"   , pParameter->asInt() == 4);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Color_Blend::On_Execute(void)
{
	//-----------------------------------------------------
	m_pGrids	= Parameters("GRIDS")->asGridList();

	if( m_pGrids->Get_Grid_Count() < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("RANGE")->asInt() )
	{
	case 0:	// each grid's range
	case 1:	// each grid's standard deviation
		{
			m_Range_Min	= 0.0;
			m_Range_Max	= 0.0;
		}
		break;

	case 2:	// overall range
		{
			CSG_Simple_Statistics	s(m_pGrids->Get_Grid(0)->Get_Statistics());

			for(int i=1; i<m_pGrids->Get_Grid_Count(); i++)
			{
				s	+= m_pGrids->Get_Grid(i)->Get_Statistics();
			}

			double	d	= Parameters("RANGE_PERCENT")->asDouble() / 100.0;

			m_Range_Min	= s.Get_Minimum() + d * s.Get_Range();
			m_Range_Max	= s.Get_Maximum() - d * s.Get_Range();
		}
		break;

	case 3:	// overall standard deviation
		{
			CSG_Simple_Statistics	s(m_pGrids->Get_Grid(0)->Get_Statistics());

			for(int i=1; i<m_pGrids->Get_Grid_Count(); i++)
			{
				s	+= m_pGrids->Get_Grid(i)->Get_Statistics();
			}

			double	d	= Parameters("RANGE_STDDEV")->asDouble();

			m_Range_Min	= s.Get_Mean() - d * s.Get_StdDev(); if( Parameters("RANGE_KEEP")->asBool() && m_Range_Min < s.Get_Minimum() ) m_Range_Min = s.Get_Minimum();
			m_Range_Max	= s.Get_Mean() + d * s.Get_StdDev(); if( Parameters("RANGE_KEEP")->asBool() && m_Range_Max > s.Get_Maximum() ) m_Range_Max = s.Get_Maximum();
		}
		break;

	case 4:	// user defined
		{
			m_Range_Min	= Parameters("RANGE_USER")->asRange()->Get_Min();
			m_Range_Max	= Parameters("RANGE_USER")->asRange()->Get_Max();
		}
		break;
	}

	//-----------------------------------------------------
	m_pGrid	= Parameters("GRID")->asGrid();
	m_pGrid	->Set_Name(_TL("Color Blending"));
	m_pGrid	->Assign(m_pGrids->Get_Grid(0));

	DataObject_Set_Colors(m_pGrid, *Parameters("COLORS")->asColors());
	DataObject_Update    (m_pGrid, SG_UI_DATAOBJECT_SHOW);

	//-----------------------------------------------------
	m_nFiles	= 0;

	int	n	= Parameters("LOOP")->asInt() ? m_pGrids->Get_Grid_Count() : m_pGrids->Get_Grid_Count() - 1;
	
	do
	{
		for(int i=0; i<n && Process_Get_Okay(); i++)
		{
			Blend(i);
		}
	}
	while( Parameters("LOOP")->asInt() == 2 && Process_Get_Okay() );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Color_Blend::Blend(int iGrid)
{
	CSG_Grid	*pA	= m_pGrids->Get_Grid( iGrid                                  );
	CSG_Grid	*pB	= m_pGrids->Get_Grid((iGrid + 1) % m_pGrids->Get_Grid_Count());

	int	nSteps	= 1 + Parameters("NSTEPS")->asInt();

	for(int iStep=1; iStep<=nSteps && Process_Get_Okay(); iStep++)
	{
		double	d	= iStep / (double)nSteps;

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pA->is_NoData(x, y) && !pB->is_NoData(x, y) )
				{
					double	a	= pA->asDouble(x, y);
					double	b	= pB->asDouble(x, y);

					m_pGrid->Set_Value(x, y, a + d * (b - a));
				}
				else
				{
					m_pGrid->Set_NoData(x, y);
				}
			}
		}

		Set_Progress(iGrid + d, Parameters("LOOP")->asInt() ? m_pGrids->Get_Grid_Count() : m_pGrids->Get_Grid_Count() - 1);

		Save();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Color_Blend::Set_Progress(double Position, double Range)
{
	bool	bResult	= CSG_Tool_Grid::Set_Progress(Position, Range);

	//-----------------------------------------------------
	switch( Parameters("RANGE")->asInt() )
	{
	case 0:	// each grid's range
		{
			double	d	= Parameters("RANGE_PERCENT")->asDouble() / 100.0;

			m_Range_Min	= m_pGrid->Get_Min() + d * m_pGrid->Get_Range();
			m_Range_Max	= m_pGrid->Get_Max() - d * m_pGrid->Get_Range();
		}
		break;

	case 1:	// each grid's standard deviation
		{
			double	d	= Parameters("RANGE_STDDEV")->asDouble();

			m_Range_Min	= m_pGrid->Get_Mean() - d * m_pGrid->Get_StdDev(); if( Parameters("RANGE_KEEP")->asBool() && m_Range_Min < m_pGrid->Get_Min() ) m_Range_Min = m_pGrid->Get_Min();
			m_Range_Max	= m_pGrid->Get_Mean() + d * m_pGrid->Get_StdDev(); if( Parameters("RANGE_KEEP")->asBool() && m_Range_Max > m_pGrid->Get_Max() ) m_Range_Max = m_pGrid->Get_Max();
		}
		break;
	}

	//-----------------------------------------------------
	if( Parameters("PROGRESS")->asBool() )
	{
		double	Mid	= m_Range_Min + (m_Range_Max - m_Range_Min) / 2.0;

		int		Value	= (int)(0.5 + (Get_NX() - 1) * Position / Range);

		for(int x=0; x<Get_NX(); x++)
		{
			int	y	= 0;

			if( x < Value )
			{
				m_pGrid->Set_Value(x, y++, m_Range_Min);
				m_pGrid->Set_Value(x, y++,         Mid);
				m_pGrid->Set_Value(x, y++, m_Range_Max);
			}
			else
			{
				m_pGrid->Set_NoData(x, y++);
				m_pGrid->Set_NoData(x, y++);
				m_pGrid->Set_NoData(x, y++);
			}
		}
	}

	//-----------------------------------------------------
	if( m_Range_Min < m_Range_Max )
	{
		DataObject_Update(m_pGrid, m_Range_Min, m_Range_Max);
	}
	else
	{
		DataObject_Update(m_pGrid);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Color_Blend::Save(void)
{
	CSG_String	File	= Parameters("FILE")->asString();

	if( !File.is_Empty() )
	{
		File	= SG_File_Make_Path(
			SG_File_Get_Path     (File), CSG_String::Format("%s%03d",
			SG_File_Get_Name     (File, false).c_str(), m_nFiles++),
			SG_File_Get_Extension(File)
		);

		bool	bResult;

		SG_RUN_TOOL(bResult, "io_grid_image", 0,
				SG_TOOL_PARAMETER_SET("GRID"    , m_pGrid)
			&&	SG_TOOL_PARAMETER_SET("FILE"    , File   )
		//	&&	SG_TOOL_PARAMETER_SET("FILE_KML", false  )
		)
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
