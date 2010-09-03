
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
	Destroy();
}

CSG_Projection::~CSG_Projection(void)
{}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(const CSG_Projection &Projection)
{
	Destroy();

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
	Destroy();

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
	Destroy();

	Create(Projection, Format);
}

bool CSG_Projection::Create(const CSG_String &Projection, TSG_Projection_Format Format)
{
	return( Assign(Projection, Format) );
}

bool CSG_Projection::Assign(const CSG_String &Projection, TSG_Projection_Format Format)
{
	int				i;
	CSG_String		s;
	CSG_MetaData	m;

	Destroy();

	switch( Format )
	{
	default:
		return( false );

	case SG_PROJ_FMT_EPSG:
		return( Projection.asInt(i) && gSG_Projections.Get_Projection(*this, i) );

	case SG_PROJ_FMT_Proj4:
		if( !gSG_Projections.WKT_from_Proj4(s, Projection) )
		{
			return( false );
		}

		m		= gSG_Projections.WKT_to_MetaData(m_WKT);

		m_WKT	= s;
		m_Proj4	= Projection;

		break;

	case SG_PROJ_FMT_WKT:
		m		= gSG_Projections.WKT_to_MetaData(Projection);

		if(	m.Get_Property("authority_name", s) && s.CmpNoCase(SG_T("EPSG")) == 0
		&&	m.Get_Property("authority_code", i) && gSG_Projections.Get_Projection(*this, i) )
		{
			return( true );
		}

		if( gSG_Projections.WKT_to_Proj4(s, Projection) )
		{
			m_Proj4	= s;
		}

		m_WKT	= Projection;

		break;
	}

	//-----------------------------------------------------
	m_Name	= m.Get_Property("name");
	m_Type	= !m.Get_Name().Cmp(SG_T("GEOCCS")) ? SG_PROJ_TYPE_CS_Geocentric
			: !m.Get_Name().Cmp(SG_T("GEOGCS")) ? SG_PROJ_TYPE_CS_Geographic
			: !m.Get_Name().Cmp(SG_T("PROJCS")) ? SG_PROJ_TYPE_CS_Projected
			: SG_PROJ_TYPE_CS_Undefined;

	return( true );
}

//---------------------------------------------------------
void CSG_Projection::Destroy(void)
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

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Description(void)	const
{
	CSG_String	s;

	s	= SG_Get_Projection_Type_Name(m_Type);

	if( is_Okay() )
	{
		if( m_EPSG > 0 )
		{
			s	+= CSG_String::Format(SG_T(" [EPSG %d]"), m_EPSG);
		}

		s	+= SG_T(":\n") + m_Name;

		if( m_Proj4.Length() > 0 )
		{
			s	+= CSG_String::Format(SG_T("\n[%s]"), m_Proj4.c_str());
		}
	}

	return( s );
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
	SG_UI_Msg_Lock(true);

	bool	bResult	= Load(File_Name);

	SG_UI_Msg_Lock(false);

	return( bResult );
}

//---------------------------------------------------------
void CSG_Projections::_On_Construction(void)
{
	m_pProjections	= new CSG_Table;

	m_pProjections->Add_Field(SG_T("srid")		, SG_DATATYPE_Int);		// PRJ_FIELD_SRID
	m_pProjections->Add_Field(SG_T("auth_name")	, SG_DATATYPE_String);	// PRJ_FIELD_AUTH_NAME
	m_pProjections->Add_Field(SG_T("auth_srid")	, SG_DATATYPE_Int);		// PRJ_FIELD_AUTH_SRID
	m_pProjections->Add_Field(SG_T("srtext")	, SG_DATATYPE_String);	// PRJ_FIELD_SRTEXT
	m_pProjections->Add_Field(SG_T("proj4text")	, SG_DATATYPE_String);	// PRJ_FIELD_PROJ4TEXT
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
	Destroy();

	CSG_String	fName(File_Name);

	//-----------------------------------------------------
	SG_File_Set_Extension(fName, SG_T("srs"));

	CSG_Table	Projections;

	if( SG_File_Exists(fName) && Projections.Create(fName) )
	{
		Projections.Set_Index(PRJ_FIELD_SRTEXT, TABLE_INDEX_Ascending);

		for(int i=0; i<Projections.Get_Count() && SG_UI_Process_Set_Progress(i, Projections.Get_Count()); i++)
		{
			m_pProjections->Add_Record(Projections.Get_Record_byIndex(i));
		}
	}

	//-----------------------------------------------------
	SG_File_Set_Extension(fName, SG_T("dic"));

	if( SG_File_Exists(fName) )
	{
		m_WKT_to_Proj4.Create(fName, false, 1, 0, true);
		m_Proj4_to_WKT.Create(fName, false, 0, 1, true);
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Projections::Save(const CSG_String &File_Name)
{
	return( m_pProjections->Save(File_Name) );
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

	pProjection->Set_Value(PRJ_FIELD_SRID     , SRID);
	pProjection->Set_Value(PRJ_FIELD_AUTH_NAME, Authority);
	pProjection->Set_Value(PRJ_FIELD_AUTH_SRID, SRID);
	pProjection->Set_Value(PRJ_FIELD_SRTEXT   , WKT);
	pProjection->Set_Value(PRJ_FIELD_PROJ4TEXT, Proj4);

	return( true );
}

//---------------------------------------------------------
CSG_Projection CSG_Projections::Get_Projection(int Index)	const
{
	CSG_Projection	Projection;

	if( Index >= 0 && Index < m_pProjections->Get_Count() )
	{
		CSG_Table_Record	*pRecord	= m_pProjections->Get_Record(Index);

		Projection.m_EPSG	= pRecord->asInt   (PRJ_FIELD_AUTH_SRID);
		Projection.m_WKT	= pRecord->asString(PRJ_FIELD_SRTEXT   );
		Projection.m_Proj4	= pRecord->asString(PRJ_FIELD_PROJ4TEXT);

		CSG_MetaData	m	= WKT_to_MetaData(Projection.m_WKT);

		Projection.m_Name	= m.Get_Property("name");
		Projection.m_Type	= !m.Get_Name().Cmp(SG_T("GEOCCS")) ? SG_PROJ_TYPE_CS_Geocentric
							: !m.Get_Name().Cmp(SG_T("GEOGCS")) ? SG_PROJ_TYPE_CS_Geographic
							: !m.Get_Name().Cmp(SG_T("PROJCS")) ? SG_PROJ_TYPE_CS_Projected
							: SG_PROJ_TYPE_CS_Undefined;
	}

	return( Projection );
}

//---------------------------------------------------------
bool CSG_Projections::Get_Projection(CSG_Projection &Projection, int EPSG_Code)	const
{
	for(int i=0; i<m_pProjections->Get_Count(); i++)
	{
		if( m_pProjections->Get_Record(i)->asInt(PRJ_FIELD_AUTH_SRID) == EPSG_Code )
		{
			Projection	= Get_Projection(i);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::EPSG_to_Proj4(CSG_String &Proj4, int EPSG_Code) const
{
	for(int i=0; i<m_pProjections->Get_Count(); i++)
	{
		if( m_pProjections->Get_Record(i)->asInt(PRJ_FIELD_AUTH_SRID) == EPSG_Code )
		{
			Proj4	= m_pProjections->Get_Record(i)->asString(PRJ_FIELD_PROJ4TEXT);

			return( true );
		}
	}

	Proj4.Printf(SG_T("+init=epsg:%d"), EPSG_Code);

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::EPSG_to_WKT(CSG_String &WKT, int EPSG_Code) const
{
	for(int i=0; i<m_pProjections->Get_Count(); i++)
	{
		if( m_pProjections->Get_Record(i)->asInt(PRJ_FIELD_AUTH_SRID) == EPSG_Code )
		{
			WKT		= m_pProjections->Get_Record(i)->asString(PRJ_FIELD_SRTEXT);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Projections::Get_Names_List(TSG_Projection_Type Type) const
{
	TSG_Projection_Type	iType;
	CSG_String			Names, iWKT;

	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Table_Record	*pProjection	= m_pProjections->Get_Record(i);

		iWKT	= pProjection->asString(PRJ_FIELD_SRTEXT);

		iType	= !iWKT.BeforeFirst('[').Cmp(SG_T("PROJCS")) ? SG_PROJ_TYPE_CS_Projected
				: !iWKT.BeforeFirst('[').Cmp(SG_T("GEOGCS")) ? SG_PROJ_TYPE_CS_Geographic
				: !iWKT.BeforeFirst('[').Cmp(SG_T("GEOCCS")) ? SG_PROJ_TYPE_CS_Geocentric
				: SG_PROJ_TYPE_CS_Undefined;

		if( Type == SG_PROJ_TYPE_CS_Undefined )
		{
			Names	+= CSG_String::Format(SG_T("{%d}%s: %s|"),
				pProjection->asInt(PRJ_FIELD_AUTH_SRID),
				SG_Get_Projection_Type_Name(iType).c_str(),
				iWKT.AfterFirst('\"').BeforeFirst('\"').c_str()
			);
		}
		else if( Type == iType )
		{
			Names	+= CSG_String::Format(SG_T("{%d}%s|"),
				pProjection->asInt(PRJ_FIELD_AUTH_SRID),
				iWKT.AfterFirst('\"').BeforeFirst('\"').c_str()
			);
		}
	}

	return( Names );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_WKT_to_MetaData(CSG_MetaData &MetaData, const CSG_String &WKT)
{
	int			i, l;
	CSG_String	Key;
	CSG_Strings	Content;

	//-----------------------------------------------------
	Content.Add(SG_T(""));

	for(i=0, l=-1; l!=0 && i<(int)WKT.Length(); i++)
	{
		if( l < 0 )	// read key
		{
			switch( WKT[i] )
			{
			default:						Key		+= WKT[i];	break;
			case SG_T('['): case SG_T('('):	l		 = 1;		break;
			case SG_T(']'): case SG_T(')'):	return( false );
			}
		}
		else		// read content
		{
			bool	bAdd;

			switch( WKT[i] )
			{
			default:						bAdd	= true;		break;
			case SG_T('\"'):				bAdd	= false;	break;
			case SG_T('['): case SG_T('('):	bAdd	= ++l > 1;	break;
			case SG_T(']'): case SG_T(')'):	bAdd	= l-- > 1;	break;
			case SG_T(','):	if( !(bAdd = l > 1) )	Content.Add(SG_T(""));	break;
			}

			if( bAdd )
			{
				Content[Content.Get_Count() - 1]	+= WKT[i];
			}
		}
	}

	if( Key.Length() == 0 || Content[0].Length() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Key.Cmp(SG_T("AUTHORITY")) && Content.Get_Count() == 2 )		// AUTHORITY  ["<name>", "<code>"]
	{
		MetaData.Add_Property(SG_T("authority_name"), Content[0]);
		MetaData.Add_Property(SG_T("authority_code"), Content[1]);

		return( true );
	}

	CSG_MetaData	*pKey	= MetaData.Add_Child(Key);

	if(	(!Key.Cmp(SG_T("GEOCCS"))		&& Content.Get_Count() >= 4)	// GEOCCS     ["<name>", <datum>, <prime meridian>, <linear unit> {,<axis>, <axis>, <axis>} {,<authority>}]
	||	(!Key.Cmp(SG_T("GEOGCS"))		&& Content.Get_Count() >= 4)	// GEOGCS     ["<name>", <datum>, <prime meridian>, <angular unit> {,<twin axes>} {,<authority>}]
	||	(!Key.Cmp(SG_T("PROJCS"))		&& Content.Get_Count() >= 3)	// PROJCS     ["<name>", <geographic cs>, <projection>, {<parameter>,}* <linear unit> {,<twin axes>}{,<authority>}]
	||	(!Key.Cmp(SG_T("DATUM"))		&& Content.Get_Count() >= 2) )	// DATUM      ["<name>", <spheroid> {,<to wgs84>} {,<authority>}]
	{
		pKey->Add_Property(SG_T("name"), Content[0]);
	}

	if(	(!Key.Cmp(SG_T("PRIMEM"))		&& Content.Get_Count() >= 2)	// PRIMEM     ["<name>", <longitude> {,<authority>}]
	||	(!Key.Cmp(SG_T("UNIT"))			&& Content.Get_Count() >= 2)	// UNIT       ["<name>", <conversion factor> {,<authority>}]
	||	(!Key.Cmp(SG_T("AXIS"))			&& Content.Get_Count() >= 2)	// AXIS       ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	||	(!Key.Cmp(SG_T("PARAMETER"))	&& Content.Get_Count() >= 2) )	// PARAMETER  ["<name>", <value>]
	{
		pKey->Add_Property(SG_T("name"), Content[0]);
		pKey->Set_Content(Content[1]);
	}

	if( (!Key.Cmp(SG_T("SPHEROID"))		&& Content.Get_Count() >= 3) )	// SPHEROID   ["<name>", <semi-major axis>, <inverse flattening> {,<authority>}]
	{
		pKey->Add_Property(SG_T("name"), Content[0]);
		pKey->Add_Child   (SG_T("a")   , Content[1]);
		pKey->Add_Child   (SG_T("rf")  , Content[2]);
	}

	if( (!Key.Cmp(SG_T("TOWGS84"))		&& Content.Get_Count() >= 7) )	// TOWGS84    [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
	{
		pKey->Add_Child(SG_T("dx")     , Content[0]);
		pKey->Add_Child(SG_T("dy")     , Content[1]);
		pKey->Add_Child(SG_T("dz")     , Content[2]);
		pKey->Add_Child(SG_T("ex")     , Content[3]);
		pKey->Add_Child(SG_T("ey")     , Content[4]);
		pKey->Add_Child(SG_T("ez")     , Content[5]);
		pKey->Add_Child(SG_T("ppm")    , Content[6]);
	}

	if( (!Key.Cmp(SG_T("PROJECTION"))	&& Content.Get_Count() >= 1) )	// PROJECTION ["<name>" {,<authority>}]
	{
		pKey->Set_Content(Content[0]);
	}

	//-----------------------------------------------------
	for(i=0; i<Content.Get_Count(); i++)
	{
		_WKT_to_MetaData(*pKey, Content[i]);
	}

	return( true );
}

//---------------------------------------------------------
CSG_MetaData CSG_Projections::WKT_to_MetaData(const CSG_String &WKT)
{
	CSG_MetaData	MetaData;

	_WKT_to_MetaData(MetaData, WKT);

	if( MetaData.Get_Children_Count() == 1 )
	{
		return( *MetaData.Get_Child(0) );
	}

	MetaData.Destroy();

	return( MetaData );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::WKT_to_Proj4(CSG_String &Proj4, const CSG_String &WKT) const
{
	CSG_MetaData	m	= WKT_to_MetaData(WKT);

//	m.Save("e:/wkt_metadata.xml");

	if( m.Get_Children_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			Authority_Code;
	CSG_String	Authority_Name;

	if(	m.Get_Property("authority_name", Authority_Name) && Authority_Name.CmpNoCase(SG_T("EPSG")) == 0
	&&	m.Get_Property("authority_code", Authority_Code) && EPSG_to_Proj4(Proj4, Authority_Code) )
	{	//	Proj4.Printf(SG_T("+init=epsg:%d"), Authority_Code);
		return( true );
	}

	//-----------------------------------------------------
	int			i;
	double		a, d;
	CSG_String	s;

	//-----------------------------------------------------
	if( !m.Get_Name().Cmp(SG_T("GEOCCS")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	// GEOGCS["<name>",
	//    DATUM  ["<name>",
	//        SPHEROID["<name>", <semi-major axis>, <inverse flattening>],
	//       *TOWGS84 [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
	//    ],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]
	if( !m.Get_Name().Cmp(SG_T("GEOGCS")) )
	{
		if(	!m["DATUM"].is_Valid()
		||	!m["DATUM"]["SPHEROID"].is_Valid()
		||	 m["DATUM"]["SPHEROID"].Get_Children_Count() != 2
		||	!m["DATUM"]["SPHEROID"][0].Get_Content().asDouble(a) || a <= 0.0
		||	!m["DATUM"]["SPHEROID"][1].Get_Content().asDouble(d) || d <= 0.0 )
		{
			return( false );
		}

		Proj4	+= CSG_String::Format(SG_T( "+proj=longlat"));

		Proj4	+= CSG_String::Format(SG_T(" +a=%f"), a);			// Semimajor radius of the ellipsoid axis
		Proj4	+= CSG_String::Format(SG_T(" +b=%f"), a - a / d);	// Semiminor radius of the ellipsoid axis

		if(	m["DATUM"]["TOWGS84"].is_Valid() && m["DATUM"]["TOWGS84"].Get_Children_Count() == 7 )
		{
			for(i=0; i<7; i++)
			{
				Proj4	+= (i == 0 ? SG_T(" +towgs84=") : SG_T(",")) + m["DATUM"]["TOWGS84"][i].Get_Content();
			}
		}

		if( m["PRIMEM"].is_Valid() && m["PRIMEM"].Get_Content().asDouble(d) && d != 0.0 )
		{
			Proj4	+= CSG_String::Format(SG_T(" +pm=%f"), d);
		}
	}

	//-----------------------------------------------------
	// PROJCS["<name>",
	//     GEOGCS["<name>",
	//           DATUM  ["<name>",
	//                   SPHEROID["<name>", <semi-major axis>, <inverse flattening>],
	//                  *TOWGS84 [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
	//           ],
	//           PRIMEM ["<name>", <longitude>],
	//           UNIT   ["<name>", <conversion factor>],
	//          *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//          *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	//     ],
	//     PROJECTION["<name>"],
	//    *PARAMETER ["<name>", <value>], ...
	//     UNIT      ["<name>", <conversion factor>],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]
	if( !m.Get_Name().Cmp(SG_T("PROJCS")) )
	{
		if( !m["PROJECTION"].is_Valid() || !m_WKT_to_Proj4.Get_Translation(m["PROJECTION"].Get_Content(), s)
		||	!m["GEOGCS"].is_Valid()
		||	!m["GEOGCS"]["DATUM"].is_Valid()
		||	!m["GEOGCS"]["DATUM"]["SPHEROID"].is_Valid()
		||	 m["GEOGCS"]["DATUM"]["SPHEROID"].Get_Children_Count() != 2
		||	!m["GEOGCS"]["DATUM"]["SPHEROID"][0].Get_Content().asDouble(a) || a <= 0.0
		||	!m["GEOGCS"]["DATUM"]["SPHEROID"][1].Get_Content().asDouble(d) || d <= 0.0 )
		{
			return( false );
		}

		Proj4	+= CSG_String::Format(SG_T( "+proj=%s"), s.c_str());

		Proj4	+= CSG_String::Format(SG_T(" +a=%f"), a);			// Semimajor radius of the ellipsoid axis
		Proj4	+= CSG_String::Format(SG_T(" +b=%f"), a - a / d);	// Semiminor radius of the ellipsoid axis

		if(	m["GEOGCS"]["DATUM"]["TOWGS84"].is_Valid() && m["GEOGCS"]["DATUM"]["TOWGS84"].Get_Children_Count() == 7 )
		{
			for(i=0; i<7; i++)
			{
				Proj4	+= (i == 0 ? SG_T(" +towgs84=") : SG_T(",")) + m["GEOGCS"]["DATUM"]["TOWGS84"][i].Get_Content();
			}
		}

		if( m["PRIMEM"].is_Valid() && m["PRIMEM"].Get_Content().asDouble(d) && d != 0.0 )
		{
			Proj4	+= CSG_String::Format(SG_T(" +pm=%f"), d);
		}

		for(i=0; i<m.Get_Children_Count(); i++)
		{
			if( !m[i].Get_Name().Cmp(SG_T("PARAMETER")) && m_WKT_to_Proj4.Get_Translation(m[i].Get_Property("name"), s) )
			{
				Proj4	+= CSG_String::Format(SG_T(" +%s=%s"), s.c_str(), m[i].Get_Content().c_str());
			}
		}

		if( m["UNIT"].is_Valid() && m["UNIT"].Get_Content().asDouble(d) && d != 0.0 && d != 1.0 )
		{
			Proj4	+= CSG_String::Format(SG_T(" +to_meter=%f"), d);
		}
	}

	//-----------------------------------------------------
	Proj4	+= CSG_String::Format(SG_T(" +no_defs"));	// Don't use the /usr/share/proj/proj_def.dat defaults file

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Read_Parameter(CSG_String &Value, const CSG_String &Proj4, const CSG_String &Key) const
{
	Value.Clear();

	int		i	= Proj4.Find(CSG_String::Format(SG_T("+%s="), Key.c_str()));

	if( i >= 0 )
	{
		for(int l=0; l<2 && i<(int)Proj4.Length(); i++)
		{
			switch( Proj4[i] )
			{
			case SG_T('='):	l++;	break;
			case SG_T('+'):	l=2;	break;
			case SG_T(' '):			break;
			default:	Value	+= Proj4[i];
			}
		}
	}

	return( Value.Length() > 0 );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Ellipsoid(CSG_String &Value, const CSG_String &Proj4) const
{
	const char	ellipsoid[42][2][32]	= 
	{	//  ellipsoid	      a				   b
		{	"MERIT"		, "6378137.0,298.257"		},	// MERIT 1983
		{	"SGS85"		, "6378136.0,298.257"		},	// Soviet Geodetic System 85
		{	"GRS80"		, "6378137.0,298.2572221"	},	// GRS 1980 (IUGG, 1980)
		{	"IAU76"		, "6378140.0,298.257"		},	// IAU 1976
		{	"airy"		, "6377563.396,299.3249753"	},	// Airy 1830
		{	"APL4.9"	, "6378137.0,298.25"		},	// Appl. Physics. 1965
		{	"NWL9D"		, "6378145.0,298.25"		},	// Naval Weapons Lab., 1965
		{	"mod_airy"	, "6377340.189,299.3249374"	},	// Modified Airy
		{	"andrae"	, "6377104.43,300"			},	// Andrae 1876 (Den., Iclnd.)
		{	"aust_SA"	, "6378160.0,298.25"		},	// Australian Natl & S. Amer. 1969
		{	"GRS67"		, "6378160.0,298.2471674"	},	// GRS 67 (IUGG 1967)
		{	"bessel"	, "6377397.155,299.1528128"	},	// Bessel 1841
		{	"bess_nam"	, "6377483.865,299.1528128"	},	// Bessel 1841 (Namibia)
		{	"clrk66"	, "6378206.4,294.9786982"	},	// Clarke 1866
		{	"clrk80"	, "6378249.145,293.4663"	},	// Clarke 1880 mod.
		{	"CPM"		, "6375738.7,334.29"		},	// Comm. des Poids et Mesures 1799
		{	"delmbr"	, "6376428.0,311.5"			},	// Delambre 1810 (Belgium)
		{	"engelis"	, "6378136.05,298.2566"		},	// Engelis 1985
		{	"evrst30"	, "6377276.345,300.8017"	},	// Everest 1830
		{	"evrst48"	, "6377304.063,300.8017"	},	// Everest 1948
		{	"evrst56"	, "6377301.243,300.8017"	},	// Everest 1956
		{	"evrst69"	, "6377295.664,300.8017"	},	// Everest 1969
		{	"evrstSS"	, "6377298.556,300.8017"	},	// Everest (Sabah & Sarawak)
		{	"fschr60"	, "6378166.0,298.3"			},	// Fischer (Mercury Datum) 1960
		{	"fschr60m"	, "6378155.0,298.3"			},	// Modified Fischer 1960
		{	"fschr68"	, "6378150.0,298.3"			},	// Fischer 1968
		{	"helmert"	, "6378200.0,298.3"			},	// Helmert 1906
		{	"hough"		, "6378270.0,297"			},	// Hough
		{	"intl"		, "6378388.0,297"			},	// International 1909 (Hayford)
		{	"krass"		, "6378245.0,298.3"			},	// Krassovsky, 1942
		{	"kaula"		, "6378163.0,298.24"		},	// Kaula 1961
		{	"lerch"		, "6378139.0,298.257"		},	// Lerch 1979
		{	"mprts"		, "6397300.0,191"			},	// Maupertius 1738
		{	"new_intl"	, "6378157.5,298.2496154"	},	// New International 1967
		{	"plessis"	, "6376523.0,308.6409971"	},	// Plessis 1817 (France)
		{	"SEasia"	, "6378155.0,298.3000002"	},	// Southeast Asia
		{	"walbeck"	, "6376896.0,302.7800002"	},	// Walbeck
		{	"WGS60"		, "6378165.0,298.3"			},	// WGS 60
		{	"WGS66"		, "6378145.0,298.25"		},	// WGS 66
		{	"WGS72"		, "6378135.0,298.26"		},	// WGS 72
		{	"WGS84"		, "6378137.0,298.2572236"	},	// WGS 84
		{	"sphere"	, "6370997.0,-1"			}	// Normal Sphere (r=6370997)
	};

	//-----------------------------------------------------
	if( _Proj4_Read_Parameter(Value, Proj4, "ellps") )
	{
		for(int i=0; i<42; i++)
		{
			if( !Value.CmpNoCase(ellipsoid[i][0]) )
			{
				Value.Printf(SG_T("SPHEROID[\"%s\",%s]"), SG_STR_MBTOSG(ellipsoid[i][0]), SG_STR_MBTOSG(ellipsoid[i][1]));

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	double	a, b;

	a	= _Proj4_Read_Parameter(Value, Proj4, "a" ) && Value.asDouble(a) ? a : 6378137.0;

	b	= _Proj4_Read_Parameter(Value, Proj4, "b" ) && Value.asDouble(b) ? a / (a - b)
		: _Proj4_Read_Parameter(Value, Proj4, "rf") && Value.asDouble(b) ? b
		: _Proj4_Read_Parameter(Value, Proj4, "f" ) && Value.asDouble(b) ? 1.0 / b
		: _Proj4_Read_Parameter(Value, Proj4, "e" ) && Value.asDouble(b) ? a / (a - sqrt(b*b - a*a))
		: _Proj4_Read_Parameter(Value, Proj4, "es") && Value.asDouble(b) ? a / (a - sqrt( b  - a*a))
		: 298.2572236;

	Value	= CSG_String::Format(SG_T("SPHEROID[\"Ellipsoid\",%f,%f]"), a, b);

	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Datum(CSG_String &Value, const CSG_String &Proj4) const
{
	const char	datum[9][3][64]	=
	{	//	datum_id		  ellipse		  definition
		{	"WGS84"			, "WGS84"		, "0,0,0,0,0,0,0"											},
		{	"GGRS87"		, "GRS80"		, "-199.87,74.79,246.62,0,0,0,0"							},	// Greek_Geodetic_Reference_System_1987
		{	"NAD83"			, "GRS80"		, "0,0,0,0,0,0,0"											},	// North_American_Datum_1983
	//	{	"NAD27"			, "clrk66"		, "nadgrids=@conus,@alaska,@ntv2_0.gsb,@ntv1_can.dat"		},	// North_American_Datum_1927
		{	"potsdam"		, "bessel"		, "606.0,23.0,413.0,0,0,0,0"								},	// Potsdam Rauenberg 1950 DHDN
		{	"carthage"		, "clark80"		, "-263.0,6.0,431.0,0,0,0,0"								},	// Carthage 1934 Tunisia
		{	"hermannskogel"	, "bessel"		, "653.0,-212.0,449.0,0,0,0,0"								},	// Hermannskogel
		{	"ire65"			, "mod_airy"	, "482.530,-130.596,564.557,-1.042,-0.214,-0.631,8.15"		},	// Ireland 1965
		{	"nzgd49"		, "intl"		, "59.47,-5.04,187.44,0.47,-0.1,1.024,-4.5993"				},	// New Zealand Geodetic Datum 1949
		{	"OSGB36"		, "airy"		, "446.448,-125.157,542.060,0.1502,0.2470,0.8421,-20.4894"	}	// Airy 1830
	};

	CSG_String	Spheroid, ToWGS84;

	//-----------------------------------------------------
	if( _Proj4_Read_Parameter(Value, Proj4, "datum") )
	{
		for(int i=0; i<9; i++)
		{
			if( !Value.CmpNoCase(datum[i][0]) && _Proj4_Get_Ellipsoid(Spheroid, CSG_String::Format(SG_T("+ellps=%s"), SG_STR_MBTOSG(datum[i][1]))) )
			{
				Value.Printf(SG_T("DATUM[\"%s\",%s,TOWGS84[%s]]"), SG_STR_MBTOSG(datum[i][0]), Spheroid.c_str(), SG_STR_MBTOSG(datum[i][2]));

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	if( _Proj4_Get_Ellipsoid(Spheroid, Proj4) )
	{
		Value	 = CSG_String::Format(SG_T("DATUM[\"Datum\",%s"), Spheroid.c_str());

		if( _Proj4_Read_Parameter(ToWGS84, Proj4, "towgs84") )
		{
			Value	+= CSG_String::Format(SG_T(",TOWGS84[%s]"), ToWGS84.c_str());
		}

		Value	+= SG_T("]");
	}

	//-----------------------------------------------------
	Value	= SG_T("DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563],TOWGS84[0,0,0,0,0,0,0]]");

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Prime_Meridian(CSG_String &Value, const CSG_String &Proj4) const
{
	const char	meridian[12][2][16]	=
	{
		{	"lisbon"	, "-9.131906111"	},
		{	"paris"		, "2.337229167"		},
		{	"bogota"	, "74.08091667"		},
		{	"madrid"	, "-3.687911111"	},
		{	"rome"		, "12.45233333"		},
		{	"bern"		, "7.439583333"		},
		{	"jakarta"	, "106.8077194"		},
		{	"ferro"		, "-17.66666667"	},
		{	"brussels"	, "4.367975"		},
		{	"stockholm"	, "18.05827778"		},
		{	"athens"	, "23.7163375"		},
		{	"oslo"		, "10.72291667"		}
	};

	//-----------------------------------------------------
	if( _Proj4_Read_Parameter(Value, Proj4, "pm") )
	{
		for(int i=0; i<12; i++)
		{
			if( !Value.CmpNoCase(meridian[i][0]) )
			{
				Value.Printf(SG_T("PRIMEM[\"%s\",%s]"), SG_STR_MBTOSG(meridian[i][0]), SG_STR_MBTOSG(meridian[i][1]));

				return( true );
			}
		}

		double	d;

		if( Value.asDouble(d) && d != 0.0 )
		{
			Value.Printf(SG_T("PRIMEM[\"Prime_Meridian\",%f]"), d);

			return( true );
		}
	}

	//-----------------------------------------------------
	Value	= SG_T("PRIMEM[\"Greenwich\",0]");

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Unit(CSG_String &Value, const CSG_String &Proj4) const
{
	const char	unit[21][3][32]	=
	{
		{	"km"		,"1000.0"				, "Kilometer"						},
		{	"m"			,"1.0"					, "Meter"							},
		{	"dm"		,"0.1"					, "Decimeter"						},
		{	"cm"		,"0.01"					, "Centimeter"						},
		{	"mm"		,"0.001"				, "Millimeter"						},
		{	"kmi"		,"1852.0"				, "International Nautical Mile"		},
		{	"in"		,"0.0254"				, "International Inch"				},
		{	"ft"		,"0.3048"				, "International Foot"				},
		{	"yd"		,"0.9144"				, "International Yard"				},
		{	"mi"		,"1609.344"				, "International Statute Mile"		},
		{	"fath"		,"1.8288"				, "International Fathom"			},
		{	"ch"		,"20.1168"				, "International Chain"				},
		{	"link"		,"0.201168"				, "International Link"				},
		{	"us-in"		,"1./39.37"				, "U.S. Surveyor's Inch"			},
		{	"us-ft"		,"0.304800609601219"	, "U.S. Surveyor's Foot"			},
		{	"us-yd"		,"0.914401828803658"	, "U.S. Surveyor's Yard"			},
		{	"us-ch"		,"20.11684023368047"	, "U.S. Surveyor's Chain"			},
		{	"us-mi"		,"1609.347218694437"	, "U.S. Surveyor's Statute Mile"	},
		{	"ind-yd"	,"0.91439523"			, "Indian Yard"						},
		{	"ind-ft"	,"0.30479841"			, "Indian Foot"						},
		{	"ind-ch"	,"20.11669506"			, "Indian Chain"					}
	};

	//-----------------------------------------------------
	if( _Proj4_Read_Parameter(Value, Proj4, "units") )
	{
		for(int i=0; i<21; i++)
		{
			if( !Value.CmpNoCase(unit[i][0]) )
			{
				Value.Printf(SG_T("UNIT[\"%s\",%s]"), SG_STR_MBTOSG(unit[i][2]), SG_STR_MBTOSG(unit[i][1]));

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	double	 d;

	if( _Proj4_Read_Parameter(Value, Proj4, "to_meter") && Value.asDouble(d) && d > 0.0 && d != 1.0 )
	{
		Value.Printf(SG_T("UNIT[\"Unit\",%f]"), d);

		return( true );
	}

	//-----------------------------------------------------
	Value	= SG_T("UNIT[\"metre\",1");

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::WKT_from_Proj4(CSG_String &WKT, const CSG_String &Proj4) const
{
	CSG_String	Value, GeogCS;

	//-----------------------------------------------------
	if( !_Proj4_Read_Parameter(Value, Proj4, "proj") )
	{
		return( false );
	}

	//-----------------------------------------------------
	// GEOGCS["<name>",
	//    DATUM  ["<name>",
	//        SPHEROID["<name>", <semi-major axis>, <inverse flattening>],
	//       *TOWGS84 [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
	//    ],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]

	GeogCS	 = SG_T("GEOGCS[\"GCS\",");

	_Proj4_Get_Datum			(Value, Proj4);	GeogCS	+= Value;	GeogCS	+= SG_T(",");
	_Proj4_Get_Prime_Meridian	(Value, Proj4);	GeogCS	+= Value;	GeogCS	+= SG_T(",");

	GeogCS	+= SG_T("UNIT[\"degree\",0.01745329251994328]]");

	//-----------------------------------------------------
	_Proj4_Read_Parameter(Value, Proj4, "proj");

	if(	!Value.CmpNoCase("lonlat") || !Value.CmpNoCase("longlat")
	||	!Value.CmpNoCase("latlon") || !Value.CmpNoCase("latlong") )
	{
		WKT	= GeogCS;

		return( true );
	}

	//-----------------------------------------------------
	// PROJCS["<name>",
	//     GEOGCS    [ ...... ],
	//     PROJECTION["<name>"],
	//    *PARAMETER ["<name>", <value>], ...
	//     UNIT      ["<name>", <conversion factor>],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]

	if( !m_Proj4_to_WKT.Get_Translation(Value, Value) )
	{
		return( false );
	}

	WKT		 = CSG_String::Format(SG_T("PROJCS[\"%s\",%s,PROJECTION[%s],"), Value.c_str(), GeogCS.c_str(), Value.c_str());

	//-----------------------------------------------------
	// Parameters ...

	//-----------------------------------------------------
	_Proj4_Get_Unit(Value, Proj4);

	WKT		+= SG_T("]");

	return( true );
}

/*/ General Parameters
	+a         Semimajor radius of the ellipsoid axis
	+alpha     ? Used with Oblique Mercator and possibly a few others
	+axis      Axis orientation (new in 4.8.0)
	+b         Semiminor radius of the ellipsoid axis
	+datum     Datum name (see `proj -ld`)
	+ellps     Ellipsoid name (see `proj -le`)
	+k         Scaling factor (old name)
	+k_0       Scaling factor (new name)
	+lat_0     Latitude of origin
	+lat_1     Latitude of first standard parallel
	+lat_2     Latitude of second standard parallel
	+lat_ts    Latitude of true scale
	+lon_0     Central meridian
	+lonc      ? Longitude used with Oblique Mercator and possibly a few others
	+lon_wrap  Center longitude to use for wrapping (see below)
	+nadgrids  Filename of NTv2 grid file to use for datum transforms (see below)
	+no_defs   Don't use the /usr/share/proj/proj_def.dat defaults file
	+over      Allow longitude output outside -180 to 180 range, disables wrapping (see below)
	+pm        Alternate prime meridian (typically a city name, see below)
	+proj      Projection name (see `proj -l`)
	+south     Denotes southern hemisphere UTM zone
	+to_meter  Multiplier to convert map units to 1.0m
	+towgs84   3 or 7 term datum transform parameters (see below)
	+units     meters, US survey feet, etc.
	+x_0       False easting
	+y_0       False northing
	+zone      UTM zone
/**/

/*/ ellipsoids (a, b)
	{	"MERIT"		, "6378137.0"	, "6356752.298"	},	// MERIT 1983
	{	"SGS85"		, "6378136.0"	, "6356751.302"	},	// Soviet Geodetic System 85
	{	"GRS80"		, "6378137.0"	, "6356752.314"	},	// GRS 1980 (IUGG, 1980)
	{	"IAU76"		, "6378140.0"	, "6356755.288"	},	// IAU 1976
	{	"airy"		, "6377563.396"	, "6356256.91"	},	// Airy 1830
	{	"APL4.9"	, "6378137.0"	, "6356751.796"	},	// Appl. Physics. 1965
	{	"NWL9D"		, "6378145.0"	, "6356759.769"	},	// Naval Weapons Lab., 1965
	{	"mod_airy"	, "6377340.189"	, "6356034.446"	},	// Modified Airy
	{	"andrae"	, "6377104.43"	, "6355847.415"	},	// Andrae 1876 (Den., Iclnd.)
	{	"aust_SA"	, "6378160.0"	, "6356774.719"	},	// Australian Natl & S. Amer. 1969
	{	"GRS67"		, "6378160.0"	, "6356774.516"	},	// GRS 67 (IUGG 1967)
	{	"bessel"	, "6377397.155"	, "6356078.963"	},	// Bessel 1841
	{	"bess_nam"	, "6377483.865"	, "6356165.383"	},	// Bessel 1841 (Namibia)
	{	"clrk66"	, "6378206.4"	, "6356583.8"	},	// Clarke 1866
	{	"clrk80"	, "6378249.145"	, "6356514.966"	},	// Clarke 1880 mod.
	{	"CPM"		, "6375738.7"	, "6356666.222"	},	// Comm. des Poids et Mesures 1799
	{	"delmbr"	, "6376428.0"	, "6355957.926"	},	// Delambre 1810 (Belgium)
	{	"engelis"	, "6378136.05"	, "6356751.323"	},	// Engelis 1985
	{	"evrst30"	, "6377276.345"	, "6356075.413"	},	// Everest 1830
	{	"evrst48"	, "6377304.063"	, "6356103.039"	},	// Everest 1948
	{	"evrst56"	, "6377301.243"	, "6356100.228"	},	// Everest 1956
	{	"evrst69"	, "6377295.664"	, "6356094.668"	},	// Everest 1969
	{	"evrstSS"	, "6377298.556"	, "6356097.55"	},	// Everest (Sabah & Sarawak)
	{	"fschr60"	, "6378166.0"	, "6356784.284"	},	// Fischer (Mercury Datum) 1960
	{	"fschr60m"	, "6378155.0"	, "6356773.32"	},	// Modified Fischer 1960
	{	"fschr68"	, "6378150.0"	, "6356768.337"	},	// Fischer 1968
	{	"helmert"	, "6378200.0"	, "6356818.17"	},	// Helmert 1906
	{	"hough"		, "6378270.0"	, "6356794.343"	},	// Hough
	{	"intl"		, "6378388.0"	, "6356911.946"	},	// International 1909 (Hayford)
	{	"krass"		, "6378245.0"	, "6356863.019"	},	// Krassovsky, 1942
	{	"kaula"		, "6378163.0"	, "6356776.992"	},	// Kaula 1961
	{	"lerch"		, "6378139.0"	, "6356754.292"	},	// Lerch 1979
	{	"mprts"		, "6397300.0"	, "6363806.283"	},	// Maupertius 1738
	{	"new_intl"	, "6378157.5"	, "6356772.2"	},	// New International 1967
	{	"plessis"	, "6376523.0"	, "6355863"		},	// Plessis 1817 (France)
	{	"SEasia"	, "6378155.0"	, "6356773.321"	},	// Southeast Asia
	{	"walbeck"	, "6376896.0"	, "6355834.847"	},	// Walbeck
	{	"WGS60"		, "6378165.0"	, "6356783.287"	},	// WGS 60
	{	"WGS66"		, "6378145.0"	, "6356759.769"	},	// WGS 66
	{	"WGS72"		, "6378135.0"	, "6356750.52"	},	// WGS 72
	{	"WGS84"		, "6378137.0"	, "6356752.314"	},	// WGS 84
	{	"sphere"	, "6370997.0"	, "6370997.0"	}	// Normal Sphere (r=6370997)
/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
