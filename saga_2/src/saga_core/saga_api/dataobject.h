
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
#include "api_core.h"
#include "geo_tools.h"
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
SAGA_API_DLL_EXPORT const char *	SG_Get_DataObject_Name	(TDataObject_Type Type);

//---------------------------------------------------------
#define HISTORY_EXT_GRID		"hgrd"
#define HISTORY_EXT_TABLE		"htab"
#define HISTORY_EXT_SHAPES		"hshp"
#define HISTORY_EXT_TIN			"htin"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DATAOBJECT_NOTSET		((void *)NULL)
#define DATAOBJECT_CREATE		((void *)1)


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
