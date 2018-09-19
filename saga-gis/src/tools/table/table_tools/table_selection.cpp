/**********************************************************
 * Version $Id: table_selection.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table_selection.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Copy::CSelection_Copy(void)
{	
	Set_Name		(_TL("Copy Selection"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Copies selected records to a new table."
	));

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"), 
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "OUT_TABLE"	, _TL("Copied Selection"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "OUT_SHAPES"	, _TL("Copied Selection"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
int CSelection_Copy::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TABLE") )
	{
		CSG_Data_Object	*pObject	= pParameter->asDataObject();

		pParameters->Get_Parameter("OUT_TABLE" )->Set_Enabled(pObject &&
			pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Table
		);

		pParameters->Get_Parameter("OUT_SHAPES")->Set_Enabled(pObject &&
			pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
		);
	}

	return( 1 );
}

//---------------------------------------------------------
bool CSelection_Copy::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pInput  = Parameters("TABLE")->asTable();

	if( pInput->Get_Selection_Count() <= 0 )
	{
		Error_Set(_TL("no records in selection"));

		return( false );
	}

	//-----------------------------------------------------
	if( pInput->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
	{
		CSG_Shapes	*pOutput	= Parameters("OUT_SHAPES")->asShapes();

		if( !pOutput || pOutput->Get_Type() == SHAPE_TYPE_Undefined )
		{
			return( false );
		}

		pOutput->Create(((CSG_Shapes *)pInput)->Get_Type(), SG_T(""), pInput);
		pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("Selection")));

		for(int i=0; i<pInput->Get_Selection_Count() && Set_Progress(i, pInput->Get_Selection_Count()); i++)
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
		pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("Selection")));

		for(int i=0; i<pInput->Get_Selection_Count() && Set_Progress(i, pInput->Get_Selection_Count()); i++)
		{
			pOutput->Add_Record(pInput->Get_Selection(i));
		}
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
CSelection_Delete::CSelection_Delete(void)
{	
	Set_Name		(_TL("Delete Selection"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Deletes selected records from table."
	));

	Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool CSelection_Delete::On_Execute(void)
{
	CSG_Table	*pInput	= Parameters("INPUT")->asTable();

	if( pInput->Get_Selection_Count() <= 0 )
	{
		Error_Set(_TL("no records in selection"));

		return( false );
	}

	pInput->Del_Selection();

	DataObject_Update(pInput);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelection_Invert::CSelection_Invert(void)
{	
	Set_Name		(_TL("Invert Selection"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Deselects selected and selects unselected records of given table."
	));

	Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Input"), 
		_TL(""),
		PARAMETER_INPUT
	);
}

//---------------------------------------------------------
bool CSelection_Invert::On_Execute(void)
{
	CSG_Table	*pInput	= Parameters("INPUT")->asTable();

	pInput->Inv_Selection();

	DataObject_Update(pInput);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_Numeric::CSelect_Numeric(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Select by Numerical Expression"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Selects records for which the expression is true."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("attribute to be searched; if not set all attributes will be searched"),
		true
	);

	Parameters.Add_String(
		NULL	, "EXPRESSION"	, _TL("Expression"),
		_TL(""),
		SG_T("a > 0")
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);
}

//---------------------------------------------------------
bool CSelect_Numeric::On_Execute(void)
{
	int				Method, Field;
	CSG_String		Expression;
	CSG_Table		*pTable;
	CSG_Formula		Formula;

	//-----------------------------------------------------
	pTable		= Parameters("TABLE"     )->asTable();
	Field		= Parameters("FIELD"     )->asInt();
	Expression	= Parameters("EXPRESSION")->asString();
	Method		= Parameters("METHOD"    )->asInt();

	//-----------------------------------------------------
	if( !Formula.Set_Formula(Expression) )
	{
		CSG_String	Message;

		if( Formula.Get_Error(Message) )
		{
			Error_Set(Message);
		}

		return( false );
	}

	//-----------------------------------------------------
	double	*Values	= new double[pTable->Get_Field_Count()];

	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		if( Field >= pTable->Get_Field_Count() )
		{
			for(int j=0; j<pTable->Get_Field_Count(); j++)
			{
				Values[j]	= pRecord->asDouble(j);
			}
		}
		else
		{
			Values[0]	= pRecord->asDouble(Field);
		}

		switch( Method )
		{
		case 0:	// New selection
			if( ( pRecord->is_Selected() && !Formula.Get_Value(Values, pTable->Get_Field_Count()))
			||	(!pRecord->is_Selected() &&  Formula.Get_Value(Values, pTable->Get_Field_Count())) )
			{
				pTable->Select(i, true);
			}
			break;

		case 1:	// Add to current selection
			if(  !pRecord->is_Selected() &&  Formula.Get_Value(Values, pTable->Get_Field_Count()) )
			{
				pTable->Select(i, true);
			}
			break;

		case 2:	// Select from current selection
			if(   pRecord->is_Selected() && !Formula.Get_Value(Values, pTable->Get_Field_Count()) )
			{
				pTable->Select(i, true);
			}
			break;

		case 3:	// Remove from current selection
			if(   pRecord->is_Selected() &&  Formula.Get_Value(Values, pTable->Get_Field_Count()) )
			{
				pTable->Select(i, true);
			}
			break;
		}
	}

	delete[](Values);

	//-----------------------------------------------------
	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("selected records"), pTable->Get_Selection_Count()));

	DataObject_Update(pTable);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSelect_String::CSelect_String(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Select by String Expression"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Searches for an character string expression in the attributes table and selects records where the expression is found."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		_TL("attribute to be searched; if not set all attributes will be searched"),
		true
	);

	Parameters.Add_String(
		NULL	, "EXPRESSION"	, _TL("Expression"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_Value(
		NULL	, "CASE"		, _TL("Case Sensitive"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		NULL	, "COMPARE"		, _TL("Select if..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("attribute is identical with search expression"),
			_TL("attribute contains search expression"),
			_TL("attribute is contained in search expression")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("new selection"),
			_TL("add to current selection"),
			_TL("select from current selection"),
			_TL("remove from current selection")
		), 0
	);
}

//---------------------------------------------------------
bool CSelect_String::On_Execute(void)
{
	int			Method;
	CSG_Table	*pTable;

	//-----------------------------------------------------
	pTable			= Parameters("TABLE"     )->asTable();
	m_Field			= Parameters("FIELD"     )->asInt();
	m_Expression	= Parameters("EXPRESSION")->asString();
	m_Case			= Parameters("CASE"      )->asBool();
	m_Compare		= Parameters("COMPARE"   )->asInt();
	Method			= Parameters("METHOD"    )->asInt();

	//-----------------------------------------------------
	if( m_Case == false )
	{
		m_Expression.Make_Upper();
	}

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		switch( Method )
		{
		case 0:	// New selection
			if( ( pRecord->is_Selected() && !Do_Select(pRecord))
			||	(!pRecord->is_Selected() &&  Do_Select(pRecord)) )
			{
				pTable->Select(i, true);
			}
			break;

		case 1:	// Add to current selection
			if(  !pRecord->is_Selected() &&  Do_Select(pRecord) )
			{
				pTable->Select(i, true);
			}
			break;

		case 2:	// Select from current selection
			if(   pRecord->is_Selected() && !Do_Select(pRecord) )
			{
				pTable->Select(i, true);
			}
			break;

		case 3:	// Remove from current selection
			if(   pRecord->is_Selected() &&  Do_Select(pRecord) )
			{
				pTable->Select(i, true);
			}
			break;
		}
	}

	//-----------------------------------------------------
	Message_Add(CSG_String::Format(SG_T("%s: %d"), _TL("selected records"), pTable->Get_Selection_Count()));

	DataObject_Update(pTable);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSelect_String::Do_Compare(const SG_Char *Value)
{
	CSG_String	s(Value);

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
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
