
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

#define	FIELD_X			0
#define	FIELD_Y			1
#define	FIELD_GRIDS		2


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

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Grid Value Request.\n")
	);


	//-----------------------------------------------------
	// 2. Standard in- and output...

	m_pGrids	= Parameters.Add_Grid_List(
		NULL	, "GRID_LIST"		, _TL("Grids to request"),
		"",
		PARAMETER_INPUT
	)->asGridList();

	Parameters.Add_Table(
		NULL	, "VALUES"			, _TL("Values"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"			, _TL("Method"),
		"",
		CSG_String::Format("%s|%s|",
			_TL("Single value"),
			_TL("Collect values")
		), 4
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOLATION"	, _TL("Interpolation"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);
}

//---------------------------------------------------------
CGrid_Value_Request::~CGrid_Value_Request(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Request::On_Execute(void)
{
	int		iGrid;

	if( m_pGrids->Get_Count() > 0 )
	{
		m_Method		= Parameters("METHOD")			->asInt();
		m_Interpolation	= Parameters("INTERPOLATION")	->asInt();

		m_pTable		= Parameters("VALUES")			->asTable();
		m_pTable->Destroy();
		m_pTable->Set_Name(_TL("Grid Values"));

		switch( m_Method )
		{
		case 0: default:
			m_pTable->Add_Field(_TL("NAME")	, TABLE_FIELDTYPE_String);
			m_pTable->Add_Field(_TL("VALUE")	, TABLE_FIELDTYPE_Double);

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				m_pTable->Add_Record()->Set_Value(FIELD_NAME, m_pGrids->asGrid(iGrid)->Get_Name());
			}
			break;

		case 1:
			m_pTable->Add_Field("X"		, TABLE_FIELDTYPE_Double);
			m_pTable->Add_Field("Y"		, TABLE_FIELDTYPE_Double);

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				m_pTable->Add_Field(m_pGrids->asGrid(iGrid)->Get_Name(), TABLE_FIELDTYPE_Double);
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
bool CGrid_Value_Request::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	int				iGrid;
	double			Value;
	CSG_Table_Record	*pRecord;

	if( Mode == MODULE_INTERACTIVE_LDOWN && m_pGrids->Get_Count() > 0 )
	{
		switch( m_Method )
		{
		default:
			return( false );

		case 0:
			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				if( m_pGrids->asGrid(iGrid)->Get_Value(ptWorld.m_point, Value, m_Interpolation, true) )
				{
					m_pTable->Get_Record(iGrid)->Set_Value(FIELD_VALUE, Value);
				}
				else
				{
					m_pTable->Get_Record(iGrid)->Set_Value(FIELD_VALUE, 0.0);
				}
			}
			break;

		case 1:
			pRecord	= m_pTable->Add_Record();

			pRecord->Set_Value(FIELD_X, ptWorld.Get_X());
			pRecord->Set_Value(FIELD_Y, ptWorld.Get_Y());

			for(iGrid=0; iGrid<m_pGrids->Get_Count(); iGrid++)
			{
				if( m_pGrids->asGrid(iGrid)->Get_Value(ptWorld.m_point, Value, m_Interpolation, true) )
				{
					pRecord->Set_Value(FIELD_GRIDS + iGrid, Value);
				}
				else
				{
					pRecord->Set_Value(FIELD_GRIDS + iGrid, 0.0);
				}
			}
			break;
		}

		DataObject_Update(m_pTable);

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
