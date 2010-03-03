
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     io_table_odbc                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Shapes.cpp                       //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
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
#include "get_connection.h"

#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_Load::CPoints_Load(void)
{
	Set_Name		(_TL("ODBC Points Import"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Imports points from a database via ODBC."
	));

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Choice(
		NULL	, "TABLES"		, _TL("Tables"),
		_TL(""),
		CSG_String::Format(SG_T("%s|"),
			_TL("--- no table available ---")
		)
	);
}

//---------------------------------------------------------
bool CPoints_Load::On_Before_Execution(void)
{
	if( !CSG_ODBC_Module::On_Before_Execution() )
	{
		return( false );
	}

	CSG_String	Table(Parameters("TABLES")->asString());

	return( true );
}

//---------------------------------------------------------
bool CPoints_Load::On_Execute(void)
{
	if( Get_Connection()->is_Connected() )
	{
		CSG_Parameter_Choice	*pTables	= Parameters("TABLES")	->asChoice();
		CSG_Shapes				*pShapes	= Parameters("POINTS")	->asShapes();

		return( Get_Connection()->Table_Load(*pShapes, pTables->asString()) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_Save::CPoints_Save(void)
{
	Set_Name		(_TL("ODBC Points Export"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Exports points to a database via ODBC."
	));

	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);
}

//---------------------------------------------------------
bool CPoints_Save::On_Execute(void)
{
	if( Get_Connection()->is_Connected() )
	{
		CSG_Shapes	*pShapes	= Parameters("POINTS")	->asShapes();

		return( Get_Connection()->Table_Save(pShapes->Get_Name(), *pShapes) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
