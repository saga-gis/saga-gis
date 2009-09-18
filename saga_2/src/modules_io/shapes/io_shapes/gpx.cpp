
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        gpx.cpp                        //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "gpx.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGPX_Import::CGPX_Import(void)
{
	Set_Name		(_TL("Import GPX"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Imports GPS data from GPS eXchange format GPX.\n"
		"\n"
		"References:\n"
		"<a href=\"http://www.topografix.com/\">The GPS Exchange Format</a>"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes_List(
		NULL	, "SHAPES"			, _TL("GPX Import"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("GPS Exchange Format (*.gpx)")	, SG_T("*.gpx"),
			_TL("All Files")					, SG_T("*.*")
		), NULL, false
 	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGPX_Import::On_Execute(void)
{
	CSG_Shapes		*pWay;
	CSG_MetaData	GPX;

	//-----------------------------------------------------
	m_Name		= Parameters("FILE")	->asString();
	m_pShapes	= Parameters("SHAPES")	->asShapesList();

	//-----------------------------------------------------
	if( !GPX.Create(m_Name) || GPX.Get_Name().CmpNoCase(SG_T("gpx")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	pWay		= SG_Create_Shapes(SHAPE_TYPE_Point, m_Name);

	m_Name		= SG_File_Get_Name(m_Name, false);

	m_pShapes->Del_Items();

	//-----------------------------------------------------
	for(int i=0; i<GPX.Get_Children_Count(); i++)
	{
		CSG_MetaData	*pChild	= GPX.Get_Child(i);

		     if( pChild->Get_Name().CmpNoCase(SG_T("wpt")) == 0 )
		{
			Add_Point(pChild, pWay);
		}
		else if( pChild->Get_Name().CmpNoCase(SG_T("rte")) == 0 )
		{
			Add_Route(pChild);
		}
		else if( pChild->Get_Name().CmpNoCase(SG_T("trk")) == 0 )
		{
			Add_Track(pChild);
		}
	}

	//-----------------------------------------------------
	if( pWay->Get_Count() > 0 )
	{
		m_pShapes->Add_Item(pWay);
	}
	else
	{
		delete(pWay);
	}

	return( m_pShapes->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_CONTENT(node, child, default)	(node->Get_Child(SG_T(child)) ? node->Get_Child(SG_T(child))->Get_Content() : SG_T(default))

//---------------------------------------------------------
bool CGPX_Import::Add_Route(CSG_MetaData *pRoute)
{
	// <name>	xsd:string
	// <cmt>	xsd:string
	// <desc>	xsd:string
	// <src>	xsd:string
	// <link>	linkType
	// <number> xsd:nonNegativeInteger
	// <type>	xsd:string

	CSG_String	Name(GET_CONTENT(pRoute, "name", "Route"));

	CSG_Shapes	*pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), m_Name.c_str(), Name.c_str()));

	m_pShapes->Add_Item(pPoints);

	for(int i=0; i<pRoute->Get_Children_Count(); i++)
	{
		CSG_MetaData	*pChild	= pRoute->Get_Child(i);

		if( pChild->Get_Name().CmpNoCase(SG_T("rtept")) == 0 )
		{
			Add_Point(pChild, pPoints);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGPX_Import::Add_Track(CSG_MetaData *pTrack)
{
	// <name>	xsd:string
	// <cmt>	xsd:string
	// <desc>	xsd:string
	// <src>	xsd:string
	// <link>	linkType
	// <number> xsd:nonNegativeInteger
	// <type>	xsd:string

	CSG_MetaData	*pSegment	= pTrack->Get_Child(SG_T("trkseg"));

	if( pSegment )
	{
		CSG_String	Name(GET_CONTENT(pTrack, "name", "Track Segment"));

		CSG_Shapes	*pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), m_Name.c_str(), Name.c_str()));

		m_pShapes->Add_Item(pPoints);

		for(int i=0; i<pSegment->Get_Children_Count(); i++)
		{
			CSG_MetaData	*pChild	= pSegment->Get_Child(i);

			if( pChild->Get_Name().CmpNoCase(SG_T("trkpt")) == 0 )
			{
				Add_Point(pChild, pPoints);
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_FIELD(key, type)	if( pNode->Get_Child(SG_T(key)) )	{	pPoints->Add_Field(SG_T(key), type);	}

bool CGPX_Import::Add_Fields(CSG_MetaData *pNode, CSG_Shapes *pPoints)
{
	if( pPoints->Get_Field_Count() == 0 )
	{
		ADD_FIELD("ele"				, SG_DATATYPE_Double);	// <ele>			xsd:decimal				(Höhe)
		ADD_FIELD("time"			, SG_DATATYPE_String);	// <time>			xsd:dateTime			(Datum und Zeit)
		ADD_FIELD("magvar"			, SG_DATATYPE_Double);	// <magvar>			degreesType				(lokale magn. Missweisung)
		ADD_FIELD("geoidheight"		, SG_DATATYPE_Double);	// <geoidheight>	xsd:decimal				(Höhe bezogen auf Geoid)
		ADD_FIELD("name"			, SG_DATATYPE_String);	// <name>			xsd:string				(Beschreibung)
		ADD_FIELD("cmt"				, SG_DATATYPE_String);	// <cmt>			xsd:string				(Kommentar)
		ADD_FIELD("desc"			, SG_DATATYPE_String);	// <desc>			xsd:string				(Elementbeschreibung)
		ADD_FIELD("src"				, SG_DATATYPE_String);	// <src>			xsd:string				(Quelle)
		ADD_FIELD("link"			, SG_DATATYPE_String);	// <link>			linkType				(Link)
		ADD_FIELD("sym"				, SG_DATATYPE_String);	// <sym>			xsd:string				(Darstellungssymbol)
		ADD_FIELD("type"			, SG_DATATYPE_String);	// <type>			xsd:string				(Klassifikation)
		ADD_FIELD("fix"				, SG_DATATYPE_Double);	// <fix>			fixType
		ADD_FIELD("sat"				, SG_DATATYPE_Int   );	// <sat>			xsd:nonNegativeInteger	(Anzahl der empfangenen Satelliten)
		ADD_FIELD("hdop"			, SG_DATATYPE_Double);	// <hdop>			xsd:decimal				(hDOP)
		ADD_FIELD("vdop"			, SG_DATATYPE_Double);	// <vdop>			xsd:decimal				(vDOP)
		ADD_FIELD("pdop"			, SG_DATATYPE_Double);	// <pdop>			xsd:decimal				(3D DOP)
		ADD_FIELD("ageofdgpsdata"	, SG_DATATYPE_Double);	// <ageofdgpsdata>	xsd:decimal				(Letzter DGPS update)
		ADD_FIELD("dgpsid"			, SG_DATATYPE_Int   );	// <dgpsid>			dgpsStationType			(DGPS ID)
	}

	return( pPoints->Get_Field_Count() > 0 );
}

//---------------------------------------------------------
bool CGPX_Import::Add_Point(CSG_MetaData *pNode, CSG_Shapes *pPoints)
{
	const SG_Char	*cString;
	TSG_Point		Point;

	if(	(cString = pNode->Get_Property(SG_T("lon"))) != NULL && CSG_String(cString).asDouble(Point.x)
	&&	(cString = pNode->Get_Property(SG_T("lat"))) != NULL && CSG_String(cString).asDouble(Point.y)
	&&	Add_Fields(pNode, pPoints) )
	{
		CSG_Shape	*pPoint	= pPoints->Add_Shape();

		pPoint->Add_Point(Point, 0);

		for(int i=0; i<pNode->Get_Children_Count(); i++)
		{
			CSG_MetaData	*pChild	= pNode->Get_Child(i);

			pPoint->Set_Value(pChild->Get_Name(), pChild->Get_Content());
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
CGPX_Export::CGPX_Export(void)
{
	Set_Name		(_TL("Export GPX"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Exports vector data points to GPS eXchange format GPX.\n"
		"\n"
		"References:\n"
		"<a href=\"http://www.topografix.com/\">The GPS Exchange Format</a>"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"			, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"			, _TL("File"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("GPS Exchange Format (*.gpx)")	, SG_T("*.gpx"),
			_TL("All Files")					, SG_T("*.*")
		), NULL, true
 	);

	Parameters.Add_Table_Field(
		pNode	, "ELE"				, _TL("Elevation"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "NAME"			, _TL("Name"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "CMT"				, _TL("Comment"),
		_TL(""),
		true
	);

	Parameters.Add_Table_Field(
		pNode	, "DESC"			, _TL("Description"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGPX_Export::On_Execute(void)
{
	int				iEle, iName, iCmt, iDesc;
	CSG_String		File;
	CSG_MetaData	GPX;
	CSG_Shapes		*pShapes;

	//-----------------------------------------------------
	File		= Parameters("FILE")	->asString();
	pShapes		= Parameters("SHAPES")	->asShapes();

	iEle		= Parameters("ELE")		->asInt();	if( iEle  >= pShapes->Get_Field_Count() )	iEle	= -1;
	iName		= Parameters("NAME")	->asInt();	if( iName >= pShapes->Get_Field_Count() )	iName	= -1;
	iCmt		= Parameters("CMT")		->asInt();	if( iCmt  >= pShapes->Get_Field_Count() )	iCmt	= -1;
	iDesc		= Parameters("DESC")	->asInt();	if( iDesc >= pShapes->Get_Field_Count() )	iDesc	= -1;

	//-----------------------------------------------------
	GPX.Set_Name(SG_T("gpx"));
	GPX.Add_Property(SG_T("version")			, SG_T("1.0"));
	GPX.Add_Property(SG_T("creator")			, SG_T("SAGA - System for Automated Geoscientific Analyses - http://www.saga-gis.org"));
	GPX.Add_Property(SG_T("xmlns:xsi")			, SG_T("http://www.w3.org/2001/XMLSchema-instance"));
	GPX.Add_Property(SG_T("xmlns")				, SG_T("http://www.topografix.com/GPX/1/0"));
	GPX.Add_Property(SG_T("xsi:schemaLocation")	, SG_T("http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd"));

	for(int iShape=0; iShape<pShapes->Get_Count(); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				CSG_MetaData	*pPoint	= GPX.Add_Child(SG_T("wpt"), "");

				pPoint->Add_Property(SG_T("lon"), pShape->Get_Point(iPoint, iPart).x);
				pPoint->Add_Property(SG_T("lat"), pShape->Get_Point(iPoint, iPart).y);

				if( iEle  > 0 )	pPoint->Add_Child(SG_T("ele" ), pShape->asString(iEle));
				if( iName > 0 )	pPoint->Add_Child(SG_T("name"), pShape->asString(iName));
				if( iCmt  > 0 )	pPoint->Add_Child(SG_T("cmt" ), pShape->asString(iCmt));
				if( iDesc > 0 )	pPoint->Add_Child(SG_T("desc"), pShape->asString(iDesc));
			}
		}
	}

	//-----------------------------------------------------
	if( !GPX.Save(File) )
	{
		return( false );
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
