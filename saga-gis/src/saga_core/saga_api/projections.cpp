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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Projection_Type	SG_Get_Projection_Type				(const CSG_String &Identifier)
{
	if( !Identifier.CmpNoCase("PROJCS") )	return( SG_PROJ_TYPE_CS_Projected  );
	if( !Identifier.CmpNoCase("GEOGCS") )	return( SG_PROJ_TYPE_CS_Geographic );
	if( !Identifier.CmpNoCase("GEOCCS") )	return( SG_PROJ_TYPE_CS_Geocentric );

	return( SG_PROJ_TYPE_CS_Undefined );
}

//---------------------------------------------------------
CSG_String			SG_Get_Projection_Type_Identifier	(TSG_Projection_Type Type)
{
	switch( Type )
	{
	case SG_PROJ_TYPE_CS_Projected :	return( "PROJCS"    );
	case SG_PROJ_TYPE_CS_Geographic:	return( "GEOGCS"    );
	case SG_PROJ_TYPE_CS_Geocentric:	return( "GEOCCS"    );
	default                        :	return( "UNDEFINED" );
	}
}

//---------------------------------------------------------
CSG_String			SG_Get_Projection_Type_Name			(TSG_Projection_Type Type)
{
	switch( Type )
	{
	case SG_PROJ_TYPE_CS_Projected :	return( _TL("Projected Coordinate System" ) );
	case SG_PROJ_TYPE_CS_Geographic:	return( _TL("Geographic Coordinate System") );
	case SG_PROJ_TYPE_CS_Geocentric:	return( _TL("Geocentric Coordinate System") );
	default                        :	return( _TL("Undefined Coordinate System" ) );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char	SG_Projection_Units[SG_PROJ_UNIT_Undefined + 1][3][32]	=
{
	{	"km"    , "Kilometers" , "Kilometer"					},
	{	"m"    	, "Meters"	   , "Meter"						},
	{	"dm"    , "Decimeters" , "Decimeter"					},
	{	"cm"    , "Centimeters", "Centimeter"					},
	{	"mm"    , "Millimeters", "Millimeter"					},
	{	"kmi"   , "Miles"      , "International Nautical Mile"	},
	{	"in"    , "Inches"     , "International Inch"			},
	{	"ft"    , "Feet"       , "International Foot"			},
	{	"yd"    , "Yards"      , "International Yard"			},
	{	"mi"    , "Miles"      , "International Statute Mile"	},
	{	"fath"  , "Fathoms"    , "International Fathom"			},
	{	"ch"    , "Chains"     , "International Chain"			},
	{	"link"  , "Links"      , "International Link"			},
	{	"us-in" , "Inches"     , "U.S. Surveyor's Inch"			},
	{	"us-ft" , "Feet"       , "U.S. Surveyor's Foot"			},
	{	"us-yd" , "Yards"      , "U.S. Surveyor's Yard"			},
	{	"us-ch" , "Chains"     , "U.S. Surveyor's Chain"		},
	{	"us-mi" , "Miles"      , "U.S. Surveyor's Statute Mile"	},
	{	"ind-yd", "Yards"      , "Indian Yard"					},
	{	"ind-ft", "Feet"       , "Indian Foot"					},
	{	"ind-ch", "Chains"     , "Indian Chain"					},
	{	""      , ""           , ""								}
};

//---------------------------------------------------------
// same as proj4.
TSG_Projection_Unit	SG_Get_Projection_Unit				(const CSG_String &Identifier)
{
	for(int i=0; i<SG_PROJ_UNIT_Undefined; i++)
	{
		if( !Identifier.CmpNoCase(SG_Projection_Units[i][0])
		||  !Identifier.CmpNoCase(SG_Projection_Units[i][2]) )
		{
			return( (TSG_Projection_Unit)i );
		}
	}

	return( !Identifier.CmpNoCase("metre") ? SG_PROJ_UNIT_Meter : SG_PROJ_UNIT_Undefined );
}

//---------------------------------------------------------
// same as proj4.
CSG_String			SG_Get_Projection_Unit_Identifier	(TSG_Projection_Unit Unit)
{
	if(	Unit < 0 || Unit > SG_PROJ_UNIT_Undefined )
		Unit	= SG_PROJ_UNIT_Undefined;

	return( SG_Projection_Units[Unit][0] );
}

//---------------------------------------------------------
CSG_String			SG_Get_Projection_Unit_Name			(TSG_Projection_Unit Unit, bool bSimple)
{
	if(	Unit < 0 || Unit > SG_PROJ_UNIT_Undefined )
		Unit	= SG_PROJ_UNIT_Undefined;

	return( SG_Projection_Units[Unit][bSimple ? 1 : 2] );
}

//---------------------------------------------------------
double				SG_Get_Projection_Unit_To_Meter		(TSG_Projection_Unit Unit)
{
	switch( Unit )
	{
	case SG_PROJ_UNIT_Kilometer        :	return( 1000.0 );
	case SG_PROJ_UNIT_Meter            :	return( 1.0 );
	case SG_PROJ_UNIT_Decimeter        :	return( 0.1 );
	case SG_PROJ_UNIT_Centimeter       :	return( 0.01 );
	case SG_PROJ_UNIT_Millimeter       :	return( 0.001 );
	case SG_PROJ_UNIT_Int_Nautical_Mile:	return( 1852.0 );
	case SG_PROJ_UNIT_Int_Inch         :	return( 0.0254 );
	case SG_PROJ_UNIT_Int_Foot         :	return( 0.3048 );
	case SG_PROJ_UNIT_Int_Yard         :	return( 0.9144 );
	case SG_PROJ_UNIT_Int_Statute_Mile :	return( 1609.344 );
	case SG_PROJ_UNIT_Int_Fathom       :	return( 1.8288 );
	case SG_PROJ_UNIT_Int_Chain        :	return( 20.1168 );
	case SG_PROJ_UNIT_Int_Link         :	return( 0.201168 );
	case SG_PROJ_UNIT_US_Inch          :	return( 1.0 / 39.37 );
	case SG_PROJ_UNIT_US_Foot          :	return( 0.304800609601219 );
	case SG_PROJ_UNIT_US_Yard          :	return( 0.914401828803658 );
	case SG_PROJ_UNIT_US_Chain         :	return( 20.11684023368047 );
	case SG_PROJ_UNIT_US_Statute_Mile  :	return( 1609.347218694437 );
	case SG_PROJ_UNIT_Indian_Yard      :	return( 0.91439523 );
	case SG_PROJ_UNIT_Indian_Foot      :	return( 0.30479841 );
	case SG_PROJ_UNIT_Indian_Chain     :	return( 20.11669506 );
	default                            :	return( 1.0 );
	}
}

//---------------------------------------------------------
bool				SG_Set_Projection_Unit		(const CSG_MetaData &m, TSG_Projection_Unit &Unit, CSG_String &Name, double &To_Meter)
{
	if( m("UNIT") )
	{
		if( m["UNIT"].Get_Property("name", Name) && (Unit = SG_Get_Projection_Unit(Name)) != SG_PROJ_UNIT_Undefined )
		{
			Name		= SG_Get_Projection_Unit_Name    (Unit);
			To_Meter	= SG_Get_Projection_Unit_To_Meter(Unit);
		}
		else if( !m["UNIT"].Get_Content().asDouble(To_Meter) || To_Meter <= 0.0 )
		{
			To_Meter	=  1.0;
		}

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
	m_Name			= Projection.m_Name;
	m_Type			= Projection.m_Type;
	m_Unit			= Projection.m_Unit;
	m_Unit_To_Meter	= Projection.m_Unit_To_Meter;
	m_Unit_Name		= Projection.m_Unit_Name;
	m_WKT			= Projection.m_WKT;
	m_Proj4			= Projection.m_Proj4;
	m_Authority		= Projection.m_Authority;
	m_Authority_ID	= Projection.m_Authority_ID;

	return( true );
}

//---------------------------------------------------------
CSG_Projection::CSG_Projection(int Authority_ID, const SG_Char *Authority)
{
	Destroy();

	Create(Authority_ID, Authority);
}

bool CSG_Projection::Create(int Authority_ID, const SG_Char *Authority)
{
	return( Assign(Authority_ID, Authority) );
}

bool CSG_Projection::Assign(int Authority_ID, const SG_Char *Authority)
{
	return( Authority && *Authority
		? gSG_Projections.Get_Projection(*this, Authority, Authority_ID)
		: gSG_Projections.Get_Projection(*this,            Authority_ID)
	);
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
	Destroy();

	if( Projection.is_Empty() )
	{
		return( false );
	}

	//-----------------------------------------------------
	int				i;
	CSG_String		s;
	CSG_MetaData	m;

	switch( Format )
	{
	default:
		return( false );

	//-----------------------------------------------------
	case SG_PROJ_FMT_EPSG:
		return( Projection.asInt(i) && Assign(i, SG_T("EPSG")) );

	//-----------------------------------------------------
	case SG_PROJ_FMT_Proj4:
		if( !gSG_Projections.WKT_from_Proj4(s, Projection) )
		{
			return( false );
		}

		m_WKT	= s;
		m_Proj4	= Projection;

		m		= gSG_Projections.WKT_to_MetaData(m_WKT);

		break;

	//-----------------------------------------------------
	case SG_PROJ_FMT_WKT:
		m		= gSG_Projections.WKT_to_MetaData(Projection);

		if(	m.Get_Property("authority_name", s) && s.CmpNoCase("EPSG") == 0
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
	m_Type	= SG_Get_Projection_Type(m.Get_Name());

	SG_Set_Projection_Unit(m, m_Unit, m_Unit_Name, m_Unit_To_Meter);

	return( true );
}

//---------------------------------------------------------
void CSG_Projection::Destroy(void)
{
	m_Name			= _TL("undefined");
	m_Type			= SG_PROJ_TYPE_CS_Undefined;
	m_Unit			= SG_PROJ_UNIT_Undefined;
	m_Unit_To_Meter	= 1.0;
	m_Unit_Name		.Clear();
	m_WKT			.Clear();
	m_Proj4			.Clear();
	m_Authority		.Clear();
	m_Authority_ID	= -1;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::Load(const CSG_String &File_Name, TSG_Projection_Format Format)
{
	CSG_File	Stream;

	if( Stream.Open(File_Name, SG_FILE_R, false) )
	{
		CSG_String	s;

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
				CSG_String	s(m_WKT);

				Stream.Write((void *)s.b_str(), s.Length());

				return( true );
			}
			break;

		case SG_PROJ_FMT_Proj4:
			if( Stream.Open(File_Name, SG_FILE_W, false) )
			{
				CSG_String	s(m_Proj4);

				Stream.Write((void *)s.b_str(), s.Length());

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

	if( (pEntry = Projection.Get_Child("OGC_WKT")) != NULL )
	{
		Assign(pEntry->Get_Content(), SG_PROJ_FMT_WKT);

		if( (pEntry = Projection.Get_Child("PROJ4")) != NULL )
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
	Projection.Del_Children();

	Projection.Add_Child("OGC_WKT", m_WKT      );
	Projection.Add_Child("PROJ4"  , m_Proj4    );
	Projection.Add_Child("EPSG"   , Get_EPSG() );

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::Set_GCS_WGS84(void)
{
	return( Create(
		"GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,"
		"AUTHORITY[\"EPSG\",\"7030\"]],"
		"AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,"
		"AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,"
		"AUTHORITY[\"EPSG\",\"9122\"]],"
		"AUTHORITY[\"EPSG\",\"4326\"]]"
	) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::is_Equal(const CSG_Projection &Projection)	const
{
	if( !m_Authority.is_Empty() && !Projection.m_Authority.is_Empty() )
	{
		return(	m_Authority.CmpNoCase(Projection.m_Authority) == 0 && m_Authority_ID == Projection.m_Authority_ID );
	}

	return( m_Proj4.CmpNoCase(Projection.m_Proj4) == 0 );
}

//---------------------------------------------------------
CSG_String CSG_Projection::Get_Description(void)	const
{
	CSG_String	s;

	s	= SG_Get_Projection_Type_Name(m_Type);

	if( is_Okay() )
	{
		if( m_Authority.Length() > 0 && m_Authority_ID > 0 )
		{
			s	+= CSG_String::Format(" [%s %d]", m_Authority.c_str(), m_Authority_ID);
		}

		s	+= ":\n" + m_Name;

		if( m_Proj4.Length() > 0 )
		{
			s	+= "\n[" + m_Proj4 + "]";
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
CSG_Projections::CSG_Projections(const CSG_String &File_DB)
{
	_On_Construction();

	Create(File_DB);
}

bool CSG_Projections::Create(const CSG_String &File_DB)
{
	SG_UI_Msg_Lock(true);

	bool	bResult	= Load_DB(File_DB);

	SG_UI_Msg_Lock(false);

	return( bResult );
}

//---------------------------------------------------------
void CSG_Projections::_On_Construction(void)
{
	m_pProjections	= new CSG_Table;

	m_pProjections->Add_Field("srid"     , SG_DATATYPE_Int   );	// PRJ_FIELD_SRID
	m_pProjections->Add_Field("auth_name", SG_DATATYPE_String);	// PRJ_FIELD_AUTH_NAME
	m_pProjections->Add_Field("auth_srid", SG_DATATYPE_Int   );	// PRJ_FIELD_AUTH_SRID
	m_pProjections->Add_Field("srtext"   , SG_DATATYPE_String);	// PRJ_FIELD_SRTEXT
	m_pProjections->Add_Field("proj4text", SG_DATATYPE_String);	// PRJ_FIELD_PROJ4TEXT

	Reset_Dictionary();
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
bool CSG_Projections::Reset_Dictionary(void)
{
	_Set_Dictionary(m_Proj4_to_WKT,  1);
	_Set_Dictionary(m_WKT_to_Proj4, -1);

	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::Load_Dictionary(const CSG_String &File_Name)
{
	CSG_Table	Table;

	if( SG_File_Exists(File_Name) && Table.Create(File_Name) && Table.Get_Field_Count() >= 3 )
	{
		CSG_Table	Proj4_to_WKT(&Table), WKT_to_Proj4(&Table);

		for(int i=0; i<Table.Get_Count(); i++)
		{
			switch( Table[i].asString(1)[0] )
			{
			case SG_T('<'):	// ignore proj4 to wkt translation
				WKT_to_Proj4.Add_Record(Table.Get_Record(i));
				break;

			case SG_T('>'):	// ignore wkt to proj4 translation
				Proj4_to_WKT.Add_Record(Table.Get_Record(i));
				break;

			default:
				Proj4_to_WKT.Add_Record(Table.Get_Record(i));
				WKT_to_Proj4.Add_Record(Table.Get_Record(i));
			}
		}

		m_Proj4_to_WKT.Create(&Proj4_to_WKT, 0, 2, true);
		m_WKT_to_Proj4.Create(&WKT_to_Proj4, 2, 0, true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::Save_Dictionary(const CSG_String &File_Name)
{
	CSG_Table	Table;

	return( _Set_Dictionary(Table, 0) && Table.Save(File_Name) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::Load_DB(const CSG_String &File_Name, bool bAppend)
{
	CSG_Table	Table;

	if( SG_File_Exists(File_Name) && Table.Create(File_Name) )
	{
		if( !bAppend )
		{
			Destroy();
		}

		Table.Set_Index(PRJ_FIELD_SRTEXT, TABLE_INDEX_Ascending);

		for(int i=0; i<Table.Get_Count() && SG_UI_Process_Set_Progress(i, Table.Get_Count()); i++)
		{
			m_pProjections->Add_Record(Table.Get_Record_byIndex(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::Save_DB(const CSG_String &File_Name)
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
bool CSG_Projections::Add(const SG_Char *WKT, const SG_Char *Proj4, const SG_Char *Authority, int Authority_ID)
{
	CSG_Table_Record	*pProjection	= m_pProjections->Add_Record();

	pProjection->Set_Value(PRJ_FIELD_SRID     , m_pProjections->Get_Count());
	pProjection->Set_Value(PRJ_FIELD_AUTH_NAME, Authority);
	pProjection->Set_Value(PRJ_FIELD_AUTH_SRID, Authority_ID);
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

		Projection.m_Authority		= pRecord->asString(PRJ_FIELD_AUTH_NAME);
		Projection.m_Authority_ID	= pRecord->asInt   (PRJ_FIELD_AUTH_SRID);
		Projection.m_WKT			= pRecord->asString(PRJ_FIELD_SRTEXT   );
		Projection.m_Proj4			= pRecord->asString(PRJ_FIELD_PROJ4TEXT);

		CSG_MetaData	m	= WKT_to_MetaData(Projection.m_WKT);

		Projection.m_Name	= m.Get_Property("name");
		Projection.m_Type	= !m.Get_Name().Cmp("GEOCCS") ? SG_PROJ_TYPE_CS_Geocentric
							: !m.Get_Name().Cmp("GEOGCS") ? SG_PROJ_TYPE_CS_Geographic
							: !m.Get_Name().Cmp("PROJCS") ? SG_PROJ_TYPE_CS_Projected
							: SG_PROJ_TYPE_CS_Undefined;

		SG_Set_Projection_Unit(m, Projection.m_Unit, Projection.m_Unit_Name, Projection.m_Unit_To_Meter);
	}

	return( Projection );
}

//---------------------------------------------------------
bool CSG_Projections::Get_Projection(CSG_Projection &Projection, int EPSG_ID)	const
{
	return( Get_Projection(Projection, "", EPSG_ID) );
}

bool CSG_Projections::Get_Projection(CSG_Projection &Projection, const CSG_String &Authority, int Authority_ID)	const
{
	for(int i=0; i<m_pProjections->Get_Count(); i++)
	{
		CSG_Table_Record	*pProjection	= m_pProjections->Get_Record(i);

		if( Authority_ID == pProjection->asInt(PRJ_FIELD_AUTH_SRID)
		&& (Authority.is_Empty() || Authority.CmpNoCase(pProjection->asString(PRJ_FIELD_AUTH_NAME)) == 0) )
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

	Proj4.Printf("+init=epsg:%d ", EPSG_Code);

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
	CSG_String	Names;

	for(int i=0; i<Get_Count(); i++)
	{
		CSG_Table_Record	*pProjection	= m_pProjections->Get_Record(i);

		CSG_String	WKT		= pProjection->asString(PRJ_FIELD_SRTEXT);
		int			SRID	= pProjection->asInt   (PRJ_FIELD_SRID);

		TSG_Projection_Type	iType;

		iType	= !WKT.BeforeFirst('[').Cmp("PROJCS") ? SG_PROJ_TYPE_CS_Projected
				: !WKT.BeforeFirst('[').Cmp("GEOGCS") ? SG_PROJ_TYPE_CS_Geographic
				: !WKT.BeforeFirst('[').Cmp("GEOCCS") ? SG_PROJ_TYPE_CS_Geocentric
				: SG_PROJ_TYPE_CS_Undefined;

		if( Type == SG_PROJ_TYPE_CS_Undefined )
		{
			Names	+= CSG_String::Format("{%d}%s: %s|", SRID,
				SG_Get_Projection_Type_Name(iType).c_str(),
				WKT.AfterFirst('\"').BeforeFirst('\"').c_str()
			);
		}
		else if( Type == iType )
		{
			Names	+= CSG_String::Format("{%d}%s|", SRID,
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
	int			i, l;
	CSG_String	Key;
	CSG_Strings	Content;

	//-----------------------------------------------------
	Content.Add("");

	for(i=0, l=-1; l!=0 && i<(int)WKT.Length(); i++)
	{
		if( l < 0 )	// read key
		{
			switch( WKT[i] )
			{
			default :           Key += WKT[i]; break;
			case ' ':	                       break;
			case '[': case '(':	l    = 1;      break;
			case ')': case ']':	               return( false );
			}
		}
		else		// read content
		{
			bool	bAdd;

			switch( WKT[i] )
			{
			default:				bAdd	= true;		break;
			case '\"': case ' ':	bAdd	= false;	break;
			case '[' : case '(':	bAdd	= ++l > 1;	break;
			case ']' : case ')':	bAdd	= l-- > 1;	break;
			case ',' : if( !(bAdd = l > 1) )	Content.Add("");	break;
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
	if( !Key.Cmp("AUTHORITY" ) && Content.Get_Count() == 2 )	// AUTHORITY  ["<name>", "<code>"]
	{
		MetaData.Add_Property("authority_name", Content[0]);
		MetaData.Add_Property("authority_code", Content[1]);

		return( true );
	}

	CSG_MetaData	*pKey	= MetaData.Add_Child(Key);

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

	if( (!Key.Cmp("TOWGS84"   ) && Content.Get_Count() >= 7) )	// TOWGS84    [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
	{
		pKey->Add_Child("dx"     , Content[0]);
		pKey->Add_Child("dy"     , Content[1]);
		pKey->Add_Child("dz"     , Content[2]);
		pKey->Add_Child("ex"     , Content[3]);
		pKey->Add_Child("ey"     , Content[4]);
		pKey->Add_Child("ez"     , Content[5]);
		pKey->Add_Child("ppm"    , Content[6]);
	}

	if( (!Key.Cmp("PROJECTION") && Content.Get_Count() >= 1) )	// PROJECTION ["<name>" {,<authority>}]
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
// DATUM  ["<name>",
//     SPHEROID["<name>", <semi-major axis>, <inverse flattening>]
//    *TOWGS84 [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
// ]
//---------------------------------------------------------
bool CSG_Projections::_WKT_to_Proj4_Set_Datum(CSG_String &Proj4, const CSG_MetaData &WKT) const
{
	double	a, b;

	if(	!WKT("SPHEROID")
	||	 WKT["SPHEROID"].Get_Children_Count() != 2
	||	!WKT["SPHEROID"][0].Get_Content().asDouble(a) || a <= 0.0
	||	!WKT["SPHEROID"][1].Get_Content().asDouble(b) || b <  0.0 )
	{
		return( false );
	}

	b	= b > 0.0 ? a - a / b : a;

	Proj4	+= CSG_String::Format(" +a=%f", a);	// Semimajor radius of the ellipsoid axis
	Proj4	+= CSG_String::Format(" +b=%f", b);	// Semiminor radius of the ellipsoid axis

	if(	WKT("TOWGS84") && WKT["TOWGS84"].Get_Children_Count() == 7 )
	{
		Proj4	+= " +towgs84=";

		for(int i=0; i<7; i++)
		{
			if( i > 0 )
			{
				Proj4	+= ",";
			}

			Proj4	+= WKT["TOWGS84"][i].Get_Content();
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Projections::WKT_to_Proj4(CSG_String &Proj4, const CSG_String &WKT) const
{
	Proj4.Clear();

	CSG_MetaData	m	= WKT_to_MetaData(WKT);

	if( m.Get_Children_Count() == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			Authority_Code;
	CSG_String	Authority_Name;

	if(	m.Get_Property("authority_name", Authority_Name) && Authority_Name.CmpNoCase("EPSG") == 0
	&&	m.Get_Property("authority_code", Authority_Code) && EPSG_to_Proj4(Proj4, Authority_Code) )
	{	//	Proj4.Printf("+init=epsg:%d", Authority_Code);
		return( true );
	}

	//-----------------------------------------------------
	double	d;

	//-----------------------------------------------------
	// GEOCCS["<name>",
	//    DATUM  ["<name>", ...],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER], AXIS...
	// ]
	if( m.Cmp_Name("GEOCCS") )
	{
		Proj4	= CSG_String::Format("+proj=geocent");

		if( !m("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["DATUM"]) )
		{
			return( false );
		}

		if( m("PRIMEM") && m["PRIMEM"].Get_Content().asDouble(d) && d != 0.0 )
		{
			Proj4	+= CSG_String::Format(" +pm=%f", d);
		}

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
		Proj4	= "+proj=longlat";

		if( !m("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["DATUM"]) )
		{
			return( false );
		}

		if( m("PRIMEM") && m["PRIMEM"].Get_Content().asDouble(d) && d != 0.0 )
		{
			Proj4	+= CSG_String::Format(" +pm=%f", d);
		}
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
		Proj4	= "+proj=" + Proj4;

		if(	!m["GEOGCS"]("DATUM") || !_WKT_to_Proj4_Set_Datum(Proj4, m["GEOGCS"]["DATUM"]) )
		{
			return( false );
		}

		if( m("PRIMEM") && m["PRIMEM"].Get_Content().asDouble(d) && d != 0.0 )
		{
			Proj4	+= CSG_String::Format(" +pm=%f", d);
		}

		for(int i=0; i<m.Get_Children_Count(); i++)
		{
			if( m[i].Cmp_Name("PARAMETER") )
			{
				CSG_String	Parameter;

				if( m_WKT_to_Proj4.Get_Translation(m[i].Get_Property("name"), Parameter) )
				{
					Proj4	+= " +" + Parameter + "=" + m[i].Get_Content();
				}
				else
				{
					SG_UI_Msg_Add_Error(CSG_String::Format(">> WKT: %s [%s]", _TL("unknown parameter"), m[i].Get_Property("name")));
				}
			}
		}

		if( m("UNIT") && m["UNIT"].Get_Content().asDouble(d) && d != 0.0 && d != 1.0 )
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

	int		l, i	= Proj4.Find(CSG_String::Format("+%s=", Key.c_str()));

	if( i >= 0 )
	{
		for(++i, l=0; l<2 && i<(int)Proj4.Length(); i++)
		{
			switch( Proj4[i] )
			{
			case '=':	l++;	break;
			case '+':	l=2;	break;
			case ' ':	l=2;	break;
			default:
				if( l == 1 )
				{
					Value	+= Proj4[i];
				}
			}
		}
	}

	return( Value.Length() > 0 );
}

//---------------------------------------------------------
/* ellipsoids (a, b)
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
*/

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
				Value.Printf("SPHEROID[\"%s\",%s]", SG_STR_MBTOSG(ellipsoid[i][0]), SG_STR_MBTOSG(ellipsoid[i][1]));

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

	Value	= CSG_String::Format("SPHEROID[\"Ellipsoid\",%f,%f]", a, b);

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
				Value.Printf("DATUM[\"%s\",%s,TOWGS84[%s]]", SG_STR_MBTOSG(datum[i][0]), Spheroid.c_str(), SG_STR_MBTOSG(datum[i][2]));

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	if( _Proj4_Get_Ellipsoid(Spheroid, Proj4) )
	{
		Value	 = "DATUM[\"Datum\","+ Spheroid;

		if( _Proj4_Read_Parameter(ToWGS84, Proj4, "towgs84") )
		{
			Value	+= ",TOWGS84[" + ToWGS84 + "]";
		}
		else
		{
			Value	+= ",TOWGS84[0,0,0,0,0,0,0]";
		}

		Value	+= "]";

		return( true );
	}

	//-----------------------------------------------------
	Value	= "DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563],TOWGS84[0,0,0,0,0,0,0]]";

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
				Value.Printf("PRIMEM[\"%s\",%s]", SG_STR_MBTOSG(meridian[i][0]), SG_STR_MBTOSG(meridian[i][1]));

				return( true );
			}
		}

		double	d;

		if( Value.asDouble(d) && d != 0.0 )
		{
			Value.Printf("PRIMEM[\"Prime_Meridian\",%f]", d);

			return( true );
		}
	}

	//-----------------------------------------------------
	Value	= "PRIMEM[\"Greenwich\",0]";

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::_Proj4_Get_Unit(CSG_String &Value, const CSG_String &Proj4) const
{
	//-----------------------------------------------------
	TSG_Projection_Unit	Unit	= _Proj4_Read_Parameter(Value, Proj4, "units") ? SG_Get_Projection_Unit(Value) : SG_PROJ_UNIT_Undefined;

	if( Unit != SG_PROJ_UNIT_Undefined )
	{
		Value	= "UNIT[\"" + SG_Get_Projection_Unit_Name(Unit) + "\"," + SG_Get_String(SG_Get_Projection_Unit_To_Meter(Unit), -16) + "]";

		return( true );
	}

	//-----------------------------------------------------
	double	 d;

	if( _Proj4_Read_Parameter(Value, Proj4, "to_meter") && Value.asDouble(d) && d > 0.0 && d != 1.0 )
	{
		Value.Printf("UNIT[\"Unit\",%f]", d);

		return( true );
	}

	//-----------------------------------------------------
	Value	= "UNIT[\"metre\",1]";

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::WKT_from_Proj4(CSG_String &WKT, const CSG_String &Proj4) const
{
	CSG_String	Value, GeogCS, ProjCS;

	//-----------------------------------------------------
	if( !_Proj4_Read_Parameter(ProjCS, Proj4, "proj") )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 >> WKT: %s", _TL("no projection type defined")));

		return( false );
	}

	//-----------------------------------------------------
	// GEOGCS["<name>
	//    DATUM  ["<name>
	//        SPHEROID["<name>", <semi-major axis>, <inverse flattening>],
	//       *TOWGS84 [<dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>]
	//    ],
	//    PRIMEM ["<name>", <longitude>],
	//    UNIT   ["<name>", <conversion factor>],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER],
	//   *AXIS   ["<name>", NORTH|SOUTH|EAST|WEST|UP|DOWN|OTHER]
	// ]

	GeogCS	 = "GEOGCS[\"GCS\",";

	_Proj4_Get_Datum			(Value, Proj4);	GeogCS	+= Value;	GeogCS	+= SG_T(",");
	_Proj4_Get_Prime_Meridian	(Value, Proj4);	GeogCS	+= Value;	GeogCS	+= SG_T(",");

	GeogCS	+= "UNIT[\"degree\",0.01745329251994328]]";

	//-----------------------------------------------------
	if(	!ProjCS.CmpNoCase("lonlat") || !ProjCS.CmpNoCase("longlat")
	||	!ProjCS.CmpNoCase("latlon") || !ProjCS.CmpNoCase("latlong") )
	{
		WKT	= GeogCS;

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

		return( false );
	}

	WKT		 = CSG_String::Format("PROJCS[\"%s\",%s,PROJECTION[%s]", Value.c_str(), GeogCS.c_str(), Value.c_str());

	//-----------------------------------------------------
	// UTM ...

	if( !ProjCS.CmpNoCase(SG_T("utm")) )
	{
		double	Zone, Northing;

		if( !_Proj4_Read_Parameter(Value, Proj4, "zone") || !Value.asDouble(Zone) )
		{
			SG_UI_Msg_Add_Error(CSG_String::Format("Proj4 >> WKT: %s", _TL("invalid utm zone")));

			return( false );
		}

		Northing	= _Proj4_Read_Parameter(Value, Proj4, "south") ? 10000000 : 0;

		WKT		+= CSG_String::Format(",PARAMETER[\"%s\",%f]", SG_T("latitude_of_origin"), 0.0);
		WKT		+= CSG_String::Format(",PARAMETER[\"%s\",%f]", SG_T("central_meridian"  ), Zone * 6.0 - 183);
		WKT		+= CSG_String::Format(",PARAMETER[\"%s\",%f]", SG_T("scale_factor"      ), 0.9996);
		WKT		+= CSG_String::Format(",PARAMETER[\"%s\",%f]", SG_T("false_easting"     ), 500000.0);
		WKT		+= CSG_String::Format(",PARAMETER[\"%s\",%f]", SG_T("false_northing"    ), Northing);
		WKT		+= ",UNIT[\"metre\",1]]";

		return( true );
	}

	//-----------------------------------------------------
	// Parameters ...

	ProjCS	= Proj4;

	while( ProjCS.Find('+') >= 0 )
	{
		CSG_String	Key;

		ProjCS	= ProjCS.AfterFirst ('+');
		Value	= ProjCS.BeforeFirst('=');

		if( m_Proj4_to_WKT.Get_Translation(Value, Key) )
		{
			Value	= ProjCS.AfterFirst(SG_T('='));

			if( Value.Find(SG_T('+')) >= 0 )
			{
				Value	= Value.BeforeFirst('+');
			}

			WKT	+= ",PARAMETER[\"" + Key + "\"," + Value + "]";
		}
	}

	//-----------------------------------------------------
	// Unit ...

	_Proj4_Get_Unit(Value, Proj4);

	WKT	+= "," + Value + "]";

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projections::_Set_Dictionary(CSG_Table &Dictionary, int Direction)
{
	const int	n	= 209;
	const char	Translation[n][4][128]	= {
		//	 PROJ4		  DIR	WKT										   DESCRIPTION

		// Projection Types, *) = not verified
		{	"aea"		, " ", "Albers_Conic_Equal_Area"				, "Albers Equal Area"	},
		{	"aea"		, "<", "Albers"									, "[ESRI] Albers Equal Area"	},
		{	"aeqd"		, " ", "Azimuthal_Equidistant"					, "Azimuthal Equidistant"	},
		{	"airy"		, " ", "Airy"									, "*) Airy"	},
		{	"aitoff"	, " ", "Aitoff"									, "[ESRI] Aitoff"	},
		{	"alsk"		, " ", "Mod_Stererographics_of_Alaska"			, "*) Mod. Stererographics of Alaska"	},
		{	"apian"		, " ", "Apian_Globular_I"						, "*) Apian Globular I"	},
		{	"august"	, " ", "August_Epicycloidal"					, "*) August Epicycloidal"	},
		{	"bacon"		, " ", "Bacon_Globular"							, "*) Bacon Globular"	},
		{	"bipc"		, " ", "Bipolar_conic_of_western_hemisphere"	, "*) Bipolar conic of western hemisphere"	},
		{	"boggs"		, " ", "Boggs_Eumorphic"						, "*) Boggs Eumorphic"	},
		{	"bonne"		, " ", "Bonne"									, "Bonne (Werner lat_1=90)"	},
		{	"cass"		, " ", "Cassini_Soldner"						, "Cassini"	},
		{	"cass"		, "<", "Cassini"								, "[ESRI] Cassini"	},
		{	"cc"		, " ", "Central_Cylindrical"					, "*) Central Cylindrical"	},
		{	"cea"		, " ", "Cylindrical_Equal_Area"					, "Equal Area Cylindrical, alias: Lambert Cyl.Eq.A., Normal Authalic Cyl. (FME), Behrmann (SP=30), Gall Orthogr. (SP=45)"	},
		{	"cea"		, "<", "Behrmann"								, "[ESRI] Behrmann (standard parallel = 30)"	},
		{	"chamb"		, " ", "Chamberlin_Trimetric"					, "*) Chamberlin Trimetric"	},
		{	"collg"		, " ", "Collignon"								, "*) Collignon"	},
		{	"crast"		, " ", "Craster_Parabolic"						, "[ESRI] Craster Parabolic (Putnins P4)"	},
		{	"denoy"		, " ", "Denoyer_Semi_Elliptical"				, "*) Denoyer Semi-Elliptical"	},
		{	"eck1"		, " ", "Eckert_I"								, "*) Eckert I"	},
		{	"eck2"		, " ", "Eckert_II"								, "*) Eckert II"	},
		{	"eck3"		, " ", "Eckert_III"								, "*) Eckert III"	},
		{	"eck4"		, " ", "Eckert_IV"								, "Eckert IV"	},
		{	"eck5"		, " ", "Eckert_V"								, "*) Eckert V"	},
		{	"eck6"		, " ", "Eckert_VI"								, "Eckert VI"	},
		{	"eqc"		, " ", "Equirectangular"						, "Equidistant Cylindrical (Plate Caree)"	},
		{	"eqc"		, "<", "Equidistant_Cylindrical"				, "[ESRI] Equidistant Cylindrical (Plate Caree)"	},
		{	"eqc"		, "<", "Plate_Carree"							, "[ESRI] Equidistant Cylindrical (Plate Caree)"	},
		{	"eqdc"		, " ", "Equidistant_Conic"						, "*) Equidistant Conic"	},
		{	"euler"		, " ", "Euler"									, "*) Euler"	},
		{	"etmerc"	, " ", "Extended_Transverse_Mercator"			, "*) Extended Transverse Mercator"	},
		{	"fahey"		, " ", "Fahey"									, "*) Fahey"	},
		{	"fouc"		, " ", "Foucault"								, "*) Foucaut"	},
		{	"fouc_s"	, " ", "Foucault_Sinusoidal"					, "*) Foucaut Sinusoidal"	},
		{	"gall"		, " ", "Gall_Stereographic"						, "Gall (Gall Stereographic)"	},
		{	"geocent"	, " ", "Geocentric"								, "*) Geocentric"	},
		{	"geos"		, " ", "GEOS"									, "Geostationary Satellite View"	},
		{	"gins8"		, " ", "Ginsburg_VIII"							, "*) Ginsburg VIII (TsNIIGAiK)"	},
		{	"gn_sinu"	, " ", "General_Sinusoidal_Series"				, "*) General Sinusoidal Series"	},
		{	"gnom"		, " ", "Gnomonic"								, "Gnomonic"	},
		{	"goode"		, " ", "Goode_Homolosine"						, "*) Goode Homolosine"	},
		{	"gs48"		, " ", "Mod_Stererographics_48"					, "*) Mod. Stererographics of 48 U.S."	},
		{	"gs50"		, " ", "Mod_Stererographics_50"					, "*) Mod. Stererographics of 50 U.S."	},
		{	"hammer"	, " ", "Hammer_Eckert_Greifendorff"				, "*) Hammer & Eckert-Greifendorff"	},
		{	"hatano"	, " ", "Hatano_Asymmetrical_Equal_Area"			, "*) Hatano Asymmetrical Equal Area"	},
		{	"igh"		, " ", "World_Goode_Homolosine_Land"			, "*) Interrupted Goode Homolosine"	},
		{	"imw_p"		, " ", "International_Map_of_the_World_Polyconic", "*) International Map of the World Polyconic"	},
		{	"kav5"		, " ", "Kavraisky_V"							, "*) Kavraisky V"	},
		{	"kav7"		, " ", "Kavraisky_VII"							, "*) Kavraisky VII"	},
		{	"krovak"	, " ", "Krovak"									, "Krovak"	},
		{	"labrd"		, " ", "Laborde_Oblique_Mercator"				, "*) Laborde"	},
		{	"laea"		, " ", "Lambert_Azimuthal_Equal_Area"			, "Lambert Azimuthal Equal Area"	},
		{	"lagrng"	, " ", "Lagrange"								, "*) Lagrange"	},
		{	"larr"		, " ", "Larrivee"								, "*) Larrivee"	},
		{	"lask"		, " ", "Laskowski"								, "*) Laskowski"	},
		{	"lcc"		, "<", "Lambert_Conformal_Conic_1SP"			, "Lambert Conformal Conic (1 standard parallel)"	},
		{	"lcc"		, "<", "Lambert_Conformal_Conic_2SP"			, "Lambert Conformal Conic (2 standard parallels)"	},
		{	"lcc"		, " ", "Lambert_Conformal_Conic"				, "Lambert Conformal Conic"	},
		{	"lcca"		, " ", "Lambert_Conformal_Conic_Alternative"	, "*) Lambert Conformal Conic Alternative"	},
		{	"leac"		, " ", "Lambert_Equal_Area_Conic"				, "*) Lambert Equal Area Conic"	},
		{	"lee_os"	, " ", "Lee_Oblated_Stereographic"				, "*) Lee Oblated Stereographic"	},
		{	"loxim"		, " ", "Loximuthal"								, "[ESRI] Loximuthal"	},
		{	"lsat"		, " ", "Space_oblique_for_LANDSAT"				, "*) Space oblique for LANDSAT"	},
		{	"mbt_s"		, " ", "McBryde_Thomas_Flat_Polar_Sine"			, "*) McBryde-Thomas Flat-Polar Sine"	},
		{	"mbt_fps"	, " ", "McBryde_Thomas_Flat_Polar_Sine_2"		, "*) McBryde-Thomas Flat-Pole Sine (No. 2)"	},
		{	"mbtfpp"	, " ", "McBryde_Thomas_Flat_Polar_Parabolic"	, "*) McBride-Thomas Flat-Polar Parabolic"	},
		{	"mbtfpq"	, " ", "Flat_Polar_Quartic"						, "[ESRI] McBryde-Thomas Flat-Polar Quartic"	},
		{	"mbtfps"	, " ", "McBryde_Thomas_Flat_Polar_Sinusoidal"	, "*) McBryde-Thomas Flat-Polar Sinusoidal"	},
		{	"merc"		, " ", "Mercator"								, "[ESRI] Mercator"	},
		{	"merc"		, "<", "Mercator_1SP"							, "Mercator (1 standard parallel)"	},
		{	"merc"		, "<", "Mercator_2SP"							, "Mercator (2 standard parallels)"	},
		{	"mil_os"	, " ", "Miller_Oblated_Stereographic"			, "*) Miller Oblated Stereographic"	},
		{	"mill"		, " ", "Miller_Cylindrical"						, "Miller Cylindrical"	},
		{	"moll"		, " ", "Mollweide"								, "Mollweide"	},
		{	"murd1"		, " ", "Murdoch_I"								, "*) Murdoch I"	},
		{	"murd2"		, " ", "Murdoch_II"								, "*) Murdoch II"	},
		{	"murd3"		, " ", "Murdoch_III"							, "*) Murdoch III"	},
		{	"nell"		, " ", "Nell"									, "*) Nell"	},
		{	"nell_h"	, " ", "Nell_Hammer"							, "*) Nell-Hammer"	},
		{	"nicol"		, " ", "Nicolosi_Globular"						, "*) Nicolosi Globular"	},
		{	"nsper"		, " ", "Near_sided_perspective"					, "*) Near-sided perspective"	},
		{	"nzmg"		, " ", "New_Zealand_Map_Grid"					, "New Zealand Map Grid"	},
		{	"ob_tran"	, " ", "General_Oblique_Transformation"			, "*) General Oblique Transformation"	},
		{	"ocea"		, " ", "Oblique_Cylindrical_Equal_Area"			, "*) Oblique Cylindrical Equal Area"	},
		{	"oea"		, " ", "Oblated_Equal_Area"						, "*) Oblated Equal Area"	},
		{	"omerc"		, " ", "Hotine_Oblique_Mercator"				, "Oblique Mercator"	},
		{	"omerc"		, "<", "Oblique_Mercator"						, "Oblique Mercator"	},
		{	"ortel"		, " ", "Ortelius_Oval"							, "*) Ortelius Oval"	},
		{	"ortho"		, " ", "Orthographic"							, "Orthographic (ESRI: World from Space)"	},
		{	"pconic"	, " ", "Perspective_Conic"						, "*) Perspective Conic"	},
		{	"poly"		, " ", "Polyconic"								, "*) Polyconic (American)"	},
		{	"putp1"		, " ", "Putnins_P1"								, "*) Putnins P1"	},
		{	"putp2"		, " ", "Putnins_P2"								, "*) Putnins P2"	},
		{	"putp3"		, " ", "Putnins_P3"								, "*) Putnins P3"	},
		{	"putp3p"	, " ", "Putnins_P3'"							, "*) Putnins P3'"	},
		{	"putp4p"	, " ", "Putnins_P4'"							, "*) Putnins P4'"	},
		{	"putp5"		, " ", "Putnins_P5"								, "*) Putnins P5"	},
		{	"putp5p"	, " ", "Putnins_P5'"							, "*) Putnins P5'"	},
		{	"putp6"		, " ", "Putnins_P6"								, "*) Putnins P6"	},
		{	"putp6p"	, " ", "Putnins_P6'"							, "*) Putnins P6'"	},
		{	"qua_aut"	, " ", "Quartic_Authalic"						, "[ESRI] Quartic Authalic"	},
		{	"robin"		, " ", "Robinson"								, "Robinson"	},
		{	"rouss"		, " ", "Roussilhe_Stereographic"				, "*) Roussilhe Stereographic"	},
		{	"rpoly"		, " ", "Rectangular_Polyconic"					, "*) Rectangular Polyconic"	},
		{	"sinu"		, " ", "Sinusoidal"								, "Sinusoidal (Sanson-Flamsteed)"	},
		{	"somerc"	, " ", "Hotine_Oblique_Mercator"				, "Swiss Oblique Mercator"	},
		{	"somerc"	, "<", "Swiss_Oblique_Cylindrical"				, "Swiss Oblique Cylindrical"	},
		{	"somerc"	, "<", "Hotine_Oblique_Mercator_Azimuth_Center"	, "[ESRI] Swiss Oblique Mercator/Cylindrical"	},
		{	"stere"		, "<", "Polar_Stereographic"					, "Stereographic"	},
		{	"stere"		, " ", "Stereographic"							, "[ESRI] Stereographic"	},
		{	"sterea"	, " ", "Oblique_Stereographic"					, "Oblique Stereographic Alternative"	},
		{	"gstmerc"	, " ", "Gauss_Schreiber_Transverse_Mercator"	, "*) Gauss-Schreiber Transverse Mercator (aka Gauss-Laborde Reunion)"	},
		{	"tcc"		, " ", "Transverse_Central_Cylindrical"			, "*) Transverse Central Cylindrical"	},
		{	"tcea"		, " ", "Transverse_Cylindrical_Equal_Area"		, "*) Transverse Cylindrical Equal Area"	},
		{	"tissot"	, " ", "Tissot_Conic"							, "*) Tissot Conic"	},
		{	"tmerc"		, " ", "Transverse_Mercator"					, "*) Transverse Mercator"	},
		{	"tmerc"		, "<", "Gauss_Kruger"							, "[ESRI] DHDN"	},
		{	"tpeqd"		, " ", "Two_Point_Equidistant"					, "*) Two Point Equidistant"	},
		{	"tpers"		, " ", "Tilted_perspective"						, "*) Tilted perspective"	},
		{	"ups"		, " ", "Universal_Polar_Stereographic"			, "*) Universal Polar Stereographic"	},
		{	"urm5"		, " ", "Urmaev_V"								, "*) Urmaev V"	},
		{	"urmfps"	, " ", "Urmaev_Flat_Polar_Sinusoidal"			, "*) Urmaev Flat-Polar Sinusoidal"	},
		{	"utm"		, ">", "Transverse_Mercator"					, "*) Universal Transverse Mercator (UTM)"	},
		{	"vandg"		, "<", "Van_Der_Grinten_I"						, "[ESRI] van der Grinten (I)"	},
		{	"vandg"		, " ", "VanDerGrinten"							, "van der Grinten (I)"	},
		{	"vandg2"	, " ", "VanDerGrinten_II"						, "*) van der Grinten II"	},
		{	"vandg3"	, " ", "VanDerGrinten_III"						, "*) van der Grinten III"	},
		{	"vandg4"	, " ", "VanDerGrinten_IV"						, "*) van der Grinten IV"	},
		{	"vitk1"		, " ", "Vitkovsky_I"							, "*) Vitkovsky I"	},
		{	"wag1"		, " ", "Wagner_I"								, "*) Wagner I (Kavraisky VI)"	},
		{	"wag2"		, " ", "Wagner_II"								, "*) Wagner II"	},
		{	"wag3"		, " ", "Wagner_III"								, "*) Wagner III"	},
		{	"wag4"		, " ", "Wagner_IV"								, "*) Wagner IV"	},
		{	"wag5"		, " ", "Wagner_V"								, "*) Wagner V"	},
		{	"wag6"		, " ", "Wagner_VI"								, "*) Wagner VI"	},
		{	"wag7"		, " ", "Wagner_VII"								, "*) Wagner VII"	},
		{	"weren"		, " ", "Werenskiold_I"							, "*) Werenskiold I"	},
		{	"wink1"		, " ", "Winkel_I"								, "[ESRI] Winkel I"	},
		{	"wink2"		, " ", "Winkel_II"								, "[ESRI] Winkel II"	},
		{	"wintri"	, " ", "Winkel_Tripel"							, "[ESRI] Winkel Tripel"	},

		// Core Projection Types and Parameters that don't require explicit translation"	},
	//	{	"lonlat"	, " ", "GEOGCS"									, "Lat/long (Geodetic)"	},
	//	{	"latlon"	, ">", "GEOGCS"									, "Lat/long (Geodetic alias)"	},
	//	{	"latlong"	, ">", "GEOGCS"									, "Lat/long (Geodetic alias)"	},
	//	{	"longlat"	, ">", "GEOGCS"									, "Lat/long (Geodetic alias)"	},

	//	{	"a"			, " ", ""										, "Semimajor radius of the ellipsoid axis"	},
	//	{	"axis"		, " ", ""										, "Axis orientation (new in 4.8.0)"	},
	//	{	"b			, " ", ""										, "Semiminor radius of the ellipsoid axis"	},
	//	{	"datum		, " ", ""										, "Datum name (see `proj -ld`)"	},
	//	{	"ellps		, " ", ""										, "Ellipsoid name (see `proj -le`)"	},
	//	{	"nadgrids	, " ", ""										, "Filename of NTv2 grid file to use for datum transforms (see below)"	},
	//	{	"no_defs	, " ", ""										, "Don't use the /usr/share/proj/proj_def.dat defaults file"	},
	//	{	"pm			, " ", ""										, "Alternate prime meridian (typically a city name, see below)"	},
	//	{	"proj		, " ", ""										, "Projection name (see `proj -l`)"	},
	//	{	"to_meter	, " ", ""										, "Multiplier to convert map units to 1.0m"	},
	//	{	"towgs84	, " ", ""										, "3 or 7 term datum transform parameters (see below)"	},
	//	{	"units		, " ", ""										, "meters, US survey feet, etc."	},

	//	{	"south		, " ", ""										, "Denotes southern hemisphere UTM zone"	},
	//	{	"zone		, " ", ""										, "UTM zone"	},

	//	{	"lon_wrap"	, " ", ""										, "Center longitude to use for wrapping (see below)"	},
	//	{	"over"		, " ", ""										, "Allow longitude output outside -180 to 180 range, disables wrapping (see below)"	},

		// General Projection Parameters"	},
		{	"alpha"		, " ", "azimuth"								, "? Used with Oblique Mercator and possibly a few others"	},
		{	"k"			, ">", "scale_factor"							, "Scaling factor (old name)"	},
		{	"K"			, ">", "scale_factor"							, "? Scaling factor (old name)"	},
		{	"k_0"		, " ", "scale_factor"							, "Scaling factor (new name)"	},
		{	"lat_0"		, " ", "latitude_of_origin"						, "Latitude of origin"	},
		{	"lat_0"		, "<", "latitude_of_center"						, "Latitude of center"	},
		{	"lat_0"		, "<", "central_parallel"						, "[ESRI] Latitude of center"	},
		{	"lat_1"		, " ", "standard_parallel_1"					, "Latitude of first standard parallel"	},
		{	"lat_2"		, " ", "standard_parallel_2"					, "Latitude of second standard parallel"	},
		{	"lat_ts"	, ">", "latitude_of_origin"						, "Latitude of true scale"	},
		{	"lon_0"		, " ", "central_meridian"						, "Central meridian"	},
		{	"lon_0"		, "<", "longitude_of_center"					, "Longitude of center"	},
		{	"lonc"		, " ", "longitude_of_center"					, "? Longitude used with Oblique Mercator and possibly a few others"	},
		{	"x_0"		, " ", "false_easting"							, "False easting"	},
		{	"y_0"		, " ", "false_northing"							, "False northing"	},

		// Additional Projection Parameters"	},
		{	"azi"		, " ", ""										, ""	},
		{	"belgium"	, " ", ""										, ""	},
		{	"beta"		, " ", ""										, ""	},
		{	"czech"		, " ", ""										, ""	},
		{	"gamma"		, " ", ""										, ""	},
		{	"geoc"		, " ", ""										, ""	},
		{	"guam"		, " ", ""										, ""	},
		{	"h"			, " ", "satellite_height"						, "Satellite height (geos - Geostationary Satellite View)"	},
		{	"lat_b"		, " ", ""										, ""	},
		{	"lat_t"		, " ", ""										, ""	},
		{	"lon_1"		, " ", ""										, ""	},
		{	"lon_2"		, " ", ""										, ""	},
		{	"lsat"		, " ", ""										, ""	},
		{	"m"			, " ", ""										, ""	},
		{	"M"			, " ", ""										, ""	},
		{	"n"			, " ", ""										, ""	},
		{	"no_cut"	, " ", ""										, ""	},
		{	"no_off"	, " ", ""										, ""	},
		{	"no_rot"	, " ", ""										, ""	},
		{	"ns"		, " ", ""										, ""	},
		{	"o_alpha"	, " ", ""										, ""	},
		{	"o_lat_1"	, " ", ""										, ""	},
		{	"o_lat_2"	, " ", ""										, ""	},
		{	"o_lat_c"	, " ", ""										, ""	},
		{	"o_lat_p"	, " ", ""										, ""	},
		{	"o_lon_1"	, " ", ""										, ""	},
		{	"o_lon_2"	, " ", ""										, ""	},
		{	"o_lon_c"	, " ", ""										, ""	},
		{	"o_lon_p"	, " ", ""										, ""	},
		{	"o_proj"	, " ", ""										, ""	},
		{	"over"		, " ", ""										, ""	},
		{	"p"			, " ", ""										, ""	},
		{	"path"		, " ", ""										, ""	},
		{	"q"			, " ", ""										, ""	},
		{	"R"			, " ", ""										, ""	},
		{	"R_a"		, " ", ""										, ""	},
		{	"R_A"		, " ", ""										, ""	},
		{	"R_g"		, " ", ""										, ""	},
		{	"R_h"		, " ", ""										, ""	},
		{	"R_lat_a"	, " ", ""										, ""	},
		{	"R_lat_g"	, " ", ""										, ""	},
		{	"rot"		, " ", ""										, ""	},
		{	"R_V"		, " ", ""										, ""	},
		{	"s"			, " ", ""										, ""	},
		{	"sym"		, " ", ""										, ""	},
		{	"t"			, " ", ""										, ""	},
		{	"theta"		, " ", ""										, ""	},
		{	"tilt"		, " ", ""										, ""	},
		{	"vopt"		, " ", ""										, ""	},
		{	"W"			, " ", ""										, ""	},
		{	"westo"		, " ", ""										, ""	},

		{	""			, " ", ""										, ""	}	// dummy
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

		for(int i=0; i<n; i++)
		{
			CSG_Table_Record	*pRecord	= Dictionary.Add_Record();

			pRecord->Set_Value(0, Translation[i][0]);
			pRecord->Set_Value(1, Translation[i][1]);
			pRecord->Set_Value(2, Translation[i][2]);
			pRecord->Set_Value(3, Translation[i][3]);
		}
	}
	else if( Direction > 0 )	// Proj4 to WKT
	{
		Dictionary.Add_Field("PROJ4", SG_DATATYPE_String);
		Dictionary.Add_Field("WKT"  , SG_DATATYPE_String);

		for(int i=0; i<n; i++)
		{
			if( Translation[i][1][0] != '<' )	// only WKT to Proj4
			{
				CSG_Table_Record	*pRecord	= Dictionary.Add_Record();

				pRecord->Set_Value(0, Translation[i][0]);
				pRecord->Set_Value(1, Translation[i][2]);
			}
		}
	}
	else if( Direction < 0 )	// WKT to Proj4
	{
		Dictionary.Add_Field("WKT"  , SG_DATATYPE_String);
		Dictionary.Add_Field("PROJ4", SG_DATATYPE_String);

		for(int i=0; i<n; i++)
		{
			if( Translation[i][1][0] != '>' )	// only Proj4 to WKT
			{
				CSG_Table_Record	*pRecord	= Dictionary.Add_Record();

				pRecord->Set_Value(0, Translation[i][2]);
				pRecord->Set_Value(1, Translation[i][0]);
			}
		}
	}

	return( Dictionary.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Projections::_Set_Dictionary(CSG_Translator &Dictionary, int Direction)
{
	CSG_Table	Table;

	return( _Set_Dictionary(Table, Direction) && Dictionary.Create(&Table, 0, 1, true) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
