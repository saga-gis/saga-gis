///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               pc_support_tool_chains.cpp              //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                    Volker Wichmann                    //
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
#include "pc_support_tool_chains.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_PointCloud_From_List::CSelect_PointCloud_From_List(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Select Point Cloud from List"));

	Set_Author		("V. Wichmann (c) 2017");

	Set_Description	(_TW(
		"Main use of this tool is to support tool chain development, allowing to pick a single point cloud from a point cloud list.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud_List(
		NULL	, "PC_LIST"	, _TL("Point Cloud List"),
		_TL("The input point cloud list."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "PC"	, _TL("Point Cloud"),
		_TL("The point cloud picked from the point cloud list."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "INDEX"	, _TL("Index"),
		_TL("The list index of the point cloud to pick. Indices start at zero."),
		PARAMETER_TYPE_Int, 0, 0, true
	);
}

//---------------------------------------------------------
bool CSelect_PointCloud_From_List::On_Execute(void)
{
	CSG_Parameter_PointCloud_List	*pPC_List	= Parameters("PC_LIST")->asPointCloudList();

	if( pPC_List->Get_Item_Count() <= 0 )
	{
		Error_Set(_TL("no point clouds in list"));

		return( false );
	}

	int	Index	= Parameters("INDEX")->asInt();

	if( Index >= pPC_List->Get_Item_Count() )
	{
		Error_Set(_TL("index out of range"));

		return( false );
	}

	Parameters("PC")->Set_Value(pPC_List->Get_PointCloud(Index));

	return( true );	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
