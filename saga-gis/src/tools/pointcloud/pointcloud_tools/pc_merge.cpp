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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     pc_merge.cpp                      //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                     Magnus Bremer                     //
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
//    e-mail:     magnus.bremer@student.uibk.ac.at       //
//                                                       //
//    contact:    Magnus Bremer                          //
//                Innrain 52                             //
//                6020 Innsbruck                         //
//                Austria                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_merge.h"



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Merge::CPC_Merge(void)
{
	Set_Name		(_TL("Merge Point Clouds"));

	Set_Author		("Magnus Bremer (c) 2012");

	Set_Description	(_TW(
		"This tool can be used to merge point clouds. "
		"The attribute fields of the merged point cloud resemble those "
		"of the first point cloud in the input list. In order to merge "
		"the attributes of the additional point cloud layers, these must "
		"be consistent (field name and type) with the first point cloud "
		"in the input list. Missing attribute values are set to no-data. "
	));

	Parameters.Add_PointCloud_List("",
		"PC_LAYERS"		, _TL("Point Clouds"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud("",
		"PC_OUT"		, _TL("Merged Point Cloud"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"DEL_LAYERS"	, _TL("Delete Input"),
		_TL("Removes input layers from memory while merging."),
		true
	);

	Parameters.Add_Bool("",
		"ADD_IDENTIFIER", _TL("Add Input Identifier"),
		_TL("Adds a field with an identifier for the input point cloud a point originates from."),
		false
	);

	Parameters.Add_Int("ADD_IDENTIFIER",
		"START_VALUE"	, _TL("Start Value"),
		_TL("The start value to be used for the identifier."),
		1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPC_Merge::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("ADD_IDENTIFIER") )
	{
		pParameters->Set_Enabled("START_VALUE", pParameter->asBool());
	}

	//-----------------------------------------------------
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Merge::On_Execute(void)
{
	CSG_Parameter_PointCloud_List	*pList	= Parameters("PC_LAYERS")->asPointCloudList();

	if( pList->Get_Data_Count() < 2 )
	{
		Error_Set(_TL("Nothing to do. Less than two input layers."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_PointCloud	*pResult	= Parameters("PC_OUT")->asPointCloud();
	CSG_PointCloud	*pPoints	= pList->Get_PointCloud(0);

	pResult->Create(pPoints);
	pResult->Set_Name(_TL("Merged"));
	pResult->Set_NoData_Value_Range(pPoints->Get_NoData_Value(), pPoints->Get_NoData_hiValue());

	int	ID = 0, fID = Parameters("ADD_IDENTIFIER")->asBool() ? pResult->Get_Field_Count() : -1;

	if( fID >= 0 )
	{
		pResult->Add_Field("ID", SG_DATATYPE_Int);

		ID	= Parameters("START_VALUE")->asInt();
	}

	bool	bDelete	= Parameters("DEL_LAYERS")->asBool();

	//-----------------------------------------------------
	for(int i=0; i<pList->Get_Data_Count() && Process_Get_Okay(); i++, ID++)
	{
		pPoints	= pList->Get_PointCloud(i);

		if( pPoints == pResult )
		{
			Message_Fmt("\n%s: %s", _TL("Warning"), _TL("Input is identical with target."));

			continue;
		}

		Process_Set_Text(CSG_String::Format("%s: %s", _TL("processing"), pPoints->Get_Name()));

		int	nPoints	= pPoints->Get_Point_Count();

		for(int iPoint=nPoints-1; iPoint>0 && Set_Progress(nPoints - iPoint, nPoints); iPoint--)
		{
			pResult->Add_Record(pPoints->Get_Record(iPoint));

			if( bDelete )
			{
				pPoints->Del_Point(iPoint);
			}

			if( fID >= 0 )
			{
				pResult->Set_Value(pResult->Get_Point_Count() - 1, fID, ID);
			}
		}

		if( bDelete && Parameters.Get_Manager() )
		{
			Parameters.Get_Manager()->Delete(pPoints, true);

			DataObject_Update(pList->Get_Item(i));
		}
	}

	//-----------------------------------------------------
	pList->Del_Items();

//	DataObject_Set_Parameter(pResult, "DISPLAY_VALUE_AGGREGATE", 3);	// highest z
//	DataObject_Set_Parameter(pResult, "COLORS_TYPE"            , 3);	// graduated colors
//	DataObject_Set_Parameter(pResult, "METRIC_ATTRIB"          , 2);	// z-field
//	DataObject_Set_Parameter(pResult, "METRIC_ZRANGE", pResult->Get_Minimum(2), pResult->Get_Maximum(2));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------