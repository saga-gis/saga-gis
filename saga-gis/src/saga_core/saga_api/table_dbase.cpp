
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define XBASE_FLDHDR_SZ			32
#define TRIM_DBF_WHITESPACE


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table_DBase::CSG_Table_DBase(void)
{
	bOpen			= false;

	hFile			= NULL;

	Record			= NULL;
	FieldOffset		= NULL;
	FieldDesc		= NULL;
	nFields			= 0;

	Result_String	= NULL;
}

//---------------------------------------------------------
CSG_Table_DBase::~CSG_Table_DBase(void)
{
	Close();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Creates a new DBase-File using FieldDescription...
bool CSG_Table_DBase::Open(const SG_Char *FileName, int anFields, TFieldDesc *aFieldDesc)
{
	Close();

#if defined(_SAGA_LINUX) && defined(_SAGA_UNICODE)
	if( (hFile = SG_FILE_OPEN(CSG_String(FileName).b_str(), "w+b")) != NULL )
#else
	if( (hFile = SG_FILE_OPEN(FileName, SG_T("w+b"))) != NULL )
#endif
	{
		bOpen		= true;
		bReadOnly	= false;

		nFields		= anFields;
		FieldDesc	= (TFieldDesc *)SG_Malloc(nFields * sizeof(TFieldDesc));
		memcpy(FieldDesc, aFieldDesc, nFields * sizeof(TFieldDesc));

		Header_Write();

		nFileBytes	= nHeaderBytes;
	}

	return( bOpen );
}


//---------------------------------------------------------
// Opens an existing DBase-File...
bool CSG_Table_DBase::Open(const SG_Char *FileName)
{
	Close();

#if defined(_SAGA_LINUX) && defined(_SAGA_UNICODE)
	if( (hFile = SG_FILE_OPEN(CSG_String(FileName).b_str(), "rb")) != NULL )
#else
	if( (hFile = SG_FILE_OPEN(FileName, SG_T("rb"))) != NULL )
#endif
	{
		bOpen		= true;
		bReadOnly	= true;

		if( Header_Read() )
		{
			fseek(hFile, 0, SEEK_END);
			nFileBytes	= ftell(hFile);
			fseek(hFile, 0, SEEK_SET);
		}
	}

	return( bOpen );
}

//---------------------------------------------------------
// Closes DBase-File if one was opened...
void CSG_Table_DBase::Close(void)
{
	//-----------------------------------------------------
	if( bOpen )
	{
		Flush_Record();
		Header_Write();
		bOpen			= false;

		fclose(hFile);
		hFile			= NULL;
	}

	//-----------------------------------------------------
	if( Record )
	{
		SG_Free(Record);
		Record			= NULL;
	}

	if( FieldOffset )
	{
		SG_Free(FieldOffset);
		FieldOffset		= NULL;
	}

	if( FieldDesc )
	{
		SG_Free(FieldDesc);
		FieldDesc		= NULL;
	}

	nFields			= 0;

	if( Result_String )
	{
		SG_Free(Result_String);
		Result_String	= NULL;
	}

	//-----------------------------------------------------
	bModified		= false;
	bRecModified	= false;

	//-----------------------------------------------------
	FileType		= 0;
	nRecords		= 0;
	nHeaderBytes	= 0;
	nRecordBytes	= 0;
	Transaction		= 0;
	bEncrypted		= 0;
	ProductionIdx	= 0;
	LanguageDrvID	= 0;

	nFileBytes		= 0;

	memset(LastUpdate, 0, 3 * sizeof(char));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_LINUX
char * _strupr(char *String)
{
	if( String )
		for(char *p=String; *p; p++)
			if( 'a' <= *p && *p <= 'z' )
				*p	+= 'A' - 'a';

	return( String );
}
#endif

//---------------------------------------------------------
void CSG_Table_DBase::Header_Write(void)
{
	char		buf[16];
	int			iField;
	time_t		ltime;
	struct tm	*pTime;
	CSG_String	s;

	if( bOpen && !bReadOnly )
	{
		//-------------------------------------------------
		// Initializations...

		FileType		= 0x03;


		time( &ltime );
		pTime			= localtime( &ltime );
		LastUpdate[0]	= (unsigned char)pTime->tm_year;
		LastUpdate[1]	= (unsigned char)pTime->tm_mon + 1;
		LastUpdate[2]	= (unsigned char)pTime->tm_mday;

		nHeaderBytes	= (nFields + 1) * 32 + 1;

		nRecordBytes	= 1;	// Delete-Flag = Byte 0...

		for(iField=0; iField<nFields; iField++)
		{
			if( FieldDesc[iField].Type == DBF_FT_CHARACTER )
			{
				if( FieldDesc[iField].Width < 1 )
				{
					FieldDesc[iField].Width	= 1;
				}
				else if( FieldDesc[iField].Width > 255 )
				{
					FieldDesc[iField].Width	= 255;
				}
			}

			nRecordBytes	+= FieldDesc[iField].Width;
		}

		Init_Record();

		fseek(hFile, 0, SEEK_SET);

		memset(buf, 0, 16 * sizeof(char));

		//-------------------------------------------------
		// Bytes 0-31: File Header...

		fwrite(&FileType		, sizeof(char),  1, hFile);	// 00		FoxBase+, FoxPro, dBaseIII+, dBaseIV, no memo	- 0x03
															//			FoxBase+, dBaseIII+ with memo					- 0x83
															//			FoxPro with memo								- 0xF5
															//			dBaseIV with memo								- 0x8B
															//			dBaseIV with SQL Table							- 0x8E
		fwrite(&LastUpdate		, sizeof(char),  3, hFile);	// 01-03	Last update, format YYYYMMDD   **correction: it is YYMMDD**
		fwrite(&nRecords		, sizeof(char),  4, hFile);	// 04-07	Number of records in file (32-bit number)
		fwrite(&nHeaderBytes	, sizeof(char),  2, hFile);	// 08-09	Number of bytes in header (16-bit number)
		fwrite(&nRecordBytes	, sizeof(char),  2, hFile);	// 10-11	Number of bytes in record (16-bit number)
		fwrite( buf				, sizeof(char),  2, hFile);	// 12-13	Reserved, fill with 0x00
		fwrite(&Transaction		, sizeof(char),  1, hFile);	// 14		dBaseIV flag, incomplete transaction
															//			Begin Transaction sets it to					- 0x01
															//			End Transaction or RollBack reset it to			- 0x00
		fwrite(&bEncrypted		, sizeof(char),  1, hFile);	// 15		Encryption flag, encrypted 0x01 else 0x00
															//			Changing the flag does not encrypt or decrypt the records
		fwrite( buf				, sizeof(char), 12, hFile);	// 16-27	dBaseIV multi-user environment use
		fwrite(&ProductionIdx	, sizeof(char),  1, hFile);	// 28		Production index exists - 0x01 else 0x00
		fwrite(&LanguageDrvID	, sizeof(char),  1, hFile);	// 29		dBaseIV language driver ID
		fwrite( buf				, sizeof(char),  2, hFile);	// 30-31	Reserved fill with 0x00


		//-------------------------------------------------
		// Bytes 32-n: Field Descriptor Array...
		for(iField=0; iField<nFields; iField++)
		{
			FieldDesc[iField].Name[10]	= '\0';
			_strupr(FieldDesc[iField].Name);

			fwrite( FieldDesc[iField].Name			, sizeof(char), 11, hFile);	// 00-10	Field Name ASCII padded with 0x00
			fwrite(&FieldDesc[iField].Type			, sizeof(char),  1, hFile);	// 11		Field Type Identifier (see table)
			fwrite(&FieldDesc[iField].Displacement	, sizeof(char),  4, hFile);	// 12-15	Displacement of field in record
			fwrite(&FieldDesc[iField].Width			, sizeof(char),  1, hFile);	// 16		Field length in bytes
			fwrite(&FieldDesc[iField].Decimals		, sizeof(char),  1, hFile);	// 17		Field decimal places
			fwrite( buf								, sizeof(char),  2, hFile);	// 18-19	Reserved
			fwrite(&FieldDesc[iField].WorkAreaID	, sizeof(char),  1, hFile);	// 20		dBaseIV work area ID
			fwrite( buf								, sizeof(char), 10, hFile);	// 21-30	Reserved
			fwrite(&FieldDesc[iField].ProductionIdx	, sizeof(char),  1, hFile);	// 31	 	Field is part of production index - 0x01 else 0x00
		}

		//-------------------------------------------------
		// Byte n+1: Header Record Terminator (0x0D)...
		buf[0]	= 0x0D;
		fwrite( buf				, sizeof(char),  1, hFile);
	}
}

//---------------------------------------------------------
bool CSG_Table_DBase::Header_Read(void)
{
	bool	Result	= false;
	char	buf[16];

	if( bOpen )
	{
		//-------------------------------------------------
		// Initializations...

		fseek(hFile, 0, SEEK_SET);


		//-------------------------------------------------
		// Bytes 0-31: File Header...

		fread(&FileType			, sizeof(char),  1, hFile);	// 00		FoxBase+, FoxPro, dBaseIII+, dBaseIV, no memo	- 0x03
															//			FoxBase+, dBaseIII+ with memo					- 0x83
															//			FoxPro with memo								- 0xF5
															//			dBaseIV with memo								- 0x8B
															//			dBaseIV with SQL Table							- 0x8E
		fread(&LastUpdate		, sizeof(char),  3, hFile);	// 01-03	Last update, format YYYYMMDD   **correction: it is YYMMDD**
		fread(&nRecords			, sizeof(char),  4, hFile);	// 04-07	Number of records in file (32-bit number)
		fread(&nHeaderBytes		, sizeof(char),  2, hFile);	// 08-09	Number of bytes in header (16-bit number)
		fread(&nRecordBytes		, sizeof(char),  2, hFile);	// 10-11	Number of bytes in record (16-bit number)
		fread( buf				, sizeof(char),  2, hFile);	// 12-13	Reserved, fill with 0x00
		fread(&Transaction		, sizeof(char),  1, hFile);	// 14		dBaseIV flag, incomplete transaction
															//			Begin Transaction sets it to					- 0x01
															//			End Transaction or RollBack reset it to			- 0x00
		fread(&bEncrypted		, sizeof(char),  1, hFile);	// 15		Encryption flag, encrypted 0x01 else 0x00
															//			Changing the flag does not encrypt or decrypt the records
		fread( buf				, sizeof(char), 12, hFile);	// 16-27	dBaseIV multi-user environment use
		fread(&ProductionIdx	, sizeof(char),  1, hFile);	// 28		Production index exists - 0x01 else 0x00
		fread(&LanguageDrvID	, sizeof(char),  1, hFile);	// 29		dBaseIV language driver ID
		fread( buf				, sizeof(char),  2, hFile);	// 30-31	Reserved fill with 0x00


		//-------------------------------------------------
		// Bytes 32-n: Field Descriptor Array...

		while(	ftell(hFile) < (long)nHeaderBytes - 1 && !feof(hFile) )
		{
			FieldDesc	= (TFieldDesc *)SG_Realloc(FieldDesc, (nFields + 1) * sizeof(TFieldDesc));

			fread( FieldDesc[nFields].Name			, sizeof(char), 11, hFile);	// 0-10		Field Name ASCII padded with 0x00
			fread(&FieldDesc[nFields].Type			, sizeof(char),  1, hFile);	// 11		Field Type Identifier (see table)
			fread(&FieldDesc[nFields].Displacement	, sizeof(char),  4, hFile);	// 12-15	Displacement of field in record
			fread(&FieldDesc[nFields].Width			, sizeof(char),  1, hFile);	// 16		Field length in bytes
			fread(&FieldDesc[nFields].Decimals		, sizeof(char),  1, hFile);	// 17		Field decimal places
			fread( buf								, sizeof(char),  2, hFile);	// 18-19	Reserved
			fread(&FieldDesc[nFields].WorkAreaID	, sizeof(char),  1, hFile);	// 20		dBaseIV work area ID
			fread( buf								, sizeof(char), 10, hFile);	// 21-30	Reserved
			fread(&FieldDesc[nFields].ProductionIdx	, sizeof(char),  1, hFile);	// 31	 	Field is part of production index - 0x01 else 0x00

			FieldDesc[nFields].Name[11]	= '\0';

			nFields++;
		}


		//-------------------------------------------------
		// Byte n+1: Header Record Terminator (0x0D)...
		fread( buf				, sizeof(char),  1, hFile);

		if( buf[0] == 0x0d )
		{
			Init_Record();
			Move_First();

			Result	= true;
		}
	}

	//-----------------------------------------------------
	if( !Result )
	{
		fclose(hFile);
		hFile	= NULL;
		bOpen	= false;
		Close();
	}

	return( Result );
}

//---------------------------------------------------------
void CSG_Table_DBase::Init_Record(void)
{
	int		iField, iPos;

	Record		= (char *)SG_Realloc(Record		, nRecordBytes	* sizeof(char));
	FieldOffset	= (int  *)SG_Realloc(FieldOffset	, nFields		* sizeof(int )); 

	for(iField=0, iPos=1; iField<nFields; iField++)
	{
		FieldOffset[iField]	= iPos;
		iPos	+= FieldDesc[iField].Width;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Table_DBase::Get_File_Position(void)
{
	return( hFile ? ftell(hFile) : 0 );
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

	if( bOpen )
	{
		Flush_Record();

		fseek(hFile, nHeaderBytes, SEEK_SET);

		if( fread(Record, nRecordBytes, sizeof(char), hFile) == 1 )
		{
			Result	= true;
		}

		fseek(hFile, nHeaderBytes, SEEK_SET);
	}

	return( Result );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Move_Next(void)
{
	bool	Result	= false;

	if( bOpen )
	{
		Flush_Record();

		fseek(hFile, nRecordBytes, SEEK_CUR);

		if( fread(Record, nRecordBytes, sizeof(char), hFile) == 1 )
		{
			Result	= true;
		}

		fseek(hFile, -nRecordBytes, SEEK_CUR);
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
	if( bOpen )
	{
		bRecModified	= true;

		memset(Record, 0, nRecordBytes);

		fseek(hFile, 0, SEEK_END);
		fwrite(Record, nRecordBytes, sizeof(char), hFile);
		fseek(hFile, -nRecordBytes, SEEK_END);

		nRecords++;

		nFileBytes		+= nRecordBytes;
	}
}

//---------------------------------------------------------
void CSG_Table_DBase::Flush_Record(void)
{
	if( bOpen && !bReadOnly && bRecModified )
	{
		bRecModified	= false;
		fwrite(Record, nRecordBytes, sizeof(char), hFile);
		fseek(hFile, -nRecordBytes, SEEK_CUR);
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
	if( bOpen && iField >= 0 && iField < nFields )
	{
		char		*c;
		int			i;
		CSG_String	s;

		for(i=0, c=Record+FieldOffset[iField]; i<FieldDesc[iField].Width && *c; i++, c++)
		{
			s	+= *c;
		}

		if( FieldDesc[iField].Type == DBF_FT_NUMERIC )
		{
			return( s.asInt(Value) );
		}

		else if( FieldDesc[iField].Type == DBF_FT_DATE && s.Length() >= 8 )
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
bool CSG_Table_DBase::asDouble(int iField, double &Value)
{
	if( bOpen && iField >= 0 && iField < nFields )
	{
		char		*c;
		int			i;
		CSG_String	s;

		for(i=0, c=Record+FieldOffset[iField]; i<FieldDesc[iField].Width && *c; i++, c++)
		{
			s	+= *c;
		}

		if( FieldDesc[iField].Type == DBF_FT_NUMERIC )
		{
			return( s.asDouble(Value) );
		}

		else if( FieldDesc[iField].Type == DBF_FT_DATE && s.Length() >= 8 )
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

	if( bOpen && iField >= 0 && iField < nFields )
	{
		if( FieldDesc[iField].Type != DBF_FT_DATE )
		{
			char	*c;
			int		i;

			for(i=0, c=Record+FieldOffset[iField]; i<FieldDesc[iField].Width && *c; i++, c++)
			{
				Value	+= *c;
			}
		}

		else // if( FieldDesc[iField].Type == DBF_FT_DATE )	// SAGA(DD.MM.YYYY) from DBASE(YYYYMMDD)
		{
			char	*s	= Record + FieldOffset[iField];

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
	static char	s[256]	= "";

	int		n;

	if( bOpen && iField >= 0 && iField < nFields && FieldDesc[iField].Width > 0 )
	{
		if( FieldDesc[iField].Type == DBF_FT_NUMERIC )
		{
			bRecModified	= true;

			if( FieldDesc[iField].Decimals > 0 )
			{
				sprintf(s, "%.*f", FieldDesc[iField].Decimals, Value);
			}
			else
			{
				sprintf(s, "%d", (int)Value);
			}

			if( (n = strlen(s)) > FieldDesc[iField].Width )
			{
				n	= FieldDesc[iField].Width;
			}

			memset(Record + FieldOffset[iField], ' ', FieldDesc[iField].Width);
			memcpy(Record + FieldOffset[iField], s	, n);

			return( true );
		}

		if( FieldDesc[iField].Type == DBF_FT_DATE )
		{
			int		y	= (int)(Value / 10000);	Value	-= y * 10000;
			int		m	= (int)(Value / 100);	Value	-= m * 100;
			int		d	= (int)(Value / 1);

			bRecModified	= true;

			sprintf(s, "%04d%02d%02d", y, m, d);

			if( (n = strlen(s)) > FieldDesc[iField].Width )
			{
				n	= FieldDesc[iField].Width;
			}

			memset(Record + FieldOffset[iField], ' ', FieldDesc[iField].Width);
			memcpy(Record + FieldOffset[iField], s	, n);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Set_Value(int iField, const char *Value)
{
	if( bOpen && iField >= 0 && iField < nFields && FieldDesc[iField].Width > 0 )
	{
		int		n	= Value && Value[0] ? strlen(Value) : 0;

		if( FieldDesc[iField].Type == DBF_FT_CHARACTER )
		{
			bRecModified	= true;

			if( n > FieldDesc[iField].Width )
			{
				n	= FieldDesc[iField].Width;
			}

			memset(Record + FieldOffset[iField], ' '	, FieldDesc[iField].Width);
			memcpy(Record + FieldOffset[iField], Value	, n);

			return( true );
		}

		if( FieldDesc[iField].Type == DBF_FT_DATE && n == 10 )	// SAGA(DD.MM.YYYY) to DBASE(YYYYMMDD)
		{
			bRecModified	= true;

			char	*s	= Record + FieldOffset[iField];

			s[0]	= Value[6];	// Y1
			s[1]	= Value[7];	// Y2
			s[2]	= Value[8];	// Y3
			s[3]	= Value[9];	// Y4
			s[4]	= Value[3];	// M1
			s[5]	= Value[4];	// M2
			s[6]	= Value[0];	// D1
			s[7]	= Value[1];	// D2

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Table_DBase::Set_NoData(int iField)
{
	if( bOpen && iField >= 0 && iField < nFields && FieldDesc[iField].Width > 0 )
	{
		memset(Record + FieldOffset[iField], ' ', FieldDesc[iField].Width);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
