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
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_field_statistics.cpp              //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
//    e-mail:     wichmann@laserdata.at                  //
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
#include "table_field_statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Field_Statistics::CTable_Field_Statistics(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Field Statistics"));

	Set_Author		(SG_T("V. Wichmann (c) 2014"));

	Set_Description	(_TW(
		"The tools allows one to calculate statistics (n, min, max, range, sum, "
		"mean, variance and standard deviation) for attribute fields of tables, "
		"shapefiles or point clouds.\n\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL("The input table."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("The (numeric) fields to calculate the statistics for.")
	);

	Parameters.Add_Table(
		NULL	, "STATISTICS"	, _TL("Statistics"),
		_TL("The calculated statistics."),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Field_Statistics::On_Execute(void)
{
	CSG_Table	*pTab_in, *pTab_out;
	int			nFeatures, *Features;

	//-----------------------------------------------------
	pTab_in		= Parameters("TABLE")->asTable();
	pTab_out	= Parameters("STATISTICS")->asTable();
	
	Features	= (int *)Parameters("FIELDS")->asPointer();
	nFeatures	=        Parameters("FIELDS")->asInt    ();

	//-----------------------------------------------------
	if( !Features || nFeatures <= 0 )
	{
		Error_Set(_TL("No attribute fields selected!"));

		return( false );
	}

	//-----------------------------------------------------
	pTab_out->Destroy();
	pTab_out->Set_Name(CSG_String::Format(_TL("%s_stats"), pTab_in->Get_Name()));
	
	pTab_out->Add_Field(_TL("Field")	, SG_DATATYPE_String);
	pTab_out->Add_Field(_TL("n")		, SG_DATATYPE_Long);
	pTab_out->Add_Field(_TL("min")		, SG_DATATYPE_Double);
	pTab_out->Add_Field(_TL("max")		, SG_DATATYPE_Double);
	pTab_out->Add_Field(_TL("range")	, SG_DATATYPE_Double);
	pTab_out->Add_Field(_TL("sum")		, SG_DATATYPE_Double);
	pTab_out->Add_Field(_TL("mean")		, SG_DATATYPE_Double);
	pTab_out->Add_Field(_TL("variance")	, SG_DATATYPE_Double);
	pTab_out->Add_Field(_TL("stddev")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(int iFeature=0; iFeature<nFeatures; iFeature++)
	{
		if( SG_Data_Type_is_Numeric(pTab_in->Get_Field_Type(Features[iFeature])) )
		{
			CSG_Table_Record *pRecord = pTab_out->Add_Record();

			pRecord->Set_Value(0, pTab_in->Get_Field_Name(Features[iFeature]));
			pRecord->Set_Value(1, pTab_in->Get_N(Features[iFeature]));
			pRecord->Set_Value(2, pTab_in->Get_Minimum(Features[iFeature]));
			pRecord->Set_Value(3, pTab_in->Get_Maximum(Features[iFeature]));
			pRecord->Set_Value(4, pTab_in->Get_Range(Features[iFeature]));
			pRecord->Set_Value(5, pTab_in->Get_Sum(Features[iFeature]));
			pRecord->Set_Value(6, pTab_in->Get_Mean(Features[iFeature]));
			pRecord->Set_Value(7, pTab_in->Get_Variance(Features[iFeature]));
			pRecord->Set_Value(8, pTab_in->Get_StdDev(Features[iFeature]));
		}
		else
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("WARNING: skipping non-numeric field '%s'!"), pTab_in->Get_Field_Name(Features[iFeature])), true);
		}
	}

	//-----------------------------------------------------
	DataObject_Update(pTab_out);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
