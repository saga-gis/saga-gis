/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pc_from_table.cpp                   //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//                Management and analysis of             //
//                laserscanning data                     //
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
#include "pc_from_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_From_Table::CPC_From_Table(void)
{
	Set_Name		(_TL("Point Cloud from Table"));

	Set_Author		(SG_T("V. Wichmann (c) 2015"));

	Set_Description	(_TW(		
		"This tool allows one to create a point cloud from a table.\n\n"
	));


	//-----------------------------------------------------

	CSG_Parameter *pNode = Parameters.Add_Table(
		NULL	, "TAB_IN"		,_TL("Table"),
		_TL("The input table."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_X"		, _TL("X"),
		_TL("The attribute field with the x-coordinate.")
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_Y"		, _TL("Y"),
		_TL("The attribute field with the y-coordinate.")
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_Z"		, _TL("Z"),
		_TL("The attribute field with the z-coordinate.")
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("The attribute fields to convert, optional.")
	);

	Parameters.Add_PointCloud(
		NULL	, "PC_OUT"		,_TL("Point Cloud"),
		_TL("The output point cloud."),
		PARAMETER_OUTPUT
	);

}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_From_Table::On_Execute(void)
{
	CSG_Table		*pTable;
	int				iFieldX, iFieldY, iFieldZ;
	int				*Features, nFeatures;
	CSG_PointCloud	*pPoints;

	//-----------------------------------------------------

	pTable		= Parameters("TAB_IN")->asTable();
	iFieldX		= Parameters("FIELD_X")->asInt();
	iFieldY		= Parameters("FIELD_Y")->asInt();
	iFieldZ		= Parameters("FIELD_Z")->asInt();

	Features	= (int *)Parameters("FIELDS")->asPointer();
	nFeatures	=        Parameters("FIELDS")->asInt    ();

	pPoints		= Parameters("PC_OUT")->asPointCloud();

	//-----------------------------------------------------

	pPoints->Create();
	pPoints->Set_Name(CSG_String::Format(SG_T("%s_pc"), pTable->Get_Name()));

	for( int j=0; j<nFeatures; j++ )
	{
		TSG_Data_Type	Data_Type;

		switch( pTable->Get_Field_Type(Features[j]) )
		{
		case SG_DATATYPE_Undefined:
		case SG_DATATYPE_Binary:
			SG_UI_Msg_Add_Error(_TL("Undefined or binary attribute field types are not supported!"));
			return( false );

		case SG_DATATYPE_String:
		case SG_DATATYPE_Date:
		case SG_DATATYPE_Color:
			Data_Type = SG_DATATYPE_String;						break;

		default:
			Data_Type = pTable->Get_Field_Type(Features[j]);	break;
		}

		pPoints->Add_Field(pTable->Get_Field_Name(Features[j]), Data_Type);
	}


	//-----------------------------------------------------
	Process_Set_Text(_TL("Converting ..."));

	for( int iRecord=0; iRecord<pTable->Get_Record_Count() && Set_Progress(iRecord, pTable->Get_Record_Count()); iRecord++ )
	{
		pPoints->Add_Point(pTable->Get_Record(iRecord)->asDouble(iFieldX), pTable->Get_Record(iRecord)->asDouble(iFieldY), pTable->Get_Record(iRecord)->asDouble(iFieldZ));

		for( int j=0; j<nFeatures; j++ )
		{
			switch( pPoints->Get_Attribute_Type(j) )
			{
			case SG_DATATYPE_String:
				pPoints->Set_Value(iRecord, j + 3, pTable->Get_Record(iRecord)->asString(Features[j]));
				break;
			default:
				pPoints->Set_Value(iRecord, j + 3, pTable->Get_Record(iRecord)->asDouble(Features[j]));
				break;
			}
		}
	}


	//-------------------------------------------------

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
