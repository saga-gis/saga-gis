/**********************************************************
 * Version $Id$
 *********************************************************/

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
//               table_running_average.cpp               //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "table_running_average.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Running_Average::CTable_Running_Average(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Running Average"));

	Set_Author		("O. Conrad (c) 2009");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("INPUT",
		"FIELD"		, _TL("Field"),
		_TL("")
	);

	Parameters.Add_Int("",
		"RANGE"		, _TL("Range of Records"),
		_TL("The number of preceding and following records to be taken into account for statistics. The total number is one plus two times the range."),
		10, 1, true
	);

	//-----------------------------------------------------
	#define ADD_OUTPUT(id, name, yes)	Parameters.Add_Bool("", id, name, "", yes); Parameters.Add_Table_Field("INPUT", CSG_String("FIELD_") + id, name, "", true);

	ADD_OUTPUT("MEAN"   , _TL("Mean"                     ),  true);
	ADD_OUTPUT("MEDIAN" , _TL("Median"                   ), false);
	ADD_OUTPUT("MIN"    , _TL("Minimum"                  ), false);
	ADD_OUTPUT("MAX"    , _TL("Maximum"                  ), false);
	ADD_OUTPUT("STDV"   , _TL("Standard Deviation"       ), false);
	ADD_OUTPUT("STDV_LO", _TL("Lower Standard Deviation" ), false);
	ADD_OUTPUT("STDV_HI", _TL("Higher Standard Deviation"), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Running_Average::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	#define SET_ENABLED(id)	if( !SG_STR_CMP(pParameter->Get_Identifier(), id) ) { pParameters->Set_Enabled(CSG_String("FIELD_") + id, pParameter->asBool()); }

	SET_ENABLED("MEAN"   );
	SET_ENABLED("MEDIAN" );
	SET_ENABLED("MIN"    );
	SET_ENABLED("MAX"    );
	SET_ENABLED("STDV"   );
	SET_ENABLED("STDV_LO");
	SET_ENABLED("STDV_HI");

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Running_Average::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("INPUT")->asTable();

	if( !pTable->is_Valid() )
	{
		return( false );
	}

	if( Parameters("OUTPUT")->asTable() && Parameters("OUTPUT")->asTable() != pTable )
	{
		pTable	= Parameters("OUTPUT")->asTable();

		pTable->Create(*Parameters("INPUT")->asTable());
	}

	//-----------------------------------------------------
	int	fValue	= Parameters("FIELD")->asInt();
	int	Range	= Parameters("RANGE")->asInt();

	//-----------------------------------------------------
	#define GET_FIELD(id, name)	(!Parameters(id)->asBool() ? -1 : Parameters(CSG_String("FIELD_") + id)->asInt() < 0 && pTable->Add_Field(CSG_String::Format("%s [%s]", pTable->Get_Field_Name(fValue), name), SG_DATATYPE_Double) ? pTable->Get_Field_Count() - 1 : Parameters(CSG_String("FIELD_") + id)->asInt())

	int	fMean	= GET_FIELD("MEAN"   , SG_T("MEAN"     )); if( fMean   >= 0 ) Parameters("FIELD_MEAN"   )->Set_Value(fMean  );
	int	fMedian	= GET_FIELD("MEDIAN" , SG_T("MEDIAN"   )); if( fMedian >= 0 ) Parameters("FIELD_MEDIAN" )->Set_Value(fMedian);
	int	fMin	= GET_FIELD("MIN"    , SG_T("MINIMUM"  )); if( fMin    >= 0 ) Parameters("FIELD_MIN"    )->Set_Value(fMin   );
	int	fMax	= GET_FIELD("MAX"    , SG_T("MAXIMUM"  )); if( fMax    >= 0 ) Parameters("FIELD_MAX"    )->Set_Value(fMax   );
	int	fStDv	= GET_FIELD("STDV"   , SG_T("STDV"     )); if( fStDv   >= 0 ) Parameters("FIELD_STDV"   )->Set_Value(fStDv  );
	int	fStDvLo	= GET_FIELD("STDV_LO", SG_T("STDV_LOW" )); if( fStDvLo >= 0 ) Parameters("FIELD_STDV_LO")->Set_Value(fStDvLo);
	int	fStDvHi	= GET_FIELD("STDV_HI", SG_T("STDV_HIGH")); if( fStDvHi >= 0 ) Parameters("FIELD_STDV_HI")->Set_Value(fStDvHi);

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Set_Progress(i, pTable->Get_Count()); i++)
	{
		CSG_Simple_Statistics	s(fMedian >= 0);

		for(int j=i-Range; j<=i+Range; j++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(j);

			if( pRecord && !pRecord->is_NoData(fValue) )
			{
				s	+= pRecord->asDouble(fValue);
			}
		}

		CSG_Table_Record	*pRecord	= pTable->Get_Record_byIndex(i);

		if( s.Get_Count() > 0 )
		{
			if( fMean   >= 0 ) pRecord->Set_Value(fMean  , s.Get_Mean   ());
			if( fMedian >= 0 ) pRecord->Set_Value(fMedian, s.Get_Median ());
			if( fMin    >= 0 ) pRecord->Set_Value(fMin   , s.Get_Minimum());
			if( fMax    >= 0 ) pRecord->Set_Value(fMax   , s.Get_Maximum());
			if( fStDv   >= 0 ) pRecord->Set_Value(fStDv  , s.Get_StdDev ());
			if( fStDvLo >= 0 ) pRecord->Set_Value(fStDvLo, s.Get_Mean   () - s.Get_StdDev());
			if( fStDvHi >= 0 ) pRecord->Set_Value(fStDvHi, s.Get_Mean   () + s.Get_StdDev());
		}
		else
		{
			if( fMean   >= 0 ) pRecord->Set_NoData(fMean  );
			if( fMedian >= 0 ) pRecord->Set_NoData(fMedian);
			if( fMin    >= 0 ) pRecord->Set_NoData(fMin   );
			if( fMax    >= 0 ) pRecord->Set_NoData(fMax   );
			if( fStDv   >= 0 ) pRecord->Set_NoData(fStDv  );
			if( fStDvLo >= 0 ) pRecord->Set_NoData(fStDvLo);
			if( fStDvHi >= 0 ) pRecord->Set_NoData(fStDvHi);
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
