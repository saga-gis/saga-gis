
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
//                 pc_drop_attribute.cpp                 //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "pc_drop_attribute.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Drop_Attribute::CPC_Drop_Attribute(void)
{

	Set_Name(_TL("Drop Point Cloud Attribute"));

	Set_Author(_TL("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The module can be used to drop an attribute from a point cloud.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "INPUT"		,_TL("Input"),
		_TL("Point Cloud to drop attribute from."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		Parameters("INPUT"), "ATTRIB", _TL("Attribute to drop"),
		_TL("Attribute to drop."),
		false
	);

	Parameters.Add_PointCloud(
		NULL	, "RESULT"		, _TL("Result"),
		_TL("Resulting Point Cloud."),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CPC_Drop_Attribute::~CPC_Drop_Attribute(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Drop_Attribute::On_Execute(void)
{
	CSG_PointCloud		*pInput, *pResult;
	int					AttrField;

	pInput				= Parameters("INPUT")->asPointCloud();
	pResult				= Parameters("RESULT")->asPointCloud();
	AttrField			= Parameters("ATTRIB")->asInt() - 3;

	if (AttrField < 0)
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("We must keep the coordinates, please choose another field than x,y,z!")));
		return (false);
	}

	pResult->Create(pInput);
	pResult->Set_Name(CSG_String::Format(SG_T("%s_drop_%s"), pInput->Get_Name(), pInput->Get_Attribute_Name(AttrField)));

	pResult->Del_Field(AttrField + 3);

	for (int i=0; i<pInput->Get_Point_Count() && SG_UI_Process_Set_Progress(i, pInput->Get_Count()); i++)
	{
		pResult->Add_Point(pInput->Get_X(i), pInput->Get_Y(i), pInput->Get_Z(i));

		int	offset = 0;
		for (int j=0; j<pInput->Get_Attribute_Count(); j++)
		{
			if (j == AttrField)
			{
				offset = -1;
				continue;
			}

			pResult->Set_Attribute(j + offset, pInput->Get_Attribute(i, j));
		}
	}

	//pResult->Del_Field(AttrField + 3);


	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
