
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
typedef enum EDataObject_Type
{
	DATAOBJECT_TYPE_Grid,
	DATAOBJECT_TYPE_Table,
	DATAOBJECT_TYPE_Shapes,
	DATAOBJECT_TYPE_TIN,
	DATAOBJECT_TYPE_Undefined
}
TDataObject_Type;

//---------------------------------------------------------
#define HISTORY_EXT_GRID		"hgrd"
#define HISTORY_EXT_TABLE		"htab"
#define HISTORY_EXT_SHAPES		"hshp"
#define HISTORY_EXT_TIN			"htin"

//---------------------------------------------------------
#define DATAOBJECT_NOTSET		((void *)NULL)
#define DATAOBJECT_CREATE		((void *)1)

//---------------------------------------------------------
SAGA_API_DLL_EXPORT const char *	SG_Get_DataObject_Name	(TDataObject_Type Type);


///////////////////////////////////////////////////////////
//														 //
//						History							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_History_Entry
{
	friend class CSG_History;

public:

	const char *				Get_Date		(void)	{	return( m_Date );		}
	const char *				Get_Name		(void)	{	return( m_Name );		}
	const char *				Get_Entry		(void)	{	return( m_Entry );		}
	class CSG_History *			Get_History		(void)	{	return( m_pHistory );	}


private:

	CSG_History_Entry(const char *Date, const char *Name, const char *Entry, class CSG_History *pHistory);
	CSG_History_Entry(const CSG_History_Entry &Entry);
	virtual ~CSG_History_Entry(void);

	CSG_String					m_Date, m_Name, m_Entry;

	class CSG_History			*m_pHistory;

};

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
	void						Add_Entry		(const char *Name, const char *Entry, CSG_History *pHistory = NULL);

	bool						Load			(const char *File_Name, const char *File_Extension);
	bool						Save			(const char *File_Name, const char *File_Extension);

	CSG_String					Get_HTML		(void);


private:

	int							m_nEntries;

	CSG_History_Entry			**m_pEntries;


	void						_Add_Entry		(CSG_History_Entry *pEntry);

	bool						_Load			(FILE *Stream);
	bool						_Save			(FILE *Stream);

};


///////////////////////////////////////////////////////////
//														 //
//						Data Object						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CDataObject
{
public:
	CDataObject(void);
	virtual ~CDataObject(void);

	virtual bool				Destroy			(void);

	virtual TDataObject_Type	Get_ObjectType	(void)	const							= 0;

	virtual bool				is_Valid		(void)	const							= 0;

	virtual bool				Save			(const char *File_Path, int Format = 0)	= 0;

	virtual bool				Assign			(CDataObject *pObject);

	void						Set_Name		(const char *Name);
	const char *				Get_Name		(void)	const;

	const char *				Get_File_Path	(bool bNullAsString = false)	const;
	int							Get_File_Type	(void)	const;

	virtual void				Set_Modified	(bool bFlag = true)	{	m_bModified	= bFlag;	}
	bool						is_Modified		(void)	const		{	return( m_bModified );	}

	CSG_History &				Get_History		(void)				{	return( m_History );	}


protected:

	void						Set_File_Path	(const char *File_Path);
	void						Set_File_Type	(int File_Type);


private:

	bool						m_bModified;

	int							m_File_Type;

	CSG_String					m_Name, m_File_Path;

	CSG_History					m_History;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__dataobject_H
