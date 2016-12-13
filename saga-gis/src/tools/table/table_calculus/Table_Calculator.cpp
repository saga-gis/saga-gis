
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
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
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
CTable_Calculator_Base::CTable_Calculator_Base(bool bShapes)
{
	Set_Author	("V.Olaya (c) 2004, O.Conrad (c) 2011");

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

	if( bShapes )
	{
		Set_Name(CSG_String::Format("%s [%s]", _TL("Table Field Calculator"), _TL("Shapes")));

		Parameters.Add_Shapes(NULL, "TABLE" , _TL("Shapes"), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Shapes(NULL, "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}
	else
	{
		Set_Name(_TL("Table Field Calculator"));

		Parameters.Add_Table (NULL, "TABLE" , _TL("Table" ), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Table (NULL, "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	Parameters.Add_Table_Field(Parameters("TABLE"),
		"FIELD"    , _TL("Field"),
		_TL(""),
		true
	);

	Parameters.Add_String(Parameters("TABLE"),
		"NAME"     , _TL("Field Name"),
		_TL(""),
		_TL("Calculation")
	);

	Parameters.Add_String(NULL,
		"FORMULA"  , _TL("Formula"),
		_TL(""),
		"f1 + f2"
	);

	Parameters.Add_Bool(NULL,
		"SELECTION", _TL("Selection"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Calculator_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Table	*pTable	= (CSG_Table *)pParameters->Get_Parameter("TABLE")->asDataObject();

	if( pTable )
	{
		CSG_Parameter	*pField	= pParameters->Get_Parameter("FIELD");

		pParameters->Set_Enabled("FIELD"    , true);
		pParameters->Set_Enabled("NAME"     , pField->asInt() < 0);	// not set
		pParameters->Set_Enabled("SELECTION", pTable->Get_Selection_Count() > 0);
	}
	else
	{
		pParameters->Set_Enabled("FIELD"    , false);
		pParameters->Set_Enabled("NAME"     , false);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
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
	CSG_Array_Int	Fields;
	CSG_Formula		Formula;

	if( !Formula.Set_Formula(Get_Formula(Parameters("FORMULA")->asString(), pTable, Fields)) )
	{
		CSG_String	Message;

		Formula.Get_Error(Message);

		Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable )
	{
		pTable	= Parameters("RESULT")->asTable();
		pTable->Create(*Parameters("TABLE")->asTable());
	}

	int	fResult	= Parameters("FIELD")->asInt();

//	pTable->Set_Name(CSG_String::Format("%s [%s]", Parameters("TABLE")->asTable()->Get_Name(), Parameters("NAME")->asString()));
	pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());

	if( fResult < 0 || fResult >= pTable->Get_Field_Count() )
	{
		fResult	= pTable->Get_Field_Count();

		pTable->Add_Field(Parameters("NAME")->asString(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	bool	bSelection	= pTable->Get_Selection_Count() > 0 && Parameters("SELECTION")->asBool();

	CSG_Vector	Values(Fields.Get_Size());

	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		if( !bSelection || pTable->is_Selected(iRecord) )
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

			bool	bNoData	= false;

			for(int iField=0; iField<Fields.Get_Size() && !bNoData; iField++)
			{
				if( !(bNoData       = pRecord->is_NoData(Fields[iField])) )
				{
					Values[iField]	= pRecord->asDouble (Fields[iField]);
				}
			}

			if( !bNoData )
			{
				pRecord->Set_Value(fResult, Formula.Get_Value(Values));
			}
			else
			{
				pRecord->Set_NoData(fResult);
			}
		}
	}

	//-----------------------------------------------------
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
CSG_String	CTable_Calculator_Base::Get_Formula(CSG_String sFormula, CSG_Table *pTable, CSG_Array_Int &Fields)
{
	const SG_Char	vars[27]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	Fields.Destroy();

	for(int iField=pTable->Get_Field_Count()-1; iField>=0 && Fields.Get_Size()<26; iField--)
	{
		bool	bUse	= false;

		CSG_String	sField;

		sField.Printf("f%d", iField + 1);

		if( sFormula.Find(sField) >= 0 )
		{
			sFormula.Replace(sField, CSG_String(vars[Fields.Get_Size()]));

			bUse	= true;
		}

		sField.Printf("[%s]", pTable->Get_Field_Name(iField));

		if( sFormula.Find(sField) >= 0 )
		{
			sFormula.Replace(sField, CSG_String(vars[Fields.Get_Size()]));

			bUse	= true;
		}

		if( bUse )
		{
			Fields	+= iField;
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
