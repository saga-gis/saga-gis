
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Projection_GeoTRANS                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    GEOTRANS_Grid.h                    //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#ifndef HEADER_INCLUDED__GEOTRANS_Grid_H
#define HEADER_INCLUDED__GEOTRANS_Grid_H

//---------------------------------------------------------
#include "GEOTRANS_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_geotrans_EXPORT CGEOTRANS_Grid : public CGEOTRANS_Base
{
public:
	CGEOTRANS_Grid(void);
	virtual ~CGEOTRANS_Grid(void);

	virtual const char *	Get_MenuPath			(void)	{	return( "R:Grid" );	}


protected:

	virtual bool			On_Execute_Conversion	(void);

	virtual int				On_Parameter_Changed	(CParameters *pParameters, CParameter *pParameter);


private:

	void					Get_MinMax				(double &xMin, double &xMax, double &yMin, double &yMax, TGEO_Point Point);
	CGrid *					Get_Target_Userdef		(CGrid *pSource, bool bNearest);
	CGrid *					Get_Target_Autofit		(CGrid *pSource, double Grid_Size, int AutoExtMode, bool bNearest);

	bool					Set_Grid				(CGrid *pSource, CGrid   *pTarget, int Interpol);
	bool					Set_Shapes				(CGrid *pSource, CShapes *pTarget);

};

#endif // #ifndef HEADER_INCLUDED__GEOTRANS_Grid_H
