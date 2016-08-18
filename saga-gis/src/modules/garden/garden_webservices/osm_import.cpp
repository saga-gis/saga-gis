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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
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

	Set_Author		(SG_T("O. Conrad (c) 2010"));

	Set_Description	(_TW(
		"This tool works as Web Map Service (WMS) client. "
		"More information on the WMS specifications can be obtained from the "
		"Open Geospatial Consortium (OGC) at "
		"<a href=\"http://www.opengeospatial.org/\">http://www.opengeospatial.org/</a>. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "POINTS"			, _TL("OSM Locations"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "WAYS"			, _TL("OSM Ways"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "RELATIONS"		, _TL("OSM Relations"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "AREAS"			, _TL("OSM Areas"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	//-----------------------------------------------------
	Parameters.Add_String(
		NULL	, "USERNAME"		, _TL("User Name"),
		_TL(""),
		SG_T("")
	);

	Parameters.Add_String(
		NULL	, "PASSWORD"		, _TL("Password"),
		_TL(""),
		SG_T(""), false, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints	= Parameters("POINTS")		->asShapes();
	m_pWays		= Parameters("WAYS")		->asShapes();
	m_pAreas	= Parameters("AREAS")		->asShapes();

	m_bDown		= false;

	//-----------------------------------------------------
	wxHTTP		Server;

	Server.SetUser		(Parameters("USERNAME")->asString());
	Server.SetPassword	(Parameters("PASSWORD")->asString());

	if( Server.Connect(SG_T("api.openstreetmap.org")) == false )
	{
		Message_Add(_TL("Unable to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	//-----------------------------------------------------
// <osm version="0.6" generator="OpenStreetMap server"></osm>
//	Request.Set_Name(SG_T("osm"));
//	Request.Add_Property(SG_T("version")	, SG_T("0.6"));
//	Request.Add_Property(SG_T("generator")	, SG_T("OpenStreetMap server"));
//	SG_T("api/capabilities");
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

			wxHTTP		Server;

		//	Server.SetUser		(Parameters("USERNAME")->asString());
		//	Server.SetPassword	(Parameters("PASSWORD")->asString());
			Server.SetUser		(SG_T(""));
			Server.SetPassword	(SG_T(""));

			if( Server.Connect(SG_T("api.openstreetmap.org")) == false )
			{
				Message_Add(_TL("Unable to connect to server."));

				return( false );
			}

			CSG_Rect		r(m_ptDown, ptWorld);
			wxInputStream	*pStream	= Server.GetInputStream(wxString::Format(SG_T("/api/0.6/map?bbox=%f,%f,%f,%f"),
				r.Get_XMin(), r.Get_YMin(), r.Get_XMax(), r.Get_YMax())
			);

			if( (pStream ) == NULL )
			{
				Message_Add(_TL("received empty stream."));

				return( false );
			}

			Process_Set_Text(_TL("loading OSM data"));

			wxXmlDocument	XML;

			if( !XML.Load(*pStream) )
			{
				return( false );
			}

			Process_Set_Text(_TL("ready"));

			if( !Load_Nodes(XML.GetRoot()) )
			{
				return( false );
			}

			Load_Ways(XML.GetRoot());

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::Load_Nodes(wxXmlNode *pRoot)
{
	long		id;
	double		lon, lat;
	wxString	sValue;

	//-----------------------------------------------------
	m_Nodes.Destroy();

	m_Nodes.Add_Field(SG_T("ID")	, SG_DATATYPE_DWord);
	m_Nodes.Add_Field(SG_T("LON")	, SG_DATATYPE_Double);
	m_Nodes.Add_Field(SG_T("LAT")	, SG_DATATYPE_Double);

	m_pPoints->Create(SHAPE_TYPE_Point, SG_T("OSM Locations"));
	m_pPoints->Add_Field(SG_T("ID"), SG_DATATYPE_DWord);

	//-----------------------------------------------------
	wxXmlNode	*pNode	= pRoot->GetChildren();

	while( pNode )
	{
		if( !pNode->GetName().CmpNoCase(SG_T("node")) )
		{
			if(	pNode->GetAttribute(SG_T("id" ), &sValue) && sValue.ToLong  (&id)
			&&	pNode->GetAttribute(SG_T("lon"), &sValue) && sValue.ToDouble(&lon)
			&&	pNode->GetAttribute(SG_T("lat"), &sValue) && sValue.ToDouble(&lat) )
			{
				wxXmlNode	*pTag	= pNode->GetChildren();

				if( !pTag || !pTag->GetName().CmpNoCase(SG_T("created_by")) )
				{
					CSG_Table_Record	*pRecord	= m_Nodes.Add_Record();

					pRecord->Set_Value(0, id);
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

		pNode	= pNode->GetNext();
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::Load_Ways(wxXmlNode *pRoot)
{
	long		id, idnode, Nodes[2000], nNodes;
	wxString	sValue;

	//-----------------------------------------------------
	m_pWays		->Create(SHAPE_TYPE_Line	, SG_T("OSM Ways"));
	m_pWays		->Add_Field(SG_T("ID"), SG_DATATYPE_DWord);

	m_pAreas	->Create(SHAPE_TYPE_Polygon	, SG_T("OSM Areas"));
	m_pAreas	->Add_Field(SG_T("ID"), SG_DATATYPE_DWord);

	//-----------------------------------------------------
	wxXmlNode	*pNode	= pRoot->GetChildren();

	while( pNode )
	{
		if( !pNode->GetName().CmpNoCase(SG_T("way")) )
		{
			if(	pNode->GetAttribute(SG_T("id" ), &sValue) && sValue.ToLong  (&id) )
			{
				wxXmlNode	*pChild	= pNode->GetChildren();

				nNodes	= 0;

				while( pChild )
				{
					if( !pChild->GetName().CmpNoCase(SG_T("nd")) && pChild->GetAttribute(SG_T("ref"), &sValue) && sValue.ToLong(&idnode) )
					{
						Nodes[nNodes++]	= idnode;
					}

					pChild	= pChild->GetNext();
				}

				if( nNodes > 1 )
				{
					CSG_Shape	*pShape	= Nodes[0] == Nodes[nNodes - 1] ? m_pAreas->Add_Shape() : m_pWays->Add_Shape();

					for(int i=0; i<nNodes; i++)
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

		pNode	= pNode->GetNext();
	}

	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
