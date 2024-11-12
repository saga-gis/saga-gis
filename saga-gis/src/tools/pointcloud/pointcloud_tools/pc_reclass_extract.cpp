
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
#include "pc_reclass_extract.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Reclass_Extract::CPC_Reclass_Extract(void)
{
	Set_Name(_TL("Point Cloud Reclassifier / Subset Extractor"));

	Set_Author("Volker Wichmann (c) 2009, LASERDATA GmbH");

	Set_Description	(_TW(
		"The tool can be used to either reclassify a Point Cloud attribute, to extract, or drop "
		"a subset of a Point Cloud based on the values of an attribute.\n\n"
		"The tool provides three different methods for selection of points to become reclassified/extracted/deleted:"
		"<ol>"
		"<li>single value</li>"
		"<li>value range</li>"
		"<li>value ranges specified in a lookup table</li>"
		"</ol>\n\n"
		"Each of these three options provides it's own parameters. The 'new value' parameters are "
		"irrelevant in case a subset is extracted.\n\n"
		"In addition to these settings, two special cases ('NoData values' and 'other values' not "
		"included in the parameter setup) are supported:\n"
		"In mode (a) and (b) the 'NoData option' is evaluated before the method settings, in mode "
		"(c) the option is evaluated only if the NoData value isn't included in the lookup table.\n"
		"The 'other values' option is always evaluated after checking the method settings.\n\n"
		"Have in mind that subset deletion will be performed on the input point cloud!"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud("",
		"INPUT"        ,_TL("Point Cloud"),
		_TL("Point Cloud to reclassify/extract"),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("INPUT",
		"ATTRIB"       , _TL("Attribute"),
		_TL("Attribute to process."),
		false
	);

	Parameters.Add_PointCloud("",
		"RESULT"       , _TL("Result"),
		_TL("Reclassified or extracted Point Cloud."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"MODE"         , _TL("Mode of Operation"),
		_TL("Choose whether to reclassify a Point Cloud or to extract a subset from a Point Cloud."),
		CSG_String::Format("%s|%s|%s",
			_TL("Reclassify"),
			_TL("Extract Subset"),
			_TL("Drop Subset")
		), 0
	);

	Parameters.Add_Bool("",
		"CREATE_ATTRIB", _TL("Create new Attribute"),
		_TL("Check this to create a new attribute with the reclassification result. If unchecked, the existing attribute is updated."),
		false
	);

	Parameters.Add_Choice("",
		"METHOD"       , _TL("Selection Method"),
		_TL("Select the desired method: 1. a single value or a range defined by a single value is reclassified, 2. a range of values is reclassified, 3. the lookup table is used to reclassify the grid."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("single value"       ),
			_TL("value range"        ),
			_TL("simple table"       ),
			_TL("user supplied table")
		), 0
	);


	//-----------------------------------------------------
	Parameters.Add_Double("",
		"OLD"          , _TL("Value"),
		_TL("Value to reclassify, extract or drop."),
		0.
	);

	Parameters.Add_Double("",
		"NEW"          , _TL("New Value"),
		_TL("New value."),
		1.
	);

	Parameters.Add_Choice("",
		"SOPERATOR"    , _TL("Operator"),
		_TL("Select the desired operator (<;.;=; >;.); it is possible to define a range above or below the old value."),
		"=|<|<=|>=|>|<>", 0
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"MIN"          , _TL("Minimum Value"),
		_TL("Minimum value of the range to be reclassified."),
		0.
	);

	Parameters.Add_Double("",
		"MAX"          , _TL("Maximum Value"),
		_TL("Maximum value of the range to be reclassified."),
		10.
	);

	Parameters.Add_Double("",
		"RNEW"         , _TL("New Value"),
		_TL("new value"),
		5.
	);

	Parameters.Add_Choice("",
		"ROPERATOR"    , _TL("Operator"),
		_TL("Select operator: eg. min < value < max."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("minimum <= value <= maximum"),
			_TL("minimum < value < maximum"),
			_TL("value < minimum or value > maximum"),
			_TL("value <= minimum or value >= maximum")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_FixedTable("",
		"RETAB"        , _TL("Lookup Table"),
		_TL("Lookup table used in method \"table\"")
	);

	Parameters.Add_Choice("",
		"TOPERATOR"    , _TL("Operator"),
		_TL("Select the desired operator (min < value < max; min . value < max; min .value . max; min < value . max)."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("minimum <= value < maximum"),
			_TL("minimum <= value <= maximum"),
			_TL("minimum < value <= maximum"),
			_TL("minimum < value < maximum")
		), 0
	);

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"RETAB_2"      , _TL("Lookup Table"),
		_TL("Lookup table used in method \"user supplied table\""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("RETAB_2",
		"F_MIN"        , _TL("Minimum Value"),
		_TL("")
	);

	Parameters.Add_Table_Field("RETAB_2",
		"F_MAX"        , _TL("Maximum Value"),
		_TL("")
	);

	Parameters.Add_Table_Field("RETAB_2",
		"F_CODE"       , _TL("New Value"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_Node("",
		"OPTIONS"      , _TL("Special Cases"),
		_TL("Parameter settings for No-Data and all other values.")
	);

	Parameters.Add_Bool("OPTIONS",
		"NODATAOPT"    , _TL("No-Data Values"),
		_TL("Use this option to reclassify No-Data values independently of the method settings."),
		false
	);

	Parameters.Add_Double("NODATAOPT",
		"NODATA"       , _TL("New Value"),
		_TL("new value"),
		0.
	);

	Parameters.Add_Bool("OPTIONS",
		"OTHEROPT"     , _TL("Other Values"),
		_TL("Use this option to reclassify all other values that are not specified in the options above."),
		false
	);

	Parameters.Add_Double("OTHEROPT",
		"OTHERS"       , _TL("New Value"),
		_TL("new value"),
		0.
	);

	//-----------------------------------------------------
	CSG_Table *pLookup = Parameters("RETAB")->asTable();

	pLookup->Add_Field(_TL("minimum"), SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("maximum"), SG_DATATYPE_Double);
	pLookup->Add_Field(_TL("new"    ), SG_DATATYPE_Double);

	#define ADD_RECORD(min, max, new) { CSG_Table_Record &r = *pLookup->Add_Record(); r.Set_Value(0, min); r.Set_Value(1, max); r.Set_Value(2, new); }

	ADD_RECORD( 0., 10., 1.);
	ADD_RECORD(10., 20., 2.);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPC_Reclass_Extract::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("METHOD") || pParameter->Cmp_Identifier("MODE") )
	{
		int Operation = (*pParameters)("MODE"  )->asInt(); // 0 == reclassify, 1 == extract, 2 == drop
		int    Method = (*pParameters)("METHOD")->asInt();

		pParameters->Set_Enabled("RESULT"       , Operation != 2);
		pParameters->Set_Enabled("CREATE_ATTRIB", Operation == 0);

		// single
		pParameters->Set_Enabled("OLD"          , Method == 0);
		pParameters->Set_Enabled("NEW"          , Method == 0 && Operation == 0);
		pParameters->Set_Enabled("SOPERATOR"    , Method == 0);

		// range
		pParameters->Set_Enabled("MIN"          , Method == 1);
		pParameters->Set_Enabled("MAX"          , Method == 1);
		pParameters->Set_Enabled("RNEW"         , Method == 1 && Operation == 0);
		pParameters->Set_Enabled("ROPERATOR"    , Method == 1);

		// simple table
		pParameters->Set_Enabled("RETAB"        , Method == 2);

		// user supplied table
		pParameters->Set_Enabled("RETAB_2"      , Method == 3);

		// lookup table
		pParameters->Set_Enabled("TOPERATOR"    , Method >= 2);

		// other options
		pParameters->Set_Enabled("OPTIONS"      , Operation == 0);
	}

	if(	pParameter->Cmp_Identifier("NODATAOPT") )
	{
		pParameters->Set_Enabled("NODATA"       , pParameter->asInt() > 0);
	}

	if(	pParameter->Cmp_Identifier("OTHEROPT") )
	{
		pParameters->Set_Enabled("OTHERS"       , pParameter->asInt() > 0);
	}

	//-----------------------------------------------------
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Reclass_Extract::On_Execute(void)
{
	m_pInput    = Parameters("INPUT" )->asPointCloud();
	m_AttrField = Parameters("ATTRIB")->asInt();

	m_Operation = Parameters("MODE"  )->asInt();

	switch( m_Operation )
	{
	case  0: // reclass
		m_pResult = Parameters("RESULT")->asPointCloud();
		m_pResult->Create(m_pInput);
		m_pResult->Fmt_Name("%s_reclass_%s", m_pInput->Get_Name(), m_pInput->Get_Field_Name(m_AttrField));

		if( (m_bCreateAttrib = Parameters("CREATE_ATTRIB")->asBool()) == true )
		{
			m_pResult->Add_Field(CSG_String::Format("%s_reclass", m_pInput->Get_Field_Name(m_AttrField)), m_pInput->Get_Field_Type(m_AttrField));
		}
		break;

	case  1: // extract
		m_pResult = Parameters("RESULT")->asPointCloud();
		m_pResult->Create(m_pInput);
		m_pResult->Fmt_Name("%s_subset_%s", m_pInput->Get_Name(), m_pInput->Get_Field_Name(m_AttrField));
		break;

	case  2: // drop
		m_pResult = NULL;
		m_pInput->Select(); // clear selection
		break;
	}

	m_iOrig = 0; // counter of unchanged points

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	case  0: if( !Reclass_Single()     ) { return( false ); } break;
	case  1: if( !Reclass_Range ()     ) { return( false ); } break;
	case  2: if( !Reclass_Table(false) ) { return( false ); } break;
	case  3: if( !Reclass_Table(true ) ) { return( false ); } break;
	default: break;
	}

	//-----------------------------------------------------
	switch( m_Operation )
	{
	case  0: // reclass
		Set_Display_Attributes(m_bCreateAttrib ? m_pResult->Get_Field_Count() - 1 : m_AttrField);

		Message_Fmt("\n%s: %lld (%lld)\n", _TL("number of reclassified points."), m_pInput->Get_Count() - m_iOrig, m_pInput->Get_Count());
		break;

	case  1: // extract
		Set_Display_Attributes(2);

		Message_Fmt("\n%s: %lld (%lld)\n ", _TL("number of extracted points."  ), m_pInput->Get_Count() - m_iOrig, m_pInput->Get_Count());
		break;

	case  2: // drop
		m_iOrig = m_pInput->Get_Selection_Count(); m_pInput->Del_Selection();

		DataObject_Update(m_pInput);

		Message_Fmt("\n%s: %lld (%lld)\n ", _TL("number of dropped points."    ), m_iOrig, m_pInput->Get_Count() + m_iOrig);
		break;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Reclass_Extract::Reclass_Single(void)
{
	double    oldValue = Parameters("OLD"   )->asDouble();
	double    newValue = Parameters("NEW"   )->asDouble();
	double      others = Parameters("OTHERS")->asDouble();
	double      noData = Parameters("NODATA")->asDouble();

	int          opera = Parameters("SOPERATOR")->asInt();

	bool      otherOpt = (m_Operation == 1) ? false : Parameters("OTHEROPT")->asBool();
	bool     noDataOpt = (m_Operation == 1) ? false : Parameters("NODATAOPT")->asBool();

	bool      floating = m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Double || m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Float;

	double noDataValue = m_pInput->Get_NoData_Value();

	for(sLong i=0, n=m_pInput->Get_Count(); i<n && Set_Progress(i, n); i++)
	{
		double value = floating ? m_pInput->Get_Value(i, m_AttrField) : (int)m_pInput->Get_Value(i, m_AttrField);

		if( noDataOpt == true && value == noDataValue ) // noData option
		{
			Set_Value(i, noData);
		}
		else
		{
			bool set = false;

			switch( opera )
			{
			case 0: set = value == oldValue; break; // operator ==
			case 1: set = value <  oldValue; break; // operator <
			case 2: set = value <= oldValue; break; // operator <=
			case 3: set = value >= oldValue; break; // operator >=
			case 4: set = value >  oldValue; break; // operator >
			case 5: set = value != oldValue; break; // operator <>
			}

			if( set ) // reclass old value
			{
				if( m_Operation == 2 ) // drop
				{
					m_pInput->Select(i, true);
				}
				else
				{
					Set_Value(i, newValue);
				}
			}
			else if( otherOpt == true && value != noDataValue ) // other values option
			{
				Set_Value(i, others);
			}
			else
			{
				if( (m_Operation == 0) )
				{
					Set_Value(i, value); // or original value
				}

				m_iOrig++;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CPC_Reclass_Extract::Reclass_Range(void)
{
	double    minValue = Parameters("MIN"   )->asDouble();
	double    maxValue = Parameters("MAX"   )->asDouble();
	double    newValue = Parameters("RNEW"  )->asDouble();
	double      others = Parameters("OTHERS")->asDouble();
	double      noData = Parameters("NODATA")->asDouble();

	int          opera = Parameters("ROPERATOR")->asInt();

	bool      otherOpt = (m_Operation == 1) ? false : Parameters("OTHEROPT" )->asBool();
	bool     noDataOpt = (m_Operation == 1) ? false : Parameters("NODATAOPT")->asBool();

	bool      floating = m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Double || m_pInput->Get_Field_Type(m_AttrField) == SG_DATATYPE_Float;

	double noDataValue = m_pInput->Get_NoData_Value();

	for(sLong i=0, n=m_pInput->Get_Count(); i<n && Set_Progress(i, n); i++)
	{
		double value = floating ? m_pInput->Get_Value(i, m_AttrField) : (int)m_pInput->Get_Value(i, m_AttrField);

		if( noDataOpt == true && value == noDataValue ) // noData option
		{
			Set_Value(i, noData);
		}
		else
		{
			bool set = false;

			switch( opera )
			{
			case 0: set = minValue <= value && value <= maxValue; break; // min <= value <= max
			case 1: set = minValue <  value && value <  maxValue; break; // min <  value <  max
			case 2: set = minValue >  value || value >  maxValue; break; // value <  min or value >  max
			case 3: set = minValue >= value || value >= maxValue; break; // value <= min or value >= max
			}

			if( set ) // reclass old range
			{
				if( m_Operation == 2 ) // drop
				{
					m_pInput->Select(i, true);
				}
				else
				{
					Set_Value(i, newValue);
				}
			}
			else if( otherOpt == true && value != noDataValue ) // other values option
			{
				Set_Value(i, others);
			}
			else // or original value
			{
				if( (m_Operation == 0) )
				{
					Set_Value(i, value);
				}

				m_iOrig++;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CPC_Reclass_Extract::Reclass_Table(bool bUser)
{
	CSG_Table  *pReTab = Parameters(bUser ? "RETAB_2" : "RETAB")->asTable();

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

	int     field_Min  = bUser ? Parameters("F_MIN" )->asInt() : 0;
	int     field_Max  = bUser ? Parameters("F_MAX" )->asInt() : 1;
	int     field_Code = bUser ? Parameters("F_CODE")->asInt() : 2;

	int          opera = Parameters("TOPERATOR")->asInt();

	double      others = Parameters("OTHERS")->asDouble();
	double      noData = Parameters("NODATA")->asDouble();

	bool      otherOpt = (m_Operation == 1) ? false : Parameters("OTHEROPT" )->asBool();
	bool     noDataOpt = (m_Operation == 1) ? false : Parameters("NODATAOPT")->asBool();

	double noDataValue = m_pInput->Get_NoData_Value();

	for(sLong i=0, n=m_pInput->Get_Count(); i<n && Set_Progress(i, n); i++)
	{
		double value = m_pInput->Get_Value(i, m_AttrField); bool set = false;

		for(sLong iRecord=0; !set && iRecord<pReTab->Get_Count(); iRecord++) // reclass
		{
			CSG_Table_Record *pRecord = pReTab->Get_Record(iRecord);

			switch( opera )
			{
			case 0: set = value >= pRecord->asDouble(field_Min) && value <  pRecord->asDouble(field_Max); break; // min <= value <  max
			case 1: set = value >= pRecord->asDouble(field_Min) && value <= pRecord->asDouble(field_Max); break; // min <= value <= max
			case 2: set = value >  pRecord->asDouble(field_Min) && value <= pRecord->asDouble(field_Max); break; // min <  value <= max
			case 3: set = value >  pRecord->asDouble(field_Min) && value <  pRecord->asDouble(field_Max); break; // min <  value <  max
			}

			if( set == true && m_Operation != 2 )
			{
				Set_Value(i, pRecord->asDouble(field_Code));
			}
		}

		if( set == false )
		{
			if( noDataOpt == true && value == noDataValue ) // noData option
			{
				Set_Value(i, noData);
			}
			else if( otherOpt == true && value != noDataValue) // other values option
			{
				Set_Value(i, others);
			}
			else // or original value
			{
				if( (m_Operation == 0) )
				{
					Set_Value(i, value);
				}

				m_iOrig++;
			}
		}
		else if( m_Operation == 2 ) // drop
		{
			m_pInput->Select(i, true);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPC_Reclass_Extract::Set_Value(sLong i, double value)
{
	if( m_pResult )
	{
		m_pResult->Add_Point(m_pInput->Get_X(i), m_pInput->Get_Y(i), m_pInput->Get_Z(i));

		for(int j=0; j<m_pInput->Get_Attribute_Count(); j++)
		{
			switch( m_pInput->Get_Attribute_Type(j) )
			{
			default                : {
				m_pResult->Set_Attribute(j, m_pInput->Get_Attribute(i, j));
				break; }

			case SG_DATATYPE_Date  :
			case SG_DATATYPE_String: { CSG_String s;
				m_pInput ->Get_Attribute(i, j, s);
				m_pResult->Set_Attribute(j, s);
				break; }
			}
		}

		if( m_Operation == 0 )
		{
			m_pResult->Set_Value(m_bCreateAttrib ? m_pResult->Get_Field_Count() - 1 : m_AttrField, value);
		}
	}

	return;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPC_Reclass_Extract::Set_Display_Attributes(int Attribute)
{
	if( m_pResult )
	{
		DataObject_Update(m_pResult);

		DataObject_Set_Parameter(m_pResult, "METRIC_ATTRIB", Attribute);
		DataObject_Set_Parameter(m_pResult, "METRIC_ZRANGE",
			m_pResult->Get_Minimum(Attribute),
			m_pResult->Get_Maximum(Attribute)
		);

		DataObject_Update(m_pResult);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
