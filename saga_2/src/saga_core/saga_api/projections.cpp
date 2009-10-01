
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

//---------------------------------------------------------
CSG_Projection::CSG_Projection(int SRID, const SG_Char *Authority, const SG_Char *OpenGIS, const SG_Char *Proj4)
{
	_Reset();

	Create(SRID, Authority, OpenGIS, Proj4);
}

bool CSG_Projection::Create(int SRID, const SG_Char *Authority, const SG_Char *OpenGIS, const SG_Char *Proj4)
{
	return( Assign(SRID, Authority, OpenGIS, Proj4) );
}

//---------------------------------------------------------
void CSG_Projection::_Reset(void)
{
	m_SRID		= -1;
	m_Type		= SG_PROJ_TYPE_CS_Undefined;
	m_Name		= LNG("undefined");
	m_OpenGIS	.Clear();
	m_Proj4		.Clear();
	m_Authority	.Clear();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::Assign(int SRID, const SG_Char *Authority, const SG_Char *OpenGIS, const SG_Char *Proj4)
{
	_Reset();

	if( !OpenGIS )
	{
		if( !_Get_OpenGIS_from_Proj4(Proj4) )
		{
			return( false );
		}
	}
	else
	{
		m_OpenGIS	= OpenGIS;
	}

	CSG_String	s(m_OpenGIS.BeforeFirst('['));

	if( !s.Cmp(SG_T("PROJCS")) )
	{
		m_Type	= SG_PROJ_TYPE_CS_Projected;
	}
	else if( !s.Cmp(SG_T("GEOGCS")) )
	{
		m_Type	= SG_PROJ_TYPE_CS_Geographic;
	}
	else if( !s.Cmp(SG_T("GEOCCS")) )
	{
		m_Type	= SG_PROJ_TYPE_CS_Geocentric;
	}
	else
	{
		return( false );
	}

	if( !Proj4 )
	{
		_Get_Proj4_from_OpenGIS(OpenGIS);
	}
	else
	{
		m_Proj4		= Proj4;
	}

	m_SRID		= SRID;
	m_Authority	= Authority ? Authority : SG_T("");
	m_Name		= m_OpenGIS.AfterFirst('\"').BeforeFirst('\"');

	return( true );
}

//---------------------------------------------------------
bool CSG_Projection::Assign(const CSG_Projection &Projection)
{
	m_SRID		= Projection.m_SRID;
	m_Type		= Projection.m_Type;
	m_Name		= Projection.m_Name;
	m_OpenGIS	= Projection.m_OpenGIS;
	m_Proj4		= Projection.m_Proj4;
	m_Authority	= Projection.m_Authority;

	return( true );
}

//---------------------------------------------------------
bool CSG_Projection::is_Equal(const CSG_Projection &Projection)	const
{
	return(	m_SRID == Projection.m_SRID
		&&	m_Type == Projection.m_Type
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Projection::_Get_OpenGIS_from_Proj4(const SG_Char *Text)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::_Get_Proj4_from_OpenGIS(const SG_Char *Text)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::from_ESRI(const CSG_String &ESRI_PRJ)
{
	return( false );
}

//---------------------------------------------------------
bool CSG_Projection::to_ESRI(CSG_String &ESRI_PRJ)	const
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Projection::asString(void) const
{
	return( m_Name );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PRJ_FIELD_SRID			0
#define PRJ_FIELD_AUTH_NAME		1
#define PRJ_FIELD_AUTH_SRID		2
#define PRJ_FIELD_SRTEXT		3
#define PRJ_FIELD_PROJ4TEXT		4

//---------------------------------------------------------
CSG_Projections *CSG_Projections::s_pProjections	= NULL;


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
		CSG_Projection		Projection;
		CSG_Table_Record	*pRecord	= pProjections->Get_Record(i);

		if( Projection.Assign(
			pRecord->asInt   (PRJ_FIELD_SRID),
			pRecord->asString(PRJ_FIELD_AUTH_NAME),
			pRecord->asString(PRJ_FIELD_SRTEXT),
			pRecord->asString(PRJ_FIELD_PROJ4TEXT)) )
		{
			Add(Projection);
		}
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
void CSG_Projections::_On_Construction(void)
{
	m_pProjections	= NULL;
	m_nProjections	= 0;
	m_nBuffer		= 0;

	m_pIdx_Names	= new CSG_Index;
	m_pIdx_SRIDs	= new CSG_Index;
}

//---------------------------------------------------------
CSG_Projections::~CSG_Projections(void)
{
	Destroy();

	delete(m_pIdx_Names);
	delete(m_pIdx_SRIDs);
}

//---------------------------------------------------------
void CSG_Projections::Destroy(void)
{
	if( m_pProjections )
	{
		for(int i=0; i<m_nProjections; i++)
		{
			delete(m_pProjections[i]);
		}

		SG_Free(m_pProjections);

		m_pProjections	= NULL;
		m_nProjections	= 0;
		m_nBuffer		= 0;
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

	Table.Add_Field(SG_T("srid")		, SG_DATATYPE_Int);
	Table.Add_Field(SG_T("auth_name")	, SG_DATATYPE_String);
	Table.Add_Field(SG_T("auth_srid")	, SG_DATATYPE_Int);
	Table.Add_Field(SG_T("srtext")		, SG_DATATYPE_String);
	Table.Add_Field(SG_T("proj4text")	, SG_DATATYPE_String);

	for(int i=0; i<m_nProjections && SG_UI_Process_Set_Progress(i, m_nProjections); i++)
	{
		CSG_Projection		*pProjection	= m_pProjections[i];
		CSG_Table_Record	*pRecord		= Table.Add_Record();

		pRecord->Set_Value(PRJ_FIELD_SRID		, pProjection->Get_SRID());
		pRecord->Set_Value(PRJ_FIELD_AUTH_NAME	, pProjection->Get_Authority());
		pRecord->Set_Value(PRJ_FIELD_AUTH_SRID	, pProjection->Get_SRID());
		pRecord->Set_Value(PRJ_FIELD_SRTEXT		, pProjection->Get_OpenGIS());
		pRecord->Set_Value(PRJ_FIELD_PROJ4TEXT	, pProjection->Get_Proj4());
	}

	return( Table.Save(File_Name) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_GROW_SIZE(n)	(n < 64 ? 1 : (n < 1024 ? 64 : 1024))

//---------------------------------------------------------
CSG_Projection * CSG_Projections::_Add(void)
{
	if( (m_nProjections + 1) >= m_nBuffer )
	{
		CSG_Projection	**pProjections	= (CSG_Projection **)SG_Realloc(m_pProjections, (m_nBuffer + GET_GROW_SIZE(m_nBuffer)) * sizeof(char *));

		if( !pProjections )
		{
			return( NULL );
		}

		m_pProjections	 = pProjections;
		m_nBuffer		+= GET_GROW_SIZE(m_nBuffer);
	}

	return( m_pProjections[m_nProjections++] = new CSG_Projection );
}

//---------------------------------------------------------
bool CSG_Projections::Add(const CSG_Projection &Projection)
{
	CSG_Projection	*pProjection	= _Add();

	if( pProjection )
	{
		return( pProjection->Assign(Projection) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Projections::Add(int SRID, const SG_Char *Authority, const SG_Char *OpenGIS, const SG_Char *Proj4)
{
	CSG_Projection	Projection;

	return( Projection.Create(SRID, Authority, OpenGIS, Proj4) && Add(Projection) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Projections::_Cmp_Names(const int iElement_1, const int iElement_2)
{
	return(	(*s_pProjections)[iElement_1].Get_Type() ==  (*s_pProjections)[iElement_2].Get_Type()
		?	(*s_pProjections)[iElement_1].Get_Name().Cmp((*s_pProjections)[iElement_2].Get_Name())
		:	(*s_pProjections)[iElement_1].Get_Type()  -  (*s_pProjections)[iElement_2].Get_Type()	);
}

//---------------------------------------------------------
int CSG_Projections::_Cmp_SRIDs(const int iElement_1, const int iElement_2)
{
	return( (*s_pProjections)[iElement_1].Get_SRID()  -  (*s_pProjections)[iElement_2].Get_SRID() );
}

//---------------------------------------------------------
CSG_String CSG_Projections::Get_Names(void) const
{
	s_pProjections	= (CSG_Projections *)this;

	if( m_pIdx_Names->Get_Count() != m_nProjections )
	{
		m_pIdx_Names->Create(m_nProjections, (TSG_PFNC_Compare)CSG_Projections::_Cmp_Names, true);
	}

	if( m_pIdx_SRIDs->Get_Count() != m_nProjections )
	{
		m_pIdx_SRIDs->Create(m_nProjections, _Cmp_SRIDs, true);
	}

	//-----------------------------------------------------
	CSG_String	s;

	for(int i=0; i<m_nProjections; i++)
	{
		CSG_Projection	*pProjection	= m_pProjections[(*m_pIdx_Names)[i]];

		s	+= CSG_String::Format(SG_T("[%s] %s|"),
				pProjection->Get_Type_Name().c_str(),
				pProjection->Get_Name().c_str()
			);
	}

	return( s );
}

//---------------------------------------------------------
int CSG_Projections::Get_SRID_byNamesIndex(int i) const
{
	if( i >= 0 && i < Get_Count() )
	{
		return( Get_Projection((*m_pIdx_Names)[i]).Get_SRID() );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
