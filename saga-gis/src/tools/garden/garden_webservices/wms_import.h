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
//                      wms_import.h                     //
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
#ifndef HEADER_INCLUDED__WMS_Import_H
#define HEADER_INCLUDED__WMS_Import_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWMS_Capabilities
{
public:
	CWMS_Capabilities(void);
	CWMS_Capabilities(const CSG_String &Server, const CSG_String &Version);

	virtual ~CWMS_Capabilities(void);

	bool				Create				(const CSG_String &Server, const CSG_String &Version);
	void				Destroy				(void);


	TSG_Rect			m_Extent;

	int					m_MaxWidth, m_MaxHeight, m_LayerLimit;

	CSG_String			m_Name, m_Version, m_Title, m_Abstract, m_Formats, m_Projections;

	CSG_Strings			m_Layers_Name, m_Layers_Title;


};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWMS_Import : public CSG_Tool
{
public:
	CWMS_Import(void);


protected:

	virtual int			On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int			On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute				(void);


private:

	bool				Do_Dialog				(CWMS_Capabilities &Cap);

	bool				Get_Map					(class wxHTTP *pServer, const CSG_String &Version, CWMS_Capabilities &Cap);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__WMS_Import_H
