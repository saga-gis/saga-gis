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
#include <wx/mstream.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWMS_Capabilities::CWMS_Capabilities(void)
{}

CWMS_Capabilities::CWMS_Capabilities(CSG_HTTP &Server, const CSG_String &Path, const CSG_String &Version)
{
	Create(Server, Path, Version);
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
	Capabilities.Destroy();
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
bool CWMS_Capabilities::Create(CSG_HTTP &Server, const CSG_String &Path, const CSG_String &Version)
{
	Destroy();

	if( !Server.Request(Path + "?SERVICE=WMS&VERSION=" + Version + "&REQUEST=GetCapabilities", Capabilities) )
	{
		return( false );
	}

	if( !Capabilities.Get_Property("version", m_Version)
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
			CSG_String	Name (Layer[i].Get_Content("Name" ));
			CSG_String	Title(Layer[i].Get_Content("Title"));

			if( Name.is_Empty() == true )
			{
				Name	= Title;
			}

			if( Name.is_Empty() == false )
			{
				m_Layers_Name	+= Name ;
				m_Layers_Title	+= Title;
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

	Parameters.Add_Grid_List(
		"", "LEGENDS"	, _TL("Legends"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	//-----------------------------------------------------
	Parameters.Add_String(
		"", "SERVER"	, _TL("Server"),
		_TL(""),
		"ogc.bgs.ac.uk/cgi-bin/BGS_Bedrock_and_Superficial_Geology/wms"	// WGS84: Center -3.5x 55.0y Cellsize 0.005
 	);

	Parameters.Add_Choice(
		"", "VERSION"	, _TL("Version"),
		_TL(""),
		CSG_String::Format("%s|%s",
			SG_T("1.1.1"),
			SG_T("1.3.0")
		)
 	);

	Parameters.Add_Bool(
		"", "LEGEND"	, _TL("Legend"),
		_TL(""),
		false
	);

	Parameters.Add_String("", "USERNAME", _TL("User Name"), _TL(""), "");
	Parameters.Add_String("", "PASSWORD", _TL("Password" ), _TL(""), "", false, true);

	//-----------------------------------------------------
	Parameters.Add_Info_String("", "ABSTRACT", _TL("Abstract"), _TL(""), "", true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::On_Before_Execution(void)
{
	On_Parameter_Changed(&Parameters, Parameters("SERVER"));

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
int CWMS_Import::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SERVER"  )
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "USERNAME")
	||  !SG_STR_CMP(pParameter->Get_Identifier(), "PASSWORD") )
	{
		CSG_HTTP	Server;	CSG_String	Path, Abstract("---");

		if( Get_Server(Server, Path,
			(*pParameters)("SERVER"  )->asString(),
			(*pParameters)("USERNAME")->asString(),
			(*pParameters)("PASSWORD")->asString()) )
		{
			CWMS_Capabilities	Capabilities;

			if( Capabilities.Create(Server, Path, (*pParameters)("VERSION")->asString()) )
			{
				Abstract	 = Capabilities.m_Abstract;

				Abstract	+= CSG_String::Format("\n\n%s:", _TL("Extent"));

				Abstract	+= CSG_String::Format("\nW-E: [%f] - [%f]", Capabilities.m_Extent.xMin, Capabilities.m_Extent.xMax);
				Abstract	+= CSG_String::Format("\nS-N: [%f] - [%f]", Capabilities.m_Extent.yMin, Capabilities.m_Extent.yMax);

				Abstract	+= CSG_String::Format("\n\n%s:", _TL("Layers"));

				for(int i=0; i<Capabilities.m_Layers_Title.Get_Count(); i++)
				{
					Abstract	+= "\n" + Capabilities.m_Layers_Title[i];
				}
			}
		}

		pParameters->Set_Parameter("ABSTRACT", Abstract);
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
	CSG_HTTP	Server;	CSG_String	Path;

	if( !Get_Server(Server, Path, Parameters("SERVER")->asString(),
		Parameters("USERNAME")->asString(),
		Parameters("PASSWORD")->asString()) )
	{
		Message_Add(_TL("Failed to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	CWMS_Capabilities	Capabilities;

	if( !Capabilities.Create(Server, Path, Parameters("VERSION")->asString()) )
	{
		Message_Add(_TL("Failed to get capabilities."));

		return( false );
	}

	//-----------------------------------------------------
	if( Get_Map(Server, Path, Capabilities) == false )
	{
		Message_Add(_TL("Failed to get map."));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::Get_Server(CSG_HTTP &Server, CSG_String &Path, const CSG_String &Address, const CSG_String &Username, const CSG_String &Password)
{
	CSG_String	Host, _Address(Address);

	#define SERVER_TRIM(s, p)	{ wxString sp(p); sp += "://"; if( s.Find(p) == 0 ) { s = s.Right(s.Length() - sp.Length()); } }

//	SERVER_TRIM(_Address, "https");
	SERVER_TRIM(_Address, "http");

	Host	= _Address.BeforeFirst('/');
	Path	= _Address.AfterFirst ('/');

	return( Server.Create(Host, Username, Password) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::Get_Map(CSG_HTTP &Server, const CSG_String &Path, CWMS_Capabilities &Capabilities)
{
	int				i;
	CSG_Rect		r(Capabilities.m_Extent);
	CSG_Parameters	P;

	//-----------------------------------------------------
//	if( Capabilities.m_MaxWidth  > 2 && NX > Capabilities.m_MaxWidth  )	NX	= Capabilities.m_MaxWidth;
//	if( Capabilities.m_MaxHeight > 2 && NY > Capabilities.m_MaxHeight )	NY	= Capabilities.m_MaxHeight;

	P.Add_Range ("", "X_RANGE" , _TL("X Range"    ), _TL(""), r.Get_XMin(), r.Get_XMax(), r.Get_XMin(), r.Get_XRange() > 0.0, r.Get_XMax(), r.Get_XRange() > 0.0);
	P.Add_Range ("", "Y_RANGE" , _TL("Y Range"    ), _TL(""), r.Get_YMin(), r.Get_YMax(), r.Get_YMin(), r.Get_YRange() > 0.0, r.Get_YMax(), r.Get_YRange() > 0.0);
	P.Add_Double("", "CELLSIZE", _TL("Cellsize"   ), _TL(""), r.Get_XRange() / 1001., 0., true);
	P.Add_Choice("", "FORMAT"  , _TL("Format"     ), _TL(""), Capabilities.m_Formats);
	P.Add_Choice("", "PROJ"    , _TL("Projections"), _TL(""), Capabilities.m_Projections);

	P("FORMAT")->Set_Value("image/png");
	P("PROJ"  )->Set_Value("EPSG:4326");

	for(i=0; i<Capabilities.m_Layers_Name.Get_Count(); i++)
	{
		if( !Capabilities.m_Layers_Name[i].is_Empty() )
		{
			P.Add_Bool("", Capabilities.m_Layers_Name[i], Capabilities.m_Layers_Title[i], "", false);
		}
	}

	//-----------------------------------------------------
	if( !Dlg_Parameters(&P, _TL("WMS Import")) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System	System(P("CELLSIZE")->asDouble(),
		P("X_RANGE")->asRange()->Get_LoVal(), P("Y_RANGE")->asRange()->Get_LoVal(),
		P("X_RANGE")->asRange()->Get_HiVal(), P("Y_RANGE")->asRange()->Get_HiVal()
	);

	//-----------------------------------------------------
	CSG_String	Layers, Styles;

	for(i=0; i<Capabilities.m_Layers_Name.Get_Count(); i++)
	{
		if( P(Capabilities.m_Layers_Name[i])->asBool() )
		{
			if( !Layers.is_Empty() )	{	Layers	+= ","; Styles	+= ",";	}

			Layers	+= Capabilities.m_Layers_Name[i];
		}
	}

	if( Layers.is_Empty() )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_String	Request(Path);

	Request	+= "?SERVICE=WMS";
	Request	+= "&VERSION=" + Capabilities.m_Version;
	Request	+= "&REQUEST=GetMap";
	Request	+= "&LAYERS=" + Layers;
	Request	+= "&STYLES=" + Styles;

	if( Capabilities.m_Projections.Length() > 0 )
	{
		Request	+= CSG_String(S_SRS(Capabilities.m_Version)) + P("PROJ")->asString();
	}

	Request	+= CSG_String::Format("&FORMAT=%s", P("FORMAT")->asString());
	Request	+= CSG_String::Format("&WIDTH=%d&HEIGHT=%d", System.Get_NX(), System.Get_NY());
	Request	+= CSG_String::Format("&BBOX=%f,%f,%f,%f", System.Get_XMin(), System.Get_YMin(), System.Get_XMax(), System.Get_YMax());

//	Message_Add("\n" + Request + "\n", false);

	//-----------------------------------------------------
	CSG_Bytes	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add("\n", false);

		Message_Add(_TL("Failed to retrieve stream"), false);

		return( false );
	}

	//-----------------------------------------------------
	wxMemoryInputStream	Stream((const void *)Answer.Get_Bytes(), (size_t)Answer.Get_Count());

	wxImage	Image;

	if( Image.LoadFile(Stream) == false )
	{
		Message_Add(_TL("Failed to read image"));

		if( Answer[Answer.Get_Count() - 1] == '\0' )
		{
			Message_Add("\n", false);

			Message_Add((const char *)Answer.Get_Bytes(), false);
		}

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= SG_Create_Grid(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin());

	#pragma omp parallel for
	for(int y=0; y<pGrid->Get_NY(); y++)
	{
		int	yy	= pGrid->Get_NY() - 1 - y;

		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			pGrid->Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
		}
	}

	//-----------------------------------------
	pGrid->Set_Name(Capabilities.m_Title);

	Parameters("MAP")->Set_Value(pGrid);

	DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 6);	// Color Classification Type: RGB Coded Values

	CSG_String	Projection	= P("PROJ")->asString();

	int	EPSG;

	if( Projection.Find("EPSG:") < 0 || !Projection.AfterFirst(':').asInt(EPSG) )
	{
		EPSG	= 4326;
	}

	pGrid->Get_Projection().Create(EPSG);

	pGrid->Get_MetaData().Add_Child(Capabilities.Capabilities);

	//-----------------------------------------------------
	if( Parameters("LEGEND")->asBool() )
	{
		for(i=0; i<Capabilities.m_Layers_Name.Get_Count(); i++)
		{
			if( P(Capabilities.m_Layers_Name[i])->asBool() )
			{
				Get_Legend(Server, Path, Capabilities.m_Version, Capabilities.m_Layers_Name[i], "image/png");
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWMS_Import::Get_Legend(CSG_HTTP &Server, const CSG_String &Path, const CSG_String &Version, const CSG_String &Layer, const CSG_String &Format)
{
	CSG_String	Request(Path);

	Request	+= "?SERVICE=WMS";
	Request	+= "&VERSION=" + Version;
	Request	+= "&REQUEST=GetLegendGraphic";
	Request	+= "&FORMAT=" + Format;
	Request	+= "&LAYER=" + Layer;

	//-----------------------------------------------------
	CSG_Bytes	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add("\n", false);

		Message_Add(_TL("Failed to retrieve stream"), false);

		return( false );
	}

	//-----------------------------------------------------
	wxMemoryInputStream	Stream((const void *)Answer.Get_Bytes(), (size_t)Answer.Get_Count());

	wxImage	Image;

	if( Image.LoadFile(Stream) == false )
	{
		Message_Add(_TL("Failed to read image"));

		if( Answer[Answer.Get_Count() - 1] == '\0' )
		{
			Message_Add("\n", false);

			Message_Add((const char *)Answer.Get_Bytes(), false);
		}

		return( false );
	}


	//-----------------------------------------------------
	CSG_Grid	*pGrid	= SG_Create_Grid(SG_DATATYPE_Int, Image.GetWidth(), Image.GetHeight(), 1, 0, 0);

	#pragma omp parallel for
	for(int y=0; y<pGrid->Get_NY(); y++)
	{
		int	yy	= pGrid->Get_NY() - 1 - y;

		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			pGrid->Set_Value(x, y, SG_GET_RGB(Image.GetRed(x, yy), Image.GetGreen(x, yy), Image.GetBlue(x, yy)));
		}
	}

	//-----------------------------------------
	pGrid->Set_Name(Layer + " - " + _TL("Legend"));

	Parameters("LEGENDS")->asGridList()->Add_Item(pGrid);

	DataObject_Add(pGrid);
	DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 6);	// Color Classification Type: RGB Coded Values

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
