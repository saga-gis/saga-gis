
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
	TABLE_INDEX_Up,
	TABLE_INDEX_Down
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
	SG_T("COLOR")
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

	class CSG_Table *			Get_Owner		(void)				{	return( m_pOwner );	}
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

	void						Assign			(CSG_Table_Record *pValues);

	bool						is_Selected		(void)					const	{	return( m_bSelected );	}


protected:

	CSG_Table_Record(class CSG_Table *pOwner, int Index);
	virtual ~CSG_Table_Record(void);


	bool						m_bSelected;

	int							m_Index;

	class CSG_Table_Value		**m_Values;

	class CSG_Table				*m_pOwner;


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
	friend class CSG_Shapes;
	friend class CSG_TIN;

public:

	CSG_Table(void);

								CSG_Table			(const CSG_Table &Table);
	bool						Create				(const CSG_Table &Table);

								CSG_Table			(const SG_Char *File_Name, SG_Char Separator = SG_T('\t'));
	bool						Create				(const SG_Char *File_Name, SG_Char Separator = SG_T('\t'));

								CSG_Table			(CSG_Table *pStructure);
	bool						Create				(CSG_Table *pStructure);

	virtual ~CSG_Table(void);

	virtual bool				Destroy				(void);

	virtual TSG_Data_Object_Type	Get_ObjectType	(void)	const			{	return( DATAOBJECT_TYPE_Table );	}

	virtual bool				Assign				(CSG_Data_Object *pSource);
	bool						Assign_Values		(CSG_Table *pTable);

	virtual bool				Save				(const SG_Char *File_Name, int Format = 0);
	virtual bool				Save				(const SG_Char *File_Name, int Format, SG_Char Separator);
	bool						Serialize			(CSG_File &Stream, bool bSave);

	//-----------------------------------------------------
	CSG_Data_Object *			Get_Owner			(void)					{	return( m_pOwner );			}
	bool						is_Private			(void)	const			{	return( m_pOwner != NULL );	}

	virtual bool				is_Valid			(void)	const			{	return( m_nFields > 0 );	}
	bool						is_Compatible		(CSG_Table *pTable, bool bExactMatch = false)	const;

	//-----------------------------------------------------
	void						Add_Field			(const SG_Char *Name, TSG_Table_Field_Type Type, int iField = -1);
#ifdef _SAGA_UNICODE
	void						Add_Field			(const char    *Name, TSG_Table_Field_Type Type, int iField = -1);
#endif
	bool						Del_Field			(int iField);

	int							Get_Field_Count		(void)			const	{	return( m_nFields );	}
	const SG_Char *				Get_Field_Name		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );			}
	TSG_Table_Field_Type		Get_Field_Type		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : TABLE_FIELDTYPE_None );	}

	double						Get_MinValue		(int iField)	const	{	return( _Range_Update(iField) ? m_Field_Val_Min[iField] : 0.0 );	}
	double						Get_MaxValue		(int iField)	const	{	return( _Range_Update(iField) ? m_Field_Val_Max[iField] : 0.0 );	}

	//-----------------------------------------------------
	CSG_Table_Record *			Add_Record			(             CSG_Table_Record *pValues = NULL);
	CSG_Table_Record *			Ins_Record			(int iRecord, CSG_Table_Record *pValues = NULL);
	bool						Del_Record			(int iRecord);
	bool						Del_Records			(void);

	int							Get_Record_Count	(void)			const	{	return( m_nRecords );	}
	CSG_Table_Record *			Get_Record			(int iRecord)	const	{	return( iRecord >= 0 && iRecord < m_nRecords ? m_Records[iRecord] : NULL );	}
	CSG_Table_Record &			operator []			(int iRecord)	const	{	return( *Get_Record(iRecord) );	}

	CSG_Table_Record *			Get_Record_byIndex	(int Index)		const
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
	bool						Set_Value			(int iRecord, int iField, const SG_Char  *Value);
	bool						Set_Value			(int iRecord, int iField, double       Value);

	bool						Get_Value			(int iRecord, int iField, CSG_String  &Value)	const;
	bool						Get_Value			(int iRecord, int iField, double      &Value)	const;

	//-----------------------------------------------------
	int							Get_Selection_Count	(void)			const	{	return( m_nSelected );	}
	CSG_Table_Record *			Get_Selection		(int Index = 0)	const	{	return( Index >= 0 && Index < m_nSelected ? m_Selected[Index] : NULL );	}

	bool						Select				(int iRecord					, bool bInvert = false);
	bool						Select				(CSG_Table_Record *pRecord = NULL	, bool bInvert = false);

	int							Del_Selection		(void);

	//-----------------------------------------------------
	bool						Set_Index			(int iField, TSG_Table_Index_Order Order);
	bool						Toggle_Index		(int iField);

	bool						is_Indexed			(void)	const		{	return( m_Index_Order != TABLE_INDEX_None );	}

	int							Get_Index_Field		(void)	const		{	return( m_Index_Field );	}
	TSG_Table_Index_Order		Get_Index_Order		(void)	const		{	return( m_Index_Order );	}


protected:

	int							m_nFields, m_nRecords, m_nBuffer, m_nSelected, *m_Index, m_Index_Field;

	double						*m_Field_Val_Min, *m_Field_Val_Max;

	TSG_Table_Field_Type		*m_Field_Type;

	TSG_Table_Index_Order		m_Index_Order;

	CSG_Table_Record			**m_Records, **m_Selected;

	CSG_String					**m_Field_Name;

	CSG_Data_Object				*m_pOwner;


	void						_On_Construction	(void);

	bool						_Create				(const CSG_Table &Table);
	bool						_Create				(const SG_Char *File_Name, SG_Char Separator);
	bool						_Create				(CSG_Table *pStructure);

	bool						_Destroy			(void);
	bool						_Destroy_Selection	(void);

	bool						_Assign				(CSG_Data_Object *pObject);

	bool						_Inc_Array			(void);
	bool						_Dec_Array			(void);

	CSG_Table_Record *			_Add_Record			(             CSG_Table_Record *pValues = NULL);
	CSG_Table_Record *			_Ins_Record			(int iRecord, CSG_Table_Record *pValues = NULL);
	bool						_Del_Record			(int iRecord);
	bool						_Del_Records		(void);

	bool						_Range_Invalidate	(void)			const;
	bool						_Range_Invalidate	(int iField)	const;
	bool						_Range_Update		(int iField)	const;

	bool						_Load				(const SG_Char *File_Name, int Format    , SG_Char Separator);
	bool						_Load_Text			(const SG_Char *File_Name, bool bHeadline, SG_Char Separator);
	bool						_Save_Text			(const SG_Char *File_Name, bool bHeadline, SG_Char Separator);
	bool						_Load_DBase			(const SG_Char *File_Name);
	bool						_Save_DBase			(const SG_Char *File_Name);

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
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(void);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_Table &Table);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const SG_Char *FileName);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(CSG_Table *pStructure);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_H
