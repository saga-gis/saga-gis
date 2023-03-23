
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
#ifndef HEADER_INCLUDED__SAGA_API__table_H
#define HEADER_INCLUDED__SAGA_API__table_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file table.h
* The table data container, a specialized CSG_Table class
* following a geometry object based approach.
* @see CSG_Table_Value
* @see CSG_Table_Record
* @see CSG_Table
* @see CSG_Data_Object
*/


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
typedef enum
{
	TABLE_FILETYPE_Undefined = 0,
	TABLE_FILETYPE_Text,
	TABLE_FILETYPE_Text_NoHeadLine,
	TABLE_FILETYPE_DBase
}
TSG_Table_File_Type;

//---------------------------------------------------------
typedef enum
{
	TABLE_INDEX_None = 0,
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
#define SG_TABLE_REC_FLAG_Modified 0x01
#define SG_TABLE_REC_FLAG_Selected 0x02


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Table_Record
{
	friend class CSG_Table; friend class CSG_PointCloud;

public:

	class CSG_Table *			Get_Table		(void)			{	return( m_pTable );	}
	sLong						Get_Index		(void)	const	{	return( m_Index  );	}

	bool						Set_Value		(int               Field, const CSG_String &Value);
	bool						Set_Value		(int               Field, const char       *Value) { return( Set_Value(           Field , CSG_String(Value)) ); }
	bool						Set_Value		(int               Field, const wchar_t    *Value) { return( Set_Value(           Field , CSG_String(Value)) ); }
	bool						Set_Value		(const CSG_String &Field, const CSG_String &Value);
	bool						Set_Value		(const char       *Field, const CSG_String &Value) { return( Set_Value(CSG_String(Field),            Value ) ); }
	bool						Set_Value		(const wchar_t    *Field, const CSG_String &Value) { return( Set_Value(CSG_String(Field),            Value ) ); }
	bool						Set_Value		(const char       *Field, const char       *Value) { return( Set_Value(CSG_String(Field), CSG_String(Value)) ); }
	bool						Set_Value		(const wchar_t    *Field, const wchar_t    *Value) { return( Set_Value(CSG_String(Field), CSG_String(Value)) ); }

	bool						Set_Value		(int               Field, const CSG_Bytes  &Value);
	bool						Set_Value		(const CSG_String &Field, const CSG_Bytes  &Value);
	bool						Set_Value		(const char       *Field, const CSG_Bytes  &Value) { return( Set_Value(CSG_String(Field), Value) ); }
	bool						Set_Value		(const wchar_t    *Field, const CSG_Bytes  &Value) { return( Set_Value(CSG_String(Field), Value) ); }

	bool						Set_Value		(int               Field, double            Value);
	bool						Set_Value		(const CSG_String &Field, double            Value);
	bool						Set_Value		(const char       *Field, double            Value) { return( Set_Value(CSG_String(Field), Value) ); }
	bool						Set_Value		(const wchar_t    *Field, double            Value) { return( Set_Value(CSG_String(Field), Value) ); }

	bool						Set_Value		(int               Field, sLong             Value);
	bool						Set_Value		(const CSG_String &Field, sLong             Value);
	bool						Set_Value		(const char       *Field, sLong             Value) { return( Set_Value(CSG_String(Field), Value) ); }
	bool						Set_Value		(const wchar_t    *Field, sLong             Value) { return( Set_Value(CSG_String(Field), Value) ); }

	bool						Set_Value		(int               Field, size_t            Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const CSG_String &Field, size_t            Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const char       *Field, size_t            Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const wchar_t    *Field, size_t            Value) { return( Set_Value(Field, (double)Value) ); }

	bool						Set_Value		(int               Field, DWORD             Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const CSG_String &Field, DWORD             Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const char       *Field, DWORD             Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const wchar_t    *Field, DWORD             Value) { return( Set_Value(Field, (double)Value) ); }

	bool						Set_Value		(int               Field, long              Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const CSG_String &Field, long              Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const char       *Field, long              Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const wchar_t    *Field, long              Value) { return( Set_Value(Field, (double)Value) ); }

	bool						Set_Value		(int               Field, int               Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const CSG_String &Field, int               Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const char       *Field, int               Value) { return( Set_Value(Field, (double)Value) ); }
	bool						Set_Value		(const wchar_t    *Field, int               Value) { return( Set_Value(Field, (double)Value) ); }

	bool						Add_Value		(int               Field, double            Value);
	bool						Add_Value		(const CSG_String &Field, double            Value);
	bool						Add_Value		(const char       *Field, double            Value) { return( Add_Value(CSG_String(Field), Value) ); }
	bool						Add_Value		(const wchar_t    *Field, double            Value) { return( Add_Value(CSG_String(Field), Value) ); }

	bool						Mul_Value		(int               Field, double            Value);
	bool						Mul_Value		(const CSG_String &Field, double            Value);
	bool						Mul_Value		(const char       *Field, double            Value) { return( Mul_Value(CSG_String(Field), Value) ); }
	bool						Mul_Value		(const wchar_t    *Field, double            Value) { return( Mul_Value(CSG_String(Field), Value) ); }

	bool						Set_NoData		(int               Field);
	bool						Set_NoData		(const CSG_String &Field);
	bool						Set_NoData		(const char       *Field)       { return( Set_NoData(CSG_String(Field)) ); }
	bool						Set_NoData		(const wchar_t    *Field)       { return( Set_NoData(CSG_String(Field)) ); }

	bool						is_NoData		(int               Field) const;
	bool						is_NoData		(const CSG_String &Field) const;
	bool						is_NoData		(const char       *Field) const { return( is_NoData(CSG_String(Field)) ); }
	bool						is_NoData		(const wchar_t    *Field) const { return( is_NoData(CSG_String(Field)) ); }

	const SG_Char *				asString		(int               Field, int Decimals = -99) const;
	const SG_Char *				asString		(const CSG_String &Field, int Decimals = -99) const;
	const SG_Char *				asString		(const char       *Field, int Decimals = -99) const { return( asString(CSG_String(Field), Decimals) ); }
	const SG_Char *				asString		(const wchar_t    *Field, int Decimals = -99) const { return( asString(CSG_String(Field), Decimals) ); }

	SG_Char						asChar			(int               Field) const;
	SG_Char						asChar			(const CSG_String &Field) const;
	SG_Char						asChar			(const char       *Field) const { return( asChar  (CSG_String(Field)) ); }
	SG_Char						asChar			(const wchar_t    *Field) const { return( asChar  (CSG_String(Field)) ); }

	short						asShort			(int               Field) const;
	short						asShort			(const CSG_String &Field) const;
	short						asShort			(const char       *Field) const { return( asShort (CSG_String(Field)) ); }
	short						asShort			(const wchar_t    *Field) const { return( asShort (CSG_String(Field)) ); }

	int							asInt			(int               Field) const;
	int							asInt			(const CSG_String &Field) const;
	int							asInt			(const char       *Field) const { return( asInt   (CSG_String(Field)) ); }
	int							asInt			(const wchar_t    *Field) const { return( asInt   (CSG_String(Field)) ); }

	sLong						asLong			(int               Field) const;
	sLong						asLong			(const CSG_String &Field) const;
	sLong						asLong			(const char       *Field) const { return( asLong  (CSG_String(Field)) ); }
	sLong						asLong			(const wchar_t    *Field) const { return( asLong  (CSG_String(Field)) ); }

	float						asFloat			(int              iField) const;
	float						asFloat			(const CSG_String &Field) const;
	float						asFloat			(const char       *Field) const { return( asFloat (CSG_String(Field)) ); }
	float						asFloat			(const wchar_t    *Field) const { return( asFloat (CSG_String(Field)) ); }

	double						asDouble		(int               Field) const;
	double						asDouble		(const CSG_String &Field) const;
	double						asDouble		(const char       *Field) const { return( asDouble(CSG_String(Field)) ); }
	double						asDouble		(const wchar_t    *Field) const { return( asDouble(CSG_String(Field)) ); }

	CSG_Table_Value *			Get_Value		(int               Field)       { return(  m_Values[Field] ); }
	CSG_Table_Value &			operator []		(int               Field) const { return( *m_Values[Field] ); }

	virtual bool				Assign			(CSG_Table_Record *pRecord);

	bool						is_Selected		(void)                    const { return( (m_Flags & SG_TABLE_REC_FLAG_Selected) != 0 ); }
	bool						is_Modified		(void)                    const { return( (m_Flags & SG_TABLE_REC_FLAG_Modified) != 0 ); }


protected:

	CSG_Table_Record(class CSG_Table *pTable, sLong Index);
	virtual ~CSG_Table_Record(void);


	char						m_Flags;

	sLong						m_Index;

	class CSG_Table_Value		**m_Values;

	class CSG_Table				*m_pTable;


	void						Set_Selected	(bool bOn = true);
	void						Set_Modified	(bool bOn = true);


	static CSG_Table_Value *	_Create_Value	(TSG_Data_Type Type);

	bool						_Add_Field		(int add_Field);
	bool						_Del_Field		(int del_Field);

	int							_Get_Field	 	(const CSG_String &Field)	const;

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

									CSG_Table			(const CSG_Table *pTemplate);
	bool							Create				(const CSG_Table *pTemplate);

									CSG_Table			(const CSG_String &File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const CSG_String &File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
									CSG_Table			(const char       *File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const char       *File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
									CSG_Table			(const wchar_t    *File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const wchar_t    *File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);

									CSG_Table			(const CSG_String &File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const CSG_String &File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
									CSG_Table			(const char       *File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const char       *File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
									CSG_Table			(const wchar_t    *File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
	bool							Create				(const wchar_t    *File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);

	virtual ~CSG_Table(void);

	virtual bool					Destroy				(void);

	virtual TSG_Data_Object_Type	Get_ObjectType		(void)	const			{	return( SG_DATAOBJECT_TYPE_Table );	}

	CSG_Table &						operator =			(const CSG_Table  &Table);
	virtual bool					Assign				(CSG_Data_Object *pTable);

	bool							Assign_Values		(const CSG_Table  &Table);
	bool							Assign_Values		(      CSG_Table *pTable);
	bool							Assign_Values		(const SG_Char *File);

	bool							Load				(const CSG_String &File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED);

	virtual bool					Save				(const CSG_String &File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED);
	virtual bool					Save				(const char       *File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED)	{ return( Save(CSG_String(File), Format, Separator, Encoding) ); }
	virtual bool					Save				(const wchar_t    *File, int Format, SG_Char Separator, int Encoding = SG_FILE_ENCODING_UNDEFINED)	{ return( Save(CSG_String(File), Format, Separator, Encoding) ); }

	virtual bool					Save				(const CSG_String &File, int Format = 0);
	virtual bool					Save				(const char       *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}
	virtual bool					Save				(const wchar_t    *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}

	bool							Set_File_Encoding	(int Encoding);
	int								Get_File_Encoding	(void)	const			{	return( m_Encoding );	}

	bool							Serialize			(CSG_File &Stream, bool bSave);

	//-----------------------------------------------------
	virtual bool					is_Valid			(void)	const			{	return( m_nFields > 0 );	}

	bool							is_Compatible		(const CSG_Table  &Table, bool bExactMatch = false)	const;
	bool							is_Compatible		(      CSG_Table *pTable, bool bExactMatch = false)	const;

	//-----------------------------------------------------
	virtual const CSG_Rect &		Get_Extent			(void)					{	return( m_Extent );	}

	//-----------------------------------------------------
	virtual bool					Add_Field			(const CSG_String &Name, TSG_Data_Type Type, int Position = -1);
	virtual bool					Add_Field			(const char       *Name, TSG_Data_Type Type, int Position = -1) { return( Add_Field(CSG_String(Name), Type, Position) ); }
	virtual bool					Add_Field			(const wchar_t    *Name, TSG_Data_Type Type, int Position = -1) { return( Add_Field(CSG_String(Name), Type, Position) ); }
	virtual bool					Del_Field			(int iField);
	virtual bool					Mov_Field			(int iField, int Position);

	int								Get_Field_Count		(void)			const	{	return( m_nFields );	}
	const SG_Char *					Get_Field_Name		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Name[iField]->c_str() : NULL );			}
	TSG_Data_Type					Get_Field_Type		(int iField)	const	{	return( iField >= 0 && iField < m_nFields ? m_Field_Type[iField] : SG_DATATYPE_Undefined );	}
	int								Get_Field_Length	(int iField, int Encoding = SG_FILE_ENCODING_UNDEFINED)	const;	// returns the maximum number of characters for data type string and zero for all other data types.
	int								Get_Field			(const CSG_String &Name)	const;	// returns the zero based position of the field named 'Name' or '-1' if there is no field with such name.

	bool							Set_Field_Name		(int iField, const SG_Char *Name);
	virtual bool					Set_Field_Type		(int iField, TSG_Data_Type  Type);

	int								Find_Field			(const CSG_String &Name            )	const;
	bool							Find_Field			(const CSG_String &Name, int &Index)	const;

	sLong							Get_N				(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Count   () : 0  );	}
	double							Get_Minimum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Minimum () : 0. );	}
	double							Get_Maximum			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Maximum () : 0. );	}
	double							Get_Range			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Range   () : 0. );	}
	double							Get_Sum				(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Sum     () : 0. );	}
	double							Get_Mean			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Mean    () : 0. );	}
	double							Get_StdDev			(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_StdDev  () : 0. );	}
	double							Get_Variance		(int iField)	const	{	return( _Stats_Update(iField) ? m_Field_Stats[iField]->Get_Variance() : 0. );	}
	const CSG_Simple_Statistics &	Get_Statistics		(int iField)	const	{	_Stats_Update(iField); return( *m_Field_Stats[iField] );	}

	//-----------------------------------------------------
	virtual CSG_Table_Record *		Add_Record			(             CSG_Table_Record *pCopy = NULL);
	virtual CSG_Table_Record *		Ins_Record			(sLong Index, CSG_Table_Record *pCopy = NULL);
	virtual bool					Set_Record			(sLong Index, CSG_Table_Record *pCopy       );
	virtual bool					Del_Record			(sLong Index);
	virtual bool					Del_Records			(void);

	virtual bool					Set_Count			(sLong nRecords);
	sLong							Get_Count			(void)			const	{	return( m_nRecords );	}

	virtual CSG_Table_Record *		Get_Record			(sLong Index)	const	{	return( Index >= 0 && Index < m_nRecords ? m_Records[Index] : NULL );	}
	virtual CSG_Table_Record &		operator []			(sLong Index)	const	{	return( *Get_Record_byIndex(Index) );	}

	sLong							Get_Index			(sLong Index)	const	{	return( Index >= 0 && Index < m_nRecords ? (is_Indexed() ? m_Index[Index] : Index) : -1 );	}

	CSG_Table_Record *				Get_Record_byIndex	(sLong Index)	const
	{
		return( Index >= 0 && Index < m_nRecords ? Get_Record(is_Indexed() ? m_Index[Index] : Index) : NULL );
	}

	//-----------------------------------------------------
	virtual bool					Find_Record			(sLong &Index, int iField, const CSG_String &Value, bool bCreateIndex = false);
	virtual CSG_Table_Record *		Find_Record			(              int iField, const CSG_String &Value, bool bCreateIndex = false);
	virtual bool					Find_Record			(sLong &Index, int iField, double            Value, bool bCreateIndex = false);
	virtual CSG_Table_Record *		Find_Record			(              int iField, double            Value, bool bCreateIndex = false);

	//-----------------------------------------------------
	virtual bool					Set_Value			(sLong Index, int iField, const SG_Char  *Value);
	virtual bool					Set_Value			(sLong Index, int iField, double          Value);

	virtual bool					Get_Value			(sLong Index, int iField, CSG_String     &Value)	const;
	virtual bool					Get_Value			(sLong Index, int iField, double         &Value)	const;

	virtual void					Set_Modified		(bool bModified = true);

	//-----------------------------------------------------
	sLong							Get_Selection_Count	(void)				const	{	return( m_Selection.Get_Size() );	}
	sLong							Get_Selection_Index	(sLong Index = 0)	const	{	return( Index >= 0 && Index < m_Selection.Get_Size() ? *((sLong *)m_Selection.Get_Entry(Index)) : Get_Count() );	}
	virtual CSG_Table_Record *		Get_Selection		(sLong Index = 0)	const	{	return( Index >= 0 && Index < m_Selection.Get_Size() ? Get_Record(Get_Selection_Index(Index)) : NULL );	}

	virtual bool					is_Selected			(sLong Index)		const	{	return( Index >= 0 && Index < m_nRecords ? m_Records[Index]->is_Selected() : false );	}

	virtual bool					Select				(sLong Index                     , bool bInvert = false);
	virtual bool					Select				(CSG_Table_Record *pRecord = NULL, bool bInvert = false);

	virtual sLong					Del_Selection		(void);
	virtual sLong					Inv_Selection		(void);

	//-----------------------------------------------------
	bool							Set_Index			(CSG_Index &Index, int Field                  , bool bAscending = true)	const;
	bool							Set_Index			(CSG_Index &Index, int Fields[], int   nFields, bool bAscending = true)	const;
	bool							Set_Index			(CSG_Index &Index, const CSG_Array_Int &Fields, bool bAscending = true)	const;

	bool							is_Indexed			(void)	const		{	return( m_nRecords > 0 && m_Index.Get_Count() == m_nRecords );	}
	bool							Set_Index			(int Field_1, TSG_Table_Index_Order Order_1 = TABLE_INDEX_Ascending, int Field_2 = -1, TSG_Table_Index_Order Order_2 = TABLE_INDEX_None, int Field_3 = -1, TSG_Table_Index_Order Order_3 = TABLE_INDEX_None);
	bool							Del_Index			(void);
	bool							Toggle_Index		(int iField);
	int								Get_Index_Field		(int    i)	const	{	return( i < 0 || i >= m_Index_Fields.Get_Size() ? -1 : abs(m_Index_Fields[i]) - 1 );	}
	TSG_Table_Index_Order			Get_Index_Order		(int    i)	const	{	return( i < 0 || i >= m_Index_Fields.Get_Size() ? TABLE_INDEX_None : m_Index_Fields[i] > 0 ? TABLE_INDEX_Ascending : TABLE_INDEX_Descending );	}
	int								Get_Index_Field		(size_t i)	const	{	return( Get_Index_Field((int)i) );	}
	TSG_Table_Index_Order			Get_Index_Order		(size_t i)	const	{	return( Get_Index_Order((int)i) );	}


protected:

	int								m_nFields, m_Encoding;

	sLong							m_nRecords, m_nBuffer;

	TSG_Data_Type					*m_Field_Type;

	CSG_String						**m_Field_Name;

	CSG_Simple_Statistics			**m_Field_Stats;

	CSG_Array						m_Selection;

	CSG_Rect						m_Extent;


	virtual void					_On_Construction	(void);

	virtual CSG_Table_Record *		_Get_New_Record		(sLong Index);

	bool							_Add_Selection		(sLong Index);
	bool							_Set_Selection		(sLong Index, sLong Selected);
	bool							_Del_Selection		(sLong Index);

	bool							_Stats_Invalidate	(void)			const;
	bool							_Stats_Invalidate	(int iField)	const;
	virtual bool					_Stats_Update		(int iField)	const;

	virtual bool					On_NoData_Changed	(void);
	virtual bool					On_Update			(void);

	virtual bool					On_Reload			(void);
	virtual bool					On_Delete			(void);


private:

	CSG_Index						m_Index;

	CSG_Array_Int					m_Index_Fields;

	CSG_Table_Record				**m_Records;


	bool							_Destroy_Selection	(void);

	bool							_Inc_Array			(void);
	bool							_Dec_Array			(void);

	size_t							_Load_Text_Trim		(      CSG_String &Text, const SG_Char Separator);
	size_t							_Load_Text_EndQuote	(const CSG_String &Text, const SG_Char Separator);

	bool							_Load_Text			(const CSG_String &File, bool bHeadline, const SG_Char Separator);
	bool							_Save_Text			(const CSG_String &File, bool bHeadline, const SG_Char Separator);

	bool							_Load_DBase			(const CSG_String &File);
	bool							_Save_DBase			(const CSG_String &File);

	void							_Index_Update		(void);

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
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(CSG_Table *pTemplate);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const char       *File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const wchar_t    *File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);
/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_String &File, TSG_Table_File_Type Format = TABLE_FILETYPE_Undefined, int Encoding = SG_FILE_ENCODING_UNDEFINED);

/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const char       *File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const wchar_t    *File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);
/** Safe table construction */
SAGA_API_DLL_EXPORT CSG_Table *	SG_Create_Table	(const CSG_String &File, TSG_Table_File_Type Format, const SG_Char Separator  , int Encoding = SG_FILE_ENCODING_UNDEFINED);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__table_H
