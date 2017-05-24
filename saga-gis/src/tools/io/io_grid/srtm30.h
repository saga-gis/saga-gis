/**********************************************************
 * Version $Id: srtm30.h 1921 2014-01-09 10:24:11Z oconrad $
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
//                       SRTM30.h                        //
//                                                       //
//                 Copyright (C) 2004 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Import_SRTM30_H
#define HEADER_INCLUDED__Import_SRTM30_H



//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CSRTM30_Import : public CSG_Tool
{
public:
	CSRTM30_Import(void);
	virtual ~CSRTM30_Import(void);

	virtual CSG_String		Get_MenuPath(void)				{	return( _TL("Import") );	}


protected:

	virtual bool			On_Execute(void);


private:

	bool					Tile_Load(const SG_Char *sTile, TSG_Rect &rTile, CSG_Grid *pOut, TSG_Rect &rOut);
	FILE *					Tile_Open(const SG_Char *sTile);

};

#endif // #ifndef HEADER_INCLUDED__Import_SRTM30_H
