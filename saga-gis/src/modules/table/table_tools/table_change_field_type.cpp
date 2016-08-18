/**********************************************************
 * Version $Id: table_change_field_type.cpp 911 2011-11-11 11:11:11Z oconrad $
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
//               table_change_field_type.cpp             //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_change_field_type.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Change_Field_Type::CTable_Change_Field_Type(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Change Field Type"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"			, _TL("Field"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"			, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"			, _TL("Data Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			SG_Data_Type_Get_Name(SG_DATATYPE_String).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Date  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Color ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Byte  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Char  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Word  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Short ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_DWord ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Int   ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_ULong ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Long  ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Float ).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Double).c_str(),
			SG_Data_Type_Get_Name(SG_DATATYPE_Binary).c_str()
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Change_Field_Type::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("TABLE"))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FIELD")) )
	{
		CSG_Table	*pTable	= pParameters->Get_Parameter("TABLE")->asTable();

		if( pTable )
		{
			int	Type;

			switch( pTable->Get_Field_Type(pParameters->Get_Parameter("FIELD")->asInt()) )
			{
			default:
			case SG_DATATYPE_String:	Type	=  0;	break;
			case SG_DATATYPE_Date:		Type	=  1;	break;
			case SG_DATATYPE_Color:		Type	=  2;	break;
			case SG_DATATYPE_Byte:		Type	=  3;	break;
			case SG_DATATYPE_Char:		Type	=  4;	break;
			case SG_DATATYPE_Word:		Type	=  5;	break;
			case SG_DATATYPE_Short:		Type	=  6;	break;
			case SG_DATATYPE_DWord:		Type	=  7;	break;
			case SG_DATATYPE_Int:		Type	=  8;	break;
			case SG_DATATYPE_ULong:		Type	=  9;	break;
			case SG_DATATYPE_Long:		Type	= 10;	break;
			case SG_DATATYPE_Float:		Type	= 11;	break;
			case SG_DATATYPE_Double:	Type	= 12;	break;
			case SG_DATATYPE_Binary:	Type	= 13;	break;
			}

			pParameters->Get_Parameter("TYPE")->Set_Value(Type);
		}
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Field_Type::On_Execute(void)
{
	//-----------------------------------------------------
	TSG_Data_Type	Type;

	switch( Parameters("TYPE")->asInt() )
	{
	default:
	case  0:	Type	= SG_DATATYPE_String;	break;
	case  1:	Type	= SG_DATATYPE_Date;		break;
	case  2:	Type	= SG_DATATYPE_Color;	break;
	case  3:	Type	= SG_DATATYPE_Byte;		break;
	case  4:	Type	= SG_DATATYPE_Char;		break;
	case  5:	Type	= SG_DATATYPE_Word;		break;
	case  6:	Type	= SG_DATATYPE_Short;	break;
	case  7:	Type	= SG_DATATYPE_DWord;	break;
	case  8:	Type	= SG_DATATYPE_Int;		break;
	case  9:	Type	= SG_DATATYPE_ULong;	break;
	case 10:	Type	= SG_DATATYPE_Long;		break;
	case 11:	Type	= SG_DATATYPE_Float;	break;
	case 12:	Type	= SG_DATATYPE_Double;	break;
	case 13:	Type	= SG_DATATYPE_Binary;	break;
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("OUTPUT")->asTable();

	if( pTable && pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Assign  (Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}
	else
	{
		pTable	= Parameters("TABLE")->asTable();
	}

	//-----------------------------------------------------
	int			Field	= Parameters("FIELD" )->asInt();

	if( Type == pTable->Get_Field_Type(Field) )
	{
		Error_Set(_TL("nothing to do: original and desired field types are identical"));

		return( false );
	}

	//-----------------------------------------------------
	pTable->Set_Field_Type(Field, Type);

	//-----------------------------------------------------
	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
