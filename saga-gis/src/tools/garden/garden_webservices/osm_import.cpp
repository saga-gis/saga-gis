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

//	https://api.openstreetmap.org/api/0.6/map?bbox=10.0,53.0,10.1,53.1

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
		"", "AREAS"		, _TL("OSM Areas"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::On_Execute(void)
{
	//-----------------------------------------------------
	if( !m_Connection.Create("https://api.openstreetmap.org") )
	{
		Message_Add(_TL("Unable to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	m_Nodes.Create(NULL);

	m_Nodes.Add_Field("ID" , SG_DATATYPE_DWord );
	m_Nodes.Add_Field("LON", SG_DATATYPE_Double);
	m_Nodes.Add_Field("LAT", SG_DATATYPE_Double);

	//-----------------------------------------------------
	m_pPoints	= Parameters("POINTS")->asShapes();
	m_pWays		= Parameters("WAYS"  )->asShapes();
	m_pAreas	= Parameters("AREAS" )->asShapes();

	m_pPoints->Create(SHAPE_TYPE_Point  , _TL("Locations"));
	m_pWays  ->Create(SHAPE_TYPE_Line   , _TL("Ways"     ));
	m_pAreas ->Create(SHAPE_TYPE_Polygon, _TL("Areas"    ));

	m_pPoints->Add_Field("ID", SG_DATATYPE_DWord);
	m_pWays  ->Add_Field("ID", SG_DATATYPE_DWord);
	m_pAreas ->Add_Field("ID", SG_DATATYPE_DWord);

	//-----------------------------------------------------
	m_bDown		= false;

	return( true );
}

//---------------------------------------------------------
bool COSM_Import::On_Execute_Finish(void)
{
	return( CSG_Tool_Interactive::On_Execute_Finish() );
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

			CSG_Rect	r(m_ptDown, ptWorld);

			CSG_String	Request(CSG_String::Format("/api/0.6/map?bbox=%f,%f,%f,%f",
				r.Get_XMin(), r.Get_YMin(),
				r.Get_XMax(), r.Get_YMax())
			);

			CSG_MetaData	Answer;

			if( m_Connection.Request(Request, Answer) == false )
			{
				Message_Add(_TL("received empty stream."));

				return( false );
			}

			if( !Load_Nodes(Answer) )
			{
				Message_Add("\n___\n" + Request, false);
				Message_Add("\n___\n" + Answer.asText(1), false);

				return( false );
			}

			#ifdef _DEBUG
				Message_Add("\n___\n" + Answer.asText(1), false);

				m_pPoints->Get_MetaData().Del_Children();
				m_pPoints->Get_MetaData().Add_Child(Answer);
			#endif

			m_pWays ->Del_Shapes();
			m_pAreas->Del_Shapes();

			Load_Ways(Answer);

			DataObject_Update(m_pPoints);
			DataObject_Update(m_pWays  );
			DataObject_Update(m_pAreas );
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
	m_Nodes.Del_Records();

	m_pPoints->Del_Shapes();

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
					CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

					pPoint->Add_Point(lon, lat);
					pPoint->Set_Value(0, id);
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Nodes.Set_Index(0, TABLE_INDEX_Ascending);

	return( m_Nodes.Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COSM_Import::Load_Ways(const CSG_MetaData &Root)
{
	//-----------------------------------------------------
	for(int i=0, WayID; i<Root.Get_Children_Count(); i++)
	{
		const CSG_MetaData	&Node	= Root[i];

		if( Node.Cmp_Name("way") &&	Node.Get_Property("id", WayID) )
		{
			CSG_Array_Int	NodeIDs;

			for(int iChild=0, NodeID; iChild<Node.Get_Children_Count(); iChild++)
			{
				const CSG_MetaData	&Child	= Node[iChild];

				if( Child.Cmp_Name("nd") && Child.Get_Property("ref", NodeID) )
				{
					NodeIDs	+= NodeID;
				}
			}

			if( NodeIDs.Get_Size() > 1 )
			{
				CSG_Shape	*pWay	= NodeIDs[0] == NodeIDs[NodeIDs.Get_Size() - 1]
					? m_pAreas->Add_Shape()
					: m_pWays ->Add_Shape();

				pWay->Set_Value(0, WayID);

				for(size_t iNode=0; iNode<NodeIDs.Get_Size(); iNode++)
				{
					CSG_Table_Record	*pNode	= m_Nodes.Find_Record(0, NodeIDs[iNode], true);

					if( pNode )
					{
						pWay->Add_Point(pNode->asDouble(1), pNode->asDouble(2));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	m_Nodes.Del_Records();

	return( true );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
