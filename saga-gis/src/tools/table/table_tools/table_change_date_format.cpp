/**********************************************************
 * Version $Id: table_change_date_format.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      table_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              table_change_date_format.cpp             //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#include "table_change_date_format.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTable_Change_Date_Format::CTable_Change_Date_Format(void)
{
	Set_Name		(_TL("Change Date Format"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Date Field"),
		_TL("")
	);

	Parameters.Add_Table("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"FMT_IN"	, _TL("Input Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("dd:mm:yyyy"),
			_TL("yyyy:mm:dd"),
			_TL("ddmmyyyy, fix size"),
			_TL("yyyymmdd, fix size"),
			_TL("ddmmyy, fix size"),
			_TL("yymmdd, fix size"),
			_TL("Julian Day"),
			_TL("Unix Time")
		)
	);

	Parameters.Add_String("FMT_IN",
		"SEP_IN"	, _TL("Separator"),
		_TL(""),
		":"
	);

	Parameters.Add_Choice("",
		"FMT_OUT"	, _TL("Output Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s",
			_TL("dd:mm:yyyy"),
			_TL("yyyy:mm:dd"),
			_TL("dd:mm:yy"),
			_TL("yy:mm:dd"),
			_TL("Julian Day"),
			_TL("Date"),
			_TL("ISO Date and Time")
		)
	);

	Parameters.Add_String("FMT_OUT",
		"SEP_OUT"	, _TL("Separator"),
		_TL(""),
		":"
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTable_Change_Date_Format::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FMT_IN" ) )
	{
		pParameters->Set_Enabled("SEP_IN" , pParameter->asInt() == 0 || pParameter->asInt() == 1);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "FMT_OUT") )
	{
		pParameters->Set_Enabled("SEP_OUT", pParameter->asInt() < 4);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TABLE")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "FIELD") )
	{
		CSG_Table	*pTable	= pParameters->Get_Parameter("TABLE")->asTable();

		if( pTable )
		{
			int	Field	= pParameters->Get_Parameter("FIELD")->asInt();

			pParameters->Set_Enabled("FMT_IN", pTable->Get_Field_Type(Field) != SG_DATATYPE_Date);
		}
		else
		{
			pParameters->Set_Enabled("FMT_IN", false);
		}
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Date_Format::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("OUTPUT")->asTable();

	if( pTable && pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Assign  (Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}
	else
	{
		pTable	= Parameters("TABLE")->asTable();
	}

	int	Field	= Parameters("FIELD")->asInt();

	//-----------------------------------------------------
	SG_Char	sep_In	= *Parameters("SEP_IN")->asString(); if( !sep_In ) sep_In  = SG_T(':');

	const SG_Char	*sep_Out	= Parameters("SEP_OUT")->asString();

	//-----------------------------------------------------
	int	fmt_In	= Parameters("FMT_IN" )->asInt();
	int	fmt_Out	= Parameters("FMT_OUT")->asInt();

	if( pTable->Get_Field_Type(Field) == SG_DATATYPE_Date )
	{
		fmt_In	= 0;
		sep_In	= SG_T('.');
	}

	pTable->Set_Field_Type(Field, SG_DATATYPE_String);

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_DateTime	Date;

		CSG_String	s(pTable->Get_Record(iRecord)->asString(Field));

		switch( fmt_In )
		{
		case 0:	// dd:mm:yyyy
			{
				int	d	= s.BeforeFirst(sep_In).asInt();
				int	m	= s.AfterFirst (sep_In).asInt();
				int	y	= s.AfterLast  (sep_In).asInt();

				Date.Set(d, (CSG_DateTime::Month)(m - 1), y);
			}
			break;

		case 1:	// yyyy:mm:dd
			{
				int	d	= s.AfterLast  (sep_In).asInt();
				int	m	= s.AfterFirst (sep_In).asInt();
				int	y	= s.BeforeFirst(sep_In).asInt();

				Date.Set(d, (CSG_DateTime::Month)(m - 1), y);
			}
			break;

		case 2:	// ddmmyyyy
			{
				int	d	= s.Left (2)   .asInt();
				int	m	= s.Mid  (2, 2).asInt();
				int	y	= s.Right(4)   .asInt();

				Date.Set(d, (CSG_DateTime::Month)(m - 1), y);
			}
			break;

		case 3:	// yyyymmdd
			{
				int	d	= s.Right(2)   .asInt();
				int	m	= s.Mid  (4, 2).asInt();
				int	y	= s.Left (4)   .asInt();

				Date.Set(d, (CSG_DateTime::Month)(m - 1), y);
			}
			break;

		case 4:	// ddmmyy
			{
				int	d	= s.Left (2)   .asInt();
				int	m	= s.Mid  (2, 2).asInt();
				int	y	= s.Right(2)   .asInt();

				Date.Set(d, (CSG_DateTime::Month)(m - 1), y);
			}
			break;

		case 5:	// yymmdd
			{
				int	d	= s.Right(2)   .asInt();
				int	m	= s.Mid  (2, 2).asInt();
				int	y	= s.Left (2)   .asInt();

				Date.Set(d, (CSG_DateTime::Month)(m - 1), y);
			}
			break;

		case 6:	// Julian Day
			Date.Set(s.asDouble());
			break;

		case 7:	// Unix Time
			Date.Set_Unix_Time(s.asInt());
			break;
		}

		//-------------------------------------------------
		int	d	= Date.Get_Day  ();
		int	m	= Date.Get_Month() + 1;
		int	y	= Date.Get_Year ();

		switch( fmt_Out )
		{
		case 0:	// dd:mm:yyyy
			s.Printf("%02d%s%02d%s%02d", d, sep_Out, m, sep_Out, y);
			break;

		case 1:	// yyyy:mm:dd
			s.Printf("%04d%s%02d%s%02d", y, sep_Out, m, sep_Out, d);
			break;

		case 2:	// dd:mm:yy
			y	= y - 100 * (int)(y / 100.0);
			s.Printf("%02d%s%02d%s%02d", d, sep_Out, m, sep_Out, y);
			break;

		case 3:	// yy:mm:dd
			y	= y - 100 * (int)(y / 100.0);
			s.Printf("%02d%s%02d%s%02d", y, sep_Out, m, sep_Out, d);
			break;

		case 4:	// Julian Day
			s.Printf("%f", Date.Get_JDN());
			break;

		case 5:	// ISO Date
			s	= Date.Format_Date();
			break;

		case 6:	// ISO Date and Time
			s	= Date.Format_ISOCombined(' ');
			break;
		}

		pTable->Get_Record(iRecord)->Set_Value(Field, s);
	}

	//-----------------------------------------------------
	switch( Parameters("FMT_OUT")->asInt() )
	{
	case 4:	pTable->Set_Field_Type(Field, SG_DATATYPE_Double);	break;	// Julian Day
	case 5:	pTable->Set_Field_Type(Field, SG_DATATYPE_Date  );	break;	// Date
	}

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
CTable_Change_Time_Format::CTable_Change_Time_Format(void)
{
	Set_Name		(_TL("Change Time Format"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"TABLE"		, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("TABLE",
		"FIELD"		, _TL("Time Field"),
		_TL("")
	);

	Parameters.Add_Table("",
		"OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"FMT_IN"	, _TL("Input Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("hh.mm.ss"),
			_TL("hh:mm:ss"),
			_TL("hhmmss, fix size"),
			_TL("hours"),
			_TL("minutes"),
			_TL("seconds")
		), 1
	);

	Parameters.Add_Choice("",
		"FMT_OUT"	, _TL("Output Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("hh.mm.ss"),
			_TL("hh:mm:ss"),
			_TL("hhmmss, fix size"),
			_TL("hours"),
			_TL("minutes"),
			_TL("seconds")
		), 5
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTable_Change_Time_Format::On_Execute(void)
{
	//-----------------------------------------------------
	int		fmt_In	= Parameters("FMT_IN" )->asInt();
	int		fmt_Out	= Parameters("FMT_OUT")->asInt();

	if( fmt_In == fmt_Out )
	{
		Error_Set(_TL("nothing to do: input format is same as output format"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("OUTPUT")->asTable();

	if( pTable && pTable != Parameters("TABLE")->asTable() )
	{
		pTable->Assign  (Parameters("TABLE")->asTable());
		pTable->Set_Name(Parameters("TABLE")->asTable()->Get_Name());
	}
	else
	{
		pTable	= Parameters("TABLE")->asTable();
	}

	//-----------------------------------------------------
	int		fTime	= Parameters("FIELD" )->asInt();

	pTable->Set_Field_Type(fTime, SG_DATATYPE_String);

	//-----------------------------------------------------
	SG_Char	sep_In	= fmt_In  == 0 ? SG_T('.') : SG_T(':');

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		CSG_String	sTime	= pRecord->asString(fTime);

		double	s;

		switch( fmt_In )
		{
		case 0:	// hh.mm.ss
		case 1:	// hh:mm:ss
			s	 = sTime.BeforeFirst(sep_In).asInt() * 3600;
			s	+= sTime.AfterFirst (sep_In).asInt() * 60;
			s	+= sTime.AfterLast  (sep_In).asDouble();
			break;

		case 2:	// hhmmss
			s	 = sTime.Left (2)   .asInt() * 3600;
			s	+= sTime.Mid  (2, 2).asInt() * 60;
			s	+= sTime.Right(2)   .asDouble();
			break;

		case 3:	// hours
			s	 = sTime.asDouble() * 3600;
			break;

		case 4:	// minutes
			s	 = sTime.asDouble() * 60;
			break;

		case 5:	// seconds
			s	 = sTime.asDouble();
			break;
		}

		switch( fmt_Out )
		{
		case 0:	case 1: case 2:
			{
				int	h	= (int)(s / 3600);	s	= s - h * 3600;
				int	m	= (int)(s /   60);	s	= s - m *   60;

				switch( fmt_Out )
				{
			//	case 0:	sTime.Printf("%02d.%02d.%02.*f", h, m, SG_Get_Significant_Decimals(s), s);	break;	// hh:mm:ss.s
			//	case 1:	sTime.Printf("%02d:%02d:%02.*f", h, m, SG_Get_Significant_Decimals(s), s);	break;	// hh:mm:ss.s
				case 0:	sTime.Printf("%02d.%02d.%02d"  , h, m, (int)(s + 0.5));	break;	// hh:mm:ss
				case 1:	sTime.Printf("%02d:%02d:%02d"  , h, m, (int)(s + 0.5));	break;	// hh:mm:ss
				case 2:	sTime.Printf("%02d%02d%02d"    , h, m, (int)(s + 0.5));	break;	// hhmmss
				}
			}
			break;

		case 3:	case 4: case 5:
			{
				switch( fmt_Out )
				{
				case 3:	s	= s / 3600;	break;	// hours
				case 4:	s	= s /   60;	break;	// minutes
			//	case 5:	s	= s       ;	break;	// seconds
				}

				sTime.Printf("%.*f", SG_Get_Significant_Decimals(s), s);
			}
			break;
		}

		pRecord->Set_Value(fTime, sTime);
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
