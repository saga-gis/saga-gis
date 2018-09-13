
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
//                      sg_curl.h                        //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__sg_curl_H
#define HEADER_INCLUDED__sg_curl_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_USE_CURL

#ifndef SG_USE_CURL
	#define CWebClient	CSG_HTTP
#else
	#define CWebClient	CSG_CURL

//---------------------------------------------------------
class CSG_CURL
{
public:
								CSG_CURL		(void);
	bool						Create			(void);

								CSG_CURL		(const CSG_String &Server, const SG_Char *Username = NULL, const SG_Char *Password = NULL);
	bool						Create			(const CSG_String &Server, const SG_Char *Username = NULL, const SG_Char *Password = NULL);

	virtual						~CSG_CURL		(void);
	bool						Destroy			(void);

	bool						is_Connected	(void)	const;

	const CSG_String &			Get_Error		(void)	const	{	return( m_Error );	}

	bool						Request			(const CSG_String &Request, CSG_Bytes    &Answer);
	bool						Request			(const CSG_String &Request, CSG_MetaData &Answer);
	bool						Request			(const CSG_String &Request, CSG_String   &Answer);
	bool						Request			(const CSG_String &Request, const SG_Char *File);


private:

	CSG_String					m_Server, m_Error;

	void						*m_pCURL;


	bool						_Perform		(void);

	static size_t				_Callback_Write_Bytes	(char *Bytes, size_t Size, size_t nBytes, void *pBuffer);
	static size_t				_Callback_Write_String	(char *Bytes, size_t Size, size_t nBytes, void *pBuffer);
	static size_t				_Callback_Write_File	(char *Bytes, size_t Size, size_t nBytes, void *pBuffer);

};

//---------------------------------------------------------
#endif // #ifdef SG_USE_CURL


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__sg_curl_H
