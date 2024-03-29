
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                         stl.h                         //
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
#ifndef HEADER_INCLUDED__stl_H
#define HEADER_INCLUDED__stl_H


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
class CSTL_Import : public CSG_Tool
{
public:
	CSTL_Import(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("Import") );	}


protected:

	virtual int				On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute			(void);


private:

	bool					m_bRotate;

	TSG_Point_3D			m_sin, m_cos;

	CSG_Grid				*m_pGrid;


	bool					Read_Facette		(CSG_File &Stream, TSG_Point_3D Point[4], WORD &Attribute);
	void					Rotate				(TSG_Point_3D &Point);

	bool					Get_Extent			(CSG_File &Stream, CSG_Rect &Extent, int nFacettes);

	void					Set_Triangle		(TSG_Point_3D p[3]);
	void					Set_Triangle_Line	(int y, double xa, double za, double xb, double zb);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSTL_Export : public CSG_Tool
{
public:
	CSTL_Export(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("Export") );	}


protected:

	virtual bool			On_Execute			(void);


private:

	bool					Get_Normal			(CSG_TIN_Triangle *pTriangle, int zField, float Normal[3]);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__stl_H
