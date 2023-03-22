
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
//                     pointcloud.h                      //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__pointcloud_H
#define HEADER_INCLUDED__SAGA_API__pointcloud_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** \file pointcloud.h
* The point cloud data container, a specialized CSG_Shapes class.
* @see CSG_PointCloud
* @see CSG_Shapes
* @see CSG_Table
* @see CSG_Data_Object
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	POINTCLOUD_FILE_FORMAT_Undefined = 0,
	POINTCLOUD_FILE_FORMAT_Normal,
	POINTCLOUD_FILE_FORMAT_Compressed
}
TSG_PointCloud_File_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_PointCloud : public CSG_Shapes
{
public:

									CSG_PointCloud		(void);
	bool							Create				(void);

									CSG_PointCloud		(const CSG_PointCloud &PointCloud);
	bool							Create				(const CSG_PointCloud &PointCloud);

									CSG_PointCloud		(const CSG_String &File);
	bool							Create				(const CSG_String &File);
									CSG_PointCloud		(const char       *File);
	bool							Create				(const char       *File);
									CSG_PointCloud		(const wchar_t    *File);
	bool							Create				(const wchar_t    *File);

									CSG_PointCloud		(CSG_PointCloud *pTemplate);
	bool							Create				(CSG_PointCloud *pTemplate);

	virtual ~CSG_PointCloud(void);

	virtual bool					Destroy				(void);

	virtual TSG_Data_Object_Type	Get_ObjectType		(void)	const			{	return( SG_DATAOBJECT_TYPE_PointCloud );	}

	virtual bool					Assign				(CSG_Data_Object *pSource);

	virtual bool					Save				(const CSG_String &File, int Format = 0);
	virtual bool					Save				(const char       *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}
	virtual bool					Save				(const wchar_t    *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}

	void							Set_XYZ_Precision	(bool bDouble)			{	m_bXYZPrecDbl	= bDouble;	}

	static bool						Get_Header_Content	(const CSG_String &FileName, CSG_MetaData &Header);

	//-----------------------------------------------------
	virtual bool					is_Valid			(void)	const			{	return( m_nFields > 0 );	}
	bool							is_Compatible		(CSG_PointCloud *pPointCloud)	const;

	//-----------------------------------------------------
	virtual bool					Add_Field			(const CSG_String &Name, TSG_Data_Type Type, int Field = -1);
	virtual bool					Del_Field			(int Field);
	virtual bool					Mov_Field			(int Field, int Position);

	virtual bool					Set_Field_Type		(int Field, TSG_Data_Type Type);

	//-----------------------------------------------------
	int								Get_Attribute_Count	(void)			const	{	return( m_nFields - 3 );	}
	const SG_Char *					Get_Attribute_Name	(int Field)		const	{	Field += 3; return( Field >= 3 && Field < m_nFields ? m_Field_Name[Field]->c_str() : NULL );			}
	TSG_Data_Type					Get_Attribute_Type	(int Field)		const	{	Field += 3; return( Field >= 3 && Field < m_nFields ? m_Field_Type[Field] : SG_DATATYPE_Undefined );	}

	//-----------------------------------------------------
	bool							Add_Point			(double x, double y, double z);
	bool							Del_Point			(sLong Index);
	bool							Del_Points			(void);

	//-----------------------------------------------------
	bool							Set_Cursor			(sLong Index)							{	return( (m_Cursor = Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL) != NULL );	}
	virtual bool					Set_Value			(             int Field, double Value)	{	return( _Set_Field_Value(m_Cursor, Field, Value) );	}
	virtual double					Get_Value			(             int Field)	const		{	return( _Get_Field_Value(m_Cursor, Field) );			}
	double							Get_X				(void)						const		{	return( _Get_Field_Value(m_Cursor, 0) );				}
	double							Get_Y				(void)						const		{	return( _Get_Field_Value(m_Cursor, 1) );				}
	double							Get_Z				(void)						const		{	return( _Get_Field_Value(m_Cursor, 2) );				}
	bool							Set_Attribute		(             int Field, double Value)	{	return( Set_Value(Field + 3, Value) );					}
	double							Get_Attribute		(             int Field)	const		{	return( Get_Value(Field + 3) );						}
	bool							Set_NoData			(             int Field)				{	return( Set_Value(Field, Get_NoData_Value()) );	}
	bool							is_NoData			(             int Field)	const		{	return( is_NoData_Value(Get_Value(Field)) );		}

	virtual bool					Set_Value			(sLong Index, int Field, double Value)	{	return( _Set_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, Field, Value) );	}
	virtual double					Get_Value			(sLong Index, int Field)	const		{	return( _Get_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, Field) );		}
	double							Get_X				(sLong Index)				const		{	return( _Get_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, 0) );				}
	double							Get_Y				(sLong Index)				const		{	return( _Get_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, 1) );				}
	double							Get_Z				(sLong Index)				const		{	return( _Get_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, 2) );				}
	bool							Set_Attribute		(sLong Index, int Field, double Value)	{	return( Set_Value(Index, Field + 3, Value) );				}
	double							Get_Attribute		(sLong Index, int Field)	const		{	return( Get_Value(Index, Field + 3) );					}
	bool							Set_NoData			(sLong Index, int Field)				{	return( Set_Value(Index, Field, Get_NoData_Value()) );}
	bool							is_NoData			(sLong Index, int Field)	const		{	return( is_NoData_Value(Get_Value(Index, Field)) );	}

	virtual bool					Set_Value			(             int Field, const SG_Char *Value)			{	return( _Set_Field_Value(m_Cursor, Field, Value) );	}
	virtual bool					Get_Value			(             int Field, CSG_String    &Value)	const	{	return( _Get_Field_Value(m_Cursor, Field, Value) );	}
	virtual bool					Set_Value			(sLong Index, int Field, const SG_Char *Value)			{	return( _Set_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, Field, Value) );	}
	virtual bool					Get_Value			(sLong Index, int Field, CSG_String    &Value)	const	{	return( _Get_Field_Value(Index >= 0 && Index < m_nRecords ? m_Points[Index] : NULL, Field, Value) );	}
	virtual bool					Set_Attribute		(             int Field, const SG_Char *Value)			{	return( Set_Value(Field + 3, Value) );			}
	virtual bool					Get_Attribute		(             int Field, CSG_String    &Value)	const	{	return( Get_Value(Field + 3, Value) );			}
	virtual bool					Set_Attribute		(sLong Index, int Field, const SG_Char *Value)			{	return( Set_Value(Index, Field + 3, Value) );	}
	virtual bool					Get_Attribute		(sLong Index, int Field, CSG_String    &Value)	const	{	return( Get_Value(Index, Field + 3, Value) );	}

	TSG_Point_3D					Get_Point			(void)			const;
	TSG_Point_3D					Get_Point			(sLong Index)	const;
	virtual bool					Set_Point			(             const TSG_Point_3D &Point);
	virtual bool					Set_Point			(sLong Index, const TSG_Point_3D &Point);

	virtual void					Set_Modified		(bool bModified = true)		{	CSG_Data_Object::Set_Modified(bModified);	}


	//-----------------------------------------------------
	// Overrides: CSG_Table, CSG_Shapes

	virtual CSG_Table_Record *		Get_Record			(sLong Index)	const;

	virtual CSG_Shape *				Get_Shape			(const CSG_Point &Point, double Epsilon = 0.);

	virtual bool					Del_Record			(sLong Index)	{	return( Del_Point(Index) );	}
	virtual bool					Del_Shape			(sLong Index)	{	return( Del_Point(Index) );	}
	virtual bool					Del_Records			(void)			{	return( Del_Points()     );	}
	virtual bool					Del_Shapes			(void)			{	return( Del_Points()     );	}

	virtual CSG_Table_Record *		Ins_Record			(sLong Index, CSG_Table_Record *pCopy = NULL);
	virtual CSG_Table_Record *		Add_Record			(             CSG_Table_Record *pCopy = NULL);
	virtual CSG_Shape *				Add_Shape			(             CSG_Table_Record *pCopy = NULL, TSG_ADD_Shape_Copy_Mode mCopy = SHAPE_COPY);
	virtual bool					Del_Shape			(CSG_Shape *pShape)	{	return( false );	}

	virtual bool					Select				(sLong Index                    , bool bInvert = false);
	virtual bool					Select				(CSG_Table_Record *pShape = NULL, bool bInvert = false);
	virtual bool					Select				(const TSG_Rect &Extent         , bool bInvert = false);
	virtual bool					Select				(const TSG_Point &Point         , bool bInvert = false);

	virtual bool					is_Selected			(sLong Index)	const;

	virtual sLong					Del_Selection		(void);
	virtual sLong					Inv_Selection		(void);
	virtual CSG_Shape *				Get_Selection		(sLong Index = 0);
	virtual const CSG_Rect &		Get_Selection_Extent(void);


protected:

	virtual bool					On_Update			(void);
	virtual bool					On_Reload			(void);
	virtual bool					On_Delete			(void);

	virtual void					_On_Construction	(void);

	virtual bool					_Stats_Update		(int Field)	const;


private:

	bool							m_bXYZPrecDbl;

	char							**m_Points, *m_Cursor;

	int								m_nPointBytes, *m_Field_Offset;
	
	sLong							m_Shapes_Index;

	CSG_Array						m_Array_Points;

	CSG_Shapes						m_Shapes;


	bool							_Load				(const CSG_String &File);
	bool							_Load				(CSG_File &Stream);
	bool							_Save				(CSG_File &Stream);
	CSG_MetaData					_Create_Header		(void)	const;

	bool							_Add_Field			(const SG_Char *Name, TSG_Data_Type Type, int Field = -1);
	bool							_Set_Field_Value	(char *pPoint, int Field, double         Value);
	double							_Get_Field_Value	(char *pPoint, int Field                      )	const;
	bool							_Set_Field_Value	(char *pPoint, int Field, const SG_Char *Value);
	bool							_Get_Field_Value	(char *pPoint, int Field, CSG_String    &Value)	const;

	bool							_Inc_Array			(void);
	bool							_Dec_Array			(void);

	CSG_Shape *						_Set_Shape			(sLong Index);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(void);

/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(const CSG_PointCloud &PointCloud);

/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(const char       *File);
/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(const wchar_t    *File);
/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(const CSG_String &File);

/** Safe Point Cloud construction */
SAGA_API_DLL_EXPORT CSG_PointCloud *	SG_Create_PointCloud	(CSG_PointCloud *pStructure);

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__pointcloud_H
