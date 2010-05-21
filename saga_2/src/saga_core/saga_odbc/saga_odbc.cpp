
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       saga_api                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       odbc.cpp                        //
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
#include "saga_odbc.h"

//---------------------------------------------------------
#define OTL_ODBC_MULTI_MODE
#define OTL_STL				// Turn on STL features
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP			// Turn on ANSI C++ typecasts
#endif

#ifdef _SAGA_UNICODE
//#define OTL_UNICODE
#endif

#ifdef _SAGA_LINUX
#define OTL_ODBC_UNIX
#endif

#include "otlv4.h"				// include the OTL 4 header file

//---------------------------------------------------------
using namespace std;

//---------------------------------------------------------
#include <sql.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Connections	g_Connections;

//---------------------------------------------------------
CSG_ODBC_Connections &	SG_ODBC_Get_Connection_Manager(void)
{
	return( g_Connections );
}

//---------------------------------------------------------
bool SG_ODBC_is_Supported(void)
{
	return( true );
}

//---------------------------------------------------------
void _Error_Message(const CSG_String &Message, const CSG_String &Additional = SG_T(""))
{
	SG_UI_Msg_Add_Execution(Message, true, SG_UI_MSG_STYLE_FAILURE);

	CSG_String	s(Message);

	s	+= SG_T(":\n");

	if( Additional )
	{
		s	+= Additional;
		s	+= SG_T("\n");
	}

	SG_UI_Msg_Add_Error(s);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define m_Connection	(*((otl_connect *)m_pConnection))


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Connection::CSG_ODBC_Connection(const CSG_String &Server, const CSG_String &User, const CSG_String &Password, const CSG_String &Directory)
{
	CSG_String	s;

	m_DBMS			= SG_ODBC_DBMS_Unknown;
	m_Size_Buffer	= 1;

	if( User.Length() > 0 )
	{
		s	+= CSG_String::Format(SG_T("UID=%s;"), User.c_str());
		s	+= CSG_String::Format(SG_T("PWD=%s;"), Password.c_str());
	}

	s		+= CSG_String::Format(SG_T("DSN=%s;"), Server.c_str());

	m_pConnection	= new otl_connect();

	try
	{
		m_Connection.rlogon(SG_STR_SGTOMB(s));
	}
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
	}

	if( !m_Connection.connected )
	{
		delete(((otl_connect *)m_pConnection));

		m_pConnection	= NULL;
	}
	else
	{
		m_DSN	= Server;

		s		= Get_DBMS_Name();

		     if( !s.Cmp(SG_T("PostgreSQL")) )
		{
			m_DBMS	= SG_ODBC_DBMS_PostgreSQL;
		}
		else if( !s.Cmp(SG_T("ACCESS")) )
		{
			m_DBMS	= SG_ODBC_DBMS_Access;
		}

		Set_Size_Buffer(is_Access() ? 1 : 50);

		Set_Size_LOB_Max(4 * 32767);
	}
}

//---------------------------------------------------------
CSG_ODBC_Connection::~CSG_ODBC_Connection(void)
{
	if( is_Connected() )
	{
		delete(((otl_connect *)m_pConnection));

		m_pConnection	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Set_Size_Buffer(int Size)
{
	if( Size > 0 )
	{
		m_Size_Buffer	= Size;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
int CSG_ODBC_Connection::Get_Size_LOB_Max(void)	const
{
	return( is_Connected() ? m_Connection.get_max_long_size() : 0 );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Set_Size_LOB_Max(int Size)
{
	if( is_Connected() && Size > 0 )
	{
		m_Connection.set_max_long_size(Size);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::_Get_DBMS_Info(int What) const
{
	CSG_String	Result;

	if( is_Connected() )
	{
		SQLSMALLINT	nBuffer;
		SQLTCHAR	Buffer[256];

		SQLGetInfo(m_Connection.get_connect_struct().get_hdbc(), What, (SQLPOINTER)Buffer, 255, &nBuffer);

		Result	= (const SG_Char *)Buffer;
	}

	return( Result );
}

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::Get_DBMS_Name(void) const
{
	return( _Get_DBMS_Info(SQL_DBMS_NAME) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::Get_Tables(void) const
{
	CSG_String	Tables;

	if( is_Connected() )
	{
		try
		{
			otl_stream	Stream(m_Size_Buffer, "$SQLTables", m_Connection);	// get a list of all tables in the current database.

			while( !Stream.eof() )
			{
				std::string	Catalog, Schema, Table, Type, Remarks;

				Stream >> Catalog >> Schema >> Table >> Type >> Remarks;

				Tables	+= Table.c_str();
				Tables	+= SG_T("|");
			}
		}
		catch( otl_exception &e )
		{
			_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
		}
	}

	return( Tables );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Exists(const CSG_String &Table_Name) const
{
	if( is_Connected() )
	{
		try
		{
			otl_stream	Stream(m_Size_Buffer, "$SQLTables", m_Connection);	// get a list of all tables in the current database.

			while( !Stream.eof() )
			{
				std::string	Catalog, Schema, Table, Type, Remarks;

				Stream >> Catalog >> Schema >> Table >> Type >> Remarks;

				if( !Table_Name.Cmp(CSG_String(Table.c_str())) )
				{
					return( true );
				}
			}
		}
		catch( otl_exception &e )
		{
			_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_Table CSG_ODBC_Connection::Get_Field_Desc(const CSG_String &Table_Name) const
{
	CSG_Table	Fields;

	Fields.Set_Name(CSG_String::Format(SG_T("%s [%s]"), Table_Name.c_str(), LNG("Field Description")));

	if( is_Connected() )
	{
		try
		{
			int				i, n;
			std::string		s;
			otl_column_desc	*desc;
			otl_stream		Stream;

			Stream.set_all_column_types(otl_all_num2str|otl_all_date2str);

			Stream.open(m_Size_Buffer, SG_STR_SGTOMB(CSG_String::Format(SG_T("$SQLColumns $3:'%s'"), Table_Name.c_str())), m_Connection);	// get a list of all columns.

			desc	= Stream.describe_select(n);

			for(i=0; i<n; i++)
			{
				Fields.Add_Field(CSG_String(desc[i].name), SG_DATATYPE_String);
			}

			while( !Stream.eof() )
			{
				CSG_Table_Record	*pField	= Fields.Add_Record();

				for(i=0; i<n; i++)
				{
					Stream >> s;

					pField->Set_Value(i, CSG_String(s.c_str()));
				}
			}
		}
		catch( otl_exception &e )
		{
			_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
		}
	}

	return( Fields );
}

//---------------------------------------------------------
CSG_String CSG_ODBC_Connection::Get_Field_Names(const CSG_String &Table_Name) const
{
	CSG_Table	Fields	= Get_Field_Desc(Table_Name);

	CSG_String	Names;

	for(int i=0; i<Fields.Get_Count(); i++)
	{
		Names	+= Fields[i].asString(3);
		Names	+= SG_T("|");
	}

	return( Names );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_ODBC_Connection::_Get_Type_To_SQL(TSG_Data_Type Type)
{
	switch( Type )
	{
	case SG_DATATYPE_String:	return( otl_var_char   );		// strings

	case SG_DATATYPE_Color:
	case SG_DATATYPE_Char:
	case SG_DATATYPE_Short:
	case SG_DATATYPE_Int:
	case SG_DATATYPE_Long:		return( otl_var_int    );		// non-floating point numbers

	case SG_DATATYPE_Float:		return( otl_var_float  );		// 32-bit floating point numbers
	case SG_DATATYPE_Double:	return( otl_var_double );		// 64-bit floating point numbers

	case SG_DATATYPE_Date:		return( otl_var_char );			// dates

	case SG_DATATYPE_Binary:	return( otl_var_varchar_long );		// dates
	}

	return( -1 );
}

//---------------------------------------------------------
TSG_Data_Type CSG_ODBC_Connection::_Get_Type_From_SQL(int Type)
{
	switch( Type )
	{
	case otl_var_char:			return( SG_DATATYPE_String );	//  1 | null terminated string
	case otl_var_short:			return( SG_DATATYPE_Short  );	//  6 | signed 16-bit integer
	case otl_var_int:			return( SG_DATATYPE_Int    );	//  4 | signed 32-bit integer
	case otl_var_unsigned_int:	return( SG_DATATYPE_DWord  );	//  5 | unsigned 32-bit integer
	case otl_var_long_int:		return( SG_DATATYPE_Long   );	//  7 | signed 32-bit integer (for 32-bit, and LLP64 C++ compilers), signed 64-bit integer (for LP-64 C++ compilers)
	case otl_var_bigint:		return( SG_DATATYPE_Long   );	// 20 | MS SQL Server, DB2, MySQL, PostgreSQL, etc. BIGINT (signed 64-bit integer) type
	case otl_var_float:			return( SG_DATATYPE_Float  );	//  3 | 4-byte floating point number
	case otl_var_double:		return( SG_DATATYPE_Double );	//  2 | 8-byte floating point number

	case otl_var_db2date:		return( SG_DATATYPE_String );	// 17 | DB2 DATE data type
	case otl_var_db2time:		return( SG_DATATYPE_String );	// 16 | DB2 TIME data type
	case otl_var_ltz_timestamp:	return( SG_DATATYPE_String );	// 19 | Oracle 9i/10g/11g TIMESTAMP WITH LOCAL TIME ZONE type
	case otl_var_timestamp:		return( SG_DATATYPE_String );	//  8 | data type that is mapped into Oracle date/timestamp, DB2 timestamp, MS SQL datetime/datetime2/time/date, Sybase timestamp, etc.
	case otl_var_tz_timestamp:	return( SG_DATATYPE_String );	// 18 | Oracle timestamp with timezone type

	case otl_var_varchar_long:	return( SG_DATATYPE_String );	//  9 | data type that is mapped into LONG in Oracle 7/8/9/10/11, TEXT in MS SQL Server and Sybase, CLOB in DB2
	case otl_var_raw:			return( SG_DATATYPE_Binary );	// 23 | RAW, BINARY, VARBINARY, BYTEA, VARCHAR BYTE, CHAR BYTE, etc.
	case otl_var_raw_long:		return( SG_DATATYPE_Binary );	// 10 | data type that is mapped into LONG RAW in Oracle, IMAGE in MS SQL Server and Sybase, BLOB in DB2
	case otl_var_clob:			return( SG_DATATYPE_Binary );	// 11 | data type that is mapped into CLOB in Oracle 8/9/10/11
	case otl_var_blob:			return( SG_DATATYPE_Binary );	// 12 | data type that is mapped into BLOB in Oracle 8/9/10/11
	}

	return( SG_DATATYPE_Undefined );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Execute(const CSG_String &SQL, bool bCommit)
{
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	try
	{
		m_Connection.direct_exec(SG_STR_SGTOMB(SQL));

		return( bCommit ? Commit() : true );

	//	if( m_Connection.direct_exec(SG_STR_SGTOMB(SQL)) >= 0 )
	//	{
	//		return( bCommit ? Commit() : true );
	//	}

	//	_Error_Message(LNG("sql excution error"), SQL);
	}
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Commit(void)
{
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	try
	{
		m_Connection.commit();

		return( true );
	}
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Rollback(void)
{
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	try
	{
		m_Connection.rollback();

		return( true );
	}
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Create(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit)
{
	if( Table.Get_Field_Count() <= 0 )
	{
		_Error_Message(LNG("no attributes in table"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	SQL;

	SQL.Printf(SG_T("CREATE TABLE \"%s\"("), Table_Name.c_str());

	for(int iField=0; iField<Table.Get_Field_Count(); iField++)
	{
		CSG_String	s;

		switch( Table.Get_Field_Type(iField) )
		{
		default:
		case SG_DATATYPE_String:
			s	= CSG_String::Format(SG_T("VARCHAR(%d)"), Table.Get_Field_Length(iField));
			break;

		case SG_DATATYPE_Char:
			s	= SG_T("SMALLINT");
			break;

		case SG_DATATYPE_Short:
			s	= SG_T("SMALLINT");
			break;

		case SG_DATATYPE_Int:
			s	= SG_T("INT");
			break;

		case SG_DATATYPE_Color:
			s	= SG_T("INT");
			break;

		case SG_DATATYPE_Long:
			s	= SG_T("INT");
			break;

		case SG_DATATYPE_Float:
			s	= SG_T("FLOAT");
			break;

		case SG_DATATYPE_Double:
			s	= is_Postgres()
				? SG_T("DOUBLE PRECISION")
				: SG_T("DOUBLE");
			break;

		case SG_DATATYPE_Binary:
			s	= SG_T("%s VARBINARY");
			break;
		}

		if( iField > 0 )
		{
			SQL	+= SG_T(", ");
		}

		SQL	+= CSG_String::Format(SG_T("%s %s"), Table.Get_Field_Name(iField), s.c_str());
	}

	SQL	+= SG_T(")");

	//-----------------------------------------------------
	return( Execute(SQL, bCommit) );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Drop(const CSG_String &Table_Name, bool bCommit)
{
	if( !Table_Exists(Table_Name) )
	{
		_Error_Message(LNG("database table does not exist"));

		return( false );
	}

	return( Execute(CSG_String::Format(SG_T("DROP TABLE \"%s\""), Table_Name.c_str()), bCommit) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Insert(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit)
{
	//-----------------------------------------------------
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	if( !Table_Exists(Table_Name) )
	{
		return( false );
	}

	CSG_Table	Fields	= Get_Field_Desc(Table_Name);

	if( Fields.Get_Count() != Table.Get_Field_Count() )
	{
		return( false );
	}

	//-----------------------------------------------------
	try
	{
		bool	bLOB	= false;

		int				iField, iRecord;
		CSG_String		Insert;
		otl_stream		Stream;

		//-------------------------------------------------
		Insert.Printf(SG_T("INSERT INTO %s VALUES("), Table_Name.c_str());

		for(iField=0; iField<Table.Get_Field_Count(); iField++)
		{
			if( iField > 0 )
			{
				Insert	+= SG_T(",");
			}

			Insert	+= CSG_String::Format(SG_T(":f%d"), 1 + iField);

			switch( Table.Get_Field_Type(iField) )
			{
			default:
			case SG_DATATYPE_String:	Insert	+= SG_T("<varchar>");	break;
			case SG_DATATYPE_Date:		Insert	+= SG_T("<char[12]>");	break;
			case SG_DATATYPE_Char:		Insert	+= SG_T("<char>");		break;
			case SG_DATATYPE_Short:		Insert	+= SG_T("<short>");		break;
			case SG_DATATYPE_Int:		Insert	+= SG_T("<int>");		break;
			case SG_DATATYPE_Color:		Insert	+= SG_T("<long>");		break;
			case SG_DATATYPE_Long:		Insert	+= SG_T("<long>");		break;
			case SG_DATATYPE_Float:		Insert	+= SG_T("<float>");		break;
			case SG_DATATYPE_Double:	Insert	+= SG_T("<double>");	break;
			}
		}

		Insert	+= SG_T(")");

		Stream.set_all_column_types(otl_all_date2str);
		Stream.set_lob_stream_mode(bLOB);
		Stream.open(bLOB ? 1 : m_Size_Buffer, SG_STR_SGTOMB(Insert), m_Connection);

		string	valString;

		//-------------------------------------------------
		for(iRecord=0; iRecord<Table.Get_Count() && SG_UI_Process_Set_Progress(iRecord, Table.Get_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= Table.Get_Record(iRecord);

			for(iField=0; iField<Table.Get_Field_Count(); iField++)
			{
				switch( Table.Get_Field_Type(iField) )
				{
				default:
				case SG_DATATYPE_String:
				case SG_DATATYPE_Date:
					valString	= SG_STR_SGTOMB(pRecord->asString(iField));
					Stream << valString;
					break;

				case SG_DATATYPE_Char:		Stream << (char)pRecord->asChar  (iField);	break;
				case SG_DATATYPE_Short:		Stream <<       pRecord->asShort (iField);	break;
				case SG_DATATYPE_Int:		Stream <<       pRecord->asInt   (iField);	break;
				case SG_DATATYPE_Color:
				case SG_DATATYPE_Long:		Stream << (long)pRecord->asInt   (iField);	break;
				case SG_DATATYPE_Float:		Stream <<       pRecord->asFloat (iField);	break;
				case SG_DATATYPE_Double:	Stream <<       pRecord->asDouble(iField);	break;
				}
			}
		}
	}
	//-----------------------------------------------------
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Save(const CSG_String &Table_Name, const CSG_Table &Table, bool bCommit)
{
	//-----------------------------------------------------
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	if( Table_Exists(Table_Name) && !Table_Drop(Table_Name, bCommit) )
	{
		return( false );
	}

	if( !Table_Create(Table_Name, Table, bCommit) )
	{
		return( false );
	}

	if( !Table_Insert(Table_Name, Table, bCommit) )
	{
		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::_Table_Load(CSG_Table &Table, const CSG_String &Select, const CSG_String &Name, bool bLOB)
{
	//-----------------------------------------------------
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	//-----------------------------------------------------
	try
	{
		int				valInt, iField, nFields;
		long			valLong;
		float			valFloat;
		double			valDouble;
		string			valString;
		otl_long_string	valRaw(m_Connection.get_max_long_size());
		otl_column_desc	*Fields;
		otl_stream		Stream;
		CSG_Bytes		BLOB;

		Stream.set_all_column_types	(otl_all_date2str);
		Stream.set_lob_stream_mode	(bLOB);
		Stream.open					(bLOB ? 1 : m_Size_Buffer, SG_STR_SGTOMB(Select), m_Connection);

		Fields	= Stream.describe_select(nFields);

		if( Fields == NULL || nFields <= 0 )
		{
			_Error_Message(LNG("no fields in selection"));

			return( false );
		}

		//-------------------------------------------------
		Table.Destroy();
		Table.Set_Name(Name);

		for(iField=0; iField<nFields; iField++)
		{
			if( _Get_Type_From_SQL(Fields[iField].otl_var_dbtype) == SG_DATATYPE_Undefined )
			{
				return( false );
			}

			Table.Add_Field(Fields[iField].name, _Get_Type_From_SQL(Fields[iField].otl_var_dbtype));
		}

		//-------------------------------------------------
		while( !Stream.eof() && SG_UI_Process_Get_Okay() )	// while not end-of-data
		{
			CSG_Table_Record	*pRecord	= Table.Add_Record();

			for(iField=0; iField<nFields; iField++)
			{
				switch( Table.Get_Field_Type(iField) )
				{
				case SG_DATATYPE_String:	Stream >> valString; if( Stream.is_null() ) pRecord->Set_NoData(iField); else pRecord->Set_Value(iField, CSG_String(valString.c_str()));	break;
				case SG_DATATYPE_Short:			
				case SG_DATATYPE_Int:		Stream >> valInt;    if( Stream.is_null() ) pRecord->Set_NoData(iField); else pRecord->Set_Value(iField, valInt);		break;
				case SG_DATATYPE_DWord:
				case SG_DATATYPE_Long:		Stream >> valLong;   if( Stream.is_null() ) pRecord->Set_NoData(iField); else pRecord->Set_Value(iField, valLong);		break;
				case SG_DATATYPE_Float:		Stream >> valFloat;  if( Stream.is_null() ) pRecord->Set_NoData(iField); else pRecord->Set_Value(iField, valFloat);		break;
				case SG_DATATYPE_Double:	Stream >> valDouble; if( Stream.is_null() ) pRecord->Set_NoData(iField); else pRecord->Set_Value(iField, valDouble);	break;
				case SG_DATATYPE_Binary:
					Stream >> valRaw;

					if( Stream.is_null() )
					{
						pRecord->Set_NoData(iField);
					}
					else
					{
						BLOB.Clear();

						for(int i=0; i<valRaw.len(); i++)
						{
							BLOB.Add((BYTE)valRaw[i]);
						}

						pRecord->Set_Value(iField, BLOB);
					}
					break;
				}
			}
		}
	}
	//-----------------------------------------------------
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Load(CSG_Table &Table, const CSG_String &Table_Name, bool bLOB)
{
	return( _Table_Load(Table, CSG_String::Format(SG_T("SELECT * FROM \"%s\""), Table_Name.c_str()), Table_Name, bLOB) );
}

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Load(CSG_Table &Table, const CSG_String &Tables, const CSG_String &Fields, const CSG_String &Where, const CSG_String &Order, bool bLOB)
{
	CSG_String	Select;

	Select.Printf(SG_T("SELECT %s FROM %s"), Fields.c_str(), Tables.c_str());

	if( Where.Length() )
	{
		Select	+= SG_T(" WHERE ") + Where;
	}

	if( Order.Length() )
	{
		Select	+= SG_T(" ORDER BY ") + Order;
	}

	return( _Table_Load(Table, Select, Table.Get_Name(), bLOB) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connection::Table_Load_BLOBs(CSG_Bytes_Array &BLOBs, const CSG_String &Table_Name, const CSG_String &Field, const CSG_String &Where, const CSG_String &Order)
{
	//-----------------------------------------------------
	if( !is_Connected() )
	{
		_Error_Message(LNG("no database connection"));

		return( false );
	}

	//-----------------------------------------------------
	try
	{
		bool			bLOB	= true;
		int				nFields;
		otl_column_desc	*Fields;
		otl_long_string	valRaw(m_Connection.get_max_long_size());
		otl_stream		Stream;
		CSG_String		Select;

		//-------------------------------------------------
		Select.Printf(SG_T("SELECT %s FROM %s"), Field.c_str(), Table_Name.c_str());

		if( Where.Length() )
		{
			Select	+= SG_T(" WHERE ") + Where;
		}

		if( Order.Length() )
		{
			Select	+= SG_T(" ORDER BY ") + Order;
		}

		//-------------------------------------------------
		Stream.set_lob_stream_mode	(bLOB);
		Stream.open					(bLOB ? 1 : m_Size_Buffer, SG_STR_SGTOMB(Select), m_Connection);

		Fields	= Stream.describe_select(nFields);

		if( Fields == NULL || nFields <= 0 )
		{
			_Error_Message(LNG("no fields in selection"));

			return( false );
		}

		if( nFields != 1 )
		{
			_Error_Message(LNG("more than one field in selection"));

			return( false );
		}

		if( _Get_Type_From_SQL(Fields[0].otl_var_dbtype) != SG_DATATYPE_Binary )//|| _Get_Type_From_SQL(Fields[0].otl_var_dbtype) != SG_DATATYPE_String )
		{
			_Error_Message(LNG("field cannot be mapped to binary object"));

			return( false );
		}

		//-------------------------------------------------
		BLOBs.Destroy();

		while( !Stream.eof() && SG_UI_Process_Get_Okay() )	// while not end-of-data
		{
			CSG_Bytes	*pBLOB	= BLOBs.Add();

			Stream >> valRaw;

			if( !Stream.is_null() )
			{
				for(int i=0; i<valRaw.len(); i++)
				{
					pBLOB->Add((BYTE)valRaw[i]);
				}
			}
		}
	}
	//-----------------------------------------------------
	catch( otl_exception &e )
	{
		_Error_Message((const char *)e.msg, CSG_String::Format(SG_T("%s [%s]"), e.stm_text, e.var_info));

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Connections::CSG_ODBC_Connections(void)
{
	otl_connect::otl_initialize(); // initialize ODBC environment

	m_hEnv			= SQL_NULL_HENV;
	m_nConnections	= 0;
	m_pConnections	= NULL;

	Create();
}

//---------------------------------------------------------
CSG_ODBC_Connections::~CSG_ODBC_Connections(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_ODBC_Connections::Create(void)
{
	Destroy();

#if ODBCVER >= 0x0300
	SQLRETURN	r	= SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
#else
	SQLRETURN	r	= SQLAllocEnv(&henv_);
#endif

	if( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO )
	{
		SG_UI_Msg_Add_Error(SG_T("Failed to allocate environment handle"));

		return( false );
	}

#if ODBCVER >= 0x0300
	// this should immediately follow an AllocEnv per ODBC3
	SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);
#endif

	return( true );
}

//---------------------------------------------------------
bool CSG_ODBC_Connections::Destroy(void)
{
	if( m_pConnections )
	{
		for(int i=0; i<m_nConnections; i++)
		{
			delete(m_pConnections[i]);
		}

		SG_Free(m_pConnections);

		m_nConnections	= 0;
		m_pConnections	= NULL;
	}

	if( m_hEnv != SQL_NULL_HENV )
	{
#if ODBCVER >= 0x0300
		SQLRETURN	r	= SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
#else
		SQLRETURN	r	= SQLFreeEnv(m_hEnv);
#endif

		if( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO )
		{
			SG_UI_Msg_Add_Error(SG_T("Failed to close ODBC connection."));
		}

		m_hEnv	= SQL_NULL_HENV;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Connection * CSG_ODBC_Connections::Add_Connection(const CSG_String &Server, const CSG_String &User, const CSG_String &Password, const CSG_String &Directory)
{
	CSG_ODBC_Connection	*pConnection	= new CSG_ODBC_Connection(Server, User, Password, Directory);

	if( pConnection )
	{
		if( pConnection->is_Connected() )
		{
			m_pConnections	= (CSG_ODBC_Connection **)SG_Realloc(m_pConnections, (m_nConnections + 1) * sizeof(CSG_ODBC_Connection *));

			m_pConnections[m_nConnections++]	= pConnection;
		}
		else
		{
			delete(pConnection);

			pConnection	= NULL;
		}
	}

	return( pConnection );
}

//---------------------------------------------------------
CSG_ODBC_Connection *  CSG_ODBC_Connections::Get_Connection(const CSG_String &Server)
{
	for(int i=0; i<m_nConnections; i++)
	{
		if( m_pConnections[i]->Get_Server().Cmp(Server) == 0 )
		{
			return( m_pConnections[i] );
		}
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_ODBC_Connections::Del_Connection(int Index, bool bCommit)
{
	if( Index >= 0 && Index < m_nConnections )
	{
		if( bCommit )
		{
			m_pConnections[Index]->Commit();
		}
		else
		{
			m_pConnections[Index]->Rollback();
		}

		delete(m_pConnections[Index]);

		for(m_nConnections--; Index<m_nConnections; Index++)
		{
			m_pConnections[Index]	= m_pConnections[Index + 1];
		}

		m_pConnections	= (CSG_ODBC_Connection **)SG_Realloc(m_pConnections, m_nConnections * sizeof(CSG_ODBC_Connection *));

		return( true );
	}

	return( false );
}

bool CSG_ODBC_Connections::Del_Connection(const CSG_String &Server, bool bCommit)
{
	for(int i=0; i<m_nConnections; i++)
	{
		if( m_pConnections[i]->Get_Server().Cmp(Server) == 0 )
		{
			return( Del_Connection(i, bCommit) );
		}
	}

	return( false );
}

bool CSG_ODBC_Connections::Del_Connection(CSG_ODBC_Connection *pConnection, bool bCommit)
{
	return( !pConnection ? false : Del_Connection(pConnection->Get_Server(), bCommit) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Strings CSG_ODBC_Connections::Get_Servers(void)
{
	CSG_Strings	Servers;

	SQLRETURN	r;
	SQLSMALLINT	dsnlen, dsclen;
	SQLTCHAR	dsn[SQL_MAX_DSN_LENGTH + 1], dsc[256];

	r	= SQLDataSources(m_hEnv, SQL_FETCH_FIRST,
			(SQLTCHAR *)dsn, SQL_MAX_DSN_LENGTH + 1, &dsnlen,
			(SQLTCHAR *)dsc, 256,                    &dsclen
		);

	if( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO )
	{
		SG_UI_Msg_Add_Error(SG_T("Unable to retrieve data source names!"));
	}
	else
	{
		do
		{
			Servers	+= CSG_String((SG_Char *)dsn);
			SG_UI_Msg_Add_Execution(CSG_String::Format(SG_T("\n[%s] %s"), dsn, dsc), false);

			r	= SQLDataSources(m_hEnv, SQL_FETCH_NEXT,
					(SQLTCHAR *)dsn, SQL_MAX_DSN_LENGTH + 1, &dsnlen,
					(SQLTCHAR *)dsc, 256,                    &dsclen
				);
		}
		while( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO );
	}

	return( Servers );
}

//---------------------------------------------------------
int CSG_ODBC_Connections::Get_Servers(CSG_String &Servers)
{
	CSG_Strings		s	= Get_Servers();

	for(int i=0; i<s.Get_Count(); i++)
	{
		Servers	+= CSG_String::Format(SG_T("%s|"), s[i].c_str());
	}

	return( s.Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Strings CSG_ODBC_Connections::Get_Connections(void)
{
	CSG_Strings		Connections;

	for(int i=0; i<m_nConnections; i++)
	{
		Connections.Add(m_pConnections[i]->Get_Server());
	}

	return( Connections );
}

//---------------------------------------------------------
int CSG_ODBC_Connections::Get_Connections(CSG_String &Connections)
{
	CSG_Strings		s	= Get_Connections();

	Connections.Clear();

	for(int i=0; i<s.Get_Count(); i++)
	{
		Connections	+= CSG_String::Format(SG_T("%s|"), s[i].c_str());
	}

	return( s.Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_ODBC_Module::CSG_ODBC_Module(void)
{
	m_Connection_Choice.Create(this, LNG("Choose ODBC Connection"), LNG(""), SG_T("CONNECTIONS"));

	m_Connection_Choice.Add_Choice(
		NULL	, "CONNECTIONS", LNG("Available Connections"),
		LNG(""),
		SG_T("")
	);

	if( !SG_UI_Get_Window_Main() )
	{
		Parameters.Add_String(
			NULL	, "ODBC_DSN"	, LNG("DSN"),
			LNG("Data Source Name"),
			SG_T("")
		);

		Parameters.Add_String(
			NULL	, "ODBC_USR"	, LNG("User"),
			LNG("User Name"),
			SG_T("")
		);

		Parameters.Add_String(
			NULL	, "ODBC_PWD"	, LNG("Password"),
			LNG("Password"),
			SG_T("")
		);

		Parameters.Add_String(
			NULL	, "ODBC_DIR"	, LNG("Directory"),
			LNG("Directory (DBase, Excel)"),
			SG_T("")
		);
	}

	m_pConnection	= NULL;
}

//---------------------------------------------------------
bool CSG_ODBC_Module::On_Before_Execution(void)
{
	CSG_String	s;

	m_pConnection	= NULL;

	if( !SG_UI_Get_Window_Main() )
	{
		m_pConnection	= SG_ODBC_Get_Connection_Manager().Add_Connection(
			Parameters("ODBC_DSN")->asString(),
			Parameters("ODBC_USR")->asString(),
			Parameters("ODBC_PWD")->asString(),
			Parameters("ODBC_DIR")->asString()
		);
	}
	else
	{
		if( SG_ODBC_Get_Connection_Manager().Get_Connections(s) > 1 )
		{
			m_Connection_Choice("CONNECTIONS")->asChoice()->Set_Items(s);

			if( SG_UI_Dlg_Parameters(&m_Connection_Choice, LNG("Choose ODBC Connection")) )
			{
				m_pConnection	= SG_ODBC_Get_Connection_Manager().Get_Connection(m_Connection_Choice("CONNECTIONS")->asString());
			}
		}
		else if( s.Length() )
		{
			m_pConnection	= SG_ODBC_Get_Connection_Manager().Get_Connection(0);
		}
	}

	if( m_pConnection == NULL )
	{
		Message_Dlg(
			LNG("No ODBC connection available!"),
			LNG("ODBC Database Connection Error")
		);
	}

	return( m_pConnection != NULL );
}

//---------------------------------------------------------
bool CSG_ODBC_Module::On_After_Execution(void)
{
	if( !SG_UI_Get_Window_Main() )
	{
		SG_ODBC_Get_Connection_Manager().Del_Connection(m_pConnection, true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
