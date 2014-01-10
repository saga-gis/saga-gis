/**********************************************************
 * Version $Id: stl.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__stl_H
#define HEADER_INCLUDED__stl_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSTL_Import : public CSG_Module
{
public:
	CSTL_Import(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("R:Import") );	}


protected:

	virtual bool			On_Execute			(void);


private:

	typedef struct
	{
		float				x, y, z;
	}
	TSTL_Point;


	double					r_sin_x, r_cos_x, r_sin_y, r_cos_y, r_sin_z, r_cos_z;

	CSG_Grid				*m_pGrid;


	bool					Read_Facette		(CSG_File &Stream, TSTL_Point p[3]);
	void					Rotate				(TSTL_Point &p);

	bool					Get_Extent			(CSG_File &Stream, CSG_Rect &Extent, int nFacettes);

	void					Set_Triangle		(TSG_Point_Z p[3]);
	void					Set_Triangle_Line	(int y, double xa, double za, double xb, double zb);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSTL_Export : public CSG_Module
{
public:
	CSTL_Export(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("R:Export") );	}


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
