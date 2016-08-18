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
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    gcs_graticule.h                    //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#ifndef HEADER_INCLUDED__gcs_graticule_H
#define HEADER_INCLUDED__gcs_graticule_H

//---------------------------------------------------------
#include "crs_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGCS_Graticule : public CCRS_Base
{
public:
	CGCS_Graticule(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	CSG_CRSProjector		m_Projector;


	bool					Get_Graticule			(const CSG_Rect &Extent);
	bool					Get_Extent				(const CSG_Rect &Extent, CSG_Rect &rGCS);
	bool					Get_Coordinate			(const CSG_Rect &Extent, CSG_Shapes *pCoordinates, CSG_Shape *pLine, int Axis);
	double					Get_Interval			(const CSG_Rect &Extent);
	CSG_String				Get_Degree				(double Value, int Precision);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__gcs_graticule_H
