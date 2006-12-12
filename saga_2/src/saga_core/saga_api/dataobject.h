
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Definition of available data types. This is returned by
  * the CSG_Data_Object::Get_ObjectType() function to allow
  * type checking at run time.
  * @see CSG_Data_Object
  * @see CSG_Table
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
	DATAOBJECT_TYPE_Undefined
}
TSG_Data_Object_Type;

//---------------------------------------------------------
#define HISTORY_EXT_GRID		SG_T("hgrd")
#define HISTORY_EXT_TABLE		SG_T("htab")
#define HISTORY_EXT_SHAPES		SG_T("hshp")
#define HISTORY_EXT_TIN			SG_T("htin")

//---------------------------------------------------------
#define DATAOBJECT_NOTSET		((void *)NULL)
#define DATAOBJECT_CREATE		((void *)1)

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const SG_Char *	SG_Get_DataObject_Name	(TSG_Data_Object_Type Type);


///////////////////////////////////////////////////////////
//														 //
//						History							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_History_Entry represents a setting, which has been
  * used to calculate a data set.
  * @see CSG_History
  * @see CSG_Data_Object
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_History_Entry
{
	friend class CSG_History;

public:

	/// Returns the date, at which the entry has been created.
	const SG_Char *				Get_Date		(void)	{	return( m_Date );		}

	/// Returns the name of the entry.
	const SG_Char *				Get_Name		(void)	{	return( m_Name );		}

	/// Returns the content of the entry.
	const SG_Char *				Get_Entry		(void)	{	return( m_Entry );		}

	/// Returns the entry's subhistory, if it has one, or NULL.
	class CSG_History *			Get_History		(void)	{	return( m_pHistory );	}


private:

	CSG_History_Entry(const SG_Char *Date, const SG_Char *Name, const SG_Char *Entry, class CSG_History *pHistory);
	CSG_History_Entry(const CSG_History_Entry &Entry);
	virtual ~CSG_History_Entry(void);

	CSG_String					m_Date, m_Name, m_Entry;

	class CSG_History			*m_pHistory;

};

//---------------------------------------------------------
/**
  * CSG_History manages a list of CSG_History_Entry objects.
  * It is used by the CSG_Data_Object class. Entries to the
  * history object of a data set are automatically added by
  * the CSG_Module class, when it is executed. Therefore it
  * it enables to see, how a data set has been calculated.
  * @see CSG_History_Entry
  * @see CSG_Data_Object
  * @see CSG_Module
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_History
{
public:
	CSG_History(void);
	CSG_History(const CSG_History &History);
	virtual ~CSG_History(void);

	void						Destroy			(void);
	void						Assign			(const CSG_History &History, bool bAdd = false);

	int							Get_Count		(void)			{	return( m_nEntries );	}
	CSG_History_Entry &			Get_Entry		(int iEntry)	{	return( *m_pEntries[iEntry] );	}
	void						Add_Entry		(const SG_Char *Name, const SG_Char *Entry, CSG_History *pHistory = NULL);

	bool						Load			(const SG_Char *File_Name, const SG_Char *File_Extension);
	bool						Save			(const SG_Char *File_Name, const SG_Char *File_Extension);

	CSG_String					Get_HTML		(void);


private:

	int							m_nEntries;

	CSG_History_Entry			**m_pEntries;


	void						_Add_Entry		(CSG_History_Entry *pEntry);

	bool						_Load			(CSG_File &Stream);
	bool						_Save			(CSG_File &Stream);

};


///////////////////////////////////////////////////////////
//														 //
//						Data Object						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Data_Object is the base class for all types of data
  * sets, which are provided by the SAGA API.
  * @see CSG_Table
  * @see CSG_Shapes
  * @see CSG_TIN
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
	virtual TSG_Data_Object_Type	Get_ObjectType	(void)	const							= 0;

	virtual bool					is_Valid		(void)	const							= 0;

	virtual bool					Save			(const SG_Char *File_Name, int Format = 0)	= 0;

	const SG_Char *					Get_File_Name	(bool bNullAsString = false)	const;
	int								Get_File_Type	(void)	const;

	virtual bool					Assign			(CSG_Data_Object *pObject);

	void							Set_Name		(const SG_Char *Name);
	const SG_Char *					Get_Name		(void)	const;

	virtual void					Set_Modified	(bool bModified = true)	{	m_bModified	= bModified;	}
	bool							is_Modified		(void)	const			{	return( m_bModified );		}

	CSG_History &					Get_History		(void)					{	return( m_History );		}

	class CSG_Table *				asTable			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Table  ? (class CSG_Table  *)this : NULL );	}
	class CSG_Shapes *				asShapes		(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Shapes ? (class CSG_Shapes *)this : NULL );	}
	class CSG_TIN *					asTIN			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_TIN    ? (class CSG_TIN    *)this : NULL );	}
	class CSG_Grid *				asGrid			(void)	{	return( Get_ObjectType() == DATAOBJECT_TYPE_Grid   ? (class CSG_Grid   *)this : NULL );	}


protected:

	void							Set_File_Name	(const SG_Char *File_Name);
	void							Set_File_Type	(int File_Type);

	CSG_History						m_History;


private:

	bool							m_bModified;

	int								m_File_Type;

	CSG_String						m_File_Name, m_Name;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__dataobject_H
