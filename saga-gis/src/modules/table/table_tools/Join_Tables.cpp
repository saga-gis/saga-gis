/**********************************************************
 * Version $Id: Join_Tables.cpp 1010 2011-04-26 11:52:02Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Table_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Join_Tables.cpp                     //
//                                                       //
//                 Copyright (C) 2005 by                 //
//              Olaf Conrad  & Victor Olaya              //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "Join_Tables.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CJoin_Tables_Base::Initialise(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Author		(SG_T("V.Olaya (c) 2005, O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Joins two tables using key attributes."
	));


	//-----------------------------------------------------
	pNode	= Parameters("TABLE_A");

	Parameters.Add_Table_Field(
		pNode	, "ID_A"		, _TL("Identifier"),
		_TL("")
	);

	pNode	= Parameters("TABLE_B");

	Parameters.Add_Table_Field(
		pNode	, "ID_B"		, _TL("Identifier"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "KEEPALL"		, _TL("Keep All"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CJoin_Tables_Base::On_Execute(void)
{
	bool		bKeepAll;
	int			id_A, id_B, off_Field;
	CSG_Table	*pTable_A, *pTable_B;

	//-----------------------------------------------------
	pTable_A	= Parameters("TABLE_A")	->asTable();
	id_A		= Parameters("ID_A")	->asInt();

	pTable_B	= Parameters("TABLE_B")	->asTable();
	id_B		= Parameters("ID_B")	->asInt();

	bKeepAll	= Parameters("KEEPALL")	->asBool();

	//-----------------------------------------------------
	if(	id_A < 0 || id_A >= pTable_A->Get_Field_Count() || pTable_A->Get_Count() <= 0
	||	id_B < 0 || id_B >= pTable_B->Get_Field_Count() || pTable_B->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable_A )
	{
		pTable_A	= Parameters("RESULT")->asTable();

		if( Parameters("RESULT")->asTable()->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
		{
			((CSG_Shapes *)pTable_A)->Create(*((CSG_Shapes *)Parameters("TABLE_A")->asTable()));
		}
		else
		{
			pTable_A->Create(*Parameters("TABLE_A")->asTable());
		}
	}

	//-----------------------------------------------------
	off_Field	= pTable_A->Get_Field_Count();

	for(int iField=0; iField<pTable_B->Get_Field_Count(); iField++)
	{
		if( iField != id_B )
		{
			pTable_A->Add_Field(pTable_B->Get_Field_Name(iField), pTable_B->Get_Field_Type(iField));
		}
	}

	pTable_A->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pTable_A->Get_Name(), pTable_B->Get_Name()));

	pTable_A->Select();	// clear selection

	//-------------------------------------------------
	for(int iA=0; iA<pTable_A->Get_Count() && Set_Progress(iA, pTable_A->Get_Count()); iA++)
	{
		CSG_Table_Record	*pRecord_A	= pTable_A->Get_Record(iA);
		CSG_Table_Record	*pRecord_B	= NULL;
		CSG_String			Key			= pRecord_A->asString(id_A);

		for(int iB=0; iB<pTable_B->Get_Count() && !pRecord_B; iB++)
		{
			if( !Key.CmpNoCase(pTable_B->Get_Record(iB)->asString(id_B)) )
			{
				pRecord_B	= pTable_B->Get_Record(iB);

				for(int Field_A=off_Field, Field_B=0; Field_B<pTable_B->Get_Field_Count(); Field_B++)
				{
					if( Field_B != id_B )
					{
						switch( pTable_A->Get_Field_Type(Field_A) )
						{
						default:
						case SG_DATATYPE_String:
						case SG_DATATYPE_Date:
							pRecord_A->Set_Value(Field_A++, pRecord_B->asString(Field_B));
							break;

						case SG_DATATYPE_Bit:
						case SG_DATATYPE_Byte:
						case SG_DATATYPE_Char:
						case SG_DATATYPE_Word:
						case SG_DATATYPE_Short:
						case SG_DATATYPE_DWord:
						case SG_DATATYPE_Int:
						case SG_DATATYPE_ULong:
						case SG_DATATYPE_Long:
						case SG_DATATYPE_Color:
							pRecord_A->Set_Value(Field_A++, pRecord_B->asInt(Field_B));
							break;

						case SG_DATATYPE_Float:
						case SG_DATATYPE_Double:
							pRecord_A->Set_Value(Field_A++, pRecord_B->asDouble(Field_B));
							break;

						case SG_DATATYPE_Binary:
							pRecord_A->Get_Value(Field_A++)->Set_Value(pRecord_B->Get_Value(Field_B)->asBinary());
							break;
						}
					}
				}
			}
		}

		//-------------------------------------------------
		if( pRecord_B == NULL && bKeepAll == false )
		{
			pTable_A->Select(iA, true);
		}
	}

	//-----------------------------------------------------
	if( pTable_A->Get_Selection_Count() > 0 )
	{
		Message_Add(CSG_String::Format(SG_T("%d %s"), pTable_A->Get_Selection_Count(), _TL("unjoined records have been removed")));

		pTable_A->Del_Selection();
	}

	if( pTable_A == Parameters("TABLE_A")->asTable() )
	{
		DataObject_Update(pTable_A);
	}

	return( pTable_A->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CJoin_Tables::CJoin_Tables(void)
{
	Set_Name		(_TL("Join Attributes from a Table"));

	Parameters.Add_Table(
		NULL	, "TABLE_A"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "TABLE_B"		, _TL("Join Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CJoin_Tables_Shapes::CJoin_Tables_Shapes(void)
{
	Set_Name		(_TL("Join Attributes from a Table (Shapes)"));

	Parameters.Add_Shapes(
		NULL	, "TABLE_A"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "TABLE_B"		, _TL("Join Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Initialise();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Append_Rows::CTable_Append_Rows(void)
{
	Set_Name		(_TL("Append Rows from Table"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description(_TW(
		""
	));

	Parameters.Add_Table(
		NULL	, "INPUT"	, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "APPEND"	, _TL("Append Rows from ..."),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Append_Rows::On_Execute(void)
{
	CSG_Table	*pTable, *pOutput, *pAppend;

	//-----------------------------------------------------
	pTable	= Parameters("INPUT" )->asTable();
	pOutput	= Parameters("OUTPUT")->asTable();
	pAppend	= Parameters("APPEND")->asTable();

	//-----------------------------------------------------
	if( pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("no records in data set"));

		return( false );
	}

	if( pAppend->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("no records to append"));

		return( false );
	}

	//-----------------------------------------------------
	if( pOutput && pOutput != pTable )
	{
		pOutput->Create		(*pTable);
		pOutput->Set_Name	( pTable->Get_Name());
		pTable	= pOutput;
	}

	//-----------------------------------------------------
	int		iField, jField, aField, nRecords;

	nRecords	= pTable->Get_Count() < pAppend->Get_Count() ? pTable->Get_Count() : pAppend->Get_Count();
	aField		= pTable->Get_Field_Count();

	for(iField=0; iField<pAppend->Get_Field_Count(); iField++)
	{
		pTable->Add_Field(pAppend->Get_Field_Name(iField), pAppend->Get_Field_Type(iField));
	}

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<nRecords && Set_Progress(iRecord, nRecords); iRecord++)
	{
		CSG_Table_Record	*pRec	= pTable ->Get_Record(iRecord);
		CSG_Table_Record	*pAdd	= pAppend->Get_Record(iRecord);

		for(iField=0, jField=aField; iField<pAppend->Get_Field_Count(); iField++)
		{
			*pRec->Get_Value(jField++)	= *pAdd->Get_Value(iField);
		}
	}

	//-----------------------------------------------------
	if( pTable == Parameters("INPUT")->asTable() )
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
