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
		"The module can be used to drop an attribute from a point cloud. "
		"In case the output dataset is not set, the attribute will be dropped "
		"from the input dataset, i.e. the input dataset will be overwritten.\n\n")
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
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL("Point Cloud with attribute dropped."),
		PARAMETER_OUTPUT_OPTIONAL
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
	CSG_PointCloud		*pInput, *pOutput;
	int					iField;
	CSG_String			sName;
	CSG_Parameters		sParms;

	pInput	= Parameters("INPUT")->asPointCloud();
	pOutput	= Parameters("OUTPUT")->asPointCloud();
	iField	= Parameters("ATTRIB")->asInt();

	if (iField < 0)
	{
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("We must keep the coordinates, please choose another field than x, y, or z!")));
		return (false);
	}

	//-----------------------------------------------------
	if (!pOutput || pOutput == pInput )
	{
		sName = pInput->Get_Name();
		pOutput = SG_Create_PointCloud(pInput);
	}
	else
	{
		sName = CSG_String::Format(SG_T("%s_%s_dropped"), pInput->Get_Name(), pInput->Get_Field_Name(iField));
		pOutput->Create(pInput);
	}
	

	pOutput->Del_Field(iField);

	DataObject_Update(pOutput);

	for (int i=0; i<pInput->Get_Point_Count() && SG_UI_Process_Set_Progress(i, pInput->Get_Count()); i++)
	{
		pOutput->Add_Point(pInput->Get_X(i), pInput->Get_Y(i), pInput->Get_Z(i));

		for (int j=0, k=0; j<pInput->Get_Attribute_Count(); j++, k++)
		{
			if (j == iField)
			{
				k--;
				continue;
			}

			pOutput->Set_Attribute(k, pInput->Get_Attribute(i, j));
		}
	}


	//-----------------------------------------------------
	if (!Parameters("OUTPUT")->asPointCloud() || Parameters("OUTPUT")->asPointCloud() == pInput )
	{
		pInput->Assign(pOutput);
		pInput->Set_Name(sName);

		DataObject_Update(pInput);
		delete(pOutput);

		DataObject_Get_Parameters(pInput, sParms);
		Set_Display_Attributes(pInput, sParms);
	}
	else
	{
		pOutput->Set_Name(sName);
		DataObject_Update(pOutput);

		DataObject_Get_Parameters(pOutput, sParms);
		Set_Display_Attributes(pOutput, sParms);
	}


	//-----------------------------------------------------
	return( true );
}


//---------------------------------------------------------
void CPC_Drop_Attribute::Set_Display_Attributes(CSG_PointCloud *pPC, CSG_Parameters &sParms)
{
	if (sParms("METRIC_ATTRIB")	&& sParms("COLORS_TYPE") && sParms("METRIC_COLORS")
		&& sParms("METRIC_ZRANGE") && sParms("DISPLAY_VALUE_AGGREGATE"))
	{
		sParms("DISPLAY_VALUE_AGGREGATE")->Set_Value(3);		// highest z
		sParms("COLORS_TYPE")->Set_Value(2);                    // graduated color
		sParms("METRIC_COLORS")->asColors()->Set_Count(255);    // number of colors
		sParms("METRIC_ATTRIB")->Set_Value(2);					// z attrib
		sParms("METRIC_ZRANGE")->asRange()->Set_Range(pPC->Get_Minimum(2), pPC->Get_Maximum(2));
	}

	DataObject_Set_Parameters(pPC, sParms);
	DataObject_Update(pPC);

	return;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
