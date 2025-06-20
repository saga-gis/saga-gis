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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "api_core.h"
#include "table_dbase.h"
#include "table.h"
#include "datetime.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_DBase::CSG_Table_DBase(int Encoding)
{
	m_hFile		= NULL;
	m_Record	= NULL;
	m_Fields	= NULL;
	m_nFields	= 0;
	m_Encoding	= Encoding;
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
//                                                       //
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
		if( bRecords_Load && Get_Count() > 0 && Move_First() )
		{
			for(int iRecord=0; iRecord<Get_Count() && SG_UI_Process_Set_Progress(iRecord, Get_Count()); iRecord++)
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
inline bool CSG_Table_DBase::Read(void *buffer, size_t size, size_t count)
{
	return( m_hFile && fread(buffer, size, count, m_hFile) == size * count );
}

//---------------------------------------------------------
inline bool CSG_Table_DBase::Write(void *buffer, size_t size, size_t count)
{
	return( m_hFile && fwrite(buffer, size, count, m_hFile) == size * count );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Header_Read(void)
{
	if( !m_hFile )
	{
		return( false );
	}

	//-----------------------------------------------------
	char buffer[16]; TDBF_Header h;

	fseek(m_hFile, 0, SEEK_SET);

	//-----------------------------------------------------
	// Bytes 0-31: File Header...
	Read(&h.FileType     , sizeof(char),  1); // 00		FoxBase+, FoxPro, dBaseIII+, dBaseIV, no memo   - 0x03
	                                          //        FoxBase+, dBaseIII+ with memo                   - 0x83
	                                          //        FoxPro with memo                                - 0xF5
	                                          //        dBaseIV with memo                               - 0x8B
	                                          //        dBaseIV with SQL Table                          - 0x8E
	Read(&h.LastUpdate   , sizeof(char),  3); // 01-03  Last update, format YYYYMMDD   **correction: it is YYMMDD**
	Read(&m_nRecords     , sizeof(char),  4); // 04-07  Number of records in file (32-bit number)
	Read(&m_nHeaderBytes , sizeof(char),  2); // 08-09  Number of bytes in header (16-bit number)
	Read(&m_nRecordBytes , sizeof(char),  2); // 10-11  Number of bytes in record (16-bit number)
	Read(buffer          , sizeof(char),  2); // 12-13  Reserved, fill with 0x00
	Read(&h.Transaction  , sizeof(char),  1); // 14     dBaseIV flag, incomplete transaction
	                                          //        Begin Transaction sets it to                    - 0x01
	                                          //        End Transaction or RollBack reset it to         - 0x00
	Read(&h.bEncrypted   , sizeof(char),  1); // 15		Encryption flag, encrypted 0x01 else 0x00
	                                          //        Changing the flag does not encrypt or decrypt the records
	Read(buffer          , sizeof(char), 12); // 16-27  dBaseIV multi-user environment use
	Read(&h.ProductionIdx, sizeof(char),  1); // 28		Production index exists                         - 0x01 else 0x00
	Read(&h.LanguageDrvID, sizeof(char),  1); // 29     dBaseIV language driver ID
	Read(buffer          , sizeof(char),  2); // 30-31  Reserved fill with                              - 0x00

	//-----------------------------------------------------
	// Bytes 32-n: Field Descriptor Array...
	while( ftell(m_hFile) < (long)m_nHeaderBytes - 1 && !feof(m_hFile) )
	{
		m_Fields = (TDBF_Field *)SG_Realloc(m_Fields, (m_nFields + 1) * sizeof(TDBF_Field));

		Read( m_Fields[m_nFields].Name         , sizeof(char), 11); // 0-10   Field Name ASCII padded with 0x00
		Read(&m_Fields[m_nFields].Type         , sizeof(char),  1); // 11     Field Type Identifier (see table)
		Read(&m_Fields[m_nFields].Displacement , sizeof(char),  4); // 12-15  Displacement of field in record
		Read(&m_Fields[m_nFields].Width        , sizeof(char),  1); // 16     Field length in bytes
		Read(&m_Fields[m_nFields].Decimals     , sizeof(char),  1); // 17     Field decimal places
		Read(buffer                            , sizeof(char),  2); // 18-19  Reserved
		Read(&m_Fields[m_nFields].WorkAreaID   , sizeof(char),  1); // 20     dBaseIV work area ID
		Read(buffer                            , sizeof(char), 10); // 21-30  Reserved
		Read(&m_Fields[m_nFields].ProductionIdx, sizeof(char),  1); // 31     Field is part of production index - 0x01 else 0x00

		m_Fields[m_nFields].Name[11] = '\0';

		m_nFields++;
	}

	//-----------------------------------------------------
	// Byte n+1: Header m_Record Terminator (0x0D)...
	Read(buffer, sizeof(char), 1);

	if( buffer[0] == 0x0d )
	{
		Init_Record();
		Move_First();

		return( true );
	}

	Close();

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
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
	m_nFields = pTable->Get_Field_Count();
	m_Fields  = (TDBF_Field *)SG_Calloc(m_nFields, sizeof(TDBF_Field));	// init all bytes with 0x00

	for(int iField=0; iField<Get_Field_Count(); iField++)
	{
		CSG_String Name(pTable->Get_Field_Name(iField)); int nBytes;

		for(int j=0; j<11 && j<(int)Name.Length(); j++)
		{
			m_Fields[iField].Name[j] = Name.b_str()[j];
		}

		switch( pTable->Get_Field_Type(iField) )
		{
		case SG_DATATYPE_String: default:
			m_Fields[iField].Type      = DBF_FT_CHARACTER;
			m_Fields[iField].Width     = (BYTE)((nBytes = pTable->Get_Field_Length(iField, m_Encoding)) > 255 ? 255 : nBytes < 1 ? 1 : nBytes);
			break;

		case SG_DATATYPE_Date  :
			m_Fields[iField].Type      = DBF_FT_DATE;
			m_Fields[iField].Width     = (BYTE)8;
			break;

		case SG_DATATYPE_Char  :
			m_Fields[iField].Type      = DBF_FT_CHARACTER;
			m_Fields[iField].Width     = (BYTE)1;
			break;

		case SG_DATATYPE_Bit   :
			m_Fields[iField].Type      = DBF_FT_NUMERIC;
			m_Fields[iField].Width     = (BYTE)1;
			break;

		case SG_DATATYPE_Byte  :
			m_Fields[iField].Type      = DBF_FT_NUMERIC;
			m_Fields[iField].Width     = (BYTE)3;
			break;

		case SG_DATATYPE_Word  :
		case SG_DATATYPE_Short :
			m_Fields[iField].Type      = DBF_FT_NUMERIC;
			m_Fields[iField].Width     = (BYTE)6;
			break;

		case SG_DATATYPE_DWord :
		case SG_DATATYPE_Int   :
		case SG_DATATYPE_ULong :
		case SG_DATATYPE_Long  :
		case SG_DATATYPE_Color :
			m_Fields[iField].Type     = DBF_FT_NUMERIC;
			m_Fields[iField].Width    = (BYTE)16;
			break;

		case SG_DATATYPE_Float:
			m_Fields[iField].Type     = DBF_FT_NUMERIC;
			m_Fields[iField].Width    = (BYTE)16;
			m_Fields[iField].Decimals = (BYTE)8;
			break;

		case SG_DATATYPE_Double:
			m_Fields[iField].Type     = DBF_FT_FLOAT;
			m_Fields[iField].Width    = (BYTE)19;
			m_Fields[iField].Decimals = (BYTE)10;
			break;
		}
	}

	Header_Write();

	m_nFileBytes = m_nHeaderBytes;

	//-----------------------------------------------------
	if( bRecords_Save )
	{
		for(sLong iRecord=0; iRecord<pTable->Get_Count() && SG_UI_Process_Set_Progress(iRecord, pTable->Get_Count()); iRecord++)
		{
			CSG_Table_Record *pRecord = pTable->Get_Record(iRecord);

			Add_Record();

			for(int iField=0; iField<Get_Field_Count(); iField++)
			{
				if( pRecord->is_NoData(iField) )
				{
					Set_NoData(iField);
				}
				else switch( Get_Field_Type(iField) )
				{
				default:
					Set_Value(iField, pRecord->asString(iField));
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
	char buffer[16]; TDBF_Header h;

	time_t ltime; time(&ltime); struct tm *pTime = localtime(&ltime);

	h.FileType      = 0x03;
	h.Transaction   = 0;
	h.bEncrypted    = 0;
	h.LanguageDrvID = 0;
	h.ProductionIdx = 0;
	h.LastUpdate[0] = (unsigned char)pTime->tm_year;
	h.LastUpdate[1] = (unsigned char)pTime->tm_mon + 1;
	h.LastUpdate[2] = (unsigned char)pTime->tm_mday;

	m_nHeaderBytes  = (m_nFields + 1) * 32 + 1;
	m_nRecordBytes  = 1; // Delete-Flag = Byte 0...

	for(int iField=0; iField<m_nFields; iField++)
	{
		if( m_Fields[iField].Type == DBF_FT_CHARACTER )
		{
			if( m_Fields[iField].Width < 1 )
			{
				m_Fields[iField].Width	= 1;
			}
			else if( m_Fields[iField].Width > 255 )
			{
				m_Fields[iField].Width = 255;
			}
		}

		m_nRecordBytes += m_Fields[iField].Width;
	}

	Init_Record();

	fseek(m_hFile, 0, SEEK_SET);

	memset(buffer, 0, 16 * sizeof(char));

	//-----------------------------------------------------
	// Bytes 0-31: File Header...
	Write(&h.FileType     , sizeof(char),  1); // 00     FoxBase+, FoxPro, dBaseIII+, dBaseIV, no memo - 0x03
	                                           //        FoxBase+, dBaseIII+ with memo                 - 0x83
	                                           //        FoxPro with memo                              - 0xF5
	                                           //        dBaseIV with memo                             - 0x8B
	                                           //        dBaseIV with SQL Table                        - 0x8E
	Write(&h.LastUpdate   , sizeof(char),  3); // 01-03  Last update, format YYYYMMDD   **correction: it is YYMMDD**
	Write(&m_nRecords     , sizeof(char),  4); // 04-07  Number of records in file (32-bit number)
	Write(&m_nHeaderBytes , sizeof(char),  2); // 08-09  Number of bytes in header (16-bit number)
	Write(&m_nRecordBytes , sizeof(char),  2); // 10-11  Number of bytes in record (16-bit number)
	Write(buffer          , sizeof(char),  2); // 12-13  Reserved, fill with 0x00
	Write(&h.Transaction  , sizeof(char),  1); // 14     dBaseIV flag, incomplete transaction
	                                           //        Begin Transaction sets it to					- 0x01
	                                           //        End Transaction or RollBack reset it to        - 0x00
	Write(&h.bEncrypted   , sizeof(char),  1); // 15     Encryption flag, encrypted 0x01 else 0x00
	                                           //        Changing the flag does not encrypt or decrypt the records
	Write(buffer          , sizeof(char), 12); // 16-27  dBaseIV multi-user environment use
	Write(&h.ProductionIdx, sizeof(char),  1); // 28     Production index exists 0x01 else 0x00
	Write(&h.LanguageDrvID, sizeof(char),  1); // 29     dBaseIV language driver ID
	Write(buffer          , sizeof(char),  2); // 30-31  Reserved fill with 0x00

	//-----------------------------------------------------
	// Bytes 32-n: Field Descriptor Array...
	for(int iField=0; iField<m_nFields; iField++)
	{
		Write( m_Fields[iField].Name         , sizeof(char), 11); // 00-10  Field Name ASCII padded with 0x00
		Write(&m_Fields[iField].Type         , sizeof(char),  1); // 11     Field Type Identifier (see table)
		Write(&m_Fields[iField].Displacement , sizeof(char),  4); // 12-15  Displacement of field in record
		Write(&m_Fields[iField].Width        , sizeof(char),  1); // 16     Field length in bytes
		Write(&m_Fields[iField].Decimals     , sizeof(char),  1); // 17     Field decimal places
		Write(buffer                         , sizeof(char),  2); // 18-19  Reserved
		Write(&m_Fields[iField].WorkAreaID   , sizeof(char),  1); // 20     dBaseIV work area ID
		Write(buffer                         , sizeof(char), 10); // 21-30  Reserved
		Write(&m_Fields[iField].ProductionIdx, sizeof(char),  1); // 31     Field is part of production index - 0x01 else 0x00
	}

	//-----------------------------------------------------
	// Byte n+1: Header m_Record Terminator (0x0D)...
	buffer[0] = 0x0D;
	Write(buffer, sizeof(char), 1);
}


///////////////////////////////////////////////////////////
//                                                       //
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::Move_First(void)
{
	bool Result = false;

	if( m_hFile )
	{
		Flush_Record();

		fseek(m_hFile, m_nHeaderBytes, SEEK_SET);

		if( Read(m_Record, sizeof(char), (size_t)m_nRecordBytes) )
		{
			Result = true;
		}

		fseek(m_hFile, m_nHeaderBytes, SEEK_SET);
	}

	return( Result );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Move_Next(void)
{
	bool Result = false;

	if( m_hFile )
	{
		Flush_Record();

		fseek(m_hFile, m_nRecordBytes, SEEK_CUR);

		if( Read(m_Record, sizeof(char), (size_t)m_nRecordBytes) )
		{
			Result = true;
		}

		fseek(m_hFile, -m_nRecordBytes, SEEK_CUR);
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Table_DBase::Add_Record(void)
{
	if( m_hFile )
	{
		m_bModified	= true;

		memset(m_Record, ' ', m_nRecordBytes);

		fseek(m_hFile, 0, SEEK_END);
		Write(m_Record, m_nRecordBytes, sizeof(char));
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
		Write(m_Record, m_nRecordBytes, sizeof(char));
		fseek(m_hFile, -m_nRecordBytes, SEEK_CUR);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::isDeleted(void)
{
	return( m_hFile && *m_Record == '*' );
}

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
	if( !m_hFile || iField < 0 || iField >= m_nFields )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String	s;

	char *c	= m_Record + m_Fields[iField].Offset;

	for(int i=0; i<m_Fields[iField].Width && *c; i++, c++)
	{
		s	+= *c;
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_FLOAT
	||  m_Fields[iField].Type == DBF_FT_NUMERIC )
	{
		s.Replace(",", ".");

		return( s.asDouble(Value) );
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_DATE )
	{
		if( s.Length() < 8 )
		{
			return( false );
		}

		int	d	= s.Mid(6, 2).asInt(); if( d < 1 ) d = 1; else if( d > 31 ) d = 31;
		int	m	= s.Mid(4, 2).asInt(); if( m < 1 ) m = 1; else if( m > 12 ) m = 12;
		int	y	= s.Mid(0, 4).asInt();

		Value	= 10000 * y + 100 * m + d;
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
CSG_String CSG_Table_DBase::asString(int iField)
{
	CSG_String	Value;

	if( !m_hFile || iField < 0 || iField >= m_nFields )
	{
		return( Value );
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type != DBF_FT_DATE )
	{
		switch( m_Encoding )
		{
		case SG_FILE_ENCODING_ANSI: default:
		{	char *s = m_Record + m_Fields[iField].Offset;

			for(int i=0; i<m_Fields[iField].Width && *s; i++, s++)
			{
				Value += *s > 0 ? *s : '?';
			}
		}	break;

		case SG_FILE_ENCODING_UTF8:
			Value = CSG_String::from_UTF8(m_Record + m_Fields[iField].Offset, m_Fields[iField].Width);
			break;
		}

		Value.Trim(true);
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_DATE )	// SAGA(DD.MM.YYYY) from DBASE(YYYYMMDD)
	{
		char *s	= m_Record + m_Fields[iField].Offset;

		Value	+= s[0];	// Y1
		Value	+= s[1];	// Y2
		Value	+= s[2];	// Y3
		Value	+= s[3];	// Y4
		Value	+= '-';
		Value	+= s[4];	// M1
		Value	+= s[5];	// M2
		Value	+= '-';
		Value	+= s[6];	// D1
		Value	+= s[7];	// D2
	}

	//-----------------------------------------------------
	return( Value );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Table_DBase::Set_Value(int iField, double Value)
{
	if( !m_hFile || iField < 0 || iField >= m_nFields || m_Fields[iField].Width < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_DATE )
	{	// Value is expected to be Julian Day Number
		CSG_DateTime	d(Value);

		return( Set_Value(iField, CSG_String::Format("%04d-%02d-%02d",
			         d.Get_Year (),
			1 + (int)d.Get_Month(),
			1 +      d.Get_Day  ()
		)));
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_FLOAT )
	{	// Number stored as a string, right justified, and padded with blanks to the width of the field.
		char	s[256];

		sprintf(s, "%*.*e", m_Fields[iField].Width, m_Fields[iField].Decimals, Value);

		size_t	n	= strlen(s); if( n > m_Fields[iField].Width ) { n = m_Fields[iField].Width; }

		memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);
		memcpy(m_Record + m_Fields[iField].Offset, s  , M_GET_MIN(strlen(s), m_Fields[iField].Width));

		m_bModified	= true;

		return( true );
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_NUMERIC )
	{	// Number stored as a string, right justified, and padded with blanks to the width of the field.
		char	s[256];

		if( m_Fields[iField].Decimals > 0 )
		{
			sprintf(s, "%*.*f", m_Fields[iField].Width, m_Fields[iField].Decimals, Value);
		}
		else
		{
			sprintf(s, "%*d"  , m_Fields[iField].Width, (int)Value);
		}

		memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);
		memcpy(m_Record + m_Fields[iField].Offset, s  , M_GET_MIN(strlen(s), m_Fields[iField].Width));

		m_bModified	= true;

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Set_Value(int iField, const CSG_String &Value)
{
	if( !m_hFile || iField < 0 || iField >= m_nFields || m_Fields[iField].Width < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_CHARACTER )
	{	// All OEM code page characters - padded with blanks to the width of the field.
		if( Value.Length() < 1 )
		{
			memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);

			m_bModified	= true;

			return( true );
		}

		CSG_Buffer	s;

		switch( m_Encoding )
		{
		case SG_FILE_ENCODING_ANSI: default:
			s	= Value.to_ASCII();
			break;

		case SG_FILE_ENCODING_UTF8:
			s	= Value.to_UTF8 ();
			break;
		}

		if( s.Get_Size() >= Value.Length() )
		{
			memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);
			memcpy(m_Record + m_Fields[iField].Offset, s.Get_Data(), M_GET_MIN(s.Get_Size(), m_Fields[iField].Width));

			m_bModified	= true;

			return( true );
		}
	}

	//-----------------------------------------------------
	if( m_Fields[iField].Type == DBF_FT_DATE )	// SAGA(YYYY-MM-DD) to DBASE(YYYYMMDD)
	{	// 8 bytes - date stored as a string in the format YYYYMMDD
		if( Value.Length() >= 10 )
		{
			char *s	= m_Record + m_Fields[iField].Offset;

			s[0]	= Value.b_str()[0];	// Y1
			s[1]	= Value.b_str()[1];	// Y2
			s[2]	= Value.b_str()[2];	// Y3
			s[3]	= Value.b_str()[3];	// Y4
			s[4]	= Value.b_str()[5];	// M1
			s[5]	= Value.b_str()[6];	// M2
			s[6]	= Value.b_str()[8];	// D1
			s[7]	= Value.b_str()[9];	// D2

			m_bModified	= true;

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Set_NoData(int iField)
{
	if( !m_hFile || iField < 0 || iField >= m_nFields || m_Fields[iField].Width < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	memset(m_Record + m_Fields[iField].Offset, ' ', m_Fields[iField].Width);

	m_bModified	= true;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
