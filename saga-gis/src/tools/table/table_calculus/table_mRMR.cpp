
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
//                     table_mRMR.cpp                    //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_mRMR.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_mRMR::CTable_mRMR(void)
{
	Set_Name		(_TL("Minimum Redundancy Feature Selection"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Identify the most relevant features for subsequent classification of tabular data.\n"
		"\n"
	) + CSG_mRMR::Get_Description());

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"DATA"		, _TL("Data"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("DATA",
		"CLASS"		, _TL("Class Identifier"),
		_TL("")
	);

	Parameters.Add_Table("",
		"SELECTION"	, _TL("Feature Selection"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"VERBOSE"	, _TL("Verbose Output"),
		_TL("output of intermediate results to execution message window"),
		true
	);

	CSG_mRMR::Parameters_Add(&Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_mRMR::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_mRMR::Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_mRMR::On_Execute(void)
{
	CSG_mRMR	mRMR;

	mRMR.Set_Verbose(Parameters("VERBOSE")->asBool());

	CSG_Table	*pData	= Parameters("DATA")->asTable();

	if( !mRMR.Set_Data(*pData, Parameters("CLASS")->asInt(), &Parameters) )
	{
		return( false );
	}

	if( !mRMR.Get_Selection(&Parameters) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pSelection	= Parameters("SELECTION")->asTable();

	pSelection->Destroy();
	pSelection->Fmt_Name("%s (%s)", _TL("Feature Selection"), pData->Get_Name());

	pSelection->Add_Field("RANK" , SG_DATATYPE_Int);
	pSelection->Add_Field("INDEX", SG_DATATYPE_Int);
	pSelection->Add_Field("NAME" , SG_DATATYPE_String);
	pSelection->Add_Field("SCORE", SG_DATATYPE_Double);

	for(int i=0; i<mRMR.Get_Count(); i++)
	{
		CSG_Table_Record	*pFeature	= pSelection->Add_Record();

		pFeature->Set_Value(0, i + 1);
		pFeature->Set_Value(1, mRMR.Get_Index(i));
		pFeature->Set_Value(2, mRMR.Get_Name (i));
		pFeature->Set_Value(3, mRMR.Get_Score(i));
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
