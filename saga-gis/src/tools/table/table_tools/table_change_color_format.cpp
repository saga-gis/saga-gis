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
//                      table_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             table_change_color_format.cpp             //
//                                                       //
//                 Copyright (C) 2013 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_change_color_format.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Change_Color_Format::CTable_Change_Color_Format(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Change Color Format"));

	Set_Author		(SG_T("Copyrights (c) 2013 by Volker Wichmann"));

	Set_Description	(_TW(
		"This tool allows one to convert table fields from SAGA RGB to R, G, B values and "
		"vice versa.\n\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Table"),
		_TL("The input table"),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_SAGA_RGB"	, _TL("SAGA RGB"),
		_TL("The field with SAGA RGB values"),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD_RED"		, _TL("Red"),
		_TL("The field with R values (0-255)"),
		true
	);
	Parameters.Add_Table_Field(
		pNode	, "FIELD_GREEN"		, _TL("Green"),
		_TL("The field with G values (0-255)"),
		true
	);
	Parameters.Add_Table_Field(
		pNode	, "FIELD_BLUE"		, _TL("Blue"),
		_TL("The field with B values (0-255)"),
		true
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"			, _TL("Output"),
		_TL("The output table"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "MODE"			, _TL("Mode of Operation"),
		_TL("Choose the mode of operation"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("SAGA RGB to R,G,B"),
			_TL("R,G,B to SAGA RGB")
		), 0
	);
}


//---------------------------------------------------------
CTable_Change_Color_Format::~CTable_Change_Color_Format(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Color_Format::On_Execute(void)
{

	//-----------------------------------------------------
	CSG_Table	*pTable		= Parameters("OUTPUT")			->asTable();
	int			iFieldRGB	= Parameters("FIELD_SAGA_RGB")	->asInt();
	int			iFieldR		= Parameters("FIELD_RED")		->asInt();
	int			iFieldG		= Parameters("FIELD_GREEN")		->asInt();
	int			iFieldB		= Parameters("FIELD_BLUE")		->asInt();

	int			iMode		= Parameters("MODE")			->asInt();


	//-----------------------------------------------------
	if( pTable && pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Assign  (Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}
	else
	{
		pTable	= Parameters("TABLE")->asTable();
	}

	switch( iMode )
	{
	default:
	case 0:
		if( iFieldRGB < 0 )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid SAGA RGB field for this kind of operation!"));
			return( false );
		}
		pTable->Add_Field(_TL("R"), SG_DATATYPE_Int);
		iFieldR = pTable->Get_Field_Count() - 1;
		pTable->Add_Field(_TL("G"), SG_DATATYPE_Int);
		pTable->Add_Field(_TL("B"), SG_DATATYPE_Int);
		break;
	case 1:
		if( iFieldR < 0 || iFieldG < 0 || iFieldB < 0 )
		{
			SG_UI_Msg_Add_Error(_TL("Please provide a valid R,G,B fields for this kind of operation!"));
			return( false );
		}
		pTable->Add_Field(_TL("RGB"), SG_DATATYPE_Int);
		iFieldRGB = pTable->Get_Field_Count() - 1;
		break;
	}


	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		switch( iMode )
		{
		default:
		case 0:
			pRecord->Set_Value(iFieldR,		SG_GET_R(pRecord->asInt(iFieldRGB)));
			pRecord->Set_Value(iFieldR + 1,	SG_GET_G(pRecord->asInt(iFieldRGB)));
			pRecord->Set_Value(iFieldR + 2,	SG_GET_B(pRecord->asInt(iFieldRGB)));
			break;
		case 1:
			pRecord->Set_Value(iFieldRGB,	SG_GET_RGB(pRecord->asInt(iFieldR), pRecord->asInt(iFieldG), pRecord->asInt(iFieldB)));
			break;
		}
	}


	//-----------------------------------------------------
	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


//---------------------------------------------------------
int CTable_Change_Color_Format::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("MODE")) )
	{
		int		iMode	= pParameters->Get_Parameter("MODE")->asInt();

		pParameters->Get_Parameter("FIELD_SAGA_RGB"	)->Set_Enabled(iMode == 0);
		pParameters->Get_Parameter("FIELD_RED"		)->Set_Enabled(iMode == 1);
		pParameters->Get_Parameter("FIELD_GREEN"	)->Set_Enabled(iMode == 1);
		pParameters->Get_Parameter("FIELD_BLUE"		)->Set_Enabled(iMode == 1);
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
