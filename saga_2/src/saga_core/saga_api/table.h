
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
#include "table_value.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Table_FileType
{
	TABLE_FILETYPE_Undefined	= 0,
	TABLE_FILETYPE_Text,
	TABLE_FILETYPE_Text_NoHeadLine,
	TABLE_FILETYPE_DBase
}
TSG_Table_File_Type;

//---------------------------------------------------------
typedef enum ESG_Table_Index_Order
{
	TABLE_INDEX_None			= 0,
	TABLE_INDEX_Ascending,
	TABLE_INDEX_Descending
}
TSG_Table_Index_Order;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Table_FieldType
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
TSG_Table_Field_Type;

//---------------------------------------------------------
const SG_Char	gSG_Table_Field_Type_Names[][32]	=
{
	SG_T("NONE"),
	SG_T("CHARACTER"),
	SG_T("SHORTINT"),
	SG_T("INTEGER"),
	SG_T("LONGINT"),
	SG_T("FLOAT"),
	SG_T("DOUBLE"),
	SG_T("STRING"),
	SG_T("COLOR"),
	SG_T("DATE")
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Table_Record
{
	friend class CSG_Table;

public:

	class CSG_Table *			Get_Table		(void)				{	return( m_pTable );	}
	int							Get_Index		(void)	const		{	return( m_Index );	}

	bool						Set_Value		(int           iField, const SG_Char *Value);
	bool						Set_Value		(const SG_Char *Field, const SG_Char *Value);
	bool						Set_Value		(int           iField, double         Value);
	bool						Set_Value		(const SG_Char *Field, double         Value);
	bool						Add_Value		(int           iField, double         Value);
	bool						Add_Value		(const SG_Char *Field, double         Value);
	bool						Mul_Value		(int           iField, double         Value);
	bool						Mul_Value		(const SG_Char *Field, double         Value);

	bool						Set_NoData		(int           iField);
	bool						Set_NoData		(const SG_Char *Field);
	bool						is_NoData		(int           iField)	const;
	bool						is_NoData		(const SG_Char *Field)	const;

	const SG_Char *				asString		(int           iField, int Decimals = -1)	const;
	const SG_Char *				asString		(const SG_Char *Field, int Decimals = -1)	const;

	SG_Char						asChar			(int           iField)	const	{	return( (SG_Char)asInt   (iField) );	}
	SG_Char						asChar			(const SG_Char *Field)	const	{	return( (SG_Char)asInt   ( Field) );	}
	short						asShort			(int           iField)	const	{	return( (short  )asInt   (iField) );	}
	short						asShort			(const SG_Char *Field)	const	{	return( (short  )asInt   ( Field) );	}
	int							asInt			(int           iField)	const;
	int							asInt			(const SG_Char *Field)	const;

	float						asFloat			(int           iField)	const	{	return( (float  )asDouble(iField) );	}
	float						asFloat			(const SG_Char *Field)	const	{	return( (float  )asDouble( Field) );	}
	double						asDouble		(int           iField)	const;
	double						asDouble		(const SG_Char *Field)	const;

	CSG_Table_Value &			operator []		(int           iField)	const	{	return( *m_Values[iField] );	}

	virtual bool				Assign			(CSG_Table_Record *pRecord);

	bool						is_Selected		(void)					const	{	return( m_bSelected );	}


protected:

	CSG_Table_Record(class CSG_Table *pTable, int Index);
	virtual ~CSG_Table_Record(void);


	bool						m_bSelected;

	int							m_Index;

	class CSG_Table_Value		**m_Values;

	class CSG_Table				*m_pTable;


	class CSG_Table_Value *		_Create_Value	(TSG_Table_Field_Type Type);

	bool						_Add_Field		(int add_Field);
	bool						_Del_Field		(int del_Field);

	int							_Get_Field	 	(const SG_Char *Field)	const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Table : public CSG_Data_Object
{
	friend class CSG_Table_Record;
	friend class CSG_TIN;

public:

	CSG_Table(void);

									CSG_Table			(const CSG_Table &Table);
	bool							Create				(const CSG_Table &Table);

									CSG_Table			(const CSG_String &File_Name, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined);
	bool							Create				(const CSG_String &File_Name, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined);

									CSG_Table			(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char *Separator);
	bool							Create				(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char *Separator);

									CSG_Table			(CSG_Table *pStructure);
	bool							Create				(CSG_Table *pStructure);

	virtual ~CSG_Table(void);

	virtual bool					Destroy				(void);

	virtual TSG_Data_Object_Type	Get_ObjectType		(void)	const			{	return( DATAOBJECT_TYPE_Table );	}

	virtual bool					Assign				(CSG_Data_Object *pSource);
	bool							Assign_Values		(CSG_Table *pTable);

	virtual bool					Save				(const CSG_String &File_Name, int Format = 0);
	virtual bool					Save				(const CSG_String &File_Name, int Format, const SG_Char *Separator);
	bool							Serialize			(CSG_File &Stream, bool bSave);

	//-----------------------------------------------------
	CSG_Data_Object *				Get_Owner			(void)					{	return( m_pOwner );			}
	bool							is_Private			(void)	const			{	return( m_pOwner != NULL );	}

	virtual bool					is_Valid			(void)	const			{	return( m_nFields > 0 );	}
	bool							is_Compatible		(CSG_Table *pTable, bool bExactMatch = false)	const;

	//-----------------------------------------------------
	void							Add_Field			(const SG_Char *Name, TSG_Table_Field_Type Type, int iField = -1);
#ifdef _SAGA_UNICODE
	void							Add_Field			(const char    *Name, TSG_Table_Field_Type Type, int iField = -1);
#endif
	bool							Del_Field			(int iField);

	int								Get_Field_Count		(void)			const	{	return( m_nFields );	}
	const SG_Char *					Get_Field_Name		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );			}
	TSG_Table_Field_Type			Get_Field_Type		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : TABLE_FIELDTYPE_None );	}

	bool							Set_Field_Name		(int iField, const SG_Char *Name);

	double							Get_Minimum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Minimum()  : 0.0 );	}
	double							Get_Maximum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Maximum()  : 0.0 );	}
	double							Get_Range			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Range()    : 0.0 );	}
	double							Get_StdDev			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_StdDev()   : 0.0 );	}
	double							Get_Variance		(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Variance() : 0.0 );	}

	//-----------------------------------------------------
	CSG_Table_Record *				Add_Record			(             CSG_Table_Record *pCopy = NULL);
	CSG_Table_Record *				Ins_Record			(int iRecord, CSG_Table_Record *pCopy = NULL);
	bool							Del_Record			(int iRecord);
	bool							Del_Records			(void);

	int								Get_Count			(void)			const	{	return( m_nRecords );	}
	int								Get_Record_Count	(void)			const	{	return( m_nRecords );	}
	CSG_Table_Record *				Get_Record			(int iRecord)	const	{	return( iRecord >= 0 && iRecord < m_nRecords ? m_Records[iRecord] : NULL );	}
	CSG_Table_Record &				operator []			(int iRecord)	const	{	return( *Get_Record(iRecord) );	}

	int								Get_Index			(int Index)		const	{	return( Index >= 0 && Index < m_nRecords ? (m_Index ? m_Index[Index] : Index) : -1 );	}

	CSG_Table_Record *				Get_Record_byIndex	(int Index)		const
	{
		if( Index >= 0 && Index < m_nRecords )
		{
			if( m_Index != NULL )
			{
				return( m_Records[m_Index[Index]] );
			}

			return( m_Records[Index] );
		}

		return( NULL );
	}

	//-----------------------------------------------------
	bool							Set_Value			(int iRecord, int iField, const SG_Char  *Value);
	bool							Set_Value			(int iRecord, int iField, double          Value);

	bool							Get_Value			(int iRecord, int iField, CSG_String     &Value)	const;
	bool							Get_Value			(int iRecord, int iField, double         &Value)	const;

	virtual void					Set_Modified		(bool bModified = true)	{	CSG_Data_Object::Set_Modified(bModified);	if( m_pOwner )	m_pOwner->Set_Modified(bModified);	}

	//-----------------------------------------------------
	int								Get_Selection_Count	(void)			const	{	return( m_nSelected );	}
	CSG_Table_Record *				Get_Selection		(int Index = 0)	const	{	return( Index >= 0 && Index < m_nSelected ? m_Selected[Index] : NULL );	}

	virtual bool					Select				(int iRecord						, bool bInvert = false);
	virtual bool					Select				(CSG_Table_Record *pRecord = NULL	, bool bInvert = false);

	int								Del_Selection		(void);
	int								Inv_Selection		(void);

	//-----------------------------------------------------
	bool							Set_Index			(int Field_1, TSG_Table_Index_Order Order_1, int Field_2 = -1, TSG_Table_Index_Order Order_2 = TABLE_INDEX_None, int Field_3 = -1, TSG_Table_Index_Order Order_3 = TABLE_INDEX_None);
	bool							Del_Index			(void);
	bool							Toggle_Index		(int iField);

	bool							is_Indexed			(void)	const		{	return( m_Index != NULL );	}

	int								Get_Index_Field		(int i)	const		{	return( i >= 0 && i < 3 ? m_Index_Field[i] : -1 );	}
	TSG_Table_Index_Order			Get_Index_Order		(int i)	const		{	return( i >= 0 && i < 3 ? m_Index_Order[i] : TABLE_INDEX_None );	}


protected:

	int								m_nFields, m_nRecords, m_nBuffer, m_nSelected, *m_Index, m_Index_Field[3];

	TSG_Table_Field_Type			*m_Field_Type;

	TSG_Table_Index_Order			m_Index_Order[3];

	CSG_Simple_Statistics			**m_Field_Stats;

	CSG_Table_Record				**m_Records, **m_Selected;

	CSG_String						**m_Field_Name;

	CSG_Data_Object					*m_pOwner;


	virtual void					_On_Construction	(void);

	virtual CSG_Table_Record *		_Get_New_Record		(int Index);

	bool							_Create				(const CSG_Table &Table);
	bool							_Create				(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char *Separator);
	bool							_Create				(CSG_Table *pStructure);

	bool							_Destroy			(void);
	bool							_Destroy_Selection	(void);

	bool							_Assign				(CSG_Data_Object *pObject);

	bool							_Inc_Array			(void);
	bool							_Dec_Array			(void);

	CSG_Table_Record *				_Add_Record			(             CSG_Table_Record *pValues = NULL);
	CSG_Table_Record *				_Ins_Record			(int iRecord, CSG_Table_Record *pValues = NULL);
	bool							_Del_Record			(int iRecord);
	bool							_Del_Records		(void);

	bool							_Stats_Invalidate	(void)			const;
	bool							_Stats_Invalidate	(int iField)	const;
	bool							_Stats_Update		(int iField)	const;

	bool							_Load				(const CSG_String &File_Name, TSG_Table_File_Type Format, const SG_Char *Separator);
	bool							_Load_Text			(const CSG_String &File_Name, bool bHeadline, const SG_Char *Separator);
	bool							_Save_Text			(const CSG_String &File_Name, bool bHeadline, const SG_Char *Separator);
	bool							_Load_DBase			(const CSG_String &File_Name);
	bool							_Save_DBase			(const CSG_String &File_Name);

	void							_Index_Create		(void);
	void							_Index_Destroy		(void);
	int								_Index_Compare		(int a, int b);
	int								_Index_Compare		(int a, int b, int Field);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(void);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_Table &Table);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_String &File_Name);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(CSG_Table *pStructure);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_H
