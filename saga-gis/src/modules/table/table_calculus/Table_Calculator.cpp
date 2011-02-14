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
#include "Table_Calculator.h"

//---------------------------------------------------------
CTableCalculator::CTableCalculator(void)
{
	Set_Name(_TL("Table calculator"));

	Set_Author(SG_T("Victor Olaya"));

	CSG_String	s(_TW(
		"The table calculator creates a new column based on existing columns and a mathematical formula. "
		"The columns are addressed by single characters (a-z) which correspond in alphabetical order to the columns "
		"('a' = first column, 'b' = second column, ...)\n"
		"Example with three columns: sin(a) * b + c\n\n"
		"The following operators are available for the formula definition:\n"
	));

	s	+= CSG_Formula::Get_Help_Operators();

	Set_Description(s);

	Parameters.Add_Table	(NULL, "TABLE"	, _TL("Table")		, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Table	(NULL, "RESULT"	, _TL("Result")		, _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_String	(NULL, "FORMULA", _TL("Formula")	, _TL(""), SG_T("a+b"));
	Parameters.Add_String	(NULL, "NAME"	, _TL("Field Name")	, _TL(""), SG_T("a+b"));
}

//---------------------------------------------------------
CTableCalculator::~CTableCalculator(void)
{}

//---------------------------------------------------------
bool CTableCalculator::On_Execute(void)
{
	int			nValues, Position;
	double		*Values;
	CSG_String	Message;
	CSG_Formula	Formula;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	Formula.Set_Formula(Parameters("FORMULA")->asString());

	if( Formula.Get_Error(&Position, &Message) )
	{
		Message_Add(Message);
		Message_Add(CSG_String::Format(SG_T("%s: #%d [%s]"), _TL("syntax error, position"), Position, Formula.Get_Formula().c_str()));

		return( false );
	}

	//-----------------------------------------------------
	pTable	= Parameters("RESULT")->asTable();

	if( Parameters("TABLE")->asTable() != pTable )
	{
		pTable->Assign(Parameters("TABLE")->asTable());
	}

	pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Parameters("TABLE")->asTable()->Get_Name(), Formula.Get_Formula().c_str()));
	pTable->Add_Field(Parameters("NAME")->asString(), SG_DATATYPE_Double);

	//-----------------------------------------------------
	nValues	= pTable->Get_Field_Count() - 1;
	Values	= new double[nValues];

	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		for(int iValue=0; iValue<nValues; iValue++)
		{
			Values[iValue]	= pRecord->asDouble(iValue);
		}

		pRecord->Set_Value(nValues, Formula.Get_Value(Values, nValues));
	}

	delete[](Values);

	//-----------------------------------------------------
	return( true );
}

