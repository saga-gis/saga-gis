
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
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
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
	Set_Name		(_TL("Drop Point Cloud Attributes"));

	Set_Author		("Volker Wichmann (c) 2010, LASERDATA GmbH");

	Set_Description	(_TW(
		"The tool can be used to drop attributes from a point cloud. "
		"In case the output dataset is not set, the attribute(s) will be dropped "
		"from the input dataset, i.e. the input dataset will be overwritten.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud("",
		"INPUT"		, _TL("Input"),
		_TL("Point cloud to drop attribute(s) from."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields("INPUT",
		"FIELDS"	, _TL("Attributes"),
		_TL("The attribute field(s) to drop.")
	);

	Parameters.Add_PointCloud("",
		"OUTPUT"	, _TL("Output"),
		_TL("Point cloud with attribute(s) dropped."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Drop_Attribute::On_Execute(void)
{
	int	*Features	= (int *)Parameters("FIELDS")->asPointer();
	int	nFeatures	=        Parameters("FIELDS")->asInt    ();

	if( !Features || nFeatures <= 0 )
	{
		Error_Set(_TL("You must specify at least one attribute to drop!"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_PointCloud	*pInput  = Parameters("INPUT" )->asPointCloud();
	CSG_PointCloud	*pOutput = Parameters("OUTPUT")->asPointCloud(), Output;

	if( !pOutput || pOutput == pInput )
	{
		pOutput	= &Output;
	}

	pOutput->Create(pInput);

	//-----------------------------------------------------
	int	i;

	std::set<int>			setCols;
	std::set<int>::iterator it;

	setCols.clear();

	for(i=0; i<nFeatures; i++)
	{
		setCols.insert(Features[i]);
	}

	for(i=0, it=setCols.begin(); it!=setCols.end(); i++, it++)
	{
		pOutput->Del_Field(*it - i);
	}

	//-----------------------------------------------------
	for(i=0; i<pInput->Get_Point_Count() && SG_UI_Process_Set_Progress(i, pInput->Get_Count()); i++)
	{
		pOutput->Add_Point(pInput->Get_X(i), pInput->Get_Y(i), pInput->Get_Z(i));

		for(int j=0, k=0; j<pInput->Get_Attribute_Count(); j++, k++)
		{
			it	= setCols.find(j + 3);

			if( it != setCols.end() )
			{
				k--;
				continue;
			}

			switch (pInput->Get_Attribute_Type(j))
			{
			default:					pOutput->Set_Attribute(k, pInput->Get_Attribute(i, j));		break;
			case SG_DATATYPE_Date:
			case SG_DATATYPE_String:	CSG_String sAttr; pInput->Get_Attribute(i, j, sAttr); pOutput->Set_Attribute(k, sAttr);		break;
			}
		}
	}

	//-----------------------------------------------------
	if( pOutput == &Output )
	{
		CSG_MetaData	History	= pInput->Get_History();
		CSG_String		Name	= pInput->Get_Name   ();

		pInput->Assign(pOutput);

		pInput->Get_History() = History;
		pInput->Set_Name(Name);

		Parameters("OUTPUT")->Set_Value(pInput);
	}
	else
	{
		pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Dropped Attributes"));
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Drop_Attribute::On_After_Execution(void)
{
	CSG_PointCloud	*pOutput	= Parameters("OUTPUT")->asPointCloud();

	if( pOutput == NULL )
	{
		pOutput	= Parameters("INPUT")->asPointCloud();
	}

	DataObject_Set_Parameter(pOutput, "DISPLAY_VALUE_AGGREGATE",  3);	// highest z
	DataObject_Set_Parameter(pOutput, "METRIC_COLORS"          , 12);	// number of colors
	DataObject_Set_Parameter(pOutput, "COLORS_TYPE"            ,  3);	// graduated color
	DataObject_Set_Parameter(pOutput, "METRIC_ATTRIB"          ,  2);	// z attrib
	DataObject_Set_Parameter(pOutput, "METRIC_ZRANGE",
		pOutput->Get_Mean(2) - 2.0 * pOutput->Get_StdDev(2),
		pOutput->Get_Mean(2) + 2.0 * pOutput->Get_StdDev(2)
	);

	DataObject_Set_Colors(pOutput, 11, SG_COLORS_RAINBOW);

	if( pOutput == Parameters("INPUT")->asPointCloud() )
	{
		Parameters("OUTPUT")->Set_Value(DATAOBJECT_NOTSET);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
