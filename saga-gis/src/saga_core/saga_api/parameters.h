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
#include "grids.h"
#include "table.h"
#include "shapes.h"
#include "tin.h"
#include "pointcloud.h"
#include "datetime.h"


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
#define PARAMETER_NOT_FOR_GUI				0x20
#define PARAMETER_NOT_FOR_CMD				0x40

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
	PARAMETER_TYPE_Date,
	PARAMETER_TYPE_Range,
	PARAMETER_TYPE_Choice,
	PARAMETER_TYPE_Choices,
	PARAMETER_TYPE_String,
	PARAMETER_TYPE_Text,
	PARAMETER_TYPE_FilePath,

	PARAMETER_TYPE_Font,
	PARAMETER_TYPE_Color,
	PARAMETER_TYPE_Colors,
	PARAMETER_TYPE_FixedTable,

	PARAMETER_TYPE_Grid_System,
	PARAMETER_TYPE_Table_Field,
	PARAMETER_TYPE_Table_Fields,

	PARAMETER_TYPE_PointCloud,
	PARAMETER_TYPE_Grid,
	PARAMETER_TYPE_Grids,
	PARAMETER_TYPE_Table,
	PARAMETER_TYPE_Shapes,
	PARAMETER_TYPE_TIN,

	PARAMETER_TYPE_Grid_List,
	PARAMETER_TYPE_Grids_List,
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
#define PARAMETER_CHECK_VALUES			0x01
#define PARAMETER_CHECK_ENABLE			0x02
#define PARAMETER_CHECK_ALL				(PARAMETER_CHECK_VALUES | PARAMETER_CHECK_ENABLE)

//---------------------------------------------------------
#define SG_PARAMETER_DATA_SET_FALSE		0
#define SG_PARAMETER_DATA_SET_TRUE		1
#define SG_PARAMETER_DATA_SET_CHANGED	2


///////////////////////////////////////////////////////////
//														 //
//					CSG_Parameter						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter
{
public:	///////////////////////////////////////////////////

	CSG_Parameters *				Get_Owner				(void)	const;
	CSG_Parameter *					Get_Parent				(void)	const;
	class CSG_Data_Manager *		Get_Manager				(void)	const;

	virtual TSG_Parameter_Type		Get_Type				(void)	const	= 0;
	CSG_String						Get_Type_Identifier		(void)	const;
	CSG_String						Get_Type_Name			(void)	const;

	const SG_Char *					Get_Identifier			(void)	const;
	bool							Cmp_Identifier			(const CSG_String &Identifier)	const;
	bool							Set_Name				(const CSG_String &Name);
	const SG_Char *					Get_Name				(void)	const;
	const SG_Char *					Get_Description			(void)	const;
	bool							Set_Description			(const CSG_String &Description);
	CSG_String						Get_Description			(int Flags)								const;
	CSG_String						Get_Description			(int Flags, const SG_Char *Separator)	const;

	bool							Set_Enabled				(bool bEnabled = true);
	bool							is_Enabled				(void)	const;

	virtual bool					is_Valid				(void)	const	{	return( true );	}
	bool							is_Input				(void)	const	{	return( !!(m_Constraint & PARAMETER_INPUT      ) );	}
	bool							is_Output				(void)	const	{	return( !!(m_Constraint & PARAMETER_OUTPUT     ) );	}
	bool							is_Optional				(void)	const	{	return( !!(m_Constraint & PARAMETER_OPTIONAL   ) );	}
	bool							is_Information			(void)	const	{	return( !!(m_Constraint & PARAMETER_INFORMATION) );	}
	bool							is_Option				(void)	const;
	bool							is_DataObject			(void)	const;
	bool							is_DataObject_List		(void)	const;
	bool							is_Parameters			(void)	const;
	bool							is_Serializable			(void)	const;
	bool							is_Compatible			(CSG_Parameter *pParameter)	const;
	bool							is_Value_Equal			(CSG_Parameter *pParameter)	const;

	void							Set_UseInGUI			(bool bDoUse = false);
	void							Set_UseInCMD			(bool bDoUse = false);

	bool							do_UseInGUI				(void)	const	{	return( !(m_Constraint & PARAMETER_NOT_FOR_GUI)   );	}
	bool							do_UseInCMD				(void)	const	{	return( !(m_Constraint & PARAMETER_NOT_FOR_CMD)   );	}

	void							ignore_Projection		(bool bIgnore);
	bool							ignore_Projection		(void)	const	{	return( !!(m_Constraint & PARAMETER_IGNORE_PROJECTION) );	}

	TSG_Data_Object_Type			Get_DataObject_Type		(void)	const;

	int								Get_Children_Count		(void)			const	{	return( m_nChildren );		}
	CSG_Parameter *					Get_Child				(int iChild)	const	{	return( iChild >= 0 && iChild < m_nChildren ? m_Children[iChild] : NULL );	}
	bool							Set_Children_Enabled	(bool bEnabled = true);

	//-----------------------------------------------------
	virtual bool					Set_Value				(int               Value);
	virtual bool					Set_Value				(double            Value);
	virtual bool					Set_Value				(const CSG_String &Value);
	virtual bool					Set_Value				(const char       *Value);
	virtual bool					Set_Value				(const wchar_t    *Value);
	virtual bool					Set_Value				(void             *Value);
	virtual bool					Set_Value				(CSG_Parameter    *Value);

	bool							Set_Default				(int               Value);
	bool							Set_Default				(double            Value);
	bool							Set_Default				(const CSG_String &Value);
	const CSG_String &				Get_Default				(void)	const;
	virtual bool					Restore_Default			(void);

	bool							Check					(bool bSilent = true);

	bool							has_Changed				(int Check_Flags = PARAMETER_CHECK_ALL);

	bool							asBool					(void)	const	{	return( (bool         )!!_asInt    () );	}
	int								asInt					(void)	const	{	return( (int            )_asInt    () );	}
	long							asColor					(void)	const	{	return( (long           )_asInt    () );	}
	double							asDouble				(void)	const	{	return( (double         )_asDouble () );	}
	const SG_Char *					asString				(void)	const	{	return( (const SG_Char *)_asString () );	}
	void *							asPointer				(void)	const	{	return( (void          *)_asPointer() );	}

	CSG_Colors                    *	asColors				(void)	const;
	const SG_Char                 *	asFont					(void)	const;
	CSG_Grid_System               *	asGrid_System			(void)	const;

	CSG_Data_Object               *	asDataObject			(void)	const;
	CSG_Grid                      *	asGrid					(void)	const;
	CSG_Grids                     *	asGrids					(void)	const;
	CSG_Table                     *	asTable					(void)	const;
	CSG_Shapes                    *	asShapes				(void)	const;
	CSG_TIN                       *	asTIN					(void)	const;
	CSG_PointCloud                *	asPointCloud			(void)	const;

	//-----------------------------------------------------
	class CSG_Parameter_Value           *	asValue			(void)	const;
	class CSG_Parameter_Date            *	asDate			(void)	const;
	class CSG_Parameter_Choice          *	asChoice		(void)	const;
	class CSG_Parameter_Choices         *	asChoices		(void)	const;
	class CSG_Parameter_Range           *	asRange			(void)	const;
	class CSG_Parameter_File_Name       *	asFilePath		(void)	const;
	class CSG_Parameter_Table_Fields    *	asTableFields	(void)	const;
	class CSG_Parameters                *	asParameters	(void)	const;

	class CSG_Parameter_List            *	asList			(void)	const;
	class CSG_Parameter_Grid_List       *	asGridList		(void)	const;
	class CSG_Parameter_Grids_List      *	asGridsList		(void)	const;
	class CSG_Parameter_Table_List      *	asTableList		(void)	const;
	class CSG_Parameter_Shapes_List     *	asShapesList	(void)	const;
	class CSG_Parameter_TIN_List        *	asTINList		(void)	const;
	class CSG_Parameter_PointCloud_List *	asPointCloudList(void)	const;

	//-----------------------------------------------------
	bool							Assign					(CSG_Parameter *pSource);
	bool							Serialize				(CSG_MetaData &MetaData, bool bSave);


protected: ////////////////////////////////////////////////

	friend class CSG_Parameters;

	CSG_Parameter(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	virtual ~CSG_Parameter(void);

	CSG_String						m_String;


	virtual int						_Set_Value				(int               Value);
	virtual int						_Set_Value				(double            Value);
	virtual int						_Set_Value				(const CSG_String &Value);
	virtual int						_Set_Value				(void             *Value);

	virtual void					_Set_String				(void);

	virtual int						_asInt					(void)	const;
	virtual double					_asDouble				(void)	const;
	virtual void *					_asPointer				(void)	const;
	virtual const SG_Char *			_asString				(void)	const;

	virtual bool					_Assign					(CSG_Parameter *pSource);
	virtual bool					_Serialize				(CSG_MetaData &MetaData, bool bSave);


private: //////////////////////////////////////////////////

	void							_Add_Child				(CSG_Parameter *pChild);


	bool							m_bEnabled;

	int								m_Constraint, m_nChildren;

	CSG_Parameter					**m_Children;

	CSG_String						m_Identifier, m_Name, m_Description, m_Default;

	CSG_Parameter					*m_pParent;

	CSG_Parameters					*m_pOwner;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Node : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Node );	}


protected:

	CSG_Parameter_Node(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Bool : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Bool );	}


protected:

	CSG_Parameter_Bool(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	bool						m_Value;


	virtual int					_Set_Value				(int               Value);
	virtual int					_Set_Value				(double            Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const	{	return( m_Value );		}

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Value : public CSG_Parameter
{
public:

	bool						Set_Valid_Range			(double Minimum, double Maximum);

	void						Set_Minimum				(double Minimum, bool bOn = true);
	double						Get_Minimum				(void)	const	{	return( m_Minimum );	}
	bool						has_Minimum				(void)	const	{	return( m_bMinimum );	}

	void						Set_Maximum				(double Maximum, bool bOn = true);
	double						Get_Maximum				(void)	const	{	return( m_Maximum );	}
	bool						has_Maximum				(void)	const	{	return( m_bMaximum );	}


protected:

	CSG_Parameter_Value(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	bool						m_bMinimum, m_bMaximum;

	double						m_Minimum, m_Maximum;


	virtual bool				_Assign					(CSG_Parameter *pSource);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Int : public CSG_Parameter_Value
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Int );		}


protected:

	CSG_Parameter_Int(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	int							m_Value;


	virtual int					_Set_Value				(int               Value);
	virtual int					_Set_Value				(double            Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const	{	return( m_Value );		}
	virtual double				_asDouble				(void)	const	{	return( m_Value );		}

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Double : public CSG_Parameter_Value
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Double );	}


protected:

	CSG_Parameter_Double(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	double						m_Value;


	virtual int					_Set_Value				(int               Value);
	virtual int					_Set_Value				(double            Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const	{	return( (int)m_Value );	}
	virtual double				_asDouble				(void)	const	{	return(      m_Value );	}

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Degree : public CSG_Parameter_Double
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Degree );	}


protected:

	CSG_Parameter_Degree(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Date : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Date );	}

	void						Set_Date				(const CSG_DateTime &Date);
	const CSG_DateTime &		Get_Date				(void)	const	{	return( m_Date );	}


protected:

	CSG_Parameter_Date(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_DateTime				m_Date;


	virtual int					_Set_Value				(int               Value);
	virtual int					_Set_Value				(double            Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const;
	virtual double				_asDouble				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Range : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Range );		}

	bool						Set_Range				(double Min, double Max);
	double						Get_Range				(void)	const	{	return( Get_Max() - Get_Min() );	}

	bool						Set_Min					(double Min);
	double						Get_Min					(void)	const;
	CSG_Parameter_Double *		Get_Min_Parameter		(void)	const	{	return( m_pMin );	}

	bool						Set_Max					(double Max);
	double						Get_Max					(void)	const;
	CSG_Parameter_Double *		Get_Max_Parameter		(void)	const	{	return( m_pMax );	}

	virtual bool				Restore_Default			(void);


protected:

	CSG_Parameter_Range(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	virtual ~CSG_Parameter_Range(void);

	CSG_Parameters				*m_pRange;

	CSG_Parameter_Double		*m_pMin, *m_pMax;


	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Choice : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Choice );	}

	void						Set_Items				(const SG_Char *String);

	const SG_Char *				Get_Item				(int Index)	const;
	CSG_String					Get_Item_Data			(int Index)	const;

	bool						Get_Data				(int        &Value)	const;
	bool						Get_Data				(double     &Value)	const;
	bool						Get_Data				(CSG_String &Value)	const;

	int							Get_Count				(void)	const	{	return( m_Items.Get_Count() );	}


protected:

	CSG_Parameter_Choice(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	int							m_Value;

	CSG_Strings					m_Items;


	virtual int					_Set_Value				(int               Value);
	virtual int					_Set_Value				(double            Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const	{	return( m_Value );		}
	virtual double				_asDouble				(void)	const	{	return( m_Value );		}

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Choices : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Choices );	}

	void						Set_Items				(const CSG_String  &Items);
	void						Set_Items				(const CSG_Strings &Items);
	void						Del_Items				(void);
	void						Add_Item				(const CSG_String &Item, const CSG_String &Data = "");

	int							Get_Item_Count			(void)	const	{	return( m_Items[0].Get_Count() );	}
	const CSG_String &			Get_Item				(int i)	const	{	return( m_Items[0][i] );	}
	const CSG_String &			Get_Item_Data			(int i)	const	{	return( m_Items[1][i] );	}

	int							Get_Selection_Count		(void)	const	{	return( (int)m_Selection.Get_Size() );	}
	const CSG_String &			Get_Selection			(int i)	const	{	return( Get_Item     (Get_Selection_Index(i)) );	}
	const CSG_String &			Get_Selection_Data		(int i)	const	{	return( Get_Item_Data(Get_Selection_Index(i)) );	}
	int							Get_Selection_Index		(int i)	const	{	return( i >= 0 && i < Get_Selection_Count() ? m_Selection[i] : -1 );	}

	bool						is_Selected				(int Index);
	bool						Select					(int Index, bool bSelect = true);
	bool						Clr_Selection			(void);


protected:

	CSG_Parameter_Choices(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Strings					m_Items[2];
	
	CSG_Array_Int				m_Selection;


	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_String : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const		{	return( PARAMETER_TYPE_String );	}

	virtual bool				is_Valid				(void)	const;

	void						Set_Password			(bool bOn);
	bool						is_Password				(void)	const;


protected:

	CSG_Parameter_String(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	bool						m_bPassword;


	virtual int					_Set_Value				(const CSG_String &Value);

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Text : public CSG_Parameter_String
{
public:
	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Text );		}


protected:

	CSG_Parameter_Text(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_File_Name : public CSG_Parameter_String
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_FilePath );	}

	void						Set_Filter				(const SG_Char *Filter);
	const SG_Char *				Get_Filter				(void)	const;

	void						Set_Flag_Save			(bool bFlag);
	bool						is_Save					(void)	const	{	return( m_bSave );		}

	void						Set_Flag_Multiple		(bool bFlag);
	bool						is_Multiple				(void)	const	{	return( m_bMultiple );	}

	void						Set_Flag_Directory		(bool bFlag);
	bool						is_Directory			(void)	const	{	return( m_bDirectory );	}

	bool						Get_FilePaths			(CSG_Strings &FilePaths)	const;


protected:

	CSG_Parameter_File_Name(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	bool						m_bSave, m_bMultiple, m_bDirectory;

	CSG_String					m_Filter;


	virtual bool				_Assign					(CSG_Parameter *pSource);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Font : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Font );		}

	virtual bool				Restore_Default			(void);


protected:

	CSG_Parameter_Font(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	int							m_Color;

	CSG_String					m_Font;


	virtual int					_Set_Value				(int   Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual int					_asInt					(void)	const;
	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Color : public CSG_Parameter_Int
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Color );		}


protected:

	CSG_Parameter_Color(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Colors : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Colors );	}


protected:

	CSG_Parameter_Colors(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Colors					m_Colors;


	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const;
	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Fixed_Table : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_FixedTable );	}


protected:

	CSG_Parameter_Fixed_Table(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Table					m_Table;


	virtual void				_Set_String				(void);

	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grid_System : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Grid_System );	}


protected:

	CSG_Parameter_Grid_System(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Grid_System				m_System;


	virtual int					_Set_Value				(void *Value);

	virtual void				_Set_String				(void);

	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table_Field : public CSG_Parameter_Int
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Table_Field );	}

	bool						Add_Default				(double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum);

	CSG_Table *					Get_Table				(void)	const;


protected:

	CSG_Parameter_Table_Field(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	int							m_Default;


	virtual int					_Set_Value				(int               Value);
	virtual int					_Set_Value				(const CSG_String &Value);

	virtual void				_Set_String				(void);

	virtual double				_asDouble				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table_Fields : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Table_Fields );	}

	int							Get_Count				(void)	const	{	return( m_nFields );	}
	int							Get_Index				(int i)	const	{	return( i >= 0 && i < m_nFields ? m_Fields[i] : -1 );	}

	CSG_Table *					Get_Table				(void)	const;


protected:

	CSG_Parameter_Table_Fields(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	virtual ~CSG_Parameter_Table_Fields(void);

	int							m_nFields, *m_Fields;


	virtual int					_Set_Value				(const CSG_String &Value);

	virtual int					_asInt					(void)	const;
	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Data_Object : public CSG_Parameter
{
public:

	virtual bool				is_Valid				(void)	const;


protected:

	CSG_Parameter_Data_Object(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Data_Object				*m_pDataObject;


	virtual int					_Set_Value				(void *Value);

	virtual void				_Set_String				(void);

	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Data_Object_Output : public CSG_Parameter_Data_Object
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_DataObject_Output );	}

	bool						Set_DataObject_Type		(TSG_Data_Object_Type Type);
	TSG_Data_Object_Type		Get_DataObject_Type		(void)	const	{	return( m_Type );	}


protected:

	CSG_Parameter_Data_Object_Output(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	TSG_Data_Object_Type		m_Type;


	virtual int					_Set_Value				(void *Value);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grid : public CSG_Parameter_Data_Object
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Grid );	}

	void						Set_Preferred_Type		(TSG_Data_Type Type);
	TSG_Data_Type				Get_Preferred_Type		(void)	const	{	return( m_Type );	}

	CSG_Grid_System *			Get_System				(void)	const;

	bool						Add_Default				(double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum);


protected:

	CSG_Parameter_Grid(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	int							m_Default;

	TSG_Data_Type				m_Type;


	virtual int					_Set_Value				(void  *Value);

	virtual int					_asInt					(void)	const;
	virtual double				_asDouble				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grids : public CSG_Parameter_Grid
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Grids );	}


protected:

	CSG_Parameter_Grids(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table : public CSG_Parameter_Data_Object
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Table );		}

	CSG_Parameter_Table(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


protected:

	virtual int					_Set_Value				(void *Value);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Shapes : public CSG_Parameter_Data_Object
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_Shapes );	}

	void						Set_Shape_Type			(TSG_Shape_Type Type);
	TSG_Shape_Type				Get_Shape_Type			(void)	const	{	return( m_Type );	}


protected:

	CSG_Parameter_Shapes(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	TSG_Shape_Type				m_Type;


	virtual int					_Set_Value				(void *Value);

	virtual bool				_Assign					(CSG_Parameter *pSource);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_TIN : public CSG_Parameter_Data_Object
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_TIN );		}


protected:

	CSG_Parameter_TIN(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	virtual int					_Set_Value				(void *Value);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_PointCloud : public CSG_Parameter_Data_Object
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)	const	{	return( PARAMETER_TYPE_PointCloud );	}


protected:

	CSG_Parameter_PointCloud(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	virtual int					_Set_Value				(void *Value);

	virtual bool				_Assign					(CSG_Parameter *pSource);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_List : public CSG_Parameter
{
public:

	virtual bool				Add_Item				(CSG_Data_Object *pItem);
	virtual bool				Del_Item				(CSG_Data_Object *pItem, bool bUpdateData = true);
	virtual bool				Del_Item				(int Index             , bool bUpdateData = true);
	virtual bool				Del_Items				(void);

	int							Get_Item_Count			(void)		const	{	return( (int)m_Objects.Get_Size() );	}
	CSG_Data_Object *			Get_Item				(int Index)	const	{	return( Index >= 0 && Index < Get_Item_Count() ? (CSG_Data_Object *)m_Objects[Index] : NULL );	}

	virtual bool				Update_Data				(void)	{	return( true );	}

	virtual int					Get_Data_Count			(void)		const	{	return( Get_Item_Count() );	}
	virtual CSG_Data_Object *	Get_Data				(int Index)	const	{	return( Get_Item(Index) );	}


protected:

	CSG_Parameter_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	virtual void				_Set_String				(void);

	virtual int					_asInt					(void)	const;
	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


private:

	CSG_Array_Pointer			m_Objects;


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grid_List : public CSG_Parameter_List
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_Grid_List );		}

	CSG_Grid_System *			Get_System				(void)		const;

	virtual bool				Add_Item				(CSG_Data_Object *pItem);
	virtual bool				Del_Item				(CSG_Data_Object *pItem, bool bUpdateData = true);
	virtual bool				Del_Item				(int Index             , bool bUpdateData = true);
	virtual bool				Del_Items				(void);

	virtual bool				Update_Data				(void);

	virtual int					Get_Data_Count			(void)		const	{	return( Get_Grid_Count() );	}
	virtual CSG_Data_Object *	Get_Data				(int Index)	const	{	return( (CSG_Data_Object *)Get_Grid(Index) );	}

	int							Get_Grid_Count			(void)		const	{	return( (int)m_Grids.Get_Size() );	}
	CSG_Grid *					Get_Grid				(int Index)	const	{	return( Index >= 0 && Index < Get_Data_Count() ? (CSG_Grid *)m_Grids[Index] : NULL );	}


protected:

	CSG_Parameter_Grid_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Array_Pointer			m_Grids;


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Grids_List : public CSG_Parameter_List
{
public:
	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_Grids_List );		}

	CSG_Grid_System *			Get_System				(void)		const;

	virtual bool				Add_Item				(CSG_Data_Object *pItem);

	CSG_Grids *					Get_Grids				(int Index)	const	{	return( (CSG_Grids *)Get_Item(Index) );	}


protected:

	CSG_Parameter_Grids_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Table_List : public CSG_Parameter_List
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_Table_List );		}

	CSG_Table *					Get_Table				(int Index)	const	{	return( (CSG_Table *)Get_Item(Index) );	}


protected:

	CSG_Parameter_Table_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Shapes_List : public CSG_Parameter_List
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_Shapes_List );		}

	void						Set_Shape_Type			(TSG_Shape_Type Type);
	TSG_Shape_Type				Get_Shape_Type			(void)		const	{	return( m_Type );}

	CSG_Shapes *				Get_Shapes				(int Index)	const	{	return( (CSG_Shapes *)Get_Item(Index) );	}


protected:

	CSG_Parameter_Shapes_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	TSG_Shape_Type				m_Type;


	virtual bool				_Assign					(CSG_Parameter *pSource);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_TIN_List : public CSG_Parameter_List
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_TIN_List );		}

	CSG_TIN *					asTIN					(int Index)	const	{	return( (CSG_TIN *)Get_Item(Index) );	}


protected:

	CSG_Parameter_TIN_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_PointCloud_List : public CSG_Parameter_List
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_PointCloud_List );		}

	CSG_PointCloud *			Get_PointCloud			(int Index)	const	{	return( (CSG_PointCloud *)Get_Item(Index) );	}


protected:

	CSG_Parameter_PointCloud_List(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);


	friend class CSG_Parameters;
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameter_Parameters : public CSG_Parameter
{
public:

	virtual TSG_Parameter_Type	Get_Type				(void)		const	{	return( PARAMETER_TYPE_Parameters );	}

	virtual bool				Restore_Default			(void);


protected:

	CSG_Parameter_Parameters(CSG_Parameters *pOwner, CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	virtual ~CSG_Parameter_Parameters(void);

	CSG_Parameters				*m_pParameters;


	virtual void				_Set_String				(void);

	virtual void *				_asPointer				(void)	const;

	virtual bool				_Assign					(CSG_Parameter *pSource);
	virtual bool				_Serialize				(CSG_MetaData &Entry, bool bSave);


	friend class CSG_Parameters;
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

	bool						Create					(CSG_Parameters *pParameters, bool bAddDefaultGrid       , CSG_Parameter   *pParent       , const CSG_String &Prefix = "");
	bool						Create					(CSG_Parameters *pParameters, bool bAddDefaultGrid = true, const CSG_String &ParentID = "", const CSG_String &Prefix = "");

	bool						Add_Grid				(const CSG_String &ID, const CSG_String &Name, bool bOptional, bool bGrids = false);

	bool						On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	bool						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool						Set_User_Defined		(CSG_Parameters *pParameters, const TSG_Rect &Extent, int Rows = 0, int Rounding = 2);
	bool						Set_User_Defined		(CSG_Parameters *pParameters, CSG_Shapes *pPoints, int Scale = 4  , int Rounding = 2);
	bool						Set_User_Defined		(CSG_Parameters *pParameters, double xMin, double yMin, double Size, int nx, int ny);
	bool						Set_User_Defined		(CSG_Parameters *pParameters, const CSG_Grid_System &System);

	CSG_Grid_System				Get_System				(void);

	CSG_Grid *					Get_Grid				(const CSG_String &ID, TSG_Data_Type Type = SG_DATATYPE_Float);
	CSG_Grid *					Get_Grid				(                      TSG_Data_Type Type = SG_DATATYPE_Float);

	CSG_Grids *					Get_Grids				(const CSG_String &ID, TSG_Data_Type Type = SG_DATATYPE_Float);
	CSG_Grids *					Get_Grids				(                      TSG_Data_Type Type = SG_DATATYPE_Float);


private:

	bool						m_bFitToCells;

	CSG_String					m_Prefix;

	CSG_Parameters				*m_pParameters;

};


///////////////////////////////////////////////////////////
//														 //
//					CSG_Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef int		(* TSG_PFNC_Parameter_Changed)	(CSG_Parameter *pParameter, int Flags);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Parameters
{
	friend class CSG_Parameter;
	friend class CSG_Tool;

public:
	CSG_Parameters(void);
	virtual ~CSG_Parameters(void);

								CSG_Parameters			(const CSG_Parameters &Parameters);
	bool						Create					(const CSG_Parameters &Parameters);

								CSG_Parameters			(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier = NULL, bool bGrid_System = false);
	bool						Create					(void *pOwner, const SG_Char *Name, const SG_Char *Description, const SG_Char *Identifier = NULL, bool bGrid_System = false);

	void						Destroy					(void);

	//-----------------------------------------------------
	void *						Get_Owner				(void)	const	{	return( m_pOwner );			}

	class CSG_Data_Manager *	Get_Manager				(void)	const	{	return( m_pManager );		}
	void						Set_Manager				(class CSG_Data_Manager *pManager);

	int							Get_Count				(void)	const	{	return( m_nParameters );	}

	void						Set_Identifier			(const CSG_String &Identifier);
	bool						Cmp_Identifier			(const CSG_String &Identifier)	const;
	const CSG_String &			Get_Identifier			(void)	const	{	return( m_Identifier );		}

	void						Set_Name				(const CSG_String &Name);
	const CSG_String &			Get_Name				(void)	const	{	return( m_Name );			}

	void						Set_Description			(const CSG_String &Description);
	const CSG_String &			Get_Description			(void)	const	{	return( m_Description );	}

	void						Add_Reference			(const CSG_String &Authors, const CSG_String &Year, const CSG_String &Title, const CSG_String &Where, const SG_Char *Link = NULL, const SG_Char *Link_Text = NULL);
	void						Add_Reference			(const CSG_String &Link, const SG_Char *Link_Text = NULL);
	const CSG_Strings &			Get_References			(void)	const	{	return( m_References );		}

	void						Set_Enabled				(bool bEnabled = true);
	void						Set_Enabled				(const CSG_String &Identifier, bool bEnabled = true);

	//-----------------------------------------------------
	TSG_PFNC_Parameter_Changed	Set_Callback_On_Parameter_Changed	(TSG_PFNC_Parameter_Changed pCallback);
	bool						Set_Callback			(bool bActive = true);

	//-----------------------------------------------------
	CSG_Parameters &			operator =				(const CSG_Parameters &Parameters)	{	Create(Parameters);	return( *this );	}

	CSG_Parameter *				Get_Parameter			(int i               )	const	{	return( i >= 0 && i < m_nParameters ? m_Parameters[i] : NULL );	}
	CSG_Parameter *				Get_Parameter			(const CSG_String &ID)	const;

	CSG_Parameter *				operator()				(int i               )	const	{	return(  Get_Parameter(i ) );	}
	CSG_Parameter *				operator()				(const CSG_String &ID)	const	{	return(  Get_Parameter(ID) );	}
	CSG_Parameter *				operator()				(const char       *ID)	const	{	return(  Get_Parameter(ID) );	}
	CSG_Parameter *				operator()				(const wchar_t    *ID)	const	{	return(  Get_Parameter(ID) );	}

	CSG_Parameter &				operator[]				(int i               )	const	{	return( *Get_Parameter(i ) );	}
	CSG_Parameter &				operator[]				(const CSG_String &ID)	const	{	return( *Get_Parameter(ID) );	}
	CSG_Parameter &				operator[]				(const char       *ID)	const	{	return( *Get_Parameter(ID) );	}
	CSG_Parameter &				operator[]				(const wchar_t    *ID)	const	{	return( *Get_Parameter(ID) );	}

	//-----------------------------------------------------
	bool						Del_Parameter			(int i);
	bool						Del_Parameter			(const CSG_String &ID);

	bool						Del_Parameters			(void);

	//-----------------------------------------------------
	CSG_Parameter *				Add_Parameter			(CSG_Parameter *pParameter);

	CSG_Parameter *				Add_Node				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);

	CSG_Parameter *				Add_Value				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Info_Value			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value = 0.0);

	CSG_Parameter *				Add_Bool				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool   Value = false);
	CSG_Parameter *				Add_Int					(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int    Value = 0  , int    Minimum = 0  , bool bMinimum = false, int    Maximum = 0  , bool bMaximum = false);
	CSG_Parameter *				Add_Double				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Degree				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Date				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0);	// Julian Day Number
	CSG_Parameter *				Add_Color				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int    Value = 0);

	CSG_Parameter *				Add_Range				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Range_Min = 0.0, double Range_Max = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Info_Range			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Range_Min = 0.0, double Range_Max = 0.0);

	CSG_Parameter *				Add_Choice				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items, int Default = 0);
	CSG_Parameter *				Add_Choices				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items);

	CSG_Parameter *				Add_String				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText = false, bool bPassword = false);
	CSG_Parameter *				Add_Info_String			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText = false);

	CSG_Parameter *				Add_FilePath			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const SG_Char *Filter = NULL, const SG_Char *Default = NULL, bool bSave = false, bool bDirectory = false, bool bMultiple = false);

	CSG_Parameter *				Add_Font				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const SG_Char *pInit = NULL);
	CSG_Parameter *				Add_Colors				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Colors    *pInit = NULL);
	CSG_Parameter *				Add_FixedTable			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Table     *pInit = NULL);

	CSG_Parameter *				Add_Grid_System			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Grid_System *pInit = NULL);
	CSG_Parameter *				Add_Grid				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true, TSG_Data_Type Preferred_Type = SG_DATATYPE_Undefined);
	CSG_Parameter *				Add_Grid_or_Const		(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false, bool bSystem_Dependent = true);
	CSG_Parameter *				Add_Grid_Output			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Grid_List			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true);

	CSG_Parameter *				Add_Grids				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true, TSG_Data_Type Preferred_Type = SG_DATATYPE_Undefined);
	CSG_Parameter *				Add_Grids_Output		(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Grids_List			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true);

	CSG_Parameter *				Add_Table_Field			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bAllowNone = false);
	CSG_Parameter *				Add_Table_Field_or_Const(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false);
	CSG_Parameter *				Add_Table_Fields		(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Table				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	CSG_Parameter *				Add_Table_Output		(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Table_List			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Parameter *				Add_Shapes				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Shape_Type = SHAPE_TYPE_Undefined);
	CSG_Parameter *				Add_Shapes_Output		(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_Shapes_List			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type = SHAPE_TYPE_Undefined);

	CSG_Parameter *				Add_TIN					(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	CSG_Parameter *				Add_TIN_Output			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_TIN_List			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Parameter *				Add_PointCloud			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);
	CSG_Parameter *				Add_PointCloud_Output	(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);
	CSG_Parameter *				Add_PointCloud_List		(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint);

	CSG_Parameter *				Add_Parameters			(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description);

	//-----------------------------------------------------
	bool						Set_Parameter			(const CSG_String &ID, CSG_Parameter   *pValue);
	bool						Set_Parameter			(const char       *ID, CSG_Parameter   *pValue);
	bool						Set_Parameter			(const wchar_t    *ID, CSG_Parameter   *pValue);
	bool						Set_Parameter			(const CSG_String &ID, void             *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, void             *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, void             *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &ID, CSG_Data_Object  *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, CSG_Data_Object  *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, CSG_Data_Object  *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &ID, int               Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, int               Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, int               Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &ID, double            Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, double            Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, double            Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &ID, const CSG_String &Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, const CSG_String &Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, const CSG_String &Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &ID, const char       *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, const char       *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, const char       *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const CSG_String &ID, const wchar_t    *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const char       *ID, const wchar_t    *Value, int Type = PARAMETER_TYPE_Undefined);
	bool						Set_Parameter			(const wchar_t    *ID, const wchar_t    *Value, int Type = PARAMETER_TYPE_Undefined);

	bool						Restore_Defaults		(bool bClearData = false);

	bool						Assign					(CSG_Parameters *pSource);
	bool						Assign_Values			(CSG_Parameters *pSource);
	bool						Assign_Parameters		(CSG_Parameters *pSource);

	bool						Serialize				(const CSG_String &File_Name, bool bSave);
	bool						Serialize				(CSG_MetaData &Entry        , bool bSave);
	bool						Serialize_Compatibility	(CSG_File &Stream);

	//-----------------------------------------------------
	bool						DataObjects_Check		(bool bSilent = false);

	//-----------------------------------------------------
	bool						Get_String				(CSG_String &String, bool bOptionsOnly);
	bool						Msg_String				(bool bOptionsOnly);

	bool						Set_History				(CSG_MetaData &History, bool bOptions = true, bool bDataObjects = true);

	CSG_Grid_System *			Get_Grid_System			(void)	const	{	return( m_pGrid_System ? m_pGrid_System->asGrid_System() : NULL );	}

	bool						is_Managed				(void)	const	{	return( m_pManager != NULL );	}


private:

	void						*m_pOwner;

	class CSG_Data_Manager		*m_pManager;

	bool						m_bCallback;

	CSG_String					m_Identifier, m_Name, m_Description;

	CSG_Strings					m_References;

	int							m_nParameters;

	CSG_Parameter				**m_Parameters, *m_pGrid_System;

	TSG_PFNC_Parameter_Changed	m_Callback;


	void						_On_Construction		(void);

	bool						_On_Parameter_Changed	(CSG_Parameter *pParameter, int Flags);

	CSG_Parameter *				_Add_Value				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bInformation, TSG_Parameter_Type Type, double Value, double Minimum, bool bMinimum, double Maximum, bool bMaximum);
	CSG_Parameter *				_Add_Range				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bInformation, double Range_Min, double Range_Max, double Minimum, bool bMinimum, double Maximum, bool bMaximum);
	CSG_Parameter *				_Add_String				(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bInformation, const SG_Char *String, bool bLongText, bool bPassword);

	CSG_Parameter *				_Add					(const CSG_String &ParentID, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, int Constraint);
	CSG_Parameter *				_Add					(CSG_Parameter *pSource);

	bool						DataObjects_Create			(void);
	bool						DataObjects_Synchronize		(void);
	bool						DataObjects_Get_Projection	(CSG_Projection &Projection)		const;
	bool						DataObjects_Set_Projection	(const CSG_Projection &Projection);


public:	// DEPRECATED BEGIN

	//	old style calls (parent as csg_parameter pointer instead of identifier)

	CSG_Parameter *	Add_Node				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_Node                (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter *	Add_Value				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false)
	{	return(     Add_Value               (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Type, Value, Minimum, bMinimum, Maximum, bMaximum) ); }
	CSG_Parameter *	Add_Info_Value			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, TSG_Parameter_Type Type, double Value = 0.0)
	{	return(     Add_Info_Value          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Type, Value) ); }
	CSG_Parameter *	Add_Bool				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool   Value = false)
	{	return(     Add_Bool                (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value) );	}
	CSG_Parameter *	Add_Int					(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int    Value = 0  , int    Minimum = 0  , bool bMinimum = false, int    Maximum = 0  , bool bMaximum = false)
	{	return(     Add_Int                 (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value, Minimum, bMinimum, Maximum, bMaximum) );	}
	CSG_Parameter *	Add_Double				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false)
	{	return(     Add_Double              (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value, Minimum, bMinimum, Maximum, bMaximum) );	}
	CSG_Parameter *	Add_Degree				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false)
	{	return(     Add_Degree              (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value, Minimum, bMinimum, Maximum, bMaximum) );	}
	CSG_Parameter *	Add_Date				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0)
	{	return(     Add_Date                (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value) );	}
	CSG_Parameter *	Add_Color				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int    Value = 0)
	{	return(     Add_Color               (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value) );	}
	CSG_Parameter *	Add_Range				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Range_Min = 0.0, double Range_Max = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false)
	{	return(     Add_Range               (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Range_Min, Range_Max, Minimum, bMinimum, Maximum, bMaximum) );	}
	CSG_Parameter *	Add_Info_Range			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Range_Min = 0.0, double Range_Max = 0.0)
	{	return(     Add_Info_Range          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Range_Min, Range_Max) );	}
	CSG_Parameter *	Add_Choice				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &Items, int Default = 0)
	{	return(     Add_Choice              (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Items, Default) );	}
	CSG_Parameter *	Add_String				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText = false, bool bPassword = false)
	{	return(     Add_String              (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, String, bLongText, bPassword) );	}
	CSG_Parameter *	Add_Info_String			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const CSG_String &String, bool bLongText = false)
	{	return(     Add_Info_String         (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, String, bLongText) );	}
	CSG_Parameter *	Add_FilePath			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const SG_Char *Filter = NULL, const SG_Char *Default = NULL, bool bSave = false, bool bDirectory = false, bool bMultiple = false)
	{	return(     Add_FilePath            (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Filter, Default, bSave, bDirectory, bMultiple) );	}
	CSG_Parameter *	Add_Font				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, const SG_Char *pInit = NULL)
	{	return(     Add_Font                (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, pInit) );	}
	CSG_Parameter *	Add_Colors				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Colors    *pInit = NULL)
	{	return(     Add_Colors              (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, pInit) );	}
	CSG_Parameter *	Add_FixedTable			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Table     *pInit = NULL)
	{	return(     Add_FixedTable          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, pInit) );	}
	CSG_Parameter *	Add_Grid_System			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, CSG_Grid_System *pInit = NULL)
	{	return(     Add_Grid_System         (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, pInit) );	}
	CSG_Parameter *	Add_Grid				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true, TSG_Data_Type Preferred_Type = SG_DATATYPE_Undefined)
	{	return(     Add_Grid                (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint, bSystem_Dependent, Preferred_Type) );	}
	CSG_Parameter *	Add_Grid_or_Const		(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false, bool bSystem_Dependent = true)
	{	return(     Add_Grid_or_Const       (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value, Minimum, bMinimum, Maximum, bMaximum, bSystem_Dependent) );	}
	CSG_Parameter *	Add_Grid_Output			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_Grid_Output         (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter *	Add_Grid_List			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, bool bSystem_Dependent = true)
	{	return(     Add_Grid_List           (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint, bSystem_Dependent) );	}
	CSG_Parameter *	Add_Table_Field			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, bool bAllowNone = false)
	{	return(     Add_Table_Field         (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, bAllowNone) );	}
	CSG_Parameter *	Add_Table_Field_or_Const(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, double Value = 0.0, double Minimum = 0.0, bool bMinimum = false, double Maximum = 0.0, bool bMaximum = false)
	{	return(     Add_Table_Field_or_Const(pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Value, Minimum, bMinimum, Maximum, bMaximum) );	}
	CSG_Parameter *	Add_Table_Fields		(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_Table_Fields        (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter *	Add_Table				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	{	return(     Add_Table               (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint) );	}
	CSG_Parameter *	Add_Table_Output		(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_Table_Output        (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter *	Add_Table_List			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	{	return(     Add_Table_List          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint) );	}
	CSG_Parameter * Add_Shapes				(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Shape_Type = SHAPE_TYPE_Undefined)
	{	return(     Add_Shapes              (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint, Shape_Type) );	}
	CSG_Parameter * Add_Shapes_Output		(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_Shapes_Output       (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter * Add_Shapes_List			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint, TSG_Shape_Type Shape_Type = SHAPE_TYPE_Undefined)
	{	return(     Add_Shapes_List         (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint, Shape_Type) );	}
	CSG_Parameter * Add_TIN					(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	{	return(     Add_TIN                 (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint) );	}
	CSG_Parameter * Add_TIN_Output			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_TIN_Output          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter * Add_TIN_List			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	{	return(     Add_TIN_List            (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint) );	}
	CSG_Parameter * Add_PointCloud			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	{	return(     Add_PointCloud          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint) );	}
	CSG_Parameter * Add_PointCloud_Output	(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_PointCloud_Output   (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}
	CSG_Parameter * Add_PointCloud_List		(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description, int Constraint)
	{	return(     Add_PointCloud_List     (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description, Constraint) );	}
	CSG_Parameter * Add_Parameters			(CSG_Parameter *pParent, const CSG_String &ID, const CSG_String &Name, const CSG_String &Description)
	{	return(     Add_Parameters          (pParent ? pParent->Get_Identifier() : SG_T(""), ID, Name, Description) );	}

	// DEPRECATED END

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__parameters_H
