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
//                      osm_import.h                     //
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
#ifndef HEADER_INCLUDED__OSM_Import_H
#define HEADER_INCLUDED__OSM_Import_H


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
class COSM_Import : public CSG_Tool_Interactive
{
public:
	COSM_Import(void);


protected:

	virtual bool		On_Execute			(void);
	virtual bool		On_Execute_Finish	(void);
	virtual bool		On_Execute_Position	(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);


private:

	bool				m_bDown;

	CSG_Point			m_ptDown;

	CSG_Table			m_Nodes;

	CSG_Shapes			*m_pPoints, *m_pWays, *m_pAreas;


	bool				Load_Nodes			(class wxXmlNode *pRoot);
	bool				Find_Node			(long id, double &lon, double &lat);
	CSG_Table_Record *	Find_Node			(long id);

	bool				Load_Ways			(class wxXmlNode *pRoot);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__OSM_Import_H
