
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  garden_webservices                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    geocoding.cpp                      //
//                                                       //
//                 Copyrights (C) 2018                   //
//                     Olaf Conrad                       //
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
#include "geocoding.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoCoding::CGeoCoding(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Geocoding"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Geocoding of addresses using internet geocoding services. "
	));

	Add_Reference("http://www.datasciencetoolkit.org",
		SG_T("The Data Science Toolkit")
	);

	Add_Reference("https://developers.google.com/maps/documentation/geocoding/start",
		SG_T("Google Maps Platform, Geocoding API")
	);

	Add_Reference("https://msdn.microsoft.com/en-us/library/ff701714.aspx",
		SG_T("Bing Maps Rest Services, Find a Location by Address")
	);

	Add_Reference("https://developer.mapquest.com/documentation/geocoding-api/",
		SG_T("MapQuest Developer, Geocoding API")
	);

	Add_Reference("https://wiki.openstreetmap.org/wiki/Nominatim",
		SG_T("Nominatim at OpenStreetMap Wiki")
	);

//	Add_Reference("https://code.google.com/archive/p/openaddresses/wikis/RESTService.wiki",
//		SG_T("OpenAddresses - RESTService.wiki")
//	);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"LOCATIONS"	, _TL("Locations"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table("",
		"ADDRESSES"	, _TL("Address List"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Table_Field("ADDRESSES",
		"FIELD"		, _TL("Address Field"),
		_TL("")
	);

	Parameters.Add_String("ADDRESSES",
		"ADDRESS"	, _TL("Single Address"),
		_TL(""),
		"Bundesstrasse 55, Hamburg, Germany"
	);

	Parameters.Add_Choice("",
		"PROVIDER"	, _TL("Service Provider"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s",
			SG_T("The Data Science Toolkit"),
			SG_T("Google"),
			SG_T("Bing"),
			SG_T("MapQuest"),
			SG_T("Yahoo"),
			SG_T("OpenStreetMap (Nominatim)")	// not supported now, because it works only through https
		), 0
	);

	Parameters.Add_String("PROVIDER",
		"API_KEY"	, _TL("API Key"),
		_TL(""),
		""
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGeoCoding::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "ADDRESSES") )
	{
		pParameters->Set_Enabled("FIELD"  , pParameter->asTable() != NULL);
		pParameters->Set_Enabled("ADDRESS", pParameter->asTable() == NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "PROVIDER") )
	{
		pParameters->Set_Enabled("API_KEY", pParameter->asInt() != 0 );
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::On_Execute(void)
{
	m_pLocations	= Parameters("LOCATIONS")->asShapes();

	m_pLocations->Create(SHAPE_TYPE_Point, _TL("Locations"));
	m_pLocations->Get_Projection().Set_GCS_WGS84();

	m_pLocations->Add_Field("ID"     , SG_DATATYPE_String);
	m_pLocations->Add_Field("ADDRESS", SG_DATATYPE_String);

	int	Provider	= Parameters("PROVIDER")->asInt();

	m_API_Key	= Parameters("API_KEY")->asString();

	//-----------------------------------------------------
	CSG_Table	Table, *pTable	= Parameters("ADDRESSES")->asTable();

	int	Field	= Parameters("FIELD")->asInt();

	if( !pTable )
	{
		pTable	= &Table;
		pTable->Add_Field("ADDRESS", SG_DATATYPE_String);
		pTable->Add_Record()->Set_Value(0, Parameters("ADDRESS")->asString());

		Field	= 0;
	}

	//-----------------------------------------------------
	CSG_HTTP	HTTP;

	switch( Provider )
	{
	default: HTTP.Create("www.datasciencetoolkit.org" ); break;	// The Data Science Toolkit
	case  1: HTTP.Create("maps.googleapis.com"        ); break;	// Google
	case  2: HTTP.Create("dev.virtualearth.net"       ); break;	// Bing
	case  3: HTTP.Create("www.mapquestapi.com"        ); break;	// MapQuest
	case  4: HTTP.Create("local.yahooapis.com"        ); break;	// Yahoo
	case  5: HTTP.Create("nominatim.openstreetmap.org"); break;	// Nominatim
	}

	if( !HTTP.is_Connected() )
	{
		Error_Set(_TL("failed to connect to server."));

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pTable->Get_Count() && Process_Get_Okay(); i++)
	{
		TSG_Point	Location;
		CSG_String	Address(pTable->Get_Record(i)->asString(Field));

		bool	bOkay;

		switch( Provider )
		{
		default: bOkay = Request_DSTK     (HTTP, Location, Address); break;	// The Data Science Toolkit
		case  1: bOkay = Request_Google   (HTTP, Location, Address); break;	// Google
		case  2: bOkay = Request_Bing     (HTTP, Location, Address); break;	// Bing
		case  3: bOkay = Request_MapQuest (HTTP, Location, Address); break;	// MapQuest
		case  4: bOkay = Request_Yahoo    (HTTP, Location, Address); break;	// Yahoo
		case  5: bOkay = Request_Nominatim(HTTP, Location, Address); break;	// Nominatim
		}

		if( bOkay )
		{
			CSG_Shape	*pLocation	= m_pLocations->Add_Shape();

			pLocation->Add_Point(Location);

			pLocation->Set_Value(0, Address);
		}
	}

	//-----------------------------------------------------
	return( m_pLocations->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void	Replace_Special_Chars(CSG_String &String)
{
	String.Replace(" ", "+" );
	String.Replace("ä", "ae");
	String.Replace("ö", "oe");
	String.Replace("ü", "ue");
	String.Replace("Ä", "Ae");
	String.Replace("Ö", "Oe");
	String.Replace("Ü", "Ue");
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_DSTK(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address)
{
	//-----------------------------------------------------
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	Request	= "maps/api/geocode/json?address=" + Request;

	//-----------------------------------------------------
	CSG_String	_Answer;

	if( !Server.Request(Request, _Answer) )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Request failed."), Request.c_str()), false);

		return( false );
	}

	CSG_MetaData	Answer;	Answer.Load_JSON(_Answer);

	Message_Add("\n\n" + _Answer + "\n", false);
//	Message_Add("\n\n" + Answer.asText(1) + "\n", false);

	//-----------------------------------------------------
	if( !Answer.Cmp_Name("root") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Warning"), Answer.Get_Name().c_str()), false);
	}

	if( !Answer("status") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Warning"), SG_T("status")));
	}
	else if( !Answer["status"].Cmp_Content("OK") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Error"), Answer.Get_Name().c_str()), false);

		if( Answer("error_message") )
		{
			Message_Add("\n" + Answer["error_message"].Get_Content(), false);
		}

		return( false );
	}

	if( !Answer("results") || !Answer["results"](0) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("results")));

		return( false );
	}

	const CSG_MetaData	&Result	= Answer["results"][0];

	if( !Result("geometry") || !Result["geometry"]("location")
	||  !Result["geometry"]["location"]("lat")
	||  !Result["geometry"]["location"]("lng") )
	{
		return( false );
	}

	Location.x	= Result["geometry"]["location"]["lng"].Get_Content().asDouble();
	Location.y	= Result["geometry"]["location"]["lat"].Get_Content().asDouble();

	if( Result("formatted_address") )
	{
		Address	= Result["formatted_address"].Get_Content();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Google(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address)
{
	//-----------------------------------------------------
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	Request	= "/maps/api/geocode/xml?address=" + Request;

	if( !m_API_Key.is_Empty() )
	{
		Request	+= "&key=" + m_API_Key;
	}

	//-----------------------------------------------------
	CSG_MetaData	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Request failed."), Request.c_str()), false);

		return( false );
	}

	//-----------------------------------------------------
	if( Answer.Get_Name().CmpNoCase("GeocodeResponse") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Warning"), Answer.Get_Name().c_str()), false);
	}

	if( !Answer("status") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Warning"), SG_T("status")));
	}
	else if( !Answer["status"].Cmp_Content("OK") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Error"), Answer.Get_Name().c_str()), false);

		if( Answer("error_message") )
		{
			Message_Add("\n" + Answer["error_message"].Get_Content(), false);
		}

		return( false );
	}

	if( !Answer("result") )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("result")));

		return( false );
	}

	const CSG_MetaData	&Result	= Answer["result"];

	if( !Result("geometry") || !Result["geometry"]("location")
	||  !Result["geometry"]["location"]("lat")
	||  !Result["geometry"]["location"]("lng") )
	{
		return( false );
	}

	Location.x	= Result["geometry"]["location"]["lng"].Get_Content().asDouble();
	Location.y	= Result["geometry"]["location"]["lat"].Get_Content().asDouble();

	if( Result("formatted_address") )
	{
		Address	= Result["formatted_address"].Get_Content();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Bing(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address)
{
	//-----------------------------------------------------
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	Request	= "REST/v1/Locations?=&location=maxResults=1&countryRegion&adminDistrict=adminDistrict&locality=locality&postalCode=postalCode&addressLine=addressLine&userLocation=userLocation&userIp=userIp&usermapView=usermapView&includeNeighborhood=includeNeighborhood" + Address;

	// dev.virtualearth.net/REST/v1/Locations?
	// countryRegion=countryRegion
	// adminDistrict=adminDistrict
	// locality=locality
	// postalCode=postalCode
	// addressLine=addressLine
	// userLocation=userLocation
	// userIp=userIp
	// usermapView=usermapView
	// includeNeighborhood=includeNeighborhood
	// maxResults=maxResults
	// key=BingMapsKey

	if( !m_API_Key.is_Empty() )
	{
		Request	+= "&key=" + m_API_Key;
	}

	CSG_MetaData	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Request failed."), Request.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	if( Answer.Get_Name().CmpNoCase("Response") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Warning"), Answer.Get_Name().c_str()), false);
	}

	if( !Answer("StatusDescription") || !Answer["StatusDescription"].Cmp_Content("OK") )
	{
		Message_Add(CSG_String::Format("\n%s [%s]", _TL("Error"), Answer.Get_Name().c_str()), false);

		if( Answer("ErrorDetails") )
		{
			Message_Add("\n" + Answer["ErrorDetails"].Get_Content(), false);
		}
	}

	if( !Answer("ResourceSets")
	||  !Answer["ResourceSets"]("ResourceSet")
	||  !Answer["ResourceSets"]["ResourceSet"]("Resources")
	||  !Answer["ResourceSets"]["ResourceSet"]["Resources"]("Location")
	||  !Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]("Point") )
	{
		return( false );
	}

	const CSG_MetaData	&Point	= Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]["Point"];

	if( !Point("Longitude") || !Point["Longitude"].Get_Content().asDouble(Location.x)
	||  !Point("Latitude" ) || !Point["Latitude" ].Get_Content().asDouble(Location.y) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("location")));

		return( false );
	}

	if( !Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]("Name") )
	{
		Address	= Answer["ResourceSets"]["ResourceSet"]["Resources"]["Location"]["Name"].Get_Content();
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_MapQuest(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address)
{
	//-----------------------------------------------------
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	CSG_String	APIKey	= "KEY";

	Request	= "geocoding/v1/address?key=" + APIKey + "&location=" + Address;
//www.mapquestapi.com/geocoding/v1/address?key=KEY&location=1600+Pennsylvania+Ave+NW,Washington,DC,20500

	CSG_MetaData	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Request failed."), Request.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	if( Answer.Get_Name().CmpNoCase("ResultSet") )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Warning"), Answer.Get_Name().c_str()));
	}

	if( !Answer("Longitude") || !Answer["Longitude"].Get_Content().asDouble(Location.x)
	||  !Answer("Latitude" ) || !Answer["Latitude" ].Get_Content().asDouble(Location.y) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("location")));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Yahoo(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address)
{
	//-----------------------------------------------------
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	CSG_String	APIKey("YahooDemo");

	Request	= "MapsService/V1/geocode?appid=" + APIKey + "&location=" + Address;
//	MapsService/V1/geocode?appid=&street=701+First+Ave&city=Sunnyvale&state=CA
//	Request	= "geocode?line2=" + Request + "&flags=J&appid=yourappid"

	CSG_MetaData	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Request failed."), Request.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	if( Answer.Get_Name().CmpNoCase("ResultSet") )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Warning"), Answer.Get_Name().c_str()));
	}

	if( !Answer("Longitude") || !Answer["Longitude"].Get_Content().asDouble(Location.x)
	||  !Answer("Latitude" ) || !Answer["Latitude" ].Get_Content().asDouble(Location.y) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("location")));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoCoding::Request_Nominatim(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address)
{
	//-----------------------------------------------------
	CSG_String	Request(Address);

	Replace_Special_Chars(Request);

	Request	= "search?q=" + Request + "&format=xml&polygon=1&addressdetails=1";

	CSG_MetaData	Answer;

	if( !Server.Request(Request, Answer) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Request failed."), Request.c_str()));

		return( false );
	}

	//-----------------------------------------------------
	if( Answer.Get_Name().CmpNoCase("searchresults") )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Warning"), Answer.Get_Name().c_str()));
	}

	if( !Answer("place") )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("place")));

		return( false );
	}

	const CSG_MetaData	&Place	= Answer["place"];

	if( !Place.Get_Property("lon", Location.x)
	||  !Place.Get_Property("lat", Location.y) )
	{
		Message_Add(CSG_String::Format("%s [%s]", _TL("Error"), SG_T("location")));

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
