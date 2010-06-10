
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
//                    projections.cpp                    //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#include "geo_tools.h"

#include "table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Projections		gSG_Projections;

//---------------------------------------------------------
CSG_Projections &	SG_Get_Projections(void)
{
	return( gSG_Projections );
}

//---------------------------------------------------------
CSG_String			SG_Get_Projection_Type_Name(TSG_Projection_Type Type)
{
	switch( Type )
	{
	default:
	case SG_PROJ_TYPE_CS_Undefined:		return( LNG("Undefined Coordinate System") );
	case SG_PROJ_TYPE_CS_Projected:		return( LNG("Projected Coordinate System") );
	case SG_PROJ_TYPE_CS_Geographic:	return( LNG("Geographic Coordinate System") );
	case SG_PROJ_TYPE_CS_Geocentric:	return( LNG("Geocentric Coordinate System") );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Projection::CSG_Projection(void)
{
	_Reset();
}

CSG_Projection::~CSG_Projection(void)
{}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(const CSG_Projection &Projection)
{
	_Reset();

	Create(Projection);
}

bool CSG_Projection::Create(const CSG_Projection &Projection)
{
	return( Assign(Projection) );
}

bool CSG_Projection::Assign(const CSG_Projection &Projection)
{
	m_Name		= Projection.m_Name;
	m_Type		= Projection.m_Type;
	m_WKT		= Projection.m_WKT;
	m_Proj4		= Projection.m_Proj4;
	m_EPSG		= Projection.m_EPSG;

	return( true );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(int EPSG_SRID)
{
	_Reset();

	Create(EPSG_SRID);
}

bool CSG_Projection::Create(int EPSG_SRID)
{
	return( Assign(EPSG_SRID) );
}

bool CSG_Projection::Assign(int EPSG_SRID)
{
	return( Assign(CSG_String::Format(SG_T("%d"), EPSG_SRID), SG_PROJ_FMT_EPSG) );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(const CSG_String &Projection, TSG_Projection_Format Format)
{
	_Reset();

	Create(Projection, Format);
}

bool CSG_Projection::Create(const CSG_String &Projection, TSG_Projection_Format Format)
{
	return( Assign(Projection, Format) );
}

bool CSG_Projection::Assign(const CSG_String &Projection, TSG_Projection_Format Format)
{
	_Reset();

	switch( Format )
	{
	default:
		return( false );

	case SG_PROJ_FMT_WKT:
		if( !gSG_Projections.WKT_to_Proj4(m_Proj4, Projection) )
		{
			return( false );
		}

		m_WKT	= Projection;

		break;

	case SG_PROJ_FMT_Proj4:
		if( !gSG_Projections.WKT_to_Proj4(m_WKT  , Projection) )
		{
			return( false );
		}

		m_Proj4	= Projection;

		break;

	case SG_PROJ_FMT_EPSG:
		if( !Projection.asInt(m_EPSG) || !gSG_Projections.Get_Projection(m_EPSG, *this) )
		{
			return( false );
		}
		break;
	}

	m_Name	= m_WKT.AfterFirst(SG_T('\"')).BeforeFirst(SG_T('\"'));

	if(      m_WKT.Make_Upper().Find(SG_T("GEOGCS")) >= 0 )
	{
		m_Type	= SG_PROJ_TYPE_CS_Geographic;
	}
	else if( m_WKT.Make_Upper().Find(SG_T("PROJCS")) >= 0 )
	{
		m_Type	= SG_PROJ_TYPE_CS_Projected;
	}
	else
	{
		m_Type	= SG_PROJ_TYPE_CS_Geocentric;
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Projection::_Reset(void)
{
	m_Name		= LNG("undefined");
	m_Type		= SG_PROJ_TYPE_CS_Undefined;
	m_WKT		.Clear();
	m_Proj4		.Clear();
	m_EPSG		= -1;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::Load(const CSG_String &File_Name, TSG_Projection_Format Format)
{
	CSG_File	Stream;
	CSG_String	s;

	if( Stream.Open(File_Name, SG_FILE_R, false) )
	{
		Stream.Read(s, Stream.Length());

		return( Assign(s, Format) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Save(const CSG_String &File_Name, TSG_Projection_Format Format) const
{
	if( is_Okay() )
	{
		CSG_File	Stream;

		switch( Format )
		{
		default:
			break;

		case SG_PROJ_FMT_WKT:
			if( Stream.Open(File_Name, SG_FILE_W, false) )
			{
				Stream.Write((void *)m_WKT.c_str(), m_WKT.Length());

				return( true );
			}
			break;

		case SG_PROJ_FMT_Proj4:
			if( Stream.Open(File_Name, SG_FILE_W, false) )
			{
				Stream.Write((void *)m_Proj4.c_str(), m_Proj4.Length());

				return( true );
			}
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Load(const CSG_MetaData &Projection)
{
	CSG_MetaData	*pEntry;

	if( (pEntry = Projection.Get_Child(SG_T("OGC_WKT"))) != NULL )
	{
		Assign(pEntry->Get_Content(), SG_PROJ_FMT_WKT);

		if( (pEntry = Projection.Get_Child(SG_T("PROJ4"))) != NULL )
		{
			m_Proj4	= pEntry->Get_Content();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Save(CSG_MetaData &Projection) const
{
	Projection.Add_Child(SG_T("OGC_WKT"), m_WKT  );
	Projection.Add_Child(SG_T("PROJ4")  , m_Proj4);
	Projection.Add_Child(SG_T("EPSG")   , m_EPSG );

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::is_Equal(const CSG_Projection &Projection)	const
{
	return(	m_Proj4 == Projection.m_Proj4 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_PROJ_FIELD_ID
{
	PRJ_FIELD_SRID		= 0,
	PRJ_FIELD_AUTH_NAME,
	PRJ_FIELD_AUTH_SRID,
	PRJ_FIELD_SRTEXT,
	PRJ_FIELD_PROJ4TEXT
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Projections::CSG_Projections(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Projections::CSG_Projections(const CSG_String &File_Name)
{
	_On_Construction();

	Create(File_Name);
}

bool CSG_Projections::Create(const CSG_String &File_Name)
{
	CSG_Table	Projections(File_Name);

	return( Create(&Projections) );
}

//---------------------------------------------------------
CSG_Projections::CSG_Projections(CSG_Table *pProjections)
{
	_On_Construction();

	Create(pProjections);
}

bool CSG_Projections::Create(CSG_Table *pProjections)
{
	Destroy();

	if( !pProjections )
	{
		return( false );
	}

	for(int i=0; i<pProjections->Get_Count() && SG_UI_Process_Set_Progress(i, pProjections->Get_Count()); i++)
	{
		m_pProjections->Add_Record(pProjections->Get_Record(i));
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
void CSG_Projections::_On_Construction(void)
{
	m_pProjections	= new CSG_Table;

	m_pProjections->Add_Field(SG_T("srid")		, SG_DATATYPE_Int);
	m_pProjections->Add_Field(SG_T("auth_name")	, SG_DATATYPE_String);
	m_pProjections->Add_Field(SG_T("auth_srid")	, SG_DATATYPE_Int);
	m_pProjections->Add_Field(SG_T("srtext")	, SG_DATATYPE_String);
	m_pProjections->Add_Field(SG_T("proj4text")	, SG_DATATYPE_String);
}

//---------------------------------------------------------
CSG_Projections::~CSG_Projections(void)
{
	Destroy();

	delete(m_pProjections);
}

//---------------------------------------------------------
void CSG_Projections::Destroy(void)
{
	if( m_pProjections )
	{
		m_pProjections->Del_Records();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::Load(const CSG_String &File_Name)
{
	return( Create(File_Name) );
}

//---------------------------------------------------------
bool CSG_Projections::Save(const CSG_String &File_Name)
{
	CSG_Table	Table;

/*	Table.Add_Field(SG_T("srid")		, SG_DATATYPE_Int);
	Table.Add_Field(SG_T("auth_name")	, SG_DATATYPE_String);
	Table.Add_Field(SG_T("auth_srid")	, SG_DATATYPE_Int);
	Table.Add_Field(SG_T("srtext")		, SG_DATATYPE_String);
	Table.Add_Field(SG_T("proj4text")	, SG_DATATYPE_String);

	for(int i=0; i<Get_Count() && SG_UI_Process_Set_Progress(i, Get_Count()); i++)
	{
		CSG_Table_Record	*pRecord	= Table.Add_Record();

		pRecord->Set_Value(PRJ_FIELD_SRID		, Table.Get_Count());
		pRecord->Set_Value(PRJ_FIELD_AUTH_NAME	, m_pProjections[i]->Get_EPSG() > 0 ? SG_T("EPSG") : SG_T(""));
		pRecord->Set_Value(PRJ_FIELD_AUTH_SRID	, m_pProjections[i]->Get_EPSG());
		pRecord->Set_Value(PRJ_FIELD_SRTEXT		, m_pProjections[i]->Get_WKT());
		pRecord->Set_Value(PRJ_FIELD_PROJ4TEXT	, m_pProjections[i]->Get_Proj4());
	}
/**/
	return( Table.Save(File_Name) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Projections::Get_Count(void) const
{
	return( m_pProjections->Get_Count() );
}

//---------------------------------------------------------
bool CSG_Projections::Add(const CSG_Projection &Projection)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::Add(int SRID, const SG_Char *Authority, const SG_Char *WKT, const SG_Char *Proj4)
{
	CSG_Table_Record	*pProjection	= m_pProjections->Add_Record();

	pProjection->Set_Value(SG_T("srid")     , SRID);
	pProjection->Set_Value(SG_T("auth_name"), Authority);
	pProjection->Set_Value(SG_T("auth_srid"), SRID);
	pProjection->Set_Value(SG_T("srtext")   , WKT);
	pProjection->Set_Value(SG_T("proj4text"), Proj4);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Projections::Get_Names(void) const
{
	CSG_String	Names, WKT, Type;

	m_pProjections->Set_Index(PRJ_FIELD_SRTEXT, TABLE_INDEX_Ascending);

	for(int i=0; i<Get_Count(); i++)
	{
		WKT	= m_pProjections->Get_Record_byIndex(i)->asString(PRJ_FIELD_SRTEXT);

		     if( !WKT.BeforeFirst('[').Cmp(SG_T("PROJCS")) )
		{
			Type	= SG_Get_Projection_Type_Name(SG_PROJ_TYPE_CS_Projected);
		}
		else if( !WKT.BeforeFirst('[').Cmp(SG_T("GEOGCS")) )
		{
			Type	= SG_Get_Projection_Type_Name(SG_PROJ_TYPE_CS_Geographic);
		}
		else // if( !WKT.BeforeFirst('[').Cmp(SG_T("GEOCCS")) )
		{
			Type	= SG_Get_Projection_Type_Name(SG_PROJ_TYPE_CS_Geocentric);
		}

		Names	+= CSG_String::Format(SG_T("[%s] %s|"), Type.c_str(), WKT.AfterFirst('\"').BeforeFirst('\"').c_str());
	}

	return( Names );
}

//---------------------------------------------------------
int CSG_Projections::Get_SRID_byNamesIndex(int i) const
{
	if( i >= 0 && i < Get_Count() )
	{
		m_pProjections->Set_Index(PRJ_FIELD_SRTEXT, TABLE_INDEX_Ascending);

		return( m_pProjections->Get_Record_byIndex(i)->asInt(PRJ_FIELD_SRID) );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::WKT_to_Proj4(CSG_String &Proj4, const CSG_String &WKT)
{
	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::WKT_from_Proj4(CSG_String &Projection, const CSG_String &Proj4)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// obsolete: to be removed...

const CSG_Projection & CSG_Projections::Get_Projection(int i)	const
{
	static CSG_Projection	p;

	return( p );
}

const CSG_Projection & CSG_Projections::operator []	(int i) const
{
	return( Get_Projection(i) );
}

//---------------------------------------------------------
bool CSG_Projections::Get_Projection(int EPSG, CSG_Projection &Projection)	const
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
