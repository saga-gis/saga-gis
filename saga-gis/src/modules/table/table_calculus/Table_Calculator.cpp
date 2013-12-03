/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    TableCalculator.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Table_Calculator.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Calculator_Base::CTable_Calculator_Base(void)
{
	Set_Author	(SG_T("V.Olaya (c) 2004, O.Conrad (c) 2011"));

	CSG_String	s(_TW(
		"The table calculator calculates a new attribute from existing attributes based on a mathematical formula. "
		"Attributes are addressed by the character 'f' (for 'field') followed by the field number (i.e.: f1, f2, ..., fn) "
		"or by the field name in square brackets (e.g.: [Field Name]).\n"
		"Examples:\n"
		"sin(f1) * f2 + f3\n"
		"[Population] / [Area]\n"
		"\nThe following operators are available for the formula definition:\n"
	));

	s	+= CSG_Formula::Get_Help_Operators();

	Set_Description(s);

	Parameters.Add_String	(NULL, "FORMULA", _TL("Formula")	, _TL(""), SG_T("f1 + f2"));
	Parameters.Add_String	(NULL, "NAME"	, _TL("Field Name")	, _TL(""), SG_T("Calculation"));
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Calculator_Base::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	if( !pTable->is_Valid() || pTable->Get_Field_Count() <= 0 || pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("invalid table"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Formula	Formula;

	int		nFields	= pTable->Get_Field_Count();
	int		*Fields	= new int[nFields];

	if( !Formula.Set_Formula(Get_Formula(Parameters("FORMULA")->asString(), pTable, Fields, nFields)) )
	{
		CSG_String	Message;

		Formula.Get_Error(Message);

		Error_Set(Message);

		delete[](Fields);

		return( false );
	}

	if( nFields == 0 )
	{
		SG_UI_Msg_Add_Error(_TL("No attribute fields specified!"));
		delete[](Fields);
		return( false );
	}


	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable )
	{
		pTable	= Parameters("RESULT")->asTable();
		pTable->Create(*Parameters("TABLE")->asTable());
	}

	int	fResult	= Parameters("FIELD")->asInt();

//	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("TABLE")->asTable()->Get_Name(), Parameters("NAME")->asString()));
	pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());

	if( fResult < 0 || fResult >= pTable->Get_Field_Count() )
	{
		fResult	= pTable->Get_Field_Count();

		pTable->Add_Field(Parameters("NAME")->asString(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	CSG_Vector	Values(nFields);

	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		bool	bOkay	= true;

		for(int iField=0; iField<nFields && bOkay; iField++)
		{
			if( !pRecord->is_NoData(iField) )
			{
				Values[iField]	= pRecord->asDouble(Fields[iField]);
			}
			else
			{
				bOkay	= false;
			}
		}

		if( bOkay )
		{
			pRecord->Set_Value(fResult, Formula.Get_Value(Values.Get_Data(), nFields));
		}
		else
		{
			pRecord->Set_NoData(fResult);
		}
	}

	//-----------------------------------------------------
	delete[](Fields);

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
CSG_String	CTable_Calculator_Base::Get_Formula(CSG_String sFormula, CSG_Table *pTable, int *Fields, int &nFields)
{
	const SG_Char	vars[27]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	int		iField;

	for(iField=pTable->Get_Field_Count()-1, nFields=0; iField>=0 && nFields<26; iField--)
	{
		bool		bUse	= false;

		CSG_String	sField;

		sField.Printf(SG_T("f%d"), iField + 1);

		if( sFormula.Find(sField) >= 0 )
		{
			sFormula.Replace(sField, CSG_String(vars[nFields]));

			bUse	= true;
		}

		sField.Printf(SG_T("[%s]"), pTable->Get_Field_Name(iField));

		if( sFormula.Find(sField) >= 0 )
		{
			sFormula.Replace(sField, CSG_String(vars[nFields]));

			bUse	= true;
		}

		if( bUse )
		{
			Fields[nFields++]	= iField;
		}
	}

	return( sFormula );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Calculator::CTable_Calculator(void)
	: CTable_Calculator_Base()
{
	Set_Name	(_TL("Table Calculator"));

	Parameters.Add_Table	(NULL, "TABLE"	, _TL("Table")		, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Table	(NULL, "RESULT"	, _TL("Result")		, _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Table_Field(Parameters("TABLE"), "FIELD", _TL("Field"), _TL(""), true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Calculator_Shapes::CTable_Calculator_Shapes(void)
	: CTable_Calculator_Base()
{
	Set_Name	(_TL("Table Calculator (Shapes)"));

	Parameters.Add_Shapes	(NULL, "TABLE"	, _TL("Shapes")		, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Shapes	(NULL, "RESULT"	, _TL("Result")		, _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Table_Field(Parameters("TABLE"), "FIELD", _TL("Field"), _TL(""), true);
}



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
