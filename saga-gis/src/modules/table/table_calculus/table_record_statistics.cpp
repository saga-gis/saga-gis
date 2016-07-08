
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    table_calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               table_record_statistics.cpp             //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                      Olaf Conrad                      //
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
#include "table_record_statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	STATS_MEAN	= 0,
	STATS_MIN,
	STATS_MAX,
	STATS_RANGE,
	STATS_SUM,
	STATS_NUM,
	STATS_VAR,
	STATS_STDV,
	STATS_PCTL,
	STATS_COUNT
};

//---------------------------------------------------------
const CSG_String	STATS[STATS_COUNT][2]	=
{
	{	"MEAN"  , _TL("Arithmetic Mean"   )	},
	{	"MIN"   , _TL("Minimum"           )	},
	{	"MAX"   , _TL("Maximum"           )	},
	{	"RANGE" , _TL("Range"             )	},
	{	"SUM"   , _TL("Sum"               )	},
	{	"NUM"   , _TL("Number of Values"  )	},
	{	"VAR"   , _TL("Variance"          )	},
	{	"STDDEV", _TL("Standard Deviation")	},
	{	"PCTL"  , _TL("Percentile"        )	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Record_Statistics_Base::CTable_Record_Statistics_Base(void)
{
	Set_Name		(_TL("Record Statistics"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		""
	));

	for(int i=0; i<STATS_COUNT; i++)
	{
		Parameters.Add_Bool(NULL, STATS[i][0], STATS[i][1], _TL(""));
	}

	Parameters.Add_Double(
		Parameters("PCTL"), "PCTL_VAL", _TL("Value"),
		_TL(""),
		50.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Record_Statistics_Base::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), "PCTL") )
	{
		pParameters->Get_Parameter("PCTL_VAL")->Set_Enabled(pParameter->asBool());
	}

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Record_Statistics_Base::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("TABLE")->asTable();

	if( !pTable->is_Valid() || pTable->Get_Field_Count() <= 0 || pTable->Get_Record_Count() <= 0 )
	{
		Error_Set(_TL("invalid table"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Array_Int	_Fields;

	int	*Fields	= (int *)Parameters("FIELDS")->asPointer();
	int	nFields	=        Parameters("FIELDS")->asInt    ();

	if( nFields == 0 )
	{
		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			if( SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
			{
				_Fields.Inc_Array(); _Fields[nFields++]	= i;
			}
		}

		if( nFields == 0 )
		{
			Error_Set(_TL("could not find any numeric attribute field"));

			return( false );
		}

		Fields	= _Fields.Get_Array();
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asTable() && Parameters("RESULT")->asTable() != pTable )
	{
		pTable	= Parameters("RESULT")->asTable();
		pTable->Create( *Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}

	//-----------------------------------------------------
	double	Quantile	= Parameters("PCTL_VAL")->asDouble() / 100.0;

	int	offResult	= pTable->Get_Field_Count();

	bool	bStats[STATS_COUNT];

	{
		for(int i=0; i<STATS_COUNT; i++)
		{
			if( (bStats[i] = Parameters(STATS[i][0])->asBool()) == true )
			{
				pTable->Add_Field(STATS[i][1], SG_DATATYPE_Double);
			}
		}

		if( pTable->Get_Field_Count() == offResult )
		{
			Error_Set(_TL("no statistical measure has been selected"));

			return( false );
		}
	}

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		CSG_Simple_Statistics	s(bStats[STATS_PCTL]);

		for(int iField=0; iField<nFields; iField++)
		{
			if( !pRecord->is_NoData(iField) )
			{
				s	+= pRecord->asDouble(Fields[iField]);
			}
		}

		//-------------------------------------------------
		int	iField	= offResult;

		if( s.Get_Count() > 0 )
		{
			if( bStats[STATS_MEAN ]	)	pRecord->Set_Value(iField++, s.Get_Mean    ());
			if( bStats[STATS_MIN  ]	)	pRecord->Set_Value(iField++, s.Get_Minimum ());
			if( bStats[STATS_MAX  ]	)	pRecord->Set_Value(iField++, s.Get_Maximum ());
			if( bStats[STATS_RANGE]	)	pRecord->Set_Value(iField++, s.Get_Range   ());
			if( bStats[STATS_SUM  ]	)	pRecord->Set_Value(iField++, s.Get_Sum     ());
			if( bStats[STATS_NUM  ]	)	pRecord->Set_Value(iField++, s.Get_Count   ());
			if( bStats[STATS_VAR  ]	)	pRecord->Set_Value(iField++, s.Get_Variance());
			if( bStats[STATS_STDV ]	)	pRecord->Set_Value(iField++, s.Get_StdDev  ());
			if( bStats[STATS_PCTL ]	)	pRecord->Set_Value(iField++, s.Get_Quantile(Quantile));
		}
		else
		{
			for(int i=0; i<STATS_COUNT; i++)
			{
				if( bStats[i] )
				{
					pRecord->Set_NoData(iField++);
				}
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Record_Statistics::CTable_Record_Statistics(void)
	: CTable_Record_Statistics_Base()
{
	CSG_Parameter	*pNode	= Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("If no field is selected statistics will be built from all numeric fields.")
	);

	Parameters.Add_Table(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Record_Statistics_Shapes::CTable_Record_Statistics_Shapes(void)
	: CTable_Record_Statistics_Base()
{
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Fields(
		pNode	, "FIELDS"		, _TL("Attributes"),
		_TL("If no field is selected statistics will be built from all numeric fields.")
	);

	Parameters.Add_Shapes(
		NULL	, "RESULT"		, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
