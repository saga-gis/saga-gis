
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
//		Interface for the CSG_Table_DBase class.		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__table_dbase_H
#define HEADER_INCLUDED__SAGA_API__table_dbase_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <stdio.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DBF_FT_NONE				'\0'
#define DBF_FT_CHARACTER		'C'
#define DBF_FT_DATE				'D'
#define DBF_FT_FLOAT			'F'
#define DBF_FT_GENERAL			'G'
#define DBF_FT_LOGICAL			'L'
#define DBF_FT_MEMO				'M'
#define DBF_FT_NUMERIC			'N'
#define DBF_FT_PICTURE			'P'


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Table_DBase  
{
public:
	//-----------------------------------------------------
	typedef struct
	{
		char					Name[14],
								Type,
								Displacement[4],
								WorkAreaID,
								ProductionIdx;

		unsigned char			Width,
								Decimals;
	}
	TFieldDesc;


public:
	CSG_Table_DBase(void);
	virtual ~CSG_Table_DBase(void);

	//-----------------------------------------------------
	bool						Open				(const SG_Char *FileName);
	bool						Open				(const SG_Char *FileName, int nFields, TFieldDesc *FieldDesc);

	void						Close				(void);

	//-----------------------------------------------------
	int							Get_FieldCount		(void)
	{	return( nFields );	}

	const char *				Get_FieldName		(int iField)
	{	return( iField >= 0 && iField < nFields ? FieldDesc[iField].Name     : NULL );	}

	char						Get_FieldType		(int iField)
	{	return( iField >= 0 && iField < nFields ? FieldDesc[iField].Type     : DBF_FT_NONE );	}

	int							Get_FieldWidth		(int iField)
	{	return( iField >= 0 && iField < nFields ? FieldDesc[iField].Width    : 0 );	}

	int							Get_FieldDecimals	(int iField)
	{	return( iField >= 0 && iField < nFields ? FieldDesc[iField].Decimals : 0 );	}


	//-----------------------------------------------------
	int							Get_File_Position	(void);
	int							Get_File_Length		(void)	{	return( nFileBytes );	}

	//-----------------------------------------------------
	bool						Move_First			(void);
	bool						Move_Next			(void);

	//-----------------------------------------------------
	void						Add_Record			(void);
	void						Flush_Record		(void);

	//-----------------------------------------------------
	int							asInt				(int iField);
	double						asDouble			(int iField);
	char *						asString			(int iField);

	//-----------------------------------------------------
	bool						Set_Value			(int iField, double Value);
	bool						Set_Value			(int iField, const char *Value);


protected:

	bool						bOpen, bReadOnly, bModified, bRecModified;

	char						LastUpdate[3], Transaction, LanguageDrvID, ProductionIdx,
								*Record, *Result_String;

	unsigned char				FileType, bEncrypted;

	short						nHeaderBytes, nRecordBytes;

	int							nFields, nRecords, *FieldOffset, nFileBytes;

	FILE						*hFile;

	TFieldDesc					*FieldDesc;


	void						Header_Write		(void);
	bool						Header_Read			(void);

	void						Init_Record			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_dbase_H
