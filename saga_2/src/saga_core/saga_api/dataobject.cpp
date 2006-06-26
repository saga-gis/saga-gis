
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
//                    dataobject.cpp                     //
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
//					class CData_Object					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dataobject.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char *	API_Get_DataObject_Name(TDataObject_Type Type)
{
	switch( Type )
	{
	case DATAOBJECT_TYPE_Grid:		return( LNG("[DAT] Grid") );
	case DATAOBJECT_TYPE_Table:		return( LNG("[DAT] Table") );
	case DATAOBJECT_TYPE_Shapes:	return( LNG("[DAT] Shapes") );
	case DATAOBJECT_TYPE_TIN:		return( LNG("[DAT] T.I.N.") );

	default:
	case DATAOBJECT_TYPE_Undefined:	return( LNG("[DAT] Undefined") );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDataObject::CDataObject(void)
{
	m_File_Type		= 0;
	m_bModified		= true;

	Set_Name		(NULL);
	Set_File_Path	(NULL);
}

//---------------------------------------------------------
CDataObject::~CDataObject(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CDataObject::Destroy(void)
{
	m_History.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDataObject::Set_Name(const char *Name)
{
	m_Name.Printf(Name ? Name : LNG("[DAT] new"));
}

const char * CDataObject::Get_Name(void)
{
	return( m_Name.c_str() );
}

//---------------------------------------------------------
void CDataObject::Set_File_Path(const char *File_Path)
{
	if( File_Path )
	{
		m_File_Path.Printf(File_Path);

		m_Name	= API_Extract_File_Name(File_Path, true);

		m_bModified	= false;
	}
	else
	{
		m_File_Path.Clear();

		Set_Name(NULL);
	}
}

const char * CDataObject::Get_File_Path(bool bNullAsString)
{
	return(	m_File_Path.Length() > 0
		?	m_File_Path.c_str()
		:	(bNullAsString ? LNG("[DAT] [not set]") : NULL)
	);
}

//---------------------------------------------------------
void CDataObject::Set_File_Type(int File_Type)
{
	m_File_Type	= File_Type;
}

int CDataObject::Get_File_Type(void)
{
	return( m_File_Type );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDataObject::Assign(CDataObject *pObject)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
