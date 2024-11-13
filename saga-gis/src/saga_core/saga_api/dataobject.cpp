
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
#include "dataobject.h"

#include <wx/string.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void *		SG_Get_Create_Pointer(void)
{
	return( DATAOBJECT_CREATE );
}

//---------------------------------------------------------
CSG_String	SG_Get_DataObject_Identifier(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	case SG_DATAOBJECT_TYPE_Grid      : return( "GRID"      );
	case SG_DATAOBJECT_TYPE_Grids     : return( "GRIDS"     );
	case SG_DATAOBJECT_TYPE_Table     : return( "TABLE"     );
	case SG_DATAOBJECT_TYPE_Shapes    : return( "SHAPES"    );
	case SG_DATAOBJECT_TYPE_TIN       : return( "TIN"       );
	case SG_DATAOBJECT_TYPE_PointCloud: return( "POINTS"    );
	default                           : return( "UNDEFINED" );
	}
}

//---------------------------------------------------------
CSG_String	SG_Get_DataObject_Name(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	case SG_DATAOBJECT_TYPE_Grid      : return( _TL("Grid"       ) );
	case SG_DATAOBJECT_TYPE_Grids     : return( _TL("Grids"      ) );
	case SG_DATAOBJECT_TYPE_Table     : return( _TL("Table"      ) );
	case SG_DATAOBJECT_TYPE_Shapes    : return( _TL("Shapes"     ) );
	case SG_DATAOBJECT_TYPE_TIN       : return( _TL("TIN"        ) );
	case SG_DATAOBJECT_TYPE_PointCloud: return( _TL("Point Cloud") );
	default                           : return( _TL("Undefined"  ) );
	}
}

//---------------------------------------------------------
CSG_String	SG_Get_DataObject_Class_Name(TSG_Data_Object_Type Type)
{
	switch( Type )
	{
	case SG_DATAOBJECT_TYPE_Grid      : return( "CSG_Grid"       );
	case SG_DATAOBJECT_TYPE_Grids     : return( "CSG_Grids"      );
	case SG_DATAOBJECT_TYPE_Table     : return( "CSG_Table"      );
	case SG_DATAOBJECT_TYPE_Shapes    : return( "CSG_Shapes"     );
	case SG_DATAOBJECT_TYPE_TIN       : return( "CSG_TIN"        );
	case SG_DATAOBJECT_TYPE_PointCloud: return( "CSG_PointCloud" );
	default                           : return( "CSG_DataObject" );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//				Data Object Statistics					 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static sLong		gSG_DataObject_Max_Samples	= 0;

//---------------------------------------------------------
bool				SG_DataObject_Set_Max_Samples	(sLong Max_Samples)
{
	if( Max_Samples >= 0 )
	{
		gSG_DataObject_Max_Samples	= Max_Samples;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
sLong				SG_DataObject_Get_Max_Samples	(void)
{
	return( gSG_DataObject_Max_Samples );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Data_Object_Delete		(CSG_Data_Object *pObject)
{
	if( pObject && !pObject->Get_Managed() )
	{
		delete(pObject);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object::CSG_Data_Object(void)
{
	static int RefCount = 0;

	#pragma omp critical
	{
		m_RefID       = ++RefCount;
	}

	m_pOwner          = NULL;

	m_File_bNative    = false;
	m_File_Type       = 0;

	m_NoData_Value[0] = -99999.;
	m_NoData_Value[1] = -99999.;

	m_Max_Samples     = gSG_DataObject_Max_Samples;
	m_bModified       = true;
	m_bUpdate         = false;

	m_MetaData.Set_Name("SAGA_METADATA");

	m_pMD_Database    = m_MetaData.Add_Child(SG_META_DATABASE);
	m_pMD_Source      = m_MetaData.Add_Child(SG_META_SOURCE  );
	m_pMD_History     = m_MetaData.Add_Child(SG_META_HISTORY );

	#ifdef WITH_LIFETIME_TRACKER
	#pragma omp critical
	{
		++m_Track_nObjects;

		if( m_Track )
		{
			SG_UI_Console_Print_StdOut(CSG_String::Format("data object (refid=%04d) constructed, new object count is %d", m_RefID, m_Track_nObjects - m_Track_Offset));
		}
	}
	#endif // WITH_LIFETIME_TRACKER
}

//---------------------------------------------------------
CSG_Data_Object::~CSG_Data_Object(void)
{
	Destroy();

	#ifdef WITH_LIFETIME_TRACKER
	#pragma omp critical
	{
		--m_Track_nObjects;

		if( m_Track )
		{
			SG_UI_Console_Print_StdOut(CSG_String::Format("data object (refid=%04d) destructed,  new object count is %d", m_RefID, m_Track_nObjects - m_Track_Offset));
		}
	}
	#endif // WITH_LIFETIME_TRACKER
}

//---------------------------------------------------------
bool CSG_Data_Object::m_Track = false; int CSG_Data_Object::m_Track_nObjects = 0; int CSG_Data_Object::m_Track_Offset = 0;

//---------------------------------------------------------
void CSG_Data_Object::Track(bool Track, bool Offset)
{
	#ifdef WITH_LIFETIME_TRACKER
	m_Track = Track; m_Track_Offset = Offset ? m_Track_nObjects : 0;

	SG_UI_Console_Print_StdOut(CSG_String::Format("data object construction/destruction tracker, state=%s, offset=%s, current object count is %d",
		m_Track ? SG_T("ON") : SG_T("OFF"), m_Track_Offset ? SG_T("ON") : SG_T("OFF"), m_Track_nObjects
	));
	#else
	SG_UI_Console_Print_StdOut("CSG_Data_Object::Track() functionality (aka data object lifetime tracker) has not been built for this configuration!");
	#endif // WITH_LIFETIME_TRACKER
}

//---------------------------------------------------------
bool CSG_Data_Object::Destroy(void)
{
	m_Name.Clear(); m_Description.Clear();

	m_pMD_Database->Destroy();
	m_pMD_Source  ->Destroy();
	m_pMD_History ->Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Data_Object::Set_Name(const char       *Name) { Set_Name(CSG_String(Name)); }
void CSG_Data_Object::Set_Name(const wchar_t    *Name) { Set_Name(CSG_String(Name)); }
void CSG_Data_Object::Set_Name(const CSG_String &Name)
{
	if( Name.is_Empty() )
	{
		m_Name = _TL("Data");
	}
	else
	{
		m_Name = Name;
	}
}

//---------------------------------------------------------
void CSG_Data_Object::Fmt_Name(const char *Format, ...)
{
	wxString _s; va_list argptr;
	
#ifdef _SAGA_LINUX
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_start(argptr, _Format); _s.PrintfV(_Format, argptr);
#else
	va_start(argptr,  Format); _s.PrintfV( Format, argptr);
#endif

	va_end(argptr);

	Set_Name(CSG_String(&_s));
}

//---------------------------------------------------------
void CSG_Data_Object::Fmt_Name(const wchar_t *Format, ...)
{
	wxString _s; va_list argptr;
	
#ifdef _SAGA_LINUX
	// workaround as we only use wide characters
	// since wx 2.9.4 so interpret strings as multibyte
	wxString _Format(Format); _Format.Replace("%s", "%ls");	// workaround as we only use wide characters since wx 2.9.4 so interpret strings as multibyte
	va_start(argptr, _Format); _s.PrintfV(_Format, argptr);
#else
	va_start(argptr,  Format); _s.PrintfV( Format, argptr);
#endif

	va_end(argptr);

	Set_Name(CSG_String(&_s));
}

//---------------------------------------------------------
const SG_Char * CSG_Data_Object::Get_Name(void) const
{
	return( m_Name );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_Description(const CSG_String &Description)
{
	m_Description = Description;
}

const SG_Char * CSG_Data_Object::Get_Description(void) const
{
	return( m_Description.is_Empty() && m_pOwner ? m_pOwner->m_Description : m_Description );
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Name	(const CSG_String &FileName)
{
	Set_File_Name(FileName, false);
}

//---------------------------------------------------------
void CSG_Data_Object::Set_File_Name(const CSG_String &FileName, bool bNative)
{
	if( FileName.is_Empty() )
	{
		m_FileName      .Clear();
		m_File_bNative	= false;
		m_bModified		= true;
	}
	else
	{
		m_FileName		= FileName;
		m_File_bNative	= bNative;
		m_bModified		= false;
		m_Name			= SG_File_Get_Name(FileName, false);
	}
}

//---------------------------------------------------------
const SG_Char * CSG_Data_Object::Get_File_Name(bool bNative)	const
{
	if( bNative && !m_File_bNative )
	{
		return( SG_T("") );
	}

	if( m_pOwner )
	{
		if( m_pOwner->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
		{
			return( m_pOwner->m_FileName.c_str() );
		}
	}

	return( m_FileName.c_str() );
}

//---------------------------------------------------------
int CSG_Data_Object::Get_File_Type(void)	const
{
	return( m_File_Type );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/*
* If the data object has a file association, this function
* can be used to reload its content.
*/
//---------------------------------------------------------
bool CSG_Data_Object::Reload(void)
{
	return( SG_File_Exists(m_FileName) && On_Reload() );
}

//---------------------------------------------------------
/*
* If the data object is stored in a native SAGA format,
* this function can be used to delete all files associated
* with it.
*/
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
		default                           : SG_File_Set_Extension(FileName, "sg-info"); break;
		}

		SG_File_Delete(FileName);

		SG_File_Set_Extension(FileName,    "prj"); SG_File_Delete(FileName);
		SG_File_Set_Extension(FileName, "sg-prj"); SG_File_Delete(FileName);

		//-------------------------------------------------
		m_FileName		= "";
		m_File_bNative	= false;
		m_File_Type		= 0;

		m_bModified		= true;

		m_pMD_Database->Destroy();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/*
* Type cast function. Returns NULL if object is not exactly
* of class type CSG_Table with bPolymorph = false or of
* one of its derivatives (CSG_Shapes, CSG_PointCloud,
* CSG_TIN) with bPolymorph = true.
*/
//---------------------------------------------------------
CSG_Table * CSG_Data_Object::asTable(bool bPolymorph) const
{
	if( bPolymorph )
	{
		return( Get_ObjectType() == SG_DATAOBJECT_TYPE_Table
			||  Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
			||  Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
			||  Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN
			? (CSG_Table *)this : NULL
		);
	}

	return( Get_ObjectType() == SG_DATAOBJECT_TYPE_Table ? (CSG_Table *)this : NULL );
}

//---------------------------------------------------------
/*
* Type cast function. Returns NULL if object is not exactly
* of class type CSG_Shapes with bPolymorph = false or of
* one of its derivatives (CSG_PointCloud) with bPolymorph = true.
*/
//---------------------------------------------------------
CSG_Shapes * CSG_Data_Object::asShapes(bool bPolymorph) const
{
	if( bPolymorph )
	{
		return( Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
			||  Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
			? (CSG_Shapes *)this : NULL
			);
	}

	return( Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes ? (CSG_Shapes *)this : NULL );
}

//---------------------------------------------------------
/*
* Type cast function. Returns NULL if object is not exactly
* of class type CSG_TIN. The bPolymorph flag has no effect.
*/
//---------------------------------------------------------
CSG_TIN * CSG_Data_Object::asTIN(bool bPolymorph) const
{
	return( Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN ? (CSG_TIN *)this : NULL );
}

//---------------------------------------------------------
/*
* Type cast function. Returns NULL if object is not exactly
* of class type CSG_PointCloud. The bPolymorph flag has no effect.
*/
//---------------------------------------------------------
CSG_PointCloud * CSG_Data_Object::asPointCloud(bool bPolymorph) const
{
	return( Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud ? (CSG_PointCloud *)this : NULL );
}

//---------------------------------------------------------
/*
* Type cast function. Returns NULL if object is not exactly
* of class type CSG_Grid. The bPolymorph flag has no effect.
*/
//---------------------------------------------------------
CSG_Grid * CSG_Data_Object::asGrid(bool bPolymorph) const
{
	return( Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid ? (CSG_Grid *)this : NULL );
}

//---------------------------------------------------------
/*
* Type cast function. Returns NULL if object is not exactly
* of class type CSG_Grids. The bPolymorph flag has no effect.
*/
//---------------------------------------------------------
CSG_Grids * CSG_Data_Object::asGrids(bool bPolymorph) const
{
	return( Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids ? (CSG_Grids *)this : NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Set_NoData_Value(double Value)
{
	return( Set_NoData_Value_Range(Value, Value) );
}

//---------------------------------------------------------
bool CSG_Data_Object::Set_NoData_Value_Range(double Lower, double Upper)
{
	if( Lower > Upper )
	{
		double d = Lower; Lower = Upper; Upper = d;
	}

	if( Lower != m_NoData_Value[0] || Upper != m_NoData_Value[1] )
	{
		m_NoData_Value[0]	= Lower;
		m_NoData_Value[1]	= Upper;

		Set_Modified();

		On_NoData_Changed();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Data_Object::On_NoData_Changed(void)
{
	if( !Get_Update_Flag() )
	{
		Set_Update_Flag();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Set_Max_Samples(sLong Max_Samples)
{
	#define	Min_Samples	100

	if( m_Max_Samples != Max_Samples && Max_Samples >= Min_Samples )
	{
		m_Max_Samples	= Max_Samples;

        Update(true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Load_MetaData(const CSG_String &_FileName)
{
	CSG_String FileName(_FileName);

	switch( Get_ObjectType() )
	{
	case SG_DATAOBJECT_TYPE_Grid      : SG_File_Set_Extension(FileName, "mgrd"   ); break;
	case SG_DATAOBJECT_TYPE_Grids     : SG_File_Set_Extension(FileName, "sg-info"); break;
	case SG_DATAOBJECT_TYPE_Table     : SG_File_Set_Extension(FileName, "mtab"   ); break;
	case SG_DATAOBJECT_TYPE_Shapes    : SG_File_Set_Extension(FileName, "mshp"   ); break;
	case SG_DATAOBJECT_TYPE_TIN       : SG_File_Set_Extension(FileName, "sg-info"); break;
	case SG_DATAOBJECT_TYPE_PointCloud: SG_File_Set_Extension(FileName, "sg-info");
		if( !SG_File_Get_Extension(_FileName).CmpNoCase("spc") )
		{
			SG_File_Set_Extension(FileName, "mpts");
		}
		break;

	default: return( false );
	}

	CSG_File Stream(FileName, SG_FILE_R, false);

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

	//-----------------------------------------------------
	if( m("DESCRIPTION") && !m["DESCRIPTION"].Get_Content().is_Empty() )
	{
		Set_Description(m["DESCRIPTION"].Get_Content());
	}

	//-----------------------------------------------------
	m_pMD_Source->Destroy();

	if( m(SG_META_SOURCE) )
		m_pMD_Source->Assign(m[SG_META_SOURCE]);

	//-----------------------------------------------------
	m_pMD_Database->Destroy();

	if( m(SG_META_DATABASE) )
		m_pMD_Database->Assign(m[SG_META_DATABASE]);

	//-----------------------------------------------------
	m_MetaData.Del_Child(SG_META_PROJECTION);

	if( m(SG_META_PROJECTION) && m_Projection.Load(m[SG_META_PROJECTION]) )
		m_MetaData.Add_Child(m[SG_META_PROJECTION]);
	else if( m[SG_META_SOURCE](SG_META_PROJECTION) && m_Projection.Load(m[SG_META_SOURCE][SG_META_PROJECTION]) )
		m_MetaData.Add_Child(m[SG_META_SOURCE][SG_META_PROJECTION]);

	//-----------------------------------------------------
	m_pMD_History->Destroy();

	if( m(SG_META_HISTORY) )
		m_pMD_History->Assign(m[SG_META_HISTORY]);
	else
		m_pMD_History->Add_Child(SG_META_FILEPATH, Get_File_Name());

	return( true );
}

//---------------------------------------------------------
bool CSG_Data_Object::Save_MetaData(CSG_File &Stream)
{
	//-----------------------------------------------------
	if( m_MetaData(SG_META_FILEPATH) )
		m_MetaData(SG_META_FILEPATH)->Set_Content(m_FileName);
	else
		m_MetaData.Add_Child(SG_META_FILEPATH, m_FileName);

	//-----------------------------------------------------
	if( m_MetaData("DESCRIPTION") )
		m_MetaData("DESCRIPTION")->Set_Content(Get_Description());
	else
		m_MetaData.Add_Child("DESCRIPTION", Get_Description());

	//-----------------------------------------------------
	if( m_Projection.Get_Type() == ESG_CRS_Type::Undefined )
		m_MetaData.Del_Child(SG_META_PROJECTION);
	else if( m_MetaData(SG_META_PROJECTION) )
		m_Projection.Save(*m_MetaData(SG_META_PROJECTION));
	else
		m_Projection.Save(*m_MetaData.Add_Child(SG_META_PROJECTION));

	//-----------------------------------------------------
	return( m_MetaData.Save(Stream) );
}

//---------------------------------------------------------
bool CSG_Data_Object::Update(bool bForce)
{
	if( m_bUpdate || bForce )
	{
		m_bUpdate	= false;

		bool bResult = On_Update();

		return( bResult );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Data_Object::Assign(CSG_Data_Object *pObject)
{
	if( pObject )// && pObject->is_Valid() )
	{
		Destroy();

		m_Name         = pObject->Get_Name();
		m_Projection   = pObject->m_Projection;
		Get_MetaData() = pObject->Get_MetaData();
	//	Get_History () = pObject->Get_History ();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "tool_chain.h"

//---------------------------------------------------------
bool CSG_Data_Object::Save_History_to_Model(const CSG_String &File)	const
{
	return( CSG_Tool_Chain::Save_History_to_Model(Get_History(), File) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
