
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
//                     parameters.h                      //
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
#ifndef HEADER_INCLUDED__SAGA_API__parameters_H
#define HEADER_INCLUDED__SAGA_API__parameters_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"
#include "table.h"
#include "shapes.h"
#include "tin.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PARAMETER_INPUT						0x01
#define PARAMETER_OUTPUT					0x02
#define PARAMETER_OPTIONAL					0x04
#define PARAMETER_INFORMATION				0x08

#define PARAMETER_INPUT_OPTIONAL			(PARAMETER_INPUT  | PARAMETER_OPTIONAL)
#define PARAMETER_OUTPUT_OPTIONAL			(PARAMETER_OUTPUT | PARAMETER_OPTIONAL)

//---------------------------------------------------------
#define PARAMETER_DESCRIPTION_NAME			0x01
#define PARAMETER_DESCRIPTION_TYPE			0x02
#define PARAMETER_DESCRIPTION_OPTIONAL		0x04
#define PARAMETER_DESCRIPTION_PROPERTIES	0x08
#define PARAMETER_DESCRIPTION_TEXT			0x10
#define PARAMETER_DESCRIPTION_ALL			(PARAMETER_DESCRIPTION_NAME | PARAMETER_DESCRIPTION_TYPE | PARAMETER_DESCRIPTION_OPTIONAL | PARAMETER_DESCRIPTION_PROPERTIES | PARAMETER_DESCRIPTION_TEXT)


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum EParameter_Type
{
	PARAMETER_TYPE_Node		= 0,

	PARAMETER_TYPE_Bool,
	PARAMETER_TYPE_Int,
	PARAMETER_TYPE_Double,
	PARAMETER_TYPE_Degree,

	PARAMETER_TYPE_Range,
	PARAMETER_TYPE_Choice,

	PARAMETER_TYPE_String,
	PARAMETER_TYPE_Text,
	PARAMETER_TYPE_FilePath,

	PARAMETER_TYPE_Font,

	PARAMETER_TYPE_Color,
	PARAMETER_TYPE_Colors,

	PARAMETER_TYPE_FixedTable,

	PARAMETER_TYPE_Grid_System,
	PARAMETER_TYPE_Table_Field,

	PARAMETER_TYPE_Grid,
	PARAMETER_TYPE_Table,
	PARAMETER_TYPE_Shapes,
	PARAMETER_TYPE_TIN,

	PARAMETER_TYPE_Grid_List,
	PARAMETER_TYPE_Table_List,
	PARAMETER_TYPE_Shapes_List,
	PARAMETER_TYPE_TIN_List,

	PARAMETER_TYPE_DataObject_Output,

	PARAMETER_TYPE_Parameters,

	PARAMETER_TYPE_Undefined
}
TParameter_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters;
class CParameter;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef int		(* PFNC_Parameter_Changed)	(CParameter *pSender);


///////////////////////////////////////////////////////////
//														 //
//					CParameter_Data						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Data
{
public:
	CParameter_Data(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Data(void);

	virtual TParameter_Type		Get_Type				(void)			= 0;
	virtual const char *		Get_Type_Name			(void);

	long						Get_Constraint			(void)	{	return( m_Constraint );	}

	virtual bool				is_Valid				(void)	{	return( true );			}

	bool						Assign					(CParameter_Data *pSource);
	bool						Serialize				(FILE *Stream, bool bSave);

	virtual bool				Set_Value				(int Value);
	virtual bool				Set_Value				(double Value);
	virtual bool				Set_Value				(void *Value);

	virtual int					asInt					(void);
	virtual double				asDouble				(void);
	virtual void *				asPointer				(void);

	virtual const char *		asString				(void);


protected:

	int							m_Constraint;

	CSG_String					m_String;

	CParameter					*m_pOwner;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Node : public CParameter_Data
{
public:
	CParameter_Node(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Node(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Node );	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Bool : public CParameter_Data
{
public:
	CParameter_Bool(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Bool(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Bool );	}

	virtual bool				Set_Value				(int Value);
	virtual bool				Set_Value				(double Value);

	virtual int					asInt					(void)	{	return( m_Value );	}

	virtual const char *		asString				(void);


protected:

	bool						m_Value;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Value : public CParameter_Data
{
public:
	CParameter_Value(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Value(void);

	bool						Set_Range				(double Minimum, double Maximum);

	void						Set_Minimum				(double Minimum, bool bOn = true);
	double						Get_Minimum				(void)	{	return( m_Minimum );	}
	bool						has_Minimum				(void)	{	return( m_bMinimum );	}

	void						Set_Maximum				(double Maximum, bool bOn = true);
	double						Get_Maximum				(void)	{	return( m_Maximum );	}
	bool						has_Maximum				(void)	{	return( m_bMaximum );	}


protected:

	bool						m_bMinimum, m_bMaximum;

	double						m_Minimum, m_Maximum;


	virtual void				On_Assign				(CParameter_Data *pSource);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Int : public CParameter_Value
{
public:
	CParameter_Int(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Int(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Int );		}

	virtual bool				Set_Value				(void *Value);
	virtual bool				Set_Value				(int Value);
	virtual bool				Set_Value				(double Value);

	virtual int					asInt					(void)	{	return( m_Value );		}
	virtual double				asDouble				(void)	{	return( m_Value );		}
	virtual const char *		asString				(void);


protected:

	int							m_Value;

	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Double : public CParameter_Value
{
public:
	CParameter_Double(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Double(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Double );	}

	virtual bool				Set_Value				(int Value);
	virtual bool				Set_Value				(double Value);
	virtual bool				Set_Value				(void *Value);

	virtual int					asInt					(void)	{	return( (int)m_Value );	}
	virtual double				asDouble				(void)	{	return( m_Value );		}
	virtual const char *		asString				(void);


protected:

	double						m_Value;

	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Degree : public CParameter_Double
{
public:
	CParameter_Degree(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Degree(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Degree );	}

	virtual bool				Set_Value				(void *Value);

	virtual const char *		asString				(void);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Range : public CParameter_Data
{
public:
	CParameter_Range(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Range(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Range );		}

	virtual const char *		asString				(void);

	bool						Set_Range				(double loVal, double hiVal);

	bool						Set_LoVal				(double newValue);
	double						Get_LoVal				(void);

	bool						Set_HiVal				(double newValue);
	double						Get_HiVal				(void);

	CParameter *				Get_LoParm				(void)	{	return( pLo );	}
	CParameter *				Get_HiParm				(void)	{	return( pHi );	}


protected:

	CParameter					*pLo, *pHi;

	CParameters					*pRange;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Choice : public CParameter_Int
{
public:
	CParameter_Choice(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Choice(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Choice );	}

	virtual const char *		asString				(void);

	void						Set_Items				(const char *String);

	const char *				Get_Item				(int Index);

	int							Get_Count				(void)	{	return( nItems );	}


protected:

	CSG_String					**Items;

	int							nItems;


	void						Del_Items				(void);

	virtual void				On_Assign				(CParameter_Data *pSource);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Table_Field : public CParameter_Int
{
public:
	CParameter_Table_Field(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Table_Field(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Table_Field );	}

	virtual const char *		asString				(void);

	virtual bool				Set_Value				(int Value);

	CTable *					Get_Table				(void);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_String : public CParameter_Data
{
public:
	CParameter_String(CParameter *pOwner, long Constraint);
	virtual ~CParameter_String(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_String );	}

	virtual const char *		asString				(void);

	virtual bool				is_Valid				(void);

	virtual bool				Set_Value				(void *Value);

	void						Set_Password			(bool bOn);
	bool						is_Password				(void);


protected:

	bool						bPassword;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Text : public CParameter_String
{
public:
	CParameter_Text(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Text(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Text );		}


protected:

	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_FilePath : public CParameter_String
{
public:
	CParameter_FilePath(CParameter *pOwner, long Constraint);
	virtual ~CParameter_FilePath(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_FilePath );	}

	void						Set_Filter				(const char *Filter);
	const char *				Get_Filter				(void);

	void						Set_Flag_Save			(bool bFlag);
	bool						is_Save					(void)	{	return( m_bSave );		}

	void						Set_Flag_Multiple		(bool bFlag);
	bool						is_Multiple				(void)	{	return( m_bMultiple );	}

	void						Set_Flag_Directory		(bool bFlag);
	bool						is_Directory			(void)	{	return( m_bDirectory );	}

	bool						Get_FilePaths			(CSG_Strings &FilePaths);


protected:

	bool						m_bSave, m_bMultiple, m_bDirectory;

	CSG_String					m_Filter;


	virtual void				On_Assign				(CParameter_Data *pSource);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Font : public CParameter_Data
{
public:
	CParameter_Font(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Font(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Font );		}

	virtual bool				Set_Value				(int Value);
	virtual bool				Set_Value				(void *Value);

	virtual int					asInt					(void)	{	return( m_Color );	}
	virtual void *				asPointer				(void)	{	return( m_pFont );	}

	virtual const char *		asString				(void);


protected:

	int							m_Color;

	class wxFont				*m_pFont;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Color : public CParameter_Int
{
public:
	CParameter_Color(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Color(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Color );		}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Colors : public CParameter_Data
{
public:
	CParameter_Colors(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Colors(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Colors );	}

	virtual int					asInt					(void)	{	return( m_Colors.Get_Count() );	}
	virtual void *				asPointer				(void)	{	return( &m_Colors );	}
	virtual const char *		asString				(void);


protected:

	CSG_Colors					m_Colors;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_FixedTable : public CParameter_Data
{
public:
	CParameter_FixedTable(CParameter *pOwner, long Constraint);
	virtual ~CParameter_FixedTable(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_FixedTable );	}

	virtual void *				asPointer				(void)	{	return( &m_Table );	}

	virtual const char *		asString				(void);


protected:

	CTable						m_Table;

	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Grid_System : public CParameter_Data
{
public:
	CParameter_Grid_System(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Grid_System(void);

	virtual TParameter_Type		Get_Type				(void)	{	return( PARAMETER_TYPE_Grid_System );	}

	virtual bool				Set_Value				(void *Value);

	virtual void *				asPointer				(void)	{	return( &m_System );}

	virtual const char *		asString				(void);


protected:

	CGrid_System				m_System;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_DataObject : public CParameter_Data
{
public:
	CParameter_DataObject(CParameter *pOwner, long Constraint);
	virtual ~CParameter_DataObject(void);

	virtual bool				is_Valid				(void);

	virtual bool				Set_Value				(int Value)	{	return( Set_Value((void *)Value) );	}
	virtual bool				Set_Value				(void *Value);

	virtual void *				asPointer				(void)		{	return( m_pDataObject );	}

	virtual const char *		asString				(void);


protected:

	CDataObject					*m_pDataObject;


	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_DataObject_Output : public CParameter_DataObject
{
public:
	CParameter_DataObject_Output(CParameter *pOwner, long Constraint);
	virtual ~CParameter_DataObject_Output(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_DataObject_Output );	}

	virtual bool				Set_Value				(void *Value);

	bool						Set_DataObject_Type		(TDataObject_Type Type);
	TDataObject_Type			Get_DataObject_Type		(void)		{	return( m_Type );	}


protected:

	TDataObject_Type			m_Type;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Grid : public CParameter_DataObject
{
public:
	CParameter_Grid(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Grid(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Grid );	}

	virtual bool				Set_Value				(void *Value);

	void						Set_Preferred_Type		(TGrid_Type Type);
	TGrid_Type					Get_Preferred_Type		(void)		{	return( m_Type );	}

	CGrid_System *				Get_System				(void);


protected:

	TGrid_Type					m_Type;


	virtual void				On_Assign				(CParameter_Data *pSource);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Table : public CParameter_DataObject
{
public:
	CParameter_Table(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Table(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Table );		}

	virtual bool				Set_Value				(void *Value);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Shapes : public CParameter_DataObject
{
public:
	CParameter_Shapes(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Shapes(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Shapes );	}

	virtual bool				Set_Value				(void *Value);

	void						Set_Shape_Type			(TShape_Type Type);
	TShape_Type					Get_Shape_Type			(void)		{	return( m_Type );	}


protected:

	TShape_Type					m_Type;


	virtual void				On_Assign				(CParameter_Data *pSource);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_TIN : public CParameter_DataObject
{
public:
	CParameter_TIN(CParameter *pOwner, long Constraint);
	virtual ~CParameter_TIN(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_TIN );		}

	virtual bool				Set_Value				(void *Value);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_List : public CParameter_Data
{
public:
	CParameter_List(CParameter *pOwner, long Constraint);
	virtual ~CParameter_List(void);

	virtual const char *		asString				(void);
	virtual int					asInt					(void)			{	return( m_nObjects );	}
	virtual void *				asPointer				(void)			{	return( m_Objects );	}

	void						Add_Item				(CDataObject *pItem);
	int							Del_Item				(int Index);
	int							Del_Item				(CDataObject *pItem);
	void						Del_Items				(void);

	int							Get_Count				(void)			{	return( m_nObjects );	}
	CDataObject *				asDataObject			(int iObject)	{	return( iObject >= 0 && iObject < m_nObjects ? m_Objects[iObject] : NULL );	}


protected:

	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);


private:

	int							m_nObjects;

	CDataObject					**m_Objects;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Grid_List : public CParameter_List
{
public:
	CParameter_Grid_List(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Grid_List(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Grid_List );		}

	CGrid_System *				Get_System				(void);

	CGrid *						asGrid					(int Index)	{	return( (CGrid *)asDataObject(Index) );	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Table_List : public CParameter_List
{
public:
	CParameter_Table_List(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Table_List(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Table_List );		}

	CTable *					asTable					(int Index)	{	return( (CTable *)asDataObject(Index) );	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Shapes_List : public CParameter_List
{
public:
	CParameter_Shapes_List(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Shapes_List(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Shapes_List );		}

	void						Set_Shape_Type			(TShape_Type Type);
	TShape_Type					Get_Shape_Type			(void)		{	return( m_Type );}

	CShapes *					asShapes				(int Index)	{	return( (CShapes *)asDataObject(Index) );	}


protected:

	TShape_Type					m_Type;


	virtual void				On_Assign				(CParameter_Data *pSource);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_TIN_List : public CParameter_List
{
public:
	CParameter_TIN_List(CParameter *pOwner, long Constraint);
	virtual ~CParameter_TIN_List(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_TIN_List );		}

	CTIN *						asTIN					(int Index)	{	return( (CTIN *)asDataObject(Index) );	}

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter_Parameters : public CParameter_Data
{
public:
	CParameter_Parameters(CParameter *pOwner, long Constraint);
	virtual ~CParameter_Parameters(void);

	virtual TParameter_Type		Get_Type				(void)		{	return( PARAMETER_TYPE_Parameters );	}

	virtual const char *		asString				(void);
	virtual void *				asPointer				(void)		{	return( m_pParameters );	}


protected:

	virtual void				On_Assign				(CParameter_Data *pSource);
	virtual bool				On_Serialize			(FILE *Stream, bool bSave);


private:

	CParameters					*m_pParameters;

};


///////////////////////////////////////////////////////////
//														 //
//					CParameter							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameter
{
	friend class CParameters;

public:

	//-----------------------------------------------------
	CParameters *				Get_Owner				(void)	{	return( m_pOwner );					}
	CParameter *				Get_Parent				(void)	{	return( m_pParent );				}
	TParameter_Type				Get_Type				(void)	{	return( m_pData->Get_Type() );		}
	const char *				Get_Type_Name			(void)	{	return( m_pData->Get_Type_Name() );	}
	CParameter_Data *			Get_Data				(void)	{	return( m_pData );					}

	const char *				Get_Identifier			(void);
	const char *				Get_Name				(void);
	const char *				Get_Description			(void);
	CSG_String					Get_Description			(int Flags, const char *Separator = "\n");

	bool						is_Valid				(void)	{	return( m_pData->is_Valid() );		}
	bool						is_Input				(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_INPUT)	      );	}
	bool						is_Output				(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_OUTPUT)      );	}
	bool						is_Optional				(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_OPTIONAL)    );	}
	bool						is_Information			(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_INFORMATION) );	}
	bool						is_Option				(void);
	bool						is_DataObject			(void);
	bool						is_DataObject_List		(void);
	bool						is_Parameters			(void);
	bool						is_Serializable			(void);

	int							Get_Children_Count		(void)			{	return( m_nChildren );		}
	CParameter *				Get_Child				(int iChild)	{	return( iChild >= 0 && iChild < m_nChildren ? m_Children[iChild] : NULL );	}

	//-----------------------------------------------------
	bool						Set_Value				(int         Value);
	bool						Set_Value				(double      Value);
	bool						Set_Value				(void       *Value);
	bool						Set_Value				(const char *Value);

	bool						asBool					(void)	{	return( (bool          )!!m_pData->asInt	() );	}
	int							asInt					(void)	{	return( (int           )m_pData->asInt		() );	}
	long						asColor					(void)	{	return( (long          )m_pData->asInt		() );	}
	double						asDouble				(void)	{	return( (double        )m_pData->asDouble	() );	}

	void *						asPointer				(void)	{	return( (void         *)m_pData->asPointer	() );	}

	const char *				asString				(void)	{	return( (const char   *)m_pData->asString	() );	}
	class wxFont *				asFont					(void)	{	return( (wxFont       *)m_pData->asPointer	() );	}
	CSG_Colors *				asColors				(void)	{	return( (CSG_Colors      *)m_pData->asPointer	() );	}
	CGrid_System *				asGrid_System			(void)	{	return( (CGrid_System *)m_pData->asPointer	() );	}

	CDataObject *				asDataObject			(void)	{	return( (CDataObject  *)m_pData->asPointer	() );	}
	CGrid *						asGrid					(void)	{	return( (CGrid        *)m_pData->asPointer	() );	}
	CTable *					asTable					(void)	{	return( (CTable       *)m_pData->asPointer	() );	}
	CShapes *					asShapes				(void)	{	return( (CShapes      *)m_pData->asPointer	() );	}
	CTIN *						asTIN					(void)	{	return( (CTIN         *)m_pData->asPointer	() );	}

	CParameter_Value *			asValue					(void)	{	return( (CParameter_Value       *)m_pData );		}
	CParameter_Choice *			asChoice				(void)	{	return( (CParameter_Choice      *)m_pData );		}
	CParameter_Range *			asRange					(void)	{	return( (CParameter_Range       *)m_pData );		}
	CParameter_FilePath *		asFilePath				(void)	{	return( (CParameter_FilePath    *)m_pData );		}

	CParameter_List *			asList					(void)	{	return( (CParameter_List        *)m_pData );		}
	CParameter_Grid_List *		asGridList				(void)	{	return( (CParameter_Grid_List   *)m_pData );		}
	CParameter_Table_List *		asTableList				(void)	{	return( (CParameter_Table_List  *)m_pData );		}
	CParameter_Shapes_List *	asShapesList			(void)	{	return( (CParameter_Shapes_List *)m_pData );		}
	CParameter_TIN_List *		asTINList				(void)	{	return( (CParameter_TIN_List    *)m_pData );		}

	CParameters *				asParameters			(void)	{	return( (CParameters *)m_pData->asPointer	() );	}

	//-----------------------------------------------------
	bool						Assign					(CParameter *pSource);
	bool						Serialize				(FILE *Stream, bool bSave);


private:

	CParameter(CParameters *pOwner, CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, int Constraint);

	virtual ~CParameter(void);

	void						_Add_Child				(CParameter *pChild);


	int							m_nChildren;

	CParameter					**m_Children;

	CSG_String					m_Identifier, m_Name, m_Description;

	CParameter_Data				*m_pData;

	CParameter					*m_pParent;

	CParameters					*m_pOwner;

};


///////////////////////////////////////////////////////////
//														 //
//					CParameters							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CParameters
{
	friend class CParameter;
	friend class CModule;

public:
	CParameters(void);
	virtual ~CParameters(void);

								CParameters				(void *pOwner, const char *Name, const char *Description, const char *Identifier = NULL, bool bGrid_System = false);
	void						Create					(void *pOwner, const char *Name, const char *Description, const char *Identifier = NULL, bool bGrid_System = false);

	void						Destroy					(void);

	//-----------------------------------------------------
	void *						Get_Owner				(void)	{	return( m_pOwner );			}
	int							Get_Count				(void)	{	return( m_nParameters );	}

	void						Set_Identifier			(const char *String);
	const char *				Get_Identifier			(void);

	void						Set_Name				(const char *String);
	const char *				Get_Name				(void);

	void						Set_Description			(const char *String);
	const char *				Get_Description			(void);

	void						Set_Translation			(CSG_Translator &Translator);

	//-----------------------------------------------------
	void						Set_Callback_On_Parameter_Changed	(PFNC_Parameter_Changed pCallback);
	void						Set_Callback			(bool bActive = true);

	//-----------------------------------------------------
	CParameter *				Get_Parameter			(const char *Identifier);
	CParameter *				operator()				(const char *Identifier)	{	return( Get_Parameter(Identifier) );	}

	CParameter *				Get_Parameter			(int iParameter);
	CParameter *				operator()				(int iParameter)			{	return( Get_Parameter(iParameter) );	}

	//-----------------------------------------------------
	CParameter *				Add_Node				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);

	CParameter *				Add_Value				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CParameter *				Add_Info_Value			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, double Value = 0.0);

	CParameter *				Add_Range				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, double Range_Min = 0.0, double Range_Max = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CParameter *				Add_Info_Range			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, double Range_Min = 0.0, double Range_Max = 0.0);

	CParameter *				Add_Choice				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *Items, int Default = 0);

	CParameter *				Add_String				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *String, bool bLongText = false, bool bPassword = false);
	CParameter *				Add_Info_String			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *String, bool bLongText = false);

	CParameter *				Add_FilePath			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, const char *Filter = NULL, const char *Default = NULL, bool bSave = false, bool bDirectory = false, bool bMultiple = false);

	CParameter *				Add_Font				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, class wxFont *pInit = NULL);
	CParameter *				Add_Colors				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, CSG_Colors      *pInit = NULL);
	CParameter *				Add_FixedTable			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, CTable   *pTemplate = NULL);

	CParameter *				Add_Grid_System			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, CGrid_System *pInit = NULL);
	CParameter *				Add_Grid				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, bool bSystem_Dependent = true, TGrid_Type Preferred_Type = GRID_TYPE_Undefined);
	CParameter *				Add_Grid_Output			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);
	CParameter *				Add_Grid_List			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, bool bSystem_Dependent = true);

	CParameter *				Add_Table_Field			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);
	CParameter *				Add_Table				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint);
	CParameter *				Add_Table_Output		(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);
	CParameter *				Add_Table_List			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint);

	CParameter *				Add_Shapes				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, TShape_Type Shape_Type = SHAPE_TYPE_Undefined);
	CParameter *				Add_Shapes_Output		(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);
	CParameter *				Add_Shapes_List			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint, TShape_Type = SHAPE_TYPE_Undefined);

	CParameter *				Add_TIN					(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint);
	CParameter *				Add_TIN_Output			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);
	CParameter *				Add_TIN_List			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, int Constraint);

	CParameter *				Add_Parameters			(CParameter *pParent, const char *Identifier, const char *Name, const char *Description);

	//-----------------------------------------------------
	bool						Set_Parameter			(const char *Identifier, CParameter *pSource);
	bool						Set_Parameter			(const char *Identifier, int Type, int         Value);
	bool						Set_Parameter			(const char *Identifier, int Type, double      Value);
	bool						Set_Parameter			(const char *Identifier, int Type, void       *Value);
	bool						Set_Parameter			(const char *Identifier, int Type, const char *Value);

	int							Assign					(CParameters *pSource);
	int							Assign_Values			(CParameters *pSource);

	bool						Serialize				(const char *File_Name	, bool bSave);
	bool						Serialize				(FILE *Stream			, bool bSave);

	//-----------------------------------------------------
	bool						DataObjects_Check		(bool bSilent = false);

	//-----------------------------------------------------
	bool						Get_String				(CSG_String &String, bool bOptionsOnly);
	bool						Msg_String				(bool bOptionsOnly);

	bool						Set_History				(CSG_History &History, bool bOptions = true, bool bDataObjects = true);

	CGrid_System *				Get_Grid_System			(void)	{	return( m_pGrid_System ? m_pGrid_System->asGrid_System() : NULL );	}

	bool						is_Managed				(void)	{	return( m_bManaged );	}


private:

	void						*m_pOwner;

	bool						m_bCallback, m_bManaged;

	CSG_String					m_Identifier, m_Name, m_Description;

	int							m_nParameters;

	CParameter					**m_Parameters, *m_pGrid_System;

	PFNC_Parameter_Changed		m_Callback;


	void						_On_Construction		(void);

	bool						_On_Parameter_Changed	(CParameter *pSender);

	CParameter *				_Add_Value				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, bool bInformation, TParameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum);
	CParameter *				_Add_Range				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, bool bInformation, double Range_Min, double Range_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum);
	CParameter *				_Add_String				(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, bool bInformation, const char *String, bool bLongText, bool bPassword);

	CParameter *				_Add					(CParameter *pParent, const char *Identifier, const char *Name, const char *Description, TParameter_Type Type, int Constraint);
	CParameter *				_Add					(CParameter *pSource);

	bool						DataObjects_Create		(void);
	bool						DataObjects_Synchronize	(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__parameters_H
