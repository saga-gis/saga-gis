
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              table_aggregate_by_field.cpp             //
//                                                       //
//                 Copyright (C) 2017 by                 //
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
#include "table_aggregate_by_field.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Aggregate_by_Field::CTable_Aggregate_by_Field(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Aggregate Values by Attributes"));

	Set_Author		("O.Conrad (c) 2017");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"AGGREGATED", _TL("Output"),
		_TL("The calculated statistics."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table_Fields("TABLE",
		"FIELDS"	, _TL("Aggregate by Field(s)"),
		_TL("")
	);

	Parameters.Add_Table_Fields("TABLE",
		"STATISTICS", _TL("Statistics Field(s)"),
		_TL("")
	);

	Parameters.Add_Bool("STATISTICS", "STAT_SUM", _TL("Sum"      ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_AVG", _TL("Mean"     ), _TL(""), true );
	Parameters.Add_Bool("STATISTICS", "STAT_MIN", _TL("Minimum"  ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_MAX", _TL("Maximum"  ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_RNG", _TL("Range"    ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_DEV", _TL("Deviation"), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_VAR", _TL("Variance" ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_LST", _TL("Listing"  ), _TL(""), false);
	Parameters.Add_Bool("STATISTICS", "STAT_NUM", _TL("Count"    ), _TL(""), false);

	Parameters.Add_Choice("STATISTICS",
		"STAT_NAMING", _TL("Field Naming"),
		_TL(""), 
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("variable type + original name"),
			_TL("original name + variable type"),
			_TL("original name"),
			_TL("variable type")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Aggregate_by_Field::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("STATISTICS") )
	{
		pParameters->Set_Enabled("STAT_SUM", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_AVG", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_MIN", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_MAX", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_RNG", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_DEV", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_VAR", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_LST", pParameter->asInt() > 0);
		pParameters->Set_Enabled("STAT_NUM", pParameter->asInt() > 0);

		pParameters->Set_Enabled("STAT_NAMING", pParameter->asInt() > 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Aggregate_by_Field::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	if(	!pTable->is_Valid() || pTable->Get_Count() < 2 )
	{
		Error_Set(_TL("nothing to aggregate"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Table_Fields	&Fields	= *Parameters("FIELDS")->asTableFields();

	if( Fields.Get_Count() == 0 )
	{
		Error_Set(_TL("no aggregation fields in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pAggregated	= Parameters("AGGREGATED")->asTable();

	pAggregated->Destroy();

	CSG_Table	Aggregate;

	Aggregate.Add_Field("INDEX", SG_DATATYPE_Int   );
	Aggregate.Add_Field("VALUE", SG_DATATYPE_String);

	Aggregate.Set_Record_Count(pTable->Get_Count());

	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(i);

		CSG_String	Value;

		for(int iField=0; iField<Fields.Get_Count(); iField++)
		{
			Value	+= pRecord->asString(Fields.Get_Index(iField));
		}

		Aggregate[i].Set_Value(0, i);
		Aggregate[i].Set_Value(1, Value);
	}

	Aggregate.Set_Index(1, TABLE_INDEX_Ascending);

	//-----------------------------------------------------
	CSG_String	Name;

	for(int iField=0; iField<Fields.Get_Count(); iField++)
	{
		if( iField > 0 )
		{
			Name	+= "; ";
		}

		Name	+= pTable->Get_Field_Name(Fields.Get_Index(iField));

		pAggregated->Add_Field(
			pTable->Get_Field_Name(Fields.Get_Index(iField)),
			pTable->Get_Field_Type(Fields.Get_Index(iField))
		);
	}

	pAggregated->Fmt_Name("%s [%s: %s]", pTable->Get_Name(), _TL("Aggregated"), Name.c_str());

	//-----------------------------------------------------
	Statistics_Initialize(pAggregated, pTable);

	//-----------------------------------------------------
	CSG_String	Value;

	CSG_Table_Record	*pAggregate	= NULL;

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(!Aggregate.Get_Count() ? i : Aggregate[i].asInt(0));

		if( !pAggregate || (Aggregate.Get_Count() && Value.Cmp(Aggregate[i].asString(1))) )
		{
			Get_Aggregated(pAggregate);

			if( Aggregate.Get_Count() )
			{
				Value	= Aggregate[i].asString(1);
			}

			pAggregate	= pAggregated->Add_Record(pRecord);

			for(int iField=0; iField<Fields.Get_Count(); iField++)
			{
				*pAggregate->Get_Value(iField)	= *pRecord->Get_Value(Fields.Get_Index(iField));
			}

			Statistics_Add(pAggregate, pRecord, true);
		}
		else
		{
			Statistics_Add(pAggregate, pRecord, false);
		}
	}

	Get_Aggregated(pAggregate);

	//-----------------------------------------------------
	if( m_Statistics )
	{
		delete[](m_Statistics);
	}

	m_List.Clear();

	return( pAggregated->is_Valid() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Aggregate_by_Field::Get_Aggregated(CSG_Table_Record *pAggregate)
{
	if( !pAggregate )
	{
		return( false );
	}

	if( m_Statistics )
	{
		for(int iField=0, jField=m_Stat_Offset; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			if( m_bSUM ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Sum     ());
			if( m_bAVG ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Mean    ());
			if( m_bMIN ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Minimum ());
			if( m_bMAX ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Maximum ());
			if( m_bRNG ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Range   ());
			if( m_bDEV ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_StdDev  ());
			if( m_bVAR ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Variance());
			if( m_bNUM ) pAggregate->Set_Value(jField++, m_Statistics[iField].Get_Count   ());
			if( m_bLST ) pAggregate->Set_Value(jField++, m_List      [iField]);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Aggregate_by_Field::Statistics_Initialize(CSG_Table *pAggregated, CSG_Table *pTable)
{
	m_Stat_pFields	= Parameters("STATISTICS")->asTableFields();

	m_bSUM	= Parameters("STAT_SUM")->asBool();
	m_bAVG	= Parameters("STAT_AVG")->asBool();
	m_bMIN	= Parameters("STAT_MIN")->asBool();
	m_bMAX	= Parameters("STAT_MAX")->asBool();
	m_bRNG	= Parameters("STAT_RNG")->asBool();
	m_bDEV	= Parameters("STAT_DEV")->asBool();
	m_bVAR	= Parameters("STAT_VAR")->asBool();
	m_bLST	= Parameters("STAT_LST")->asBool();
	m_bNUM	= Parameters("STAT_NUM")->asBool();

	if( m_Stat_pFields->Get_Count() > 0 && (m_bSUM || m_bAVG || m_bMIN || m_bMAX || m_bRNG || m_bDEV || m_bVAR || m_bLST || m_bNUM) )
	{
		m_Statistics	= new CSG_Simple_Statistics[m_Stat_pFields->Get_Count()];

		m_Stat_Offset	= pAggregated->Get_Field_Count();

		for(int iField=0; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			CSG_String	s	= pTable->Get_Field_Name(m_Stat_pFields->Get_Index(iField));

			if( m_bSUM ) pAggregated->Add_Field(Statistics_Get_Name("SUM", s), SG_DATATYPE_Double);
			if( m_bAVG ) pAggregated->Add_Field(Statistics_Get_Name("AVG", s), SG_DATATYPE_Double);
			if( m_bMIN ) pAggregated->Add_Field(Statistics_Get_Name("MIN", s), SG_DATATYPE_Double);
			if( m_bMAX ) pAggregated->Add_Field(Statistics_Get_Name("MAX", s), SG_DATATYPE_Double);
			if( m_bRNG ) pAggregated->Add_Field(Statistics_Get_Name("RNG", s), SG_DATATYPE_Double);
			if( m_bDEV ) pAggregated->Add_Field(Statistics_Get_Name("STD", s), SG_DATATYPE_Double);
			if( m_bVAR ) pAggregated->Add_Field(Statistics_Get_Name("VAR", s), SG_DATATYPE_Double);
			if( m_bNUM ) pAggregated->Add_Field(Statistics_Get_Name("NUM", s), SG_DATATYPE_Int   );
			if( m_bLST ) pAggregated->Add_Field(Statistics_Get_Name("LST", s), SG_DATATYPE_String);
		}

		if( m_bLST )
		{
			m_List.Set_Count(m_Stat_pFields->Get_Count());
		}

		return( true );
	}

	m_Statistics	= NULL;

	if( pAggregated->Get_Field_Count() == 0 )
	{
		pAggregated->Add_Field("OID", SG_DATATYPE_Int);
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CTable_Aggregate_by_Field::Statistics_Get_Name(const CSG_String &Type, const CSG_String &Name)
{
	CSG_String	s;
	
	switch( Parameters("STAT_NAMING")->asInt() )
	{
	default: s.Printf("%s_%s", Type.c_str(), Name.c_str());	break;
	case  1: s.Printf("%s_%s", Name.c_str(), Type.c_str());	break;
	case  2: s.Printf("%s"   , Name.c_str()              );	break;
	case  3: s.Printf("%s"   , Type.c_str()              );	break;
	}

	return( s );
}

//---------------------------------------------------------
bool CTable_Aggregate_by_Field::Statistics_Add(CSG_Table_Record *pAggregate, CSG_Table_Record *pRecord, bool bReset)
{
	if( m_Statistics )
	{
		for(int iField=0; iField<m_Stat_pFields->Get_Count(); iField++)
		{
			if( bReset )
			{
				m_Statistics[iField].Create();

				if( iField < m_List.Get_Count() )
				{
					m_List[iField].Clear();
				}
			}

			if( !pRecord->is_NoData(m_Stat_pFields->Get_Index(iField)) )
			{
				m_Statistics[iField]	+= pRecord->asDouble(m_Stat_pFields->Get_Index(iField));
			}

			if( iField < m_List.Get_Count() )
			{
				if( !m_List[iField].is_Empty() )
				{
					m_List[iField]	+= "|";
				}

				if( !pRecord->is_NoData(m_Stat_pFields->Get_Index(iField)) )
				{
					m_List[iField]	+= pRecord->asString(m_Stat_pFields->Get_Index(iField));
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
