
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
//                       table.h                         //
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
#ifndef HEADER_INCLUDED__SAGA_API__table_H
#define HEADER_INCLUDED__SAGA_API__table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dataobject.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ETable_FileType
{
	TABLE_FILETYPE_Undefined	= 0,
	TABLE_FILETYPE_Text,
	TABLE_FILETYPE_Text_NoHeadLine,
	TABLE_FILETYPE_DBase
}
TTable_FileType;

//---------------------------------------------------------
typedef enum ETable_Index_Order
{
	TABLE_INDEX_None			= 0,
	TABLE_INDEX_Up,
	TABLE_INDEX_Down
}
TTable_Index_Order;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ETable_FieldType
{
	TABLE_FIELDTYPE_None		= 0,
	TABLE_FIELDTYPE_Char,
	TABLE_FIELDTYPE_Short,
	TABLE_FIELDTYPE_Int,
	TABLE_FIELDTYPE_Long,
	TABLE_FIELDTYPE_Float,
	TABLE_FIELDTYPE_Double,
	TABLE_FIELDTYPE_String,
	TABLE_FIELDTYPE_Color,
	TABLE_FIELDTYPE_Date
}
TTable_FieldType;

//---------------------------------------------------------
const char	TABLE_FieldType_Names[][32]	=
{
	"NONE",
	"CHARACTER",
	"SHORTINT",
	"INTEGER",
	"LONGINT",
	"FLOAT",
	"DOUBLE",
	"STRING",
	"COLOR"
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTable_Record
{
	friend class CTable;

public:

	class CTable *				Get_Owner		(void)				{	return( m_pOwner );	}
	int							Get_Index		(void)	const		{	return( m_Index );	}

	bool						Set_Value		(int        iField, const char *Value);
	bool						Set_Value		(const char *Field, const char *Value);
	bool						Set_Value		(int        iField, double      Value);
	bool						Set_Value		(const char *Field, double      Value);
	bool						Add_Value		(int        iField, double      Value);
	bool						Add_Value		(const char *Field, double      Value);
	bool						Mul_Value		(int        iField, double      Value);
	bool						Mul_Value		(const char *Field, double      Value);

	bool						Set_NoData		(int        iField);
	bool						Set_NoData		(const char *Field);
	bool						is_NoData		(int        iField)	const;
	bool						is_NoData		(const char *Field)	const;

	const char *				asString		(int        iField, int Decimals = -1)	const;
	const char *				asString		(const char *Field, int Decimals = -1)	const;

	char						asChar			(int        iField)	const	{	return( (char  )asInt   (iField) );	}
	char						asChar			(const char *Field)	const	{	return( (char  )asInt   ( Field) );	}
	short						asShort			(int        iField)	const	{	return( (short )asInt   (iField) );	}
	short						asShort			(const char *Field)	const	{	return( (short )asInt   ( Field) );	}
	int							asInt			(int        iField)	const;
	int							asInt			(const char *Field)	const;
	long						asLong			(int        iField)	const	{	return( (long  )asInt   (iField) );	}
	long						asLong			(const char *Field)	const	{	return( (long  )asInt   ( Field) );	}

	float						asFloat			(int        iField)	const	{	return( (float )asDouble(iField) );	}
	float						asFloat			(const char *Field)	const	{	return( (float )asDouble( Field) );	}
	double						asDouble		(int        iField)	const;
	double						asDouble		(const char *Field)	const;

	double						operator []		(int        iField)	const	{	return(         asDouble(iField) );	}

	void						Assign			(CTable_Record *pValues);

	bool						is_Selected		(void)				const	{	return( m_bSelected );	}


protected:

	CTable_Record(class CTable *pOwner, int Index);
	virtual ~CTable_Record(void);


	bool						m_bSelected;

	int							m_Index;

	class CTable_Value			**m_Values;

	class CTable				*m_pOwner;


	class CTable_Value *		_Create_Value	(TTable_FieldType Type);

	bool						_Add_Field		(int add_Field);
	bool						_Del_Field		(int del_Field);

	int							_Get_Field	 	(const char *Field)	const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTable : public CDataObject
{
	friend class CTable_Record;
	friend class CShapes;
	friend class CTIN;

public:

	CTable(void);

								CTable				(const CTable &Table);
	bool						Create				(const CTable &Table);

								CTable				(const char *File_Name, char Separator = '\t');
	bool						Create				(const char *File_Name, char Separator = '\t');

								CTable				(CTable *pStructure);
	bool						Create				(CTable *pStructure);

	virtual ~CTable(void);

	virtual bool				Destroy				(void);

	virtual TDataObject_Type	Get_ObjectType		(void)	const			{	return( DATAOBJECT_TYPE_Table );	}

	virtual bool				Assign				(CDataObject *pSource);
	bool						Assign_Values		(CTable *pTable);

	virtual bool				Save				(const char *File_Name, int Format = 0);
	virtual bool				Save				(const char *File_Name, int Format, char Separator);
	bool						Serialize			(FILE *Stream, bool bSave);

	//-----------------------------------------------------
	CDataObject *				Get_Owner			(void)					{	return( m_pOwner );			}
	bool						is_Private			(void)	const			{	return( m_pOwner != NULL );	}

	virtual bool				is_Valid			(void)	const			{	return( m_nFields > 0 );	}
	bool						is_Compatible		(CTable *pTable, bool bExactMatch = false)	const;

	//-----------------------------------------------------
	void						Add_Field			(const char *Name, TTable_FieldType Type, int iField = -1);
	bool						Del_Field			(int iField);

	int							Get_Field_Count		(void)			const	{	return( m_nFields );	}
	const char *				Get_Field_Name		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );			}
	TTable_FieldType			Get_Field_Type		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : TABLE_FIELDTYPE_None );	}

	double						Get_MinValue		(int iField)	const	{	return( _Range_Update(iField) ? m_Field_Val_Min[iField] : 0.0 );	}
	double						Get_MaxValue		(int iField)	const	{	return( _Range_Update(iField) ? m_Field_Val_Max[iField] : 0.0 );	}

	//-----------------------------------------------------
	CTable_Record *				Add_Record			(             CTable_Record *pValues = NULL);
	CTable_Record *				Ins_Record			(int iRecord, CTable_Record *pValues = NULL);
	bool						Del_Record			(int iRecord);
	bool						Del_Records			(void);

	int							Get_Record_Count	(void)			const	{	return( m_nRecords );	}
	CTable_Record *				Get_Record			(int iRecord)	const	{	return( iRecord >= 0 && iRecord < m_nRecords ? m_Records[iRecord] : NULL );	}
	CTable_Record &				operator []			(int iRecord)	const	{	return( *Get_Record(iRecord) );	}

	CTable_Record *				Get_Record_byIndex	(int Index)		const
	{
		if( Index >= 0 && Index < m_nRecords )
		{
			switch( m_Index_Order )
			{
			case TABLE_INDEX_None:	return( m_Records[Index] );
			case TABLE_INDEX_Up:	return( m_Records[m_Index[Index]] );
			case TABLE_INDEX_Down:	return( m_Records[m_Index[m_nRecords - 1 - Index]] );
			}
		}

		return( NULL );
	}

	//-----------------------------------------------------
	bool						Set_Value			(int iRecord, int iField, const char  *Value);
	bool						Set_Value			(int iRecord, int iField, double       Value);

	bool						Get_Value			(int iRecord, int iField, CSG_String  &Value)	const;
	bool						Get_Value			(int iRecord, int iField, double      &Value)	const;

	//-----------------------------------------------------
	int							Get_Selection_Count	(void)			const	{	return( m_nSelected );	}
	CTable_Record *				Get_Selection		(int Index = 0)	const	{	return( Index >= 0 && Index < m_nSelected ? m_Selected[Index] : NULL );	}

	bool						Select				(int iRecord					, bool bInvert = false);
	bool						Select				(CTable_Record *pRecord = NULL	, bool bInvert = false);

	int							Del_Selection		(void);

	//-----------------------------------------------------
	bool						Set_Index			(int iField, TTable_Index_Order Order);
	bool						Toggle_Index		(int iField);

	bool						is_Indexed			(void)	const		{	return( m_Index_Order != TABLE_INDEX_None );	}

	int							Get_Index_Field		(void)	const		{	return( m_Index_Field );	}
	TTable_Index_Order			Get_Index_Order		(void)	const		{	return( m_Index_Order );	}


protected:

	int							m_nFields, m_nRecords, m_nSelected, *m_Index, m_Index_Field;

	double						*m_Field_Val_Min, *m_Field_Val_Max;

	TTable_FieldType			*m_Field_Type;

	TTable_Index_Order			m_Index_Order;

	CTable_Record				**m_Records, **m_Selected;

	CSG_String					**m_Field_Name;

	CDataObject					*m_pOwner;


	void						_On_Construction	(void);

	bool						_Create				(const CTable &Table);
	bool						_Create				(const char *File_Name, char Separator);
	bool						_Create				(CTable *pStructure);

	bool						_Destroy			(void);
	bool						_Destroy_Selection	(void);

	bool						_Assign				(CDataObject *pObject);

	CTable_Record *				_Add_Record			(             CTable_Record *pValues = NULL);
	CTable_Record *				_Ins_Record			(int iRecord, CTable_Record *pValues = NULL);
	bool						_Del_Record			(int iRecord);
	bool						_Del_Records		(void);

	bool						_Range_Invalidate	(void)			const;
	bool						_Range_Invalidate	(int iField)	const;
	bool						_Range_Update		(int iField)	const;

	bool						_Load				(const char *File_Name, int Format, char Separator);
	bool						_Load_Text			(const char *File_Name, bool bHeadline, char Separator);
	bool						_Save_Text			(const char *File_Name, bool bHeadline, char Separator);
	bool						_Load_DBase			(const char *File_Name);
	bool						_Save_DBase			(const char *File_Name);

	void						_Index_Create		(int iField);
	void						_Index_Destroy		(void);
	int							_Index_Compare		(int a, int b);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe table construction */
SAGA_API_DLL_EXPORT CTable *	SG_Create_Table	(void);

/** Safe table construction */
SAGA_API_DLL_EXPORT CTable *	SG_Create_Table	(const CTable &Table);

/** Safe table construction */
SAGA_API_DLL_EXPORT CTable *	SG_Create_Table	(const char *FileName);

/** Safe table construction */
SAGA_API_DLL_EXPORT CTable *	SG_Create_Table	(CTable *pStructure);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_H
