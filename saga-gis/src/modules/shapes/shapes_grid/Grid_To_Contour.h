/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_To_Contour.h                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#ifndef HEADER_INCLUDED__Grid_To_Contour_H
#define HEADER_INCLUDED__Grid_To_Contour_H



///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_To_Contour : public CSG_Module
{
public:
	CGrid_To_Contour(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Vectorization") );	}


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bParts;

	CSG_Grid				*m_pGrid, m_Edge;

	CSG_Shapes				*m_pContours, *m_pPolygons;


	bool					is_Edge					(int x, int y);

	void					Set_Row					(int x, int y, bool bOn);
	bool					Get_Row					(int x, int y);
	void					Set_Col					(int x, int y, bool bOn);
	bool					Get_Col					(int x, int y);

	bool					Get_Contour				(double z);
	bool					Get_Contour				(double z, int x, int y);
	bool					Get_Contour_Cell		(int &Dir, int &x, int &y, bool &bRow);

	bool					Get_Polygons			(CSG_Shape_Line *pContour_Lo, CSG_Shape_Line *pContour_Hi);
	bool					Add_Contour				(CSG_Shapes &Segments, CSG_Shape_Polygon *pPolygon, CSG_Shape_Line *pContour);
	bool					Add_Edge				(CSG_Shapes &Segments, int x, int y);
	bool					Add_Segment				(CSG_Shape *pPolygon, int iPart, CSG_Shape *pContour, bool bAscending);
	CSG_Shape *				Get_Segment				(const CSG_Shapes &Segments, int &x, int &y, bool &bAscending);

	bool					Split_Polygon_Parts		(CSG_Shapes *pPolygons);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_To_Contour_H
