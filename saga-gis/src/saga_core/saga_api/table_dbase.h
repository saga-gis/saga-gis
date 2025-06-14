
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
//                    table_dbase.h                      //
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
#ifndef HEADER_INCLUDED__SAGA_API__table_dbase_H
#define HEADER_INCLUDED__SAGA_API__table_dbase_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "api_core.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DBF_FT_NONE				'\0'
#define DBF_FT_CHARACTER		'C'
#define DBF_FT_DATE				'D'
#define DBF_FT_FLOAT			'F'
#define DBF_FT_NUMERIC			'N'
#define DBF_FT_LOGICAL			'L'
#define DBF_FT_MEMO				'M'


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_DBase  
{
public:
	CSG_Table_DBase(int Encoding = SG_FILE_ENCODING_ANSI);
	virtual ~CSG_Table_DBase(void);

	//-----------------------------------------------------
	bool						Open_Read			(const SG_Char *FileName, class CSG_Table *pTable, bool bRecords_Load = true);
	bool						Open_Write			(const SG_Char *FileName, class CSG_Table *pTable, bool bRecords_Save = true);

	void						Close				(void);

	bool						is_Open				(void) const { return( m_hFile != NULL ); }

	//-----------------------------------------------------
	int							Get_Field_Count		(void)
	{	return( m_nFields );	}

	const char *				Get_Field_Name		(int iField)
	{	return( iField >= 0 && iField < m_nFields ? m_Fields[iField].Name     : NULL );	}

	char						Get_Field_Type		(int iField)
	{	return( iField >= 0 && iField < m_nFields ? m_Fields[iField].Type     : DBF_FT_NONE );	}

	int							Get_Field_Width		(int iField)
	{	return( iField >= 0 && iField < m_nFields ? m_Fields[iField].Width    : 0 );	}

	int							Get_Field_Decimals	(int iField)
	{	return( iField >= 0 && iField < m_nFields ? m_Fields[iField].Decimals : 0 );	}


	//-----------------------------------------------------
	int							Get_File_Position	(void);
	int							Get_File_Length		(void)	{	return( m_nFileBytes );	}
	int							Get_Count	(void)	{	return( m_nRecords   );	}

	//-----------------------------------------------------
	bool						Move_First			(void);
	bool						Move_Next			(void);

	//-----------------------------------------------------
	void						Add_Record			(void);
	void						Flush_Record		(void);

	//-----------------------------------------------------
	bool						isDeleted			(void);

	bool						asInt				(int iField, int    &Value);
	bool						asDouble			(int iField, double &Value);
	CSG_String					asString			(int iField);

	//-----------------------------------------------------
	bool						Set_Value			(int iField, double            Value);
	bool						Set_Value			(int iField, const CSG_String &Value);
	bool						Set_NoData			(int iField);


private:
	typedef struct
	{
		char					Name[12], Type, Displacement[4], WorkAreaID, ProductionIdx;

		unsigned char			Width, Decimals;

		int						Offset;
	}
	TDBF_Field;

	typedef struct
	{
		char					LastUpdate[3], Transaction, LanguageDrvID, ProductionIdx;

		unsigned char			FileType, bEncrypted;
	}
	TDBF_Header;


private:

	bool						m_bReadOnly, m_bModified;

	char						*m_Record;

	short						m_nHeaderBytes, m_nRecordBytes;

	int							m_nFields, m_nRecords, m_Encoding;

	long						m_nFileBytes;

	FILE						*m_hFile;

	TDBF_Field					*m_Fields;


	bool						Write				(void *buffer, size_t size, size_t count);
	bool						Read				(void *buffer, size_t size, size_t count);

	void						Header_Write		(void);
	bool						Header_Read			(void);

	void						Init_Record			(void);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_dbase_H
