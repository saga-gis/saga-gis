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
//                       Tutorial                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Value_Request.cpp                //
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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
#include "Grid_Value_Request.h"

//---------------------------------------------------------
#define	FIELD_NAME		0
#define	FIELD_VALUE		1

#define	FIELD_X_WORLD	0
#define	FIELD_Y_WORLD	1
#define FIELD_X_GRID	2
#define FIELD_Y_GRID	3
#define	FIELD_GRIDS		4


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_Request::CGrid_Value_Request(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Grid Value Request"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"The tool reads out grids values from one or more input grids and reports these in a table. "
		"Besides the grid value(s), the x- and y-coordinates of the queried cell(s) are reported. The "
		"reported grid coordinates (column, row) are those of the first input grid.\n")
	);


	//-----------------------------------------------------
	// 2. Standard in- and output...

	m_pGrids	= Parameters.Add_Grid_List(
		NULL	, "GRID_LIST"		, _TL("Grids to request"),
		_TL(""),
		PARAMETER_INPUT
	)->asGridList();

	Parameters.Add_Table(
		NULL	, "VALUES"			, _TL("Values"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Single value"),
			_TL("Collect values")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"		, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Set_Drag_Mode(TOOL_INTERACTIVE_DRAG_NONE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Request::On_Execute(void)
{
	int		iGrid;

	if( m_pGrids->Get_Count() > 0 )
	{
		m_Method	= Parameters("METHOD")->asInt();

		m_pTable	= Parameters("VALUES")->asTable();
		m_pTable->Destroy();
		m_pTable->Set_Name(_TL("Grid Values"));

		switch( Parameters("RESAMPLING")->asInt() )
		{
		default:	m_Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
		case  1:	m_Resampling	= GRID_RESAMPLING_Bilinear;			break;
		case  2:	m_Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
		case  3:	m_Resampling	= GRID_RESAMPLING_BSpline;			break;
		}

		switch( m_Method )
		{
		case 0: default:
			m_pTable->Add_Field(_TL("NAME" ), SG_DATATYPE_String);
			m_pTable->Add_Field(_TL("VALUE"), SG_DATATYPE_Double);

			m_pTable->Add_Record()->Set_Value(FIELD_NAME, _TL("X World"));
			m_pTable->Add_Record()->Set_Value(FIELD_NAME, _TL("Y World"));
			m_pTable->Add_Record()->Set_Value(FIELD_NAME, _TL("X Grid 1"));
			m_pTable->Add_Record()->Set_Value(FIELD_NAME, _TL("Y Grid 1"));

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				m_pTable->Add_Record()->Set_Value(FIELD_NAME, m_pGrids->asGrid(iGrid)->Get_Name());
			}
			break;

		case 1:
			m_pTable->Add_Field(_TL("X World" ), SG_DATATYPE_Double);
			m_pTable->Add_Field(_TL("Y World" ), SG_DATATYPE_Double);
			m_pTable->Add_Field(_TL("X Grid 1"), SG_DATATYPE_Int);
			m_pTable->Add_Field(_TL("Y Grid 1"), SG_DATATYPE_Int);

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				m_pTable->Add_Field(m_pGrids->asGrid(iGrid)->Get_Name(), SG_DATATYPE_Double);
			}
			break;
		}

		DataObject_Update(m_pTable, true);

		return( true );
	}

	Message_Dlg(_TL("There is no grid to request."), Get_Name());

	return( false );
}

//---------------------------------------------------------
bool CGrid_Value_Request::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	int					iGrid;
	double				Value;
	CSG_Table_Record	*pRecord;

	if( m_pGrids->Get_Count() > 0 )
	{
		switch( m_Method )
		{
		//-------------------------------------------------
		case 0:
			if( Mode == TOOL_INTERACTIVE_LDOWN || Mode == TOOL_INTERACTIVE_MOVE_LDOWN )
			{
				m_pTable->Get_Record(FIELD_X_WORLD)->Set_Value(FIELD_VALUE, ptWorld.Get_X());
				m_pTable->Get_Record(FIELD_Y_WORLD)->Set_Value(FIELD_VALUE, ptWorld.Get_Y());

				m_pTable->Get_Record(FIELD_X_GRID)->Set_Value(FIELD_VALUE, m_pGrids->asGrid(0)->Get_System().Get_xWorld_to_Grid(ptWorld.Get_X()));
				m_pTable->Get_Record(FIELD_Y_GRID)->Set_Value(FIELD_VALUE, m_pGrids->asGrid(0)->Get_System().Get_yWorld_to_Grid(ptWorld.Get_Y()));

				for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
				{
					if( m_pGrids->asGrid(iGrid)->Get_Value(ptWorld, Value, m_Resampling, false, true) )
					{
						m_pTable->Get_Record(iGrid + FIELD_GRIDS)->Set_Value(FIELD_VALUE, Value);
					}
					else
					{
						m_pTable->Get_Record(iGrid + FIELD_GRIDS)->Set_Value(FIELD_VALUE, 0.0);
					}
				}

				DataObject_Update(m_pTable);

				return( true );
			}
			break;

		//-------------------------------------------------
		case 1:
			if( Mode == TOOL_INTERACTIVE_LDOWN )
			{
				pRecord	= m_pTable->Add_Record();

				pRecord->Set_Value(FIELD_X_WORLD, ptWorld.Get_X());
				pRecord->Set_Value(FIELD_Y_WORLD, ptWorld.Get_Y());

				pRecord->Set_Value(FIELD_X_GRID, m_pGrids->asGrid(0)->Get_System().Get_xWorld_to_Grid(ptWorld.Get_X()));
				pRecord->Set_Value(FIELD_Y_GRID, m_pGrids->asGrid(0)->Get_System().Get_yWorld_to_Grid(ptWorld.Get_Y()));

				for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
				{
					if( m_pGrids->asGrid(iGrid)->Get_Value(ptWorld, Value, m_Resampling, false, true) )
					{
						pRecord->Set_Value(FIELD_GRIDS + iGrid, Value);
					}
					else
					{
						pRecord->Set_Value(FIELD_GRIDS + iGrid, 0.0);
					}
				}

				DataObject_Update(m_pTable);

				return( true );
			}
			break;
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
