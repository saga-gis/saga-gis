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
#include "pointcloud.h"
#include "stdint.h"

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
#define PARAMETER_IGNORE_PROJECTION			0x10

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
typedef enum ESG_Parameter_Type
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

	PARAMETER_TYPE_PointCloud,
	PARAMETER_TYPE_Grid,
	PARAMETER_TYPE_Table,
	PARAMETER_TYPE_Shapes,
	PARAMETER_TYPE_TIN,

	PARAMETER_TYPE_Grid_List,
	PARAMETER_TYPE_Table_List,
	PARAMETER_TYPE_Shapes_List,
	PARAMETER_TYPE_TIN_List,
	PARAMETER_TYPE_PointCloud_List,

	PARAMETER_TYPE_DataObject_Output,

	PARAMETER_TYPE_Parameters,

	PARAMETER_TYPE_Undefined
}
TSG_Parameter_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_String			SG_Parameter_Type_Get_Name			(TSG_Parameter_Type Type);
SAGA_API_DLL_EXPORT CSG_String			SG_Parameter_Type_Get_Identifier	(TSG_Parameter_Type Type);
SAGA_API_DLL_EXPORT TSG_Parameter_Type	SG_Parameter_Type_Get_Type			(const CSG_String &Identifier);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Parameters;
class CSG_Parameter;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PARAMETER_CHECK_VALUES				0x01
#define PARAMETER_CHECK_ENABLE				0x02
#define PARAMETER_CHECK_ALL					(PARAMETER_CHECK_VALUES | PARAMETER_CHECK_ENABLE)

//---------------------------------------------------------
typedef int		(* TSG_PFNC_Parameter_Changed)	(CSG_Parameter *pParameter, int Flags);


///////////////////////////////////////////////////////////
//														 //
//					CSG_Parameter_Data					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Data
{
public:
	CSG_Parameter_Data(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Data(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)			= 0;
	virtual CSG_String			Get_Type_Identifier		(void);
	virtual CSG_String			Get_Type_Name			(void);

	long						Get_Constraint			(void)	{	return( m_Constraint );	}

	virtual bool				is_Valid				(void)	{	return( true );			}

	bool						Assign					(CSG_Parameter_Data *pSource);
	bool						Serialize				(CSG_MetaData &Entry, bool bSave);

	virtual bool				Set_Value				(int               Value);
	virtual bool				Set_Value				(double            Value);
	virtual bool				Set_Value				(const CSG_String &Value);
	virtual bool				Set_Value				(void             *Value);

	virtual int					asInt					(void);
	virtual double				asDouble				(void);
	virtual void *				asPointer				(void);

	virtual const SG_Char *		asString				(void);

	CSG_String					Get_Default				(void)	{	return( m_Default );	}
	void						Set_Default				(int               Value);
	void						Set_Default				(double            Value);
	void						Set_Default				(const CSG_String &Value);

	virtual bool				Restore_Default			(void)	{	return( false );		}


protected:

	int							m_Constraint;

	CSG_String					m_String, m_Default;

	CSG_Parameter				*m_pOwner;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Node : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Node(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Node(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Node );	}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Bool : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Bool(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Bool(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Bool );	}

	virtual bool				Set_Value				(int    Value);
	virtual bool				Set_Value				(double Value);

	virtual int					asInt					(void)	{	return( m_Value );		}

	virtual const SG_Char *		asString				(void);

	virtual bool				Restore_Default			(void)	{	return( Set_Value(m_Default.asInt()) );	}


protected:

	bool						m_Value;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Value : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Value(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Value(void)	{}

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


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Int : public CSG_Parameter_Value
{
public:
	CSG_Parameter_Int(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Int(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Int );		}

	virtual bool				Set_Value				(int               Value);
	virtual bool				Set_Value				(double            Value);
	virtual bool				Set_Value				(const CSG_String &Value);

	virtual int					asInt					(void)	{	return( m_Value );		}
	virtual double				asDouble				(void)	{	return( m_Value );		}
	virtual const SG_Char *		asString				(void);

	virtual bool				Restore_Default			(void)	{	return( Set_Value(m_Default.asInt()) );	}


protected:

	int							m_Value;

	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Double : public CSG_Parameter_Value
{
public:
	CSG_Parameter_Double(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Double(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Double );	}

	virtual bool				Set_Value				(int               Value);
	virtual bool				Set_Value				(double            Value);
	virtual bool				Set_Value				(const CSG_String &Value);

	virtual int					asInt					(void)	{	return( (int)m_Value );	}
	virtual double				asDouble				(void)	{	return( m_Value );		}
	virtual const SG_Char *		asString				(void);

	virtual bool				Restore_Default			(void)	{	return( Set_Value(m_Default.asDouble()) );	}


protected:

	double						m_Value;

	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Degree : public CSG_Parameter_Double
{
public:
	CSG_Parameter_Degree(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Degree(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Degree );	}

	virtual bool				Set_Value				(const CSG_String &Value);

	virtual const SG_Char *		asString				(void);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Range : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Range(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Range(void);

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Range );		}

	virtual const SG_Char *		asString				(void);

	bool						Set_Range				(double loVal, double hiVal);

	bool						Set_LoVal				(double newValue);
	double						Get_LoVal				(void);

	bool						Set_HiVal				(double newValue);
	double						Get_HiVal				(void);

	CSG_Parameter *				Get_LoParm				(void)	{	return( m_pLo );	}
	CSG_Parameter *				Get_HiParm				(void)	{	return( m_pHi );	}

	virtual bool				Restore_Default			(void);


protected:

	CSG_Parameter				*m_pLo, *m_pHi;

	CSG_Parameters				*m_pRange;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Choice : public CSG_Parameter_Int
{
public:
	CSG_Parameter_Choice(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Choice(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Choice );	}

	virtual bool				Set_Value				(const CSG_String &Value);

	virtual const SG_Char *		asString				(void);

	void						Set_Items				(const SG_Char *String);

	const SG_Char *				Get_Item				(int Index);

	bool						Get_Data				(int        &Value);
	bool						Get_Data				(double     &Value);
	bool						Get_Data				(CSG_String &Value);

	int							Get_Count				(void)	{	return( m_Items.Get_Count() );	}


protected:

	CSG_Strings					m_Items;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_String : public CSG_Parameter_Data
{
public:
	CSG_Parameter_String(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_String(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_String );	}

	virtual const SG_Char *		asString				(void)		{	return( m_String );	}

	virtual bool				is_Valid				(void);

	virtual bool				Set_Value				(const CSG_String &Value);

	void						Set_Password			(bool bOn)	{	m_bPassword	= bOn;		}
	bool						is_Password				(void)		{	return( m_bPassword );	}

	virtual bool				Restore_Default			(void)		{	return( Set_Value(m_Default) );	}


protected:

	bool						m_bPassword;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Text : public CSG_Parameter_String
{
public:
	CSG_Parameter_Text(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Text(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Text );		}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_File_Name : public CSG_Parameter_String
{
public:
	CSG_Parameter_File_Name(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_File_Name(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_FilePath );	}

	void						Set_Filter				(const SG_Char *Filter);
	const SG_Char *				Get_Filter				(void);

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


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Font : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Font(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Font(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Font );		}

	virtual int					asInt					(void)	{	return( m_Color );					}
	virtual void *				asPointer				(void)	{	return( (void *)m_Font.c_str() );	}
	virtual const SG_Char *		asString				(void)	{	return( m_String );					}

	virtual bool				Set_Value				(int   Value);
	virtual bool				Set_Value				(const CSG_String &Value);

	virtual bool				Restore_Default			(void);


protected:

	int							m_Color;

	CSG_String					m_Font;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Color : public CSG_Parameter_Int
{
public:
	CSG_Parameter_Color(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Color(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Color );		}


protected:

	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Colors : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Colors(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Colors(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Colors );	}

	virtual int					asInt					(void)	{	return( m_Colors.Get_Count() );	}
	virtual void *				asPointer				(void)	{	return( &m_Colors );	}
	virtual const SG_Char *		asString				(void);


protected:

	CSG_Colors					m_Colors;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Fixed_Table : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Fixed_Table(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Fixed_Table(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_FixedTable );	}

	virtual void *				asPointer				(void)	{	return( &m_Table );	}

	virtual const SG_Char *		asString				(void);


protected:

	CSG_Table					m_Table;

	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grid_System : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Grid_System(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Grid_System(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Grid_System );	}

	virtual bool				Set_Value				(void *Value);

	virtual void *				asPointer				(void)	{	return( &m_System );}

	virtual const SG_Char *		asString				(void);


protected:

	CSG_Grid_System				m_System;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table_Field : public CSG_Parameter_Int
{
public:
	CSG_Parameter_Table_Field(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Table_Field(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)	{	return( PARAMETER_TYPE_Table_Field );	}

	virtual const SG_Char *		asString				(void);

	virtual bool				Set_Value				(int               Value);
	virtual bool				Set_Value				(const CSG_String &Value);

	CSG_Table *					Get_Table				(void);


protected:

	bool						m_bAllowNone;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Data_Object : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Data_Object(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Data_Object(void)	{}

	virtual bool				is_Valid				(void);

	virtual bool				Set_Value				(uintptr_t Value)	{	return( Set_Value((void *)Value) );	}
	virtual bool				Set_Value				(void *Value);

	virtual void *				asPointer				(void)			{	return( m_pDataObject );	}

	virtual const SG_Char *		asString				(void);


protected:

	CSG_Data_Object				*m_pDataObject;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Data_Object_Output : public CSG_Parameter_Data_Object
{
public:
	CSG_Parameter_Data_Object_Output(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Data_Object_Output(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_DataObject_Output );	}

	virtual bool				Set_Value				(void *Value);

	bool						Set_DataObject_Type		(TSG_Data_Object_Type Type);
	TSG_Data_Object_Type		Get_DataObject_Type		(void)		{	return( m_Type );	}


protected:

	TSG_Data_Object_Type		m_Type;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grid : public CSG_Parameter_Data_Object
{
public:
	CSG_Parameter_Grid(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Grid(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Grid );	}

	virtual bool				Set_Value				(void *Value);

	void						Set_Preferred_Type		(TSG_Data_Type Type);
	TSG_Data_Type				Get_Preferred_Type		(void)		{	return( m_Type );	}

	CSG_Grid_System *			Get_System				(void);


protected:

	TSG_Data_Type				m_Type;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table : public CSG_Parameter_Data_Object
{
public:
	CSG_Parameter_Table(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Table(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Table );		}

	virtual bool				Set_Value				(void *Value);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Shapes : public CSG_Parameter_Data_Object
{
public:
	CSG_Parameter_Shapes(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Shapes(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Shapes );	}

	virtual bool				Set_Value				(void *Value);

	void						Set_Shape_Type			(TSG_Shape_Type Type);
	TSG_Shape_Type				Get_Shape_Type			(void)		{	return( m_Type );	}


protected:

	TSG_Shape_Type				m_Type;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_TIN : public CSG_Parameter_Data_Object
{
public:
	CSG_Parameter_TIN(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_TIN(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_TIN );		}

	virtual bool				Set_Value				(void *Value);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_PointCloud : public CSG_Parameter_Data_Object
{
public:
	CSG_Parameter_PointCloud(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_PointCloud(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_PointCloud );	}

	virtual bool				Set_Value				(void *Value);


protected:

	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_List : public CSG_Parameter_Data
{
public:
	CSG_Parameter_List(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_List(void);

	virtual const SG_Char *		asString				(void);
	virtual int					asInt					(void)			{	return( m_nObjects );	}
	virtual void *				asPointer				(void)			{	return( m_Objects );	}

	void						Add_Item				(CSG_Data_Object *pItem);
	int							Del_Item				(int Index);
	int							Del_Item				(CSG_Data_Object *pItem);
	void						Del_Items				(void);

	int							Get_Count				(void)			{	return( m_nObjects );	}
	CSG_Data_Object *			asDataObject			(int iObject)	{	return( iObject >= 0 && iObject < m_nObjects ? m_Objects[iObject] : NULL );	}


protected:

	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);


private:

	int							m_nObjects;

	CSG_Data_Object				**m_Objects;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grid_List : public CSG_Parameter_List
{
public:
	CSG_Parameter_Grid_List(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Grid_List(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Grid_List );		}

	CSG_Grid_System *			Get_System				(void);

	CSG_Grid *					asGrid					(int Index)	{	return( (CSG_Grid *)asDataObject(Index) );	}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table_List : public CSG_Parameter_List
{
public:
	CSG_Parameter_Table_List(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Table_List(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Table_List );		}

	CSG_Table *					asTable					(int Index)	{	return( (CSG_Table *)asDataObject(Index) );	}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Shapes_List : public CSG_Parameter_List
{
public:
	CSG_Parameter_Shapes_List(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Shapes_List(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Shapes_List );		}

	void						Set_Shape_Type			(TSG_Shape_Type Type);
	TSG_Shape_Type				Get_Shape_Type			(void)		{	return( m_Type );}

	CSG_Shapes *				asShapes				(int Index)	{	return( (CSG_Shapes *)asDataObject(Index) );	}


protected:

	TSG_Shape_Type				m_Type;


	virtual void				On_Assign				(CSG_Parameter_Data *pSource);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_TIN_List : public CSG_Parameter_List
{
public:
	CSG_Parameter_TIN_List(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_TIN_List(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_TIN_List );		}

	CSG_TIN *					asTIN					(int Index)	{	return( (CSG_TIN *)asDataObject(Index) );	}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_PointCloud_List : public CSG_Parameter_List
{
public:
	CSG_Parameter_PointCloud_List(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_PointCloud_List(void)	{}

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_PointCloud_List );		}

	CSG_PointCloud *			asPointCloud			(int Index)	{	return( (CSG_PointCloud *)asDataObject(Index) );	}

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Parameters : public CSG_Parameter_Data
{
public:
	CSG_Parameter_Parameters(CSG_Parameter *pOwner, long Constraint);
	virtual ~CSG_Parameter_Parameters(void);

	virtual TSG_Parameter_Type	Get_Type				(void)		{	return( PARAMETER_TYPE_Parameters );	}

	virtual const SG_Char *		asString				(void);
	virtual void *				asPointer				(void)		{	return( m_pParameters );	}

	virtual bool				Restore_Default			(void);


protected:

	virtual void				On_Assign				(CSG_Parameter_Data *pSource);
	virtual bool				On_Serialize			(CSG_MetaData &Entry, bool bSave);


private:

	CSG_Parameters				*m_pParameters;

};


///////////////////////////////////////////////////////////
//														 //
//				Grid Target Selector					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameters_Grid_Target
{
public:
	CSG_Parameters_Grid_Target(void);

	void						Create					(void);

	bool						Add_Parameters_User		(CSG_Parameters *pParameters, bool bAddDefaultGrid = true);
	bool						Add_Parameters_Grid		(CSG_Parameters *pParameters, bool bAddDefaultGrid = true);
	bool						Add_Parameters_System	(CSG_Parameters *pParameters);

	bool						Add_Grid_Parameter		(const CSG_String &Identifier, const CSG_String &Name, bool bOptional);

	bool						On_User_Changed			(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool						Init_User				(const TSG_Rect &Extent, int Rows = 100);

	CSG_Grid *					Get_User				(                              TSG_Data_Type Type = SG_DATATYPE_Float);
	CSG_Grid *					Get_User				(const CSG_String &Identifier, TSG_Data_Type Type = SG_DATATYPE_Float);
	CSG_Grid *					Get_Grid				(                              TSG_Data_Type Type = SG_DATATYPE_Float);
	CSG_Grid *					Get_Grid				(const CSG_String &Identifier, TSG_Data_Type Type = SG_DATATYPE_Float);

	bool						Get_System_User			(CSG_Grid_System &System);
	bool						Get_System				(CSG_Grid_System &System);


private:

	CSG_Parameters				*m_pUser, *m_pGrid, *m_pSystem;

};


///////////////////////////////////////////////////////////
//														 //
//					CSG_Parameter						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter
{
	friend class CSG_Parameters;

public:

	//-----------------------------------------------------
	CSG_Parameters *			Get_Owner				(void)	{	return( m_pOwner );							}
	CSG_Parameter *				Get_Parent				(void)	{	return( m_pParent );						}
	TSG_Parameter_Type			Get_Type				(void)	{	return( m_pData->Get_Type() );				}
	CSG_String					Get_Type_Identifier		(void)	{	return( m_pData->Get_Type_Identifier() );	}
	CSG_String					Get_Type_Name			(void)	{	return( m_pData->Get_Type_Name() );			}
	CSG_Parameter_Data *		Get_Data				(void)	{	return( m_pData );							}

	const SG_Char *				Get_Identifier			(void);
	const SG_Char *				Get_Name				(void);
	const SG_Char *				Get_Description			(void);
	CSG_String					Get_Description			(int Flags);
	CSG_String					Get_Description			(int Flags, const SG_Char *Separator);

	bool						Set_Enabled				(bool bEnabled);
	bool						is_Enabled				(void)	{	return( m_bEnabled );				}

	bool						ignore_Projection		(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_IGNORE_PROJECTION) );	}

	bool						is_Valid				(void)	{	return( m_pData->is_Valid() );		}
	bool						is_Input				(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_INPUT)	        );	}
	bool						is_Output				(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_OUTPUT)        );	}
	bool						is_Optional				(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_OPTIONAL)      );	}
	bool						is_Information			(void)	{	return( !!(m_pData->Get_Constraint() & PARAMETER_INFORMATION)   );	}
	bool						is_Option				(void);
	bool						is_DataObject			(void);
	bool						is_DataObject_List		(void);
	bool						is_Parameters			(void);
	bool						is_Serializable			(void);

	int							Get_Children_Count		(void)			{	return( m_nChildren );		}
	CSG_Parameter *				Get_Child				(int iChild)	{	return( iChild >= 0 && iChild < m_nChildren ? m_Children[iChild] : NULL );	}

	//-----------------------------------------------------
	bool						Set_Value				(int               Value);
	bool						Set_Value				(double            Value);
	bool						Set_Value				(const CSG_String &Value);
	bool						Set_Value				(void             *Value);
	bool						Set_Value				(CSG_Parameter    *Value);

	void						Set_Default				(int               Value)	{	m_pData->Set_Default(Value);	}
	void						Set_Default				(double            Value)	{	m_pData->Set_Default(Value);	}
	void						Set_Default				(const CSG_String &Value)	{	m_pData->Set_Default(Value);	}

	bool						Restore_Default			(void)	{	return( m_pData->Restore_Default() );	}

	bool						has_Changed				(int Check_Flags = PARAMETER_CHECK_ALL);

	bool						asBool					(void)	{	return( (bool             )!!m_pData->asInt  () );	}
	int							asInt					(void)	{	return( (int              )m_pData->asInt    () );	}
	long						asColor					(void)	{	return( (long             )m_pData->asInt    () );	}
	double						asDouble				(void)	{	return( (double           )m_pData->asDouble () );	}

	void *						asPointer				(void)	{	return( (void            *)m_pData->asPointer() );	}

	const SG_Char *				asString				(void)	{	return( (const SG_Char   *)m_pData->asString () );	}
	CSG_Colors *				asColors				(void)	{	return( (CSG_Colors      *)m_pData->asPointer() );	}
	CSG_Grid_System *			asGrid_System			(void)	{	return( (CSG_Grid_System *)m_pData->asPointer() );	}
	const SG_Char *				asFont					(void)	{	return( (const SG_Char   *)m_pData->asPointer() );	}

	CSG_Data_Object *			asDataObject			(void)	{	return( (CSG_Data_Object *)m_pData->asPointer() );	}
	CSG_Grid *					asGrid					(void)	{	return( (CSG_Grid        *)m_pData->asPointer() );	}
	CSG_Table *					asTable					(void)	{	return( (CSG_Table       *)m_pData->asPointer() );	}
	CSG_Shapes *				asShapes				(void)	{	return( (CSG_Shapes      *)m_pData->asPointer() );	}
	CSG_TIN *					asTIN					(void)	{	return( (CSG_TIN         *)m_pData->asPointer() );	}
	CSG_PointCloud *			asPointCloud			(void)	{	return( (CSG_PointCloud  *)m_pData->asPointer() );	}

	CSG_Parameters *			asParameters			(void)	{	return( (CSG_Parameters  *)m_pData->asPointer() );	}

	CSG_Parameter_Value *		asValue					(void)	{	return( (CSG_Parameter_Value           *)m_pData );	}
	CSG_Parameter_Choice *		asChoice				(void)	{	return( (CSG_Parameter_Choice          *)m_pData );	}
	CSG_Parameter_Range *		asRange					(void)	{	return( (CSG_Parameter_Range           *)m_pData );	}
	CSG_Parameter_File_Name *	asFilePath				(void)	{	return( (CSG_Parameter_File_Name       *)m_pData );	}

	CSG_Parameter_List *		asList					(void)	{	return( (CSG_Parameter_List            *)m_pData );	}
	CSG_Parameter_Grid_List *	asGridList				(void)	{	return( (CSG_Parameter_Grid_List       *)m_pData );	}
	CSG_Parameter_Table_List *	asTableList				(void)	{	return( (CSG_Parameter_Table_List      *)m_pData );	}
	CSG_Parameter_Shapes_List *	asShapesList			(void)	{	return( (CSG_Parameter_Shapes_List     *)m_pData );	}
	CSG_Parameter_TIN_List *	asTINList				(void)	{	return( (CSG_Parameter_TIN_List        *)m_pData );	}
	CSG_Parameter_PointCloud_List *	asPointCloudList	(void)	{	return( (CSG_Parameter_PointCloud_List *)m_pData );	}

	//-----------------------------------------------------
	bool						Assign					(CSG_Parameter *pSource);

	CSG_MetaData *				Serialize				(CSG_MetaData &MetaData, bool bSave);


private:

	CSG_Parameter(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, int Constraint);

	virtual ~CSG_Parameter(void);

	void						_Add_Child				(CSG_Parameter *pChild);


	bool						m_bEnabled;

	int							m_nChildren;

	CSG_Parameter				**m_Children;

	CSG_String					m_Identifier, m_Name, m_Description;

	CSG_Parameter_Data			*m_pData;

	CSG_Parameter				*m_pParent;

	CSG_Parameters				*m_pOwner;

};


///////////////////////////////////////////////////////////
//														 //
//					CSG_Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameters
{
	friend class CSG_Parameter;
	friend class CSG_Module;

public:
	CSG_Parameters(void);
	virtual ~CSG_Parameters(void);

								CSG_Parameters			(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier = NULL, bool bGrid_System = false);
	void						Create					(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier = NULL, bool bGrid_System = false);

	void						Destroy					(void);

	//-----------------------------------------------------
	void *						Get_Owner				(void)	const	{	return( m_pOwner );			}
	int							Get_Count				(void)	const	{	return( m_nParameters );	}

	void						Set_Identifier			(const CSG_String &String);
	const CSG_String &			Get_Identifier			(void)	const	{	return( m_Identifier );		}

	void						Set_Name				(const CSG_String &String);
	const CSG_String &			Get_Name				(void)	const	{	return( m_Name );			}

	void						Set_Description			(const CSG_String &String);
	const CSG_String &			Get_Description			(void)	const	{	return( m_Description );	}

	void						Set_Enabled				(bool bEnabled);

	//-----------------------------------------------------
	void						Set_Callback_On_Parameter_Changed	(TSG_PFNC_Parameter_Changed pCallback);
	void						Set_Callback			(bool bActive = true);

	//-----------------------------------------------------
	CSG_Parameter *				Get_Parameter			(int iParameter);
	CSG_Parameter *				Get_Parameter			(const CSG_String &Identifier);

	CSG_Parameter *				operator()				(int iParameter)				{	return( Get_Parameter(iParameter) );	}
	CSG_Parameter *				operator()				(const CSG_String &Identifier)	{	return( Get_Parameter(Identifier) );	}

	//-----------------------------------------------------
	bool						Del_Parameter			(int iParameter);
	bool						Del_Parameter			(const CSG_String &Identifier);

	bool						Del_Parameters			(void);

	//-----------------------------------------------------
	CSG_Parameter *				Add_Node				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);

	CSG_Parameter *				Add_Value				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Info_Value			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value = 0.0);

	CSG_Parameter *				Add_Range				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Range_Min = 0.0, double Range_Max = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Info_Range			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, double Range_Min = 0.0, double Range_Max = 0.0);

	CSG_Parameter *				Add_Choice				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items, int Default = 0);

	CSG_Parameter *				Add_String				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText = false, bool bPassword = false);
	CSG_Parameter *				Add_Info_String			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText = false);

	CSG_Parameter *				Add_FilePath			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const SG_Char *Filter = NULL, const SG_Char *Default = NULL, bool bSave = false, bool bDirectory = false, bool bMultiple = false);

	CSG_Parameter *				Add_Font				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, const SG_Char *pInit = NULL);
	CSG_Parameter *				Add_Colors				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, CSG_Colors    *pInit = NULL);
	CSG_Parameter *				Add_FixedTable			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, CSG_Table     *pInit = NULL);

	CSG_Parameter *				Add_Grid_System			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, CSG_Grid_System *pInit = NULL);
	CSG_Parameter *				Add_Grid				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true, TSG_Data_Type Preferred_Type = SG_DATATYPE_Undefined);
	CSG_Parameter *				Add_Grid_Output			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Grid_List			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true);

	CSG_Parameter *				Add_Table_Field			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bAllowNone = false);
	CSG_Parameter *				Add_Table				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint);
	CSG_Parameter *				Add_Table_Output		(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Table_List			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Parameter *				Add_Shapes				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Shape_Type = SHAPE_TYPE_Undefined);
	CSG_Parameter *				Add_Shapes_Output		(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Shapes_List			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type = SHAPE_TYPE_Undefined);

	CSG_Parameter *				Add_TIN					(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint);
	CSG_Parameter *				Add_TIN_Output			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_TIN_List			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Parameter *				Add_PointCloud			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint);
	CSG_Parameter *				Add_PointCloud_Output	(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_PointCloud_List		(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Parameter *				Add_Parameters			(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);

	//-----------------------------------------------------
	bool						Set_Parameter			(const CSG_String &Identifier, CSG_Parameter *pSource);
	bool						Set_Parameter			(const CSG_String &Identifier, int            Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &Identifier, double         Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &Identifier, void          *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &Identifier, const SG_Char *Value, int Type = PARAMETER_TYPE_Undefined);

	bool						Restore_Defaults		(void);

	int							Assign					(CSG_Parameters *pSource);
	int							Assign_Values			(CSG_Parameters *pSource);

	bool						Serialize				(const CSG_String &File_Name, bool bSave);
	bool						Serialize				(CSG_MetaData &Entry        , bool bSave);
	bool						Serialize_Compatibility	(CSG_File &Stream);

	//-----------------------------------------------------
	bool						DataObjects_Check		(bool bSilent = false);

	//-----------------------------------------------------
	bool						Get_String				(CSG_String &String, bool bOptionsOnly);
	bool						Msg_String				(bool bOptionsOnly);

	bool						Set_History				(CSG_MetaData &History, bool bOptions = true, bool bDataObjects = true);

	CSG_Grid_System *			Get_Grid_System			(void)	{	return( m_pGrid_System ? m_pGrid_System->asGrid_System() : NULL );	}

	bool						is_Managed				(void)	{	return( m_bManaged );	}


private:

	void						*m_pOwner;

	bool						m_bCallback, m_bManaged;

	CSG_String					m_Identifier, m_Name, m_Description;

	int							m_nParameters;

	CSG_Parameter				**m_Parameters, *m_pGrid_System;

	TSG_PFNC_Parameter_Changed	m_Callback;


	void						_On_Construction		(void);

	bool						_On_Parameter_Changed	(CSG_Parameter *pParameter, int Flags);

	CSG_Parameter *				_Add_Value				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bInformation, TSG_Parameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum);
	CSG_Parameter *				_Add_Range				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bInformation, double Range_Min, double Range_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum);
	CSG_Parameter *				_Add_String				(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, bool bInformation, const SG_Char *String, bool bLongText, bool bPassword);

	CSG_Parameter *				_Add					(CSG_Parameter *pParent, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, int Constraint);
	CSG_Parameter *				_Add					(CSG_Parameter *pSource);

	bool						DataObjects_Create			(void);
	bool						DataObjects_Synchronize		(void);
	bool						DataObjects_Get_Projection	(CSG_Projection &Projection)		const;
	bool						DataObjects_Set_Projection	(const CSG_Projection &Projection);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__parameters_H
