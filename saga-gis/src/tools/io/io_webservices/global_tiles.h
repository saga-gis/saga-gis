
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    io_webservices                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    global_tiles.h                     //
//                                                       //
//                 Copyrights (C) 2024                   //
//                     Olaf Conrad                       //
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
#ifndef HEADER_INCLUDED__global_tiles_H
#define HEADER_INCLUDED__global_tiles_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sg_curl.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTiles_Provider : public CSG_Tool
{
public:
	CTiles_Provider(bool bLogin = false);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Global Elevation Data") );	}

//	virtual bool				do_Sync_Projections		(void)	const	{	return( false  );	}

	virtual bool				On_Before_Execution		(void);
	virtual bool				On_After_Execution		(void);


protected:

	CSG_String					m_ServerPath, m_VRT_Name, m_Grid_Name, m_Grid_Extension;


	virtual int                 On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);

	virtual CSG_Rect_Int		Get_Tiles				(const CSG_Rect &Extent) const = 0;
	virtual CSG_String			Get_Tile_Name			(int Col, int Row)       const = 0;
	virtual CSG_String			Get_Tile_Archive		(int Col, int Row)       const = 0;
	virtual CSG_String			Get_Tile_Archive_File	(int Col, int Row)       const = 0;


private:

	CSG_Parameters_CRSPicker	m_CRS;


	bool						Provide_Tiles			(const CSG_String &Directory, CSG_Rect Extent , bool DeleteArchive = true);
	int							Provide_Tile			(const CSG_String &Directory, int Col, int Row, bool DeleteArchive = true);

	bool						Update_VRT				(const CSG_String &Directory);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSRTM_CGIAR : public CTiles_Provider
{
public:
	CSRTM_CGIAR(void);


protected:

	virtual CSG_Rect_Int		Get_Tiles				(const CSG_Rect &Extent) const;
	virtual CSG_String			Get_Tile_Name			(int Col, int Row)       const;
	virtual CSG_String			Get_Tile_Archive		(int Col, int Row)       const;
	virtual CSG_String			Get_Tile_Archive_File	(int Col, int Row)       const;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSRTM_USGS : public CTiles_Provider
{
public:
	CSRTM_USGS(void);


protected:

	virtual CSG_Rect_Int		Get_Tiles				(const CSG_Rect &Extent) const;
	virtual CSG_String			Get_Tile_Name			(int Col, int Row)       const;
	virtual CSG_String			Get_Tile_Archive		(int Col, int Row)       const;
	virtual CSG_String			Get_Tile_Archive_File	(int Col, int Row)       const;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCopernicus_DEM : public CTiles_Provider
{
public:
	CCopernicus_DEM(void);


protected:

	virtual CSG_Rect_Int		Get_Tiles				(const CSG_Rect &Extent) const;
	virtual CSG_String			Get_Tile_Name			(int Col, int Row)       const;
	virtual CSG_String			Get_Tile_Archive		(int Col, int Row)       const;
	virtual CSG_String			Get_Tile_Archive_File	(int Col, int Row)       const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__global_tiles_H
