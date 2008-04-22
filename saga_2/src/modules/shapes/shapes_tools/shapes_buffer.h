
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Shapes_Buffer.h                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Shapes_Buffer_H
#define HEADER_INCLUDED__Shapes_Buffer_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CShapes_Buffer : public CSG_Module
{
public:
	CShapes_Buffer(void);
	virtual ~CShapes_Buffer(void);


protected:

	virtual bool				On_Execute				(void);

	bool						Initialise				(void);
	bool						Finalise				(void);

	bool						Get_Buffers				(CSG_Shapes *pBuffers, double dZone);


private:

	int							m_ID, m_Type, m_Field;

	double						m_Distance, m_dArc, m_Scale;

	CSG_Shape					*m_pSegment, *m_pUnion, *m_pBuffer;

	CSG_Shapes					*m_pShapes, m_Tmp;


	bool						Get_Buffer_Point		(CSG_Shape *pPoint);
	bool						Get_Buffer_Points		(CSG_Shape *pPoints);
	bool						Get_Buffer_Line			(CSG_Shape *pLine);
	bool						Get_Buffer_Polygon		(CSG_Shape *pPolygon);

	void						Add_Vertex				(const TSG_Point &Center, double theta);
	void						Add_Arc					(const TSG_Point &Center, double alpha, double beta);
	void						Add_Arc					(const TSG_Point &Center, const TSG_Point &A, const TSG_Point &B);
	bool						Get_Parallel			(const TSG_Point &A, const TSG_Point &B, TSG_Point AB[2]);
	void						Del_Duplicates			(CSG_Shape *pShape);

	void						Add_Line				(CSG_Shape_Line    *pShape, int iPart);
	void						Add_Polygon				(CSG_Shape_Polygon *pShape, int iPart);

	void						Add_Buffer				(void);

	void						Get_SelfIntersection	(void);
	void						Get_SelfIntersection	(CSG_Shape_Polygon *pSegment);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Shapes_Buffer_H
