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
class CShapes_Buffer : public CSG_Tool
{
public:
	CShapes_Buffer(void);


protected:

	virtual bool				On_Execute				(void);

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	bool						m_bPolyInner;

	double						m_dArc;


	bool						Get_Buffers				(CSG_Shapes *pShapes, int Field, CSG_Shapes *pBuffers, double Scale, bool bDissolve);

	bool						Get_Buffer				(CSG_Shape *pShape  , CSG_Shape *pBuffer, double Distance);
	bool						Get_Buffer_Point		(CSG_Shape *pPoint  , CSG_Shape *pBuffer, double Distance);
	bool						Get_Buffer_Points		(CSG_Shape *pPoints , CSG_Shape *pBuffer, double Distance);
	bool						Get_Buffer_Line			(CSG_Shape *pLine   , CSG_Shape *pBuffer, double Distance);
	bool						Get_Buffer_Polygon		(CSG_Shape *pPolygon, CSG_Shape *pBuffer, double Distance);

	double						Get_Direction			(const TSG_Point &From, const TSG_Point &To);
	bool						Get_Parallel			(const TSG_Point &A, const TSG_Point &B, TSG_Point AB[2], double Distance);

	void						Add_Vertex				(CSG_Shape *pBuffer, const TSG_Point &Center, double Distance, double theta);
	void						Add_Arc					(CSG_Shape *pBuffer, const TSG_Point &Center, double Distance, double alpha, double beta);
	void						Add_Arc					(CSG_Shape *pBuffer, const TSG_Point &Center, double Distance, const TSG_Point &A, const TSG_Point &B);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Shapes_Buffer_H
