/**********************************************************
 * Version $Id: table.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   garden_webservices                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     wms_import.cpp                    //
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
#include <wx/protocol/http.h>
#include <wx/xml/xml.h>
#include <wx/image.h>

#include "wms_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define V_SRS(Version)	(Version.Contains(SG_T("1.3")) ? SG_T("CRS") : SG_T("SRS"))
#define S_SRS(Version)	(Version.Cmp(SG_T("1.3.0")) ? SG_T("&SRS=") : SG_T("&CRS="))

#define V_MAP(Version)	(Version.Cmp(SG_T("1.0.0")) ? SG_T("GetMap") : SG_T("Map"))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWMS_Capabilities::CWMS_Capabilities(void)
{
	m_pLayers	= NULL;

	_Reset();
}

CWMS_Capabilities::CWMS_Capabilities(wxHTTP *pServer, const CSG_String &Directory, CSG_String &Version)
{
	m_pLayers	= NULL;

	_Reset();

	Create(pServer, Directory, Version);
}

//---------------------------------------------------------
CWMS_Capabilities::~CWMS_Capabilities(void)
{}

//---------------------------------------------------------
void CWMS_Capabilities::_Reset(void)
{
	m_MaxLayers		= -1;
	m_MaxWidth		= -1;
	m_MaxHeight		= -1;

	m_Name			.Clear();
	m_Title			.Clear();
	m_Abstract		.Clear();
	m_Online		.Clear();
	m_Contact		.Clear();
	m_Fees			.Clear();
	m_Access		.Clear();
	m_Keywords		.Clear();

	m_Formats		.Clear();
	m_Projections	.Clear();

	m_Layers_Title	.Clear();

	if( m_pLayers )
	{
		for(int i=0; i<m_nLayers; i++)
		{
			delete(m_pLayers[i]);
		}

		SG_Free(m_pLayers);
	}

	m_nLayers		= 0;
	m_pLayers		= NULL;
}

//---------------------------------------------------------
bool CWMS_Capabilities::Create(wxHTTP *pServer, const CSG_String &Directory, CSG_String &Version)
{
	bool	bResult	= false;

	_Reset();

	if( pServer )
	{
		CSG_String	sRequest(Directory);

		sRequest	+= SG_T("?SERVICE=WMS");
		sRequest	+= SG_T("&VERSION=1.3.0");
		sRequest	+= SG_T("&REQUEST=GetCapabilities");

		//-------------------------------------------------
		wxInputStream	*pStream;

		if( (pStream = pServer->GetInputStream(sRequest.c_str())) != NULL )
		{
			wxXmlDocument	Capabilities;

			if( Capabilities.Load(*pStream) )
			{
				bResult	= _Get_Capabilities(Capabilities.GetRoot(), Version);

				Capabilities.Save(CSG_String::Format("e:\\%s.xml", m_Title.c_str()).c_str());
			}

			delete(pStream);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxXmlNode * CWMS_Capabilities::_Get_Child(wxXmlNode *pNode, const CSG_String &Name)
{
	if( pNode && (pNode = pNode->GetChildren()) != NULL )
	{
		do
		{
			if( !pNode->GetName().CmpNoCase(Name.c_str()) )
			{
				return( pNode );
			}
		}
		while( (pNode = pNode->GetNext()) != NULL );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CWMS_Capabilities::_Get_Child_Content(wxXmlNode *pNode, CSG_String &Value, const CSG_String &Name)
{
	if( (pNode = _Get_Child(pNode, Name)) != NULL )
	{
		Value	= pNode->GetNodeContent();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWMS_Capabilities::_Get_Child_Content(wxXmlNode *pNode, int &Value, const CSG_String &Name)
{
	long	lValue;

	if( (pNode = _Get_Child(pNode, Name)) != NULL && pNode->GetNodeContent().ToLong(&lValue) )
	{
		Value	= lValue;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWMS_Capabilities::_Get_Child_Content(wxXmlNode *pNode, double &Value, const CSG_String &Name)
{
	double	dValue;

	if( (pNode = _Get_Child(pNode, Name)) != NULL && pNode->GetNodeContent().ToDouble(&dValue) )
	{
		Value	= dValue;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWMS_Capabilities::_Get_Node_PropVal(wxXmlNode *pNode, CSG_String &Value, const CSG_String &Property)
{
	wxString	PropVal;

	if( pNode != NULL && pNode->GetPropVal(Property.c_str(), &PropVal) )
	{
		Value	= PropVal.c_str();

		return( true );
	}

	return( false );
}

bool CWMS_Capabilities::_Get_Child_PropVal(wxXmlNode *pNode, CSG_String &Value, const CSG_String &Name, const CSG_String &Property)
{
	return( (pNode = _Get_Child(pNode, Name)) != NULL && _Get_Node_PropVal(pNode, Value, Property) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Capabilities::_Get_Capabilities(wxXmlNode *pRoot, CSG_String &Version)
{
	wxXmlNode	*pNode, *pChild;

	//-----------------------------------------------------
	// 1. Service

	if( (pNode = _Get_Child(pRoot, SG_T("Service"))) == NULL )
	{
		return( false );
	}

	_Get_Node_PropVal (pRoot, Version		, SG_T("version"));

	_Get_Child_Content(pNode, m_Name		, SG_T("Name"));
	_Get_Child_Content(pNode, m_Title		, SG_T("Title"));
	_Get_Child_Content(pNode, m_Abstract	, SG_T("Abstract"));
	_Get_Child_Content(pNode, m_Fees		, SG_T("Fees"));
	_Get_Child_Content(pNode, m_Access		, SG_T("AccessConstraints"));
	_Get_Child_Content(pNode, m_MaxLayers	, SG_T("LayerLimit"));
	_Get_Child_Content(pNode, m_MaxWidth	, SG_T("MaxWidth"));
	_Get_Child_Content(pNode, m_MaxHeight	, SG_T("MaxHeight"));
	_Get_Child_PropVal(pNode, m_Online		, SG_T("OnlineResource"), SG_T("xlink:href"));

	if( (pChild = _Get_Child(pNode, SG_T("KeywordList"))) != NULL )
	{
		wxXmlNode	*pKeyword	= pChild->GetChildren();

		while( pKeyword )
		{
			if( !pKeyword->GetName().CmpNoCase(SG_T("Format")) )
			{
				m_Keywords.Add(pKeyword->GetNodeContent().c_str());
			}

			pKeyword	= pKeyword->GetNext();
		}
	}

	if( (pChild = _Get_Child(pNode, SG_T("ContactInformation"))) != NULL )
	{
	}


	//-----------------------------------------------------
	// 2. Capabilities

	if( (pNode = _Get_Child(pRoot, SG_T("Capability"))) == NULL )
	{
		return( false );
	}

	//-----------------------------------------------------
	// 2.a) Request

	if( (pChild = _Get_Child(_Get_Child(_Get_Child(pNode, SG_T("Request")), V_MAP(Version)), SG_T("Format"))) != NULL )
	{
		if( !Version.Cmp(SG_T("1.0.0")) )
		{
			pChild	= pChild->GetChildren();

			while( pChild )
			{
				m_Formats	+= pChild->GetName().c_str();
				m_Formats	+= SG_T("|");

				pChild	= pChild->GetNext();
			}
		}
		else
		{
			do
			{
				if( !pChild->GetName().CmpNoCase(SG_T("Format")) )
				{
					m_Formats	+= pChild->GetNodeContent().c_str();
					m_Formats	+= SG_T("|");
				}
			}
			while( (pChild = pChild->GetNext()) != NULL );
		}
	}

	//-----------------------------------------------------
	// 2.b) Exception, Vendor Specific Capabilities, User Defined Symbolization, ...


	//-----------------------------------------------------
	// 2.c) Layers

	if( (pNode = _Get_Child(pNode, SG_T("Layer"))) == NULL )
	{
		return( false );
	}

	CSG_String	s;

	if(	!(_Get_Child_PropVal(pNode, s, SG_T("BoundingBox"), SG_T("minx")) && s.asDouble(m_GeoBBox.xMin))
	||	!(_Get_Child_PropVal(pNode, s, SG_T("BoundingBox"), SG_T("miny")) && s.asDouble(m_GeoBBox.yMin))
	||	!(_Get_Child_PropVal(pNode, s, SG_T("BoundingBox"), SG_T("maxx")) && s.asDouble(m_GeoBBox.xMax))
	||	!(_Get_Child_PropVal(pNode, s, SG_T("BoundingBox"), SG_T("maxy")) && s.asDouble(m_GeoBBox.yMax)) )
	{
		m_GeoBBox.xMin	= m_GeoBBox.yMin	= m_GeoBBox.xMax	= m_GeoBBox.yMax	= 0.0;
	}

	_Get_Child_Content(pNode, m_Layers_Title	, SG_T("Title"));

	if( (pChild	= _Get_Child(pNode, V_SRS(Version))) != NULL )
	{
		do
		{
			if( !pChild->GetName().CmpNoCase(V_SRS(Version)) )
			{
				m_sProjections	.Add(pChild->GetNodeContent().c_str());

			//	m_Projections	+= Get_EPSG_Name(pChild->GetNodeContent().c_str());
				m_Projections	+= SG_T("|");
			}
		}
		while( (pChild = pChild->GetNext()) != NULL );
	}

	if( (pChild	= pNode->GetChildren()) != NULL )
	{
		do
		{
			_Get_Layer(pChild);
		}
		while( (pChild = pChild->GetNext()) != NULL );
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
bool CWMS_Capabilities::_Get_Layer(wxXmlNode *pNode)
{
	if( pNode && !pNode->GetName().CmpNoCase(SG_T("Layer")) )
	{
		CWMS_Layer	*pLayer	= new CWMS_Layer;

		if( _Get_Child_Content(pNode, pLayer->m_Name, SG_T("Name")) )
		{
			_Get_Child_Content(pNode, pLayer->m_Title, SG_T("Title"));

			//---------------------------------------------
			m_pLayers	= (CWMS_Layer **)SG_Realloc(m_pLayers, (m_nLayers + 1) * sizeof(CWMS_Layer *));
			m_pLayers[m_nLayers++]	= pLayer;

			return( true );
		}

		delete(pLayer);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CWMS_Capabilities::Get_Summary(void)
{
	CSG_String	s;

	if( m_Name.Length() > 0 )
	{
		s	+= SG_T("\n[Name] ")			+ m_Name		+ SG_T("\n");
	}

	if( m_Title.Length() > 0 )
	{
		s	+= SG_T("\n[Title] ")			+ m_Title		+ SG_T("\n");
	}

	if( m_Abstract.Length() > 0 )
	{
		s	+= SG_T("\n[Abstract] ")		+ m_Abstract	+ SG_T("\n");
	}

	if( m_Fees.Length() > 0 )
	{
		s	+= SG_T("\n[Fees] ")			+ m_Fees		+ SG_T("\n");
	}

	if( m_Online.Length() > 0 )
	{
		s	+= SG_T("\n[Online Resource] ")	+ m_Online		+ SG_T("\n");
	}

	if( m_Keywords.Get_Count() > 0 )
	{
		s	+= SG_T("\n[Keywords] ");

		for(int i=0; i<m_Keywords.Get_Count(); i++)
		{
			if( i > 0 )	s	+= SG_T(", ");

			s	+= m_Keywords[i];
		}

		s	+= SG_T("\n");
	}

	if( m_MaxLayers > 0 )
	{
		s	+= CSG_String::Format(SG_T("\n[Max. Layers] %d\n"), m_MaxLayers);
	}

	if( m_MaxWidth > 0 )
	{
		s	+= CSG_String::Format(SG_T("\n[Max. Width] %d\n"), m_MaxWidth);
	}

	if( m_MaxHeight > 0 )
	{
		s	+= CSG_String::Format(SG_T("\n[Max. Height] %d\n"), m_MaxHeight);
	}

	if( m_Contact.Length() > 0 )
	{
		s	+= SG_T("\n[Contact] ")			+ m_Contact		+ SG_T("\n");
	}

	if( m_Access.Length() > 0 )
	{
		s	+= SG_T("\n[Access] ")			+ m_Access		+ SG_T("\n");
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWMS_Import::CWMS_Import(void)
{
	Set_Name		(_TL("Import a Map via Web Map Service (WMS)"));

	Set_Author		(SG_T("O. Conrad (c) 2008"));

	Set_Description	(_TW(
		"This module works as Web Map Service (WMS) client. "
		"More information on the WMS specifications can be obtained from the "
		"Open Geospatial Consortium (OGC) at "
		"<a href=\"http://www.opengeospatial.org/\">http://www.opengeospatial.org/</a>. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "MAP"				, _TL("WMS Map"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_String(
		NULL	, "SERVER"			, _TL("Server"),
		_TL(""),
	//	SG_T("www.gaia-mv.de/dienste/DTK10f")	// 260000.0x, 5950000.0y Cellsize 1.0
	//	SG_T("www.gis2.nrw.de/wmsconnector/wms/stobo")
	//	SG_T("www2.demis.nl/mapserver/request.asp")
	//	SG_T("www.geoserver.nrw.de/GeoOgcWms1.3/servlet/TK25")
	//	SG_T("www.geographynetwork.com/servlet/com.esri.wms.Esrimap")
		SG_T("ogc.bgs.ac.uk/cgi-bin/BGS_Bedrock_and_Superficial_Geology/wms")	// WGS84: Center -3.5x 55.0y Cellsize 0.005
 	);

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

//---------------------------------------------------------
CWMS_Import::~CWMS_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::On_Execute(void)
{
	wxHTTP				Server;
	CSG_String			sServer, sDirectory, sVersion;
	CWMS_Capabilities	Capabilities;

	//-----------------------------------------------------
	sServer		= Parameters("SERVER")->asString();

	if( sServer.Contains(SG_T("http://")) )
	{
		sServer		= Parameters("SERVER")->asString() + 7;
	}

	sDirectory	= SG_T("/") + sServer.AfterFirst(SG_T('/'));
	sServer		= sServer.BeforeFirst(SG_T('/'));

	//-----------------------------------------------------
	Server.SetUser		(Parameters("USERNAME")->asString());
	Server.SetPassword	(Parameters("PASSWORD")->asString());

	if( Server.Connect(sServer.c_str()) == false )
	{
		Message_Add(_TL("Unable to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	if( Capabilities.Create(&Server, sDirectory, sVersion) == false )
	{
		Message_Add(_TL("Unable to get capabilities."));

		return( false );
	}

	Message_Add(Capabilities.Get_Summary(), false);

	Message_Add(CSG_String::Format(SG_T("\n%s\nmin: %fx - %fy\nmax: %fx - %fy\n"), _TL("Extent"),
		Capabilities.m_GeoBBox.xMin,
		Capabilities.m_GeoBBox.yMin,
		Capabilities.m_GeoBBox.xMax,
		Capabilities.m_GeoBBox.yMax), false
	);

	//-----------------------------------------------------
	if( Get_Map(&Server, sDirectory, sVersion, Capabilities) == false )
	{
		Message_Add(_TL("Unable to get map."));

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
bool CWMS_Import::Do_Dialog(CWMS_Capabilities &Cap)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::Get_Map(wxHTTP *pServer, const CSG_String &Directory, const CSG_String &Version, CWMS_Capabilities &Cap)
{
	bool	bResult	= false;

	int				i, n;
	CSG_Rect		r(Cap.m_GeoBBox);
	CSG_Parameters	p;

	//-----------------------------------------------------
//	if( Cap.m_MaxWidth  > 2 && NX > Cap.m_MaxWidth  )	NX	= Cap.m_MaxWidth;
//	if( Cap.m_MaxHeight > 2 && NY > Cap.m_MaxHeight )	NY	= Cap.m_MaxHeight;

	p.Add_Range	(NULL	, "X_RANGE"	, _TL("X Range")	, _TL(""), r.Get_XMin(), r.Get_XMax(), r.Get_XMin(), r.Get_XRange() > 0.0, r.Get_XMax(), r.Get_XRange() > 0.0);
	p.Add_Range	(NULL	, "Y_RANGE"	, _TL("Y Range")	, _TL(""), r.Get_YMin(), r.Get_YMax(), r.Get_YMin(), r.Get_YRange() > 0.0, r.Get_YMax(), r.Get_YRange() > 0.0);

	p.Add_Value	(NULL	, "CELLSIZE", _TL("Cellsize")	, _TL(""), PARAMETER_TYPE_Double, r.Get_XRange() / 2001.0, 0.0, true);

	p.Add_Choice(NULL	, "FORMAT"	, _TL("Format")		, _TL(""), Cap.m_Formats);
	p.Add_Choice(NULL	, "PROJ"	, _TL("Projections"), _TL(""), Cap.m_Projections);

	CSG_Parameter	*pNode	= p("FORMAT");
	for(i=0; i<pNode->asChoice()->Get_Count(); i++)
	{
		CSG_String	s(pNode->asChoice()->Get_Item(i));
		if( !s.CmpNoCase(SG_T("image/png")) )
			pNode->Set_Value(i);
	}

	for(i=0; i<Cap.m_nLayers; i++)
	{
		p.Add_Value(NULL	, Cap.m_pLayers[i]->m_Name, Cap.m_pLayers[i]->m_Title, SG_T(""), PARAMETER_TYPE_Bool, false);
	}

	//-----------------------------------------------------
	if( pServer && Dlg_Parameters(&p, _TL("WMS Import")) )
	{
		long		tFormat;
		int			NX, NY;
		double		Cellsize;
		CSG_String	Layers, Format;

		//-------------------------------------------------
		r.Assign(
			p("X_RANGE")->asRange()->Get_LoVal(),
			p("Y_RANGE")->asRange()->Get_LoVal(),
			p("X_RANGE")->asRange()->Get_HiVal(),
			p("Y_RANGE")->asRange()->Get_HiVal()
		);

		Cellsize	= p("CELLSIZE")	->asDouble();

		NX			= 1 + (int)(r.Get_XRange() / Cellsize);
		NY			= 1 + (int)(r.Get_YRange() / Cellsize);

		//-------------------------------------------------
		Layers.Clear();

		for(i=0, n=0; i<Cap.m_nLayers; i++)
		{
			if( p(Cap.m_pLayers[i]->m_Name)->asBool() )
			{
				if( n++ > 0 )	Layers	+= SG_T(",");
				Layers	+= Cap.m_pLayers[i]->m_Name;
			}
		}

		if( n == 0 )
		{
			return( false );
		}

		//-------------------------------------------------
		Format	= p("FORMAT")->asString();

		if(      Format.Contains(SG_T("image/gif")) )	tFormat	= wxBITMAP_TYPE_GIF;
		else if( Format.Contains(SG_T("image/jpeg")) )	tFormat	= wxBITMAP_TYPE_JPEG;
		else if( Format.Contains(SG_T("image/png")) )	tFormat	= wxBITMAP_TYPE_PNG;
		else if( Format.Contains(SG_T("image/wbmp")) )	tFormat	= wxBITMAP_TYPE_BMP;
		else if( Format.Contains(SG_T("image/bmp")) )	tFormat	= wxBITMAP_TYPE_BMP;
		else if( Format.Contains(SG_T("image/tiff")) )	tFormat	= wxBITMAP_TYPE_TIF;
		else if( Format.Contains(SG_T("GIF")) )			tFormat	= wxBITMAP_TYPE_GIF;
		else if( Format.Contains(SG_T("JPEG")) )		tFormat	= wxBITMAP_TYPE_JPEG;
		else if( Format.Contains(SG_T("PNG")) )			tFormat	= wxBITMAP_TYPE_PNG;
		else
		{
			return( false );
		}

		//-------------------------------------------------
		CSG_String	sRequest(Directory);

		sRequest	+= SG_T("?SERVICE=WMS");
		sRequest	+= SG_T("&VERSION=")	+ Version;
		sRequest	+= SG_T("&REQUEST=GetMap");

		sRequest	+= SG_T("&LAYERS=")		+ Layers;

		if( Cap.m_sProjections.Get_Count() > 0 )
			sRequest	+= S_SRS(Version)		+ Cap.m_sProjections[p("PROJ")->asInt()];

		sRequest	+= SG_T("&FORMAT=")		+ Format;

		sRequest	+= CSG_String::Format(SG_T("&WIDTH=%d&HEIGHT=%d"), NX, NY);
		sRequest	+= CSG_String::Format(SG_T("&BBOX=%f,%f,%f,%f"), r.m_rect.xMin, r.m_rect.yMin, r.m_rect.xMax, r.m_rect.yMax);

		Message_Add(sRequest, true);

		//-------------------------------------------------
		wxInputStream	*pStream;

		if( (pStream = pServer->GetInputStream(sRequest.c_str())) == NULL )
		{
			Message_Add(_TL("could not open GetMap stream"));
		}
		else
		{
			wxImage	Image;

			if( Image.LoadFile(*pStream, tFormat) == false )
			{
				Message_Add(_TL("could not read image"));

				CSG_String	s	= SG_T("\n");

				pStream->SeekI(0, wxFromStart);

				while( !pStream->Eof() )
				{
					s	+= pStream->GetC();
				}

				Message_Add(s);
			}
			else
			{
				CSG_Grid	*pGrid	= SG_Create_Grid(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), Cellsize, r.m_rect.xMin, r.m_rect.yMin);

				for(int y=0, yy=pGrid->Get_NY()-1; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, yy--)
				{
					for(int x=0; x<pGrid->Get_NX(); x++)
					{
						pGrid->Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
					}
				}

				//-----------------------------------------
				pGrid->Set_Name(Cap.m_Title);
				Parameters("MAP")->Set_Value(pGrid);
				DataObject_Set_Colors(pGrid, 100, SG_COLORS_BLACK_WHITE);

				CSG_Parameters	Parms;

				if( DataObject_Get_Parameters(pGrid, Parms) && Parms("COLORS_TYPE") )
				{
					Parms("COLORS_TYPE")->Set_Value(3);	// Color Classification Type: RGB

					DataObject_Set_Parameters(pGrid, Parms);
				}

				bResult	= true;
			}

			delete(pStream);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
