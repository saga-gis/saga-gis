
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     table_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                table_text_replacer.cpp                //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "table_text_replacer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_REPLACEMENT(FROM, TO)	{	CSG_Table_Record *pR = pTable->Add_Record(); pR->Set_Value(0, FROM); pR->Set_Value(1, TO);	}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Text_Replacer::CTable_Text_Replacer(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Replace Text"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"For the selected attribute or, if not specified, for all text attributes "
		"this tool replaces text strings with replacements as defined in table "
		"'Text Replacements'."
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Attribute"),
		_TL(""),
		true
	);

	Parameters.Add_Table("",
		"OUT_TABLE"	, _TL("Table with Text Replacements"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes("",
		"OUT_SHAPES", _TL("Shapes with Text Replacements"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters.Add_FixedTable("",
		"REPLACE"	, _TL("Text Replacements"),
		_TL("")
	)->asTable();

	pTable->Add_Field(_TL("Original"   ), SG_DATATYPE_String);
	pTable->Add_Field(_TL("Replacement"), SG_DATATYPE_String);

	ADD_REPLACEMENT("ä", "ae");
	ADD_REPLACEMENT("ö", "oe");
	ADD_REPLACEMENT("ü", "ue");
	ADD_REPLACEMENT("Ä", "Ae");
	ADD_REPLACEMENT("Ö", "Oe");
	ADD_REPLACEMENT("Ü", "Ue");
	ADD_REPLACEMENT("ß", "sz");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Text_Replacer::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_Data_Object	*pObject	= pParameter->asDataObject();

		pParameters->Set_Enabled("OUT_TABLE" , pObject && pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table );
		pParameters->Set_Enabled("OUT_SHAPES", pObject && pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Text_Replacer::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	if( pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
	{
		CSG_Shapes	*pOutput	= Parameters("OUT_SHAPES")->asShapes();

		if( pOutput )
		{
			pOutput->Create(*((CSG_Shapes *)pTable));

			pTable	= pOutput;
		}
	}
	else // if( pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table )
	{
		CSG_Table	*pOutput	= Parameters("OUT_TABLE" )->asTable();

		if( pOutput )
		{
			pOutput->Create(*pTable);

			pTable	= pOutput;
		}
	}

	if( pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Replacements"));
	}

	//-----------------------------------------------------
	m_pReplacer	= Parameters("REPLACE")->asTable();
	int	iField	= Parameters("FIELD"  )->asInt  ();

	size_t	nChanges	= 0;

	for(int iRecord=0; iRecord<pTable->Get_Count(); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		if( iField >= 0 )
		{
			nChanges	+= Replace(pRecord, iField);
		}
		else for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			if( pTable->Get_Field_Type(i) == SG_DATATYPE_String )
			{
				nChanges	+= Replace(pRecord, i);
			}
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %d", _TL("number of replacements"), nChanges);

	if( pTable == Parameters("TABLE")->asTable() )
	{
		DataObject_Update(pTable);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline size_t CTable_Text_Replacer::Replace(CSG_Table_Record *pRecord, int iField)
{
	CSG_String	Text(pRecord->asString(iField));

	size_t	nChanges	= 0;

	for(int i=0; i<m_pReplacer->Get_Count(); i++)
	{
		CSG_Table_Record	*pReplace	= m_pReplacer->Get_Record(i);

		nChanges	+= Text.Replace(pReplace->asString(0), pReplace->asString(1));
	}

	if( nChanges > 0 )
	{
		pRecord->Set_Value(iField, Text);
	}

	return( nChanges );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
