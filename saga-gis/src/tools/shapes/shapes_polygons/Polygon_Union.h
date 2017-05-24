/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    shapes_polygons                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Polygon_Union.h                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__Polygon_Union_H
#define HEADER_INCLUDED__Polygon_Union_H


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
class CPolygon_Dissolve : public CSG_Tool  
{
public:
	CPolygon_Dissolve(void);


protected:

	virtual bool				On_Execute				(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	bool						m_bSUM, m_bAVG, m_bMIN, m_bMAX, m_bRNG, m_bDEV, m_bVAR, m_bLST, m_bNUM;

	int							m_Stat_Offset;

	CSG_Strings					m_List;

	CSG_Parameter_Table_Fields	*m_Stat_pFields;

	CSG_Simple_Statistics		*m_Statistics;


	bool						Get_Dissolved			(CSG_Shape *pDissolve, bool bDissolve, double nimArea);

	bool						Statistics_Initialize	(CSG_Shapes *pDissolved, CSG_Shapes *pPolygons);
	CSG_String					Statistics_Get_Name		(const CSG_String &Type, const CSG_String &Name);
	bool						Statistics_Add			(CSG_Shape *pDissolve, CSG_Shape *pPolygon, bool bReset);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Polygon_Union_H
