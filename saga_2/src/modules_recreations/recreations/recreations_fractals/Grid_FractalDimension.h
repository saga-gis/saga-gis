
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_FractalDimension.h                //
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


#ifndef HEADER_INCLUDED__GC_MEASUREFRACTDIM_H
#define HEADER_INCLUDED__GC_MEASUREFRACTDIM_H



#include "MLB_Interface.h"

class CGrid_FractalDimension : public CModule_Grid
{
public:
	CGrid_FractalDimension(void);
	virtual ~CGrid_FractalDimension(void);


protected:

	virtual bool			On_Execute(void);


private:

	int						dimCount;

	double					*dimAreas;

	CGrid					*pGrid;


	void					Get_Surface(int Step);
	void					Get_SurfaceRow(int Step, int xStep, int yStep, int ya, int yb);

	double					Get_Area(double dist, double z1, double z2, double z3, double z4);
	double					Get_Area(double xdist, double ydist, double z1, double z2, double z3, double z4);

	double					Get_Distance(double z1, double z2, double dist);
};

#endif // #ifndef HEADER_INCLUDED__GC_MEASUREFRACTDIM_H
