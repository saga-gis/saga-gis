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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#define SG_META_EXT_GRID		SG_T("mgrd" )
#define SG_META_EXT_GRIDS		SG_T("mgrds")
#define SG_META_EXT_TABLE		SG_T("mtab" )
#define SG_META_EXT_SHAPES		SG_T("mshp" )
#define SG_META_EXT_TIN			SG_T("mtin" )
#define SG_META_EXT_POINTCLOUD	SG_T("mpts" )

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
  * @see CSG_Grids
*/
//---------------------------------------------------------
typedef enum ESG_Data_Object_Type
{
	DATAOBJECT_TYPE_Grid,
	DATAOBJECT_TYPE_Grids,
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
SAGA_API_DLL_EXPORT CSG_String	SG_Get_DataObject_Identifier	(TSG_Data_Object_Type Type);
SAGA_API_DLL_EXPORT CSG_String	SG_Get_DataObject_Name			(TSG_Data_Object_Type Type);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT void		SG_Set_History_Depth			(int Depth);
SAGA_API_DLL_EXPORT int			SG_Get_History_Depth			(void);
SAGA_API_DLL_EXPORT void		SG_Set_History_Ignore_Lists		(int Ignore);
SAGA_API_DLL_EXPORT int			SG_Get_History_Ignore_Lists		(void);


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
  * @see CSG_Grids
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Data_Object
{
public:
	CSG_Data_Object(void);
	virtual ~CSG_Data_Object(void);

	virtual bool					Destroy			(void);

	/// Returns the object type as defined by TSG_Data_Object_Type. Used for run time type checking.
	virtual TSG_Data_Object_Type	Get_ObjectType	(void)	const	= 0;

	virtual bool					is_Valid		(void)	const	= 0;

	virtual bool					Save			(const CSG_String &File_Name, int Format = 0)	= 0;

	/// If there is an associated file data can be reloaded with this command.
	bool							Reload			(void);

	/// Deletes all files associated with this data object if possible. Works only with native SAGA files. Returns true on success.
	bool							Delete			(void);

	void							Set_File_Name	(const CSG_String &File_Name);
	const SG_Char *					Get_File_Name	(bool bNative = true)	const;
	int								Get_File_Type	(void)					const;

	bool							is_File_Native	(void)	const			{	return( m_File_bNative );	}

	virtual bool					Assign			(CSG_Data_Object *pObject);

	void							Set_Name		(const CSG_String &Name);
	const SG_Char *					Get_Name		(void)	const			{	return( m_Name );			}

	void							Set_Description	(const CSG_String &Description);
	const SG_Char *					Get_Description	(void)	const			{	return( m_Description );	}

	virtual void					Set_Modified	(bool bOn = true)		{	m_bModified	= bOn;			}
	bool							is_Modified		(void)	const			{	return( m_bModified );		}
	bool							Update			(void);


	CSG_MetaData &					Get_MetaData	(void)	const			{	return( *m_pMetaData );		}
	CSG_MetaData &					Get_MetaData_DB	(void)					{	return( *m_pMetaData_DB );	}
	CSG_MetaData &					Get_History		(void)					{	return( *m_pHistory );		}
	const CSG_MetaData &			Get_History		(void)	const			{	return( *m_pHistory );		}

	CSG_Projection &				Get_Projection	(void)					{	return( m_Projection );		}
	const CSG_Projection &			Get_Projection	(void)	const			{	return( m_Projection );		}

	class CSG_Table *				asTable			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Table      ? (class CSG_Table      *)this : NULL );	}
	class CSG_Shapes *				asShapes		(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Shapes     ? (class CSG_Shapes     *)this : NULL );	}
	class CSG_TIN *					asTIN			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_TIN        ? (class CSG_TIN        *)this : NULL );	}
	class CSG_PointCloud *			asPointCloud	(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_PointCloud ? (class CSG_PointCloud *)this : NULL );	}
	class CSG_Grid *				asGrid			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Grid       ? (class CSG_Grid       *)this : NULL );	}
	class CSG_Grids *				asGrids			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Grids      ? (class CSG_Grids      *)this : NULL );	}

	bool							Set_NoData_Value		(double Value);
	bool							Set_NoData_Value_Range	(double loValue, double hiValue);
	double							Get_NoData_Value		(void)	const	{	return( m_NoData_Value );	}
	double							Get_NoData_hiValue		(void)	const	{	return( m_NoData_hiValue );	}

	bool							is_NoData_Value			(double Value)	const
	{
		return( SG_is_NaN(Value) || (m_NoData_Value < m_NoData_hiValue ? m_NoData_Value <= Value && Value <= m_NoData_hiValue : Value == m_NoData_Value) );
	}

	bool							Save_History_to_Model	(const CSG_String &File)	const;


protected:

	void							Set_File_Name		(const CSG_String &File_Name, bool bNative);
	void							Set_File_Type		(int Type)			{	m_File_Type	= Type;			}

	virtual bool					On_Reload			(void)	= 0;
	virtual bool					On_Delete			(void)	= 0;

	bool							Load_MetaData		(const SG_Char *File_Name);
	bool							Save_MetaData		(const SG_Char *File_Name);

	void							Set_Update_Flag		(bool bOn = true)	{	m_bUpdate	= bOn;			}
	bool							Get_Update_Flag		(void)				{	return( m_bUpdate );		}
	virtual bool					On_Update			(void)				{	return( true );				}
	virtual bool					On_NoData_Changed	(void)				{	return( true );				}


private:

	bool							m_bModified, m_bUpdate, m_File_bNative;

	int								m_File_Type;

	double							m_NoData_Value, m_NoData_hiValue;

	CSG_String						m_File_Name, m_Name, m_Description;


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
