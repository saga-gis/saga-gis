
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
double	g_NoData_loValue	= -99999.;
double	g_NoData_hiValue	= -99999.;

//---------------------------------------------------------
double	fnc_NoData_Value(void)
{
	return( g_NoData_loValue );
}

//---------------------------------------------------------
double	fnc_is_NoData_Value(double Value)
{
	return( SG_IS_BETWEEN(g_NoData_loValue, Value, g_NoData_hiValue) );
}


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
		"The table calculator calculates a new attribute from existing "
		"attributes based on a mathematical formula. Attributes are addressed "
		"by the character 'f' (for 'field') followed by the field number "
		"(i.e.: f1, f2, ..., fn) or by the field name in square brackets "
		"(e.g.: [Field Name]).\n"
		"Examples:\n"
		"- sin(f1) * f2 + f3\n"
		"- [Population] / [Area]\n"
		"\n"
		"If the use no-data flag is unchecked and a no-data value appears in "
		"a record's input, no calculation is performed for it and the result "
		"is set to no-data.\n"
		"\n"
		"Following operators are available for the formula definition:\n"
	));

	const CSG_String	Operators[5][2]	=
	{
		{	"nodata()"   , _TL("Returns tables's no-data value"                            )	},
		{	"isnodata(x)", _TL("Returns true (1), if x is a no-data value, else false (0)" )	},
		{	"", ""	}
	};

	m_Formula.Add_Function(SG_T("nodata"  ), (TSG_PFNC_Formula_1)fnc_NoData_Value   , 0, 0);
	m_Formula.Add_Function(SG_T("isnodata"), (TSG_PFNC_Formula_1)fnc_is_NoData_Value, 1, 0);

	s	+= CSG_Formula::Get_Help_Operators(true, Operators);

	Set_Description(s);

	//-----------------------------------------------------
	if( bShapes )
	{
		Set_Name(CSG_String::Format("%s [%s]", _TL("Field Calculator"), _TL("Shapes")));

		Parameters.Add_Shapes("", "TABLE" , _TL("Shapes"), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Shapes("", "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}
	else
	{
		Set_Name(_TL("Field Calculator"));

		Parameters.Add_Table ("", "TABLE" , _TL("Table" ), _TL(""), PARAMETER_INPUT);
		Parameters.Add_Table ("", "RESULT", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	//-----------------------------------------------------
	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Field"),
		_TL(""),
		true
	);

	Parameters.Add_String("TABLE",
		"NAME"		, _TL("Field Name"),
		_TL(""),
		_TL("Calculation")
	);

	Parameters.Add_String("",
		"FORMULA"	, _TL("Formula"),
		_TL(""),
		"f1 + f2"
	);

	Parameters.Add_Bool("",
		"SELECTION"	, _TL("Selection"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"USE_NODATA", _TL("Use No-Data"),
		_TL(""),
		false
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
	if( !m_Formula.Set_Formula(Get_Formula(Parameters("FORMULA")->asString(), pTable, m_Values)) )
	{
		CSG_String	Message;

		m_Formula.Get_Error(Message);

		Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable )
	{
		pTable	= Parameters("RESULT")->asTable();

		if( pTable->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{
			pTable->Create(*Parameters("TABLE")->asShapes());
		}
		else
		{
			pTable->Create(*Parameters("TABLE")->asTable());
		}
	}

	pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());

	//-----------------------------------------------------
	m_Result	= Parameters("FIELD")->asInt();

	if( m_Result < 0 || m_Result >= pTable->Get_Field_Count() )
	{
		m_Result	= pTable->Get_Field_Count();

		pTable->Add_Field(Parameters("NAME")->asString(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	m_bNoData	= Parameters("USE_NODATA")->asBool();

	g_NoData_loValue	= pTable->Get_NoData_Value  ();
	g_NoData_hiValue	= pTable->Get_NoData_hiValue();

	if( pTable->Get_Selection_Count() > 0 && Parameters("SELECTION")->asBool() )
	{
		for(size_t i=0; i<pTable->Get_Selection_Count() && Set_Progress(i, pTable->Get_Selection_Count()); i++)
		{
			Get_Value(pTable->Get_Selection(i));
		}
	}
	else
	{
		for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
		{
			Get_Value(pTable->Get_Record(i));
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
bool CTable_Calculator_Base::Get_Value(CSG_Table_Record *pRecord)
{
	CSG_Vector	Values(m_Values.Get_Size());

	bool	bNoData	= false;

	for(int i=0; i<m_Values.Get_Size(); i++)
	{
		Values[i]	= pRecord->asDouble(m_Values[i]);

		if( !m_bNoData && pRecord->is_NoData(m_Values[i]) )
		{
			bNoData	= true;
		}
	}

	if( bNoData == false )
	{
		pRecord->Set_Value(m_Result, m_Formula.Get_Value(Values));

		return( true );
	}

	pRecord->Set_NoData(m_Result);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CTable_Calculator_Base::Get_Formula(CSG_String Formula, CSG_Table *pTable, CSG_Array_Int &Values)
{
	const SG_Char	vars[27]	= SG_T("abcdefghijklmnopqrstuvwxyz");

	Values.Destroy();

	for(int iField=pTable->Get_Field_Count()-1; iField>=0 && Values.Get_Size()<26; iField--)
	{
		bool	bUse	= false;

		CSG_String	s;

		s.Printf("f%d", iField + 1);

		if( Formula.Find(s) >= 0 )
		{
			Formula.Replace(s, CSG_String(vars[Values.Get_Size()]));

			bUse	= true;
		}

		s.Printf("F%d", iField + 1);

		if( Formula.Find(s) >= 0 )
		{
			Formula.Replace(s, CSG_String(vars[Values.Get_Size()]));

			bUse	= true;
		}

		s.Printf("[%s]", pTable->Get_Field_Name(iField));

		if( Formula.Find(s) >= 0 )
		{
			Formula.Replace(s, CSG_String(vars[Values.Get_Size()]));

			bUse	= true;
		}

		if( bUse )
		{
			Values	+= iField;
		}
	}

	return( Formula );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
