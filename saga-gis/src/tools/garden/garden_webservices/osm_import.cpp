/**********************************************************
 * Version $Id: table.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   garden_webservices                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     osm_import.cpp                    //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/protocol/http.h>
#include <wx/xml/xml.h>

#include "osm_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COSM_Import::COSM_Import(void)
{
	Set_Name		(_TL("Import from Open Street Map"));

	Set_Author		("O. Conrad (c) 2010");

	Set_Description	(_TW(
		"This tool works as Web Map Service (WMS) client. "
		"More information on the WMS specifications can be obtained from the "
		"OpenStreetMap project. "
	));

	Add_Reference("http://openstreetmap.org/",
		SG_T("OpenStreetMap")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		"", "POINTS"	, _TL("OSM Locations"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		"", "WAYS"		, _TL("OSM Ways"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "RELATIONS"	, _TL("OSM Relations"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		"", "AREAS"		, _TL("OSM Areas"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	//-----------------------------------------------------
//	Parameters.Add_String("", "USERNAME", _TL("User Name"), _TL(""), "");
//	Parameters.Add_String("", "PASSWORD", _TL("Password" ), _TL(""), "", false, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints	= Parameters("POINTS")->asShapes();
	m_pWays		= Parameters("WAYS"  )->asShapes();
	m_pAreas	= Parameters("AREAS" )->asShapes();

	m_bDown		= false;

	//-----------------------------------------------------
	CSG_HTTP	Server;

	if( Server.Create("api.openstreetmap.org") == false )
	{
		Message_Add(_TL("Unable to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	//-----------------------------------------------------
// <osm version="0.6" generator="OpenStreetMap server"></osm>
//	Request.Set_Name("osm");
//	Request.Add_Property("version"	, "0.6");
//	Request.Add_Property("generator", "OpenStreetMap server");
//	"api/capabilities";
//	http://api.openstreetmap.org/api/0.6/map?bbox=10.0,53.0,10.1,53.1

	return( true );
}

//---------------------------------------------------------
bool COSM_Import::On_Execute_Finish(void)
{
	return( true );
}

//---------------------------------------------------------
bool COSM_Import::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LDOWN:
		if( m_bDown == false )
		{
			m_bDown		= true;
			m_ptDown	= ptWorld;
		}

		break;

	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LUP:
		if( m_bDown == true )
		{
			m_bDown		= false;

			CSG_HTTP	Server;

			if( Server.Create("api.openstreetmap.org") == false )
			{
				Message_Add(_TL("Unable to connect to server."));

				return( false );
			}

			CSG_Rect	r(m_ptDown, ptWorld);

			CSG_String	Request(CSG_String::Format("/api/0.6/map?bbox=%f,%f,%f,%f",
				r.Get_XMin(), r.Get_YMin(),
				r.Get_XMax(), r.Get_YMax())
			);

			CSG_MetaData	Answer;

			if( Server.Request(Request, Answer) == false )
			{
				Message_Add(_TL("received empty stream."));

				return( false );
			}

			if( !Load_Nodes(Request) )
			{
				return( false );
			}

			Load_Ways(Request);

			DataObject_Update(m_pPoints);
			DataObject_Update(m_pWays);
			DataObject_Update(m_pAreas);

			m_Nodes.Destroy();
		}

		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::Load_Nodes(const CSG_MetaData &Root)
{
	//-----------------------------------------------------
	m_Nodes.Destroy();

	m_Nodes.Add_Field("ID" , SG_DATATYPE_DWord );
	m_Nodes.Add_Field("LON", SG_DATATYPE_Double);
	m_Nodes.Add_Field("LAT", SG_DATATYPE_Double);

	m_pPoints->Create(SHAPE_TYPE_Point, _TL("Locations"));
	m_pPoints->Add_Field("ID", SG_DATATYPE_DWord);

	//-----------------------------------------------------
	for(int i=0; i<Root.Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Node	= Root[i];

		if( Node.Cmp_Name("node") )
		{
			int	id;	double	lon, lat;

			if(	Node.Get_Property("id" , id )
			&&	Node.Get_Property("lon", lon)
			&&	Node.Get_Property("lat", lat) )
			{
				if( !Node("created_by") )
				{
					CSG_Table_Record	*pRecord	= m_Nodes.Add_Record();

					pRecord->Set_Value(0, id );
					pRecord->Set_Value(1, lon);
					pRecord->Set_Value(2, lat);
				}
				else
				{
					CSG_Shape	*pShape	= m_pPoints->Add_Shape();

					pShape->Add_Point(lon, lat);
					pShape->Set_Value(0, id);
				}
			}
		}
	}

	m_Nodes.Set_Index(0, TABLE_INDEX_Ascending);

	//-----------------------------------------------------
	return( m_Nodes.Get_Count() > 0 );
}

//---------------------------------------------------------
bool COSM_Import::Find_Node(long id, double &lon, double &lat)
{
	CSG_Table_Record	*pRecord	= Find_Node(id);

	if( pRecord )
	{
		lon	= pRecord->asDouble(1);
		lat	= pRecord->asDouble(2);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Table_Record * COSM_Import::Find_Node(long id)
{
	int		a, b, d, i;

	if( m_Nodes.Get_Count() == 1 )
	{
		if( m_Nodes.Get_Record_byIndex(0)->asInt(0) == id )
		{
			return( m_Nodes.Get_Record_byIndex(0) );
		}
	}

	else if( m_Nodes.Get_Count() > 1 )
	{
		a	= 0;
		if( id < (d = m_Nodes.Get_Record_byIndex(a)->asInt(0)) )	return( NULL );		if( id == d )	return( m_Nodes.Get_Record_byIndex(a) );

		b	= m_Nodes.Get_Count() - 1;
		if( id > (d = m_Nodes.Get_Record_byIndex(b)->asInt(0)) )	return( NULL );		if( id == d )	return( m_Nodes.Get_Record_byIndex(b) );

		for( ; b - a > 1; )
		{
			i	= a + (b - a) / 2;
			d	= m_Nodes.Get_Record_byIndex(i)->asInt(0);

			if( id > d )
			{
				a	= i;
			}
			else if( id < d )
			{
				b	= i;
			}
			else
			{
				return( m_Nodes.Get_Record_byIndex(i) );
			}
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::Load_Ways(const CSG_MetaData &Root)
{
	//-----------------------------------------------------
	m_pWays ->Create(SHAPE_TYPE_Line   , _TL("Ways" ));
	m_pWays ->Add_Field("ID", SG_DATATYPE_DWord);

	m_pAreas->Create(SHAPE_TYPE_Polygon, _TL("Areas"));
	m_pAreas->Add_Field("ID", SG_DATATYPE_DWord);

	//-----------------------------------------------------
	for(int i=0; i<Root.Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Node	= Root[i];

		if( Node.Cmp_Name("way") )
		{
			int	id;

			if(	Node.Get_Property("id" , id) )
			{
				int	j;	CSG_Array_Int	Nodes;

				for(j=0; j<Node.Get_Children_Count(); j++)
				{
					const CSG_MetaData	&Child	= Node[j];

					if( Child.Cmp_Name("nd") && Child.Get_Property("ref", id) )
					{
						Nodes	+= id;
					}
				}

				if( Nodes.Get_Size() > 1 )
				{
					CSG_Shape	*pShape	= Nodes[0] == Nodes[Nodes.Get_Size() - 1] ? m_pAreas->Add_Shape() : m_pWays->Add_Shape();

					for(j=0; j<Nodes.Get_Size(); j++)
					{
						double	lon, lat;

						if( Find_Node(Nodes[i], lon, lat) )
						{
							pShape->Add_Point(lon, lat);
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
