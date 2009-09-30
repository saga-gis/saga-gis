
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	PRJ_FIELD_SRID	= 0,
	PRJ_FIELD_AUTH_NAME,
	PRJ_FIELD_AUTH_SRID,
	PRJ_FIELD_SRTEXT,
	PRJ_FIELD_PROJ4TEXT,
	PRJ_FIELD_NAME,
	PRJ_FIELD_TYPE
};


///////////////////////////////////////////////////////////
//														 //
//														 //
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

	for(int iRecord=0; iRecord<pProjections->Get_Count() && SG_UI_Process_Set_Progress(iRecord, pProjections->Get_Count()); iRecord++)
	{
		CSG_Table_Record	*pRecord	= pProjections->Get_Record(iRecord);

		if( pRecord->asInt(SG_T("srid")) > 0 )
		{
			CSG_Table_Record	*pProjection	= m_pProjections->Add_Record();

			CSG_String	s(pRecord->asString(SG_T("srtext")));

			pProjection->Set_Value(PRJ_FIELD_SRID		, pRecord->asInt   (SG_T("srid")));
			pProjection->Set_Value(PRJ_FIELD_AUTH_NAME	, pRecord->asString(SG_T("auth_name")));
			pProjection->Set_Value(PRJ_FIELD_AUTH_SRID	, pRecord->asInt   (SG_T("auth_srid")));
			pProjection->Set_Value(PRJ_FIELD_SRTEXT		, pRecord->asString(s));
			pProjection->Set_Value(PRJ_FIELD_PROJ4TEXT	, pRecord->asString(SG_T("proj4text")));
			pProjection->Set_Value(PRJ_FIELD_NAME		, s.AfterFirst('\"').BeforeFirst('\"'));
			pProjection->Set_Value(PRJ_FIELD_TYPE		, s.BeforeFirst('['));
		}
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
void CSG_Projections::_On_Construction(void)
{
	m_pProjections	= new CSG_Table;

	m_pProjections->Set_Name(SG_T("PROJECTIONS"));

	m_pProjections->Add_Field(SG_T("SRID")		, SG_DATATYPE_Int);
	m_pProjections->Add_Field(SG_T("AUTH_NAME")	, SG_DATATYPE_String);
	m_pProjections->Add_Field(SG_T("AUTH_SRID")	, SG_DATATYPE_Int);
	m_pProjections->Add_Field(SG_T("SRTEXT")	, SG_DATATYPE_String);
	m_pProjections->Add_Field(SG_T("PROJ4TEXT")	, SG_DATATYPE_String);
	m_pProjections->Add_Field(SG_T("NAME")		, SG_DATATYPE_String);
	m_pProjections->Add_Field(SG_T("TYPE")		, SG_DATATYPE_String);
}

//---------------------------------------------------------
CSG_Projections::~CSG_Projections(void)
{
	delete(m_pProjections);
}

//---------------------------------------------------------
void CSG_Projections::Destroy(void)
{
	m_pProjections->Del_Records();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Projections::Get_Count(void)
{
	return( m_pProjections->Get_Count() );
}

//---------------------------------------------------------
CSG_String CSG_Projections::Get_Names_List(void)
{
	CSG_String	s;

	m_pProjections->Set_Index(PRJ_FIELD_TYPE, TABLE_INDEX_Ascending, PRJ_FIELD_NAME, TABLE_INDEX_Ascending);

	for(int i=0; i<m_pProjections->Get_Count(); i++)
	{
		CSG_Table_Record	*pProjection	= m_pProjections->Get_Record_byIndex(i);

		s	+= CSG_String::Format(SG_T("[%s] %s|"),
				pProjection->asString(PRJ_FIELD_TYPE),
				pProjection->asString(PRJ_FIELD_NAME)
			);
	}

	return( s );
}

//---------------------------------------------------------
int CSG_Projections::Get_SRID_byNameIndex(int Index)
{
	if( Index >= 0 && Index < Get_Count() )
	{
	//	m_pProjections->Set_Index(PRJ_FIELD_TYPE, TABLE_INDEX_Ascending, PRJ_FIELD_NAME, TABLE_INDEX_Ascending);

		return( m_pProjections->Get_Record_byIndex(Index)->asInt(PRJ_FIELD_SRID) );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
