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
#include "pc_drop_attribute.h"

#include <set>
#include <algorithm>


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Drop_Attribute::CPC_Drop_Attribute(void)
{

	Set_Name(_TL("Drop Point Cloud Attributes"));

	Set_Author(_TL("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The module can be used to drop attributes from a point cloud. "
		"In case the output dataset is not set, the attribute(s) will be dropped "
		"from the input dataset, i.e. the input dataset will be overwritten.\n\n"
	));


	//-----------------------------------------------------
	CSG_Parameter *pNode = Parameters.Add_PointCloud(
		NULL	, "INPUT"		,_TL("Input"),
		_TL("Point cloud to drop attribute(s) from."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("The attribute fields to drop.")
	);

	Parameters.Add_PointCloud(
		NULL	, "OUTPUT"		, _TL("Output"),
		_TL("Point cloud with attribute(s) dropped."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Drop_Attribute::On_Execute(void)
{
	CSG_PointCloud		*pInput, *pOutput;
	CSG_String			sName;
	CSG_Parameters		sParms;
	int					*Features, nFeatures;
	std::set<int>		setCols;
	std::set<int>::iterator it;


	pInput	= Parameters("INPUT")->asPointCloud();
	pOutput	= Parameters("OUTPUT")->asPointCloud();


	//-------------------------------------------------
	Features	= (int *)Parameters("FIELDS")->asPointer();
	nFeatures	=        Parameters("FIELDS")->asInt    ();

	if( !Features || nFeatures <= 0 )
	{
		Error_Set(_TL("You must specify at least one attribute to drop!"));

		return( false );
	}

	//-----------------------------------------------------
	if( !pOutput || pOutput == pInput )
	{
		sName = pInput->Get_Name();
		pOutput = SG_Create_PointCloud(pInput);
	}
	else
	{
		sName = CSG_String::Format(SG_T("%s_droppedAttr"), pInput->Get_Name());
		pOutput->Create(pInput);
	}
	

	//-----------------------------------------------------
	setCols.clear();

	for( int i=0; i<nFeatures; i++ )
	{
		setCols.insert(Features[i]);
	}


	//-----------------------------------------------------
	int j = 0;
	for(it=setCols.begin(); it!=setCols.end(); it++)
	{
		pOutput->Del_Field(*it - j);
		j++;
	}

	DataObject_Update(pOutput);

	for(int i=0; i<pInput->Get_Point_Count() && SG_UI_Process_Set_Progress(i, pInput->Get_Count()); i++)
	{
		pOutput->Add_Point(pInput->Get_X(i), pInput->Get_Y(i), pInput->Get_Z(i));

		for(int j=0, k=0; j<pInput->Get_Attribute_Count(); j++, k++)
		{
			it = setCols.find(j + 3);
			if( it != setCols.end() )
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
