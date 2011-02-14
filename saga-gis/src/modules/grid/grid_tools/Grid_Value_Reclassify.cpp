
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     reklovw@web.de                         //
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
		"The module can be used to reclassify the values of a grid. It provides three different options: (a) "
		"reclassification of single values, (b) reclassification of a range of values and (c) reclassification "
		"of value ranges specified in a lookup table. In addition to theses methods, two special cases "
		"(No-Data values and values not included in the reclassification setup) are supported. In mode (a) "
		"and (b) the 'No-Data option' is evaluated before the method settings, in mode (c) the option is "
		"evaluated only if the No-Data value ins't included in the lookup table. The 'other values' option "
		"is always evaluated after checking the method settings. ")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		,_TL("Grid"),
		_TL("Grid to reclassify"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Reclassified Grid"),
		_TL("Reclassifed grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Select the desired method: 1. a single value or a range defined by a single value is reclassified, 2. a range of values is reclassified, 3. the lookup table is used to reclassify the grid."),
		_TL("single|range|simple table|user supplied table|"), 0
	);


	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "SINGLE", _TL("Method single"),
		_TL("Parameter settings for method single.")
	);

	Parameters.Add_Value(
		pNode	, "OLD"			, _TL("old value"),
		_TL("Value to reclassify."),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		pNode	, "NEW"			, _TL("new value"),
		_TL("New value."),
		PARAMETER_TYPE_Double, 1
	);

	Parameters.Add_Choice(
		pNode	, "SOPERATOR"	, _TL("operator"),
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
	pNode	= Parameters.Add_Node(
		NULL	, "RANGE"		, _TL("Method range"),
		_TL("Parameter settings for method range.")
	);

	Parameters.Add_Value(
		pNode	, "MIN"			, _TL("minimum value"),
		_TL("Minimum value of the range to be reclassified."),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		pNode	, "MAX"			, _TL("maximum value"),
		_TL("Maximum value of the range to be reclassified."),
		PARAMETER_TYPE_Double, 10
	);

	Parameters.Add_Value(
		pNode	, "RNEW"		, _TL("new value"),
		_TL("new value"),
		PARAMETER_TYPE_Double, 5
	);

	Parameters.Add_Choice(
		pNode	, "ROPERATOR"	, _TL("operator"),
		_TL("Select operator: eg. min < value < max."),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("<="),
			_TL("<")
		), 0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "TABLE"		, _TL("Method simple table"),
		_TL("Parameter settings for method simple table.")
	);

	Parameters.Add_FixedTable(
		pNode	, "RETAB"		, _TL("Lookup Table"),
		_TL("Lookup table used in method \"table\"")
	);

	Parameters.Add_Choice(
		pNode	, "TOPERATOR"	, _TL("operator"),
		_TL("Select the desired operator (min < value < max; min . value < max; min .value . max; min < value . max)."),

		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("min <= value < max"),
			_TL("min <= value <= max"),
			_TL("min < value <= max"),
			_TL("min < value < max")
		), 0
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "TABLE_2"		, _TL("Method user supplied table"),
		_TL("Parameter settings for method user supplied table.")
	);

	pNode	= Parameters.Add_Table(
		pNode	, "RETAB_2"		, _TL("Lookup Table"),
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
		pNode	, "NODATA"		, _TL("no data values >> value"),
		_TL("new value"),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		pNode	, "OTHEROPT"	, _TL("other values"),
		_TL("Use this option to reclassify all other values that are not specified in the options above."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		pNode	, "OTHERS"		, _TL("other values >> value"),
		_TL("new value"),
		PARAMETER_TYPE_Double, 0
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

	pInput		= Parameters("INPUT")->asGrid();
	pResult		= Parameters("RESULT")->asGrid();
	method		= Parameters("METHOD")->asInt();

	//-----------------------------------------------------
	switch( method )
	{
	case 0:	return( ReclassSingle() );
	case 1:	return( ReclassRange() );
	case 2:	return( ReclassTable(false) );
	case 3:	return( ReclassTable(true) );
	}

	//-----------------------------------------------------
	return( false );
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
	int		x, y, opera;
	double	minValue, maxValue, value, others, noData, noDataValue, newValue;


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

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
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
	int		x, y, opera;
	double	oldValue, newValue, value, others, noData, noDataValue;


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

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
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
#define MAX_CAT	128

//---------------------------------------------------------
bool CGrid_Value_Reclassify::ReclassTable(bool bUser)
{
	bool			set, otherOpt, noDataOpt;
	int				n, x, y, opera, recCount, count[MAX_CAT], field_Min, field_Max, field_Code;
	double			min[MAX_CAT], max[MAX_CAT], code[MAX_CAT], value, others, noData, noDataValue;

	CSG_Table			*pReTab;
	CSG_Table_Record	*pRecord = NULL;

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

	recCount = pReTab->Get_Record_Count();
	if( recCount > MAX_CAT )
	{
		Error_Set(_TL("At the moment the reclass table is limited to 128 categories!\n"));
		return( false );
	}

	if( recCount == 0 )
	{
		Error_Set(_TL("You must specify a reclass table with a minimium of one record!\n"));
		return( false );
	}

	for(n=0; n<recCount ; n++)								// initialize reclass arrays
	{
		pRecord		= pReTab->Get_Record(n);
		min[n]		= pRecord->asDouble(field_Min);
		max[n]		= pRecord->asDouble(field_Max);
		code[n]		= pRecord->asDouble(field_Code);
		count[n]	= 0;
	}


	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			value	= pInput->asDouble(x, y);
			set		= false;

			for(n=0; n< recCount; n++)									// reclass
			{
				if( opera == 0 )										// min <= value < max
				{
					if( value >= min[n] && value < max[n] )
					{
						pResult->Set_Value(x, y, code[n]);
						set = true;
						count[n] += 1;
						break;
					}
				}
				else if( opera == 1 )									// min <= value <= max
				{
					if( value >= min[n] && value <= max[n] )
					{
						pResult->Set_Value(x, y, code[n]);
						set = true;
						count[n] += 1;
						break;
					}
				}
				else if( opera == 2 )									// min < value <= max
				{
					if( value > min[n] && value <= max[n] )
					{
						pResult->Set_Value(x, y, code[n]);
						set = true;
						count[n] += 1;
						break;
					}
				}
				else if( opera == 3 )									// min < value < max
				{
					if( value > min[n] && value < max[n] )
					{
						pResult->Set_Value(x, y, code[n]);
						set = true;
						count[n] += 1;
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
