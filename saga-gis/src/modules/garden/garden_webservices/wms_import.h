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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
class CWMS_Layer
{
public:
	CWMS_Layer(void)			{}
	virtual ~CWMS_Layer(void)	{}


	TSG_Rect			m_GeoBBox;

	CSG_String			m_Name, m_Title;

};

//---------------------------------------------------------
class CWMS_Capabilities
{
public:
	CWMS_Capabilities(void);
	CWMS_Capabilities(class wxHTTP *pServer, const CSG_String &Directory, CSG_String &Version);

	virtual ~CWMS_Capabilities(void);


	bool				Create				(class wxHTTP *pServer, const CSG_String &Directory, CSG_String &Version);

	CSG_String			Get_Summary			(void);


	int					m_MaxLayers, m_MaxWidth, m_MaxHeight, m_nLayers;

	TSG_Rect			m_GeoBBox;

	CSG_String			m_Name, m_Title, m_Abstract, m_Online, m_Contact, m_Fees, m_Access, m_Formats, m_Layers_Title, m_Projections;

	CSG_Strings			m_Keywords, m_sProjections;

	CWMS_Layer			**m_pLayers;


private:

	void				_Reset				(void);

	class wxXmlNode *	_Get_Child			(class wxXmlNode *pNode, const CSG_String &Name);
	bool				_Get_Child_Content	(class wxXmlNode *pNode, CSG_String &Value, const CSG_String &Name);
	bool				_Get_Child_Content	(class wxXmlNode *pNode, int        &Value, const CSG_String &Name);
	bool				_Get_Child_Content	(class wxXmlNode *pNode, double     &Value, const CSG_String &Name);
	bool				_Get_Node_PropVal	(class wxXmlNode *pNode, CSG_String &Value, const CSG_String &Property);
	bool				_Get_Child_PropVal	(class wxXmlNode *pNode, CSG_String &Value, const CSG_String &Name, const CSG_String &Property);

	bool				_Get_Capabilities	(class wxXmlNode *pRoot, CSG_String &Version);
	bool				_Get_Layer			(class wxXmlNode *pNode);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWMS_Import : public CSG_Module
{
public:
	CWMS_Import(void);
	virtual ~CWMS_Import(void);


protected:

	virtual bool		On_Execute			(void);


private:

	bool				Do_Dialog			(CWMS_Capabilities &Cap);

	bool				Get_Map				(class wxHTTP *pServer, const CSG_String &Directory, const CSG_String &Version, CWMS_Capabilities &Cap);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__WMS_Import_H
