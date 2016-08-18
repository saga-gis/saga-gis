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

	Set_Author		(SG_T("Magnus Bremer (c) 2012"));

	Set_Description	(_TW("The tool can be used to merge point clouds.\n"
						 "The attribute fields of the merged point cloud resemble those "
						 "of the main point cloud. In order to merge the attributes of the "
						 "additional point cloud layers, these must be consistent "
						 "(field name and type) with the main point cloud. Missing "
						 "attribute values are set to NoData.\n\n"
	));

	Parameters.Add_PointCloud(
		NULL	, "PC_IN"	, _TL("Main Point Cloud"),
		_TL("Main layer. The output layer will have the same fields in the attribute table as this layer."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud_List(
		NULL	, "PC_LAYERS"	, _TL("Additional Point Clouds"),
		_TL("Additional point clouds to merge with main point cloud."),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "PC_OUT"	, _TL("Merged Point Cloud"),
		_TL("The merged point cloud."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "ADD_IDENTIFIER"	, _TL("Add Identifier"),
		_TL("Add unique identifier attribute field to output, ID resembles processing order."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		Parameters("ADD_IDENTIFIER")	, "START_VALUE"	, _TL("Start Value"),
		_TL("The start value to use for the identifier."),
		PARAMETER_TYPE_Int, 0
	);
}


//---------------------------------------------------------
CPC_Merge::~CPC_Merge(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Merge::On_Execute(void)
{
	CSG_PointCloud		*pMain, *pAdd, *pResult;
	bool				bID;
	int					iStartValue;
	int					iAccept = 0;

	CSG_Parameter_PointCloud_List	*pPointCloudList;
	std::vector<CSG_String>			FieldNames;
	std::vector<TSG_Data_Type>		FieldTypes;


	pMain				= Parameters("PC_IN")->asPointCloud();
	pPointCloudList		= Parameters("PC_LAYERS")->asPointCloudList();
	pResult				= Parameters("PC_OUT")->asPointCloud();
	bID					= Parameters("ADD_IDENTIFIER")->asBool();
	iStartValue			= Parameters("START_VALUE")->asInt();

	pResult->Create(pMain);
	pResult->Set_Name(CSG_String::Format(_TL("%s_merged"), pMain->Get_Name()));
	if( bID )
		pResult->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
	DataObject_Update(pResult);


	//copy main layer into destination
	for(int iField=0; iField<pMain->Get_Attribute_Count(); iField++)
	{
		FieldNames.push_back(pMain->Get_Attribute_Name(iField));
		FieldTypes.push_back(pMain->Get_Attribute_Type(iField));
	}

	for(int i=0; i<pMain->Get_Count() && Set_Progress(i, pMain->Get_Count()); i++)
	{
		pResult->Add_Point(pMain->Get_X(i), pMain->Get_Y(i), pMain->Get_Z(i));

		for(int iField=0; iField<pMain->Get_Attribute_Count(); iField++)
		{
			switch (pMain->Get_Attribute_Type(iField))
			{
			default:					pResult->Set_Attribute(iAccept, iField, pMain->Get_Attribute(i, iField));		break;
			case SG_DATATYPE_Date:
			case SG_DATATYPE_String:	CSG_String sAttr; pMain->Get_Attribute(i, iField, sAttr); pResult->Set_Attribute(iAccept, iField, sAttr);		break;
			}
		}

		if( bID )
			pResult->Set_Attribute(iAccept, pMain->Get_Attribute_Count(), iStartValue);

		iAccept++;	
	}


	//copy additional layer into destination
	if( (pPointCloudList = Parameters("PC_LAYERS")->asPointCloudList()) != NULL && pPointCloudList->Get_Count() > 0 )
	{
		for(int iLayer=0; iLayer<pPointCloudList->Get_Count(); iLayer++)
		{
			if( (pAdd = pPointCloudList->asPointCloud(iLayer)) != NULL )
			{
				if( pAdd->Get_Type() == pMain->Get_Type() )
				{
					pAdd = pPointCloudList->asPointCloud(iLayer);

					std::vector<int>	FieldMapping(FieldNames.size(), -1);	// pMain to pAdd

					for(int iField=0; iField<pAdd->Get_Attribute_Count(); iField++)
					{
						CSG_String ThisField = pAdd->Get_Attribute_Name(iField);

						for(int j=0; j<FieldNames.size(); j++)
						{	
							if( !ThisField.CmpNoCase(FieldNames[j]) )
							{
								if( pAdd->Get_Attribute_Type(iField) == FieldTypes[j] )
								{
									FieldMapping[j] = iField;
									break;
								}
							}
						}
					}

					for(int i=0; i<pAdd->Get_Count() && Set_Progress(i, pAdd->Get_Count()); i++)
					{
						pResult->Add_Point(pAdd->Get_X(i), pAdd->Get_Y(i), pAdd->Get_Z(i));

						for(int iField=0; iField<pMain->Get_Attribute_Count(); iField++)
						{
							if( FieldMapping[iField] > -1 )
							{
								switch (pAdd->Get_Attribute_Type(iField))
								{
								default:					pResult->Set_Attribute(iAccept, iField, pAdd->Get_Attribute(i, FieldMapping[iField]));		break;
								case SG_DATATYPE_Date:
								case SG_DATATYPE_String:	CSG_String sAttr; pAdd->Get_Attribute(i, FieldMapping[iField], sAttr); pResult->Set_Attribute(iAccept, iField, sAttr);		break;
								}
							}
							else
								pResult->Set_Attribute(iAccept, iField, pMain->Get_NoData_Value());
						}

						if( bID )
						{
							pResult->Set_Attribute(iAccept, pMain->Get_Attribute_Count(), iLayer + iStartValue + 1);
						}

						iAccept++;
					}
				}
			}
		}
	}


	CSG_Parameters	sParms;
	DataObject_Get_Parameters(pResult, sParms);
	
	if( sParms("METRIC_ATTRIB")	&& sParms("COLORS_TYPE") && sParms("METRIC_COLORS")
		&& sParms("METRIC_ZRANGE") && sParms("DISPLAY_VALUE_AGGREGATE") )
	{
		sParms("DISPLAY_VALUE_AGGREGATE")		->Set_Value(3);
		sParms("COLORS_TYPE")					->Set_Value(2);
		sParms("METRIC_COLORS")->asColors()		->Set_Count(255);
		sParms("METRIC_ATTRIB")					->Set_Value(2);
		sParms("METRIC_ZRANGE")->asRange()		->Set_Range(pResult->Get_Minimum(2), pResult->Get_Maximum(2));
	}

	DataObject_Set_Parameters(pResult, sParms);
	DataObject_Update(pResult);

	return( true );
}


//---------------------------------------------------------
int CPC_Merge::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("ADD_IDENTIFIER")) )
	{
		pParameters->Get_Parameter("START_VALUE")->Set_Enabled(pParameter->asBool());
	}

	//-----------------------------------------------------
	return (1);
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------