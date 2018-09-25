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
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Fill.cpp                      //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                    Andre Ringeler                     //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Grid_Fill.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Filler::CGrid_Filler(void)
{
	m_pGrid	= NULL;
}

//---------------------------------------------------------
void CGrid_Filler::Parameters_Add(CSG_Parameters &Parameters)
{
	Parameters.Add_Grid("",
		"GRID"			, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"GRID_OUT"		, _TL("Changed Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"REPLACE"		, _TL("Value to be replaced"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("value at mouse position"),
			_TL("fixed value")
		), 0
	);

	Parameters.Add_Double("REPLACE",
		"REPLACE_VALUE"	, _TL("Fixed value to be replaced"),
		_TL("Replace only this value with respect to the specified tolerance."),
		0.0
	);

	Parameters.Add_Double("REPLACE",
		"TOLERANCE"		, _TL("Tolerance"),
		_TL(""),
		1.0, 0.0, true
	);

	Parameters.Add_Bool("REPLACE",
		"IGNORE_NODATA"	, _TL("Ignore No-Data"),
		_TL("Do not fill areas representing no-data."),
		true
	);

	Parameters.Add_Bool("",
		"FILL_NODATA"	, _TL("Fill with No-Data"),
		_TL(""),
		false
	);

	Parameters.Add_Double("FILL_NODATA",
		"FILL_VALUE"	, _TL("Fill Value"),
		_TL(""),
		0.0
	);
}

//---------------------------------------------------------
void CGrid_Filler::Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("REPLACE") )
	{
		pParameters->Set_Enabled("REPLACE_VALUE", pParameter->asInt() == 1);
		pParameters->Set_Enabled("IGNORE_NODATA", pParameter->asInt() != 1);
	}

	if( pParameter->Cmp_Identifier("FILL_NODATA") )
	{
		pParameters->Set_Enabled("FILL_VALUE", pParameter->asBool() == false);
	}
}

//---------------------------------------------------------
bool CGrid_Filler::Parameters_Set(CSG_Parameters &Parameters)
{
	m_pGrid	= Parameters("GRID_OUT")->asGrid();

	if( m_pGrid && m_pGrid != Parameters("GRID")->asGrid() )
	{
		m_pGrid->Assign(Parameters("GRID")->asGrid());
		m_pGrid->Fmt_Name("%s [%s]", Parameters("GRID")->asGrid()->Get_Name(), _TL("Flood Fill"));
	}
	else
	{
		m_pGrid	= Parameters("GRID")->asGrid();
	}

	m_Replace		= Parameters("REPLACE"      )->asInt   ();
	m_zReplace		= Parameters("REPLACE_VALUE")->asDouble();
	m_zTolerance	= Parameters("TOLERANCE"    )->asDouble();
	m_zFill			= Parameters("FILL_NODATA"  )->asBool  () ? m_pGrid->Get_NoData_Value()
					: Parameters("FILL_VALUE"   )->asDouble();
	m_bNoData		= Parameters("IGNORE_NODATA")->asBool  () == false;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Filler::Fill(const TSG_Point &Point)
{
	int	x	= m_pGrid->Get_System().Get_xWorld_to_Grid(Point.x);
	int	y	= m_pGrid->Get_System().Get_yWorld_to_Grid(Point.y);

	if( !m_pGrid->is_InGrid(x, y, m_bNoData) )
	{
		return( 0 );
	}

	//-----------------------------------------------------
	double	zMin, zMax;

	switch( m_Replace )
	{
	default:	// value at mouse position
		zMin	= m_pGrid->asDouble(x, y) - m_zTolerance;
		zMax	= m_pGrid->asDouble(x, y) + m_zTolerance;
		break;

	case  1:	// fixed value
		zMin	= m_zReplace - m_zTolerance;
		zMax	= m_zReplace + m_zTolerance;
		break;
	}

	//-----------------------------------------------------
	int	nReplaced	= 1;
	m_pGrid->Set_Value(x, y, m_zFill);
	m_Stack.Push(x, y);

	while( m_Stack.Get_Size() > 0 && SG_UI_Process_Set_Progress(nReplaced, m_pGrid->Get_NCells()) )
	{
		m_Stack.Pop(x, y);

		for(int i=0; i<8; i+=2)
		{
			int	ix	= m_pGrid->Get_System().Get_xTo(i, x);
			int	iy	= m_pGrid->Get_System().Get_yTo(i, y);

			if(	m_pGrid->is_InGrid(ix, iy, m_bNoData) )
			{
				double	z	= m_pGrid->asDouble(ix, iy);

				if( z != m_zFill && z >= zMin && z <= zMax )
				{
					nReplaced++;
					m_pGrid->Set_Value(ix, iy, m_zFill);
					m_Stack.Push(ix, iy);
				}
			}
		}
	}

	m_Stack.Clear();

	//-----------------------------------------------------
	return( nReplaced );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Fill::CGrid_Fill(void)
{
	Set_Name		(_TL("Change Grid Values - Flood Fill"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"A flood fill algorithm will be used for replacement of grid cell values "
		"starting at the positions of the input points. "
		"If one or more points are selected, only these will be processed, otherwise all. "
		"If the target grid is not set, the changes will be applied to the original grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters_Add(Parameters);
}

//---------------------------------------------------------
int CGrid_Fill::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CGrid_Fill::On_Execute(void)
{
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( !pPoints->is_Valid() || !pPoints->Get_Extent().Intersects(Get_System()->Get_Extent()) || !Parameters_Set(Parameters) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int	nReplaced	= 0;

	#define GET_NPOINTS  (bSelection ? pPoints->Get_Selection_Count() : pPoints->Get_Count())
	#define GET_POINT(i) (bSelection ? pPoints->Get_Selection(i)->Get_Point(0) : pPoints->Get_Shape(i)->Get_Point(0))

	bool	bSelection	= pPoints->Get_Selection_Count() > 0;

	for(int i=0; i<GET_NPOINTS && Process_Get_Okay(); i++)
	{
		nReplaced	+= Fill(GET_POINT(i));
	}

	Message_Fmt("\n%d %s\n", nReplaced, _TL("replacements"));

	return( true );	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Fill_Interactive::CGrid_Fill_Interactive(void)
{
	Set_Name		(_TL("Change Grid Values - Flood Fill"));

	Set_Author		("A.Ringeler (c) 2005, O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Interactively use the flood fill method for replacement of grid cell values "
		"starting at the positions clicked with the left mouse button. "
		"If the target grid is not set, the changes will be applied to the original grid. "
	));

	//-----------------------------------------------------
	Parameters_Add(Parameters);
}

//---------------------------------------------------------
int CGrid_Fill_Interactive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool_Grid_Interactive::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CGrid_Fill_Interactive::On_Execute(void)
{
	return( Parameters_Set(Parameters) );
}

//---------------------------------------------------------
bool CGrid_Fill_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	if( Mode == TOOL_INTERACTIVE_LDOWN )
	{
		Message_Add(_TL("Starting flood fill..."));

		int	nReplaced	= Fill(ptWorld);

		Message_Add(_TL("ready"), false);
		Message_Fmt("\n%d %s", nReplaced, _TL("replacements"));

		DataObject_Update(m_pGrid, m_pGrid->Get_Min(), m_pGrid->Get_Max());

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
