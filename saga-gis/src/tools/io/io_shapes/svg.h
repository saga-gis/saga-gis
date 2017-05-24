/**********************************************************
 * Version $Id: svg.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                         svg.h                         //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#ifndef HEADER_INCLUDED__svg_H
#define HEADER_INCLUDED__svg_H


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
class CSVG_Export : public CSG_Tool
{
public:
	CSVG_Export(void);

	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("Export") );	}


protected:

	virtual bool			On_Execute		(void);


private:

	double					m_dStroke;


	bool					Get_Points		(CSG_Shape *pShape, int iPart, CSG_String &Points);

	void					Add_Points		(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color, double Size, int Symbol);
	void					Add_Line		(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color, double Size);
	void					Add_Polygon		(CSG_MetaData &SVG, CSG_Shape *pShape, int iPart, long Color);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSVG_Import : public CSG_Tool
{
public:
	CSVG_Import(void);

	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("Import") );	}


protected:

	virtual bool			On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__svg_H
