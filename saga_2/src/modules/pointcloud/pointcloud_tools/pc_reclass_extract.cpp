
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 pc_reclass_extract.cpp                //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
#include "pc_reclass_extract.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Reclass_Extract::CPC_Reclass_Extract(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Point Cloud Reclassifier / Subset Extractor"));

	Set_Author(_TL("Volker Wichmann (c) 2009, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The module can be used to either reclassify a Point Cloud attribute or to extract "
		"a subset of a Point Cloud based on the values of an attribute.\n\n"
		"The module provides three different options:\n"
		"(a) reclassification of (or extraction based on) single values,\n"
		"(b) reclassification of (or extraction based on) a range of values and\n"
		"(c) reclassification of (or extraction based on) value ranges specified in a lookup table.\n\n"
		"Each of these three options provides it's own parameters. The 'new value' parameters are "
		"irrelevant in case a subset is extracted.\n\n"
		"In addition to these settings, two special cases ('NoData values' and 'other values' not "
		"included in the parameter setup) are supported:\n"
		"In mode (a) and (b) the 'NoData option' is evaluated before the method settings, in mode "
		"(c) the option is evaluated only if the NoData value ins't included in the lookup table.\n"
		"The 'other values' option is always evaluated after checking the method settings.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "INPUT"		,_TL("Point Cloud"),
		_TL("Point Cloud to reclassify/extract"),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		Parameters("INPUT"), "ATTRIB", _TL("Attribute"),
		_TL("Attribute to process."),
		false
	);

	Parameters.Add_PointCloud(
		NULL	, "RESULT"		, _TL("Result"),
		_TL("Reclassifed or extracted Point Cloud."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "MODE"	, _TL("Mode of operation"),
		_TL("Choose whether to reclassify a Point Cloud or to extract a subset from a Point Cloud."),
		_TL("Reclassify|Extract Subset|"), 0
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
CPC_Reclass_Extract::~CPC_Reclass_Extract(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Reclass_Extract::On_Execute(void)
{
	int		method;

	m_pInput		= Parameters("INPUT")->asPointCloud();
	m_pResult		= Parameters("RESULT")->asPointCloud();
	method			= Parameters("METHOD")->asInt();
	m_AttrField		= Parameters("ATTRIB")->asInt();
	m_bExtract		= Parameters("MODE")->asInt() == 0 ? false : true;

	m_pResult->Create(m_pInput);

	if (m_bExtract)
		m_pResult->Set_Name(CSG_String::Format(SG_T("%s_subset_%s"), m_pInput->Get_Name(), m_pInput->Get_Field_Name(m_AttrField)));
	else
		m_pResult->Set_Name(CSG_String::Format(SG_T("%s_reclass_%s"), m_pInput->Get_Name(), m_pInput->Get_Field_Name(m_AttrField)));


	//-----------------------------------------------------
	switch( method )
	{
	case 0:	return( Reclass_Single() );
	case 1:	return( Reclass_Range() );
	case 2:	return( Reclass_Table(false) );
	case 3:	return( Reclass_Table(true) );
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
bool CPC_Reclass_Extract::Reclass_Range(void)
{
	bool		otherOpt, noDataOpt, floating;
	int			opera;
	double		minValue, maxValue, value, others, noData, noDataValue, newValue;


	minValue	= Parameters("MIN")->asDouble();
	maxValue	= Parameters("MAX")->asDouble();
	newValue	= Parameters("RNEW")->asDouble();
	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	otherOpt	= Parameters("OTHEROPT")->asBool();
	noDataOpt	= Parameters("NODATAOPT")->asBool();
	opera		= Parameters("ROPERATOR")->asInt();

	noDataValue = m_pInput->Get_NoData_Value();

	if( (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Double) || (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Float) )
		floating = true;
	else
		floating = false;

	for (int i=0; i<m_pInput->Get_Point_Count(); i++)
	{
		if( floating == true )
			value = m_pInput->Get_Value(i, m_AttrField);
		else
			value = (int)m_pInput->Get_Value(i, m_AttrField);

		if( opera == 0 )												// operator <=
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( minValue <= value && value <= maxValue )			// reclass old range
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value		
			}
		}

		if( opera == 1 )												// operator <
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( minValue < value && value < maxValue )				// reclass old range
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CPC_Reclass_Extract::Reclass_Single(void)
{
	bool		otherOpt, noDataOpt, floating;
	int			opera;
	double		oldValue, newValue, value, others, noData, noDataValue;


	oldValue	= Parameters("OLD")->asDouble();
	newValue	= Parameters("NEW")->asDouble();
	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	otherOpt	= Parameters("OTHEROPT")->asBool();
	noDataOpt	= Parameters("NODATAOPT")->asBool();
	opera		= Parameters("SOPERATOR")->asInt();

	noDataValue = m_pInput->Get_NoData_Value();

	if( (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Double) || (m_pInput->Get_Attribute_Type(m_AttrField) == SG_DATATYPE_Float) )
		floating = true;
	else
		floating = false;

		
	for (int i=0; i<m_pInput->Get_Point_Count(); i++)
	{
		if( floating == true )
			value = m_pInput->Get_Value(i, m_AttrField);
		else
			value = (int)m_pInput->Get_Value(i, m_AttrField);


		if( opera == 0 )												// operator =
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( value == oldValue )								// reclass old value
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value
			}
		}

		if( opera == 1 )												// operator <
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( value < oldValue )									// reclass old value
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value
			}
		}

		if( opera == 2 )												// operator <=
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( value <= oldValue )								// reclass old value
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value
			}
		}

		if( opera == 3 )												// operator >=
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( value >= oldValue )								// reclass old value
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value
			}
		}

		if( opera == 4 )												// operator >
		{
			if( noDataOpt == true && value == noDataValue )				// noData option
				Set_Value(i, noData);
			else if( value > oldValue )									// reclass old value
				Set_Value(i, newValue);
			else if( otherOpt == true && value != noDataValue )			// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);								// or original value
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
#define MAX_CAT	128

//---------------------------------------------------------
bool CPC_Reclass_Extract::Reclass_Table(bool bUser)
{
	bool				set, otherOpt, noDataOpt;
	int					n, opera, recCount, count[MAX_CAT], field_Min, field_Max, field_Code;
	double				min[MAX_CAT], max[MAX_CAT], code[MAX_CAT], value, others, noData, noDataValue;

	CSG_Table			*pReTab;
	CSG_Table_Record	*pRecord = NULL;

	if( bUser )
	{
		pReTab			= Parameters("RETAB_2")	->asTable();
		field_Min		= Parameters("F_MIN")	->asInt();
		field_Max		= Parameters("F_MAX")	->asInt();
		field_Code		= Parameters("F_CODE")	->asInt();
	}
	else
	{
		pReTab			= Parameters("RETAB")	->asTable();
		field_Min		= 0;
		field_Max		= 1;
		field_Code		= 2;
	}

	others				= Parameters("OTHERS")->asDouble();
	noData				= Parameters("NODATA")->asDouble();
	otherOpt			= Parameters("OTHEROPT")->asBool();
	noDataOpt			= Parameters("NODATAOPT")->asBool();
	opera				= Parameters("TOPERATOR")->asInt();

	noDataValue			= m_pInput->Get_NoData_Value();


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


	for (int i=0; i<m_pInput->Get_Point_Count(); i++)
	{
		value	= m_pInput->Get_Value(i, m_AttrField);
		set		= false;

		for(n=0; n< recCount; n++)									// reclass
		{
			if( opera == 0 )										// min <= value < max
			{
				if( value >= min[n] && value < max[n] )
				{
					Set_Value(i, code[n]);
					set = true;
					count[n] += 1;
					break;
				}
			}
			else if( opera == 1 )									// min <= value <= max
			{
				if( value >= min[n] && value <= max[n] )
				{
					Set_Value(i, code[n]);
					set = true;
					count[n] += 1;
					break;
				}
			}
			else if( opera == 2 )									// min < value <= max
			{
				if( value > min[n] && value <= max[n] )
				{
					Set_Value(i, code[n]);
					set = true;
					count[n] += 1;
					break;
				}
			}
			else if( opera == 3 )									// min < value < max
			{
				if( value > min[n] && value < max[n] )
				{
					Set_Value(i, code[n]);
					set = true;
					count[n] += 1;
					break;
				}
			}
		}

		if( set == false )
		{
			if( noDataOpt == true && value == noDataValue )			// noData option
				Set_Value(i, noData);
			else if( otherOpt == true && value != noDataValue)		// other values option
				Set_Value(i, others);
			else
			{
				if (!m_bExtract)
					Set_Value(i, value);							// or original value
			}
		}
	}

	return (true);
}


//---------------------------------------------------------
void CPC_Reclass_Extract::Set_Value(int i, double value)
{
	m_pResult->Add_Point(m_pInput->Get_X(i), m_pInput->Get_Y(i), m_pInput->Get_Z(i));

	for (int j=0; j<m_pInput->Get_Attribute_Count(); j++)
		m_pResult->Set_Attribute(j, m_pInput->Get_Attribute(i, j));

	if (!m_bExtract)
		m_pResult->Set_Value(m_AttrField, value);

	return;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
