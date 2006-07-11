
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Interpolation_Shepard.h                //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
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
//			Interpolation_Shepard.h     				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Interpolation_Shepard_H
#define HEADER_INCLUDED__Interpolation_Shepard_H

//---------------------------------------------------------


//---------------------------------------------------------
#include "Interpolation.h"
#include "Shepard.h"

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct 
{
    double x ;
    double y ;
	double val;
}
Data_Point;

//---------------------------------------------------------
class CInterpolation_Shepard : public CInterpolation  
{
public:
	CInterpolation_Shepard(void);
	virtual ~CInterpolation_Shepard(void);


protected:

	virtual bool		On_Initialize_Parameters(void);
	virtual void		On_Finalize_Parameters	(void);

	CShapes *			_Get_Point_Shapes(CShapes *pShapes);

	virtual bool		Get_Grid_Value			(int x, int y);

	void				Remove_Duplicate		(void);

	virtual bool		Use_SearchEngine		(void)	{	return( false );	};


private:

	int					MaxPoints, Quadratic_Neighbors, Weighting_Neighbors;

	CShapes				*pShapes;

	CShepard2d			Interpolator;

	double				*x_vals, *y_vals, *f_vals;

};

#endif // #ifndef HEADER_INCLUDED__Interpolation_Shepard_H
