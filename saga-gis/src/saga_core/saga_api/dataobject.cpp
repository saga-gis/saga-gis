
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
CSG_String	SG_Get_DataObject_Identifier(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	default                        : return( "UNDEFINED");
	case SG_DATAOBJECT_TYPE_Grid      : return( "GRID"     );
	case SG_DATAOBJECT_TYPE_Grids     : return( "GRIDS"    );
	case SG_DATAOBJECT_TYPE_Table     : return( "TABLE"    );
	case SG_DATAOBJECT_TYPE_Shapes    : return( "SHAPES"   );
	case SG_DATAOBJECT_TYPE_TIN       : return( "TIN"      );
	case SG_DATAOBJECT_TYPE_PointCloud: return( "POINTS"   );
	}
}

//---------------------------------------------------------
CSG_String	SG_Get_DataObject_Name(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	default                        : return( _TL("Undefined"  ) );
	case SG_DATAOBJECT_TYPE_Grid      : return( _TL("Grid"       ) );
	case SG_DATAOBJECT_TYPE_Grids     : return( _TL("Grids"      ) );
	case SG_DATAOBJECT_TYPE_Table     : return( _TL("Table"      ) );
	case SG_DATAOBJECT_TYPE_Shapes    : return( _TL("Shapes"     ) );
	case SG_DATAOBJECT_TYPE_TIN       : return( _TL("TIN"        ) );
	case SG_DATAOBJECT_TYPE_PointCloud: return( _TL("Point Cloud") );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int		g_History_Depth	= -1;

void	SG_Set_History_Depth			(int Depth)
{
	g_History_Depth	= Depth;
}

int		SG_Get_History_Depth			(void)
{
	return( g_History_Depth );
}

//---------------------------------------------------------
int		g_History_Ignore_Lists	= true;

void	SG_Set_History_Ignore_Lists		(int Ignore)
{
	g_History_Ignore_Lists	= Ignore != 0;
}

int		SG_Get_History_Ignore_Lists		(void)
{
	return( g_History_Ignore_Lists );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object::CSG_Data_Object(void)
{
	m_MetaData.Set_Name("SAGA_METADATA");

	m_pHistory			= m_MetaData  .Add_Child(SG_META_HST);

	m_pMetaData			= m_MetaData  .Add_Child(SG_META_SRC);
	m_pFile				= m_pMetaData->Add_Child(SG_META_SRC_FILE);
	m_pMetaData_DB		= m_pMetaData->Add_Child(SG_META_SRC_DB);
	m_pProjection		= m_pMetaData->Add_Child(SG_META_SRC_PROJ);

	//-----------------------------------------------------
	m_File_bNative		= false;
	m_File_Type			= 0;
	m_bModified			= true;

	m_NoData_Value		= -99999.0;
	m_NoData_hiValue	= -99999.0;

	m_Name				.Clear();
	m_Description		.Clear();

	m_bUpdate			= false;

	m_pOwner			= NULL;
}

//---------------------------------------------------------
CSG_Data_Object::~CSG_Data_Object(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Data_Object::Destroy(void)
{
	m_pHistory->Destroy();

	m_Name       .Clear();
	m_Description.Clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Data_Object::Set_Name(const CSG_String &Name)
{
	m_Name			= Name.Length() > 0 ? Name.c_str() : _TL("new");
}

const SG_Char * CSG_Data_Object::Get_Name(void) const
{
	return( m_Name );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_Description(const CSG_String &Description)
{
	m_Description	= Description;
}

const SG_Char * CSG_Data_Object::Get_Description(void) const
{
	return( m_pOwner ? m_pOwner->m_Description : m_Description );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Name	(const CSG_String &FileName)
{
	Set_File_Name(FileName, false);
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Name(const CSG_String &FileName, bool bNative)
{
	m_FileName		= FileName;
	m_File_bNative	= bNative;

	m_Name			= SG_File_Get_Name(FileName, false);

	m_bModified		= false;

	m_pFile->Set_Content(m_FileName);
}

//---------------------------------------------------------
const SG_Char * CSG_Data_Object::Get_File_Name(bool bNative)	const
{
	return( !bNative || m_File_bNative ? m_FileName.c_str() : SG_T("") );
}

//---------------------------------------------------------
int CSG_Data_Object::Get_File_Type(void)	const
{
	return( m_File_Type );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Reload(void)
{
	return( m_File_bNative && SG_File_Exists(m_FileName) && On_Reload() );
}

//---------------------------------------------------------
bool CSG_Data_Object::Delete(void)
{
	if( m_File_bNative && SG_File_Exists(m_FileName) && On_Delete() )
	{
		CSG_String	FileName	= m_FileName;

		switch( Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid      : SG_File_Set_Extension(FileName, "mgrd"   ); break;
		case SG_DATAOBJECT_TYPE_Grids     : SG_File_Set_Extension(FileName, "sg-info"); break;
		case SG_DATAOBJECT_TYPE_Table     : SG_File_Set_Extension(FileName, "mtab"   ); break;
		case SG_DATAOBJECT_TYPE_Shapes    : SG_File_Set_Extension(FileName, "mshp"   ); break;
		case SG_DATAOBJECT_TYPE_TIN       : SG_File_Set_Extension(FileName, "sg-info"); break;
		case SG_DATAOBJECT_TYPE_PointCloud: SG_File_Set_Extension(FileName, "sg-info"); break;
		default                        : SG_File_Set_Extension(FileName, "sg-info"); break;
		}

		SG_File_Delete(FileName);

		SG_File_Set_Extension(FileName,    "prj"); SG_File_Delete(FileName);
		SG_File_Set_Extension(FileName, "sg-prj"); SG_File_Delete(FileName);

		//-------------------------------------------------
		m_FileName		= "";
		m_File_bNative	= false;
		m_File_Type		= 0;

		m_bModified		= true;

		m_pFile       ->Set_Content("");
		m_pMetaData_DB->Del_Children();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Projection & CSG_Data_Object::Get_Projection(void)
{
	return( m_pOwner ? m_pOwner->m_Projection : m_Projection );
}

//---------------------------------------------------------
const CSG_Projection & CSG_Data_Object::Get_Projection(void) const
{
	return( m_pOwner ? m_pOwner->m_Projection : m_Projection );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Load_MetaData(const CSG_String &_FileName)
{
	CSG_String	FileName(_FileName);

	switch( Get_ObjectType() )
	{
	case SG_DATAOBJECT_TYPE_Grid      : SG_File_Set_Extension(FileName, "mgrd"   ); break;
	case SG_DATAOBJECT_TYPE_Grids     : SG_File_Set_Extension(FileName, "sg-info"); break;
	case SG_DATAOBJECT_TYPE_Table     : SG_File_Set_Extension(FileName, "mtab"   ); break;
	case SG_DATAOBJECT_TYPE_Shapes    : SG_File_Set_Extension(FileName, "mshp"   ); break;
	case SG_DATAOBJECT_TYPE_TIN       : SG_File_Set_Extension(FileName, "sg-info"); break;
	case SG_DATAOBJECT_TYPE_PointCloud: SG_File_Set_Extension(FileName, "sg-info");
		if( !SG_File_Get_Extension(_FileName).CmpNoCase("spc") ) SG_File_Set_Extension(FileName, "mpts"); break;

	default:	return( false );
	}

	CSG_File	Stream(FileName, SG_FILE_R, false);

	return( Load_MetaData(Stream) );
}

//---------------------------------------------------------
bool CSG_Data_Object::Save_MetaData(const CSG_String &_FileName)
{
	CSG_String	FileName(_FileName);

	switch( Get_ObjectType() )
	{
	case SG_DATAOBJECT_TYPE_Grid      : SG_File_Set_Extension(FileName, "mgrd"   ); break;
	case SG_DATAOBJECT_TYPE_Grids     : SG_File_Set_Extension(FileName, "sg-info"); break;
	case SG_DATAOBJECT_TYPE_Table     : SG_File_Set_Extension(FileName, "mtab"   ); break;
	case SG_DATAOBJECT_TYPE_Shapes    : SG_File_Set_Extension(FileName, "mshp"   ); break;
	case SG_DATAOBJECT_TYPE_TIN       : SG_File_Set_Extension(FileName, "sg-info"); break;
	case SG_DATAOBJECT_TYPE_PointCloud: SG_File_Set_Extension(FileName, "sg-info"); break;

	default:	return( false );
	}

	CSG_File	Stream(FileName, SG_FILE_W, false);

	return( Save_MetaData(Stream) );
}

//---------------------------------------------------------
bool CSG_Data_Object::Load_MetaData(CSG_File &Stream)
{
	CSG_MetaData	m;

	if( !m.Load(Stream) )
	{
		return( false );
	}

	if( m("DESCRIPTION") && !m["DESCRIPTION"].Get_Content().is_Empty() )
	{
		Set_Description(m["DESCRIPTION"].Get_Content());
	}

	if( m(SG_META_SRC) )
	{
		m_pMetaData_DB->Destroy();

		if( m[SG_META_SRC](SG_META_SRC_DB) )
		{
			m_pMetaData_DB->Assign(m[SG_META_SRC][SG_META_SRC_DB]);
		}

		m_pProjection->Destroy();

		if( m[SG_META_SRC](SG_META_SRC_PROJ) && m_pProjection->Assign(m[SG_META_SRC][SG_META_SRC_PROJ]) )
		{
			m_Projection.Load(*m_pProjection);
		}
	}

	m_pHistory->Destroy();

	if( m(SG_META_HST) )
	{
		m_pHistory->Assign(m[SG_META_HST]);
	}
	else
	{
		m_pHistory->Add_Child(SG_META_SRC_FILE, Get_File_Name());
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Data_Object::Save_MetaData(CSG_File &Stream)
{	// update meta data before saving
	if( m_MetaData("DESCRIPTION") )
	{
		m_MetaData("DESCRIPTION")->Set_Content(Get_Description());
	}
	else
	{
		m_MetaData.Add_Child("DESCRIPTION", Get_Description());
	}

	if( m_Projection.Get_Type() == SG_PROJ_TYPE_CS_Undefined )
	{
		m_pProjection->Destroy();
	}
	else
	{
		m_Projection.Save(*m_pProjection);
	}

	return( m_MetaData.Save(Stream) );
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Assign(CSG_Data_Object *pObject)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool_chain.h"

//---------------------------------------------------------
bool CSG_Data_Object::Save_History_to_Model(const CSG_String &File)	const
{
	return( CSG_Tool_Chain::Save_History_to_Model(Get_History(), File) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
