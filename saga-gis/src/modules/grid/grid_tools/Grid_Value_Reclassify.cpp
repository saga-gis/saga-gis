/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Value_Reclassify.cpp               //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Value_Reclassify.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_Reclassify::CGrid_Value_Reclassify(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Reclassify Grid Values"));

	Set_Author(_TL("Copyrights (c) 2005 by Volker Wichmann"));

	Set_Description	(_TW(
		"The module can be used to reclassify the values of a grid. It provides three different options:\n"
		"(a) reclassification of single values\n"
		"(b) reclassification of a range of values\n"
		"(c) reclassification of value ranges specified in a lookup table\n\n"
		"In addition to theses methods, two special cases (NoData values and values not included in the "
		"reclassification setup) are supported.\n"
		"With reclassification mode (a) and (b), the 'NoData option' is evaluated before the 'Method' "
		"settings. In reclassification mode (c) the option is evaluated only if the NoData value is not "
		"included in the lookup table.\n"
		"The 'other values' option is always evaluated after checking the 'Method' settings.\n\n"
		"The module allows to define the NoData value of the output grid (header): by default, the "
		"output grid gets assigned the NoData value of the input grid. But it is also possible to "
		"assign a user defined NoData value.\n\n"
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		,_TL("Grid"),
		_TL("Grid to reclassify"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Reclassified Grid"),
		_TL("Reclassified grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Select the desired method: 1. a single value or a range defined by a single value is reclassified, 2. a range of values is reclassified, 3. the lookup table is used to reclassify the grid."),
		_TL("single|range|simple table|user supplied table|"), 0
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "OLD"			, _TL("old value"),
		_TL("Value to reclassify."),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		NULL	, "NEW"			, _TL("new value"),
		_TL("New value."),
		PARAMETER_TYPE_Double, 1
	);

	Parameters.Add_Choice(
		NULL	, "SOPERATOR"	, _TL("operator"),
		_TL("Select the desired operator (<;.;=; >;.); it is possible to define a range above or below the old value."),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("="),
			_TL("<"),
			_TL("<="),
			_TL(">="),
			_TL(">")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "MIN"			, _TL("minimum value"),
		_TL("Minimum value of the range to be reclassified."),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		NULL	, "MAX"			, _TL("maximum value"),
		_TL("Maximum value of the range to be reclassified."),
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Value(
		NULL	, "RNEW"		, _TL("new value"),
		_TL("new value"),
		PARAMETER_TYPE_Double, 5
	);

	Parameters.Add_Choice(
		NULL	, "ROPERATOR"	, _TL("operator"),
		_TL("Select operator: eg. min < value < max."),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("<="),
			_TL("<")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_FixedTable(
		NULL	, "RETAB"		, _TL("Lookup Table"),
		_TL("Lookup table used in method \"table\"")
	);

	Parameters.Add_Choice(
		NULL	, "TOPERATOR"	, _TL("operator"),
		_TL("Select the desired operator (min < value < max; min . value < max; min .value . max; min < value . max)."),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("min <= value < max"),
			_TL("min <= value <= max"),
			_TL("min < value <= max"),
			_TL("min < value < max")
		), 0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Table(
		NULL	, "RETAB_2"		, _TL("Lookup Table"),
		_TL("Lookup table used in method \"user supplied table\""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field(
		pNode	, "F_MIN"		, _TL("minimum value"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "F_MAX"		, _TL("maximum value"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "F_CODE"		, _TL("new value"),
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL, "OPTIONS"			, _TL("Special cases"),
		_TL("Parameter settings for No-Data and all other values.")
	);

	Parameters.Add_Value(
		pNode	, "NODATAOPT"	, _TL("no data values"),
		_TL("Use this option to reclassify No-Data values independently of the method settings."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		Parameters("NODATAOPT")	, "NODATA"		, _TL("new value"),
		_TL("new value"),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		pNode	, "OTHEROPT"	, _TL("other values"),
		_TL("Use this option to reclassify all other values that are not specified in the options above."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		Parameters("OTHEROPT")	, "OTHERS"		, _TL("new value"),
		_TL("new value"),
		PARAMETER_TYPE_Double, 0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL, "OPT_RESULT"			, _TL("NoData Output Grid"),
		_TL("Parameter settings for the NoData value of the output grid (header).")
	);

	Parameters.Add_Choice(
		pNode	, "RESULT_NODATA_CHOICE"	, _TL("Assign ..."),
		_TL("Choose how to handle the NoData value of the output grid."),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("NoData value of input grid"),
			_TL("user defined NoData value")
		), 0
	);

	Parameters.Add_Value(
		Parameters("RESULT_NODATA_CHOICE")	, "RESULT_NODATA_VALUE"		, _TL("NoData Value"),
		_TL("User defined NoData value for output grid."),
		PARAMETER_TYPE_Double, -99999
	);

	//-----------------------------------------------------
	CSG_Table			*pLookup;
	CSG_Table_Record	*pRecord;

	pLookup	= Parameters("RETAB")->asTable();

	pLookup->Add_Field(_TL("minimum")	, SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("maximum")	, SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("new")		, SG_DATATYPE_Double);

	pRecord	= pLookup->Add_Record();	pRecord->Set_Value(0,  0.0);	pRecord->Set_Value(1, 10.0);	pRecord->Set_Value(2, 1.0);
	pRecord	= pLookup->Add_Record();	pRecord->Set_Value(0, 10.0);	pRecord->Set_Value(1, 20.0);	pRecord->Set_Value(2, 2.0);
}

//---------------------------------------------------------
CGrid_Value_Reclassify::~CGrid_Value_Reclassify(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Reclassify::On_Execute(void)
{
	int		method;
	bool    bSuccess = false;

	pInput		= Parameters("INPUT")->asGrid();
	pResult		= Parameters("RESULT")->asGrid();
	method		= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	switch( method )
	{
    default:
	case 0:	bSuccess = ReclassSingle();     break;
	case 1:	bSuccess = ReclassRange();      break;
	case 2:	bSuccess = ReclassTable(false); break;
	case 3:	bSuccess = ReclassTable(true);  break;
	}

	//-----------------------------------------------------
	if( bSuccess )
	{
		if( Parameters("RESULT_NODATA_CHOICE") == 0 )
		{
			pResult->Set_NoData_Value(pInput->Get_NoData_Value());
		}
		else
		{
			pResult->Set_NoData_Value(Parameters("RESULT_NODATA_VALUE")->asDouble());
		}

	    pResult->Set_Name(CSG_String::Format(SG_T("%s_reclassified"), pInput->Get_Name()));

	    return( true );
	}
	else
	{
        return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Value_Reclassify::ReclassRange(void)
{
	bool	otherOpt, noDataOpt, floating;
	int		opera;
	double	minValue, maxValue, others, noData, noDataValue, newValue;


	minValue	= Parameters("MIN")->asDouble();
	maxValue	= Parameters("MAX")->asDouble();
	newValue	= Parameters("RNEW")->asDouble();
	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	otherOpt	= Parameters("OTHEROPT")->asBool();
	noDataOpt	= Parameters("NODATAOPT")->asBool();
	opera		= Parameters("ROPERATOR")->asInt();

	noDataValue = pInput->Get_NoData_Value();

	if( (pInput->Get_Type() == SG_DATATYPE_Double) || (pInput->Get_Type() == SG_DATATYPE_Float) )
		floating = true;
	else
		floating = false;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	value;

			if( floating == true )
				value = pInput->asDouble(x, y);
			else
				value = pInput->asInt(x, y);

			if( opera == 0 )												// operator <=
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
					pResult->Set_Value(x, y, noData);
				else if( minValue <= value && value <= maxValue )			// reclass old range
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}

			if( opera == 1 )												// operator <
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
					pResult->Set_Value(x, y, noData);
				else if( minValue < value && value < maxValue )				// reclass old range
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Value_Reclassify::ReclassSingle(void)
{
	bool	otherOpt, noDataOpt, floating;
	int		opera;
	double	oldValue, newValue, others, noData, noDataValue;


	oldValue	= Parameters("OLD")->asDouble();
	newValue	= Parameters("NEW")->asDouble();
	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	otherOpt	= Parameters("OTHEROPT")->asBool();
	noDataOpt	= Parameters("NODATAOPT")->asBool();
	opera		= Parameters("SOPERATOR")->asInt();

	noDataValue = pInput->Get_NoData_Value();

	if( (pInput->Get_Type() == SG_DATATYPE_Double) || (pInput->Get_Type() == SG_DATATYPE_Float) )
		floating = true;
	else
		floating = false;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	value;

			if( floating == true )
				value = pInput->asDouble(x, y);
			else
				value = pInput->asInt(x, y);

			if( opera == 0 )												// operator =
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
					pResult->Set_Value(x, y, noData);
				else if( value == oldValue )								// reclass old value
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}

			if( opera == 1 )												// operator <
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
					pResult->Set_Value(x, y, noData);
				else if( value < oldValue )									// reclass old value
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}

			if( opera == 2 )												// operator <=
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
						pResult->Set_Value(x, y, noData);
				else if( value <= oldValue )								// reclass old value
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}

			if( opera == 3 )												// operator >=
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
						pResult->Set_Value(x, y, noData);
				else if( value >= oldValue )								// reclass old value
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}

			if( opera == 4 )												// operator >
			{
				if( noDataOpt == true && value == noDataValue )				// noData option
						pResult->Set_Value(x, y, noData);
				else if( value > oldValue )									// reclass old value
					pResult->Set_Value(x, y, newValue);
				else if( otherOpt == true && value != noDataValue )			// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);						// or original value
			}
		}
	}

	return( true );
}


//---------------------------------------------------------
bool CGrid_Value_Reclassify::ReclassTable(bool bUser)
{
	bool			otherOpt, noDataOpt;
	int				opera, field_Min, field_Max, field_Code;
	double			others, noData, noDataValue;

	CSG_Table			*pReTab;

	if( bUser )
	{
		pReTab		= Parameters("RETAB_2")	->asTable();
		field_Min	= Parameters("F_MIN")	->asInt();
		field_Max	= Parameters("F_MAX")	->asInt();
		field_Code	= Parameters("F_CODE")	->asInt();
	}
	else
	{
		pReTab		= Parameters("RETAB")	->asTable();
		field_Min	= 0;
		field_Max	= 1;
		field_Code	= 2;
	}

	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	otherOpt	= Parameters("OTHEROPT")->asBool();
	noDataOpt	= Parameters("NODATAOPT")->asBool();
	opera		= Parameters("TOPERATOR")->asInt();

	noDataValue = pInput->Get_NoData_Value();


	if( pReTab == NULL )
	{
		Error_Set(_TL("You must specify a reclass table with a minimium (field 1), a maximum (field 2) and a code value (field 3)!\n"));
		return( false );
	}

	if( pReTab->Get_Record_Count() == 0 )
	{
		Error_Set(_TL("You must specify a reclass table with a minimium of one record!\n"));
		return( false );
	}


	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	value	= pInput->asDouble(x, y);
			bool	set		= false;

			for(int iRecord=0; iRecord<pReTab->Get_Record_Count(); iRecord++)									// reclass
			{
				CSG_Table_Record	*pRecord = pReTab->Get_Record(iRecord);

				if( opera == 0 )										// min <= value < max
				{
					if( value >= pRecord->asDouble(field_Min) && value < pRecord->asDouble(field_Max) )
					{
						pResult->Set_Value(x, y, pRecord->asDouble(field_Code));
						set = true;
						break;
					}
				}
				else if( opera == 1 )									// min <= value <= max
				{
					if( value >= pRecord->asDouble(field_Min) && value <= pRecord->asDouble(field_Max) )
					{
						pResult->Set_Value(x, y, pRecord->asDouble(field_Code));
						set = true;
						break;
					}
				}
				else if( opera == 2 )									// min < value <= max
				{
					if( value > pRecord->asDouble(field_Min) && value <= pRecord->asDouble(field_Max) )
					{
						pResult->Set_Value(x, y, pRecord->asDouble(field_Code));
						set = true;
						break;
					}
				}
				else if( opera == 3 )									// min < value < max
				{
					if( value > pRecord->asDouble(field_Min) && value < pRecord->asDouble(field_Max) )
					{
						pResult->Set_Value(x, y, pRecord->asDouble(field_Code));
						set = true;
						break;
					}
				}
			}

			if( set == false )
			{
				if( noDataOpt == true && value == noDataValue )			// noData option
					pResult->Set_Value(x, y, noData);
				else if( otherOpt == true && value != noDataValue)		// other values option
					pResult->Set_Value(x, y, others);
				else
					pResult->Set_Value(x, y, value);					// or original value
			}
		}
	}

	return (true);
}


//---------------------------------------------------------
int CGrid_Value_Reclassify::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		int		Value	= pParameter->asInt();

		// single
		pParameters->Get_Parameter("OLD"		)->Set_Enabled(Value == 0);
		pParameters->Get_Parameter("NEW"		)->Set_Enabled(Value == 0);
		pParameters->Get_Parameter("SOPERATOR"	)->Set_Enabled(Value == 0);

		// range
		pParameters->Get_Parameter("MIN"		)->Set_Enabled(Value == 1);
		pParameters->Get_Parameter("MAX"		)->Set_Enabled(Value == 1);
		pParameters->Get_Parameter("RNEW"		)->Set_Enabled(Value == 1);
		pParameters->Get_Parameter("ROPERATOR"	)->Set_Enabled(Value == 1);

		// simple table
		pParameters->Get_Parameter("RETAB"		)->Set_Enabled(Value == 2);

		// user supplied table
		pParameters->Get_Parameter("RETAB_2"	)->Set_Enabled(Value == 3);

		pParameters->Get_Parameter("TOPERATOR"	)->Set_Enabled(Value == 2 || Value == 3);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NODATAOPT")) )
	{
		pParameters->Get_Parameter("NODATA"		)->Set_Enabled(pParameter->asInt() > 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("OTHEROPT")) )
	{
		pParameters->Get_Parameter("OTHERS"		)->Set_Enabled(pParameter->asInt() > 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("RESULT_NODATA_CHOICE")) )
	{
		pParameters->Get_Parameter("RESULT_NODATA_VALUE")->Set_Enabled(pParameter->asInt() > 0);
	}
	
	//-----------------------------------------------------
	return (1);
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
