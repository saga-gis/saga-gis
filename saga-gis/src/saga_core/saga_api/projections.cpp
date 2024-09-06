
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "geo_tools.h"

#include "table.h"
#include "shapes.h"
#include "data_manager.h"
#include "tool_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Projections gSG_Projections;

//---------------------------------------------------------
CSG_Projections & SG_Get_Projections(void)
{
	return( gSG_Projections );
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
	Create(Projection);
}

bool CSG_Projection::Create(const CSG_Projection &Projection)
{
	m_Name      = Projection.m_Name;
	m_Type      = Projection.m_Type;
	m_Unit      = Projection.m_Unit;

	m_WKT1      = Projection.m_WKT1;
	m_WKT2      = Projection.m_WKT2;
	m_PROJ      = Projection.m_PROJ;
	m_ESRI      = Projection.m_ESRI;
	m_Authority = Projection.m_Authority;
	m_Code      = Projection.m_Code;

	return( true );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(const char *Definition)
{
	Create(Definition);
}

bool CSG_Projection::Create(const char *Definition)
{
	if( Definition && *Definition)
	{
		return( Create(CSG_String(Definition)) );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(const wchar_t *Definition)
{
	Create(Definition);
}

bool CSG_Projection::Create(const wchar_t *Definition)
{
	if( Definition && *Definition )
	{
		return( Create(CSG_String(Definition)) );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(const CSG_String &Definition)
{
	Create(Definition);
}

bool CSG_Projection::Create(const CSG_String &Definition)
{
	Destroy();

	if( CSG_Projections::Parse(Definition, &m_WKT1, &m_WKT2, &m_PROJ, &m_ESRI) )
	{
		CSG_MetaData WKT(CSG_Projections::_WKT_to_MetaData(m_WKT1));

		m_Type = CSG_Projections::Get_CRS_Type(WKT.Get_Name());

		m_Name = WKT.Get_Property("name");

		if( WKT("UNIT") && WKT["UNIT"].Get_Property("name") )
		{
			m_Unit = CSG_Projections::Get_Unit(WKT["UNIT"].Get_Property("name"));
		}

		CSG_String Authority; int Code;

		if(	WKT.Get_Property("authority_name", Authority) && WKT.Get_Property("authority_code", Code) )
		{
			m_Authority = Authority; m_Code = Code;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(int Code, const SG_Char *Authority)
{
	Create(Code, Authority);
}

bool CSG_Projection::Create(int Code, const SG_Char *Authority)
{
	if( Create(CSG_String::Format("%s:%d", Authority && *Authority ? Authority : SG_T("EPSG"), Code)) )
	{
		return( true );
	}

	return( Create(gSG_Projections.Get_Projection(Code, Authority)) ); // request SAGA's internal CRS database (might provide definitions not included in PROJ's default database)
}

//---------------------------------------------------------
void CSG_Projection::Destroy(void)
{
	m_Name      = _TL("undefined");
	m_Type      = ESG_CRS_Type::Undefined;
	m_Unit      = ESG_Projection_Unit::Undefined;

	m_WKT1      .Clear();
	m_WKT2      .Clear();
	m_PROJ      .Clear();
	m_ESRI      .Clear();
	m_Authority .Clear();
	m_Code      = -1;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::Load(const CSG_String &FileName)
{
	CSG_File Stream(FileName, SG_FILE_R, false);

	return( Load(Stream) );
}

//---------------------------------------------------------
bool CSG_Projection::Save(const CSG_String &FileName, ESG_CRS_Format Format) const
{
	if( is_Okay() )
	{
		CSG_File Stream(FileName, SG_FILE_W, false);

		return( Save(Stream, Format) );
	}

	if( SG_File_Exists(FileName) )
	{
		SG_File_Delete(FileName);
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Load(CSG_File &Stream)
{
	if( Stream.is_Reading() )
	{
		CSG_String s; Stream.Read(s, (size_t)Stream.Length());

		return( Create(s) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Save(CSG_File &Stream, ESG_CRS_Format Format)	const
{
	if( is_Okay() && Stream.is_Writing() )
	{
		switch( Format )
		{
		default:
		case ESG_CRS_Format::WKT1: return( !m_WKT1.is_Empty() && Stream.Write(m_WKT1) == m_WKT1.Length() );
		case ESG_CRS_Format::WKT2: return( !m_WKT2.is_Empty() && Stream.Write(m_WKT2) == m_WKT2.Length() );
		case ESG_CRS_Format::PROJ: return( !m_PROJ.is_Empty() && Stream.Write(m_PROJ) == m_PROJ.Length() );
		case ESG_CRS_Format::ESRI: return( !m_ESRI.is_Empty() && Stream.Write(m_ESRI) == m_ESRI.Length() );
		case ESG_CRS_Format::CODE: return( !m_Authority.is_Empty() && m_Code > 0 && Stream.Printf("%s:%d", m_Authority.c_str(), m_Code) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Load(const CSG_MetaData &Projection)
{
	if( Projection("WKT1") ) { return( Create(Projection["WKT1"].Get_Content()) ); }
	if( Projection("PROJ") ) { return( Create(Projection["PROJ"].Get_Content()) ); }

	//-----------------------------------------------------
	// >>> backward compatibilty

	if( Projection("OGC_WKT") ) { return( Create(Projection["OGC_WKT"].Get_Content()) ); }
	if( Projection("PROJ4"  ) ) { return( Create(Projection["PROJ4"  ].Get_Content()) ); }

	// <<< backward compatibilty
	//-----------------------------------------------------

	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::Save(CSG_MetaData &Projection) const
{
	Projection.Del_Children();

	Projection.Add_Child("WKT1", m_WKT1);
	Projection.Add_Child("PROJ", m_PROJ);
	Projection.Add_Child("CODE", m_Code)->Add_Property("authority", m_Authority);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Description(bool bDetails) const
{
	if( !is_Okay() )
	{
		return( _TL("Unknown Spatial Reference") );
	}

	CSG_MetaData WKT(CSG_Projections::_WKT_to_MetaData(m_WKT1)), *pGCS = NULL;

	if( !bDetails )
	{
		CSG_String s;
		
		if( !m_Name.is_Empty() && m_Name.CmpNoCase("unknown") )
		{
			s = m_Name;
		}
		else if( WKT("PROJECTION") )
		{
			s.Printf("%s [%s]", WKT.Get_Content("PROJECTION"), _TL("user defined")); s.Replace("_", " ");
		}
		else
		{
			s.Printf("[%s]", _TL("user defined"));
		}

		if( m_Code > 0 && !m_Authority.is_Empty() )
		{
			s += CSG_String::Format(" [%s:%d]", m_Authority.c_str(), m_Code);
		}

		return( s );
	}

	//-----------------------------------------------------
	#define ADD_HEAD(name, value) { CSG_String n(name), v(value); n.Replace("_", " "); v.Replace("_", " "); s += CSG_String::Format("<tr><th>%s</th><th>%s</th></tr>", n.c_str(), v.c_str()); }
	#define ADD_INFO(name, value) { CSG_String n(name), v(value); n.Replace("_", " "); v.Replace("_", " "); s += CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>", n.c_str(), v.c_str()); }
	#define ADD_CONT(name, entry)       if( entry                              ) { ADD_INFO(name, entry->Get_Content()     ); }
	#define ADD_PROP(name, entry, prop) if( entry && entry->Get_Property(prop) ) { ADD_INFO(name, entry->Get_Property(prop)); }

	CSG_String s = "<table border=\"1\">";

	if( is_Projection() )
	{
		ADD_HEAD(_TL("Projected Coordinate System" ), WKT.Get_Property("name") && !WKT.Cmp_Property("name", "unknown", true) ? WKT.Get_Property("name") : SG_T(""));
		ADD_CONT(_TL("Projection"                  ), WKT("PROJECTION"));
		if( m_Code > 0 && !m_Authority.is_Empty() )
		{
			ADD_INFO(_TL("Authority Code"          ), CSG_String::Format("%d", m_Code) );
			ADD_INFO(_TL("Authority"               ), m_Authority);
		}
		ADD_PROP(_TL("Linear Unit"                 ), WKT("UNIT"), "name");

		for(int i=0; i<WKT.Get_Children_Count(); i++)
		{
			if( WKT[i].Cmp_Name("PARAMETER") )
			{
				CSG_String Name(WKT[i].Get_Property("name")); Name.Replace("_", " ");
				ADD_INFO(Name.c_str(), WKT[i].Get_Content().c_str());
			}
		}

		pGCS = WKT("GEOGCS");
	}
	else if( is_Geographic() )
	{
		pGCS = &WKT;
	}

	if( pGCS && pGCS->Cmp_Name("GEOGCS") )
	{
		ADD_HEAD(_TL("Geographic Coordinate System"),   pGCS->Get_Property("name") && !pGCS->Cmp_Property("name", "unknown", true) ? pGCS->Get_Property("name") : _TL(""));
		ADD_PROP(_TL("Authority Code"              ),   pGCS, "authority_code");
		ADD_PROP(_TL("Authority"                   ),   pGCS, "authority_name");
		ADD_PROP(_TL("Prime Meridian"              ), (*pGCS)("PRIMEM"        ), "name");
		ADD_PROP(_TL("Angular Unit"                ), (*pGCS)("UNIT"          ), "name");
		ADD_PROP(_TL("Datum"                       ), (*pGCS)("DATUM"         ), "name");
		ADD_PROP(_TL("Spheroid"                    ), (*pGCS)("DATUM.SPHEROID"), "name");
		ADD_CONT(_TL("Semimajor Axis"              ), (*pGCS)("DATUM.SPHEROID.a" ));
		ADD_CONT(_TL("Inverse Flattening"          ), (*pGCS)("DATUM.SPHEROID.rf"));
		ADD_CONT(_TL("Extension"                   ), (*pGCS)("DATUM.EXTENSION"));

		ADD_CONT(CSG_String::Format("%s, %s [X]", _TL("Datum Shift"), _TL("Translation")), (*pGCS)("DATUM.TOWGS84.dx"));
		ADD_CONT(CSG_String::Format("%s, %s [Y]", _TL("Datum Shift"), _TL("Translation")), (*pGCS)("DATUM.TOWGS84.dy"));
		ADD_CONT(CSG_String::Format("%s, %s [Z]", _TL("Datum Shift"), _TL("Translation")), (*pGCS)("DATUM.TOWGS84.dz"));
		ADD_CONT(CSG_String::Format("%s, %s [X]", _TL("Datum Shift"), _TL("Rotation"   )), (*pGCS)("DATUM.TOWGS84.rx"));
		ADD_CONT(CSG_String::Format("%s, %s [Y]", _TL("Datum Shift"), _TL("Rotation"   )), (*pGCS)("DATUM.TOWGS84.ry"));
		ADD_CONT(CSG_String::Format("%s, %s [Z]", _TL("Datum Shift"), _TL("Rotation"   )), (*pGCS)("DATUM.TOWGS84.rz"));
		ADD_CONT(CSG_String::Format("%s, %s [X]", _TL("Datum Shift"), _TL("Scaling"    )), (*pGCS)("DATUM.TOWGS84.sc"));
	}

	s += "</table>";

//	if( m_WKT1.Length() > 0 ) { s += "\n[" + m_WKT1 + "]"; }
//	if( m_PROJ.Length() > 0 ) { s += "\n[" + m_PROJ + "]"; }

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::is_Equal(const CSG_Projection &Projection)	const
{
	if( Get_Type() != Projection.Get_Type() )
	{
		return( false );
	}

	if( !is_Okay() ) // both are not valid => ESG_CRS_Type::Undefined
	{
		return( true );
	}

	if( !m_Authority.is_Empty() && !m_Authority.CmpNoCase(Projection.m_Authority) && m_Code == Projection.m_Code )
	{
		return(	true );
	}

	if( !m_PROJ.CmpNoCase(Projection.m_PROJ) ) // the simple case, identical PROJ strings...
	{
		return( true );
	}

	//-----------------------------------------------------
	// okay, let's perform a more detailed check...

	#define CMP_CONTENT(a, b    ) (a && b && a->Cmp_Content(b->Get_Content()))
	#define CMP_PROPERTY(a, b, p) (a && b && a->Get_Property(p) && b->Cmp_Property(p, a->Get_Property(p), true))
	#define CMP_PARAMETER(a, b  ) (a && b && ((!a->Cmp_Name("PARAMETER") && !b->Cmp_Name("PARAMETER")) || CMP_PROPERTY(a, b, "name") && a->Cmp_Content(b->Get_Content())))

	CSG_MetaData WKT[2] = {
		CSG_Projections::_WKT_to_MetaData(           m_WKT1),
		CSG_Projections::_WKT_to_MetaData(Projection.m_WKT1)
	}, *pGCS[2] = { NULL, NULL };

	if( is_Projection() )
	{

		if( !CMP_CONTENT (WKT[0]("PROJECTION"), WKT[1]("PROJECTION")        ) ) { return( false ); }
		if( !CMP_PROPERTY(WKT[0]("UNIT"      ), WKT[1]("UNIT"      ), "name") ) { return( false ); }

		for(int i=0; i<WKT[0].Get_Children_Count() && i<WKT[1].Get_Children_Count(); i++)
		{
			if( !CMP_PARAMETER(WKT[0](i), WKT[1](i)) ) { return( false ); }
		}

		pGCS[0] = WKT[0]("GEOGCS");
		pGCS[1] = WKT[1]("GEOGCS");
	}
	else if( is_Geographic() )
	{
		pGCS[0] = &WKT[0];
		pGCS[1] = &WKT[1];
	}

	if( !pGCS[0] || !pGCS[1] )
	{
		return( false );
	}

	if( !CMP_CONTENT((*pGCS[0])("PRIMEM"           ), (*pGCS[1])("PRIMEM"           )) ) { return( false ); }
	if( !CMP_CONTENT((*pGCS[0])("UNIT"             ), (*pGCS[1])("UNIT"             )) ) { return( false ); }
	if( !CMP_CONTENT((*pGCS[0])("DATUM.SPHEROID.a" ), (*pGCS[1])("DATUM.SPHEROID.a" )) ) { return( false ); }
	if( !CMP_CONTENT((*pGCS[0])("DATUM.SPHEROID.rf"), (*pGCS[1])("DATUM.SPHEROID.rf")) ) { return( false ); }

	if( (*pGCS[0])("DATUM.TOWGS84") || (*pGCS[1])("DATUM.TOWGS84") )
	{
		#define CMP_TOWGS84(id) (\
		   ((*pGCS[0])("DATUM.TOWGS84." id) ? (*pGCS[0])["DATUM.TOWGS84." id].Get_Content().asDouble() : 0.)\
		== ((*pGCS[1])("DATUM.TOWGS84." id) ? (*pGCS[1])["DATUM.TOWGS84." id].Get_Content().asDouble() : 0.) )

		if( !CMP_TOWGS84("dx") ) { return( false ); }
		if( !CMP_TOWGS84("dy") ) { return( false ); }
		if( !CMP_TOWGS84("dz") ) { return( false ); }
		if( !CMP_TOWGS84("rx") ) { return( false ); }
		if( !CMP_TOWGS84("ry") ) { return( false ); }
		if( !CMP_TOWGS84("rz") ) { return( false ); }
		if( !CMP_TOWGS84("sc") ) { return( false ); }
	}

	if( (*pGCS[0])("DATUM.EXTENSION") || (*pGCS[1])("DATUM.EXTENSION") )
	{
		if( !CMP_CONTENT((*pGCS[0])("DATUM.EXTENSION"), (*pGCS[1])("DATUM.EXTENSION")) ) { return( false ); }
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Type_Identifier(void) const
{
	return( CSG_Projections::Get_CRS_Type_Identifier(m_Type) );
}

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Type_Name(void) const
{
	return( CSG_Projections::Get_CRS_Type_Name(m_Type) );
}

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Unit_Identifier(void) const
{
	return( CSG_Projections::Get_Unit_Identifier(m_Unit) );
}

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Unit_Name(void) const
{
	return( CSG_Projections::Get_Unit_Name(m_Unit) );
}

//---------------------------------------------------------
double CSG_Projection::Get_Unit_To_Meter(void) const
{
	return( CSG_Projections::Get_Unit_To_Meter(m_Unit) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const CSG_Projection & CSG_Projection::Get_GCS_WGS84(void)
{
	static CSG_Projection GCS_WGS84;

	if( !GCS_WGS84.is_Okay() )
	{
		GCS_WGS84.Set_GCS_WGS84();
	}

	return( GCS_WGS84 );
}

//---------------------------------------------------------
bool CSG_Projection::Set_GCS_WGS84(void)
{
	return( Create(4326) );
}

//---------------------------------------------------------
CSG_Projection CSG_Projection::Get_UTM_WGS84(int Zone, bool bSouth)
{
	CSG_Projection Projection;

	Projection.Set_UTM_WGS84(Zone, bSouth);

	return( Projection );
}

//---------------------------------------------------------
bool CSG_Projection::Set_UTM_WGS84(int Zone, bool bSouth)
{
	if( Zone < 1 || Zone > 60 )
	{
		return( false );
	}

	int	EPSG_ID	= (bSouth ? 32700 : 32600) + Zone;

	if( Create(EPSG_ID) )
	{
		return( true );
	}

	//-----------------------------------------------------
	#define WKT1_GCS_WGS84 "GEOGCS[\"WGS 84\",AUTHORITY[\"EPSG\",\"4326\"]],"\
		"DATUM[\"WGS_1984\",AUTHORITY[\"EPSG\",\"6326\"]],"\
			"SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],"\
		"PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],"\
		"UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]]"

	#define WKT2_GCS_WGS84 "GEODCRS[\"WGS 84\","\
		"DATUM[\"World Geodetic System 1984\","\
			"ELLIPSOID[\"WGS 84\",6378137,298.257223563]],"\
		"CS[ellipsoidal,2],"\
			"AXIS[\"geodetic longitude (Lon)\",east],"\
			"AXIS[\"geodetic latitude (Lat)\",north],"\
			"UNIT[\"degree\",0.0174532925199433],"\
		"ID[\"EPSG\",4326]]"

	CSG_String WKT;

	WKT.Printf("PROJCS[\"WGS 84 / UTM zone %d%c\",%s"						// Zone, N/S, Datum
		"PROJECTION[\"Transverse_Mercator\"],AUTHORITY[\"EPSG\",\"%d\"]]"	// EPSG ID
			"PARAMETER[\"latitude_of_origin\",0],"
			"PARAMETER[\"central_meridian\",%d],"							// Central Meridian
			"PARAMETER[\"scale_factor\",0.9996],"
			"PARAMETER[\"false_easting\",500000],"
			"PARAMETER[\"false_northing\",%d],"								// False Northing
			"AXIS[\"Easting\",EAST],"
			"AXIS[\"Northing\",NORTH],"
			"UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]]",
		Zone, bSouth ? 'S' : 'N', CSG_String(WKT2_GCS_WGS84).c_str(), EPSG_ID, 6 * (Zone - 1) - 177, bSouth ? 10000000 : 0
	);

	return( Create(WKT) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_PROJ_FIELD_ID
{
	PRJ_FIELD_SRID = 0,
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
CSG_Projections::CSG_Projections(bool LoadDefault)
{
	_On_Construction();

	Create(LoadDefault);
}

bool CSG_Projections::Create(bool LoadDefault)
{
	Destroy();

	if( LoadDefault ) // load spatial reference system database and dictionary
	{
		#if defined(_SAGA_LINUX)
			CSG_String Path_Shared = SHARE_PATH;
		#else
			CSG_String Path_Shared = SG_UI_Get_Application_Path(true);
		#endif

		SG_UI_Msg_Lock(true);

		_Load(m_pProjections, SG_File_Make_Path(Path_Shared, "saga", "srs"));

		if( _Load(m_pPreferences, SG_File_Make_Path(Path_Shared, "saga_preferences", "srs")) )
		{
			_Add_Preferences();
		}

		SG_UI_Msg_Lock(false);
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Projections::_On_Construction(void)
{
	m_pProjections = new CSG_Table;

	m_pProjections->Add_Field("srid"     , SG_DATATYPE_Int   );	// PRJ_FIELD_SRID
	m_pProjections->Add_Field("auth_name", SG_DATATYPE_String);	// PRJ_FIELD_AUTH_NAME
	m_pProjections->Add_Field("auth_srid", SG_DATATYPE_Int   );	// PRJ_FIELD_AUTH_SRID
	m_pProjections->Add_Field("srtext"   , SG_DATATYPE_String);	// PRJ_FIELD_SRTEXT
	m_pProjections->Add_Field("proj4text", SG_DATATYPE_String);	// PRJ_FIELD_PROJ4TEXT

	m_pPreferences = new CSG_Table(m_pProjections);

	_Set_Dictionary();
}

//---------------------------------------------------------
CSG_Projections::~CSG_Projections(void)
{
	Destroy();

	delete(m_pProjections);
	delete(m_pPreferences);
}

//---------------------------------------------------------
void CSG_Projections::Destroy(void)
{
	if( m_pProjections ) { m_pProjections->Del_Records(); }
	if( m_pPreferences ) { m_pPreferences->Del_Records(); }
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
sLong CSG_Projections::Get_Count(void) const
{
	return( m_pProjections->Get_Count() );
}

//---------------------------------------------------------
bool CSG_Projections::Add(const CSG_Projection &Projection)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::Add(const SG_Char *WKT, const SG_Char *Proj4, const SG_Char *Authority, int Authority_ID)
{
	CSG_Table_Record *pProjection = m_pProjections->Add_Record();

	pProjection->Set_Value(PRJ_FIELD_SRID     , (int)m_pProjections->Get_Count());
	pProjection->Set_Value(PRJ_FIELD_AUTH_NAME, Authority);
	pProjection->Set_Value(PRJ_FIELD_AUTH_SRID, Authority_ID);
	pProjection->Set_Value(PRJ_FIELD_SRTEXT   , WKT);
	pProjection->Set_Value(PRJ_FIELD_PROJ4TEXT, Proj4);

	return( true );
}

//---------------------------------------------------------
CSG_Projection CSG_Projections::_Get_Projection(CSG_Table_Record *pProjection)
{
	CSG_Projection Projection;

	if( pProjection )
	{
		Projection.m_Authority = pProjection->asString(PRJ_FIELD_AUTH_NAME);
		Projection.m_Code      = pProjection->asInt   (PRJ_FIELD_AUTH_SRID);
		Projection.m_WKT1      = pProjection->asString(PRJ_FIELD_SRTEXT   );
		Projection.m_PROJ      = pProjection->asString(PRJ_FIELD_PROJ4TEXT);

		CSG_MetaData WKT = _WKT_to_MetaData(Projection.m_WKT1);

		Projection.m_Name = WKT.Get_Property("name");
		Projection.m_Type = Get_CRS_Type(WKT.Get_Name());
		Projection.m_Unit = WKT("UNIT") && WKT["UNIT"].Get_Property("name") ?
			Get_Unit(WKT["UNIT"].Get_Property("name")) : ESG_Projection_Unit::Undefined;
	}

	return( Projection );
}

//---------------------------------------------------------
CSG_Projection CSG_Projections::Get_Projection(sLong Index)	const
{
	return( _Get_Projection(m_pProjections->Get_Record(Index)) );
}

//---------------------------------------------------------
const SG_Char * CSG_Projections::Get_Projection(int Code, const SG_Char *_Authority) const
{
	CSG_String Authority(_Authority && *_Authority ? _Authority : SG_T("EPSG"));

	for(sLong i=0; i<m_pProjections->Get_Count(); i++)
	{
		CSG_Table_Record *pProjection = m_pProjections->Get_Record(i);

		if( Code == pProjection->asInt(PRJ_FIELD_AUTH_SRID) && !Authority.CmpNoCase(pProjection->asString(PRJ_FIELD_AUTH_NAME)) )
		{
			return( pProjection->asString(PRJ_FIELD_SRTEXT) );
		}
	}

	return( SG_T("") );
}

//---------------------------------------------------------
bool CSG_Projections::Get_Projection(CSG_Projection &Projection, int Code, const SG_Char *_Authority) const
{
	CSG_String Authority(_Authority && *_Authority ? _Authority : SG_T("EPSG"));

	for(sLong i=0; i<m_pProjections->Get_Count(); i++)
	{
		CSG_Table_Record *pProjection = m_pProjections->Get_Record(i);

		if( Code == pProjection->asInt(PRJ_FIELD_AUTH_SRID) && !Authority.CmpNoCase(pProjection->asString(PRJ_FIELD_AUTH_NAME)) )
		{
			Projection = _Get_Projection(pProjection);

			return( Projection.is_Okay() );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_Add_Preferences(void)
{
	if( !m_pProjections || !m_pPreferences || m_pPreferences->Get_Count() < 1 )
	{
		return( false );
	}

	m_pProjections->Set_Index(PRJ_FIELD_AUTH_NAME, TABLE_INDEX_Ascending, PRJ_FIELD_AUTH_SRID, TABLE_INDEX_Ascending);
	m_pPreferences->Set_Index(PRJ_FIELD_AUTH_NAME, TABLE_INDEX_Ascending, PRJ_FIELD_AUTH_SRID, TABLE_INDEX_Ascending);

	for(sLong iPreference=0, iProjection=0; iPreference<m_pPreferences->Get_Count() && iProjection<m_pProjections->Get_Count(); )
	{
		CSG_Table_Record *pPreference = m_pPreferences->Get_Record_byIndex(iPreference);
		CSG_Table_Record *pProjection = m_pProjections->Get_Record_byIndex(iProjection);

		CSG_String Authority = pProjection->asString(PRJ_FIELD_AUTH_NAME);

		int Comparison = Authority.CmpNoCase(pPreference->asString(PRJ_FIELD_AUTH_NAME));
		
		if( Comparison < 0 ) { iProjection++; } else if( Comparison > 0 ) { iPreference++; } else
		{
			Comparison = pPreference->asInt(PRJ_FIELD_AUTH_SRID) - pProjection->asInt(PRJ_FIELD_AUTH_SRID);

			if( Comparison < 0 ) { iProjection++; } else if( Comparison > 0 ) { iPreference++; } else
			{
				pProjection->Set_Value(PRJ_FIELD_SRTEXT   , pPreference->asString(PRJ_FIELD_SRTEXT   ));
				pProjection->Set_Value(PRJ_FIELD_PROJ4TEXT, pPreference->asString(PRJ_FIELD_PROJ4TEXT));

				m_pPreferences->Select(iPreference++); iProjection++;
			}
		}
	}

	m_pProjections->Del_Index();
	m_pPreferences->Del_Index();

	if( m_pPreferences->Get_Selection_Count() < m_pPreferences->Get_Count() )
	{
		for(sLong iPreference=0; iPreference<m_pPreferences->Get_Count(); iPreference++)
		{
			CSG_Table_Record *pPreference = m_pPreferences->Get_Record_byIndex(iPreference);

			if( !pPreference->is_Selected() )
			{
				m_pProjections->Add_Record(pPreference);
			}
		}
	}
	
	m_pPreferences->Select(); // unselect all records

	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::_Get_Preferences(CSG_Projection &Projection, int Code, const CSG_String &Authority) const
{
	for(sLong i=0; i<m_pPreferences->Get_Count(); i++)
	{
		CSG_Table_Record *pProjection = m_pPreferences->Get_Record(i);

		if( Code == pProjection->asInt(PRJ_FIELD_AUTH_SRID) && !Authority.CmpNoCase(pProjection->asString(PRJ_FIELD_AUTH_NAME)) )
		{
			Projection = _Get_Projection(pProjection);

			return( Projection.is_Okay() );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_Load(CSG_Table *pTable, const CSG_String &File, bool bAppend) const
{
	CSG_Table Table;

	if( pTable && Table.Create(File) && Table.Get_Count() > 0 && Table.Get_Field_Count() >= 5 )
	{
		if( bAppend )
		{
			for(sLong i=0; i<pTable->Get_Count(); i++)
			{
				Table.Add_Record(pTable->Get_Record(i));
			}
		}

		pTable->Del_Records();

		Table.Set_Index(PRJ_FIELD_SRTEXT, TABLE_INDEX_Ascending);

		for(sLong i=0; i<Table.Get_Count(); i++)
		{
			pTable->Add_Record(Table.Get_Record_byIndex(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::Load(const CSG_String &File, bool bAppend)
{
	return( _Load(m_pProjections, File, bAppend) );
}

//---------------------------------------------------------
bool CSG_Projections::Save(const CSG_String &File)
{
	return( m_pProjections->Save(File) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::Parse(const CSG_String &Definition, CSG_String *WKT1, CSG_String *WKT2, CSG_String *PROJ, CSG_String *ESRI)
{
	if( Definition.is_Empty() )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Definition.BeforeFirst(':').is_Empty() ) // check white list first !
	{
		CSG_Projection Projection; int Code;

		if( Definition.AfterFirst(':').asInt(Code) && gSG_Projections._Get_Preferences(Projection, Code, Definition.BeforeFirst(':')) )
		{
			if( WKT1 ) { *WKT1 = Projection.Get_WKT1(); }
			if( WKT2 ) { *WKT2 = Projection.Get_WKT2(); }
			if( PROJ ) { *PROJ = Projection.Get_PROJ(); }
			if( ESRI ) { *ESRI = Projection.Get_ESRI(); }

			return( true );
		}
	}

	//-----------------------------------------------------
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 19); // Coordinate Reference System Format Conversion

	if( pTool ) // check proj.lib
	{
		pTool->Set_Parameter("DEFINITION", Definition);
		pTool->Set_Parameter("MULTILINE" , false);
		pTool->Set_Parameter("SIMPLIFIED", true);

		SG_UI_ProgressAndMsg_Lock(true);
		bool bResult = pTool->Execute();
		SG_UI_ProgressAndMsg_Lock(false);

		if( bResult )
		{
			if( WKT1 ) { *WKT1 = pTool->Get_Parameter("WKT1")->asString(); }
			if( WKT2 ) { *WKT2 = pTool->Get_Parameter("WKT2")->asString(); }
			if( PROJ ) { *PROJ = pTool->Get_Parameter("PROJ")->asString(); }
			if( ESRI ) { *ESRI = pTool->Get_Parameter("ESRI")->asString(); }
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		return( bResult );
	}

	//-----------------------------------------------------
	else // proj.lib parser not available ...fallback!
	{
		if( Definition.Find("+proj") == 0 )
		{
			CSG_String WKT;

			if( gSG_Projections._WKT_from_Proj4(WKT, Definition) )
			{
				if( WKT1 ) { *WKT1 = WKT       ; }
				if( PROJ ) { *PROJ = Definition; }

				return( true );
			}

			return( false );
		}

		//-------------------------------------------------
		CSG_MetaData WKT(CSG_Projections::_WKT_to_MetaData(Definition));

		int Code; CSG_String Authority; CSG_Projection Projection;

		if(	WKT.Get_Property("authority_name", Authority) && WKT.Get_Property("authority_code", Code)
		&&  gSG_Projections.Get_Projection(Projection, Code, Authority) )
		{
			if( WKT1 ) { *WKT1 = Projection.Get_WKT1(); }
			if( WKT2 ) { *WKT2 = Projection.Get_WKT2(); }
			if( PROJ ) { *PROJ = Projection.Get_PROJ(); }
			if( ESRI ) { *ESRI = Projection.Get_ESRI(); }

			return( true );
		}

		//-------------------------------------------------
		CSG_String Proj4;

		if( gSG_Projections._WKT_to_Proj4(Proj4, Definition) )
		{
			if( WKT1 ) { *WKT1 = Definition; }
			if( PROJ ) { *PROJ = Proj4     ; }

			return( true );
		}

		//-------------------------------------------------
		Authority = Definition.BeforeFirst(':');

		if( !Authority.is_Empty() && Definition.AfterFirst(':').asInt(Code)
		&&  gSG_Projections.Get_Projection(Projection, Code, Authority) )
		{
			if( WKT1 ) { *WKT1 = Projection.Get_WKT1(); }
			if( WKT2 ) { *WKT2 = Projection.Get_WKT2(); }
			if( PROJ ) { *PROJ = Projection.Get_PROJ(); }
			if( ESRI ) { *ESRI = Projection.Get_ESRI(); }

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_EPSG_to_Proj4(CSG_String &Proj4, int EPSG_Code) const
{
	for(sLong i=0; i<m_pProjections->Get_Count(); i++)
	{
		if( m_pProjections->Get_Record(i)->asInt(PRJ_FIELD_AUTH_SRID) == EPSG_Code )
		{
			Proj4 = m_pProjections->Get_Record(i)->asString(PRJ_FIELD_PROJ4TEXT);

			return( true );
		}
	}

	Proj4.Printf("+init=epsg:%d ", EPSG_Code);

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_EPSG_to_WKT(CSG_String &WKT, int EPSG_Code) const
{
	for(sLong i=0; i<m_pProjections->Get_Count(); i++)
	{
		if( m_pProjections->Get_Record(i)->asInt(PRJ_FIELD_AUTH_SRID) == EPSG_Code )
		{
			WKT = m_pProjections->Get_Record(i)->asString(PRJ_FIELD_SRTEXT);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Projections::Get_Names_List(ESG_CRS_Type Type, bool bAddSelect) const
{
	CSG_String Names;

	if( bAddSelect )
	{
		Names.Printf("{}<%s>|", _TL("select"));
	}

	m_pProjections->Set_Index(PRJ_FIELD_SRTEXT, TABLE_INDEX_Ascending);

	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Table_Record *pProjection = m_pProjections->Get_Record_byIndex(i);

		CSG_String WKT = pProjection->asString(PRJ_FIELD_SRTEXT);

		ESG_CRS_Type _Type =
			!WKT.BeforeFirst('[').Cmp("PROJCS") ? ESG_CRS_Type::Projection :
			!WKT.BeforeFirst('[').Cmp("GEOGCS") ? ESG_CRS_Type::Geographic :
			!WKT.BeforeFirst('[').Cmp("GEOCCS") ? ESG_CRS_Type::Geocentric : ESG_CRS_Type::Undefined;

		if( Type == ESG_CRS_Type::Undefined )
		{
			Names += CSG_String::Format("{%s:%d}%s: %s|",
				pProjection->asString(PRJ_FIELD_AUTH_NAME),
				pProjection->asInt   (PRJ_FIELD_SRID     ),
				CSG_Projections::Get_CRS_Type_Name(_Type).c_str(),
				WKT.AfterFirst('\"').BeforeFirst('\"').c_str()
			);
		}
		else if( Type == _Type )
		{
			Names += CSG_String::Format("{%s:%d}%s|",
				pProjection->asString(PRJ_FIELD_AUTH_NAME),
				pProjection->asInt   (PRJ_FIELD_SRID     ),
				WKT.AfterFirst('\"').BeforeFirst('\"').c_str()
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
	CSG_String Key; CSG_Strings Content; Content.Add("");

	for(int i=0, l=-1; l!=0 && i<(int)WKT.Length(); i++)
	{
		if( l < 0 )	// read key
		{
			switch( WKT[i] )
			{
			default           : Key += WKT[i]; break;
			case ' '          :                break;
			case '[': case '(': l    = 1     ; break;
			case ')': case ']':      return( false );
			}
		}
		else		// read content
		{
			bool bAdd;

			switch( WKT[i] )
			{
			default            : bAdd =  true;                      break;
			case '\"'          : bAdd = false;                      break;
			case '[' : case '(': bAdd = ++l > 1;                    break;
			case ']' : case ')': bAdd = l-- > 1;                    break;
			case ',' :     if( !(bAdd = l   > 1) ) Content.Add(""); break;
			}

			if( bAdd )
			{
				Content[Content.Get_Count() - 1] += WKT[i];
			}
		}
	}

	if( Key.is_Empty() || Content[0].is_Empty() )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Key.Cmp("AUTHORITY") && Content.Get_Count() == 2 )	// AUTHORITY  ["<name>", "<code>"]
	{
		MetaData.Add_Property("authority_name", Content[0]);
		MetaData.Add_Property("authority_code", Content[1]);

		return( true );
	}

	CSG_MetaData *pKey = MetaData.Add_Child(Key);

	if(	(!Key.Cmp("GEOCCS"    ) && Content.Get_Count() >= 4)  	// GEOCCS     ["<name>", <datum>, <prime meridian>, <linear unit> {,<axis>, <axis>, <axis>} {,<authority>}]
	||	(!Key.Cmp("GEOGCS"    ) && Content.Get_Count() >= 4)  	// GEOGCS     ["<name>", <datum>, <prime meridian>, <angular unit> {,<twin axes>} {,<authority>}]
	||	(!Key.Cmp("PROJCS"    ) && Content.Get_Count() >= 3)  	// PROJCS     ["<name>", <geographic cs>, <projection>, {<parameter>,}* <linear unit> {,<twin axes>}{,<authority>}]
	||	(!Key.Cmp("DATUM"     ) && Content.Get_Count() >= 2) )	// DATUM      ["<name>", <spheroid> {,<to wgs84>} {,<authority>}]
	{
		pKey->Add_Property("name", Content[0]);
	}

	if(	(!Key.Cmp("PRIMEM"    ) && Content.Get_Count() >= 2)  	// PRIMEM     ["<name>", <longitude> {,<authority>}]
	||	(!Key.Cmp("UNIT"      ) && Content.Get_Count() >= 2)  	// UNIT       ["<name>", <conversion factor> {,<authority>}]
	||	(!Key.Cmp("AXIS"      ) && Content.Get_Count() >= 2)  	// AXIS       ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	||	(!Key.Cmp("PARAMETER" ) && Content.Get_Count() >= 2) )	// PARAMETER  ["<name>", <value>]
	{
		pKey->Add_Property("name", Content[0]);

		pKey->Set_Content(Content[1]);
	}

	if( (!Key.Cmp("SPHEROID"  ) && Content.Get_Count() >= 3) )	// SPHEROID   ["<name>", <semi-major axis>, <inverse flattening> {,<authority>}]
	{
		pKey->Add_Property("name", Content[0]);
		pKey->Add_Child   ("a"   , Content[1]);
		pKey->Add_Child   ("rf"  , Content[2]);
	}

	if( (!Key.Cmp("TOWGS84"   ) && Content.Get_Count() >= 7) )	// TOWGS84    [<dx>, <dy>, <dz>, <rx>, <ry>, <rz>, <sc>]
	{
		pKey->Add_Child("dx"     , Content[0]);
		pKey->Add_Child("dy"     , Content[1]);
		pKey->Add_Child("dz"     , Content[2]);
		pKey->Add_Child("rx"     , Content[3]);
		pKey->Add_Child("ry"     , Content[4]);
		pKey->Add_Child("rz"     , Content[5]);
		pKey->Add_Child("sc"     , Content[6]);
	}

	if( (!Key.Cmp("EXTENSION" ) && Content.Get_Count() >= 2) )	// EXTENSION  [<name>, <value>]
	{
		pKey->Add_Property("name", Content[0]);
		pKey->Set_Content(         Content[1]);
	}

	if( (!Key.Cmp("PROJECTION") && Content.Get_Count() >= 1) )	// PROJECTION ["<name>" {,<authority>}]
	{
		pKey->Set_Content(Content[0]);
	}

	//-----------------------------------------------------
	for(int i=0; i<Content.Get_Count(); i++)
	{
		_WKT_to_MetaData(*pKey, Content[i]);
	}

	return( true );
}

//---------------------------------------------------------
CSG_MetaData CSG_Projections::_WKT_to_MetaData(const CSG_String &WKT)
{
	CSG_MetaData MetaData;

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
// DATUM  ["<name>",
//     SPHEROID["<name>", <semi-major axis>, <inverse flattening>]
//    *TOWGS84 [<dx>, <dy>, <dz>, <rx>, <ry>, <rz>, <sc>]
// ]
//---------------------------------------------------------
bool CSG_Projections::_WKT_to_Proj4_Set_Datum(CSG_String &Proj4, const CSG_MetaData &WKT) const
{
	if( WKT.Cmp_Property("name", "WGS84") )
	{
		Proj4 += " +datum=WGS84";

		return( true );
	}

	double a, b;

	if(	!WKT("SPHEROID") ||	WKT["SPHEROID"].Get_Children_Count() != 2
	||	!WKT["SPHEROID"][0].Get_Content().asDouble(a) || a <= 0.
	||	!WKT["SPHEROID"][1].Get_Content().asDouble(b) || b <  0. )
	{
		return( false );
	}

	b = b > 0. ? a - a / b : a;

	Proj4 += CSG_String::Format(" +a=%f", a);	// Semimajor radius of the ellipsoid axis
	Proj4 += CSG_String::Format(" +b=%f", b);	// Semiminor radius of the ellipsoid axis

	if(	WKT("TOWGS84") && WKT["TOWGS84"].Get_Children_Count() == 7 )
	{
		Proj4 += " +towgs84=";

		for(int i=0; i<7; i++)
		{
			if( i > 0 )
			{
				Proj4 += ",";
			}

			Proj4 += WKT["TOWGS84"][i].Get_Content();
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::_WKT_to_Proj4(CSG_String &Proj4, const CSG_String &WKT) const
{
	Proj4.Clear();

	CSG_MetaData m = _WKT_to_MetaData(WKT);

	if( m.Get_Children_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int Authority_Code; CSG_String Authority_Name;

	if(	m.Get_Property("authority_name", Authority_Name) && Authority_Name.CmpNoCase("EPSG") == 0
	&&	m.Get_Property("authority_code", Authority_Code) && _EPSG_to_Proj4(Proj4, Authority_Code) )
	{	//	Proj4.Printf("+init=epsg:%d", Authority_Code);
		return( true );
	}

	//-----------------------------------------------------
	double d;

	//-----------------------------------------------------
	// GEOCCS["<name>",
	//    DATUM  ["<name>", ...],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER], AXIS...
	// ]
	if( m.Cmp_Name("GEOCCS") )
	{
		Proj4 = CSG_String::Format("+proj=geocent");

		if( !m("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["DATUM"]) )
		{
			return( false );
		}

		if( m("PRIMEM") && m["PRIMEM"].Get_Content().asDouble(d) && d != 0. )
		{
			Proj4 += CSG_String::Format(" +pm=%f", d);
		}

		Proj4 += CSG_String::Format(" +no_defs"); // Don't use the /usr/share/proj/proj_def.dat defaults file

		return( true );
	}

	//-----------------------------------------------------
	// GEOGCS["<name>,
	//    DATUM  ["<name>", ...],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER], AXIS...
	// ]
	if( m.Cmp_Name("GEOGCS") )
	{
		Proj4 = "+proj=longlat";

		if( !m("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["DATUM"]) )
		{
			return( false );
		}

		if( m("PRIMEM") && m["PRIMEM"].Get_Content().asDouble(d) && d != 0. )
		{
			Proj4 += CSG_String::Format(" +pm=%f", d);
		}

		Proj4 += CSG_String::Format(" +no_defs");	// Don't use the /usr/share/proj/proj_def.dat defaults file

		return( true );
	}

	//-----------------------------------------------------
	// PROJCS["<name>,
	//     GEOGCS    ["<name>, ...],
	//     PROJECTION["<name>"],
	//    *PARAMETER ["<name>", <value>], PARAMETER...
	//     UNIT      ["<name>", <conversion factor>],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER], AXIS...
	// ]
	if( m.Cmp_Name("PROJCS") && m("GEOGCS") && m("PROJECTION") && m_WKT_to_Proj4.Get_Translation(m["PROJECTION"].Get_Content(), Proj4) )
	{
		if( m["PROJECTION"].Cmp_Content("Transverse_Mercator") ) // UTM ???
		{
			double Scale = -1., Easting = -1., Northing = -1., Meridian = -1., Latitude = -1.;

			for(int i=0; i<m.Get_Children_Count(); i++)
			{
				if( m[i].Cmp_Name("PARAMETER") )
				{
					double v;

					if( m[i].Cmp_Property("name", "central_meridian"  , true) && m[i].Get_Content().asDouble(v) ) Meridian = v;
					if( m[i].Cmp_Property("name", "latitude_of_origin", true) && m[i].Get_Content().asDouble(v) ) Latitude = v;
					if( m[i].Cmp_Property("name", "scale_factor"      , true) && m[i].Get_Content().asDouble(v) ) Scale    = v;
					if( m[i].Cmp_Property("name", "false_easting"     , true) && m[i].Get_Content().asDouble(v) ) Easting  = v;
					if( m[i].Cmp_Property("name", "false_northing"    , true) && m[i].Get_Content().asDouble(v) ) Northing = v;
				}
			}

			if( Latitude == 0. && Scale == 0.9996 && Easting  == 500000. && (Northing == 0. || Northing == 10000000.) )
			{
				Proj4 = "+proj=utm";

				if(	!m["GEOGCS"]("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["GEOGCS"]["DATUM"]) )
				{
					return( false );
				}

				Proj4 += CSG_String::Format(" +zone=%d", (int)((183. + Meridian) / 6.));

				if( Northing == 10000000. )
				{
					Proj4 += " +south";
				}

				Proj4 += CSG_String::Format(" +no_defs");	// Don't use the /usr/share/proj/proj_def.dat defaults file

				return( true );
			}
		}

		//-------------------------------------------------
		Proj4 = "+proj=" + Proj4;

		if(	!m["GEOGCS"]("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["GEOGCS"]["DATUM"]) )
		{
			return( false );
		}

		if( m("PRIMEM") && m["PRIMEM"].Get_Content().asDouble(d) && d != 0. )
		{
			Proj4 += CSG_String::Format(" +pm=%f", d);
		}

		for(int i=0; i<m.Get_Children_Count(); i++)
		{
			if( m[i].Cmp_Name("PARAMETER") )
			{
				CSG_String	Parameter;

				if( m_WKT_to_Proj4.Get_Translation(m[i].Get_Property("name"), Parameter) )
				{
					Proj4 += " +" + Parameter + "=" + m[i].Get_Content();
				}
				else
				{
					SG_UI_Msg_Add_Error(CSG_String::Format(">> WKT: %s [%s]", _TL("unknown parameter"), m[i].Get_Property("name")));
				}
			}
		}

		if( m("UNIT") && m["UNIT"].Get_Content().asDouble(d) && d != 0. && d != 1. )
		{
			Proj4 += CSG_String::Format(" +to_meter=%f", d);
		}

		Proj4 += CSG_String::Format(" +no_defs");	// Don't use the /usr/share/proj/proj_def.dat defaults file

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Find_Parameter(const CSG_String &Proj4, const CSG_String &Key)
{
	return(	Proj4.Find("+" + Key) >= 0 );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Read_Parameter(CSG_String &Value, const CSG_String &Proj4, const CSG_String &Key)
{
	Value.Clear();

	int l, i = Proj4.Find("+" + Key + "=");

	if( i >= 0 )
	{
		for(++i, l=0; l<2 && i<(int)Proj4.Length(); i++)
		{
			switch( Proj4[i] )
			{
			case '=': l++; break;
			case '+': l=2; break;
			case ' ': l=2; break;
			default :
				if( l == 1 )
				{
					Value += Proj4[i];
				}
			}
		}
	}

	return( Value.Length() > 0 );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Ellipsoid(CSG_String &Value, const CSG_String &Proj4)
{
	const char ellipsoid[42][2][32] = 
	{	//	ellipsoid	a, b
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
				Value.Printf("SPHEROID[\"%s\",%s]", SG_STR_MBTOSG(ellipsoid[i][0]), SG_STR_MBTOSG(ellipsoid[i][1]));

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	double a = _Proj4_Read_Parameter(Value, Proj4, "a" ) && Value.asDouble(a) ? a : 6378137.;

	double b = _Proj4_Read_Parameter(Value, Proj4, "b" ) && Value.asDouble(b) ? a / (a - b)
	         : _Proj4_Read_Parameter(Value, Proj4, "rf") && Value.asDouble(b) ? b
	         : _Proj4_Read_Parameter(Value, Proj4, "f" ) && Value.asDouble(b) ? 1. / b
	         : _Proj4_Read_Parameter(Value, Proj4, "e" ) && Value.asDouble(b) ? a / (a - sqrt(b*b - a*a))
	         : _Proj4_Read_Parameter(Value, Proj4, "es") && Value.asDouble(b) ? a / (a - sqrt( b  - a*a))
	         : 298.2572236;

	Value = CSG_String::Format("SPHEROID[\"Ellipsoid\",%f,%f]", a, b);

	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Datum(CSG_String &Value, const CSG_String &Proj4)
{
	const char datum[9][3][64] =
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

	CSG_String Spheroid, ToWGS84;

	//-----------------------------------------------------
	if( _Proj4_Read_Parameter(Value, Proj4, "datum") )
	{
		for(int i=0; i<9; i++)
		{
			if( !Value.CmpNoCase(datum[i][0]) && _Proj4_Get_Ellipsoid(Spheroid, CSG_String::Format("+ellps=%s", SG_STR_MBTOSG(datum[i][1]))) )
			{
				Value.Printf("DATUM[\"%s\",%s,TOWGS84[%s]]", SG_STR_MBTOSG(datum[i][0]), Spheroid.c_str(), SG_STR_MBTOSG(datum[i][2]));

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	if( _Proj4_Get_Ellipsoid(Spheroid, Proj4) )
	{
		Value = "DATUM[\"Datum\","+ Spheroid;

		if( _Proj4_Read_Parameter(ToWGS84, Proj4, "towgs84") )
		{
			CSG_Strings s = SG_String_Tokenize(ToWGS84, ",");

			if( s.Get_Count() == 3 )
			{
				ToWGS84	+= ",0,0,0,0";
			}

			Value += ",TOWGS84[" + ToWGS84 + "]";
		}
		else
		{
			Value += ",TOWGS84[0,0,0,0,0,0,0]";
		}

		Value += "]";

		return( true );
	}

	//-----------------------------------------------------
	Value = "DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563],TOWGS84[0,0,0,0,0,0,0]]";

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Prime_Meridian(CSG_String &Value, const CSG_String &Proj4)
{
	const char meridian[12][2][16] =
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
				Value.Printf("PRIMEM[\"%s\",%s]", SG_STR_MBTOSG(meridian[i][0]), SG_STR_MBTOSG(meridian[i][1]));

				return( true );
			}
		}

		double d;

		if( Value.asDouble(d) && d != 0. )
		{
			Value.Printf("PRIMEM[\"Prime_Meridian\",%f]", d);

			return( true );
		}
	}

	//-----------------------------------------------------
	Value = "PRIMEM[\"Greenwich\",0]";

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Unit(CSG_String &Value, const CSG_String &Proj4)
{
	ESG_Projection_Unit	Unit = _Proj4_Read_Parameter(Value, Proj4, "units") ? CSG_Projections::Get_Unit(Value) : ESG_Projection_Unit::Undefined;

	if( Unit != ESG_Projection_Unit::Undefined )
	{
		Value = "UNIT[\"" + CSG_Projections::Get_Unit_Name(Unit) + "\"," + SG_Get_String(CSG_Projections::Get_Unit_To_Meter(Unit), -16) + "]";

		return( true );
	}

	//-----------------------------------------------------
	double d;

	if( _Proj4_Read_Parameter(Value, Proj4, "to_meter") && Value.asDouble(d) && d > 0. && d != 1. )
	{
		Value.Printf("UNIT[\"Unit\",%f]", d);

		return( true );
	}

	//-----------------------------------------------------
	Value = "UNIT[\"metre\",1]";
//	Value = "UNIT[\"degree\",0.01745329251994328]]";

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_WKT_from_Proj4(CSG_String &WKT, const CSG_String &Proj4) const
{
	CSG_String Value, ProjCS;

	//-----------------------------------------------------
	if( !_Proj4_Read_Parameter(ProjCS, Proj4, "proj") )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 >> WKT: %s", _TL("no projection type defined")));

		return( false );
	}

	//-----------------------------------------------------
	// GEOCCS["<name>
	//    DATUM  ["<name>
	//        SPHEROID["<name>", <semi-major axis>, <inverse flattening>],
	//       *TOWGS84 [<dx>, <dy>, <dz>, <rx>, <ry>, <rz>, <sc>]
	//    ],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]

	if(	!ProjCS.CmpNoCase("geocent") )
	{
		WKT = "GEOGCS[\"GCS\"";

		if( _Proj4_Get_Datum         (Value, Proj4) ) { WKT += "," + Value; }
		if( _Proj4_Get_Prime_Meridian(Value, Proj4) ) { WKT += "," + Value; }
		if( _Proj4_Get_Unit          (Value, Proj4) ) { WKT += "," + Value; }

		WKT += "]";

		return( true );
	}

	//-----------------------------------------------------
	// GEOGCS["<name>
	//    DATUM  ["<name>
	//        SPHEROID["<name>", <semi-major axis>, <inverse flattening>],
	//       *TOWGS84 [<dx>, <dy>, <dz>, <rx>, <ry>, <rz>, <sc>]
	//    ],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]

	CSG_String GeogCS = "GEOGCS[\"GCS\"";

	if( _Proj4_Get_Datum         (Value, Proj4) ) { GeogCS += "," + Value; }
	if( _Proj4_Get_Prime_Meridian(Value, Proj4) ) { GeogCS += "," + Value; }
	if( _Proj4_Get_Unit          (Value, Proj4) ) { GeogCS += "," + Value; } else { GeogCS += "UNIT[\"degree\",0.01745329251994328]"; }

	GeogCS += "]";

	if(	!ProjCS.CmpNoCase("lonlat") || !ProjCS.CmpNoCase("longlat")
	||	!ProjCS.CmpNoCase("latlon") || !ProjCS.CmpNoCase("latlong") )
	{
		WKT = GeogCS;

		return( true );
	}

	//-----------------------------------------------------
	// PROJCS["<name>
	//     GEOGCS    [ ...... ],
	//     PROJECTION["<name>"],
	//    *PARAMETER ["<name>", <value>], ...
	//     UNIT      ["<name>", <conversion factor>],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//    *AXIS      ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]

	if( !m_Proj4_to_WKT.Get_Translation(ProjCS, Value) )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 >> WKT: %s [%s]", _TL("no translation available"), ProjCS.c_str()));

	//	return( false );
	}

	//-----------------------------------------------------
	// UTM ...

	if( !ProjCS.CmpNoCase("utm") )
	{
		double Zone;

		if( !_Proj4_Read_Parameter(Value, Proj4, "zone") || !Value.asDouble(Zone) )
		{
			SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 >> WKT: %s", _TL("invalid utm zone")));

			return( false );
		}

		bool South = _Proj4_Find_Parameter(Proj4, "south");

		WKT  = CSG_String::Format("PROJCS[\"UTM zone %d%c\",%s,PROJECTION[Transverse_Mercator]", (int)Zone, South ? 'S' : 'N', GeogCS.c_str());

		WKT += CSG_String::Format(",PARAMETER[\"%s\",%d]", SG_T("latitude_of_origin"), 0);
		WKT += CSG_String::Format(",PARAMETER[\"%s\",%d]", SG_T("central_meridian"  ), (int)(Zone * 6 - 183));
		WKT += CSG_String::Format(",PARAMETER[\"%s\",%f]", SG_T("scale_factor"      ), 0.9996);
		WKT += CSG_String::Format(",PARAMETER[\"%s\",%d]", SG_T("false_easting"     ), 500000);
		WKT += CSG_String::Format(",PARAMETER[\"%s\",%d]", SG_T("false_northing"    ), South ? 10000000 : 0);
		WKT += ",UNIT[\"metre\",1]]";

		return( true );
	}

	//-----------------------------------------------------
	// Parameters ...

	WKT = CSG_String::Format("PROJCS[\"%s\",%s,PROJECTION[%s]", Value.c_str(), GeogCS.c_str(), Value.c_str());

	ProjCS = Proj4;

	while( ProjCS.Find('+') >= 0 )
	{
		CSG_String Key;

		ProjCS = ProjCS.AfterFirst ('+');
		Value  = ProjCS.BeforeFirst('=');

		if( m_Proj4_to_WKT.Get_Translation(Value, Key) )
		{
			Value = ProjCS.AfterFirst('=');

			if( Value.Find('+') >= 0 )
			{
				Value = Value.BeforeFirst('+');
			}

			WKT += ",PARAMETER[\"" + Key + "\"," + Value + "]";
		}
	}

	//-----------------------------------------------------
	// Unit ...

	if( _Proj4_Get_Unit(Value, Proj4) ) { WKT += "," + Value; }

	//-----------------------------------------------------
	WKT += "]";

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
ESG_CRS_Type CSG_Projections::Get_CRS_Type(const CSG_String &Identifier)
{
	if( !Identifier.CmpNoCase("PROJCS") ) { return( ESG_CRS_Type::Projection ); }
	if( !Identifier.CmpNoCase("GEOGCS") ) { return( ESG_CRS_Type::Geographic ); }
	if( !Identifier.CmpNoCase("GEOCCS") ) { return( ESG_CRS_Type::Geocentric ); }

	return( ESG_CRS_Type::Undefined );
}

//---------------------------------------------------------
CSG_String CSG_Projections::Get_CRS_Type_Identifier(ESG_CRS_Type Type)
{
	switch( Type )
	{
	case ESG_CRS_Type::Projection: return( "PROJCS"    );
	case ESG_CRS_Type::Geographic: return( "GEOGCS"    );
	case ESG_CRS_Type::Geocentric: return( "GEOCCS"    );
	default                      : return( "UNDEFINED" );
	}
}

//---------------------------------------------------------
CSG_String CSG_Projections::Get_CRS_Type_Name(ESG_CRS_Type Type)
{
	switch( Type )
	{
	case ESG_CRS_Type::Projection: return( _TL("Projected Coordinate System" ) );
	case ESG_CRS_Type::Geographic: return( _TL("Geographic Coordinate System") );
	case ESG_CRS_Type::Geocentric: return( _TL("Geocentric Coordinate System") );
	default                      : return( _TL("Undefined Coordinate System" ) );
	}
}

//---------------------------------------------------------
ESG_Projection_Unit CSG_Projections::Get_Unit(const CSG_String &Identifier)
{
	for(int i=0; i<(int)ESG_Projection_Unit::Undefined; i++)
	{
		ESG_Projection_Unit Unit = (ESG_Projection_Unit)i;

		if( !Identifier.CmpNoCase(Get_Unit_Identifier(Unit))
		||  !Identifier.CmpNoCase(Get_Unit_Name      (Unit)) )
		{
			return( Unit );
		}
	}

	return( !Identifier.CmpNoCase("metre") ? ESG_Projection_Unit::Meter : ESG_Projection_Unit::Undefined );
}

//---------------------------------------------------------
const CSG_String CSG_Projections::Get_Unit_Identifier(ESG_Projection_Unit Unit)
{
	switch( Unit )
	{
	case ESG_Projection_Unit::Kilometer        : return( "km"     ); // Kilometers
	case ESG_Projection_Unit::Meter            : return( "m"      ); // Meters
	case ESG_Projection_Unit::Decimeter        : return( "dm"     ); // Decimeters
	case ESG_Projection_Unit::Centimeter       : return( "cm"     ); // Centimeters
	case ESG_Projection_Unit::Millimeter       : return( "mm"     ); // Millimeters
	case ESG_Projection_Unit::Int_Nautical_Mile: return( "kmi"    ); // Miles
	case ESG_Projection_Unit::Int_Inch         : return( "in"     ); // Inches
	case ESG_Projection_Unit::Int_Foot         : return( "ft"     ); // Feet
	case ESG_Projection_Unit::Int_Yard         : return( "yd"     ); // Yards
	case ESG_Projection_Unit::Int_Statute_Mile : return( "mi"     ); // Miles
	case ESG_Projection_Unit::Int_Fathom       : return( "fath"   ); // Fathoms
	case ESG_Projection_Unit::Int_Chain        : return( "ch"     ); // Chains
	case ESG_Projection_Unit::Int_Link         : return( "link"   ); // Links
	case ESG_Projection_Unit::US_Inch          : return( "us-in"  ); // Inches
	case ESG_Projection_Unit::US_Foot          : return( "us-ft"  ); // Feet
	case ESG_Projection_Unit::US_Yard          : return( "us-yd"  ); // Yards
	case ESG_Projection_Unit::US_Chain         : return( "us-ch"  ); // Chains
	case ESG_Projection_Unit::US_Statute_Mile  : return( "us-mi"  ); // Miles
	case ESG_Projection_Unit::Indian_Yard      : return( "ind-yd" ); // Yards
	case ESG_Projection_Unit::Indian_Foot      : return( "ind-ft" ); // Feet
	case ESG_Projection_Unit::Indian_Chain     : return( "ind-ch" ); // Chains
	default: return( "" );
	};
}

//---------------------------------------------------------
const CSG_String CSG_Projections::Get_Unit_Name(ESG_Projection_Unit Unit, bool bSimple)
{
	switch( Unit )
	{
	case ESG_Projection_Unit::Kilometer        : return( bSimple ? "Kilometers"  : "Kilometer"                    );
	case ESG_Projection_Unit::Meter            : return( bSimple ? "Meters"      : "Meter"                        );
	case ESG_Projection_Unit::Decimeter        : return( bSimple ? "Decimeters"  : "Decimeter"                    );
	case ESG_Projection_Unit::Centimeter       : return( bSimple ? "Centimeters" : "Centimeter"                   );
	case ESG_Projection_Unit::Millimeter       : return( bSimple ? "Millimeters" : "Millimeter"                   );
	case ESG_Projection_Unit::Int_Nautical_Mile: return( bSimple ? "Miles"       : "International Nautical Mile"  );
	case ESG_Projection_Unit::Int_Inch         : return( bSimple ? "Inches"      : "International Inch"           );
	case ESG_Projection_Unit::Int_Foot         : return( bSimple ? "Feet"        : "International Foot"           );
	case ESG_Projection_Unit::Int_Yard         : return( bSimple ? "Yards"       : "International Yard"           );
	case ESG_Projection_Unit::Int_Statute_Mile : return( bSimple ? "Miles"       : "International Statute Mile"   );
	case ESG_Projection_Unit::Int_Fathom       : return( bSimple ? "Fathoms"     : "International Fathom"         );
	case ESG_Projection_Unit::Int_Chain        : return( bSimple ? "Chains"      : "International Chain"          );
	case ESG_Projection_Unit::Int_Link         : return( bSimple ? "Links"       : "International Link"           );
	case ESG_Projection_Unit::US_Inch          : return( bSimple ? "Inches"      : "U.S. Surveyor's Inch"         );
	case ESG_Projection_Unit::US_Foot          : return( bSimple ? "Feet"        : "U.S. Surveyor's Foot"         );
	case ESG_Projection_Unit::US_Yard          : return( bSimple ? "Yards"       : "U.S. Surveyor's Yard"         );
	case ESG_Projection_Unit::US_Chain         : return( bSimple ? "Chains"      : "U.S. Surveyor's Chain"        );
	case ESG_Projection_Unit::US_Statute_Mile  : return( bSimple ? "Miles"       : "U.S. Surveyor's Statute Mile" );
	case ESG_Projection_Unit::Indian_Yard      : return( bSimple ? "Yards"       : "Indian Yard"                  );
	case ESG_Projection_Unit::Indian_Foot      : return( bSimple ? "Feet"        : "Indian Foot"                  );
	case ESG_Projection_Unit::Indian_Chain     : return( bSimple ? "Chains"      : "Indian Chain"                 );
	default: return( "" );
	}
}

//---------------------------------------------------------
double CSG_Projections::Get_Unit_To_Meter(ESG_Projection_Unit Unit)
{
	switch( Unit )
	{
	case ESG_Projection_Unit::Kilometer        : return( 1000. );
	case ESG_Projection_Unit::Meter            : return( 1. );
	case ESG_Projection_Unit::Decimeter        : return( 0.1 );
	case ESG_Projection_Unit::Centimeter       : return( 0.01 );
	case ESG_Projection_Unit::Millimeter       : return( 0.001 );
	case ESG_Projection_Unit::Int_Nautical_Mile: return( 1852. );
	case ESG_Projection_Unit::Int_Inch         : return( 0.0254 );
	case ESG_Projection_Unit::Int_Foot         : return( 0.3048 );
	case ESG_Projection_Unit::Int_Yard         : return( 0.9144 );
	case ESG_Projection_Unit::Int_Statute_Mile : return( 1609.344 );
	case ESG_Projection_Unit::Int_Fathom       : return( 1.8288 );
	case ESG_Projection_Unit::Int_Chain        : return( 20.1168 );
	case ESG_Projection_Unit::Int_Link         : return( 0.201168 );
	case ESG_Projection_Unit::US_Inch          : return( 1. / 39.37 );
	case ESG_Projection_Unit::US_Foot          : return( 0.304800609601219 );
	case ESG_Projection_Unit::US_Yard          : return( 0.914401828803658 );
	case ESG_Projection_Unit::US_Chain         : return( 20.11684023368047 );
	case ESG_Projection_Unit::US_Statute_Mile  : return( 1609.347218694437 );
	case ESG_Projection_Unit::Indian_Yard      : return( 0.91439523 );
	case ESG_Projection_Unit::Indian_Foot      : return( 0.30479841 );
	case ESG_Projection_Unit::Indian_Chain     : return( 20.11669506 );
	default                                    : return( 1. );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_Set_Dictionary(CSG_Table &Dictionary, int Direction)
{
	const char Translation[][4][128] = {
//	{ PROJ4			, DIR	, WELL-KNOWN-TEXT							, DESCRIPTION, *) projection type not verified

//	--- projection types ---
	{ "aea"			, " ", "Albers_Conic_Equal_Area"					, "Albers Equal Area" },
	{ "aea"			, "<", "Albers"										, "[ESRI] Albers Equal Area" },
	{ "aeqd"		, " ", "Azimuthal_Equidistant"						, "Azimuthal Equidistant" },
	{ "airy"		, " ", "Airy 1830"									, "Airy 1830" },
	{ "aitoff"		, " ", "Sphere_Aitoff"								, "Aitoff" },
	{ "alsk"		, " ", "Mod_Stererographics_of_Alaska"				, "*) Mod. Stererographics of Alaska" },
	{ "Amersfoort"	, "<", "D_Amersfoort"								, "[ESRI] datum RD_NEW" },
	{ "Amersfoort"	, "<", "GCS_Amersfoort"								, "[ESRI] GCS RD_NEW" },
	{ "Amersfoort / RD New", "<", "Amersfoort_RD_New"					, "[ESRI] RD_NEW" },
	{ "apian"		, " ", "Apian_Globular_I"							, "*) Apian Globular I" },
	{ "august"		, " ", "August_Epicycloidal"						, "*) August Epicycloidal" },
	{ "bacon"		, " ", "Bacon_Globular"								, "*) Bacon Globular" },
	{ "bipc"		, " ", "Bipolar_conic_of_western_hemisphere"		, "*) Bipolar conic of western hemisphere" },
	{ "boggs"		, " ", "Boggs_Eumorphic"							, "*) Boggs Eumorphic" },
	{ "bonne"		, " ", "Bonne"										, "Bonne (Werner lat_1=90)" },
	{ "cass"		, " ", "Cassini_Soldner"							, "Cassini" },
	{ "cass"		, "<", "Cassini"									, "[ESRI] Cassini" },
	{ "cc"			, " ", "Central_Cylindrical"						, "*) Central Cylindrical" },
	{ "cea"			, " ", "Cylindrical_Equal_Area"						, "Equal Area Cylindrical, alias: Lambert Cyl.Eq.A., Normal Authalic Cyl. (FME), Behrmann (SP=30), Gall Orthogr. (SP=45)" },
	{ "cea"			, "<", "Behrmann"									, "[ESRI] Behrmann (standard parallel = 30)" },
	{ "chamb"		, " ", "Chamberlin_Trimetric"						, "*) Chamberlin Trimetric" },
	{ "collg"		, " ", "Collignon"									, "*) Collignon" },
	{ "crast"		, " ", "Craster_Parabolic"							, "[ESRI] Craster Parabolic (Putnins P4)" },
	{ "denoy"		, " ", "Denoyer_Semi_Elliptical"					, "*) Denoyer Semi-Elliptical" },
	{ "eck1"		, " ", "Eckert_I"									, "*) Eckert I" },
	{ "eck2"		, " ", "Eckert_II"									, "*) Eckert II" },
	{ "eck3"		, " ", "Eckert_III"									, "*) Eckert III" },
	{ "eck4"		, " ", "Eckert_IV"									, "Eckert IV" },
	{ "eck5"		, " ", "Eckert_V"									, "*) Eckert V" },
	{ "eck6"		, " ", "Eckert_VI"									, "Eckert VI" },
	{ "eqearth"		, " ", "Equal_Earth"								, "*) Equal Earth" },
	{ "eqc"			, " ", "Equirectangular"							, "Equidistant Cylindrical (Plate Caree)" },
	{ "eqc"			, "<", "Equidistant_Cylindrical"					, "[ESRI] Equidistant Cylindrical (Plate Caree)" },
	{ "eqc"			, "<", "Plate_Carree"								, "[ESRI] Equidistant Cylindrical (Plate Caree)" },
	{ "eqdc"		, " ", "Equidistant_Conic"							, "*) Equidistant Conic" },
	{ "euler"		, " ", "Euler"										, "*) Euler" },
	{ "etmerc"		, " ", "Extended_Transverse_Mercator"				, "*) Extended Transverse Mercator" },
	{ "fahey"		, " ", "Fahey"										, "*) Fahey" },
	{ "fouc"		, " ", "Foucault"									, "*) Foucaut" },
	{ "fouc_s"		, " ", "Foucault_Sinusoidal"						, "*) Foucaut Sinusoidal" },
	{ "gall"		, " ", "Gall_Stereographic"							, "Gall (Gall Stereographic)" },
	{ "geocent"		, " ", "Geocentric"									, "*) Geocentric" },
	{ "geos"		, " ", "GEOS"										, "Geostationary Satellite View" },
	{ "gins8"		, " ", "Ginsburg_VIII"								, "*) Ginsburg VIII (TsNIIGAiK)" },
	{ "gn_sinu"		, " ", "General_Sinusoidal_Series"					, "*) General Sinusoidal Series" },
	{ "gnom"		, " ", "Gnomonic"									, "Gnomonic" },
	{ "goode"		, " ", "Goode_Homolosine"							, "*) Goode Homolosine" },
	{ "gs48"		, " ", "Mod_Stererographics_48"						, "*) Mod. Stererographics of 48 U.S." },
	{ "gs50"		, " ", "Mod_Stererographics_50"						, "*) Mod. Stererographics of 50 U.S." },
	{ "hammer"		, " ", "Hammer_Eckert_Greifendorff"					, "*) Hammer & Eckert-Greifendorff" },
	{ "hatano"		, " ", "Hatano_Asymmetrical_Equal_Area"				, "*) Hatano Asymmetrical Equal Area" },
	{ "igh"			, " ", "Interrupted_Goodes_Homolosine"				, "*) Interrupted Goode's Homolosine" },
	{ "igh_o"		, " ", "Interrupted_Goodes_Homolosine_Ocean"		, "*) Interrupted Goode's Homolosine (Ocean)" },
	{ "imw_p"		, " ", "International_Map_of_the_World_Polyconic"	, "*) International Map of the World Polyconic" },
	{ "kav5"		, " ", "Kavraisky_V"								, "*) Kavraisky V" },
	{ "kav7"		, " ", "Kavraisky_VII"								, "*) Kavraisky VII" },
	{ "krovak"		, " ", "Krovak"										, "Krovak" },
	{ "labrd"		, " ", "Laborde_Oblique_Mercator"					, "*) Laborde" },
	{ "laea"		, " ", "Lambert_Azimuthal_Equal_Area"				, "Lambert Azimuthal Equal Area" },
	{ "lagrng"		, " ", "Lagrange"									, "*) Lagrange" },
	{ "larr"		, " ", "Larrivee"									, "*) Larrivee" },
	{ "lask"		, " ", "Laskowski"									, "*) Laskowski" },
	{ "lcc"			, "<", "Lambert_Conformal_Conic_1SP"				, "Lambert Conformal Conic (1 standard parallel)" },
	{ "lcc"			, "<", "Lambert_Conformal_Conic_2SP"				, "Lambert Conformal Conic (2 standard parallels)" },
	{ "lcc"			, " ", "Lambert_Conformal_Conic"					, "Lambert Conformal Conic" },
	{ "lcca"		, " ", "Lambert_Conformal_Conic_Alternative"		, "*) Lambert Conformal Conic Alternative" },
	{ "leac"		, " ", "Lambert_Equal_Area_Conic"					, "*) Lambert Equal Area Conic" },
	{ "lee_os"		, " ", "Lee_Oblated_Stereographic"					, "*) Lee Oblated Stereographic" },
	{ "loxim"		, " ", "Loximuthal"									, "[ESRI] Loximuthal" },
	{ "lsat"		, " ", "Space_oblique_for_LANDSAT"					, "*) Space oblique for LANDSAT" },
	{ "mbt_s"		, " ", "McBryde_Thomas_Flat_Polar_Sine"				, "*) McBryde-Thomas Flat-Polar Sine" },
	{ "mbt_fps"		, " ", "McBryde_Thomas_Flat_Polar_Sine_2"			, "*) McBryde-Thomas Flat-Pole Sine (No. 2)" },
	{ "mbtfpp"		, " ", "McBryde_Thomas_Flat_Polar_Parabolic"		, "*) McBride-Thomas Flat-Polar Parabolic" },
	{ "mbtfpq"		, " ", "Flat_Polar_Quartic"							, "[ESRI] McBryde-Thomas Flat-Polar Quartic" },
	{ "mbtfps"		, " ", "McBryde_Thomas_Flat_Polar_Sinusoidal"		, "*) McBryde-Thomas Flat-Polar Sinusoidal" },
	{ "merc"		, " ", "Mercator"									, "[ESRI] Mercator" },
	{ "merc"		, "<", "Mercator_1SP"								, "Mercator (1 standard parallel)" },
	{ "merc"		, "<", "Mercator_2SP"								, "Mercator (2 standard parallels)" },
	{ "mil_os"		, " ", "Miller_Oblated_Stereographic"				, "*) Miller Oblated Stereographic" },
	{ "mill"		, " ", "Miller_Cylindrical"							, "Miller Cylindrical" },
	{ "moll"		, " ", "Mollweide"									, "Mollweide" },
	{ "murd1"		, " ", "Murdoch_I"									, "*) Murdoch I" },
	{ "murd2"		, " ", "Murdoch_II"									, "*) Murdoch II" },
	{ "murd3"		, " ", "Murdoch_III"								, "*) Murdoch III" },
	{ "nell"		, " ", "Nell"										, "*) Nell" },
	{ "nell_h"		, " ", "Nell_Hammer"								, "*) Nell-Hammer" },
	{ "nicol"		, " ", "Nicolosi_Globular"							, "*) Nicolosi Globular" },
	{ "nsper"		, " ", "Near_sided_perspective"						, "*) Near-sided perspective" },
	{ "nzmg"		, " ", "New_Zealand_Map_Grid"						, "New Zealand Map Grid" },
	{ "ob_tran"		, " ", "General_Oblique_Transformation"				, "*) General Oblique Transformation" },
	{ "ocea"		, " ", "Oblique_Cylindrical_Equal_Area"				, "*) Oblique Cylindrical Equal Area" },
	{ "oea"			, " ", "Oblated_Equal_Area"							, "*) Oblated Equal Area" },
	{ "omerc"		, " ", "Hotine_Oblique_Mercator"					, "Oblique Mercator" },
	{ "omerc"		, "<", "Oblique_Mercator"							, "Oblique Mercator" },
	{ "ortel"		, " ", "Ortelius_Oval"								, "*) Ortelius Oval" },
	{ "ortho"		, " ", "Orthographic"								, "Orthographic (ESRI: World from Space)" },
	{ "pconic"		, " ", "Perspective_Conic"							, "*) Perspective Conic" },
	{ "poly"		, " ", "Polyconic"									, "*) Polyconic (American)" },
	{ "putp1"		, " ", "Putnins_P1"									, "*) Putnins P1" },
	{ "putp2"		, " ", "Putnins_P2"									, "*) Putnins P2" },
	{ "putp3"		, " ", "Putnins_P3"									, "*) Putnins P3" },
	{ "putp3p"		, " ", "Putnins_P3'"								, "*) Putnins P3'" },
	{ "putp4p"		, " ", "Putnins_P4'"								, "*) Putnins P4'" },
	{ "putp5"		, " ", "Putnins_P5"									, "*) Putnins P5" },
	{ "putp5p"		, " ", "Putnins_P5'"								, "*) Putnins P5'" },
	{ "putp6"		, " ", "Putnins_P6"									, "*) Putnins P6" },
	{ "putp6p"		, " ", "Putnins_P6'"								, "*) Putnins P6'" },
	{ "qua_aut"		, " ", "Quartic_Authalic"							, "[ESRI] Quart	c Authalic" },
	{ "robin"		, " ", "Robinson"									, "Robinson" },
	{ "rouss"		, " ", "Roussilhe_Stereographic"					, "*) Roussilhe Stereographic" },
	{ "rpoly"		, " ", "Rectangular_Polyconic"						, "*) Rectangular Polyconic" },
	{ "sinu"		, " ", "Sinusoidal"									, "Sinusoidal (Sanson-Flamsteed)" },
	{ "somerc"		, " ", "Hotine_Oblique_Mercator"					, "Swiss Oblique Mercator" },
	{ "somerc"		, "<", "Swiss_Oblique_Cylindrical"					, "Swiss Oblique Cylindrical" },
	{ "somerc"		, "<", "Hotine_Oblique_Mercator_Azimuth_Center"		, "[ESRI] Swiss Oblique Mercator/Cylindrical" },
	{ "stere"		, "<", "Polar_Stereographic"						, "Stereographic" },
	{ "stere"		, " ", "Stereographic"								, "[ESRI] Stereographic" },
	{ "sterea"		, " ", "Oblique_Stereographic"						, "Oblique Stereographic Alternative" },
	{ "sterea"		, "<", "Double_Stereographic"						, "[ESRI]" },
	{ "gstmerc"		, " ", "Gauss_Schreiber_Transverse_Mercator"		, "*) Gauss-Schreiber Transverse Mercator (aka Gauss-Laborde Reunion)" },
	{ "tcc"			, " ", "Transverse_Central_Cylindrical"				, "*) Transverse Central Cylindrical" },
	{ "tcea"		, " ", "Transverse_Cylindrical_Equal_Area"			, "*) Transverse Cylindrical Equal Area" },
	{ "tissot"		, " ", "Tissot_Conic"								, "*) Tissot Conic" },
	{ "tmerc"		, " ", "Transverse_Mercator"						, "*) Transverse Mercator" },
	{ "tmerc"		, "<", "Gauss_Kruger"								, "[ESRI] DHDN" },
	{ "tpeqd"		, " ", "Two_Point_Equidistant"						, "*) Two Point Equidistant" },
	{ "tpers"		, " ", "Tilted_perspective"							, "*) Tilted perspective" },
	{ "ups"			, " ", "Universal_Polar_Stereographic"				, "*) Universal Polar Stereographic" },
	{ "urm5"		, " ", "Urmaev_V"									, "*) Urmaev V" },
	{ "urmfps"		, " ", "Urmaev_Flat_Polar_Sinusoidal"				, "*) Urmaev Flat-Polar Sinusoidal" },
	{ "utm"			, ">", "Transverse_Mercator"						, "*) Universal Transverse Mercator (UTM)" },
	{ "vandg"		, "<", "Van_Der_Grinten_I"							, "[ESRI] van der Grinten (I)" },
	{ "vandg"		, " ", "VanDerGrinten"								, "van der Grinten (I)" },
	{ "vandg2"		, " ", "VanDerGrinten_II"							, "*) van der Grinten II" },
	{ "vandg3"		, " ", "VanDerGrinten_III"							, "*) van der Grinten III" },
	{ "vandg4"		, " ", "VanDerGrinten_IV"							, "*) van der Grinten IV" },
	{ "vitk1"		, " ", "Vitkovsky_I"								, "*) Vitkovsky I" },
	{ "wag1"		, " ", "Wagner_I"									, "*) Wagner I (Kavraisky VI)" },
	{ "wag2"		, " ", "Wagner_II"									, "*) Wagner II" },
	{ "wag3"		, " ", "Wagner_III"									, "*) Wagner III" },
	{ "wag4"		, " ", "Wagner_IV"									, "*) Wagner IV" },
	{ "wag5"		, " ", "Wagner_V"									, "*) Wagner V" },
	{ "wag6"		, " ", "Wagner_VI"									, "*) Wagner VI" },
	{ "wag7"		, " ", "Wagner_VII"									, "*) Wagner VII" },
	{ "webmerc"		, " ", "Mercator_1SP"								, "Web Mercator" },
	{ "webmerc"		, "<", "Mercator_Auxiliary_Sphere"					, "[ESRI] Web Mercator" },
	{ "weren"		, " ", "Werenskiold_I"								, "*) Werenskiold I" },
	{ "wink1"		, " ", "Winkel_I"									, "[ESRI] Winkel I" },
	{ "wink2"		, " ", "Winkel_II"									, "[ESRI] Winkel II" },
	{ "wintri"		, " ", "Winkel_Tripel"								, "[ESRI] Winkel Tripel" },

//	--- general projection parameters ---
	{ "alpha"		, " ", "azimuth"				, "? Used with Oblique Mercator and possibly a few others" },
	{ "k"			, ">", "scale_factor"			, "Scaling factor (old name)" },
	{ "K"			, ">", "scale_factor"			, "? Scaling factor (old name)" },
	{ "k_0"			, " ", "scale_factor"			, "Scaling factor (new name)" },
	{ "lat_0"		, " ", "latitude_of_origin"		, "Latitude of origin" },
	{ "lat_0"		, "<", "latitude_of_center"		, "Latitude of center" },
	{ "lat_0"		, "<", "central_parallel"		, "[ESRI] Latitude of center" },
	{ "lat_1"		, " ", "standard_parallel_1"	, "Latitude of first standard parallel" },
	{ "lat_2"		, " ", "standard_parallel_2"	, "Latitude of second standard parallel" },
	{ "lat_ts"		, ">", "latitude_of_origin"		, "Latitude of true scale" },
	{ "lon_0"		, " ", "central_meridian"		, "Central meridian" },
	{ "lon_0"		, "<", "longitude_of_center"	, "Longitude of center" },
	{ "lonc"		, ">", "longitude_of_center"	, "? Longitude used with Oblique Mercator and possibly a few others" },
	{ "x_0"			, " ", "false_easting"			, "False easting" },
	{ "y_0"			, " ", "false_northing"			, "False northing" },

//	--- special projection parameters ---
//	{ "azi"			, " ", "", "" },
//	{ "belgium"		, " ", "", "" },
//	{ "beta"		, " ", "", "" },
//	{ "czech"		, " ", "", "" },
//	{ "gamma"		, " ", "", "" },
//	{ "geoc"		, " ", "", "" },
//	{ "guam"		, " ", "", "" },
	{ "h"			, " ", "satellite_height", "Satellite height (geos - Geostationary Satellite View)" },
//	{ "lat_b"		, " ", "", "" },
//	{ "lat_t"		, " ", "", "" },
//	{ "lon_1"		, " ", "", "" },
//	{ "lon_2"		, " ", "", "" },
//	{ "lsat"		, " ", "", "" },
//	{ "m"			, " ", "", "" },
//	{ "M"			, " ", "", "" },
//	{ "n"			, " ", "", "" },
//	{ "no_cut"		, " ", "", "" },
//	{ "no_off"		, " ", "", "" },
//	{ "no_rot"		, " ", "", "" },
//	{ "ns"			, " ", "", "" },
//	{ "o_alpha"		, " ", "", "" },
//	{ "o_lat_1"		, " ", "", "" },
//	{ "o_lat_2"		, " ", "", "" },
//	{ "o_lat_c"		, " ", "", "" },
//	{ "o_lat_p"		, " ", "", "" },
//	{ "o_lon_1"		, " ", "", "" },
//	{ "o_lon_2"		, " ", "", "" },
//	{ "o_lon_c"		, " ", "", "" },
//	{ "o_lon_p"		, " ", "", "" },
//	{ "o_proj"		, " ", "", "" },
//	{ "over"		, " ", "", "" },
//	{ "p"			, " ", "", "" },
//	{ "path"		, " ", "", "" },
//	{ "q"			, " ", "", "" },
//	{ "R"			, " ", "", "" },
//	{ "R_a"			, " ", "", "" },
//	{ "R_A"			, " ", "", "" },
//	{ "R_g"			, " ", "", "" },
//	{ "R_h"			, " ", "", "" },
//	{ "R_lat_a"		, " ", "", "" },
//	{ "R_lat_g"		, " ", "", "" },
//	{ "rot"			, " ", "", "" },
//	{ "R_V"			, " ", "", "" },
//	{ "s"			, " ", "", "" },
//	{ "sym"			, " ", "", "" },
//	{ "t"			, " ", "", "" },
//	{ "theta"		, " ", "", "" },
//	{ "tilt"		, " ", "", "" },
//	{ "vopt"		, " ", "", "" },
//	{ "W"			, " ", "", "" },
//	{ "westo"		, " ", "", "" },

//	--- core projection types and parameters that don't require explicit translation ---
//	{ "lonlat"		, " ", "GEOGCS", "Lat/long (Geodetic)"	},
//	{ "latlon"		, ">", "GEOGCS", "Lat/long (Geodetic alias)"	},
//	{ "latlong"		, ">", "GEOGCS", "Lat/long (Geodetic alias)"	},
//	{ "longlat"		, ">", "GEOGCS", "Lat/long (Geodetic alias)"	},

//	{ "a"			, " ", "", "Semimajor radius of the ellipsoid axis"	},
//	{ "axis"		, " ", "", "Axis orientation (new in 4.8.0)"	},
//	{ "b			, " ", "", "Semiminor radius of the ellipsoid axis"	},
//	{ "datum		, " ", "", "Datum name (see `proj -ld`)"	},
//	{ "ellps		, " ", "", "Ellipsoid name (see `proj -le`)"	},
//	{ "nadgrids		, " ", "", "Filename of NTv2 grid file to use for datum transforms (see below)"	},
//	{ "no_defs		, " ", "", "Don't use the /usr/share/proj/proj_def.dat defaults file"	},
//	{ "pm			, " ", "", "Alternate prime meridian (typically a city name, see below)"	},
//	{ "proj			, " ", "", "Projection name (see `proj -l`)"	},
//	{ "to_meter		, " ", "", "Multiplier to convert map units to 1.0m"	},
//	{ "towgs84		, " ", "", "3 or 7 term datum transform parameters (see below)"	},
//	{ "units		, " ", "", "meters, US survey feet, etc."	},
//	{ "south		, " ", "", "Denotes southern hemisphere UTM zone"	},
//	{ "zone			, " ", "", "UTM zone"	},
//	{ "lon_wrap"	, " ", "", "Center longitude to use for wrapping (see below)"	},
//	{ "over"		, " ", "", "Allow longitude output outside -180 to 180 range, disables wrapping (see below)"	},
	{ "", "", "", "" }	// end of records
};

	//-----------------------------------------------------
	Dictionary.Destroy();
	Dictionary.Set_Name("Proj.4-WKT Dictionary");

	if( Direction == 0 )
	{
		Dictionary.Add_Field("PROJ4", SG_DATATYPE_String);
		Dictionary.Add_Field("DIR"  , SG_DATATYPE_String);
		Dictionary.Add_Field("WKT"  , SG_DATATYPE_String);
		Dictionary.Add_Field("DESC" , SG_DATATYPE_String);

		for(int i=0; *Translation[i][0]; i++)
		{
			CSG_Table_Record &Entry = *Dictionary.Add_Record();

			Entry.Set_Value(0, Translation[i][0]);
			Entry.Set_Value(1, Translation[i][1]);
			Entry.Set_Value(2, Translation[i][2]);
			Entry.Set_Value(3, Translation[i][3]);
		}
	}
	else if( Direction > 0 )	// Proj4 to WKT
	{
		Dictionary.Add_Field("PROJ4", SG_DATATYPE_String);
		Dictionary.Add_Field("WKT"  , SG_DATATYPE_String);

		for(int i=0; *Translation[i][0]; i++)
		{
			if( Translation[i][1][0] != '<' )	// only WKT to Proj4
			{
				CSG_Table_Record &Entry = *Dictionary.Add_Record();

				Entry.Set_Value(0, Translation[i][0]);
				Entry.Set_Value(1, Translation[i][2]);
			}
		}
	}
	else if( Direction < 0 )	// WKT to Proj4
	{
		Dictionary.Add_Field("WKT"  , SG_DATATYPE_String);
		Dictionary.Add_Field("PROJ4", SG_DATATYPE_String);

		for(int i=0; *Translation[i][0]; i++)
		{
			if( Translation[i][1][0] != '>' )	// only Proj4 to WKT
			{
				CSG_Table_Record &Entry = *Dictionary.Add_Record();

				Entry.Set_Value(0, Translation[i][2]);
				Entry.Set_Value(1, Translation[i][0]);
			}
		}
	}

	return( Dictionary.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Projections::_Set_Dictionary(void)
{
	CSG_Table Table;

	return( _Set_Dictionary(Table,  1) && m_Proj4_to_WKT.Create(&Table, 0, 1, true)
		&&  _Set_Dictionary(Table, -1) && m_WKT_to_Proj4.Create(&Table, 0, 1, true)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Get_Projected	(CSG_Shapes *pSource, CSG_Shapes *pTarget, const CSG_Projection &Target)
{
	if( pSource && pSource->is_Valid() && pSource->Get_Projection().is_Okay() && Target.is_Okay() )
	{
		if( pSource->Get_Projection() == Target )
		{
			return( pTarget ? pTarget->Create(*pSource) : true );
		}

		if( pTarget )
		{
			pTarget->Create(*pSource); pSource = pTarget;
		}

		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 2); // Coordinate Transformation (Shapes)

		if( pTool )
		{
			CSG_Data_Manager Data; Data.Add(pSource); pTool->Set_Manager(&Data);

			pTool->Set_Parameter("SOURCE"    , pSource);
			pTool->Set_Parameter("CRS_STRING", Target.Get_WKT());
			pTool->Set_Parameter("COPY"      , false);
			pTool->Set_Parameter("PARALLEL"  , true);

			SG_UI_ProgressAndMsg_Lock(true);
			bool bResult = pTool->Execute();
			SG_UI_ProgressAndMsg_Lock(false);

			Data.Delete(pSource, true);
			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			return( bResult );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Get_Projected	(const CSG_Projection &Source, const CSG_Projection &Target, TSG_Point &Point)
{
	if( Source == Target )
	{
		return( true );
	}

	if( Source.is_Okay() && Target.is_Okay() )
	{
		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 29); // Single Coordinate Transformation

		if( pTool )
		{
			pTool->Set_Manager(NULL);
			pTool->Set_Parameter("TARGET_CRS", Target.Get_WKT());
			pTool->Set_Parameter("SOURCE_CRS", Source.Get_WKT());
			pTool->Set_Parameter("SOURCE_X"  , Point.x);
			pTool->Set_Parameter("SOURCE_Y"  , Point.y);

			SG_UI_ProgressAndMsg_Lock(true);
			bool bResult = pTool->Execute();
			SG_UI_ProgressAndMsg_Lock(false);

			if( bResult )
			{
				Point.x	= pTool->Get_Parameter("TARGET_X")->asDouble();
				Point.y	= pTool->Get_Parameter("TARGET_Y")->asDouble();
			}

			SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

			return( bResult );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool	SG_Get_Projected	(const CSG_Projection &Source, const CSG_Projection &Target, TSG_Rect &Rectangle)
{
	if( Source == Target )
	{
		return( true );
	}

	if( Source.is_Okay() && Target.is_Okay() )
	{
		CSG_Shapes Points(SHAPE_TYPE_Point); Points.Get_Projection().Create(Source);

		Points.Add_Shape()->Add_Point(Rectangle.xMin, Rectangle.yMin);
		Points.Add_Shape()->Add_Point(Rectangle.xMin, Rectangle.yMax);
		Points.Add_Shape()->Add_Point(Rectangle.xMax, Rectangle.yMax);
		Points.Add_Shape()->Add_Point(Rectangle.xMax, Rectangle.yMin);

		if( SG_Get_Projected(&Points, NULL, Target) )
		{
			Rectangle = Points.Get_Extent();

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	SG_Grid_Get_Geographic_Coordinates	(CSG_Grid *pGrid, CSG_Grid *pLon, CSG_Grid *pLat)
{
	bool bResult = false;

	if( pGrid && pGrid->is_Valid() && pGrid->Get_Projection().is_Okay() && (pLon || pLat) )
	{
		CSG_Grid Lon; if( !pLon ) { pLon = &Lon; } pLon->Create(pGrid->Get_System());
		CSG_Grid Lat; if( !pLat ) { pLat = &Lat; } pLat->Create(pGrid->Get_System());

		SG_RUN_TOOL(bResult, "pj_proj4", 17,	// geographic coordinate grids
				SG_TOOL_PARAMETER_SET("GRID", pGrid)
			&&	SG_TOOL_PARAMETER_SET("LON" , pLon )
			&&	SG_TOOL_PARAMETER_SET("LAT" , pLat )
		)
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
