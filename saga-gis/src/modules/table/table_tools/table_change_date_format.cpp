/**********************************************************
 * Version $Id: table_change_date_format.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Change Date Format"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"			, _TL("Date Field"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"			, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "FMT_IN"			, _TL("Input Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|",
			_TL("dd:mm:yyyy"),
			_TL("yyyy:mm:dd"),
			_TL("ddmmyyyy, fix size"),
			_TL("yyyymmdd, fix size"),
			_TL("ddmmyy, fix size"),
			_TL("yymmdd, fix size"),
			_TL("Julian Day")
		)
	);

	Parameters.Add_String(
		pNode	, "SEP_IN"			, _TL("Separator"),
		_TL(""),
		":"
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "FMT_OUT"			, _TL("Output Format"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("dd:mm:yyyy"),
			_TL("yyyy:mm:dd"),
			_TL("ddmmyyyy, fix size"),
			_TL("yyyymmdd, fix size"),
			_TL("ddmmyy, fix size"),
			_TL("yymmdd, fix size"),
			_TL("Julian Day"),
			_TL("Date")
		)
	);

	Parameters.Add_String(
		pNode	, "SEP_OUT"			, _TL("Separator"),
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
		pParameters->Set_Enabled("SEP_OUT", pParameter->asInt() == 0 || pParameter->asInt() == 1);
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

	return( CSG_Module::On_Parameters_Enable(pParameters, pParameter) );
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
	SG_Char	sep_In	= *Parameters("SEP_IN" )->asString(); if( !sep_In  ) sep_In  = SG_T(':');
	SG_Char	sep_Out	= *Parameters("SEP_OUT")->asString(); if( !sep_Out ) sep_Out = SG_T(':');

	//-----------------------------------------------------
	int	fmt_In	= Parameters("FMT_IN" )->asInt();
	int	fmt_Out	= Parameters("FMT_OUT")->asInt();

	if( pTable->Get_Field_Type(Field) == SG_DATATYPE_Date )
	{
		fmt_In	= 0;
		sep_In	= SG_T('.');
	}

	if( fmt_Out == 7 )
	{
		fmt_Out	= 0;
		sep_Out	= SG_T('.');
	}

	pTable->Set_Field_Type(Field, SG_DATATYPE_String);

	//-----------------------------------------------------
	for(int iRecord=0; iRecord<pTable->Get_Count() && Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

		CSG_String	s(pRecord->asString(Field));

		int	y, m, d;

		switch( fmt_In )
		{
		case 0:	// dd:mm:yyyy
			d	= s.BeforeFirst(sep_In).asInt();
			m	= s.AfterFirst (sep_In).asInt();
			y	= s.AfterLast  (sep_In).asInt();
			break;

		case 1:	// yyyy:mm:dd
			d	= s.AfterLast  (sep_In).asInt();
			m	= s.AfterFirst (sep_In).asInt();
			y	= s.BeforeFirst(sep_In).asInt();
			break;

		case 2:	// ddmmyyyy
			d	= s.Left (2)   .asInt();
			m	= s.Mid  (2, 2).asInt();
			y	= s.Right(4)   .asInt();
			break;

		case 3:	// yyyymmdd
			d	= s.Right(2)   .asInt();
			m	= s.Mid  (4, 2).asInt();
			y	= s.Left (4)   .asInt();
			break;

		case 4:	// ddmmyy
			d	= s.Left (2)   .asInt();
			m	= s.Mid  (2, 2).asInt();
			y	= s.Right(2)   .asInt();
			break;

		case 5:	// yymmdd
			d	= s.Right(2)   .asInt();
			m	= s.Mid  (2, 2).asInt();
			y	= s.Left (2)   .asInt();
			break;

		case 6:	// Julian Day
			{
				CSG_DateTime	Date(s.asDouble());

				d	= Date.Get_Day  ();
				m	= Date.Get_Month();
				y	= Date.Get_Year ();
			}
			break;
		}

		//-------------------------------------------------
		switch( fmt_Out )
		{
		case 0:	s.Printf("%02d%c%02d%c%02d", d, sep_Out, m, sep_Out, y       );	break;	// dd:mm:yyyy
		case 1:	s.Printf("%02d%c%02d%c%02d", y, sep_Out, m, sep_Out, d       );	break;	// yyyy:mm:dd
		case 2:	s.Printf("%02d%02d%04d"    , d, m, y                         );	break;	// ddmmyyyy
		case 3:	s.Printf("%04d%02d%02d"    , y, m, d                         );	break;	// yyyymmdd
		case 4:	s.Printf("%02d%02d%02d"    , d, m, y - 100 * (int)(y / 100.0));	break;	// ddmmyy
		case 5:	s.Printf("%02d%02d%02d"    , y - 100 * (int)(y / 100.0), m, d);	break;	// yymmdd

		case 6:	// Julian Day
			{
				CSG_DateTime	Date(d, (CSG_DateTime::Month)(m - 1), y);

				s.Printf("%d", (int)Date.Get_JDN());
			}
			break;
		}

		pRecord->Set_Value(Field, s);
	}

	//-----------------------------------------------------
	switch( Parameters("FMT_OUT")->asInt() )
	{
	case 6:	pTable->Set_Field_Type(Field, SG_DATATYPE_Int );	break;	// Julian Day
	case 7:	pTable->Set_Field_Type(Field, SG_DATATYPE_Date);	break;	// Date
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Change Time Format"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		""
	));


	//-----------------------------------------------------
	// 2. Parameters...

	pNode	= Parameters.Add_Table(
		NULL	, "TABLE"			, _TL("Table"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"			, _TL("Time Field"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "OUTPUT"			, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "FMT_IN"			, _TL("Input Format"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("hh.mm.ss"),
			_TL("hh:mm:ss"),
			_TL("hhmmss, fix size"),
			_TL("hours"),
			_TL("minutes"),
			_TL("seconds")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "FMT_OUT"			, _TL("Output Format"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
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
			//	case 0:	sTime.Printf(SG_T("%02d.%02d.%02.*f"), h, m, SG_Get_Significant_Decimals(s), s);	break;	// hh:mm:ss.s
			//	case 1:	sTime.Printf(SG_T("%02d:%02d:%02.*f"), h, m, SG_Get_Significant_Decimals(s), s);	break;	// hh:mm:ss.s
				case 0:	sTime.Printf(SG_T("%02d.%02d.%02d"  ), h, m, (int)(s + 0.5));	break;	// hh:mm:ss
				case 1:	sTime.Printf(SG_T("%02d:%02d:%02d"  ), h, m, (int)(s + 0.5));	break;	// hh:mm:ss
				case 2:	sTime.Printf(SG_T("%02d%02d%02d"    ), h, m, (int)(s + 0.5));	break;	// hhmmss
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

				sTime.Printf(SG_T("%.*f"), SG_Get_Significant_Decimals(s), s);
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
