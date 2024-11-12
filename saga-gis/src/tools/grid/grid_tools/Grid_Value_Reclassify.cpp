
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "Grid_Value_Reclassify.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Value_Reclassify::CGrid_Value_Reclassify(void)
{
	Set_Name	(_TL("Reclassify Grid Values"));

	Set_Author	("V. Wichmann (c) 2005-2019");

	Set_Description	(_TW(
		"The tool can be used to reclassify the values of a grid. It provides three different options:\n"
		"(a) reclassification of single values\n"
		"(b) reclassification of a range of values\n"
		"(c) reclassification of value ranges specified in a lookup table (simple or user supplied table)\n\n"
		"In addition to these methods, two special cases (No Data values and values not included in the "
		"reclassification setup) are supported.\n"
		"With reclassification mode (a) and (b), the 'No Data' option is evaluated before the 'Method' "
		"settings. In reclassification mode (c) the option is evaluated only if the No Data value is not "
		"included in the lookup table.\n"
		"The 'Other Values' option is always evaluated after checking the 'Method' settings.\n\n"
		"The tool also provides options to control the data storage type and No Data value of the "
		"output grid.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"		,_TL("Grid"),
		_TL("Grid to reclassify."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"	, _TL("Reclassified Grid"),
		_TL("Reclassified grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL("Select the desired method: (1) a single value or a range defined by a single value is reclassified, (2) a range of values is reclassified, (3) and (4) a lookup table is used to reclassify the grid."),
		_TL("single|range|simple table|user supplied table"), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"OLD"	, _TL("Old Value"),
		_TL("Value to reclassify."),
		0.0
	);

	Parameters.Add_Double("",
		"NEW"	, _TL("New Value"),
		_TL("The value to assign (with method 'single value')."),
		1.0
	);

	Parameters.Add_Choice("",
		"SOPERATOR"	, _TL("Operator"),
		_TL("Select the desired operator; it is possible to define a range above or below the old value."),
		CSG_String::Format("%s|%s|%s|%s|%s",
			SG_T("="),
			SG_T("<"),
			SG_T("<="),
			SG_T(">="),
			SG_T(">")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"MIN"	, _TL("Minimum Value"),
		_TL("The minimum value of the range to be reclassified."),
		0.0
	);

	Parameters.Add_Double("",
		"MAX"	, _TL("Maximum Value"),
		_TL("The maximum value of the range to be reclassified."),
		10.0
	);

	Parameters.Add_Double("",
		"RNEW"	, _TL("New Value"),
		_TL("The value to assign (with method 'range')."),
		5.0
	);

	Parameters.Add_Choice("",
		"ROPERATOR"	, _TL("Operator"),
		_TL("Select the desired operator (for method 'range'): eg. min < value < max."),
		CSG_String::Format(SG_T("%s|%s"),
			SG_T("<="),
			SG_T("<")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_FixedTable("",
		"RETAB"		, _TL("Lookup Table"),
		_TL("The lookup table used with method 'table'.")
	);

	Parameters.Add_Choice("",
		"TOPERATOR"	, _TL("Operator"),
		_TL("Select the desired operator (for method 'table')."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("min <= value < max"),
			_TL("min <= value <= max"),
			_TL("min < value <= max"),
			_TL("min < value < max")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"RETAB_2"	, _TL("Lookup Table"),
		_TL("The lookup table used with method 'user supplied table'."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("RETAB_2",
		"F_MIN"		, _TL("Minimum Value"),
		_TL("The table field with the minimum value.")
	);

	Parameters.Add_Table_Field("RETAB_2",
		"F_MAX"		, _TL("Maximum Value"),
		_TL("The table field with the maximum value.")
	);

	Parameters.Add_Table_Field("RETAB_2",
		"F_CODE"	, _TL("New Value"),
		_TL("The table field with the value to assign.")
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"OPTIONS"	, _TL("Special Cases"),
		_TL("Parameter settings for the special cases (a) No-Data values and (b) values not included in the reclassification setup.")
	);

	Parameters.Add_Bool("OPTIONS",
		"NODATAOPT"	, _TL("No Data Values"),
		_TL("Use this option to reclassify No Data values independently of the method settings."),
		false
	);

	Parameters.Add_Double("NODATAOPT",
		"NODATA"	, _TL("New Value"),
		_TL("The value to assign to No Data values."),
		0.0
	);

	Parameters.Add_Bool("OPTIONS",
		"OTHEROPT"	, _TL("Other Values"),
		_TL("Use this option to reclassify all values that are not included in the reclassification setup."),
		false
	);

	Parameters.Add_Double("OTHEROPT",
		"OTHERS"	, _TL("New Value"),
		_TL("The value to assign to all values not included in the reclassification setup."),
		0.0
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"OPT_RESULT"	, _TL("Output Grid"),
		_TL("The parameter settings for the output grid.")
	);

	Parameters.Add_Data_Type("OPT_RESULT",
		"RESULT_TYPE"		, _TL("Data Storage Type"),
		_TL("The data storage type of the output grid."),
		SG_DATATYPES_Numeric|SG_DATATYPES_Bit, SG_DATATYPE_Undefined, _TL("same as first grid in list")
	);

	Parameters.Add_Choice("OPT_RESULT",
		"RESULT_NODATA_CHOICE"	, _TL("No Data Value"),
		_TL("Choose how to handle the No Data value of the output grid."),
		CSG_String::Format(SG_T("%s|%s|%s"),
			_TL("No Data value of input grid"),
			_TL("user defined No Data value"),
			_TL("No Data value of data storage type")
		), 0
	);

	Parameters.Add_Double("RESULT_NODATA_CHOICE",
		"RESULT_NODATA_VALUE"		, _TL("No Data Value"),
		_TL("User defined No Data value for output grid."),
		-99999.0
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

	//---------------------------------------------------------
	pResult->Create(pInput, Parameters("RESULT_TYPE")->asDataType()->Get_Data_Type(pInput->Get_Type()));


	//---------------------------------------------------------
	switch( method )
	{
	case 0:
	default:	bSuccess = ReclassSingle();			break;
	case 1:		bSuccess = ReclassRange();			break;
	case 2:		bSuccess = ReclassTable(false);		break;
	case 3:		bSuccess = ReclassTable(true);		break;
	}


	//---------------------------------------------------------
	if( bSuccess )
	{
		switch( Parameters("RESULT_NODATA_CHOICE")->asInt() )
		{
		case 0:
		default:		pResult->Set_NoData_Value(pInput->Get_NoData_Value());							break;
		case 1:			pResult->Set_NoData_Value(Parameters("RESULT_NODATA_VALUE")->asDouble());		break;
		case 2:			
						switch( pResult->Get_Type() )
						{
                        case SG_DATATYPE_Bit   : pResult->Set_NoData_Value(          0.);	break;
                        case SG_DATATYPE_Byte  : pResult->Set_NoData_Value(          0.);	break;
                        case SG_DATATYPE_Char  : pResult->Set_NoData_Value(       -127.);	break;
                        case SG_DATATYPE_Word  : pResult->Set_NoData_Value(      65535.);	break;
                        case SG_DATATYPE_Short : pResult->Set_NoData_Value(     -32767.);	break;
                        case SG_DATATYPE_DWord : pResult->Set_NoData_Value( 4294967295.);	break;
                        case SG_DATATYPE_Int   : pResult->Set_NoData_Value(-2147483647.);	break;
                        case SG_DATATYPE_ULong : pResult->Set_NoData_Value( 4294967295.);	break;
                        case SG_DATATYPE_Long  : pResult->Set_NoData_Value(-2147483647.);	break;
                        case SG_DATATYPE_Float : pResult->Set_NoData_Value(     -99999.);	break;
                        case SG_DATATYPE_Double: pResult->Set_NoData_Value(     -99999.);	break;
                        case SG_DATATYPE_Color : pResult->Set_NoData_Value( 4294967295.);	break;
						default:	pResult->Set_NoData_Value(pInput->Get_NoData_Value());	break;
						}
			
						break;
		}


	    pResult->Fmt_Name("%s (%s)", pInput->Get_Name(), _TL("Reclassified"));

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

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
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

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
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
		Error_Set(_TL("You must specify a reclass table with a minimum (field 1), a maximum (field 2) and a code value (field 3)!\n"));
		return( false );
	}

	if( pReTab->Get_Count() == 0 )
	{
		Error_Set(_TL("You must specify a reclass table with a minimum of one record!\n"));
		return( false );
	}


	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	value	= pInput->asDouble(x, y);
			bool	set		= false;

			for(sLong iRecord=0; iRecord<pReTab->Get_Count(); iRecord++)	// reclass
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
	if(	pParameter->Cmp_Identifier(SG_T("METHOD")) )
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

	if(	pParameter->Cmp_Identifier(SG_T("NODATAOPT")) )
	{
		pParameters->Get_Parameter("NODATA"		)->Set_Enabled(pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier(SG_T("OTHEROPT")) )
	{
		pParameters->Get_Parameter("OTHERS"		)->Set_Enabled(pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier(SG_T("RESULT_NODATA_CHOICE")) )
	{
		pParameters->Get_Parameter("RESULT_NODATA_VALUE")->Set_Enabled(pParameter->asInt() == 1);
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
