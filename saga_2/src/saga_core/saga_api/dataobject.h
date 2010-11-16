
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
//                     dataobject.h                      //
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
#ifndef HEADER_INCLUDED__SAGA_API__dataobject_H
#define HEADER_INCLUDED__SAGA_API__dataobject_H

//---------------------------------------------------------
/** \file dataobject.h
  * Base class for data objects and related declarations.
  * @see CSG_Data_Object
  * @see CSG_History
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mat_tools.h"
#include "metadata.h"


///////////////////////////////////////////////////////////
//														 //
//						Meta-Data						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_META_EXT_GRID		SG_T("mgrd")
#define SG_META_EXT_TABLE		SG_T("mtab")
#define SG_META_EXT_SHAPES		SG_T("mshp")
#define SG_META_EXT_TIN			SG_T("mtin")
#define SG_META_EXT_POINTCLOUD	SG_T("mpts")

//---------------------------------------------------------
#define SG_META_SRC				SG_T("SOURCE")
#define SG_META_SRC_FILE		SG_T("FILE")
#define SG_META_SRC_DB			SG_T("DATABASE")
#define SG_META_SRC_PROJ		SG_T("PROJECTION")

#define SG_META_HST				SG_T("HISTORY")
#define SG_META_HST_FILE		SG_T("FILE")


///////////////////////////////////////////////////////////
//														 //
//						Data Objects					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Definition of available data types. This is returned by
  * the CSG_Data_Object::Get_ObjectType() function to allow
  * type checking at run time.
  * @see CSG_Data_Object
  * @see CSG_Table
  * @see CSG_PointCloud
  * @see CSG_Shapes
  * @see CSG_TIN
  * @see CSG_Grid
*/
//---------------------------------------------------------
typedef enum ESG_Data_Object_Type
{
	DATAOBJECT_TYPE_Grid,
	DATAOBJECT_TYPE_Table,
	DATAOBJECT_TYPE_Shapes,
	DATAOBJECT_TYPE_TIN,
	DATAOBJECT_TYPE_PointCloud,
	DATAOBJECT_TYPE_Undefined
}
TSG_Data_Object_Type;

//---------------------------------------------------------
#define DATAOBJECT_NOTSET		((void *)NULL)
#define DATAOBJECT_CREATE		((void *)1)

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const SG_Char *	SG_Get_DataObject_Identifier	(TSG_Data_Object_Type Type);
SAGA_API_DLL_EXPORT const SG_Char *	SG_Get_DataObject_Name			(TSG_Data_Object_Type Type);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Data_Object is the base class for all types of data
  * sets, which are provided by the SAGA API.
  * @see CSG_Table
  * @see CSG_Shapes
  * @see CSG_TIN
  * @see CSG_PointCloud
  * @see CSG_Grid
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Data_Object
{
public:
	CSG_Data_Object(void);
	virtual ~CSG_Data_Object(void);

	virtual bool					Destroy			(void);

	/// Returns the object type as defined by TSG_Data_Object_Type. Used for run time type checking.
	virtual TSG_Data_Object_Type	Get_ObjectType	(void)	const									= 0;

	virtual bool					is_Valid		(void)	const									= 0;

	virtual bool					Save			(const CSG_String &File_Name, int Format = 0)	= 0;

	const SG_Char *					Get_File_Name	(bool bNullAsString = false)	const;
	int								Get_File_Type	(void)	const;

	virtual bool					Assign			(CSG_Data_Object *pObject);

	void							Set_Name		(const SG_Char *Name);
	const SG_Char *					Get_Name		(void)	const;

	virtual void					Set_Modified	(bool bOn = true)	{	m_bModified	= bOn;			}
	bool							is_Modified		(void)	const		{	return( m_bModified );		}
	bool							Update			(void);


	CSG_MetaData &					Get_MetaData	(void)	const		{	return( *m_pMetaData );		}
	CSG_MetaData &					Get_MetaData_DB	(void)				{	return( *m_pMetaData_DB );	}
	CSG_MetaData &					Get_History		(void)				{	return( *m_pHistory );		}

	CSG_Projection &				Get_Projection	(void)				{	return( m_Projection );		}

	class CSG_Table *				asTable			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Table      ? (class CSG_Table      *)this : NULL );	}
	class CSG_Shapes *				asShapes		(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Shapes     ? (class CSG_Shapes     *)this : NULL );	}
	class CSG_TIN *					asTIN			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_TIN        ? (class CSG_TIN        *)this : NULL );	}
	class CSG_PointCloud *			asPointCloud	(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_PointCloud ? (class CSG_PointCloud *)this : NULL );	}
	class CSG_Grid *				asGrid			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Grid       ? (class CSG_Grid       *)this : NULL );	}

	bool							Set_NoData_Value		(double Value);
	bool							Set_NoData_Value_Range	(double loValue, double hiValue);
	double							Get_NoData_Value		(void)	const	{	return( m_NoData_Value );	}
	double							Get_NoData_hiValue		(void)	const	{	return( m_NoData_hiValue );	}

	bool							is_NoData_Value			(double Value)	const
	{
		return( m_NoData_Value < m_NoData_hiValue ? m_NoData_Value <= Value && Value <= m_NoData_hiValue : Value == m_NoData_Value );
	}


protected:

	void							Set_File_Name		(const SG_Char *File_Name);
	void							Set_File_Type		(int File_Type);

	bool							Load_MetaData		(const SG_Char *File_Name);
	bool							Save_MetaData		(const SG_Char *File_Name);

	void							Set_Update_Flag		(bool bOn = true)	{	m_bUpdate	= bOn;		}
	bool							Get_Update_Flag		(void)				{	return( m_bUpdate );	}
	virtual bool					On_Update			(void)				{	return( true );			}
	virtual bool					On_NoData_Changed	(void)				{	return( true );			}


private:

	bool							m_bModified, m_bUpdate;

	int								m_File_Type;

	double							m_NoData_Value, m_NoData_hiValue;

	CSG_String						m_File_Name, m_Name;

	CSG_MetaData					m_MetaData, *m_pMetaData, *m_pHistory, *m_pFile, *m_pProjection, *m_pMetaData_DB;

	CSG_Projection					m_Projection;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__dataobject_H
