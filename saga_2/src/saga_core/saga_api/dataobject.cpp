
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
const SG_Char *	SG_Get_DataObject_Name(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	case DATAOBJECT_TYPE_Grid:		return( LNG("[DAT] Grid"  ) );
	case DATAOBJECT_TYPE_Table:		return( LNG("[DAT] Table" ) );
	case DATAOBJECT_TYPE_Shapes:	return( LNG("[DAT] Shapes") );
	case DATAOBJECT_TYPE_TIN:		return( LNG("[DAT] TIN"   ) );

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
CSG_Data_Object::CSG_Data_Object(void)
{
	m_File_Type		= 0;
	m_bModified		= true;

	Set_Name		(NULL);
	Set_File_Name	(NULL);
}

//---------------------------------------------------------
CSG_Data_Object::~CSG_Data_Object(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Data_Object::Destroy(void)
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
void CSG_Data_Object::Set_Name(const SG_Char *Name)
{
	m_Name.Printf(Name ? wxString( Name, wxConvUTF8 ).c_str() : LNG("[DAT] new") );
}

const SG_Char * CSG_Data_Object::Get_Name(void) const
{
	return( m_Name.c_str() );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Name(const SG_Char *File_Name)
{
	if( File_Name )
	{
		m_File_Name.Printf(File_Name);

		m_Name	= SG_File_Get_Name(File_Name, true);

		m_bModified	= false;
	}
	else
	{
		m_File_Name.Clear();

		Set_Name(NULL);
	}
}

const SG_Char * CSG_Data_Object::Get_File_Name(bool bNullAsString)	const
{
	return(	m_File_Name.Length() > 0
		?	m_File_Name.c_str()
		:	(bNullAsString ? (SG_Char*) LNG("[DAT] [not set]") : (SG_Char*) NULL)
	);
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Type(int File_Type)
{
	m_File_Type	= File_Type;
}

int CSG_Data_Object::Get_File_Type(void) const
{
	return( m_File_Type );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Assign(CSG_Data_Object *pObject)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
