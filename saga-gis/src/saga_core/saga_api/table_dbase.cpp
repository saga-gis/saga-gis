/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   table_dbase.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "api_core.h"
#include "table_dbase.h"
#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_DBase::CSG_Table_DBase(void)
{
	m_hFile		= NULL;
	m_Record	= NULL;
	m_Fields	= NULL;
	m_nFields	= 0;
}

//---------------------------------------------------------
CSG_Table_DBase::~CSG_Table_DBase(void)
{
	Close();
}

//---------------------------------------------------------
void CSG_Table_DBase::Close(void)
{
	if( m_hFile )
	{
		Flush_Record();
		Header_Write();

		fclose(m_hFile);
		m_hFile	= NULL;
	}

	SG_FREE_SAFE(m_Record);
	SG_FREE_SAFE(m_Fields);

	m_nFields		= 0;
	m_nRecords		= 0;
	m_nHeaderBytes	= 0;
	m_nRecordBytes	= 0;
	m_nFileBytes	= 0;

	m_bModified		= false;
}


///////////////////////////////////////////////////////////
//														 //
//						Read							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::Open_Read(const SG_Char *FileName, CSG_Table *pTable, bool bRecords_Load)
{
	Close();

	if( (m_hFile = fopen(CSG_String(FileName), "rb")) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("dbf read: could not open file"));

		return( false );
	}

	m_bReadOnly	= true;

	if( !Header_Read() )
	{
		SG_UI_Msg_Add_Error(_TL("dbf read: could not read header"));

		Close();

		return( false );
	}

	fseek(m_hFile, 0, SEEK_END);
	m_nFileBytes	= ftell(m_hFile);
	fseek(m_hFile, 0, SEEK_SET);

	//-----------------------------------------------------
	if( pTable )
	{
		int		iField;

		pTable->Destroy();

		for(iField=0; iField<Get_Field_Count(); iField++)
		{
			switch( Get_Field_Type(iField) )
			{
			case DBF_FT_LOGICAL:
				pTable->Add_Field(Get_Field_Name(iField), SG_DATATYPE_Char);
				break;

			case DBF_FT_CHARACTER:	default:
				pTable->Add_Field(Get_Field_Name(iField), SG_DATATYPE_String);
				break;

			case DBF_FT_DATE:
				pTable->Add_Field(Get_Field_Name(iField), SG_DATATYPE_Date);
				break;

			case DBF_FT_FLOAT:
				pTable->Add_Field(Get_Field_Name(iField), SG_DATATYPE_Double);
				break;

			case DBF_FT_NUMERIC:
				pTable->Add_Field(Get_Field_Name(iField), Get_Field_Decimals(iField) > 0
					? SG_DATATYPE_Double
					: SG_DATATYPE_Long
				);
			}
		}

		//-------------------------------------------------
		if( bRecords_Load && Get_Record_Count() > 0 && Move_First() )
		{
			for(int iRecord=0; iRecord<Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Record_Count()); iRecord++)
			{
				CSG_Table_Record	*pRecord	= pTable->Add_Record();

				for(iField=0; iField<Get_Field_Count(); iField++)
				{
					switch( Get_Field_Type(iField) )
					{
					default:
						pRecord->Set_Value(iField, asString(iField));
						break;

					case DBF_FT_FLOAT:
					case DBF_FT_NUMERIC:
						{
							double	Value;

							if( asDouble(iField, Value) )
								pRecord->Set_Value(iField, Value);
							else
								pRecord->Set_NoData(iField);
						}
						break;
					}
				}

				Move_Next();
			}

			SG_UI_Process_Set_Ready();
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Header_Read(void)
{
	if( !m_hFile )
	{
		return( false );
	}

	//-----------------------------------------------------
	char		buf[16];
	TDBF_Header	h;

	fseek(m_hFile, 0, SEEK_SET);

	//-----------------------------------------------------
	// Bytes 0-31: File Header...
	fread(&h.FileType		, sizeof(char),  1, m_hFile);	// 00		FoxBase+, FoxPro, dBaseIII+, dBaseIV, no memo	- 0x03
															//			FoxBase+, dBaseIII+ with memo					- 0x83
															//			FoxPro with memo								- 0xF5
															//			dBaseIV with memo								- 0x8B
															//			dBaseIV with SQL Table							- 0x8E
	fread(&h.LastUpdate		, sizeof(char),  3, m_hFile);	// 01-03	Last update, format YYYYMMDD   **correction: it is YYMMDD**
	fread(&m_nRecords		, sizeof(char),  4, m_hFile);	// 04-07	Number of records in file (32-bit number)
	fread(&m_nHeaderBytes	, sizeof(char),  2, m_hFile);	// 08-09	Number of bytes in header (16-bit number)
	fread(&m_nRecordBytes	, sizeof(char),  2, m_hFile);	// 10-11	Number of bytes in record (16-bit number)
	fread( buf				, sizeof(char),  2, m_hFile);	// 12-13	Reserved, fill with 0x00
	fread(&h.Transaction	, sizeof(char),  1, m_hFile);	// 14		dBaseIV flag, incomplete transaction
															//			Begin Transaction sets it to					- 0x01
															//			End Transaction or RollBack reset it to			- 0x00
	fread(&h.bEncrypted		, sizeof(char),  1, m_hFile);	// 15		Encryption flag, encrypted 0x01 else 0x00
															//			Changing the flag does not encrypt or decrypt the records
	fread( buf				, sizeof(char), 12, m_hFile);	// 16-27	dBaseIV multi-user environment use
	fread(&h.ProductionIdx	, sizeof(char),  1, m_hFile);	// 28		Production index exists - 0x01 else 0x00
	fread(&h.LanguageDrvID	, sizeof(char),  1, m_hFile);	// 29		dBaseIV language driver ID
	fread( buf				, sizeof(char),  2, m_hFile);	// 30-31	Reserved fill with 0x00

	//-----------------------------------------------------
	// Bytes 32-n: Field Descriptor Array...
	while( ftell(m_hFile) < (long)m_nHeaderBytes - 1 && !feof(m_hFile) )
	{
		m_Fields	= (TDBF_Field *)SG_Realloc(m_Fields, (m_nFields + 1) * sizeof(TDBF_Field));

		fread( m_Fields[m_nFields].Name				, sizeof(char), 11, m_hFile);	// 0-10		Field Name ASCII padded with 0x00
		fread(&m_Fields[m_nFields].Type				, sizeof(char),  1, m_hFile);	// 11		Field Type Identifier (see table)
		fread(&m_Fields[m_nFields].Displacement		, sizeof(char),  4, m_hFile);	// 12-15	Displacement of field in record
		fread(&m_Fields[m_nFields].Width			, sizeof(char),  1, m_hFile);	// 16		Field length in bytes
		fread(&m_Fields[m_nFields].Decimals			, sizeof(char),  1, m_hFile);	// 17		Field decimal places
		fread( buf									, sizeof(char),  2, m_hFile);	// 18-19	Reserved
		fread(&m_Fields[m_nFields].WorkAreaID		, sizeof(char),  1, m_hFile);	// 20		dBaseIV work area ID
		fread( buf									, sizeof(char), 10, m_hFile);	// 21-30	Reserved
		fread(&m_Fields[m_nFields].ProductionIdx	, sizeof(char),  1, m_hFile);	// 31	 	Field is part of production index - 0x01 else 0x00

		m_Fields[m_nFields].Name[11]	= '\0';

		m_nFields++;
	}

	//-----------------------------------------------------
	// Byte n+1: Header m_Record Terminator (0x0D)...
	fread( buf				, sizeof(char),  1, m_hFile);

	if( buf[0] == 0x0d )
	{
		Init_Record();
		Move_First();

		return( true );
	}

	Close();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Write							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::Open_Write(const SG_Char *FileName, CSG_Table *pTable, bool bRecords_Save)
{
	Close();

	if( pTable == NULL || pTable->Get_Field_Count() <= 0 )
	{
		SG_UI_Msg_Add_Error(_TL("dbf write: invalid table"));

		return( false );
	}

	if( (m_hFile = fopen(CSG_String(FileName), "w+b")) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("dbf write: could open file"));

		return( false );
	}

	m_bReadOnly	= false;

	//-----------------------------------------------------
	int		iField, nBytes;

	m_nFields	= pTable->Get_Field_Count();
	m_Fields	= (TDBF_Field *)SG_Calloc(m_nFields, sizeof(TDBF_Field));	// init all bytes with 0x00

	for(iField=0; iField<Get_Field_Count(); iField++)
	{
		CSG_String	Name(pTable->Get_Field_Name(iField));

		for(int j=0; j<11 && j<Name.Length(); j++)
		{
			m_Fields[iField].Name[j]	= Name.b_str()[j];
		}

		switch( pTable->Get_Field_Type(iField) )
		{
		case SG_DATATYPE_String: default:
			m_Fields[iField].Type		= DBF_FT_CHARACTER;
			m_Fields[iField].Width		= (BYTE)((nBytes = pTable->Get_Field_Length(iField)) > 255 ? 255 : nBytes < 1 ? 1 : nBytes);
			break;

		case SG_DATATYPE_Date:
			m_Fields[iField].Type		= DBF_FT_DATE;
			m_Fields[iField].Width		= (BYTE)8;
			break;

		case SG_DATATYPE_Char:
			m_Fields[iField].Type		= DBF_FT_CHARACTER;
			m_Fields[iField].Width		= (BYTE)1;
			break;

		case SG_DATATYPE_Short:
		case SG_DATATYPE_Int:
		case SG_DATATYPE_Long:
		case SG_DATATYPE_Color:
			m_Fields[iField].Type		= DBF_FT_NUMERIC;
			m_Fields[iField].Width		= (BYTE)16;
			break;

		case SG_DATATYPE_Float:
			m_Fields[iField].Type		= DBF_FT_NUMERIC;
			m_Fields[iField].Width		= (BYTE)16;
			m_Fields[iField].Decimals	= (BYTE)8;
			break;

		case SG_DATATYPE_Double:
			m_Fields[iField].Type		= DBF_FT_FLOAT;
			m_Fields[iField].Width		= (BYTE)19;
			m_Fields[iField].Decimals	= (BYTE)10;
			break;
		}
	}

	Header_Write();

	m_nFileBytes	= m_nHeaderBytes;

	//-----------------------------------------------------
	if( bRecords_Save )
	{
		for(int iRecord=0; iRecord<pTable->Get_Record_Count() && SG_UI_Process_Set_Progress(iRecord, pTable->Get_Record_Count()); iRecord++)
		{
			CSG_Table_Record	*pRecord	= pTable->Get_Record(iRecord);

			Add_Record();

			for(iField=0; iField<Get_Field_Count(); iField++)
			{
				if( pRecord->is_NoData(iField) )
				{
					Set_NoData(iField);
				}
				else switch( Get_Field_Type(iField) )
				{
				default:
					Set_Value(iField, CSG_String(pRecord->asString(iField)));
					break;

				case DBF_FT_FLOAT:
				case DBF_FT_NUMERIC:
					Set_Value(iField, pRecord->asDouble(iField));
					break;
				}
			}

			Flush_Record();
		}

		SG_UI_Process_Set_Ready();
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Table_DBase::Header_Write(void)
{
	if( !m_hFile || m_bReadOnly )
	{
		return;
	}

	//-----------------------------------------------------
	char		buf[16];
	int			iField;
	time_t		ltime;
	TDBF_Header	h;

	time(&ltime);
	struct tm	*pTime	= localtime(&ltime);

	h.FileType		= 0x03;
	h.Transaction	= 0;
	h.bEncrypted	= 0;
	h.LanguageDrvID	= 0;
	h.ProductionIdx	= 0;
	h.LastUpdate[0]	= (unsigned char)pTime->tm_year;
	h.LastUpdate[1]	= (unsigned char)pTime->tm_mon + 1;
	h.LastUpdate[2]	= (unsigned char)pTime->tm_mday;

	m_nHeaderBytes	= (m_nFields + 1) * 32 + 1;
	m_nRecordBytes	= 1;	// Delete-Flag = Byte 0...

	for(iField=0; iField<m_nFields; iField++)
	{
		if( m_Fields[iField].Type == DBF_FT_CHARACTER )
		{
			if( m_Fields[iField].Width < 1 )
			{
				m_Fields[iField].Width	= 1;
			}
			else if( m_Fields[iField].Width > 255 )
			{
				m_Fields[iField].Width	= 255;
			}
		}

		m_nRecordBytes	+= m_Fields[iField].Width;
	}

	Init_Record();

	fseek(m_hFile, 0, SEEK_SET);

	memset(buf, 0, 16 * sizeof(char));

	//-----------------------------------------------------
	// Bytes 0-31: File Header...
	fwrite(&h.FileType		, sizeof(char),  1, m_hFile);	// 00		FoxBase+, FoxPro, dBaseIII+, dBaseIV, no memo	- 0x03
															//			FoxBase+, dBaseIII+ with memo					- 0x83
															//			FoxPro with memo								- 0xF5
															//			dBaseIV with memo								- 0x8B
															//			dBaseIV with SQL Table							- 0x8E
	fwrite(&h.LastUpdate	, sizeof(char),  3, m_hFile);	// 01-03	Last update, format YYYYMMDD   **correction: it is YYMMDD**
	fwrite(&m_nRecords		, sizeof(char),  4, m_hFile);	// 04-07	Number of records in file (32-bit number)
	fwrite(&m_nHeaderBytes	, sizeof(char),  2, m_hFile);	// 08-09	Number of bytes in header (16-bit number)
	fwrite(&m_nRecordBytes	, sizeof(char),  2, m_hFile);	// 10-11	Number of bytes in record (16-bit number)
	fwrite( buf				, sizeof(char),  2, m_hFile);	// 12-13	Reserved, fill with 0x00
	fwrite(&h.Transaction	, sizeof(char),  1, m_hFile);	// 14		dBaseIV flag, incomplete transaction
															//			Begin Transaction sets it to					- 0x01
															//			End Transaction or RollBack reset it to			- 0x00
	fwrite(&h.bEncrypted	, sizeof(char),  1, m_hFile);	// 15		Encryption flag, encrypted 0x01 else 0x00
															//			Changing the flag does not encrypt or decrypt the records
	fwrite( buf				, sizeof(char), 12, m_hFile);	// 16-27	dBaseIV multi-user environment use
	fwrite(&h.ProductionIdx	, sizeof(char),  1, m_hFile);	// 28		Production index exists - 0x01 else 0x00
	fwrite(&h.LanguageDrvID	, sizeof(char),  1, m_hFile);	// 29		dBaseIV language driver ID
	fwrite( buf				, sizeof(char),  2, m_hFile);	// 30-31	Reserved fill with 0x00

	//-----------------------------------------------------
	// Bytes 32-n: Field Descriptor Array...
	for(iField=0; iField<m_nFields; iField++)
	{
		fwrite( m_Fields[iField].Name			, sizeof(char), 11, m_hFile);	// 00-10	Field Name ASCII padded with 0x00
		fwrite(&m_Fields[iField].Type			, sizeof(char),  1, m_hFile);	// 11		Field Type Identifier (see table)
		fwrite(&m_Fields[iField].Displacement	, sizeof(char),  4, m_hFile);	// 12-15	Displacement of field in record
		fwrite(&m_Fields[iField].Width			, sizeof(char),  1, m_hFile);	// 16		Field length in bytes
		fwrite(&m_Fields[iField].Decimals		, sizeof(char),  1, m_hFile);	// 17		Field decimal places
		fwrite( buf								, sizeof(char),  2, m_hFile);	// 18-19	Reserved
		fwrite(&m_Fields[iField].WorkAreaID		, sizeof(char),  1, m_hFile);	// 20		dBaseIV work area ID
		fwrite( buf								, sizeof(char), 10, m_hFile);	// 21-30	Reserved
		fwrite(&m_Fields[iField].ProductionIdx	, sizeof(char),  1, m_hFile);	// 31	 	Field is part of production index - 0x01 else 0x00
	}

	//-----------------------------------------------------
	// Byte n+1: Header m_Record Terminator (0x0D)...
	buf[0]	= 0x0D;
	fwrite( buf				, sizeof(char),  1, m_hFile);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table_DBase::Init_Record(void)
{
	m_Record	= (char *)SG_Realloc(m_Record, m_nRecordBytes * sizeof(char));
	m_Record[0]	= ' ';	// Data records are preceded by one byte, that is, a space (0x20) if the record is not deleted, an asterisk (0x2A) if the record is deleted.

	for(int iField=0, iPos=1; iField<m_nFields; iPos+=m_Fields[iField++].Width)
	{
		m_Fields[iField].Offset	= iPos;
	}
}

//---------------------------------------------------------
int CSG_Table_DBase::Get_File_Position(void)
{
	return( m_hFile ? ftell(m_hFile) : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::Move_First(void)
{
	bool	Result	= false;

	if( m_hFile )
	{
		Flush_Record();

		fseek(m_hFile, m_nHeaderBytes, SEEK_SET);

		if( fread(m_Record, m_nRecordBytes, sizeof(char), m_hFile) == 1 )
		{
			Result	= true;
		}

		fseek(m_hFile, m_nHeaderBytes, SEEK_SET);
	}

	return( Result );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Move_Next(void)
{
	bool	Result	= false;

	if( m_hFile )
	{
		Flush_Record();

		fseek(m_hFile, m_nRecordBytes, SEEK_CUR);

		if( fread(m_Record, m_nRecordBytes, sizeof(char), m_hFile) == 1 )
		{
			Result	= true;
		}

		fseek(m_hFile, -m_nRecordBytes, SEEK_CUR);
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table_DBase::Add_Record(void)
{
	if( m_hFile )
	{
		m_bModified	= true;

		memset(m_Record, ' ', m_nRecordBytes);

		fseek(m_hFile, 0, SEEK_END);
		fwrite(m_Record, m_nRecordBytes, sizeof(char), m_hFile);
		fseek(m_hFile, -m_nRecordBytes, SEEK_END);

		m_nRecords		++;
		m_nFileBytes	+= m_nRecordBytes;
	}
}

//---------------------------------------------------------
void CSG_Table_DBase::Flush_Record(void)
{
	if( m_hFile && !m_bReadOnly && m_bModified )
	{
		m_bModified	= false;
		fwrite(m_Record, m_nRecordBytes, sizeof(char), m_hFile);
		fseek(m_hFile, -m_nRecordBytes, SEEK_CUR);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::asInt(int iField, int &Value)
{
	double	d;

	if( asDouble(iField, d) )
	{
		Value	= (int)d;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::asDouble(int iField, double &Value)
{
	if( m_hFile && iField >= 0 && iField < m_nFields )
	{
		char		*c;
		int			i;
		CSG_String	s;

		for(i=0, c=m_Record+m_Fields[iField].Offset; i<m_Fields[iField].Width && *c; i++, c++)
		{
			s	+= *c;
		}

		if( m_Fields[iField].Type == DBF_FT_FLOAT
		||  m_Fields[iField].Type == DBF_FT_NUMERIC )
		{
			s.Replace(",", ".");

			return( s.asDouble(Value) );
		}

		if( m_Fields[iField].Type == DBF_FT_DATE && s.Length() >= 8 )
		{
			int	d	= s.Mid(6, 2).asInt();	if( d < 1 )	d	= 1;	else if( d > 31 )	d	= 31;
			int	m	= s.Mid(4, 2).asInt();	if( m < 1 )	m	= 1;	else if( m > 12 )	m	= 12;
			int	y	= s.Mid(0, 4).asInt();

			Value	= 10000 * y + 100 * m + d;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_String CSG_Table_DBase::asString(int iField)
{
	CSG_String	Value;

	if( m_hFile && iField >= 0 && iField < m_nFields )
	{
		if( m_Fields[iField].Type != DBF_FT_DATE )
		{
			char	*c;
			int		i;

			for(i=0, c=m_Record+m_Fields[iField].Offset; i<m_Fields[iField].Width && *c; i++, c++)
			{
				Value	+= *c;
			}

			Value.Trim(true);
		}

		else // if( m_Fields[iField].Type == DBF_FT_DATE )	// SAGA(DD.MM.YYYY) from DBASE(YYYYMMDD)
		{
			char	*s	= m_Record + m_Fields[iField].Offset;

			Value	+= s[6];	// D1
			Value	+= s[7];	// D2
			Value	+= '.';
			Value	+= s[4];	// M1
			Value	+= s[5];	// M2
			Value	+= '.';
			Value	+= s[0];	// Y1
			Value	+= s[1];	// Y2
			Value	+= s[2];	// Y3
			Value	+= s[3];	// Y4
		}
	}

	return( Value );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::Set_Value(int iField, double Value)
{
	static char	s[256];

	if( m_hFile && iField >= 0 && iField < m_nFields && m_Fields[iField].Width > 0 )
	{
		if( m_Fields[iField].Type == DBF_FT_FLOAT )
		{	// Number stored as a string, right justified, and padded with blanks to the width of the field.
			sprintf(s, "%*.*e", m_Fields[iField].Width, m_Fields[iField].Decimals, Value);

			int	n	= (int)strlen(s);	if( n > m_Fields[iField].Width )	{	n	= m_Fields[iField].Width;	}

			memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);
			memcpy(m_Record + m_Fields[iField].Offset, s  , n);

			m_bModified	= true;

			return( true );
		}

		if( m_Fields[iField].Type == DBF_FT_NUMERIC )
		{	// Number stored as a string, right justified, and padded with blanks to the width of the field.
			if( m_Fields[iField].Decimals > 0 )
			{
				sprintf(s, "%*.*f", m_Fields[iField].Width, m_Fields[iField].Decimals, Value);
			}
			else
			{
				sprintf(s, "%*d"  , m_Fields[iField].Width, (int)Value);
			}

			int	n	= (int)strlen(s);	if( n > m_Fields[iField].Width )	{	n	= m_Fields[iField].Width;	}

			memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);
			memcpy(m_Record + m_Fields[iField].Offset, s  , n);

			m_bModified	= true;

			return( true );
		}

		if( m_Fields[iField].Type == DBF_FT_DATE )
		{
			int		y	= (int)(Value / 10000);	Value	-= y * 10000;
			int		m	= (int)(Value / 100);	Value	-= m * 100;
			int		d	= (int)(Value / 1);

			sprintf(s, "%04d%02d%02d", y, m, d);

			return( Set_Value(iField, s) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Set_Value(int iField, const char *Value)
{
	if( m_hFile && iField >= 0 && iField < m_nFields && m_Fields[iField].Width > 0 )
	{
		int		n	= Value && Value[0] ? (int)strlen(Value) : 0;

		if( m_Fields[iField].Type == DBF_FT_CHARACTER )
		{	// All OEM code page characters - padded with blanks to the width of the field.
			if( n > m_Fields[iField].Width )
			{
				n	= m_Fields[iField].Width;
			}

			memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);
			memcpy(m_Record + m_Fields[iField].Offset, Value, n);

			m_bModified	= true;

			return( true );
		}

		if( m_Fields[iField].Type == DBF_FT_DATE && n == 10 )	// SAGA(DD.MM.YYYY) to DBASE(YYYYMMDD)
		{	// 8 bytes - date stored as a string in the format YYYYMMDD
			char	*s	= m_Record + m_Fields[iField].Offset;

			s[0]	= Value[6];	// Y1
			s[1]	= Value[7];	// Y2
			s[2]	= Value[8];	// Y3
			s[3]	= Value[9];	// Y4
			s[4]	= Value[3];	// M1
			s[5]	= Value[4];	// M2
			s[6]	= Value[0];	// D1
			s[7]	= Value[1];	// D2

			m_bModified	= true;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Set_NoData(int iField)
{
	if( m_hFile && iField >= 0 && iField < m_nFields && m_Fields[iField].Width > 0 )
	{
		memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);

		m_bModified	= true;

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
