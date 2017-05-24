/**********************************************************
 * Version $Id: wrf.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                         wrf.h                         //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__wrf_Import_H
#define HEADER_INCLUDED__wrf_Import_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWRF_Index
{
public:
	CWRF_Index(void);

	bool					Reset					(void);

	bool					Load					(const CSG_String &File);
	bool					Save					(const CSG_String &File);

	
	bool					m_SIGNED, m_ENDIAN;

	int						m_TYPE, m_WORDSIZE, m_TILE_X, m_TILE_Y, m_TILE_Z, m_TILE_Z_START, m_TILE_Z_END, m_CATEGORY_MIN, m_CATEGORY_MAX, m_TILE_BDR, m_ROW_ORDER;

	int						m_ISWATER, m_ISLAKE, m_ISICE, m_ISURBAN, m_ISOILWATER;

	double					m_DX, m_DY, m_KNOWN_X, m_KNOWN_Y, m_KNOWN_LAT, m_KNOWN_LON, m_STDLON, m_TRUELAT1, m_TRUELAT2, m_MISSING_VALUE, m_SCALE_FACTOR;

	CSG_String				m_PROJECTION, m_UNITS, m_DESCRIPTION, m_MMINLU;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWRF_Import : public CSG_Tool
{
public:
	CWRF_Import(void);

	virtual CSG_String		Get_MenuPath			(void)		{	return( _TL("Import") );	}


protected:

	virtual bool			On_Execute				(void);


private:

	CWRF_Index				m_Index;


	bool					Load					(const CSG_String &File);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWRF_Export : public CSG_Tool_Grid
{
public:
	CWRF_Export(void);

	virtual CSG_String		Get_MenuPath			(void)		{	return( _TL("Export") );	}


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	CWRF_Index				m_Index;


	bool					Save					(const CSG_String &Directory, CSG_Parameter_Grid_List *pGrids);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__wrf_Import_H
