
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
//                  table_selection.cpp                  //
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
#include "table_selection.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Copy::CSelection_Copy(void)
{	
	Set_Name		(_TL("Copy Selection"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Copies selected records to a new table."
	));

	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"), 
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"OUT_TABLE"	, _TL("Copied Selection"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes("",
		"OUT_SHAPES", _TL("Copied Selection"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
int CSelection_Copy::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_Data_Object *pObject = pParameter->asDataObject();

		pParameters->Set_Enabled("OUT_TABLE" , pObject && pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table );
		pParameters->Set_Enabled("OUT_SHAPES", pObject && pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CSelection_Copy::On_Execute(void)
{
	CSG_Table *pInput = Parameters("TABLE")->asTable();

	if( pInput->Get_Selection_Count() <= 0 )
	{
		Error_Set(_TL("no records in selection"));

		return( false );
	}

	//-----------------------------------------------------
	if( pInput->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
	{
		CSG_Shapes *pOutput = Parameters("OUT_SHAPES")->asShapes();

		if( !pOutput || pOutput->Get_Type() == SHAPE_TYPE_Undefined )
		{
			return( false );
		}

		pOutput->Create(((CSG_Shapes *)pInput)->Get_Type(), SG_T(""), pInput);
		pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Selection"));

		for(sLong i=0; i<pInput->Get_Selection_Count() && Set_Progress(i, pInput->Get_Selection_Count()); i++)
		{
			pOutput->Add_Shape(pInput->Get_Selection(i));
		}
	}

	//-----------------------------------------------------
	else // if( pInput->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table )
	{
		CSG_Table	*pOutput	= Parameters("OUT_TABLE" )->asTable();

		if( !pOutput )
		{
			return( false );
		}
			
		pOutput->Create(pInput);
		pOutput->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Selection"));

		for(sLong i=0; i<pInput->Get_Selection_Count() && Set_Progress(i, pInput->Get_Selection_Count()); i++)
		{
			pOutput->Add_Record(pInput->Get_Selection(i));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Delete::CSelection_Delete(void)
{	
	Set_Name		(_TL("Delete Selection"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Deletes selected records from table."
	));

	Parameters.Add_Table("",
		"INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool CSelection_Delete::On_Execute(void)
{
	CSG_Table *pInput = Parameters("INPUT")->asTable();

	if( pInput->Get_Selection_Count() < 1 )
	{
		Error_Set(_TL("no records in selection"));

		return( false );
	}

	pInput->Del_Selection();

	DataObject_Update(pInput);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Invert::CSelection_Invert(void)
{	
	Set_Name		(_TL("Invert Selection"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Deselects selected and selects unselected records of given table."
	));

	Parameters.Add_Table("",
		"INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool CSelection_Invert::On_Execute(void)
{
	CSG_Table *pInput = Parameters("INPUT")->asTable();

	pInput->Inv_Selection();

	DataObject_Update(pInput);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Numeric::CSelect_Numeric(void)
{
	Set_Name		(_TL("Select by Numerical Expression"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Selects records for which the expression evaluates to non-zero. "
		"The expression syntax is the same as the one for the table calculator. "
		"If an attribute field is selected, the expression evaluates only "
		"this attribute, which can be addressed with the letter 'x' in the "
		"expression formula. If no attribute is selected, attributes are addressed "
		"by the character 'f' (for 'field') followed by the field number "
		"(i.e.: f1, f2, ..., fn) or by the field in quota (e.g.: \"Field Name\").\n"
		"Examples:<ul>"
		"<li>f1 > f2</li>"
		"<li>eq(\"Population\" * 0.001, \"Area\")</li>"
		"</ul>"
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"     , _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"     , _TL("Attribute"),
		_TL("attribute to be searched; if not set all attributes will be searched"),
		true
	);

	Parameters.Add_String("",
		"EXPRESSION", _TL("Expression"),
		_TL(""),
		"f1 > 0"
	);

	Parameters.Add_Bool("EXPRESSION",
		"INVERSE"   , _TL("Inverse"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"USE_NODATA", _TL("Use No-Data"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"METHOD"    , _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);

	Parameters.Add_Choice("",
		"POSTJOB"   , _TL("Post Job"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("none"),
			_TL("copy"),
			_TL("move"),
			_TL("delete")
		), 0
	);

	Parameters.Add_Shapes("",
		"COPY"      , _TL("Copy"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
int CSelect_Numeric::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POSTJOB") )
	{
		pParameters->Set_Enabled("COPY", pParameter->asInt() == 1 || pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CSelect_Numeric::On_Execute(void)
{
	CSG_Table *pTable = Parameters("TABLE")->asTable();

	if( pTable->Get_Count() < 1 || pTable->Get_Field_Count() < 1 )
	{
		Error_Set(_TL("empty or invalid shapes layer"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Formula Formula; CSG_Array_Int Fields;

	if( !Formula.Set_Formula(Get_Formula(Parameters("EXPRESSION")->asString(), pTable, Fields)) )
	{
		CSG_String Message;

		if( Formula.Get_Error(Message) )
		{
			Error_Set(Message);
		}

		return( false );
	}

	//-----------------------------------------------------
	int     Method = Parameters("METHOD"    )->asInt ();
	bool   Inverse = Parameters("INVERSE"   )->asBool();
	bool UseNoData = Parameters("USE_NODATA")->asBool();
	bool    Single = Parameters("FIELD"     )->asInt () >= 0;

	CSG_Vector Values((int)Fields.Get_Size());

	//-----------------------------------------------------
	for(sLong i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record *pRecord = pTable->Get_Record(i);

		bool bOkay = true;

		for(sLong Field=0; bOkay && Field<Fields.Get_Size(); Field++)
		{
			if( (bOkay = UseNoData || !pRecord->is_NoData(Fields[Field])) == true )
			{
				Values[Field] = pRecord->asDouble(Fields[Field]);
			}
		}

		//-------------------------------------------------
		if( bOkay )
		{
			bool bSelect = (Single ? Formula.Get_Value(Values[0]) : Formula.Get_Value(Values)) ? !Inverse : Inverse;

			switch( Method )
			{
			default: // New selection
				if( ( pRecord->is_Selected() && !bSelect)
				||	(!pRecord->is_Selected() &&  bSelect) )
				{
					pTable->Select(i, true);
				}
				break;

			case  1: // Add to current selection
				if(  !pRecord->is_Selected() &&  bSelect )
				{
					pTable->Select(i, true);
				}
				break;

			case  2: // Select from current selection
				if(   pRecord->is_Selected() && !bSelect )
				{
					pTable->Select(i, true);
				}
				break;

			case  3: // Remove from current selection
				if(   pRecord->is_Selected() &&  bSelect )
				{
					pTable->Select(i, true);
				}
				break;
			}
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %lld", _TL("selected records"), pTable->Get_Selection_Count());

	if( Parameters("POSTJOB")->asInt() == 1 || Parameters("POSTJOB")->asInt() == 2 ) // copy
	{
		CSG_Table *pCopy = Parameters("COPY")->asTable(); pCopy->Create(pTable); pCopy->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Selection"));

		for(sLong i=0; i<pTable->Get_Selection_Count() && Set_Progress(i, pTable->Get_Selection_Count()); i++)
		{
			pCopy->Add_Record(pTable->Get_Selection(i));
		}
	}

	if( Parameters("POSTJOB")->asInt() == 2 || Parameters("POSTJOB")->asInt() == 3 ) // delete
	{
		pTable->Del_Selection();
	}

	DataObject_Update(pTable);

	return( true );
}

//---------------------------------------------------------
CSG_String CSelect_Numeric::Get_Formula(CSG_String Formula, CSG_Table *pTable, CSG_Array_Int &Fields)
{
	Fields.Destroy();

	if(	Parameters("FIELD")->asInt() >= 0 )
	{
		Fields += Parameters("FIELD")->asInt();

		Formula.Replace_Single_Char('a', 'x');

		return( Formula );
	}

	//---------------------------------------------------------
	const SG_Char vars[27] = SG_T("abcdefghijklmnopqrstuvwxyz");

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
CSelect_String::CSelect_String(void)
{
	Set_Name		(_TL("Select by String Expression"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Searches for an character string expression in the attributes table and selects records where the expression is found."
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"     , _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"     , _TL("Attribute"),
		_TL("attribute to be searched; if not set all attributes will be searched"),
		true
	);

	Parameters.Add_String("",
		"EXPRESSION", _TL("Expression"),
		_TL(""),
		""
	);

	Parameters.Add_Bool("",
		"CASE"      , _TL("Case Sensitive"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"COMPARE"   , _TL("Select if..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("attribute is identical with search expression"),
			_TL("attribute contains search expression"),
			_TL("attribute is contained in search expression")
		), 1
	);

	Parameters.Add_Bool("COMPARE",
		"INVERSE"   , _TL("Inverse"),
		_TL(""),
		false
	);

	Parameters.Add_Choice("",
		"METHOD"    , _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);

	Parameters.Add_Choice("",
		"POSTJOB"   , _TL("Post Job"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("none"),
			_TL("copy"),
			_TL("move"),
			_TL("delete")
		), 0
	);

	Parameters.Add_Table("",
		"COPY"      , _TL("Copy"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
int CSelect_String::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POSTJOB") )
	{
		pParameters->Set_Enabled("COPY", pParameter->asInt() == 1 || pParameter->asInt() == 2);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CSelect_String::On_Execute(void)
{
	CSG_Table *pTable = Parameters("TABLE")->asTable();

	m_Field      = Parameters("FIELD"     )->asInt   ();
	m_Expression = Parameters("EXPRESSION")->asString();
	m_Case       = Parameters("CASE"      )->asBool  ();
	m_Compare    = Parameters("COMPARE"   )->asInt   ();
	int	  Method = Parameters("METHOD"    )->asInt   ();
	bool Inverse = Parameters("INVERSE"   )->asBool  ();

	//-----------------------------------------------------
	if( m_Case == false )
	{
		m_Expression.Make_Upper();
	}

	//-----------------------------------------------------
	for(sLong i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record *pRecord = pTable->Get_Record(i);

		bool bSelect = Do_Select(pRecord) ? !Inverse : Inverse;

		switch( Method )
		{
		default: // New selection
			if( ( pRecord->is_Selected() && !bSelect)
			||  (!pRecord->is_Selected() &&  bSelect) )
			{
				pTable->Select(i, true);
			}
			break;

		case  1: // Add to current selection
			if(  !pRecord->is_Selected() &&  bSelect )
			{
				pTable->Select(i, true);
			}
			break;

		case  2: // Select from current selection
			if(   pRecord->is_Selected() && !bSelect )
			{
				pTable->Select(i, true);
			}
			break;

		case  3: // Remove from current selection
			if(   pRecord->is_Selected() &&  bSelect )
			{
				pTable->Select(i, true);
			}
			break;
		}
	}

	//-----------------------------------------------------
	Message_Fmt("\n%s: %lld", _TL("selected records"), pTable->Get_Selection_Count());

	if( Parameters("POSTJOB")->asInt() == 1 || Parameters("POSTJOB")->asInt() == 2 ) // copy
	{
		CSG_Table *pCopy = Parameters("COPY")->asTable(); pCopy->Create(pTable); pCopy->Fmt_Name("%s [%s]", pTable->Get_Name(), _TL("Selection"));

		for(sLong i=0; i<pTable->Get_Selection_Count() && Set_Progress(i, pTable->Get_Selection_Count()); i++)
		{
			pCopy->Add_Record(pTable->Get_Selection(i));
		}
	}

	if( Parameters("POSTJOB")->asInt() == 2 || Parameters("POSTJOB")->asInt() == 3 ) // delete
	{
		pTable->Del_Selection();
	}

	DataObject_Update(pTable);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSelect_String::Do_Compare(const SG_Char *Value)
{
	CSG_String s(Value);

	if( m_Case == false )
	{
		s.Make_Upper();
	}

	switch( m_Compare )
	{
	case 0:	// identical
		return( m_Expression.Cmp(s) == 0 );

	case 1:	// contains
		return( s.Find(m_Expression) >= 0 );

	case 2:	// contained
		return( m_Expression.Find(s) >= 0 );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSelect_String::Do_Select(CSG_Table_Record *pRecord)
{
	if( m_Field >= 0 && m_Field < pRecord->Get_Table()->Get_Field_Count() )
	{
		return( Do_Compare(pRecord->asString(m_Field)) );
	}

	for(int i=0; i<pRecord->Get_Table()->Get_Field_Count(); i++)
	{
		if( Do_Compare(pRecord->asString(i)) )
		{
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
