
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
//                     geocoding.h                       //
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
#ifndef HEADER_INCLUDED__geocoding_H
#define HEADER_INCLUDED__geocoding_H


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
class CGeoCoding : public CSG_Tool
{
public:
	CGeoCoding(void);

//	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("") );	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	CSG_String				m_API_Key;

	CSG_Shapes				*m_pLocations;


	bool					Request_DSTK			(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address);
	bool					Request_Google			(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address);
	bool					Request_Bing			(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address);
	bool					Request_MapQuest		(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address);
	bool					Request_Yahoo			(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address);
	bool					Request_Nominatim		(CSG_HTTP &Server, TSG_Point &Location, CSG_String &Address);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__geocoding_H
