
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#ifndef HEADER_INCLUDED__Grid_To_Contour_H
#define HEADER_INCLUDED__Grid_To_Contour_H



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
class CGrid_To_Contour : public CSG_Tool
{
public:
	CGrid_To_Contour(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Vectorization") );	}


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	CSG_Grid				*m_pGrid, m_Edge;


	bool					is_Edge					(int x, int y);

	void					Set_Row					(int x, int y, bool bOn);
	bool					Get_Row					(int x, int y);
	void					Set_Col					(int x, int y, bool bOn);
	bool					Get_Col					(int x, int y);

	bool					Get_Contour				(CSG_Shapes     *pContours, double z, double minLength);
	bool					Get_Contour				(CSG_Shape_Line *pContour , double z, int x, int y);
	bool					Get_Contour_Cell		(int &Dir, int &x, int &y, bool &bRow);

	bool					Get_Edge_Segments		(CSG_Shapes &Edges, CSG_Shapes *pContours);
	bool					Add_Edge_Segment		(CSG_Shape &Edge, int x, int y);
	bool					Add_Edge_Point			(CSG_Shapes &Edges, const CSG_Point &Point, sLong Line, int Part);

	bool					Get_Polygons			(CSG_Shapes *pPolygons, CSG_Shapes &Edges, CSG_Shape_Line *pContour_Lo, CSG_Shape_Line *pContour_Hi);
	bool					Add_Polygon_Segment		(CSG_Array_Pointer &Segments, CSG_Shape_Part *pPolygon);

	bool					Split_Line_Parts		(CSG_Shapes *pLines);
	bool					Split_Polygon_Parts		(CSG_Shapes *pPolygons);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_To_Contour_H
