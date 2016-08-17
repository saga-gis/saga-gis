///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             Shapes_Support_Tool_Chains.cpp            //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Shapes_Support_Tool_Chains.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Shapes_From_List::CSelect_Shapes_From_List(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Select Shapes from List"));

	Set_Author		("V. Wichmann (c) 2016");

	Set_Description	(_TW(
		"Main use of this tool is to support tool chain development, allowing to pick a single shapefile from a shapes list.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL	, "SHAPESLIST"	, _TL("Shapes List"),
		_TL("The input shapes list."),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL("The shapefile picked from the shapes list."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "INDEX"	, _TL("Index"),
		_TL("The list index of the shapefile to pick. Indices start at zero."),
		PARAMETER_TYPE_Int, 0, 0, true
	);
}

//---------------------------------------------------------
bool CSelect_Shapes_From_List::On_Execute(void)
{
	CSG_Parameter_Shapes_List	*pShapes	= Parameters("SHAPESLIST")->asShapesList();

	if( pShapes->Get_Count() <= 0 )
	{
		Error_Set(_TL("no shapes in list"));

		return( false );
	}

	int	Index	= Parameters("INDEX")->asInt();

	if( Index >= pShapes->Get_Count() )
	{
		Error_Set(_TL("index out of range"));

		return( false );
	}

	Parameters("SHAPES")->Set_Value(pShapes->asShapes(Index));

	return( true );	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
