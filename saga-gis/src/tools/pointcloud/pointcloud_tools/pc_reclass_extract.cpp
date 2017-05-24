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

	Set_Author(SG_T("Volker Wichmann (c) 2009, LASERDATA GmbH"));

	Set_Description	(_TW(
		"The tool can be used to either reclassify a Point Cloud attribute or to extract "
		"a subset of a Point Cloud based on the values of an attribute.\n\n"
		"The tool provides three different options:\n"
		"(a) reclassification of (or extraction based on) single values,\n"
		"(b) reclassification of (or extraction based on) a range of values and\n"
		"(c) reclassification of (or extraction based on) value ranges specified in a lookup table.\n\n"
		"Each of these three options provides it's own parameters. The 'new value' parameters are "
		"irrelevant in case a subset is extracted.\n\n"
		"In addition to these settings, two special cases ('NoData values' and 'other values' not "
		"included in the parameter setup) are supported:\n"
		"In mode (a) and (b) the 'NoData option' is evaluated before the method settings, in mode "
		"(c) the option is evaluated only if the NoData value isn't included in the lookup table.\n"
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
		_TL("Reclassified or extracted Point Cloud."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "MODE"	, _TL("Mode of operation"),
		_TL("Choose whether to reclassify a Point Cloud or to extract a subset from a Point Cloud."),
		_TL("Reclassify|Extract Subset|"), 0
	);

	Parameters.Add_Value(
		NULL	, "CREATE_ATTRIB"	, _TL("Create new Attribute"),
		_TL("Check this to create a new attribute with the reclassification result. If unchecked, the existing attribute is updated."),
		PARAMETER_TYPE_Bool, false
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
	int				method;
	CSG_Parameters	sParms;

	m_pInput		= Parameters("INPUT")->asPointCloud();
	m_pResult		= Parameters("RESULT")->asPointCloud();
	method			= Parameters("METHOD")->asInt();
	m_AttrField		= Parameters("ATTRIB")->asInt();
	m_bExtract		= Parameters("MODE")->asInt() == 0 ? false : true;
	m_bCreateAttrib	= Parameters("CREATE_ATTRIB")->asBool();

	m_pResult->Create(m_pInput);

	if (m_bExtract)
		m_pResult->Set_Name(CSG_String::Format(SG_T("%s_subset_%s"), m_pInput->Get_Name(), m_pInput->Get_Field_Name(m_AttrField)));
	else
	{
		m_pResult->Set_Name(CSG_String::Format(SG_T("%s_reclass_%s"), m_pInput->Get_Name(), m_pInput->Get_Field_Name(m_AttrField)));
		if( m_bCreateAttrib )
			m_pResult->Add_Field(CSG_String::Format(SG_T("%s_reclass"), m_pInput->Get_Field_Name(m_AttrField)), m_pInput->Get_Field_Type(m_AttrField));
	}

	m_iOrig = 0;	// counter of unchanged points

	//-----------------------------------------------------
	switch( method )
	{
	case 0:	Reclass_Single();		break;
	case 1:	Reclass_Range();		break;
	case 2:	if( Reclass_Table(false) )
				break;
			else
				return( false );
	case 3:	if( Reclass_Table(true) )
				break;
			else
				return( false );
	default: break;
	}

	//-----------------------------------------------------
	DataObject_Update(m_pResult);

	DataObject_Get_Parameters(m_pResult, sParms);
	if (m_bExtract)
		Set_Display_Attributes(m_pResult, 2, sParms);
	else
	{
		if( m_bCreateAttrib )
			Set_Display_Attributes(m_pResult, m_pResult->Get_Field_Count()-1, sParms);
		else
			Set_Display_Attributes(m_pResult, m_AttrField, sParms);
	}

	if( m_bExtract)
		SG_UI_Msg_Add(CSG_String::Format(_TL("%d points out of %d extracted."), m_pInput->Get_Point_Count()-m_iOrig, m_pInput->Get_Point_Count()), true);
	else
		SG_UI_Msg_Add(CSG_String::Format(_TL("%d points out of %d reclassified."), m_pInput->Get_Point_Count()-m_iOrig, m_pInput->Get_Point_Count()), true);


	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPC_Reclass_Extract::Reclass_Range(void)
{
	bool		otherOpt, noDataOpt, floating;
	int			opera;
	double		minValue, maxValue, value, others, noData, noDataValue, newValue;

	minValue	= Parameters("MIN")->asDouble();
	maxValue	= Parameters("MAX")->asDouble();
	newValue	= Parameters("RNEW")->asDouble();
	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	opera		= Parameters("ROPERATOR")->asInt();

	otherOpt	= m_bExtract ? false : Parameters("OTHEROPT")->asBool();
	noDataOpt	= m_bExtract ? false : Parameters("NODATAOPT")->asBool();

	noDataValue = m_pInput->Get_NoData_Value();

	if( (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Double) || (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Float) )
		floating = true;
	else
		floating = false;

	for (int i=0; i<m_pInput->Get_Point_Count() && Set_Progress(i, m_pInput->Get_Point_Count()); i++)
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

				m_iOrig++;
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

				m_iOrig++;
			}
		}
	}

	return;
}

//---------------------------------------------------------
void CPC_Reclass_Extract::Reclass_Single(void)
{
	bool		otherOpt, noDataOpt, floating;
	int			opera;
	double		oldValue, newValue, value, others, noData, noDataValue;

	oldValue	= Parameters("OLD")->asDouble();
	newValue	= Parameters("NEW")->asDouble();
	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	opera		= Parameters("SOPERATOR")->asInt();

	otherOpt	= m_bExtract ? false : Parameters("OTHEROPT")->asBool();
	noDataOpt	= m_bExtract ? false : Parameters("NODATAOPT")->asBool();

	noDataValue = m_pInput->Get_NoData_Value();

	if( (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Double) || (m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Float) )
		floating = true;
	else
		floating = false;


	for (int i=0; i<m_pInput->Get_Point_Count() && Set_Progress(i, m_pInput->Get_Point_Count()); i++)
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

				m_iOrig++;
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

				m_iOrig++;
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

				m_iOrig++;
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

				m_iOrig++;
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

				m_iOrig++;
			}
		}
	}

	return;
}


//---------------------------------------------------------
bool CPC_Reclass_Extract::Reclass_Table(bool bUser)
{
	bool				set, otherOpt, noDataOpt;
	int					opera, field_Min, field_Max, field_Code;
	double				value, others, noData, noDataValue;

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

	others		= Parameters("OTHERS")->asDouble();
	noData		= Parameters("NODATA")->asDouble();
	opera		= Parameters("TOPERATOR")->asInt();

	otherOpt	= m_bExtract ? false : Parameters("OTHEROPT")->asBool();
	noDataOpt	= m_bExtract ? false : Parameters("NODATAOPT")->asBool();

	noDataValue	= m_pInput->Get_NoData_Value();


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


	for (int i=0; i<m_pInput->Get_Point_Count() && Set_Progress(i, m_pInput->Get_Point_Count()); i++)
	{
		value	= m_pInput->Get_Value(i, m_AttrField);
		set		= false;

		for(int iRecord=0; iRecord<pReTab->Get_Record_Count(); iRecord++)									// reclass
		{
			pRecord		= pReTab->Get_Record(iRecord);

			if( opera == 0 )										// min <= value < max
			{
				if( value >= pRecord->asDouble(field_Min) && value < pRecord->asDouble(field_Max) )
				{
					Set_Value(i, pRecord->asDouble(field_Code));
					set = true;
					break;
				}
			}
			else if( opera == 1 )									// min <= value <= max
			{
				if( value >= pRecord->asDouble(field_Min) && value <= pRecord->asDouble(field_Max) )
				{
					Set_Value(i, pRecord->asDouble(field_Code));
					set = true;
					break;
				}
			}
			else if( opera == 2 )									// min < value <= max
			{
				if( value > pRecord->asDouble(field_Min) && value <= pRecord->asDouble(field_Max) )
				{
					Set_Value(i, pRecord->asDouble(field_Code));
					set = true;
					break;
				}
			}
			else if( opera == 3 )									// min < value < max
			{
				if( value > pRecord->asDouble(field_Min) && value < pRecord->asDouble(field_Max) )
				{
					Set_Value(i, pRecord->asDouble(field_Code));
					set = true;
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

				m_iOrig++;
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
	{
		switch (m_pInput->Get_Attribute_Type(j))
		{
		default:					m_pResult->Set_Attribute(j, m_pInput->Get_Attribute(i, j));		break;
		case SG_DATATYPE_Date:
		case SG_DATATYPE_String:	CSG_String sAttr; m_pInput->Get_Attribute(i, j, sAttr); m_pResult->Set_Attribute(j, sAttr);		break;
		}
	}

	if (!m_bExtract)
	{
		if (m_bCreateAttrib)
			m_pResult->Set_Value(m_pResult->Get_Field_Count()-1, value);
		else
			m_pResult->Set_Value(m_AttrField, value);
	}

	return;
}


//---------------------------------------------------------
void CPC_Reclass_Extract::Set_Display_Attributes(CSG_PointCloud *pPC, int iField, CSG_Parameters &sParms)
{
	if (sParms("METRIC_ATTRIB")	&& sParms("COLORS_TYPE") && sParms("METRIC_COLORS")
		&& sParms("METRIC_ZRANGE") && sParms("DISPLAY_VALUE_AGGREGATE"))
	{
		sParms("DISPLAY_VALUE_AGGREGATE")->Set_Value(3);		// highest z
		sParms("COLORS_TYPE")->Set_Value(2);                    // graduated color
		sParms("METRIC_COLORS")->asColors()->Set_Count(255);    // number of colors
		sParms("METRIC_ATTRIB")->Set_Value(iField);				// attrib
		sParms("METRIC_ZRANGE")->asRange()->Set_Range(pPC->Get_Minimum(iField), pPC->Get_Maximum(iField));
	}

	DataObject_Set_Parameters(pPC, sParms);
	DataObject_Update(pPC);

	return;
}


//---------------------------------------------------------
int CPC_Reclass_Extract::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) || !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("MODE")) )
	{
		int		iMode	= pParameters->Get_Parameter("MODE")->asInt();		// 0 == reclassify, 1 == extract
		int		Value	= pParameters->Get_Parameter("METHOD")->asInt();

		pParameters->Get_Parameter("CREATE_ATTRIB")->Set_Enabled(iMode == 0);

		// single
		pParameters->Get_Parameter("OLD"		)->Set_Enabled(Value == 0);
		pParameters->Get_Parameter("NEW"		)->Set_Enabled(Value == 0 && iMode == 0);
		pParameters->Get_Parameter("SOPERATOR"	)->Set_Enabled(Value == 0);

		// range
		pParameters->Get_Parameter("MIN"		)->Set_Enabled(Value == 1);
		pParameters->Get_Parameter("MAX"		)->Set_Enabled(Value == 1);
		pParameters->Get_Parameter("RNEW"		)->Set_Enabled(Value == 1 && iMode == 0);
		pParameters->Get_Parameter("ROPERATOR"	)->Set_Enabled(Value == 1);

		// simple table
		pParameters->Get_Parameter("RETAB"		)->Set_Enabled(Value == 2);
		pParameters->Get_Parameter("TOPERATOR"	)->Set_Enabled(Value == 2);

		// user supplied table
		pParameters->Get_Parameter("RETAB_2"	)->Set_Enabled(Value == 3);
		//pParameters->Get_Parameter("F_MIN"		)->Set_Enabled(Value == 3);
		//pParameters->Get_Parameter("F_MAX"		)->Set_Enabled(Value == 3);
		//pParameters->Get_Parameter("F_CODE"		)->Set_Enabled(Value == 3);

		// other options
		pParameters->Get_Parameter("NODATAOPT"	)->Set_Enabled(iMode == 0);
		pParameters->Get_Parameter("OTHEROPT"	)->Set_Enabled(iMode == 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NODATAOPT")) )
	{
		pParameters->Get_Parameter("NODATA"		)->Set_Enabled(pParameter->asInt() > 0);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("OTHEROPT")) )
	{
		pParameters->Get_Parameter("OTHERS"		)->Set_Enabled(pParameter->asInt() > 0);
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
