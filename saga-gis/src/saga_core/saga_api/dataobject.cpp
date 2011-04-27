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
const SG_Char *	SG_Get_DataObject_Identifier(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	default:
	case DATAOBJECT_TYPE_Undefined:		return( SG_T("UNDEFINED") );
	case DATAOBJECT_TYPE_Grid:			return( SG_T("GRID") );
	case DATAOBJECT_TYPE_Table:			return( SG_T("TABLE") );
	case DATAOBJECT_TYPE_Shapes:		return( SG_T("SHAPES") );
	case DATAOBJECT_TYPE_TIN:			return( SG_T("TIN") );
	case DATAOBJECT_TYPE_PointCloud:	return( SG_T("POINTS") );
	}
}

//---------------------------------------------------------
const SG_Char *	SG_Get_DataObject_Name(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	default:
	case DATAOBJECT_TYPE_Undefined:		return( LNG("[DAT] Undefined") );
	case DATAOBJECT_TYPE_Grid:			return( LNG("[DAT] Grid") );
	case DATAOBJECT_TYPE_Table:			return( LNG("[DAT] Table") );
	case DATAOBJECT_TYPE_Shapes:		return( LNG("[DAT] Shapes") );
	case DATAOBJECT_TYPE_TIN:			return( LNG("[DAT] TIN") );
	case DATAOBJECT_TYPE_PointCloud:	return( LNG("[DAT] Point Cloud") );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		g_History_Depth	= -1;

//---------------------------------------------------------
void	SG_Set_History_Depth			(int Depth)
{
	g_History_Depth	= Depth;
}

//---------------------------------------------------------
int		SG_Get_History_Depth			(void)
{
	return( g_History_Depth );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object::CSG_Data_Object(void)
{
	m_MetaData.Set_Name(SG_T("SAGA_METADATA"));

	m_pHistory			= m_MetaData  .Add_Child(SG_META_HST);

	m_pMetaData			= m_MetaData  .Add_Child(SG_META_SRC);
	m_pFile				= m_pMetaData->Add_Child(SG_META_SRC_FILE);
	m_pMetaData_DB		= m_pMetaData->Add_Child(SG_META_SRC_DB);
	m_pProjection		= m_pMetaData->Add_Child(SG_META_SRC_PROJ);

	//-----------------------------------------------------
	m_File_Type			= 0;
	m_bModified			= true;

	m_NoData_Value		= -99999.0;
	m_NoData_hiValue	= -999.0;

	m_Name				.Clear();
	m_Description		.Clear();

	m_bUpdate			= false;
}

//---------------------------------------------------------
CSG_Data_Object::~CSG_Data_Object(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Data_Object::Destroy(void)
{
	m_pHistory			->Destroy();

	m_Name				.Clear();
	m_Description		.Clear();

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
	m_Name	= Name ? CSG_String(Name).c_str() : LNG("[DAT] new");
}

const SG_Char * CSG_Data_Object::Get_Name(void) const
{
	return( m_Name.c_str() );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_Description(const SG_Char *Description)
{
	m_Description	= Description ? Description : SG_T("");
}

const SG_Char * CSG_Data_Object::Get_Description(void) const
{
	return( m_Description.c_str() );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Name(const SG_Char *File_Name)
{
	if( File_Name )
	{
		m_File_Name	= File_Name;

		m_Name		= SG_File_Get_Name(File_Name, false);

		m_bModified	= false;
	}
	else
	{
		m_File_Name.Clear();

		Set_Name(NULL);
	}

	m_pFile->Set_Content(m_File_Name);
}

const SG_Char * CSG_Data_Object::Get_File_Name(bool bNullAsString)	const
{
	return(	m_File_Name.Length() > 0
		?	m_File_Name.c_str()
		:	(bNullAsString ? (SG_Char *)LNG("[DAT] [not set]") : (SG_Char *)NULL)
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
bool CSG_Data_Object::Set_NoData_Value(double Value)
{
	return( Set_NoData_Value_Range(Value, Value) );
}

//---------------------------------------------------------
bool CSG_Data_Object::Set_NoData_Value_Range(double loValue, double hiValue)
{
	if( loValue > hiValue )
	{
		double	d	= loValue;
		loValue		= hiValue;
		hiValue		= d;
	}

	if( loValue != m_NoData_Value || hiValue != m_NoData_hiValue )
	{
		if( !Get_Update_Flag() )
		{
			Set_Update_Flag();
		}

		m_NoData_Value		= loValue;
		m_NoData_hiValue	= hiValue;

		On_NoData_Changed();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Load_MetaData(const SG_Char *File_Name)
{
	CSG_MetaData	m, *p;

	switch( Get_ObjectType() )
	{
	default:							return( false );;
	case DATAOBJECT_TYPE_Grid:			m.Load(File_Name, SG_META_EXT_GRID);		break;
	case DATAOBJECT_TYPE_Table:			m.Load(File_Name, SG_META_EXT_TABLE);		break;
	case DATAOBJECT_TYPE_Shapes:		m.Load(File_Name, SG_META_EXT_SHAPES);		break;
	case DATAOBJECT_TYPE_TIN:			m.Load(File_Name, SG_META_EXT_TIN);			break;
	case DATAOBJECT_TYPE_PointCloud:	m.Load(File_Name, SG_META_EXT_POINTCLOUD);	break;
	}

	if( (p = m.Get_Child(SG_META_SRC)) != NULL )
	{
		m_pMetaData_DB->Destroy();

		if( p->Get_Child(SG_META_SRC_DB) )
		{
			m_pMetaData_DB->Assign(*p->Get_Child(SG_META_SRC_DB));
		}

		m_pProjection->Destroy();

		if( p->Get_Child(SG_META_SRC_PROJ) && m_pProjection->Assign(*p->Get_Child(SG_META_SRC_PROJ)) )
		{
			m_Projection.Load(*m_pProjection);
		}
	}

	m_pHistory->Destroy();

	if( (p = m.Get_Child(SG_META_HST)) != NULL )
	{
		m_pHistory->Assign(*m.Get_Child(SG_META_HST));
	}
	else
	{
		m_pHistory->Add_Child(SG_META_SRC_FILE, File_Name);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Data_Object::Save_MetaData(const SG_Char *File_Name)
{
	if( m_Projection.Get_Type() == SG_PROJ_TYPE_CS_Undefined )
	{
		m_pProjection->Destroy();
	}
	else
	{
		m_Projection.Save(*m_pProjection);
	}

	switch( Get_ObjectType() )
	{
	default:							return( m_MetaData.Save(File_Name) );
	case DATAOBJECT_TYPE_Grid:			return( m_MetaData.Save(File_Name, SG_META_EXT_GRID) );
	case DATAOBJECT_TYPE_Table:			return( m_MetaData.Save(File_Name, SG_META_EXT_TABLE) );
	case DATAOBJECT_TYPE_Shapes:		return( m_MetaData.Save(File_Name, SG_META_EXT_SHAPES) );
	case DATAOBJECT_TYPE_TIN:			return( m_MetaData.Save(File_Name, SG_META_EXT_TIN) );
	case DATAOBJECT_TYPE_PointCloud:	return( m_MetaData.Save(File_Name, SG_META_EXT_POINTCLOUD) );
	}
}

//---------------------------------------------------------
bool CSG_Data_Object::Update(void)
{
	if( m_bUpdate )
	{
		m_bUpdate	= false;

		return( On_Update() );
	}

	return( true );
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
