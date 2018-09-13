
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
//                     sg_curl.cpp                       //
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
#include "sg_curl.h"

//---------------------------------------------------------
#ifdef SG_USE_CURL

#include <curl.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_CURL_Global
{
public:
	CSG_CURL_Global(void)
	{
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	virtual ~CSG_CURL_Global(void)
	{
		curl_global_cleanup();
	}
};

//---------------------------------------------------------
static CSG_CURL_Global	SG_CURL_Global;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CURL_SET_OPT(Error, Key, Value)	{ CURLcode Result = curl_easy_setopt(m_pCURL, Key, Value);\
	if( Result != CURLE_OK ) { m_Error = curl_easy_strerror(Result);\
	if( Error != 0 ) { if( Error == 2 ) { Destroy(); } return( false ); } } }


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_CURL::CSG_CURL(void)
{
	m_pCURL	= NULL;
}

//---------------------------------------------------------
bool CSG_CURL::Create(void)
{
	return( Destroy() );
}

//---------------------------------------------------------
CSG_CURL::CSG_CURL(const CSG_String &Server, const SG_Char *Username, const SG_Char *Password)
{
	m_pCURL	= NULL;

	Create(Server, Username, Password);
}

//---------------------------------------------------------
bool CSG_CURL::Create(const CSG_String &Server, const SG_Char *Username, const SG_Char *Password)
{
	Destroy();

	if( (m_pCURL = curl_easy_init()) == NULL )
	{
		return( false );
	}

	m_Server	= Server;

	if( m_Server.Find("://") < 0 )
	{
		m_Server.Prepend("http://");
	}

	if( Username && *Username )	{ CSG_String s(Username); curl_easy_setopt(m_pCURL, CURLOPT_USERNAME, s.b_str()); }
	if( Password && *Password )	{ CSG_String s(Password); curl_easy_setopt(m_pCURL, CURLOPT_PASSWORD, s.b_str()); }

	CURL_SET_OPT(2, CURLOPT_URL           , m_Server.b_str());
	CURL_SET_OPT(2, CURLOPT_WRITEFUNCTION , _Callback_Write_String);
	CURL_SET_OPT(2, CURLOPT_WRITEDATA     , &m_Error);

	CURL_SET_OPT(0, CURLOPT_SSL_VERIFYPEER, 0L);
	CURL_SET_OPT(0, CURLOPT_SSL_VERIFYHOST, 0L);
	CURL_SET_OPT(0, CURLOPT_USERAGENT     , "libcurl-agent/1.0");

	m_Error.Clear();

	if( !_Perform() )
	{
		Destroy();

		return( false );
	}

	return( true );
}

//---------------------------------------------------------
CSG_CURL::~CSG_CURL(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_CURL::Destroy(void)
{
	if( m_pCURL )
	{
		curl_easy_cleanup(m_pCURL);

		m_pCURL	= NULL;
	}

	m_Server.Clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CURL::is_Connected(void)	const
{
	return( m_pCURL != NULL );
}

//---------------------------------------------------------
bool CSG_CURL::_Perform(void)
{
	CURLcode	Result	= curl_easy_perform(m_pCURL);

	if( Result == CURLE_OK )
	{
		return( true );
	}

	m_Error	= curl_easy_strerror(Result);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSG_CURL::_Callback_Write_Bytes(char *Bytes, size_t Size, size_t nBytes, void *pBuffer)
{
	if( !Bytes || !pBuffer )	{	return( 0 );	}	nBytes	*= Size;

	((CSG_Bytes *)pBuffer)->Add(Bytes, (int)nBytes, false);

	return( nBytes );
}

//---------------------------------------------------------
size_t CSG_CURL::_Callback_Write_String(char *Bytes, size_t Size, size_t nBytes, void *pBuffer)
{
	if( !Bytes || !pBuffer )	{	return( 0 );	}	nBytes	*= Size;

	CSG_String	&s	= *((CSG_String *)pBuffer);

	for(size_t i=0; i<nBytes; i++)
	{
		s	+= Bytes[i];
	}

	return( nBytes );
}

//---------------------------------------------------------
size_t CSG_CURL::_Callback_Write_File(char *Bytes, size_t Size, size_t nBytes, void *pBuffer)
{
	if( !Bytes || !pBuffer )	{	return( 0 );	}

	return( ((CSG_File *)pBuffer)->Write(Bytes, Size, nBytes) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_CURL::Request(const CSG_String &Request, CSG_MetaData &Answer)
{
	CSG_String	_Answer;

	return( CSG_CURL::Request(Request, _Answer) && Answer.from_XML(_Answer) );
}

//---------------------------------------------------------
bool CSG_CURL::Request(const CSG_String &Request, CSG_Bytes &Answer)
{
	if( !is_Connected() ) { return( false ); }

	Answer.Clear();

	CSG_String	URL	= m_Server + "/" + Request;

	CURL_SET_OPT(1, CURLOPT_URL           , URL.b_str());
	CURL_SET_OPT(1, CURLOPT_FOLLOWLOCATION, 1L);
	CURL_SET_OPT(1, CURLOPT_WRITEFUNCTION , _Callback_Write_Bytes);
	CURL_SET_OPT(1, CURLOPT_WRITEDATA     , &Answer);

	return( _Perform() );
}

//---------------------------------------------------------
bool CSG_CURL::Request(const CSG_String &Request, CSG_String &Answer)
{
	if( !is_Connected() ) { return( false ); }

	Answer.Clear();

	CSG_String	URL	= m_Server + "/" + Request;

	CURL_SET_OPT(1, CURLOPT_URL           , URL.b_str());
	CURL_SET_OPT(1, CURLOPT_FOLLOWLOCATION, 1L);
	CURL_SET_OPT(1, CURLOPT_WRITEFUNCTION , _Callback_Write_String);
	CURL_SET_OPT(1, CURLOPT_WRITEDATA     , &Answer);

	return( _Perform() );
}

//---------------------------------------------------------
bool CSG_CURL::Request(const CSG_String &Request, const SG_Char *File)
{
	if( !is_Connected() ) { return( false ); }

	CSG_File	Answer;

	if( !Answer.Open(File, SG_FILE_W) )
	{
		m_Error.Printf("%s [%s]", _TL("Failed to open file"), File);

		return( false );
	}

	CSG_String	URL	= m_Server + "/" + Request;

	CURL_SET_OPT(1, CURLOPT_URL           , URL.b_str());
	CURL_SET_OPT(1, CURLOPT_FOLLOWLOCATION, 1L);
	CURL_SET_OPT(1, CURLOPT_WRITEFUNCTION , _Callback_Write_File);
	CURL_SET_OPT(1, CURLOPT_WRITEDATA     , &Answer);

	return( _Perform() );
}

//---------------------------------------------------------
#endif // #ifndef SG_USE_CURL


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
