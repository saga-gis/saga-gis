/**********************************************************
 * Version $Id: polygon_shared_edges.h 911 2011-02-14 16:38:15Z reklov_w $
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
//                 polygon_shared_edges.h                //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__polygon_shared_edges_H
#define HEADER_INCLUDED__polygon_shared_edges_H


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPolygon_Shared_Edges : public CSG_Tool
{
public: ////// public members and functions: //////////////

	CPolygon_Shared_Edges(void);

//	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("Landsat") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute				(void);


private: ///// private members and functions: /////////////

	bool				m_bDouble;

	int					m_Field;

	CSG_Shapes			*m_pEdges, *m_pNodes;


	bool				Get_Shared_Edges		(CSG_Shape_Part *pA, CSG_Shape_Part *pB, double Epsilon);

	int					Get_Next_Vertex			(CSG_Shape_Part *pPoints, int iPoint, bool bAscending);

	int					Check_Vertices			(CSG_Shape_Part *pPolygon, CSG_Shape_Part *pVertices, double Epsilon);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPolygon_Vertex_Check : public CSG_Tool
{
public: ////// public members and functions: //////////////

	CPolygon_Vertex_Check(void);

//	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("Landsat") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute				(void);


private: ///// private members and functions: /////////////

	CSG_Shapes			*m_pAdded;


	bool				Check_Vertices			(CSG_Shape_Part *pPolygon, CSG_Shape_Part *pVertices, double Epsilon);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__polygon_shared_edges_H
