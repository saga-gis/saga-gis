
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  vertex_inspector.h                   //
//                                                       //
//                 Copyright (C) 2024 by                 //
//                  Justus Spitzmueller                  //
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
//    e-mail:     jspitzmueller@scilands.de              //
//                                                       //
//    contact:    Justus Spitzmueller                    //
//                scilands GmbH                          //
//                Goethe-Allee 11                        //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#pragma once

//---------------------------------------------------------
#include "saga_api/shapes.h"
#include <saga_api/saga_api.h>
#include <vector>
#include <map>


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVertexInspector : public CSG_Tool_Interactive
{
public: 

	CVertexInspector(void);							


protected: 

	virtual bool		On_Execute(void);
	virtual bool		On_Execute_Finish(void);
	virtual int 		On_Parameter_Changed( CSG_Parameters *pParameters, CSG_Parameter *pParameter );
	virtual bool 		On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);

	bool 				Select_from_Drag_Box( CSG_Rect Drag_Bog );
	bool 				Handle_Mutable_Point();

private: 

	struct Index
	{
		CSG_Shapes 	*shapes = NULL;
		CSG_Shape 	*shape = NULL;
		sLong 		part = 0;
		sLong 		point = 0;

	}; 

	bool 				m_bDown = false;
	bool 				m_bConfirm = false;
	TSG_Point 			m_ptDown = {};

	TSG_Point_4D 		m_ptOld = {0.0,0.0,0.0,0.0};
	TSG_Point_4D 		m_ptNew = {0.0,0.0,0.0,0.0};
	Index m_LastPoint;

	CSG_Shapes 			*m_pIndicator = NULL;

	CSG_Table 			*m_pTable = NULL;

	CSG_Parameter_Shapes_List *m_pList;

	std::map<sLong, std::map<sLong, std::map<sLong, std::vector<Index>>>> m_Map;
	
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////


