
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Table_Calculator.h                   //
//                                                       //
//                    Copyrights (c)                     //
//            V.Olaya, O.Conrad, J.Spitzmueller          //
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
#include "Table_Calculator.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Field_Calculator::CTable_Field_Calculator(void)
{
	Set_Name	(_TL("Field Calculator"));

	Set_Author	("V.Olaya (c) 2004, O.Conrad (c) 2011, J.Spitzmueller, scilands GmbH (c) 2022");

	CSG_String	s(_TW(
		"The table calculator calculates a new attribute from existing "
		"attributes based on a mathematical formula. Attributes are addressed "
		"by the character 'f' (for 'field') followed by the field number "
		"(i.e.: f1, f2, ..., fn) or by the field name in quotation marks or square brackets "
		"(e.g.: \"Field Name\").\n"
		"Examples:\n"
		"- sin(f1) * f2 + f3\n"
		"- \"Population\" / \"Area\"\n"
		"One can also use the drop-down-menu to append fields numbers to the formula.\n"
		"\n"
		"If the use no-data flag is unchecked and a no-data value appears in "
		"a record's input, no calculation is performed for it and the result "
		"is set to no-data.\n"
		"\n"
		"Following operators are available for the formula definition:\n"
	));

	const CSG_String Operators[5][2] =
	{
		{ "nodata()"   , _TL("Returns tables's no-data value"                            ) },
		{ "isnodata(x)", _TL("Returns true (1), if x is a no-data value, else false (0)" ) },
		{ "", "" }
	};

	m_Formula.Add_Function("nodata"  , (TSG_Formula_Function_1)fnc_NoData_Value   , 0, false);
	m_Formula.Add_Function("isnodata", (TSG_Formula_Function_1)fnc_is_NoData_Value, 1, false);

	s	+= CSG_Formula::Get_Help_Operators(true, Operators);

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Result Field"),
		_TL("Select a field for the results. If not set a new field for the results will be added."),
		true
	);

	Parameters.Add_Table ("", "RESULT_TABLE" , _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Shapes("", "RESULT_SHAPES", _TL("Result"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_String("FIELD",
		"NAME"		, _TL("Field Name"),
		_TL(""),
		_TL("Calculation")
	);

	Parameters.Add_String("",
		"FORMULA"	, _TL("Formula"),
		_TL(""),
		"f1 + f2"
	);

	Parameters.Add_Choice("",
		"FIELD_SELECTOR", _TL("Add Field to Formula"),
		_TL("Convenient way to append a field number to the formula. Shows one all numeric fields with number, name and datatype overview."),
		""
	)->Set_UseInCMD(false);

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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Field_Calculator::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_String Fields; CSG_Table *pTable = pParameters->Get_Parameter("TABLE")->asTable();

		if( pTable ) // have a table? then populate the field selector choices...
		{
			Fields.Printf("<%s>", _TL("select from numeric fields"));

			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
				{
					Fields += CSG_String::Format("|{%d}f%d [%s]", i + 1, // put choice data in first place in curly brackets {data}
						i + 1, pTable->Get_Field_Name(i)
					);
				}
			}
		}

		pParameters->Get_Parameter("FIELD_SELECTOR")->asChoice()->Set_Items(Fields);
	}

	if( pParameter->Cmp_Identifier("FIELD_SELECTOR") )
	{
		int Field;

		if( pParameter->asChoice()->Get_Data(Field) )
		{
			pParameters->Set_Parameter("FORMULA", CSG_String::Format("%s f%d", (*pParameters)["FORMULA"].asString(), Field));

			pParameter->Set_Value(0);
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CTable_Field_Calculator::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Table *pTable = (CSG_Table *)pParameters->Get_Parameter("TABLE")->asDataObject();

	if( pParameter->Cmp_Identifier("TABLE") )
	{
		On_Parameter_Changed(pParameters, pParameter);
	}

	if( pTable )
	{

		CSG_Parameter *pField = pParameters->Get_Parameter("FIELD");

		pParameters->Set_Enabled("RESULT_TABLE"  , pTable->asShapes() == NULL);
		pParameters->Set_Enabled("RESULT_SHAPES" , pTable->asShapes() != NULL);
		pParameters->Set_Enabled("FIELD"         , true);
		pParameters->Set_Enabled("NAME"          , pField->asInt() < 0); // not set
		pParameters->Set_Enabled("SELECTION"     , pTable->Get_Selection_Count() > 0);
		pParameters->Set_Enabled("FIELD_SELECTOR", true);
	}
	else
	{
		pParameters->Set_Enabled("RESULT_TABLE"  , false);
		pParameters->Set_Enabled("RESULT_SHAPES" , false);
		pParameters->Set_Enabled("FIELD"    	 , false);
		pParameters->Set_Enabled("NAME"     	 , false);
		pParameters->Set_Enabled("FIELD_SELECTOR", false);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Field_Calculator::On_Execute(void)
{
	CSG_Table *pTable = Parameters("TABLE")->asTable();

	if( !pTable->is_Valid() || pTable->Get_Field_Count() <= 0 || pTable->Get_Count() <= 0 )
	{
		Error_Set(_TL("invalid table"));

		return( false );
	}

	//-----------------------------------------------------
	if( !m_Formula.Set_Formula(Get_Formula(Parameters("FORMULA")->asString(), pTable, m_Fields)) )
	{
		CSG_String Message; m_Formula.Get_Error(Message); Error_Set(Message);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table *pResult = Parameters(pTable->asShapes() ? "RESULT_SHAPES" : "RESULT_TABLE")->asTable();

	if( pResult && pResult != pTable )
	{
		if( pResult->asShapes() )
		{
			pResult->Create(*pTable->asShapes());
		}
		else
		{
			pResult->Create(*pTable);
		}

		pTable = pResult;
	}

	//-----------------------------------------------------
	m_Result = Parameters("FIELD")->asInt();

	if( m_Result < 0 || m_Result >= pTable->Get_Field_Count() )
	{
		m_Result = pTable->Get_Field_Count();

		pTable->Add_Field(Parameters("NAME")->asString(), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	m_bNoData = Parameters("USE_NODATA")->asBool();

	g_NoData_loValue = pTable->Get_NoData_Value(false);
	g_NoData_hiValue = pTable->Get_NoData_Value(true );

	if( pTable->Get_Selection_Count() > 0 && Parameters("SELECTION")->asBool() )
	{
		for(sLong i=0; i<pTable->Get_Selection_Count() && Set_Progress(i, pTable->Get_Selection_Count()); i++)
		{
			Get_Value(pTable->Get_Selection(i));
		}
	}
	else
	{
		for(sLong i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Field_Calculator::Get_Value(CSG_Table_Record *pRecord)
{
	CSG_Vector Values(m_Fields.Get_uSize());

	bool bNoData = false;

	for(sLong i=0; i<m_Fields.Get_Size(); i++)
	{
		Values[i] = pRecord->asDouble(m_Fields[i]);

		if( !m_bNoData && pRecord->is_NoData(m_Fields[i]) )
		{
			bNoData = true;
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CTable_Field_Calculator::Get_Formula(CSG_String Formula, CSG_Table *pTable, CSG_Array_Int &Fields)
{
	const SG_Char vars[27] = SG_T("abcdefghijklmnopqrstuvwxyz");

	Fields.Destroy();

	for(int Field=pTable->Get_Field_Count()-1; Field>=0 && Fields.Get_Size()<26; Field--)
	{
		bool bUse = false;

		#define FINDVAR(fmt, val) { CSG_String s; s.Printf(fmt, val); if( Formula.Find(s) >= 0 )\
			{ Formula.Replace(s, CSG_String(vars[Fields.Get_Size()])); bUse = true; }\
		}

		FINDVAR("f%d", Field + 1);
		FINDVAR("F%d", Field + 1);
		FINDVAR( "[%s]" , pTable->Get_Field_Name(Field));
		FINDVAR("\"%s\"", pTable->Get_Field_Name(Field));

		if( bUse )
		{
			Fields += Field;
		}
	}

	return( Formula );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
