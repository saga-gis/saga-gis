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
#include "wms_import.h"

#include <wx/image.h>
#include <wx/protocol/http.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWMS_Capabilities::CWMS_Capabilities(void)
{}

CWMS_Capabilities::CWMS_Capabilities(const CSG_String &Server, const CSG_String &Version)
{
	Create(Server, Version);
}

//---------------------------------------------------------
CWMS_Capabilities::~CWMS_Capabilities(void)
{
	Destroy();
}

//---------------------------------------------------------
void CWMS_Capabilities::Destroy(void)
{
	m_Name        .Clear();
	m_Title       .Clear();
	m_Abstract    .Clear();
	m_Formats     .Clear();
	m_Projections .Clear();
	m_Layers_Name .Clear();
	m_Layers_Title.Clear();
}

//---------------------------------------------------------
#define V_SRS(Version)	(Version.Find("1.3"  ) == 0 ?  "CRS"  :  "SRS"  )
#define S_SRS(Version)	(Version.Cmp ("1.3.0") == 0 ? "&CRS=" : "&SRS=" )
#define V_MAP(Version)	(Version.Cmp ("1.0.0") == 0 ? "Map"   : "GetMap")

//---------------------------------------------------------
#define CAP_GET_STRING(GROUP, ID, VALUE, MUSTEXIST)	if( GROUP(ID) ) VALUE = GROUP[ID].Get_Content();            else if( !MUSTEXIST ) VALUE = "" ; else return( false );
#define CAP_GET_DOUBLE(GROUP, ID, VALUE, MUSTEXIST)	if( GROUP(ID) ) VALUE = GROUP[ID].Get_Content().asDouble(); else if( !MUSTEXIST ) VALUE = 0.0; else return( false );
#define CAP_GET____INT(GROUP, ID, VALUE, MUSTEXIST)	if( GROUP(ID) ) VALUE = GROUP[ID].Get_Content().asInt   (); else if( !MUSTEXIST ) VALUE = 0  ; else return( false );

//---------------------------------------------------------
bool CWMS_Capabilities::Create(const CSG_String &Path, const CSG_String &Version)
{
	Destroy();

	CSG_String	Request	= "http://" + Path + "?SERVICE=WMS&VERSION=" + Version + "&REQUEST=GetCapabilities";

	CSG_MetaData	Capabilities;

	if( !Capabilities.Load(Request)
	||  !Capabilities.Get_Property("version", m_Version)
	||  !Capabilities("Service")
	||  !Capabilities("Capability")
	||  !Capabilities["Capability"]("Request")
	||  !Capabilities["Capability"]["Request"](V_MAP(m_Version))
	||  !Capabilities["Capability"]("Layer") )
	{
		return( false );
	}

	int		i;

	//-----------------------------------------------------
	const CSG_MetaData	&Service	= Capabilities["Service"];

	CAP_GET_STRING(Service, "Name"      , m_Name      ,  true);
	CAP_GET_STRING(Service, "Title"     , m_Title     ,  true);
	CAP_GET_STRING(Service, "Abstract"  , m_Abstract  ,  true);
	CAP_GET____INT(Service, "LayerLimit", m_LayerLimit, false);
	CAP_GET____INT(Service, "MaxWidth"  , m_MaxWidth  , false);
	CAP_GET____INT(Service, "MaxHeight" , m_MaxHeight , false);

	//-----------------------------------------------------
	const CSG_MetaData	&GetMap	= Capabilities["Capability"]["Request"][V_MAP(m_Version)];

	for(i=0; i<GetMap.Get_Children_Count(); i++)
	{
		if( GetMap[i].Cmp_Name   ("Format"    )
		&& (GetMap[i].Cmp_Content("image/png" )
		||  GetMap[i].Cmp_Content("image/jpeg")
		||  GetMap[i].Cmp_Content("image/gif" )
		||  GetMap[i].Cmp_Content("image/tiff")) )
		{
			m_Formats	+= GetMap[i].Get_Content() + "|";
		}
	}

	//-----------------------------------------------------
	const CSG_MetaData	&Layer	= Capabilities["Capability"]["Layer"];

	if( m_Version.Cmp("1.3.0") == 0 )
	{
		if( !Layer("EX_GeographicBoundingBox") )
		{
			return( false );
		}

		CAP_GET_DOUBLE(Layer["EX_GeographicBoundingBox"], "westBoundLongitude", m_Extent.xMin, true);
		CAP_GET_DOUBLE(Layer["EX_GeographicBoundingBox"], "eastBoundLongitude", m_Extent.xMax, true);
		CAP_GET_DOUBLE(Layer["EX_GeographicBoundingBox"], "southBoundLatitude", m_Extent.yMin, true);
		CAP_GET_DOUBLE(Layer["EX_GeographicBoundingBox"], "northBoundLatitude", m_Extent.yMax, true);
	}
	else // version < 1.3.0
	{
		if( !Layer("LatLonBoundingBox")
		||  !Layer["LatLonBoundingBox"].Get_Property("minx", m_Extent.xMin)
		||  !Layer["LatLonBoundingBox"].Get_Property("maxx", m_Extent.xMax)
		||  !Layer["LatLonBoundingBox"].Get_Property("miny", m_Extent.yMin)
		||  !Layer["LatLonBoundingBox"].Get_Property("maxy", m_Extent.yMax) )
		{
			return( false );
		}
	}

	for(i=0; i<Layer.Get_Children_Count(); i++)
	{
		if( Layer[i].Cmp_Name(V_SRS(m_Version)) )
		{
			m_Projections	+= Layer[i].Get_Content() + "|";
		}

		else if( Layer[i].Cmp_Name("Layer") )
		{
			m_Layers_Name	+= Layer[i].Get_Content("Name" );
			m_Layers_Title	+= Layer[i].Get_Content("Title");
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
CWMS_Import::CWMS_Import(void)
{
	Set_Name		(_TL("Import a Map via Web Map Service (WMS)"));

	Set_Author		("O. Conrad (c) 2008");

	Set_Description	(_TW(
		"This tool works as Web Map Service (WMS) client. "
		"More information on the WMS specifications can be obtained from the "
		"Open Geospatial Consortium (OGC). "
	));

	Add_Reference("http://www.opengeospatial.org",
		SG_T("Open Geospatial Consortium")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		"", "MAP"		, _TL("WMS Map"),
		_TL("")
	);

	//-----------------------------------------------------
	Parameters.Add_String(
		"", "SERVER"	, _TL("Server"),
		_TL(""),
	//	"www.gaia-mv.de/dienste/DTK10f"	// 260000.0x, 5950000.0y Cellsize 1.0
	//	"www.gis2.nrw.de/wmsconnector/wms/stobo"
	//	"www2.demis.nl/mapserver/request.asp"
	//	"www.geoserver.nrw.de/GeoOgcWms1.3/servlet/TK25"
	//	"www.geographynetwork.com/servlet/com.esri.wms.Esrimap"
		"ogc.bgs.ac.uk/cgi-bin/BGS_Bedrock_and_Superficial_Geology/wms"	// WGS84: Center -3.5x 55.0y Cellsize 0.005
 	);

	Parameters.Add_String("", "USERNAME", _TL("User Name"), _TL(""), "");
	Parameters.Add_String("", "PASSWORD", _TL("Password" ), _TL(""), "", false, true);

	//-----------------------------------------------------
	Parameters.Add_Info_String("", "ABSTRACT", _TL("Abstract"), _TL(""), "", true);
	Parameters.Add_Double     ("", "GCS_XMIN", _TL("Left"    ), _TL(""));
	Parameters.Add_Double     ("", "GCS_XMAX", _TL("Right"   ), _TL(""));
	Parameters.Add_Double     ("", "GCS_YMIN", _TL("Bottom"  ), _TL(""));
	Parameters.Add_Double     ("", "GCS_YMAX", _TL("Top"     ), _TL(""));
	Parameters.Add_Choice     ("", "LAYER"   , _TL("Layer"   ), _TL(""), "");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWMS_Import::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SERVER") )
	{
		CWMS_Capabilities	Capabilities;

		if( Capabilities.Create(pParameter->asString(), "1.1.1") )
		{
			pParameters->Set_Parameter("ABSTRACT", Capabilities.m_Abstract   );
			pParameters->Set_Parameter("GCS_XMIN", Capabilities.m_Extent.xMin);
			pParameters->Set_Parameter("GCS_XMAX", Capabilities.m_Extent.xMax);
			pParameters->Set_Parameter("GCS_YMIN", Capabilities.m_Extent.yMin);
			pParameters->Set_Parameter("GCS_YMAX", Capabilities.m_Extent.yMax);

			CSG_String	Items;

			for(int i=0; i<Capabilities.m_Layers_Title.Get_Count(); i++)
			{
				Items	+= Capabilities.m_Layers_Title[i] + "|";
			}

			pParameters->Get_Parameter("LAYER")->asChoice()->Set_Items(Items);
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CWMS_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::On_Execute(void)
{
	CSG_String	Address(Parameters("SERVER")->asString());

	//-----------------------------------------------------
	CWMS_Capabilities	Capabilities;

	if( Capabilities.Create(Address, "1.1.1") == false )
	{
		Message_Add(_TL("Unable to get capabilities."));

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Domain(      Address.BeforeFirst('/'));
	CSG_String	Path  ("/" + Address.AfterFirst ('/'));

	wxHTTP	Server;

	Server.SetUser    (Parameters("USERNAME")->asString());
	Server.SetPassword(Parameters("PASSWORD")->asString());

	if( Server.Connect(Domain.c_str()) == false )
	{
		Message_Add(_TL("Unable to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	if( Get_Map(&Server, Path, Capabilities) == false )
	{
		Message_Add(_TL("Unable to get map."));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::Do_Dialog(CWMS_Capabilities &Cap)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::Get_Map(wxHTTP *pServer, const CSG_String &Directory, CWMS_Capabilities &Cap)
{
	int				i, n;
	CSG_Rect		r(Cap.m_Extent);
	CSG_Parameters	p;

	//-----------------------------------------------------
//	if( Cap.m_MaxWidth  > 2 && NX > Cap.m_MaxWidth  )	NX	= Cap.m_MaxWidth;
//	if( Cap.m_MaxHeight > 2 && NY > Cap.m_MaxHeight )	NY	= Cap.m_MaxHeight;

	p.Add_Range ("", "X_RANGE" , _TL("X Range"    ), _TL(""), r.Get_XMin(), r.Get_XMax(), r.Get_XMin(), r.Get_XRange() > 0.0, r.Get_XMax(), r.Get_XRange() > 0.0);
	p.Add_Range ("", "Y_RANGE" , _TL("Y Range"    ), _TL(""), r.Get_YMin(), r.Get_YMax(), r.Get_YMin(), r.Get_YRange() > 0.0, r.Get_YMax(), r.Get_YRange() > 0.0);
	p.Add_Double("", "CELLSIZE", _TL("Cellsize"   ), _TL(""), r.Get_XRange() / 2001.0, 0.0, true);
	p.Add_Choice("", "FORMAT"  , _TL("Format"     ), _TL(""), Cap.m_Formats);
	p.Add_Choice("", "PROJ"    , _TL("Projections"), _TL(""), Cap.m_Projections);

	CSG_Parameter	*pFormat	= p("FORMAT");

	for(i=0; i<pFormat->asChoice()->Get_Count(); i++)
	{
		if( !SG_STR_CMP(pFormat->asChoice()->Get_Item(i), "image/png") )
		{
			pFormat->Set_Value(i);
		}
	}

	for(i=0; i<Cap.m_Layers_Name.Get_Count(); i++)
	{
		p.Add_Bool("", Cap.m_Layers_Name[i], Cap.m_Layers_Title[i], "", false);
	}

	//-----------------------------------------------------
	if( !Dlg_Parameters(&p, _TL("WMS Import")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	int			NX, NY;
	double		Cellsize;
	CSG_String	Layers, Format;

	r.Assign(
		p("X_RANGE")->asRange()->Get_LoVal(),
		p("Y_RANGE")->asRange()->Get_LoVal(),
		p("X_RANGE")->asRange()->Get_HiVal(),
		p("Y_RANGE")->asRange()->Get_HiVal()
	);

	Cellsize	= p("CELLSIZE")	->asDouble();

	NX			= 1 + (int)(r.Get_XRange() / Cellsize);
	NY			= 1 + (int)(r.Get_YRange() / Cellsize);

	//-----------------------------------------------------
	Layers.Clear();

	for(i=0, n=0; i<Cap.m_Layers_Name.Get_Count(); i++)
	{
		if( p(Cap.m_Layers_Name[i])->asBool() )
		{
			if( n++ > 0 )	Layers	+= ",";

			Layers	+= Cap.m_Layers_Name[i];
		}
	}

	if( n == 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	wxBitmapType	tFormat;

	Format	= p("FORMAT")->asString();

	if(      Format.Contains("image/gif" ) )	tFormat	= wxBITMAP_TYPE_GIF ;
	else if( Format.Contains("image/jpeg") )	tFormat	= wxBITMAP_TYPE_JPEG;
	else if( Format.Contains("image/png" ) )	tFormat	= wxBITMAP_TYPE_PNG ;
	else if( Format.Contains("image/wbmp") )	tFormat	= wxBITMAP_TYPE_BMP ;
	else if( Format.Contains("image/bmp" ) )	tFormat	= wxBITMAP_TYPE_BMP ;
	else if( Format.Contains("image/tiff") )	tFormat	= wxBITMAP_TYPE_TIF ;
	else if( Format.Contains("GIF"       ) )	tFormat	= wxBITMAP_TYPE_GIF ;
	else if( Format.Contains("JPEG"      ) )	tFormat	= wxBITMAP_TYPE_JPEG;
	else if( Format.Contains("PNG"       ) )	tFormat	= wxBITMAP_TYPE_PNG ;
	else
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String	sRequest(Directory);

	sRequest	+= "?SERVICE=WMS";
	sRequest	+= "&VERSION="	+ Cap.m_Version;
	sRequest	+= "&REQUEST=GetMap";

	sRequest	+= "&LAYERS="	+ Layers;

	if( Cap.m_Projections.Length() > 0 )
		sRequest	+= CSG_String(S_SRS(Cap.m_Version)) + p("PROJ")->asString();

	sRequest	+= "&FORMAT="	+ Format;

	sRequest	+= CSG_String::Format("&WIDTH=%d&HEIGHT=%d", NX, NY);
	sRequest	+= CSG_String::Format("&BBOX=%f,%f,%f,%f", r.m_rect.xMin, r.m_rect.yMin, r.m_rect.xMax, r.m_rect.yMax);

	Message_Add(sRequest, true);

	//-----------------------------------------------------
	wxInputStream	*pStream;

	if( (pStream = pServer->GetInputStream(sRequest.c_str())) == NULL )
	{
		Message_Add(_TL("could not open GetMap stream"));

		return( false );
	}

	//-----------------------------------------------------
	wxImage	Image;

	if( Image.LoadFile(*pStream, tFormat) == false )
	{
		delete(pStream);

		Message_Add(_TL("could not read image"));

		return( false );
	}

	//-----------------------------------------------------
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

	delete(pStream);

	return( true );
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
