/**********************************************************
 * Version $Id: Join_Tables.cpp 1010 2011-04-26 11:52:02Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
	Set_Author		("V.Olaya (c) 2005, O.Conrad (c) 2011");

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

	Parameters.Add_Bool(
		pNode	, "FIELDS_ALL"	, _TL("Add All Fields"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Fields"),
		_TL("")
	);

	Parameters.Add_Bool(
		NULL	, "KEEP_ALL"	, _TL("Keep All"),
		_TL(""),
		true
	);

	Parameters.Add_Bool(
		NULL	, "CMP_CASE"	, _TL("Case Sensitive String Comparison"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CJoin_Tables_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "FIELDS_ALL") )
	{
		pParameters->Get_Parameter("FIELDS")->Set_Enabled(pParameter->asBool() == false);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CJoin_Tables_Base::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pT_A	= Parameters("TABLE_A")->asTable();
	int			 id_A	= Parameters(   "ID_A")->asInt();

	CSG_Table	*pT_B	= Parameters("TABLE_B")->asTable();
	int			 id_B	= Parameters(   "ID_B")->asInt();

	if(	id_A < 0 || id_A >= pT_A->Get_Field_Count() || pT_A->Get_Count() <= 0
	||	id_B < 0 || id_B >= pT_B->Get_Field_Count() || pT_B->Get_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pT_A )
	{
		pT_A	= Parameters("RESULT")->asTable();

		if( Parameters("RESULT")->asTable()->Get_ObjectType() == DATAOBJECT_TYPE_Shapes )
		{
			((CSG_Shapes *)pT_A)->Create(*Parameters("TABLE_A")->asShapes());
		}
		else
		{
			pT_A->Create(*Parameters("TABLE_A")->asTable());
		}
	}

	//-----------------------------------------------------
	int		nJoins, *Join, Offset	= pT_A->Get_Field_Count();

	if( Parameters("FIELDS_ALL")->asBool() )
	{
		if( (nJoins = pT_B->Get_Field_Count() - 1) <= 0 )
		{
			Error_Set(_TL("no fields to add"));

			return( false );
		}

		Join	= new int[nJoins];

		for(int i=0, j=0; i<pT_B->Get_Field_Count(); i++)
		{
			if( i != id_B )
			{
				pT_A->Add_Field(pT_B->Get_Field_Name(i), pT_B->Get_Field_Type(i));

				Join[j++]	= i;
			}
		}
	}
	else
	{
		CSG_Parameter_Table_Fields	*pFields	= Parameters("FIELDS")->asTableFields();

		if( (nJoins = pFields->Get_Count()) <= 0 )
		{
			Error_Set(_TL("no fields to add"));

			return( false );
		}

		Join	= new int[nJoins];

		for(int j=0; j<pFields->Get_Count(); j++)
		{
			int	i	= pFields->Get_Index(j);

			pT_A->Add_Field(pT_B->Get_Field_Name(i), pT_B->Get_Field_Type(i));

			Join[j]	= i;
		}
	}

	pT_A->Set_Name(CSG_String::Format("%s [%s]", pT_A->Get_Name(), pT_B->Get_Name()));

	//-----------------------------------------------------
	m_bCmpNoCase	= Parameters("CMP_CASE")->asBool() == false;

	m_bCmpNumeric	=  SG_Data_Type_is_Numeric(pT_A->Get_Field_Type(id_A))
					|| SG_Data_Type_is_Numeric(pT_B->Get_Field_Type(id_B));

	CSG_Table	Delete;	if( !Parameters("KEEP_ALL")->asBool() )	Delete.Add_Field("ID", SG_DATATYPE_Int);

	pT_A->Set_Index(id_A, TABLE_INDEX_Ascending);
	pT_B->Set_Index(id_B, TABLE_INDEX_Ascending);

	CSG_Table_Record	*pRecord_B	= pT_B->Get_Record_byIndex(0);

	for(int a=0, b=0, Cmp; pRecord_B && a<pT_A->Get_Count() && Set_Progress(a, pT_A->Get_Count()); a++)
	{
		CSG_Table_Record	*pRecord_A	= pT_A->Get_Record_byIndex(a);

		while( pRecord_B && (Cmp = Cmp_Keys(pRecord_A->Get_Value(id_A), pRecord_B->Get_Value(id_B))) < 0 )
		{
			pRecord_B	= pT_B->Get_Record_byIndex(++b);
		}

		if( pRecord_B && Cmp == 0 )
		{
			for(int i=0; i<nJoins; i++)
			{
				*pRecord_A->Get_Value(Offset + i)	= *pRecord_B->Get_Value(Join[i]);
			}
		}
		else if( Delete.Get_Field_Count() == 0 )
		{
			for(int i=0; i<nJoins; i++)
			{
				pRecord_A->Set_NoData(Offset + i);
			}
		}
		else
		{
			Delete.Add_Record()->Set_Value(0, pRecord_A->Get_Index());
		}
	}

	//-----------------------------------------------------
	delete[](Join);

	pT_A->Set_Index(id_A, TABLE_INDEX_None);
	pT_B->Set_Index(id_B, TABLE_INDEX_None);

	if( Delete.Get_Count() > 0 )
	{
		Delete.Set_Index(0, TABLE_INDEX_Descending);

		for(int i=0; i<Delete.Get_Count(); i++)
		{
		//	((CSG_Shapes *)pT_A)->Del_Shape(Delete[i].asInt(0));

			pT_A->Del_Record(Delete[i].asInt(0));
		}

		Message_Add(CSG_String::Format("%d %s", pT_A->Get_Selection_Count(), _TL("unjoined records have been removed")));
	}

	if( pT_A == Parameters("TABLE_A")->asTable() )
	{
		DataObject_Update(pT_A);
	}

	return( pT_A->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CJoin_Tables_Base::Cmp_Keys(CSG_Table_Value *pA, CSG_Table_Value *pB)
{
	if( pB == NULL )
	{
		return( 1 );
	}

	if( m_bCmpNumeric )
	{
		double	d	= pB->asDouble() - pA->asDouble();

		return( d < 0.0 ? -1 : d > 0.0 ? 1 : 0 );
	}

	CSG_String	Key(pB->asString());

	return( m_bCmpNoCase ? Key.CmpNoCase(pA->asString()) : Key.Cmp(pA->asString()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
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
CTable_Append_Cols::CTable_Append_Cols(void)
{
	Set_Name		(_TL("Append Fields from another Table"));

	Set_Author		("O.Conrad (c) 2012");

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
bool CTable_Append_Cols::On_Execute(void)
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
