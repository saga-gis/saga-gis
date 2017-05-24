/**********************************************************
 * Version $Id: GEOTRANS_Shapes.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Projection_GeoTRANS                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   GEOTRANS_Shapes.h                   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__GEOTRANS_Shapes_H
#define HEADER_INCLUDED__GEOTRANS_Shapes_H

//---------------------------------------------------------
#include "GEOTRANS_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_geotrans_EXPORT CGEOTRANS_Shapes : public CGEOTRANS_Base
{
public:
	CGEOTRANS_Shapes(void);
	virtual ~CGEOTRANS_Shapes(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Alternatives") );	}


protected:

	virtual bool				On_Execute_Conversion	(void);


};

#endif // #ifndef HEADER_INCLUDED__GEOTRANS_Shapes_H
